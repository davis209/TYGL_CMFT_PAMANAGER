#include "pch.h"
#include <omnithread.h>
#include "STISTCPClient.h"
#include "core/sockets/src/TcpObservedSocket.h"
#include "core/sockets/src/TcpNonblockingSocket.h"
#include "core/utilities/src/CallstackLogger.h"
#include "core/utility/src/base_ex/DebugUtilEx.h"
#include "core/utility/src/base_ex/RunParamsEx.h"
#include "core/utility/src/core/Map.h"
#include "core/utility/src/core/StdEx.h"
#include "core/utility/src/core/SimpleConditionVariable.h"
#include "core/utility/src/core/MakeNameValuePairsString.h"
#include "core/utility/src/core/algorithm/strings.h"
#include <boost/thread.hpp>
#include <boost/program_options.hpp>
#include <boost/scope_exit.hpp>

#define RPARAM_DEBUGSTISTCPCLIENT "DebugSTISTCPClient"

namespace
{
    const size_t        DEFAULT_CONNECT_TIMEOUT_MS = 1000;
    const size_t        DEFAULT_CONNECTION_TEST_TIMEOUT_MS = 100;
    const std::string   DEFAULT_STIS_SERVER_IP = "localhost";
    const std::string   DEFAULT_STIS_SERVER_PORT = "15285";
    const size_t        DEFAULT_RESPONSE_TIMEOUT_MS = 3000;
    const size_t        DEFAULT_MAX_RETRIES = 3;

    const unsigned char STX = 0x02;
    const unsigned char ETX = 0x03;
}

namespace TA_IRS_App::STIS_PROTOCOL::IMPL::stistcpclient::detail
{
    using namespace std::chrono;
    using namespace std::literals;
    using st::SimpleConditionVariable;
    using TA_Base_Ex::RunParamsEx;
    using namespace TA_Base_Core;
    using ClientSocket = TA_Base_Core::TcpObservedSocket<TA_Base_Core::TcpNonblockingSocket>;
    using ClientSocketPtr = std::shared_ptr<ClientSocket>;
    using Blob = std::vector<unsigned char>;
    using Lock = std::scoped_lock<std::recursive_mutex>;

    struct STISTCPClient::Impl : TA_Base_Core::ITcpSocketObserver
    {
        Impl(std::string options)
        {
            parse_options(std::move(options));
        }

        ~Impl()
        {
            stop();
        }

        void start()
        {
            if (!m_running)
            {
                m_running = true;
                boost::async([&] { auto_reconnect_thread(); });
            }
        }

        void stop()
        {
            if (m_running)
            {
                m_running = false;
                m_responses.notify();
                m_responses_6.notify();
                m_connected.notify_all();
                disconnect();
                clear();
            }
        }

        milliseconds get_timeout(const std::string& message_id)
        {
            static st::map<std::string, milliseconds, st::CompareNoCase> s_timeouts =
            {
                {"M50", milliseconds{1000 * RunParamsEx::get_or("message-timeout-seconds-for-m50", 5)}},
                {"M51", milliseconds{1000 * RunParamsEx::get_or("message-timeout-seconds-for-m51", 20)}},
                {"M52", milliseconds{1000 * RunParamsEx::get_or("message-timeout-seconds-for-m52", 10)}},
            };

            return s_timeouts.get_value_or(message_id, m_response_timeout);
        }

        GENERALPACKET_PTR send_message(GENERALPACKET packet)
        {
            if (!m_connected)
            {
                LOG_ERROR("send_data(): can not connect to STIS server");
                return {};
            }

            pre_send_message(packet);

            auto sqn = packet.SQN.str();
            LOG_CALLSTACK(boost::format("STISTCPClient::send_message[%s:%s]") % packet.MessageID.str() % sqn);
            m_responses.insert_or_assign(sqn, GENERALPACKET_PTR{});
            BOOST_SCOPE_EXIT_ALL(&) { m_responses.erase(sqn); };

            auto data = packet.data();

            for (auto i = 0; i < m_max_retries && m_running && m_connected; ++i)
            {
                LOGLARGESTRING_DEBUG_IF(m_debug, "send_message(): %s:%s(%d): %s", packet.MessageID.str(), sqn, data.size(), packet.dump());
                send_data(data);

                if (auto response = wait_response(sqn, get_timeout(packet.MessageID)))
                {
                    return response;
                }
            }

#if 1
            LOG_ERROR_IF(m_debug, "send_message(): failed to send message, will force reconnect");
            m_connected = false; // force reconnect
#endif
            return {};
        }

        GENERALPACKET_PTR wait_response(const std::string& sqn, milliseconds timeout)
        {
            if (auto value = m_responses.get_value_optional(sqn))
            {
                auto res = m_responses.wait_for(timeout, [&] { return !m_running || !m_connected || *value; });
                LOG_DEBUG_IF_NOT(res, "wait_response(): timeout, %s", nvps(sqn, timeout));
            }

            return m_responses.get_value_or(sqn, GENERALPACKET_PTR());
        }

        GENERALPACKET_6_PTR send_message_6(GENERALPACKET packet)
        {
            if (!m_connected)
            {
                LOG_ERROR("send_data(): can not connect to STIS server");
                return {};
            }

            pre_send_message(packet);

            auto sqn = packet.SQN.str();
            LOG_CALLSTACK(boost::format("STISTCPClient::send_message[%s:%s]") % packet.MessageID.str() % sqn);
            m_responses_6.insert_or_assign(sqn, GENERALPACKET_6_PTR{});
            BOOST_SCOPE_EXIT_ALL(&) { m_responses_6.erase(sqn); };

            auto data = packet.data();

            for (auto i = 0; i < m_max_retries && m_running && m_connected; ++i)
            {
                LOGLARGESTRING_DEBUG_IF(m_debug, "send_message(): %s:%s(%d): %s", packet.MessageID.str(), sqn, data.size(), packet.dump());
                send_data(data);

                if (auto response = wait_response_6(sqn, get_timeout(packet.MessageID)))
                {
                    return response;
                }
            }

#if 1
            LOG_ERROR_IF(m_debug, "send_message(): failed to send message, will force reconnect");
            m_connected = false; // force reconnect
#endif
            return {};
        }

        GENERALPACKET_6_PTR wait_response_6(const std::string& sqn, milliseconds timeout)
        {
            if (auto value = m_responses_6.get_value_optional(sqn))
            {
                auto res = m_responses_6.wait_for(timeout, [&] { return !m_running || !m_connected || *value; });
                LOG_DEBUG_IF_NOT(res, "wait_response(): timeout, %s", nvps(sqn, timeout));
            }

            return m_responses_6.get_value_or(sqn, GENERALPACKET_6_PTR());
        }

        void send_data(const Blob& buffer)
        {
            if (m_connected)
            {
                LOG_DEBUG("send_data(): Writing data to socket");
                m_socket->write((char*)buffer.data(), buffer.size());
            }
        }

        bool connect()
        {
            if (!m_socket)
            {
                Lock lock(m_socket_mutex);
                m_socket = std::make_shared<ClientSocket>(m_server_ip, m_server_port, UINT_MAX, m_connect_timeout.count(), m_connection_test_timeout.count());
                m_socket->addObserver(*this);
                m_socket->start();
                m_connected.wait_for(m_connect_timeout, [&] { return m_connected == true; });
            }

            return m_connected;
        }

        void disconnect()
        {
            if (m_socket)
            {
                Lock lock(m_socket_mutex);
                m_socket->terminateAndWait();
                m_socket->removeObserver(*this);
                m_socket.reset();
                m_connected = false;
            }
        }

        //
        // ITcpSocketObserver interface(s):
        //

        virtual void processReceivedData(std::vector<unsigned char>& data, ISocket*) override
        {
            LOG_CALLSTACK("STISTCPClient::processReceivedData");

            LOG_DEBUG_IF(false, "processReceivedData(): received(%d): %s", data.size(), data);
            boost::assign::push_back(m_buffer).range(data);
            data.clear();

            for (; m_buffer.size();)
            {
                auto stx = std::find(m_buffer.begin(), m_buffer.end(), STX);

                if (stx == m_buffer.end())
                {
                    LOGLARGESTRING_ERROR("processReceivedData(): can not find STX: %s", st2::to_string(m_buffer));
                    m_buffer.clear();
                    break;
                }

                if (stx != m_buffer.begin())
                {
                    LOGLARGESTRING_ERROR("processReceivedData(): not starts with STX: %s", std::string(m_buffer.begin(), stx));
                    m_buffer.erase(m_buffer.begin(), stx);
                    stx = m_buffer.begin();
                }

                auto etx = std::find(m_buffer.begin(), m_buffer.end(), ETX);

                if (etx == m_buffer.end())
                {
                    // NOT an error
                    // LOGLARGESTRING_ERROR("processReceivedData(): can not find ETX: %s", st2::to_string(m_buffer));
                    break;
                }

                GENERICPACKET_HEADER header{stx, etx};

                if (header.is_message<A51>() || header.is_message<A52>())
                {
                    auto packet = std::make_shared<GENERALPACKET_6>(stx, ++etx);

                    LOGLARGESTRING_DEBUG_IF(m_debug, "processReceivedData(): received response: %s:%s(%d): %s", packet->MessageID.str(), packet->SQN.str(), packet->size(), packet->dump());

                    if (m_responses_6.count(packet->SQN.str()))
                    {
                        // post_receive_message(*packet);
                        m_responses_6.insert_or_assign(packet->SQN.str(), packet);
                    }
                    else
                    {
                        LOGLARGESTRING_DEBUG_IF(m_debug, "processReceivedData(): mismatched response: %s", packet->dump());
                    }
                }
                else
                {
                    auto packet = std::make_shared<GENERALPACKET>(stx, ++etx);

                    LOGLARGESTRING_DEBUG_IF(m_debug, "processReceivedData(): received response: %s:%s(%d): %s", packet->MessageID.str(), packet->SQN.str(), packet->size(), packet->dump());

                    if (m_responses.count(packet->SQN.str()))
                    {
                        post_receive_message(*packet);
                        m_responses.insert_or_assign(packet->SQN.str(), packet);
                    }
                    else
                    {
                        LOGLARGESTRING_DEBUG_IF(m_debug, "processReceivedData(): mismatched response: %s", packet->dump());
                    }
                }

                m_buffer.erase(stx, etx);
            }
        }

        virtual void writeFailed() override
        {
            LOG_ERROR("writeFailed(): writeFailed");
            // m_connected = false;
        }

        virtual bool connectionLost() override
        {
            LOG_DEBUG("connectionLost(): connectionLost");
            return m_connected = false;  // return FALSE, do not re-connect using same address-port
        }

        virtual void connectionEstablished() override
        {
            m_connected = true;
            LOG_DEBUG("connectionEstablished(): connected with port %d", m_socket->getSocketId());
        }

        void auto_reconnect_thread()
        {
            LOG_CALLSTACK("STISTCPClient::auto_reconnect_thread");

            while (m_running)
            {
                while (m_running && !m_connected)
                {
                    disconnect();
                    connect();
                }

                m_connected.wait_for(1s, [&] { return !m_running || !m_connected; });
            }
        }

    public: // implementation

        void clear()
        {
            m_socket.reset();
            m_buffer.clear();
            m_responses.clear();
            m_connected = false;
        }

        // special messages (e.g. A50)

        GENERALPACKET& pre_send_message(GENERALPACKET& packet)
        {
            /**
             * Ad Hoc Message Structure (MSGFREE) - Message Tag
             *
             * Message Tag will be automatically generated by ISCS. Timestamp in the format of ¡°YYMMDDHHMMSS¡± will be used as the message tag.
             * Should more than one messages are sent at the same second, the later ones will be time stamped artificially with one second¡¯s incremental each,
             * in order to ensure the unique time stamp is assigned to each message.
             */
            if (packet.MessageID == M11::ID)
            {
                static std::map<std::string, std::time_t> s_last_timestamps;
                static std::mutex s_mutex;
                std::scoped_lock<std::mutex> lock(s_mutex);

                auto m = packet.cast_to<M11>();
                auto msg_tag = m.Message.MessageTag.str();
                auto station = m.Destination.StationID.str();
                auto& last_timestamp = s_last_timestamps[station];
                auto t = st::from_time_YYMMDDHHMMSS(msg_tag);

                if (t <= last_timestamp)
                {
                    t = ++last_timestamp;
                    msg_tag = st::get_time_YYMMDDHHMMSS(t);
                    m.Message.MessageTag = msg_tag;
                    packet = m;
                }

                last_timestamp = t;
            }

            return packet;
        }

        GENERALPACKET& post_receive_message(GENERALPACKET& packet)
        {
#if 0
            // NOTE: current implementation of A50 need to know the length of message-list
            if (packet.MessageID == A50::ID)
            {
                size_t data_length = packet.Data.m_length;
                // total: 43
                // 6: Report Station
                // 3: Report PID
                // 32: TLPRE().size()
                // 2: Message Status
                auto length_from_report_station_to_message_tag = 6 + 3 + TLPRE().size() + 2;
                auto messages_length = data_length - length_from_report_station_to_message_tag;
                packet.Data.set_length(data_length + 4);
                auto str = packet.Data.str();
                auto pos = 4 + length_from_report_station_to_message_tag; // 4: Data Length
                str.insert(pos, DigitString<4>{messages_length}.str());
                std::stringstream{std::move(str)} >> packet.Data;
            }
#endif

            return packet;
        }

        void parse_options(std::string options)
        {
            using namespace boost::program_options;

            if (m_options == options)
            {
                return;
            }

            m_options = std::move(options);

            clear();

            options_description desc;
            desc.add_options()
                ("stis-server-ip-address", value<std::string>())("stis-server-ip-addr", value<std::string>())("server-ip", value<std::string>())("ip", value<std::string>())
                ("stis-server-port", value<std::string>())("server-port", value<std::string>())("port", value<std::string>())
                ("connect-timeout-ms", value<size_t>())
                ("connection-test-timeout-ms", value<size_t>())
                ("response-timeout-ms", value<size_t>())
                ("max-retries", value<size_t>())
                ;

            st::set_value_from_options(m_options, desc)
                (m_server_ip, {"stis-server-ip-address", "stis-server-ip-addr", "server-ip", "ip"})
                (m_server_port, {"stis-server-ip-address", "stis-server-port", "server-port", "port"})
                .set<milliseconds, size_t>(m_connect_timeout, "connect-timeout-ms")
                .set<milliseconds, size_t>(m_connection_test_timeout, "connection-test-timeout-ms")
                .set<milliseconds, size_t>(m_response_timeout, "response-timeout-ms")
                (m_max_retries, "max-retries")
                ;

            LOG_DEBUG("parse_options(): %s", nvps(m_server_ip, m_server_port, m_connect_timeout, m_max_retries));
        }

        // configurations
        std::string m_options = "uninitialized";
        std::string m_server_ip = DEFAULT_STIS_SERVER_IP;
        std::string m_server_port = DEFAULT_STIS_SERVER_PORT;
        size_t m_max_retries = DEFAULT_MAX_RETRIES;
        milliseconds m_connect_timeout = milliseconds(DEFAULT_CONNECT_TIMEOUT_MS);
        milliseconds m_connection_test_timeout = milliseconds(DEFAULT_CONNECTION_TEST_TIMEOUT_MS);
        milliseconds m_response_timeout = milliseconds(DEFAULT_RESPONSE_TIMEOUT_MS);

        // status
        Blob m_buffer;
        SimpleConditionVariable m_running;
        ClientSocketPtr m_socket;
        SimpleConditionVariable m_connected;
        st::map<std::string, GENERALPACKET_PTR> m_responses;
        st::map<std::string, GENERALPACKET_6_PTR> m_responses_6;
        std::recursive_mutex m_socket_mutex;
        bool m_debug = !RunParamsEx::is_false(RPARAM_DEBUGSTISTCPCLIENT);
    };

    STISTCPClient::STISTCPClient(std::string options)
        : m_impl(std::make_shared<Impl>(std::move(options)))
    {
    }

    void STISTCPClient::start()
    {
        m_impl->start();
    }

    void STISTCPClient::stop()
    {
        m_impl->stop();
    }

    void STISTCPClient::parse_options(std::string options)
    {
        m_impl->parse_options(std::move(options));
    }

    GENERALPACKET_PTR STISTCPClient::send_message(GENERALPACKET packet)
    {
        return m_impl->send_message(std::move(packet));
    }

    GENERALPACKET_6_PTR STISTCPClient::send_message_6(GENERALPACKET packet)
    {
        return m_impl->send_message_6(std::move(packet));
    }
}
