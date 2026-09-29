#include "pch.h"
#include "STISLibraryAgentServer.h"
#include "STISStatusServer.h"
#include "STISEvents.h"
#include "app/signs/common_library/src/stis_protocol/STSTMLIB_XML.h"
#include "app/signs/common_library/src/stis_protocol/STSMSGLIB_XML.h"
#include "app/signs/common_library/src/stis_protocol/STISLibraryClient.h"
#include "app/signs/common_library/src/stis_protocol/STISMessageClient.h"
#include "app/signs/common_library/src/stis_protocol/CommonDefs.h"
#include "app/signs/common_library/src/stis_protocol/STISLibraryVersionsSQLite.h"
#include "app/signs/common_library/src/stis_protocol/STISAllLibraryVersionsSQLite.h"
#include "app/signs/common_library/src/stis_protocol/STISAdHocSQLite.h"
#include "core/corba/src/ServantBase.h"
#include "core/utilities/src/CallstackLogger.h"
#include "core/utility/src/base_ex/DAI.h"
#include "core/utility/src/base_ex/GenericServantCorbaDef.h"
#include "core/utility/src/base_ex/LocationAccessFactoryEx.h"
#include "core/utility/src/base_ex/DebugUtilEx.h"
#include "core/utility/src/base_ex/SimpleFileServer.h"
#include "core/utility/src/core/FileEx.h"
#include "core/utility/src/core/StdEx.h"
#include "core/utility/src/core/StaticObject.h"
#include "core/utility/src/core/Serialize.h"
#include "core/utility/src/core/Vector.h"
#include "core/utility/src/core/Set.h"
#include "core/utility/src/core/SimpleTimer.h"
#include "core/utility/src/core/MakeNameValuePairsString.h"
#include "core/utility/src/core/algorithm/paralle/parallel.h"
#include "core/utility/src/core/algorithm/file_system.h"
#include "core/utility/src/core/algorithm/files.h"
#include "core/utility/src/core/SimpleConditionVariable.h"
#include <boost/scope_exit.hpp>

#define RPARAM_STISLIBRARYAGENTSERVERIORFILEPATH "STISLibraryAgentServerIorFilePath"

using namespace std::chrono;
using namespace std::literals;
using namespace boost::program_options;
using boost::filesystem::path;
using st::FileEx;
using st::Serialize;
using st::SimpleTimer;
using st::SimpleTimerPtr;
using st::StaticObject;
using st::SimpleConditionVariable;
//using namespace TA_Base_Ex;
using TA_Base_Ex::SimpleFileServer;
using namespace TA_Base_Core;
using namespace TA_IRS_App::STIS_PROTOCOL::IMPL;

namespace
{
    const std::string DEFAULT_STIS_LIBRARY_AGENT_SERVER_IOR_FILENAME = "stis_library_agent_server.ior";
}

namespace TA_IRS_App::STIS_PROTOCOL::INTERFACES::stislibraryagentserver::detail
{
    struct STISLibraryAgentServer::Impl : GenericServantCorbaDef
    {
        using ThisClass = Impl;
        using Timer = StaticObject<SimpleTimer, ThisClass>;

        Impl(std::string options = "")
        {
            set_class_name("STISLibraryAgentServer");
            parse_options(std::move(options));
        }

        void start()
        {
            initialize();

            if (!is_servant_activated())
            {
                m_file_server.start();

                if (ThisLocation::is_not_occ() && m_occ_agent_library_client)
                {
                    m_occ_agent_library_client->start();
                }

                activate_servant_with_name(STIS_LIBRARY_SERVANT_NAME);

                if (!m_ior.empty())
                {
                    FileEx(m_ior).set_content(CorbaUtil::objectToString(m_this)).write();
                }
            }
        }

        void stop()
        {
            if (is_servant_activated())
            {
                deactivate_servant();

                if (ThisLocation::is_not_occ() && m_occ_agent_library_client)
                {
                    m_occ_agent_library_client->stop();
                }

                m_file_server.stop();
            }
        }

        void initialize()
        {
            std::call_once(m_once, [&]
            {
            });
        }

        // GenericServantCorbaDef

        BOOST_DESCRIBE_CLASS
        (
            ThisClass, (GenericServantCorbaDef),
            (
                // on_generic_corba_invoke
                upgrade_library_version,
                set_versions,
                set_ad_hoc_message,
                delete_ad_hoc_message,
                unlock_ad_hoc_message,
                lock_ad_hoc_message_heartbeat,
                // on_generic_corba_invoke_return
                // download_library,
                lock_ad_hoc_message,
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

        // implementation

        Blob download_library(const std::string& category, const std::string& version)
        {
            return m_occ_gateway_library_client->download_library(category, version);
        }

        std::map<path, Blob> download_templates(const std::vector<path>& excludes)
        {
            return m_occ_gateway_library_client->download_templates(excludes);
        }

        void upgrade_library_version(std::string category, std::string version, const std::string& session)
        {
            LOG_CALLSTACK(boost::format("STISLibraryAgentServer::upgrade_library_version[%s:%s]") % category % version);

            if (ThisLocation::is_not_occ())
            {
                // NOTE: no need write db by itself, just sync from OCC
                // set_versions({{"current-" + category, version}});
                STISStatusServer::instance().sync_with_stis_now();
                STISStatusServer::instance().change_sync_with_stis_interval_for_a_while(500, 10 * 1000);
                m_occ_agent_library_client->sync_versions();
                m_occ_agent_library_client->change_sync_interval_for_a_while(100, 10 * 1000);
                STISStatusServer::instance().report_status_to_occ();
                return;
            }

            STISStatusServer::instance().wait_for_synchronizing_with_stis_complete();

            if (!m_upgradings.insert(category).second)
            {
                LOG_DEBUG("upgrade_library_version(): %s upgrading is in progress", category);
                return;
            }

            auto old_version = m_client.get_library_version(category, "current");
            set_versions({{"current-" + category, version}});
            s_stis_events->signal("library-upgrade-started", std::tie(category, version, session));

            boost::async([=]
            {
                LOG_CALLSTACK(boost::format("STISLibraryAgentServer::upgrade_library_version[%s:%s]") % category % version);

                try
                {
                    auto type = boost::iequals(category, "message") ? ELibraryType::PredefinedMessage : ELibraryType::DisplayTemplate;
                    m_occ_gateway_stis_client->submit_M70_UpgradePredefinedMessageDisplayTemplateLibraryRequest(type, version);
                    STISStatusServer::instance().sync_with_stis_now();
                }
                catch (...)
                {
                    // rollback
                    set_versions({{"current-" + category, old_version}});
                    LOG_DEBUG("upgrade_library_version(): failed to send M70 to STIS, %s upgrade failed", category);
                    return;
                }

                LOG_DEBUG("upgrade_library_version(): upgrade %s to %s for stations begin", category, version);
                BOOST_SCOPE_EXIT_ALL(&) { m_upgradings.erase(category); };

                static st::vector<std::string> s_agents = DAI::get_all_tis_agent_names("--without-this");
                static auto s_clients = st::transform_to_vector(s_agents, [&](auto& agent)
                {
                    return std::make_shared<STISLibraryClient>(str(boost::format("--server=%s --remote-only --root-dir=%s") % agent % m_root.string()));
                });

                st::vector<std::string> success;
                st::vector<std::string> agents = STISStatusServer::instance().get_online_tis_agent_names();
                auto clients = st::filter_copy(s_clients, [&](auto& client) { return st::any_of_iequal(agents, client->get_server_name()); });
                LOG_DEBUG("upgrade_library_version(): %s", nvps(m_library_upgrade_timeout, category, version, agents.m_vector));

                using Executor = st::algorithm::parallel::BasicExecutor<boost::executors::thread_executor>;
                Executor executor;
                executor.submit_retry_until_timeout(clients, m_library_upgrade_timeout, 1000ms, [=, &success](auto client)
                {
                    auto agent = client->get_server_name();
                    LOG_CALLSTACK(boost::format("STISLibraryAgentServer::upgrade_library_version[%s:%s]") % category % version, agent);
                    LOG_TRACE("upgrade_library_version(): %s is upgrading %s to %s", agent, category, version);
                    client->upgrade_library_version(category, version);
                    // TODO: wait for station really has the nex version
                    success.emplace_back(agent);
                    LOG_DEBUG("upgrade_library_version(): %s upgraded %s to %s", agent, category, version);
                });

                executor.wait_all();

                if (executor.all_success())  // all success
                {
                    LOG_DEBUG("upgrade_library_version(): all success");
                }
                else
                {
                    // some failed
                    auto failed = agents;
                    failed.remove(success);
                    LOG_DEBUG("upgrade_library_version():\nfailed: %s\nsucceeded: %s", failed.m_vector, success.m_vector);
                }

                if (agents.remove(success); agents.size())
                {
                    s_stis_events->signal("library-upgrade-timedout", std::tie(category, version, session, agents.m_vector));
                }
                else
                {
                    s_stis_events->signal("library-upgrade-completed", std::tie(category, version, session));
                }

                LOG_DEBUG("upgrade_library_version(): upgrade %s to %s for stations completed", category, version);
            });
        }

        void wait_for_library_upgrading_complete()
        {
            m_upgradings.wait([&] { return m_upgradings.empty(); });
        }

        void set_versions(const std::map<std::string, std::string>& versions)
        {
            // 1. update versions
            m_library_versions_sqlite.update_versions(versions);

            // 2. update all versions
            // NOTE: DO NOT use versions, it may only contains one item, not all of 4
            m_all_library_versions_sqlite.update_versions(ThisLocation::display_name(), CurNxtMsgTmpLibVers::from_map(m_library_versions_sqlite.get_versions()).to_vector());
        }

        void set_ad_hoc_message(int key, const std::string& title, const std::string& content)
        {
            m_ad_hoc_sqlite.set(key, title, content);
            unlock_ad_hoc_message(key);
        }

        void delete_ad_hoc_message(int key)
        {
            m_ad_hoc_sqlite.del(key);
            unlock_ad_hoc_message(key);
        }

        std::pair<std::string, bool> lock_ad_hoc_message(int key, std::string name)
        {
            LOG_CALLSTACK(boost::format("STISLibraryAgentServer::lock_ad_hoc_message[%d:%s]") % key % name);

            auto res = m_ad_hoc_sqlite.lock(key, name);

            if (res.second)
            {
                LOG_DEBUG("lock_ad_hoc_message(): will auto unlock ad hoc message %d in %lld ms", key, m_lock_ad_hoc_message_timeout.count());
                Timer::instance().submit_once(std::to_string(key), m_lock_ad_hoc_message_timeout, [=]
                {
                    LOG_DEBUG("lock_ad_hoc_message(): timedout(%lld ms), unlock ad hoc message %d", m_lock_ad_hoc_message_timeout.count(), key);
                    unlock_ad_hoc_message(key);
                });
            }

            return res;
        }

        void unlock_ad_hoc_message(int key)
        {
            LOG_CALLSTACK(boost::format("STISLibraryAgentServer::unlock_ad_hoc_message[%d]") % key);
            m_ad_hoc_sqlite.unlock(key);
            Timer::instance().remove(std::to_string(key));
        }

        void lock_ad_hoc_message_heartbeat(int key, std::string name)
        {
            LOG_CALLSTACK(boost::format("STISLibraryAgentServer::lock_ad_hoc_message_heartbeat[%d:%s]") % key % name);

            LOG_TRACE("lock_ad_hoc_message(): received heartbeat %s, will auto unlock in %lld ms", nvps(key, name), m_lock_ad_hoc_message_timeout.count());
            Timer::instance().submit_once(std::to_string(key), m_lock_ad_hoc_message_timeout, [=]
            {
                LOG_DEBUG("lock_ad_hoc_message_heartbeat(): timedout(%lld ms), unlock ad hoc message %d", m_lock_ad_hoc_message_timeout.count(), key);
                unlock_ad_hoc_message(key);
            });
        }

        STISLibraryClient& client()
        {
            return m_client;
        }

        STISLibraryClient& occ_client()
        {
            return *m_occ_agent_library_client;
        }

        STISAdHocSQLite& ad_hoc_sqlite()
        {
            return m_ad_hoc_sqlite;
        }

        STISLibraryVersionsSQLite& versions_sqlite()
        {
            return m_library_versions_sqlite;
        }

        STISAllLibraryVersionsSQLite& all_versions_sqlite()
        {
            return m_all_library_versions_sqlite;
        }

        void parse_options(std::string options)
        {
            if (m_options == options)
            {
                return;
            }

            m_options = std::move(options);
            LOG_DEBUG("parse_options(): %s", m_options);

            options_description desc;
            desc.add_options()
                ("root-dir", value<std::string>())
                ("versions-filename", value<std::string>())
                ("ad-hoc-message-library-filename", value<std::string>())
                ("lock-ad-hoc-message-timeout-ms", value<size_t>())
                ("library-upgrade-timeout-seconds", value<size_t>())
                ("library-upgrade-timeout-minutes", value<size_t>())
                ("generate-ior-file", value<std::string>()->implicit_value("default"))
                ;

            st::set_value_from_options(m_options, desc)
                .set<path, std::string>(m_root, "root-dir")
                .set<path, std::string>(m_ior, "generate-ior-file")
                (m_versions_filename, "versions-filename")
                (m_ad_hoc_message_library_filename, "ad-hoc-message-library-filename")
                .set<seconds, size_t>(m_library_upgrade_timeout, "library-upgrade-timeout-seconds")
                .set<seconds, size_t, minutes>(m_library_upgrade_timeout, "library-upgrade-timeout-minutes")
                .set<milliseconds, size_t>(m_lock_ad_hoc_message_timeout, "lock-ad-hoc-message-timeout-ms")
                ;

            stdex2::absoluted(m_root);

            if (m_ior == "default")
            {
                m_ior = stdex2::absoluted_copy(RunParamsEx::get_or(RPARAM_STISLIBRARYAGENTSERVERIORFILEPATH, DEFAULT_STIS_LIBRARY_AGENT_SERVER_IOR_FILENAME), m_root);
            }

            m_occ_gateway_library_client = std::make_shared<STISLibraryClient>("--server=occ-tis-gateway");

            if (ThisLocation::is_occ())
            {
                m_occ_gateway_stis_client = std::make_shared<STISMessageClient>("--server=occ-tis-gateway");
            }
            else
            {
                m_occ_agent_library_client = std::make_shared<STISLibraryClient>(str(boost::format
                (
                    "--root-dir=%s "
                    "--server=occ-tis-agent "
                    "--sync-versions "
                    "--sync-current-next-predefined-message-library "
                    "--sync-current-next-display-template-library "
                    "--sync-current-next-display-templates "
                    "--sync-all-predefined-message-libraries "
                    "--sync-all-display-template-libraries "
                ) % m_root.string()));
            }

            m_client.parse_options(m_options + " --local-only");
            m_ad_hoc_sqlite.parse_options(m_options);
            m_library_versions_sqlite.pare_options(m_options);
            m_all_library_versions_sqlite.pare_options(m_options);
            m_file_server.parse_options(m_options);
            LOG_DEBUG("parse_options(): %s", nvps(m_root, m_library_upgrade_timeout, m_lock_ad_hoc_message_timeout));
        }

        std::string m_versions_filename = RunParamsEx::expand(RunParamsEx::get_or(RPARAM_STISLIBRARYVERSIONSFILENAME, DEFAULT_STIS_LIBRARY_VERSIONS_FILENAME));
        std::string m_ad_hoc_message_library_filename = RunParamsEx::expand(RunParamsEx::get_or(RPARAM_STISADHOCMESSAGELIBRARYFILENAME, DEFAULT_STIS_AD_HOC_MESSAGE_LIBRARY_FILENAME));
        path m_root = stdex2::absoluted_copy(RunParamsEx::expand(RunParamsEx::get_or(RPARAM_STISROOTDIR, DEFAULT_STIS_ROOT_DIR)));
        path m_ior;
        std::once_flag m_once;
        std::string m_options = "uninitialized";
        STISLibraryClientPtr m_occ_agent_library_client;
        STISLibraryClientPtr m_occ_gateway_library_client;
        STISMessageClientPtr m_occ_gateway_stis_client;
        seconds m_library_upgrade_timeout = seconds(10);
        milliseconds m_lock_ad_hoc_message_timeout = 3s;
        SimpleFileServer::Server m_file_server;
        STISLibraryVersionsSQLite m_library_versions_sqlite;
        STISAllLibraryVersionsSQLite m_all_library_versions_sqlite;
        STISAdHocSQLite m_ad_hoc_sqlite;
        STISLibraryClient m_client; // this library
        std::recursive_mutex m_library_upgrade_mutex;
        std::recursive_mutex m_template_library_upgrade_mutex;
        st::set<std::string, st::CompareNoCase> m_upgradings;
    };

    STISLibraryAgentServer& STISLibraryAgentServer::instance()
    {
        return StaticObject<STISLibraryAgentServer>::value();
    }

    STISLibraryAgentServer::STISLibraryAgentServer(std::string options)
        : m_impl(std::make_shared<Impl>(std::move(options)))
    {
    }

    void STISLibraryAgentServer::parse_options(std::string options)
    {
        return m_impl->parse_options(std::move(options));
    }

    void STISLibraryAgentServer::start()
    {
        if (!m_impl)
        {
            LOG_ERROR("STISLibraryAgentServer::start(): m_impl is null");
            return;
        }
        return m_impl->start();
    }

    void STISLibraryAgentServer::stop()
    {
        if (!m_impl)
        {
            LOG_ERROR("STISLibraryAgentServer::stop(): m_impl is null");
            return;
        }
        return m_impl->stop();
    }

    Blob STISLibraryAgentServer::download_library(const std::string& category, const std::string& version)
    {
        return m_impl->download_library(category, version);
    }

    std::map<path, Blob> STISLibraryAgentServer::download_templates(const std::vector<path>& excludes)
    {
        return m_impl->download_templates(excludes);
    }

    STISLibraryClient& STISLibraryAgentServer::client()
    {
        return m_impl->client();
    }

    STISLibraryClient& STISLibraryAgentServer::occ_client()
    {
        return m_impl->occ_client();
    }

    STISAdHocSQLite& STISLibraryAgentServer::ad_hoc_sqlite()
    {
        return m_impl->ad_hoc_sqlite();
    }

    STISLibraryVersionsSQLite& STISLibraryAgentServer::versions_sqlite()
    {
        return m_impl->versions_sqlite();
    }

    STISAllLibraryVersionsSQLite& STISLibraryAgentServer::all_versions_sqlite()
    {
        return m_impl->all_versions_sqlite();
    }

    void STISLibraryAgentServer::wait_for_library_upgrading_complete()
    {
        m_impl->wait_for_library_upgrading_complete();
    }
}
