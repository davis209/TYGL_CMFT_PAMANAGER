#include "pch.h"
#include "STISConnection.h"
#include "OptionalOutput.h"
#include "StationManager.h"
#include "STISSimulator.h"
#include "app/signs/common_library/src/stis_protocol/message_types/MessageDataTypes.h"
#include "app/signs/common_library/src/stis_protocol/STSMSGLIB_XML.h"
#include "app/signs/common_library/src/stis_protocol/XMLParser.h"
#include "app/signs/common_library/src/stis_protocol/STISMessageClient.h"
#include "core/utilities/src/CallstackLogger.h"
#include "core/utility/src/base_ex/DebugUtilEx.h"
#include "core/utility/src/base_ex/RunParamsEx.h"
#include "core/utility/src/core/StdEx.h"
#include "core/utility/src/core/CacheDecorator.h"
#include "core/utility/src/core/algorithm/strings.h"
#include <iostream>

namespace TA_IRS_App::STIS_PROTOCOL::SIMULATOR::stissimulator::detail
{
    using namespace TA_Base_Ex;
    using namespace std::literals;
    using namespace STIS_PROTOCOL::MESSAGE_TYPES;
    using boost::filesystem::path;

    OptionalOutput s_cout;

    STISConnection::STISConnection(SocketClient* socket, STISStatusPtr status, StationManagerPtr station_manager)
        : m_socket(socket),
        m_status(status),
        m_station_manager(station_manager)
    {
        m_connected = true; // already connected
    }

    STISConnection::~STISConnection()
    {
        stop();
    }

    void STISConnection::start()
    {
        m_socket->addObserver(*this);
        m_socket->start();
        m_connected.wait([&] { return !m_connected; });
    }

    void STISConnection::stop()
    {
        m_socket->removeObserver(*this);
        m_socket->terminateAndWait();
        m_connected = false;
    }

    void STISConnection::send_response(GENERALPACKET a)
    {
        LOG_CALLSTACK(boost::format("STISConnection::send_response[%s:%s]") % a.MessageID.str() % a.SQN.str());
        pre_send_response(a);
        auto data = a.data();
        LOGLARGESTRING_DEBUG_IF(true, boost::format("send_response(): %s:%s(%d): %s") % a.MessageID.str() % a.SQN.str() % a.size() % a.dump());
        s_cout << boost::format("\rRESPONSE: %s:%s(%d): %s\n") % a.MessageID.str() % a.SQN.str() % a.size() % st2::remove_stx_etx_copy(a.dump());
        m_socket->write((const char*)data.data(), data.size());
    }

    void STISConnection::send_response(GENERALPACKET_6 a)
    {
        LOG_CALLSTACK(boost::format("STISConnection::send_response[%s:%s]") % a.MessageID.str() % a.SQN.str());
        // pre_send_response(a);
        auto data = a.data();
        LOGLARGESTRING_DEBUG_IF(true, boost::format("send_response(): %s:%s(%d): %s") % a.MessageID.str() % a.SQN.str() % a.size() % a.dump());
        s_cout << boost::format("\rRESPONSE: %s:%s(%d): %s\n") % a.MessageID.str() % a.SQN.str() % a.size() % st2::remove_stx_etx_copy(a.dump());
        m_socket->write((const char*)data.data(), data.size());
    }

    void STISConnection::processReceivedData(Blob& data, ISocket*)
    {
        LOG_CALLSTACK("STISConnection::processReceivedData");

        static const unsigned char STX = 0x02;
        static const unsigned char ETX = 0x03;

        LOG_DEBUG_IF(false, "processReceivedData(): received(%d): %s", data.size(), data);
        boost::assign::push_back(m_buffer).range(data);
        data.clear();

        for (;;)
        {
            auto stx = std::find(m_buffer.begin(), m_buffer.end(), STX);

            if (stx == m_buffer.end())
            {
                LOG_ERROR("processReceivedData(): can not find STX: %s", m_buffer);
                m_buffer.clear();
                break;
            }

            if (stx != m_buffer.begin())
            {
                LOG_ERROR("processReceivedData(): not starts with STX: %s", Blob{m_buffer.begin(), stx});
                m_buffer.erase(m_buffer.begin(), stx);
                stx = m_buffer.begin();
            }

            auto etx = std::find(m_buffer.begin(), m_buffer.end(), ETX);

            if (etx == m_buffer.end())
            {
                LOG_ERROR("processReceivedData(): can not find ETX: %s", m_buffer);
                break;
            }

            GENERALPACKET m(stx, ++etx);

            LOGLARGESTRING_DEBUG_IF(true, boost::format("processReceivedData(): received message: %s:%s(%d): %s") % m.MessageID.str() % m.SQN.str() % m.size() % m.dump());
            s_cout << boost::format("\rRECEIVED: %s:%s(%d): %s\n") % m.MessageID.str() % m.SQN.str() % m.size() % st2::remove_stx_etx_copy(m.dump());

            post_recieve_message(m);
            dispatch_message(m);

            if (m_buffer.erase(stx, etx); m_buffer.empty())
            {
                break;
            }
        }
    }

    void STISConnection::dispatch_message(const GENERALPACKET& m)
    {
        LOG_CALLSTACK(boost::format("STISConnection::dispatch_message[%s:%s]") % m.MessageID.str() % m.SQN.str());

        if (m_status->is_response_nack())
        {
            A99 a;
            a.SQN = m.SQN;
            a.Reason = m_status->get_nack_reason();
            a.DataLength = a.datasize();
            send_response(a);

            if (m_status->is_show_details(a.MessageID))
            {
                s_cout << "\r" << a.dump_details(4);
            }

            LOGLARGESTRING_DEBUG_IF(true, boost::format("dispatch_message(): message: %s:%s(%d):\n%s") % m.MessageID.str() % m.SQN.str() % m.size() % a.dump_details());
            return;
        }

        if (m.MessageID == M10::ID)
        {
            on_M10(*m.cast_to_ptr<M10>());
        }
        else if (m.MessageID == M11::ID)
        {
            on_M11(*m.cast_to_ptr<M11>());
        }
        else if (m.MessageID == M20::ID)
        {
            on_M20(*m.cast_to_ptr<M20>());
        }
        else if (m.MessageID == M21::ID)
        {
            on_M21(*m.cast_to_ptr<M21>());
        }
        else if (m.MessageID == M22::ID)
        {
            on_M22(*m.cast_to_ptr<M22>());
        }
        else if (m.MessageID == M23::ID)
        {
            on_M23(*m.cast_to_ptr<M23>());
        }
        else if (m.MessageID == M24::ID)
        {
            on_M24(*m.cast_to_ptr<M24>());
        }
        else if (m.MessageID == M30::ID)
        {
            on_M30(*m.cast_to_ptr<M30>());
        }
        else if (m.MessageID == M31::ID)
        {
            on_M31(*m.cast_to_ptr<M31>());
        }
        else if (m.MessageID == M32::ID)
        {
            on_M32(*m.cast_to_ptr<M32>());
        }
        else if (m.MessageID == M33::ID)
        {
            on_M33(*m.cast_to_ptr<M33>());
        }
        else if (m.MessageID == M50::ID)
        {
            on_M50(*m.cast_to_ptr<M50>());
        }
        else if (m.MessageID == M51::ID)
        {
            on_M51(*m.cast_to_ptr<M51>());
        }
        else if (m.MessageID == M52::ID)
        {
            on_M52(*m.cast_to_ptr<M52>());
        }
        else if (m.MessageID == M53::ID)
        {
            on_M53(*m.cast_to_ptr<M53>());
        }
        else if (m.MessageID == M70::ID)
        {
            on_M70(*m.cast_to_ptr<M70>());
        }
    }

#if 0
    template <auto mf, class M>
    void on_message(STISConnection& c, const M& m)
    {
        if (c.m_status->is_show_details(m.MessageID))
        {
            s_cout << "\r" << m.dump_details(4);
        }

        LOGLARGESTRING_DEBUG_IF(true, boost::format("on_message(): message: %s:%s(%d):\n%s") % m.MessageID.str() % m.SQN.str() % m.size() % m.dump_details());

        auto a = (c.m_station_manager.get()->*mf)(m);

        c.send_response(a);

        if (c.m_status->is_show_details(a.MessageID))
        {
            s_cout << "\r" << a.dump_details(4);
        }

        LOGLARGESTRING_DEBUG_IF(true, boost::format("on_message(): message: %s:%s(%d):\n%s") % m.MessageID.str() % m.SQN.str() % m.size() % a.dump_details());
    }
#endif

    template <class M, class A, A(StationManager::* mf)(const M&)>
    void on_message(STISConnection& c, const M& m)
    {
        if (c.m_status->is_show_details(m.MessageID))
        {
            if (std::is_same_v<M, M11>)
            {
                // show the FFFF...
                s_cout << "HEX: " << st2::to_hex_string(m.data()) << std::endl;
            }

            s_cout << "\r" << m.dump_details(4);
        }

        LOGLARGESTRING_DEBUG_IF(true, boost::format("on_message(): message: %s:%s(%d):\n%s") % m.MessageID.str() % m.SQN.str() % m.size() % m.dump_details());

        auto a = (c.m_station_manager.get()->*mf)(m);

        c.send_response(a);

        if (c.m_status->is_show_details(a.MessageID))
        {
            s_cout << "\r" << a.dump_details(4);
        }

        LOGLARGESTRING_DEBUG_IF(true, boost::format("on_message(): message: %s:%s(%d):\n%s") % m.MessageID.str() % m.SQN.str() % m.size() % a.dump_details());
    }

    void STISConnection::on_M10(const M10& m)
    {
        on_message<M10, A10, &StationManager::on_M10>(*this, m);
    }

    void STISConnection::on_M11(const M11& m)
    {
        on_message<M11, A10, &StationManager::on_M11>(*this, m);
    }

    void STISConnection::on_M20(const M20& m)
    {
        on_message<M20, A20, &StationManager::on_M20>(*this, m);
    }

    void STISConnection::on_M21(const M21& m)
    {
        on_message<M21, A21, &StationManager::on_M21>(*this, m);
    }

    void STISConnection::on_M22(const M22& m)
    {
        on_message<M22, A22, &StationManager::on_M22>(*this, m);
    }

    void STISConnection::on_M23(const M23& m)
    {
        on_message<M23, A23, &StationManager::on_M23>(*this, m);
    }

    void STISConnection::on_M24(const M24& m)
    {
        on_message<M24, A24, &StationManager::on_M24>(*this, m);
    }

    void STISConnection::on_M30(const M30& m)
    {
        on_message<M30, A30, &StationManager::on_M30>(*this, m);
    }

    void STISConnection::on_M31(const M31& m)
    {
        on_message<M31, A31, &StationManager::on_M31>(*this, m);
    }

    void STISConnection::on_M32(const M32& m)
    {
        on_message<M32, A32, &StationManager::on_M32>(*this, m);
    }

    void STISConnection::on_M33(const M33& m)
    {
        on_message<M33, A33, &StationManager::on_M33>(*this, m);
    }

    void STISConnection::on_M50(const M50& m)
    {
        on_message<M50, A50, &StationManager::on_M50>(*this, m);
    }

    void STISConnection::on_M51(const M51& m)
    {
        on_message<M51, A51, &StationManager::on_M51>(*this, m);
    }

    void STISConnection::on_M52(const M52& m)
    {
        on_message<M52, A52, &StationManager::on_M52>(*this, m);
    }

    void STISConnection::on_M53(const M53& m)
    {
        on_message<M53, A53, &StationManager::on_M53>(*this, m);
    }

    void STISConnection::on_M70(const M70& m)
    {
        if (m.Type == ELibraryType::PredefinedMessage)
        {
            m_status->set_current_message_library_version(m.Version.str());
        }
        else
        {
            m_status->set_current_template_library_version(m.Version.str());
        }

        on_message<M70, A70, &StationManager::on_M70>(*this, m);
    }

    void STISConnection::writeFailed()
    {
        LOG_DEBUG("writeFailed(): writeFailed");
    }

    bool STISConnection::connectionLost()
    {
        m_connected = false;
        LOG_DEBUG("connectionLost(): connectionLost");
        return true;  // re-connect
    }

    void STISConnection::connectionEstablished()
    {
        m_connected = true;
        LOG_DEBUG("connectionEstablished(): connectionEstablished");
    }

    void STISConnection::enable_output(bool b)
    {
        s_cout.enable(b);
    }

    // special messages (e.g. A50)

    GENERALPACKET& STISConnection::pre_send_response(GENERALPACKET& a)
    {
#if 0
        // NOTE: current implementation of A50 need to know the length of message-list
        if (a.MessageID == A50::ID)
        {
            size_t data_length = a.Data.m_length;
            // total: 43
            // 6: Report Station
            // 3: Report PID
            // 32: TLPRE().size()
            // 2: Message Status
            auto length_from_report_station_to_message_tag = 6 + 3 + TLPRE().size() + 2;
            auto messages_length = data_length - length_from_report_station_to_message_tag;
            a.Data.set_length(data_length - 4);
            auto str = a.Data.str();
            auto pos = 4 + length_from_report_station_to_message_tag; // 4: Data Length
            str.replace(pos, 4, "");
            std::stringstream{std::move(str)} >> a.Data;
        }
#endif

        return a;
    }

    GENERALPACKET& STISConnection::post_recieve_message(GENERALPACKET& a) // TODO: rename to: pre_dispatch_message
    {
        return a;
    }
}
