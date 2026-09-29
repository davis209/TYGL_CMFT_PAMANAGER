#include "pch.h"
#include "STISStatusServer.h"
#include "STISLibraryAgentServer.h"
#include "STISEvents.h"
#include "app/signs/common_library/src/stis_protocol/CommonDefs.h"
#include "app/signs/common_library/src/stis_protocol/STISMessageClient.h"
#include "app/signs/common_library/src/stis_protocol/STISLibraryClient.h"
#include "app/signs/common_library/src/stis_protocol/STISStatusClient.h"
#include "app/signs/common_library/src/stis_protocol/STISAllLibraryVersionsSQLite.h"
#include "bus/scada/common_library/src/CommonDefs.h"
#include "core/utility/src/base_ex/GenericServantCorbaDef.h"
#include "core/utility/src/base_ex/DebugUtilEx.h"
#include "core/utility/src/base_ex/LocationAccessFactoryEx.h"
#include "core/utility/src/base_ex/DAI.h"
#include "core/utility/src/base_ex/DataPointUtil.h"
#include "core/utility/src/core/StdEx.h"
#include "core/utility/src/core/Vector.h"
#include "core/utility/src/core/FileEx.h"
#include "core/utility/src/core/Map.h"
#include "core/utility/src/core/SimpleConditionVariable.h"
#include "core/utility/src/core/Serialize.h"
#include "core/utility/src/core/MakeNameValuePairsString.h"
#include "core/utility/src/core/algorithm/file_system.h"
#include "core/utility/src/core/algorithm/strings.h"
#include "core/utility/src/core/algorithm/container_utility.h"
#include "core/utility/src/core/StaticObject.h"
#include "core/utility/src/core/SimpleTimer.h"
#include "core/utility/src/core/SimpleConditionVariable.h"
#include <boost/mp11.hpp>
#include <functional>

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

#define RPARAM_LOCALTISGATEWAY "LocalTisGateway"

namespace TA_IRS_App::STIS_PROTOCOL::INTERFACES::stisstatusclient::detail
{
    BOOST_DESCRIBE_STRUCT(StationStatusReport, (),
                          (entity_name,
                           location_key,
                           location_name,
                           location_display_name,
                           has_current_message_library,
                           has_next_message_library,
                           has_current_template_library,
                           has_next_template_library,
                           versions));
}

namespace TA_IRS_App::STIS_PROTOCOL::INTERFACES::stisstatusserver::detail
{
    using namespace std::chrono;
    using namespace std::string_literals;
    using std::placeholders::_1;
    using boost::filesystem::path;
    using st::SimpleConditionVariable;
    using st::Serialize;
    using st::FileEx;
    using st::StaticObject;
    using st::SimpleTimer;
    using st::SimpleTimerPtr;
    using TA_Base_Ex::DataPointUtil;
    using TA_Base_Ex::GenericServantCorbaDefNamedObject;
    using namespace TA_Base_Core;
    using namespace TA_Base_Bus;
    using namespace TA_IRS_App::STIS_PROTOCOL::IMPL;

    using STISServerStatusServerNamedObject = GenericServantCorbaDefNamedObject;

    struct StationStatusReportWithTimestamp
    {
        StationStatusReport report;
        steady_clock::time_point timestamp = steady_clock::time_point::min();
    };

    struct STISStatusServer::Impl : GenericServantCorbaDef
    {
        using ThisClass = Impl;
        using Timer = StaticObject<SimpleTimer, ThisClass>;

        Impl(std::string options = "")
        {
            set_class_name("STISStatusServer");
            parse_options(std::move(options));
        }

        ~Impl()
        {
            stop();
        }

        void start()
        {
            initialize();

            if (!m_running)
            {
                m_running = true;
                m_executor = std::make_shared<boost::executors::thread_executor>();

                activate_servant_with_name(STIS_STATUS_SERVANT_NAME);

                if (ThisLocation::is_occ())
                {
                    m_executor->submit([&] { occ_sync_status_with_occ_stis_server_thread(); });
                    m_executor->submit([&] { occ_request_all_station_status_with_occ_stis_server_thread(); });
                    m_executor->submit([&] { occ_process_station_status_reports_thread(); });
                    m_executor->submit([&] { check_local_libraries_thread(); });
                }
                else
                {
                    boost::async([=]
                    {
                        station_initialize_from_occ_loop();
                        m_executor->submit([&] { station_sync_status_with_station_stis_server_thread(); });
                        m_executor->submit([&] { station_report_status_to_occ_thread(); });
                        m_executor->submit([&] { check_local_libraries_thread(); });
                    });
                }
            }
        }

        void stop()
        {
            if (m_running)
            {
                m_running = false;

                deactivate_servant();

                m_executor->close();
                m_executor.reset();
            }
        }

        // GenericServantCorbaDef

        BOOST_DESCRIBE_CLASS
        (
            Impl, (GenericServantCorbaDef),
            (
                // on_generic_corba_invoke
                station_report_status_to_occ,
                // on_generic_corba_invoke_return
                ), (), ()
        );

        virtual void on_generic_corba_invoke(std::string fn, std::string args) override
        {
            LOG_CORBA("on_generic_corba_invoke(): %s", fn);
            dispatch_generic_corba_invoke(this, std::move(fn), std::move(args));
        }

        virtual GenericServantCorbaDef::Result on_generic_corba_invoke_return(std::string fn, std::string args) override
        {
            LOG_CORBA("on_generic_corba_invoke_return(): %s", fn);
            return dispatch_generic_corba_invoke_return(this, std::move(fn), std::move(args));
        }

        // corba-interface

        void station_report_status_to_occ(StationStatusReport report)
        {
            LOG_CALLSTACK(boost::format("STISStatusServer::station_report_status_to_occ[%s]") % report.location_name);
            auto& sr = m_station_reports[report.location_display_name];
            LOG_DEBUG_IF(sr.report != report, "station_report_status_to_occ(): %s", report);
            sr.report = std::move(report);
            sr.timestamp = steady_clock::now();

            if (sr.report.versions.size())
            {
                STISLibraryAgentServer::instance().all_versions_sqlite().update_versions(sr.report.versions);
            }
        }

        // implementation

        void check_local_libraries_thread()
        {
            LOG_CALLSTACK("STISStatusServer::check_local_libraries_thread");

            auto& client = STISLibraryAgentServer::instance().client();

            while (m_running)
            {
                auto iscs = client.versions();

                download_message_library_impl(iscs.current_next_message_library_versions());
                download_template_library_impl(iscs.current_next_template_library_versions());

                if (!ThisLocation::is_occ())
                {
                    // download_templates();
                    // download_display_templates_impl(iscs.current_next_template_library_versions());
                }

                m_running.wait_for(1s, [&] { return !m_running; });
            }
        }

        void occ_sync_status_with_occ_stis_server_thread()
        {
            LOG_CALLSTACK("STISStatusServer::occ_sync_status_with_occ_stis_server_thread");

            auto& client = STISLibraryAgentServer::instance().client();
            AllCurNxtMsgTmpLibVers last_versions;

            while (m_running)
            {
                STISLibraryAgentServer::instance().wait_for_library_upgrading_complete();
                m_is_synchronizing_with_stis = true;

                {
                    AllCurNxtMsgTmpLibVers versions;

                    try
                    {
                        auto status = m_gateway_stis_client.submit_M33_OCCSTISStatusSyncRequest(ThisLocation::name(), client.versions_tuple());
                        versions.stis = status.versions;
                        versions.iscs = handle_stis_server_library_versions(status.versions);
                    }
                    catch (...)
                    {
                        versions.iscs = client.versions();
                    }

                    if (versions != last_versions || client.versions() != versions.iscs)
                    {
                        LOG_DEBUG_IF(last_versions.stis != versions.stis, "stis=%s", versions.stis.tied());
                        LOG_DEBUG_IF(last_versions.iscs != versions.iscs, "iscs=%s", versions.iscs.tied());
                        client.set_versions(versions.iscs);
                        STISLibraryAgentServer::instance().all_versions_sqlite().update_versions({{ThisLocation::display_name(), versions.to_vector()}});
                        last_versions = std::move(versions);
                    }
                }

                m_is_synchronizing_with_stis = false;
                m_running.wait_for(m_stis_status_sync_interval, [&] { return !m_running; });
            }
        }

        void occ_request_all_station_status_with_occ_stis_server_thread()
        {
            LOG_CALLSTACK("STISStatusServer::occ_request_all_station_status_with_occ_stis_server_thread");

            while (m_running)
            {
                try
                {
                    update_pid_datapoints(m_gateway_stis_client.submit_M32_AllStationSTISStatusRequest(ThisLocation::name()));
                }
                catch (...)
                {
                    update_pid_datapoints_on_connection_fail("OCC_");
                }

                m_running.wait_for(m_stis_all_station_status_sync_interval, [&] { return !m_running; });
            }
        }

        void station_sync_status_with_station_stis_server_thread()
        {
            LOG_CALLSTACK("STISStatusServer::station_sync_status_with_station_stis_server_thread");

            auto& client = STISLibraryAgentServer::instance().client();
            AllCurNxtMsgTmpLibVers last_versions;

            while (m_running)
            {
                AllCurNxtMsgTmpLibVers versions;

                try
                {
                    auto status = m_gateway_stis_client.submit_M30_StationSTISStatusRequest(ThisLocation::name());
                    versions.stis = status.versions;
                    versions.iscs = handle_stis_server_library_versions(status.versions);
                    update_pid_datapoints(status);
                }
                catch (...)
                {
                    versions.iscs = client.versions();
                    update_pid_datapoints_on_connection_fail(ThisLocation::name());
                }

                if (versions != last_versions)
                {
                    LOG_DEBUG_IF(last_versions.stis != versions.stis, "stis=%s", versions.stis.tied());
                    LOG_DEBUG_IF(last_versions.iscs != versions.iscs, "iscs=%s", versions.iscs.tied());
                    // NOTE: station only sync iscs versions with occ
                    STISLibraryAgentServer::instance().all_versions_sqlite().update_versions({{ThisLocation::display_name(), versions.to_vector()}});
                    last_versions = std::move(versions);
                }

                m_running.wait_for(m_stis_status_sync_interval, [&] { return !m_running || std::exchange(m_sync_with_stis_now, false); });
            }
        }

        void wait_for_synchronizing_with_stis_complete()
        {
            m_is_synchronizing_with_stis.wait([&] { return !m_is_synchronizing_with_stis; });
        }

        void station_report_status_to_occ_thread()
        {
            LOG_CALLSTACK("STISStatusServer::station_report_status_to_occ_thread");

            while (m_running)
            {
                report_status_to_occ();
                m_running.wait_for(m_station_report_status_to_occ_interval, [&] { return !m_running; });
            }
        }

        void occ_process_station_status_reports_thread()
        {
            LOG_CALLSTACK("STISStatusServer::occ_process_station_status_reports_thread");

            std::vector<std::string> last_offlines;

            while (m_running)
            {
                std::vector<std::string> offlines;

                for (auto& [station, report] : m_station_reports)
                {
                    if (report.timestamp == steady_clock::time_point::min() ||
                        m_station_report_status_to_occ_timeout < steady_clock::now() - report.timestamp)
                    {
                        offlines.emplace_back(station);
                    }
                }

                if (offlines != last_offlines)
                {
                    STISLibraryAgentServer::instance().all_versions_sqlite().delete_versions(offlines);
                    last_offlines = std::move(offlines);
                    LOG_DEBUG("%s", nvps(last_offlines));
                }

                m_running.wait_for(100ms, [&] { return !m_running; });
            }
        }

        void station_initialize_from_occ_loop()
        {
            LOG_CALLSTACK("STISStatusServer::station_initialize_from_occ_loop");

            auto& client = STISLibraryAgentServer::instance().client();

            while (m_running && (client.has_invalid_version() || client.has_empty_library()))
            {
                try
                {
                    m_occ_agent_library_client.sync_versions();
                    download_message_library_impl(client.current_next_message_library_versions());
                    download_template_library_impl(client.current_next_template_library_versions());
                }
                catch (std::exception& e)
                {
                    LOG_ERROR("%s", e);
                }

                if (!client.has_invalid_version() && !client.has_empty_library())
                {
                    break;
                }

                m_running.wait_for(1s, [&] { return !m_running; });
            }
        }

        void update_pid_datapoints_on_connection_fail(std::string station_prefix)
        {
            auto re = str(boost::format(R"(^ %s .*? \. TIS \. .+ \. (LCD|LED|PDP) \d+ \. .+ -Status$)") % station_prefix);
            DataPointUtil::instance().set_datapoints_enum_value_if_iregex(re, 0, QUALITY_BAD_COMM_FAILURE);
        }

        void update_pid_datapoints(const A30_StationSTISStatusReport& a30)
        {
            update_pid_datapoints(boost::trim_copy(a30.report_station), a30.pid_status_list);
        }

        void update_pid_datapoints(const A32_AllStationStatusDetails& a32)
        {
            for (auto&& a30 : a32)
            {
                update_pid_datapoints("OCC_" + boost::trim_copy(a30.report_station), a30.pid_status_list);
            }
        }

        void update_pid_datapoints(const std::string& station_prefix, const PIDStatusList& pidstatus)
        {
            for (auto&& [id, status] : pidstatus)
            {
                auto n = std::stoi(id);

                if (auto dp = DataPointUtil::instance().get_datapoint_iregex(str(boost::format(R"(^%s .*? \. TIS \. \w+ \. (LCD|LED|PDP) 0* %d \. .+ -Status$)") % station_prefix % n)))
                {
                    DataPointUtil::instance().set_datapoint_enum_value(dp, static_cast<int>(status), QUALITY_GOOD_NO_SPECIFIC_REASON);
                    continue;
                }

#if 0
                LOG_ERROR("update_pid_datapoints(): can not find pid with %s", nvps(station_prefix, id));
#endif
            }
        }

        CurNxtMsgTmpLibVers handle_stis_server_library_versions(CurNxtMsgTmpLibVers& stis)
        {
            LOG_CALLSTACK("STISStatusServer::handle_stis_server_library_versions");

            auto& client = STISLibraryAgentServer::instance().client();

            download_message_library_impl(stis.current_next_message_library_versions());
            download_template_library_impl(stis.current_next_template_library_versions());
            // download_templates();
            // download_one_display_templates(stis.next_template_library_version);

#if 0
            // if (!ThisLocation::is_occ() && !client.has_display_templates(stis.current_template_library_version))
            if (!ThisLocation::is_occ())
            {
                auto& occ_client = STISLibraryAgentServer::instance().occ_client();
                occ_client.sync_display_templates(stis.current_template_library_version);
                occ_client.sync_templates();
            }
#endif

            auto iscs = client.versions();

            // iscs current message version only change by message-library-upgrade command

            if (auto& version = stis.current_message_library_version; is_invalid_version(iscs.current_message_library_version) && client.has_message_library(version))
            {
                iscs.current_message_library_version = version;
            }

            // iscs next message version change on template library download complete

            if (auto& version = stis.next_message_library_version; is_valid_version(version) && client.has_message_library(version))
            {
                iscs.next_message_library_version = version;
            }

            // iscs current template version only change by template-library-upgrade command

            if (auto& version = stis.current_template_library_version; is_invalid_version(iscs.current_template_library_version) && client.has_template_library(version))
            {
                iscs.current_template_library_version = version;
            }

            // iscs next template version change on display templates download complete

            if (auto& version = stis.next_template_library_version; is_valid_version(version) && client.has_template_library(version))
            {
                iscs.next_template_library_version = version;
            }

            return iscs.normalize();
        }

        void download_message_library_impl(std::initializer_list<std::string> versions)
        {
            download_message_library_impl(std::vector<std::string>(versions));
        }

        void download_message_library_impl(std::pair<std::string, std::string> versions)
        {
            download_message_library_impl(std::vector<std::string>{versions.first, versions.second});
        }

        void download_message_library_impl(std::vector<std::string> versions)
        {
            boost::for_each(versions, std::bind(&ThisClass::download_one_message_library, this, _1));
        }

        void download_one_message_library(std::string version)
        {
            LOG_CALLSTACK(boost::format("STISStatusServer::download_one_message_library[%s]") % version);

            auto lock = std::scoped_lock{m_download_mutex};

            auto& client = STISLibraryAgentServer::instance().client();

            if (is_valid_version(version) && !client.has_message_library(version))
            {
                LOG_DEBUG("download_one_message_library(): begin download message library for version %s", version);

                if (ThisLocation::is_occ())
                {
                    if (auto blob = m_occ_gateway_library_client.download_library("message", version); blob.size())
                    {
                        client.add_message_library(version, blob);
                        s_stis_events->signal("library-downloaded", std::make_pair("message"s, version));
                    }
                }
                else
                {
                    m_occ_agent_library_client.sync_predefined_message_library(version);
                }

                LOG_DEBUG("download_one_message_library(): end download message library for version %s", version);
            }
        }

        void download_template_library_impl(std::initializer_list<std::string> versions)
        {
            download_template_library_impl(std::vector<std::string>(versions));
        }

        void download_template_library_impl(std::pair<std::string, std::string> versions)
        {
            download_template_library_impl(std::vector<std::string>{versions.first, versions.second});
        }

        void download_template_library_impl(std::vector<std::string> versions)
        {
            boost::for_each(versions, std::bind(&ThisClass::download_one_template_library, this, _1));
        }

        void download_one_template_library(std::string version)
        {
            LOG_CALLSTACK(boost::format("STISStatusServer::download_one_template_library[%s]") % version);

            auto lock = std::scoped_lock{m_download_mutex};

            auto& client = STISLibraryAgentServer::instance().client();

            if (is_valid_version(version) && !client.has_template_library(version))
            {
                LOG_DEBUG("download_one_template_library(): begin download template library for version %s", version);

                download_templates();

                if (ThisLocation::is_occ())
                {
                    if (auto blob = m_occ_gateway_library_client.download_library("template", version); blob.size())
                    {
                        client.add_template_library(version, blob);
                        s_stis_events->signal("library-downloaded", std::make_pair("template"s, version));
                    }
                }
                else
                {
                    m_occ_agent_library_client.sync_display_template_library(version);
                }

                LOG_DEBUG("download_one_template_library(): end download template library for version %s", version);
            }

            if (!client.has_display_templates())
            {
                download_templates();
            }
        }

        void download_templates()
        {
            LOG_CALLSTACK("STISStatusServer::download_templates");
            LOG_DEBUG("download_templates(): begin download templates");

            // NOTE: always get, but missing only
            auto& client = STISLibraryAgentServer::instance().client();

            if (ThisLocation::is_occ())
            {
                auto dir_names = std::vector<std::string>{"lcdemgtemplate", "lcdtemplate", "ledemgtemplate", "ledtemplate", "snapshot"};
                auto dir_paths = st::transform_to_vector(dir_names, [&](auto& dir) { return m_root / TMLIBRARY / dir; });
                auto files = st2::file_system::get_files(dir_paths);
                auto& excludes = st2::file_system::relatived(files, m_root);

                for (auto&& [file, blob] : m_occ_gateway_library_client.download_templates(excludes))
                {
                    auto x = m_root / TMLIBRARY / file;
                    FileEx(x).set_content(std::move(blob)).write();
                    LOG_DEBUG("download_templates(): %s", x);
                }
            }
            else
            {
                m_occ_agent_library_client.sync_templates();
            }

            LOG_DEBUG("download_templates(): end download templates");
        }

        void sync_with_stis_now()
        {
            m_sync_with_stis_now = true;
            m_running.notify_all();
        }

        void change_sync_with_stis_interval_for_a_while(size_t new_interval_ms, size_t duration_ms)
        {
            LOG_DEBUG("STISStatusServer::change_sync_with_stis_interval_for_a_while(): %s", nvps(new_interval_ms, duration_ms));
            m_stis_status_sync_interval = duration_cast<seconds>(milliseconds{new_interval_ms});
            Timer::instance().submit_once(duration_ms, [=]
            {
                m_stis_status_sync_interval = m_stis_status_sync_interval_backup;
                LOG_DEBUG("change_sync_with_stis_interval_for_a_while(): change back to %s", nvps(m_stis_status_sync_interval));
            });
            m_running.notify_all();
        }

        void initialize()
        {
            std::call_once(m_once, [&]
            {
            });
        }

        void report_status_to_occ()
        {
            st::no_throw([&] { m_occ_agent_status_client.station_report_status_to_occ(); });
        }

        std::vector<std::string> get_online_tis_agent_names()
        {
            std::vector<std::string> res;

            for (auto& [station, report] : m_station_reports)
            {
                if (report.timestamp != steady_clock::time_point::min() && steady_clock::now() - report.timestamp < m_station_report_status_to_occ_timeout)
                {
                    res.emplace_back(report.report.entity_name);
                }
            }

            return res;
        }

        void parse_options(std::string options)
        {
            using namespace boost::program_options;

            if (m_options == options)
            {
                return;
            }

            m_options = std::move(options);
            LOG_DEBUG("parse_options(): %s", m_options);

            options_description desc;
            desc.add_options()
                ("root-dir", value<std::string>())
                ("status-sync-interval-seconds", value<size_t>())
                ("station-report-status-to-occ-interval-seconds", value<size_t>())
                ("station-report-status-to-occ-timeout-seconds", value<size_t>())
                ;

            st::set_value_from_options(m_options, desc)
                .set<path, std::string>(m_root, "root-dir")
                .set<seconds, size_t>(m_stis_status_sync_interval, "status-sync-interval-seconds")
                .set<seconds, size_t>(m_stis_status_sync_interval_backup, "status-sync-interval-seconds")
                .set<seconds, size_t>(m_station_report_status_to_occ_interval, "station-report-status-to-occ-interval-seconds")
                .set<seconds, size_t>(m_station_report_status_to_occ_timeout, "station-report-status-to-occ-timeout-seconds")
                ;

            m_gateway_stis_client.parse_options(m_options + " --server=" + RunParamsEx::get_or(RPARAM_LOCALTISGATEWAY, "local-tis-gateway"));

            if (ThisLocation::is_occ())
            {
                m_occ_gateway_library_client.parse_options(m_options + " --server=occ-tis-gateway");

                for (auto& entity : DAI::get_all_tis_agent_entities("--exclude-this-location"))
                {
                    auto name = LocationEx::to_display_name(entity->getLocation());
                    m_station_reports[name] = StationStatusReportWithTimestamp{};
                }
            }
            else
            {
                m_occ_agent_library_client.parse_options(m_options + " --server=occ-tis-agent");
                m_occ_agent_status_client.parse_options(m_options + " --server=occ-tis-agent");
                m_occ_agent_status_client.set_client(STISLibraryAgentServer::instance().client());
                m_occ_agent_status_client.set_client(STISLibraryAgentServer::instance().all_versions_sqlite());
            }

            st2::absoluted(m_root);
            LOG_DEBUG("parse_options(): %s", nvps(m_root, m_stis_status_sync_interval, m_station_report_status_to_occ_interval));
        }

        path m_root = st2::absoluted_copy(RunParamsEx::expand(RunParamsEx::get_or(RPARAM_STISROOTDIR, DEFAULT_STIS_ROOT_DIR)));
        std::once_flag m_once;
        std::string m_options = "uninitialized";
        STISMessageClient m_gateway_stis_client;
        STISStatusClient m_occ_agent_status_client;
        seconds m_stis_status_sync_interval = 5s; // by protocol
        seconds m_stis_status_sync_interval_backup = m_stis_status_sync_interval; // by protocol
        seconds m_stis_all_station_status_sync_interval = 5s; // by protocol
        seconds m_station_report_status_to_occ_interval = 1s;
        seconds m_station_report_status_to_occ_timeout = 3s;
        bool m_sync_with_stis_now = false;
        SimpleConditionVariable m_running;
        STISLibraryClient m_occ_gateway_library_client;
        STISLibraryClient m_occ_agent_library_client;
        std::shared_ptr<boost::executors::thread_executor> m_executor;
        st::map<std::string, StationStatusReportWithTimestamp> m_station_reports;
        SimpleConditionVariable m_is_synchronizing_with_stis;
        std::recursive_mutex m_download_mutex;
    };

    STISStatusServer& STISStatusServer::instance()
    {
        return StaticObject<STISStatusServer>::value();
    }

    STISStatusServer::STISStatusServer(std::string options)
        : m_impl(std::make_shared<Impl>(std::move(options)))
    {
    }

    void STISStatusServer::start()
    {
        m_impl->start();
    }

    void STISStatusServer::stop()
    {
        m_impl->stop();
    }

    void STISStatusServer::parse_options(std::string options)
    {
        m_impl->parse_options(std::move(options));
    }

    void STISStatusServer::sync_with_stis_now()
    {
        m_impl->sync_with_stis_now();
    }

    void STISStatusServer::change_sync_with_stis_interval_for_a_while(size_t new_interval_ms, size_t duration_ms)
    {
        m_impl->change_sync_with_stis_interval_for_a_while(new_interval_ms, duration_ms);
    }

    void STISStatusServer::report_status_to_occ()
    {
        m_impl->report_status_to_occ();
    }

    std::vector<std::string> STISStatusServer::get_online_tis_agent_names()
    {
        return m_impl->get_online_tis_agent_names();
    }

    void STISStatusServer::wait_for_synchronizing_with_stis_complete()
    {
        m_impl->wait_for_synchronizing_with_stis_complete();
    }
}
