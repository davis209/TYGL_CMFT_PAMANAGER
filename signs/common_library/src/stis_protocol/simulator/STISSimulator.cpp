#include "pch.h"
#include "STISSimulator.h"
#include "STISConnection.h"
#include "NamePortMap.h"
#include "OptionalOutput.h"
#include "CommonDefs.h"
#include "core/utilities/src/CallstackLogger.h"
#include "core/utility/src/base_ex/DebugUtilEx.h"
#include "core/utility/src/base_ex/GenericServantCorbaDef.h"
#include "core/utility/src/base_ex/CorbaUtilEx.h"
#include "core/utility/src/core/StdEx.h"
#include "core/utility/src/core/Serialize.h"
#include "core/sockets/src/TcpServerSocket.h"
#include "core/utility/IDL/src/IGenericServantCorbaDef.h"
#include <iostream>

namespace TA_IRS_App::STIS_PROTOCOL::SIMULATOR::stissimulator::detail
{
    using namespace std::literals;
    using namespace std::chrono;
    using namespace boost::program_options;
    using stdex::Serialize;
    using TA_Base_Ex::CorbaUtilEx;
    using SocketServer = TcpServerSocket<TcpObservedSocket<TcpNonblockingSocket>>;
    using SocketServerPtr = std::shared_ptr<SocketServer>;

    struct STISSimulator::Impl
    {
        virtual ~Impl() = default;
        virtual void parse_options(std::string options) = 0;
        virtual void start() = 0;
        virtual void stop() = 0;
        virtual void set_versions(const VersionsTuple& versions) = 0;
        virtual void set_current_message_library_version(std::string version) = 0;
        virtual void set_next_message_library_version(std::string version) = 0;
        virtual void set_current_template_library_version(std::string version) = 0;
        virtual void set_next_template_library_version(std::string version) = 0;
        virtual void set_connection_link_status(int status) = 0;
        virtual void set_occ_server_status(int status) = 0;
        virtual void set_pid_status(std::string pid, int status) = 0;
        virtual void set_response_nack(bool enable, int reason) = 0;
        virtual void reset_all_to_default() = 0;
        virtual VersionsTuple get_versions() = 0;
        virtual bool is_occ() = 0;
        virtual std::string get_name() = 0;
        virtual std::ostream& dump(std::ostream& os) = 0;
        virtual std::string dump() = 0;
        virtual bool is_online() = 0;
        virtual void exec_system_cmd(std::string cmd) = 0;
        virtual void show_details(std::string msgs) {}
        virtual bool is_show_details(std::string msg) { return false; }
    };

    struct STISSimulatorProxy : STISSimulator::Impl
    {
        STISSimulatorProxy(std::string m_options)
        {
            parse_options(std::move(m_options));
        }

        virtual void parse_options(std::string options)
        {
            m_options = std::move(options);

            options_description desc("STIS SIMULATOR");
            desc.add_options()
                ("hostname", value<std::string>())
                ("name", value<std::string>())
                ("port", value<int>())
                ;

            auto vm = stdex::parsed_options(m_options, desc);

            std::string hostname = "localhost";
            int port = OCC_SIMULATOR_CORBA_PORT;
            std::string name = "occ";

            if (vm.count("name"))
            {
                name = vm["name"].as<std::string>();
                port = NamePortMap::port_from_name<int>(name);
            }

            stdex::set_value_from_options(vm)
                (hostname, "hostname")
                (port, "port")
                ;

            boost::to_lower(name);
            name[0] = std::toupper(name[0]);

            TA_Base_Ex::IGenericServantCorbaDef_var obj = CorbaUtilEx::stringToObjectUnchecked<TA_Base_Ex::IGenericServantCorbaDef>(hostname, port, SIMULATOR_SERVANT_KEY);
            m_server.set_names(name + "STISSimulator", SIMULATOR_SERVANT_KEY);
            m_server.assign_object(obj);
            m_server.set_object_timeout(1);
        }

        virtual void start() override
        {
        }

        virtual void stop() override
        {
        }

        virtual void set_versions(const VersionsTuple& versions) override
        {
            m_server.corba_call("set_versions", versions);
        }

        virtual void set_current_message_library_version(std::string version) override
        {
            m_server.corba_call("set_current_message_library_version", version);
        }

        virtual void set_next_message_library_version(std::string version) override
        {
            m_server.corba_call("set_next_message_library_version", version);
        }

        virtual void set_current_template_library_version(std::string version) override
        {
            m_server.corba_call("set_current_template_library_version", version);
        }

        virtual void set_next_template_library_version(std::string version) override
        {
            m_server.corba_call("set_next_template_library_version", version);
        }

        virtual void set_connection_link_status(int status) override
        {
            m_server.corba_call("set_connection_link_status", status);
        }

        virtual void set_occ_server_status(int status) override
        {
            m_server.corba_call("set_connection_link_status", status);
        }

        virtual void set_pid_status(std::string pid, int status) override
        {
            m_server.corba_call("set_connection_link_status", std::tie(pid, status));
        }

        virtual void set_response_nack(bool enable, int reason) override
        {
            m_server.corba_call("set_connection_link_status", std::tie(enable, reason));
        }

        virtual void reset_all_to_default() override
        {
            m_server.corba_call("reset_all_to_default");
        }

        virtual VersionsTuple get_versions() override
        {
            return m_server.corba_call_return("get_versions");
        }

        virtual bool is_occ() override
        {
            return m_server.corba_call_return("is_occ");
        }

        virtual std::string get_name() override
        {
            return m_server.corba_call_return("get_name");
        }

        virtual std::ostream& dump(std::ostream& os) override
        {
            return os << m_server.corba_call_return<std::string>("dump");
        }

        virtual std::string dump() override
        {
            return m_server.corba_call_return("dump");
        }

        virtual bool is_online() override
        {
            return m_server.is_online();
        }

        virtual void exec_system_cmd(std::string cmd) override
        {
            m_server.corba_system(cmd);
        }

        std::string m_options;
        mutable GenericServantCorbaDefNamedObject m_server;
    };

    struct CorbaSTISSimulator : STISSimulator::Impl, GenericServantCorbaDef
    {
        CorbaSTISSimulator() = default;

        CorbaSTISSimulator(int argc, char* argv[])
        {
            std::stringstream ss;
            boost::for_each(boost::irange(1, argc), [&](auto i) { ss << argv[i] << " "; });
            parse_options(ss.str());
        }

        CorbaSTISSimulator(std::string options)
        {
            parse_options(std::move(options));
        }

        ~CorbaSTISSimulator()
        {
            stop();
        }

        void start() override
        {
            LOG_CALLSTACK("CorbaSTISSimulator::start");

            initialize();

            if (!m_running)
            {
                boost::async([&]
                {
                    // TODO: fix me
                    // if (stdex::is_port_used(m_port, 5000))
                    if (stdex::is_port_used(m_port))
                    {
                        ::MessageBox(nullptr, str(boost::format("The port %d is in using.") % m_port).c_str(), (boost::to_upper_copy(get_name()) + " STISSimulator").c_str(), MB_ICONSTOP | MB_OK);
                        ::exit(-1);
                    }

                    m_running = true;
                    m_listener = std::make_shared<SocketServer>("localhost", m_port);
                    m_listener->bind();
                    m_listener->listen();

                    boost::async([&]
                    {
                        for (;;)
                        {
                            auto server = std::make_shared<STISConnection>(m_listener->accept(), m_status, m_station_manager);
                            boost::remove_erase_if(m_connections, [](auto& weak) { return weak.expired(); });
                            m_connections.emplace_back(server);
                            boost::async([=] { server->start(); });
                        }
                    });

                    m_running.wait([&] { return !m_running; });
                });
            }
        }

        void stop() override
        {
            if (m_running)
            {
                for (auto& weak : m_connections)
                {
                    if (auto connection = weak.lock())
                    {
                        connection->stop();
                    }
                }

                m_running = false;
            }
        }

        // GenericServantCorbaDef

        virtual void on_generic_corba_invoke(std::string fn, std::string args) override
        {
            if (boost::iequals(fn, "set_versions"))
            {
                auto versions = Serialize::deserialize<VersionsTuple>(args);
                return set_versions(versions);
            }
            else if (boost::iequals(fn, "set_current_message_library_version"))
            {
                set_current_message_library_version(args);
            }
            else if (boost::iequals(fn, "set_next_message_library_version"))
            {
                set_next_message_library_version(args);
            }
            else if (boost::iequals(fn, "set_current_template_library_version"))
            {
                set_current_template_library_version(args);
            }
            else if (boost::iequals(fn, "set_next_template_library_version"))
            {
                set_next_template_library_version(args);
            }
            else if (boost::iequals(fn, "set_connection_link_status"))
            {
                using Args = int;
                auto status = Serialize::deserialize<Args>(std::move(args));
                set_connection_link_status(status);
            }
            else if (boost::iequals(fn, "set_occ_server_status"))
            {
                using Args = int;
                auto status = Serialize::deserialize<Args>(std::move(args));
                set_occ_server_status(status);
            }
            else if (boost::iequals(fn, "set_pid_status"))
            {
                using Args = std::tuple<std::string, int>;
                auto&& [pid, status] = Serialize::deserialize<Args>(std::move(args));
                set_pid_status(pid, status);
            }
            else if (boost::iequals(fn, "set_response_nack"))
            {
                using Args = std::tuple<bool, int>;
                auto&& [enable, reason] = Serialize::deserialize<Args>(std::move(args));
                set_response_nack(enable, reason);
            }
            else if (boost::iequals(fn, "reset_all_to_default"))
            {
                reset_all_to_default();
            }
        }

        virtual GenericServantCorbaDef::Result on_generic_corba_invoke_return(std::string fn, std::string args) override
        {
            if (boost::iequals(fn, "get_versions"))
            {
                return get_versions();
            }
            else if (boost::iequals(fn, "get_name"))
            {
                return m_status->get_name();
            }
            else if (boost::iequals(fn, "is_occ"))
            {
                return m_status->is_occ();
            }
            else if (boost::iequals(fn, "dump"))
            {
                return dump();
            }

            return {};
        }

        // implementation

        void set_versions(const std::tuple<std::string, std::string, std::string, std::string>& versions) override
        {
            m_status->set_versions(versions);
        }

        void set_current_message_library_version(std::string version) override
        {
            m_status->set_current_message_library_version(std::move(version));
        }

        void set_next_message_library_version(std::string version) override
        {
            m_status->set_next_message_library_version(std::move(version));
        }

        void set_current_template_library_version(std::string version) override
        {
            m_status->set_current_template_library_version(std::move(version));
        }

        void set_next_template_library_version(std::string version) override
        {
            m_status->set_next_template_library_version(std::move(version));
        }

        void set_connection_link_status(int status) override
        {
            m_status->set_connection_link_status(status);
        }

        void set_occ_server_status(int status) override
        {
            m_status->set_occ_server_status(status);
        }

        void set_pid_status(std::string pid, int status) override
        {
            m_station_manager->set_pid_status(pid, status);
        }

        void set_response_nack(bool enable, int reason) override
        {
            m_status->set_response_nack(enable, reason);
        }

        void reset_all_to_default() override
        {
            m_status->reset();
        }

        VersionsTuple get_versions() override
        {
            return m_status->get_versions();
        }

        std::string get_name() override
        {
            return m_status->get_name();
        }

        std::ostream& dump(std::ostream& os) override
        {
            return m_status->dump(os);
        }

        std::string dump() override
        {
            std::stringstream ss;
            dump(ss);
            return ss.str();
        }

        bool is_online() override
        {
            return true;
        }

        virtual void exec_system_cmd(std::string cmd) override
        {
            std::system(cmd.c_str());
        }

        static std::string help()
        {
            std::stringstream ss;
            ss << options_desc();
            return ss.str();
        }

        static options_description& options_desc()
        {
            static options_description s_desc = boost::hof::eval([]
            {
                options_description desc("STIS SIMULATOR");
                desc.add_options()
                    ("name", value<std::string>()->default_value("OCC"), ("default name: OCC, supported: " + boost::join(NamePortMap::names(), ",")).c_str())
                    ("port", value<std::string>(), "default port: 15285")
                    ("corba-port", value<std::string>(), "default is 25285 for occ, 0 for stations")
                    ("sync-with-occ-simulator", value<bool>()->implicit_value(true), "default is true")
                    ("occ-simulator-hostname", value<std::string>(), "default is localhost")
                    ("occ-simulator-corba-port", value<std::string>())
                    ("sync-interval-ms", value<size_t>(), "default is 1000ms")
                    ("show-details", value<std::string>(), str(boost::format("show details of messages, values: *,M*,A*,%s") % stdex::join(ALL_MESSAGE_TYPES, ",")).c_str())
                    ;
                return desc;
            });
            return s_desc;
        }

        void parse_options(std::string options) override
        {
            if (options.empty() || m_options == options)
            {
                return;
            }

            m_options = std::move(options);

            auto vm = stdex::parsed_options(m_options, options_desc());

            auto name = vm["name"].as<std::string>();
            m_port = NamePortMap::port_from_name(name);

            m_status = std::make_shared<STISStatus>(name);
            m_station_manager = std::make_shared<StationManager>(m_status);

            if (is_occ())
            {
                m_corba_port = OCC_SIMULATOR_CORBA_PORT;
            }

            std::string msgs;

            stdex::set_value_from_options(vm)
                (m_port, "port")
                (m_occ_simulator_hostname, "occ-simulator-hostname")
                (m_corba_port, "corba-port") // high-priority, override default port
                .set<milliseconds, size_t>(m_sync_interval, "sync-interval-ms")
                (msgs, "show-details")
                ;

            m_status->show_details(msgs);
        }

        void initialize()
        {
            LOG_CALLSTACK("CorbaSTISSimulator::initialize");

            std::call_once(m_once, [&]
            {
                RunParamsEx::set(RPARAM_OPERATIONMODE, RPARAM_CONTROL);
                RunParamsEx::set(RPARAM_PROCESSSTATUS, RPARAM_RUNNINGCONTROL);

                CorbaUtil::getInstance().initialise(m_corba_port);
                CorbaUtil::getInstance().activate();

                activate_servant_with_key(SIMULATOR_SERVANT_KEY);

                if (!is_occ() && m_sync_with_occ_simulator)
                {
                    boost::async([&]
                    {
                        TA_Base_Ex::IGenericServantCorbaDef_var occ = CorbaUtilEx::stringToObjectUnchecked<TA_Base_Ex::IGenericServantCorbaDef>
                            (
                                m_occ_simulator_hostname,
                                m_occ_simulator_corba_port,
                                SIMULATOR_SERVANT_KEY
                                );

                        for (;;)
                        {
                            try
                            {
                                CORBA::String_var res;
                                res = occ->generic_invoke_return("get_versions", "");

                                if (std::strlen(res.in()))
                                {
                                    // TODO: refresh console
                                    auto versions = Serialize::deserialize<VersionsTuple>(res.in());

                                    if (auto versions = Serialize::deserialize<VersionsTuple>(res.in()); versions != m_status->get_versions())
                                    {
                                        m_status->set_versions(versions);
                                        s_cout << "\n" << "NOTE: library version changed, press ENTER to refresh" << std::endl;
                                    }
                                }
                            }
                            catch (...)
                            {
                            }

                            stdex::sleep_for(m_sync_interval);
                        }
                    });
                }
            });
        }

        bool is_occ() override
        {
            return m_status->is_occ();
        }

        void show_details(std::string msgs)
        {
            m_status->show_details(msgs);
        }

        bool is_show_details(std::string msg)
        {
            return m_status->is_show_details(msg);
        }

        std::string m_name = "OCC";
        std::string m_port = "15285";
        std::string m_options;
        SocketServerPtr m_listener;
        SimpleConditionVariable m_running;
        STISConnectionWeakPtrList m_connections;
        STISStatusPtr m_status;
        StationManagerPtr m_station_manager;
        std::once_flag m_once;
        int m_corba_port = 0;
        bool m_sync_with_occ_simulator = true;
        std::string m_occ_simulator_hostname = "localhost";
        int m_occ_simulator_corba_port = OCC_SIMULATOR_CORBA_PORT;
        milliseconds m_sync_interval = milliseconds(1000);
    };

    struct STISSimulatorFactory
    {
        static std::shared_ptr<STISSimulator::Impl> create(std::string options)
        {
            if (stdex::icontains_any(options, {"--remote", "--proxy"}))
            {
                return std::make_shared<STISSimulatorProxy>(std::move(options));
            }

            return std::make_shared<CorbaSTISSimulator>(std::move(options));
        }

        static std::shared_ptr<STISSimulator::Impl> create(int argc, char* argv[])
        {
            return std::make_shared<CorbaSTISSimulator>(argc, argv);
        }
    };

    STISSimulator& STISSimulator::instance()
    {
        static auto s_instance = new STISSimulator();
        return *s_instance;
    }

    STISSimulator::STISSimulator(int argc, char* argv[])
        : m_impl(STISSimulatorFactory::create(argc, argv))
    {
    }

    STISSimulator::STISSimulator(std::string options)
        : m_impl(STISSimulatorFactory::create(std::move(options)))
    {
    }

    void STISSimulator::parse_options(std::string options)
    {
        m_impl->parse_options(std::move(options));
    }

    void STISSimulator::start()
    {
        m_impl->start();
    }

    void STISSimulator::stop()
    {
        m_impl->stop();
    }

    void STISSimulator::set_versions(const std::tuple<std::string, std::string, std::string, std::string>& versions)
    {
        m_impl->set_versions(versions);
    }

    void STISSimulator::set_current_message_library_version(std::string version)
    {
        m_impl->set_current_message_library_version(std::move(version));
    }

    void STISSimulator::set_next_message_library_version(std::string version)
    {
        m_impl->set_next_message_library_version(std::move(version));
    }

    void STISSimulator::set_current_template_library_version(std::string version)
    {
        m_impl->set_current_template_library_version(std::move(version));
    }

    void STISSimulator::set_next_template_library_version(std::string version)
    {
        m_impl->set_next_template_library_version(std::move(version));
    }

    void STISSimulator::set_connection_link_status(int status)
    {
        m_impl->set_connection_link_status(status);
    }

    void STISSimulator::set_occ_server_status(int status)
    {
        m_impl->set_occ_server_status(status);
    }

    void STISSimulator::set_pid_status(std::string pid, int status)
    {
        m_impl->set_pid_status(pid, status);
    }

    void STISSimulator::set_response_nack(bool enable, int reason)
    {
        m_impl->set_response_nack(enable, reason);
    }

    void STISSimulator::reset_all_to_default()
    {
        m_impl->reset_all_to_default();
    }

    bool STISSimulator::is_occ()
    {
        return m_impl->is_occ();
    }

    std::string STISSimulator::get_name()
    {
        return m_impl->get_name();
    }

    std::ostream& STISSimulator::dump(std::ostream& os)
    {
        return m_impl->dump(os);
    }

    std::string STISSimulator::dump()
    {
        return m_impl->dump();
    }

    bool STISSimulator::is_online()
    {
        return m_impl->is_online();
    }

    void STISSimulator::exec_system_cmd(std::string cmd)
    {
        m_impl->exec_system_cmd(std::move(cmd));
    }

    std::string STISSimulator::help()
    {
        return CorbaSTISSimulator::help();
    }

    std::string STISSimulator::name_to_port(std::string name)
    {
        return NamePortMap::port_from_name(name);
    }

    void STISSimulator::enable_receive_response_output(bool enable)
    {
        STISConnection::enable_output(enable);
    }

    void STISSimulator::show_details(std::string msgs)
    {
        m_impl->show_details(msgs);
    }

    bool STISSimulator::is_show_details(std::string msg_id)
    {
        return m_impl->is_show_details(msg_id);
    }
}
