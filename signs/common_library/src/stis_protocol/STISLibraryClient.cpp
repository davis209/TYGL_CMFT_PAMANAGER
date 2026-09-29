#include "pch.h"
#include "STISLibraryClient.h"
#include "STISLibraryVersionsSQLite.h"
#include "STISAllLibraryVersionsSQLite.h"
#include "STISAdHocSQLite.h"
#include "STISLibraryVersionsMySQL.h"
#include "STISAllLibraryVersionsMySQL.h"
#include "STISAdHocMySQL.h"
#include "XMLParser.h"
#include "CommonDefs.h"
#include "core/utilities/src/Hostname.h"
#include "core/utilities/src/CallstackLogger.h"
#include "core/utility/src/base_ex/DAI.h"
#include "core/utility/src/base_ex/GenericServantCorbaDef.h"
#include "core/utility/src/base_ex/DebugUtilEx.h"
#include "core/utility/src/base_ex/RunParamsEx.h"
#include "core/utility/src/base_ex/SimpleFileServer.h"
#include "core/utility/src/base_ex/LocationAccessFactoryEx.h"
#include "core/utility/src/core/FileEx.h"
#include "core/utility/src/core/MakeNameValuePairsString.h"
#include "core/utility/src/core/StdEx.h"
#include "core/utility/src/core/SimpleTimer.h"
#include "core/utility/src/core/CacheDecorator.h"
#include "core/utility/src/core/SimpleConditionVariable.h"
#include "core/utility/src/core/StaticObject.h"
#include "core/utility/src/core/algorithm/files.h"
#include "core/utility/src/core/algorithm/file_system.h"
#include "core/utility/src/core/fixed_length_data/all.h"
#include <boost/functional/hash.hpp>
#include <boost/signals2.hpp>
#include <functional>

#ifdef min
#undef min
#endif

#define RPARAM_TEMPLATESDOWNLOADTIMEOUTSECONDS "TemplatesDownloadTimeoutSeconds"

#ifdef STIS_USE_SQLITE_STORAGE
using STISVersionsStore = TA_IRS_App::STIS_PROTOCOL::IMPL::STISLibraryVersionsSQLite;
using STISAllVersionsStore = TA_IRS_App::STIS_PROTOCOL::IMPL::STISAllLibraryVersionsSQLite;
using STISAdHocStore = TA_IRS_App::STIS_PROTOCOL::IMPL::STISAdHocSQLite;
#else
using STISVersionsStore = TA_IRS_App::STIS_PROTOCOL::IMPL::STISLibraryVersionsMySQL;
using STISAllVersionsStore = TA_IRS_App::STIS_PROTOCOL::IMPL::STISAllLibraryVersionsMySQL;
using STISAdHocStore = TA_IRS_App::STIS_PROTOCOL::IMPL::STISAdHocMySQL;
#endif

using namespace std::literals;
using namespace std::string_literals;
using namespace std::chrono;
using namespace boost::program_options;
using boost::filesystem::path;
using boost::executors::thread_executor;

using st::FileEx;
using st::SimpleTimer;
using st::SimpleTimerPtr;
using st::SimpleConditionVariable;
using st::StaticObject;
using st::make_cached;
using st::fixed_length_data::DigitString;
using st2::SimpleMD5FileManager;
using TA_Base_Ex::RunParamsEx;
using TA_Base_Ex::SimpleFileServer;
using TA_Base_Ex::GenericServantCorbaDefNamedObject;
using TA_Base_Ex::LocationEx;
using TA_Base_Ex::ThisLocation;
using namespace TA_Base_Core;
using namespace TA_IRS_App::STIS_PROTOCOL::IMPL;
using STISLibraryServerNamedObject = GenericServantCorbaDefNamedObject;
using Blob = std::vector<unsigned char>;
using AdHocMessageItem = std::tuple<int, std::string, std::string>;
using AdHocMessageMap = std::map<int, AdHocMessageItem>;
using AdHocMessageMapPtr = std::shared_ptr<AdHocMessageMap>;

namespace
{
    boost::system::error_code s_error_code;
}

namespace TA_IRS_App::STIS_PROTOCOL::INTERFACES::stislibraryclient::detail
{
    struct STISLibraryClient::Impl
    {
        using Timer = StaticObject<SimpleTimer, Impl>;

        Impl(std::string options = "")
        {
            parse_options(std::move(options));
        }

        ~Impl()
        {
            stop();
        }

        // predefined message library

        std::string current_message_library_version()
        {
            return m_versions_store.get_versions()["current-message"];
        }

        std::string next_message_library_version()
        {
            return m_versions_store.get_versions()["next-message"];
        }

        std::pair<std::string, std::string> current_next_message_library_versions()
        {
            return std::make_pair(current_message_library_version(), next_message_library_version());
        }

        void set_current_message_library_version(std::string version)
        {
            set_versions_impl({{"current-message", normalize_version(version)}});
        }

        void set_next_message_library_version(std::string version)
        {
            set_versions_impl({{"next-message", normalize_version(version)}});
        }

        path make_message_library_path(std::string version)
        {
            return m_root / PMLIBRARY / normalize_version(version) / "STSMSGLIB.XML";
        }

        path make_template_library_path(std::string version)
        {
            return m_root / TMLIBRARY / normalize_version(version) / "STSTMLIB.XML";
        }

        path make_library_path(const std::string& category, std::string version)
        {
            if (boost::iequals(category, "message"))
            {
                return make_message_library_path(normalize_version(version));
            }

            if (boost::iequals(category, "template"))
            {
                return make_template_library_path(normalize_version(version));
            }

            return {};
        }

        void upgrade_message_library_version(std::string version)
        {
            upgrade_library_version("message", normalize_version(version));
        }

        bool has_current_message_library()
        {
            return has_message_library(current_message_library_version());
        }

        bool has_next_message_library()
        {
            return has_message_library(next_message_library_version());
        }

        bool has_message_library(std::string version)
        {
            return exists(make_message_library_path(normalize_version(version)));
        }

        STSMSGLIB_XML_PTR load_current_message_library_xml()
        {
            return load_message_library_xml(current_message_library_version());
        }

        IPredefinedMessageLibraryPtr load_message_library()
        {
            return XMLParser::from_xml(load_current_message_library_xml());
        }

        STSMSGLIB_XML_PTR load_next_message_library_xml()
        {
            return load_message_library_xml(next_message_library_version());
        }

        STSMSGLIB_XML_PTR load_message_library_xml(std::string version)
        {
            if (!has_message_library(normalize_version(version)))
            {
                sync_predefined_message_library(version);
            }

            return XMLParser::parse_message_library(make_message_library_path(version));
        }

        void add_message_library(std::string version, const Blob& blob)
        {
            LOG_CALLSTACK("STISLibraryClient::add_message_library");
            auto xml = make_message_library_path(normalize_version(version));
            FileEx(xml).set_content(blob).write();
            LOG_DEBUG("add_message_library(): %s", xml);
        }

        void remove_message_library(std::string version)
        {
            remove(make_message_library_path(normalize_version(version)));
        }

        Blob download_predefined_message_library(std::string version)
        {
            LOG_CALLSTACK(boost::format("STISLibraryClient::download_predefined_message_library[%s]") % version);

            if (m_server.has_names() && !m_local_only)
            {
                return m_server.corba_call_return("download_predefined_message_library", normalize_version(version));
            }

            // TODO:
            return {};
        }

        // ad hoc message library

        AdHocMessageMapPtr load_ad_hoc_message_library()
        {
            return m_ad_hoc_store.load();
        }

        void set_ad_hoc_message(int key, const std::string& title, const std::string& content)
        {
            LOG_CALLSTACK("STISLibraryClient::set_ad_hoc_message");

            if (m_server.has_names() && !m_local_only)
            {
                m_server.corba_call("set_ad_hoc_message", std::tie(key, title, content));

                if (!m_remote_only)
                {
                    sync_ad_hoc_message_library();
                }
            }
            else
            {
                m_ad_hoc_store.set(key, title, content);
            }
        }

        void delete_ad_hoc_message(int key)
        {
            LOG_CALLSTACK("STISLibraryClient::delete_ad_hoc_message");

            if (m_server.has_names() && !m_local_only)
            {
                m_server.corba_call("delete_ad_hoc_message", key);

                if (!m_remote_only)
                {
                    sync_ad_hoc_message_library();
                }
            }
            else
            {
                m_ad_hoc_store.del(key);
            }
        }

        AdHocMessageItem get_ad_hoc_message(int key)
        {
            return m_ad_hoc_store.get(key);
        }

        std::pair<std::string, bool> lock_ad_hoc_message(int key)
        {
            LOG_CALLSTACK(boost::format("STISLibraryClient::lock_ad_hoc_message[%d]") % key);

            if (m_server.has_names() && !m_local_only)
            {
                try
                {
                    auto res = m_server.corba_call_return<std::pair<std::string, bool>>("lock_ad_hoc_message", std::make_tuple(key, m_hostname));

                    if (res.second)
                    {
                        Timer::instance().submit(std::to_string(key), m_lock_ad_hoc_message_heartbeat_interval, [=]
                        {
                            m_server.corba_call_timeout<1000>("lock_ad_hoc_message_heartbeat", std::make_tuple(key, m_hostname));
                        });
                    }

                    return res;
                }
                catch (...)
                {
                    return {"CONNECTION FAILED", false};
                }
            }
            else
            {
                auto res = m_ad_hoc_store.lock(key, m_hostname);

                if (res.second)
                {
                    Timer::instance().submit_once(std::to_string(key), m_lock_ad_hoc_message_heartbeat_timeout, [=]
                    {
                        m_ad_hoc_store.unlock(key);
                    });
                }

                return res;
            }
        }

        void unlock_ad_hoc_message(int key)
        {
            LOG_CALLSTACK(boost::format("STISLibraryClient::unlock_ad_hoc_message[%d]") % key);

            if (m_server.has_names() && !m_local_only)
            {
                m_server.corba_call("unlock_ad_hoc_message", std::make_tuple(key));
            }
            else
            {
                m_ad_hoc_store.unlock(key);
            }

            Timer::instance().remove(std::to_string(key));
        }

        // display template library

        std::string current_template_library_version()
        {
            LOG_CALLSTACK("STISLibraryClient::current_template_library_version");
            return m_versions_store.get_versions()["current-template"];
        }

        std::string next_template_library_version()
        {
            LOG_CALLSTACK("STISLibraryClient::next_template_library_version");
            return m_versions_store.get_versions()["next-template"];
        }

        std::pair<std::string, std::string> current_next_template_library_versions()
        {
            return std::make_pair(current_template_library_version(), next_template_library_version());
        }

        void set_current_template_library_version(std::string version)
        {
            set_versions_impl({{"current-template", normalize_version(version)}});
        }

        void set_next_template_library_version(std::string version)
        {
            set_versions_impl({{"next-template", normalize_version(version)}});
        }

        void upgrade_template_library_version(std::string version)
        {
            upgrade_library_version("template", normalize_version(version));
        }

        bool has_current_template_library()
        {
            return has_template_library(current_template_library_version());
        }

        bool has_next_template_library()
        {
            return has_template_library(next_template_library_version());
        }

        bool has_template_library(std::string version)
        {
            return exists(make_template_library_path(normalize_version(version)));
        }

        bool has_display_templates(std::string version)
        {
            static const std::vector<path> s_template_directory_names = {"lcdtemplate", "lcdemgtemplate", "ledtemplate", "ledemgtemplate"};

            return boost::algorithm::all_of(s_template_directory_names, [&](auto& template_dir_name)
            {
                auto dir = m_root / TMLIBRARY / normalize_version(version) / template_dir_name;
                return st2::has_file(dir, ".xml");
            });
        }

        bool has_display_templates()
        {
            static const std::vector<path> s_template_directory_names = {"lcdtemplate", "lcdemgtemplate", "ledtemplate", "ledemgtemplate", "snapshot"};

            return boost::algorithm::all_of(s_template_directory_names, [&](auto& template_dir_name)
            {
                auto dir = m_root / TMLIBRARY / template_dir_name;
                return st2::recursive_has_file(dir, {".xml", ".bmp"});
            });
        }

        STSTMLIB_XML_PTR load_current_template_library_xml()
        {
            return load_template_library_xml(current_template_library_version());
        }

        STSTMLIB_XML_PTR load_next_template_library_xml()
        {
            return load_template_library_xml(next_template_library_version());
        }

        ITemplateLibraryPtr load_template_library()
        {
            return XMLParser::from_xml(load_current_template_library_xml());
        }

        STSTMLIB_XML_PTR load_template_library_xml(std::string version)
        {
            LOG_CALLSTACK(boost::format("STISLibraryClient::load_template_library_xml[%s]") % version);

            if (!has_template_library(normalize_version(version)))
            {
                sync_display_template_library(version);
            }

            return XMLParser::parse_template_library(make_template_library_path(version));
        }

        void add_template_library(std::string version, const Blob& blob)
        {
            auto xml = make_template_library_path(normalize_version(version));
            FileEx(xml).set_content(blob).write();
            LOG_DEBUG("add_template_library(): %s", xml);
        }

        void remove_template_library(std::string version)
        {
            remove(make_template_library_path(normalize_version(version)));
        }

        const std::string& get_lcd_template(const std::string& id)
        {
            //return get_template_content_impl(m_root / TMLIBRARY / current_template_library_version() / "lcdtemplate" / ("TMP3" + DigitString<3>(id).str() + ".xml"));
            return get_template_content_impl(m_root / TMLIBRARY / "lcdtemplate" / ("TMP3" + DigitString<3>(id).str() + ".xml"));
        }

        const std::string& get_lcd_emergency_template(const std::string& id)
        {
            // return get_template_content_impl(m_root / TMLIBRARY / current_template_library_version() / "lcdemgtemplate" / ("EMG1" + DigitString<3>(id).str() + ".xml"));
            return get_template_content_impl(m_root / TMLIBRARY / "lcdemgtemplate" / ("EMG1" + DigitString<3>(id).str() + ".xml"));
        }

        const std::string& get_led_template(const std::string& id)
        {
            // return get_template_content_impl(m_root / TMLIBRARY / current_template_library_version() / "ledtemplate" / ("TMP4" + DigitString<3>(id).str() + ".xml"));
            return get_template_content_impl(m_root / TMLIBRARY / "ledtemplate" / ("TMP4" + DigitString<3>(id).str() + ".xml"));
        }

        const std::string& get_led_emergency_template(const std::string& id)
        {
            // return get_template_content_impl(m_root / TMLIBRARY / current_template_library_version() / "ledemgtemplate" / ("EMG2" + DigitString<3>(id).str() + ".xml"));
            return get_template_content_impl(m_root / TMLIBRARY / "ledemgtemplate" / ("EMG2" + DigitString<3>(id).str() + ".xml"));
        }

        const std::string& get_template_content_impl(const path& file)
        {
            LOG_CALLSTACK(boost::format("STISLibraryClient::get_template_content_impl[%s]") % file.string());

            static auto s_func = make_cached([](const path& file, const std::string& md5)
            {
                return FileEx(file).string();
            });

            return s_func(file, m_md5_mgr.get_file_md5(file));
        }

        Blob download_display_template_library(std::string version)
        {
            LOG_CALLSTACK(boost::format("STISLibraryClient::download_display_template_library[%s]") % version);

            if (m_server.has_names() && !m_local_only)
            {
                return m_server.corba_call_return("download_display_template_library", normalize_version(version));
            }

            // TODO
            return {};
        }

        std::map<path, Blob> download_templates(const std::vector<path>& excludes)
        {
            LOG_CALLSTACK("STISLibraryClient::download_templates");

            if (m_server.has_names() && !m_local_only)
            {
                // auto timeout = RunParamsEx::get_or(RPARAM_TEMPLATESDOWNLOADTIMEOUTSECONDS, 60 * 60 * 1000);
                // TODO: change to non-template
                return m_server.corba_call_return_timeout<60 * 60 * 1000, std::map<path, Blob>>("download_templates", excludes);
            }

            // TODO
            return {};
        }

        // both

        void set_versions(const CurNxtMsgTmpLibVers& versions)
        {
            set_versions_impl(versions.to_map());
        }

        CurNxtMsgTmpLibVers versions()
        {
            return CurNxtMsgTmpLibVers::from_map(m_versions_store.get_versions());
        }

        bool has_invalid_version()
        {
            return boost::algorithm::any_of(st::values(m_versions_store.get_versions()), is_invalid_version);
        }

        bool has_empty_library()
        {
            return !(has_current_message_library() &&
                     has_next_message_library() &&
                     has_current_template_library() &&
                     has_next_template_library());
        }

        std::string get_library_version(const std::string& category, const std::string& stage)
        {
            LOG_CALLSTACK(boost::format("STISLibraryClient::get_library_version[%s:%s]") % category % stage);
            return m_versions_store.get_versions()[stage + "-" + category];
        }

        void set_library_version(const std::string& category, const std::string& stage, std::string version)
        {
            set_versions_impl({{stage + "-" + category, normalize_version(version)}}); // current/next-message/template = version
        }

        void upgrade_library_version(const std::string& category, std::string version)
        {
            LOG_CALLSTACK(boost::format("STISLibraryClient::upgrade_library_version[%s:%s]") % category % version);

            if (m_server.has_names() && !m_local_only)
            {
                m_server.corba_call_timeout<10 * 1000>("upgrade_library_version", std::make_tuple(category, normalize_version(version), RPARAM_SESSIONID_v));

                if (!m_remote_only)
                {
                    sync_versions();
                    sync_all_versions();
                }
            }
            else
            {
                set_library_version(category, "current", normalize_version(version));
            }
        }

        bool has_library(const std::string& category, std::string version)
        {
            return exists(make_library_path(category, normalize_version(version)));
        }

        Blob get_library(const std::string& category, std::string version)
        {
            return FileEx(make_library_path(category, normalize_version(version))).get_blob();
        }

        void add_library(const std::string& category, std::string version, const Blob& blob)
        {
            auto xml = make_library_path(category, normalize_version(version));
            FileEx(xml).set_content(blob).write();
            LOG_DEBUG("add_library(): %s", xml);
        }

        void remove_library(const std::string& category, std::string version)
        {
            remove(make_library_path(category, normalize_version(version)));
        }

        AllCurNxtMsgTmpLibVers get_this_station_iscs_stis_library_versions()
        {
            if (auto value = st::get_value_optional(m_all_versions_store.get_versions(), ThisLocation::name()))
            {
                return AllCurNxtMsgTmpLibVers::from_vector(*value);
            }

            return {};
        }

        AllCurNxtMsgTmpLibVersInfoList get_all_station_iscs_stis_library_versions()
        {
            AllCurNxtMsgTmpLibVersInfoList res;

            for (auto&& [name, v] : m_all_versions_store.get_versions())
            {
                AllCurNxtMsgTmpLibVersInfo info;
                info.location_name = name;
                info.location_key = LocationEx::to_key(name);
                info.versions = v;
                res.emplace_back(std::move(info));
            }

            return res;
        }

        // synchronize

        template <class F, class D>
        auto make_while_loop(D interval, F func)
        {
            return[=, f = std::move(func)]
            {
                while (m_running)
                {
                    f();

                    m_running.wait_for(interval, [&]
                    {
                        return !m_running || std::exchange(m_sync_now, false);
                    });
                }
            };
        }

        void start()
        {
            LOG_CALLSTACK("STISLibraryClient::start");

            initialize();

            if (!m_running)
            {
                m_running = true;
                m_executor = std::make_shared<thread_executor>();

                if (m_sync_versions)
                {
                    m_executor->submit(make_while_loop(m_sync_interval, [&] { sync_versions(); }));
                }

                if (m_sync_all_versions)
                {
                    m_executor->submit(make_while_loop(m_sync_interval, [&] { sync_all_versions(); }));
                }

                if (m_sync_ad_hoc_message_library)
                {
                    m_executor->submit(make_while_loop(m_sync_interval, [&] { sync_ad_hoc_message_library(); }));
                }

                if (m_sync_current_next_predefined_message_library)
                {
                    m_executor->submit(make_while_loop(m_sync_interval, [&] { sync_current_next_predefined_message_library(); }));
                }

                if (m_sync_current_next_display_template_library)
                {
                    m_executor->submit(make_while_loop(m_sync_interval, [&] { sync_current_next_display_template_library(); }));
                }

#if 0
                if (m_sync_current_next_display_templates)
                {
                    m_executor->submit(make_while_loop(m_sync_current_next_display_templates_interval, [&] { sync_current_next_display_templates(); }));
                }
#endif

                if (m_sync_templates)
                {
                    m_executor->submit(make_while_loop(m_sync_templates_interval, [&] { sync_templates(); }));
                }

                if (m_sync_all_predefined_message_libraries)
                {
                    m_executor->submit(make_while_loop(m_sync_all_predefined_message_libraries_interval, [&] { sync_all_predefined_message_libraries(); }));
                }

                // specifical-case, too many files
                if (m_sync_all_display_template_libraries)
                {
                    m_executor->submit([&]
                    {
                        while (m_running)
                        {
                            m_running.wait_for(m_sync_all_display_template_libraries_interval, [&] { return !m_running; });
                            auto future = boost::async([&] { sync_all_display_template_libraries(); });
                            m_running.wait([&] { return !m_running || future.has_value(); });
                        }
                    });
                }

                if (m_server.has_names() && !m_local_only)
                {
                    m_executor->submit([&]
                    {
                        auto server_online_last_timestamp = steady_clock::now();

                        while (m_running)
                        {
                            if (m_server.is_online())
                            {
                                server_online_last_timestamp = steady_clock::now();
                            }
                            else
                            {
                                if (3s < steady_clock::now() - server_online_last_timestamp)
                                {
                                    remove(m_root / m_versions_filename, s_error_code);
                                    remove(m_root / m_all_versions_filename, s_error_code);
                                }
                            }

                            m_running.wait_for(1s, [&] { return !m_running; });
                        }
                    });
                }
            }
        }

        void stop()
        {
            LOG_CALLSTACK("STISLibraryClient::stop");

            if (m_running)
            {
                m_running = false;
                m_executor->close();
                m_executor.reset();
            }
        }

        void async_now()
        {
            m_sync_now = true;
            m_running.notify_all();
        }

        bool is_server_online()
        {
            return m_server.has_names() ? m_server.is_online() : true;
        }

        bool sync_versions()
        {
            LOG_CALLSTACK("STISLibraryClient::sync_versions");
            return added_or_updated(m_file_server_client.sync_file(m_versions_filename));
        }

        bool sync_all_versions()
        {
            LOG_CALLSTACK("STISLibraryClient::sync_all_versions");
            return added_or_updated(m_file_server_client.sync_file(m_all_versions_filename));
        }

        bool sync_ad_hoc_message_library()
        {
            LOG_CALLSTACK("STISLibraryClient::sync_ad_hoc_message_library");
            return added_or_updated(m_file_server_client.sync_file(m_ad_hoc_message_library_filename));
        }

        size_t sync_current_message_template_library()
        {
            LOG_CALLSTACK("STISLibraryClient::sync_current_message_template_library");

            size_t count = 0;

            if (auto version = current_message_library_version(); is_valid_version(version))
            {
                count += sync_predefined_message_library(version);
            }

            if (auto version = current_template_library_version(); is_valid_version(version))
            {
                count += sync_display_template_library(version);
            }

            return count;
        }

        size_t sync_current_next_message_template_library()
        {
            LOG_CALLSTACK("STISLibraryClient::sync_current_next_message_template_library");

            size_t count = 0;
            auto vers = versions();

            // current

            if (auto version = current_message_library_version(); is_valid_version(version))
            {
                count += sync_predefined_message_library(version);
            }

            if (auto version = current_template_library_version(); is_valid_version(version))
            {
                count += sync_display_template_library(version);
            }

            // next

            if (auto version = vers.next_message_library_version; version != vers.current_message_library_version && is_valid_version(version))
            {
                count += sync_predefined_message_library(version);
            }

            if (auto version = vers.next_template_library_version; version != vers.current_template_library_version && is_valid_version(version))
            {
                count += sync_display_template_library(version);
            }

            return count;
        }

        size_t sync_all_predefined_message_libraries()
        {
            LOG_CALLSTACK("STISLibraryClient::sync_all_predefined_message_libraries");
            auto res = m_file_server_client.sync(PMLIBRARY);
            return res.added.size() + res.updated.size();
        }

        size_t sync_predefined_message_library(std::string version)
        {
            return sync_library_by_version_impl(PMLIBRARY, normalize_version(version), "STSMSGLIB.XML");
        }

        size_t sync_current_predefined_message_library()
        {
            return sync_predefined_message_library(current_message_library_version());
        }

        size_t sync_next_predefined_message_library()
        {
            return sync_predefined_message_library(next_message_library_version());
        }

        size_t sync_current_next_predefined_message_library()
        {
            auto [current, next] = current_next_message_library_versions();
            auto count = sync_predefined_message_library(current);
            return count + (current != next ? sync_predefined_message_library(next) : 0);
        }

        size_t sync_all_display_template_libraries()
        {
            LOG_CALLSTACK("STISLibraryClient::sync_all_display_template_libraries");
            auto res = m_file_server_client.sync(TMLIBRARY);
            return res.added.size() + res.updated.size();
        }

        size_t sync_current_display_template_library()
        {
            return sync_display_template_library(current_template_library_version());
        }

        size_t sync_current_next_display_template_library()
        {
            auto [current, next] = current_next_template_library_versions();
            auto count = sync_display_template_library(current);
            return count + (current != next ? sync_display_template_library(next) : 0);
        }

        size_t sync_display_template_library(std::string version)
        {
            return sync_library_by_version_impl(TMLIBRARY, normalize_version(version), "STSTMLIB.XML");
        }

        size_t sync_library_by_version_impl(path dir, std::string version, path filename)
        {
            LOG_CALLSTACK(boost::format("STISLibraryClient::sync_library_by_version_impl[%s:%s:%s]") % dir.string() % version % filename.string());
            return is_valid_version(normalize_version(version)) &&
                added_or_updated(m_file_server_client.sync_file(dir / version / filename));
        }

        size_t sync_current_display_templates()
        {
            return sync_display_templates(current_template_library_version());
        }

        size_t sync_current_next_display_templates()
        {
            auto [current, next] = current_next_template_library_versions();
            auto count = sync_display_templates(current);
            return current != next ? sync_display_templates(next) : 0;
        }

        size_t sync_display_templates(std::string version)
        {
            LOG_CALLSTACK(boost::format("STISLibraryClient::sync_display_templates[%s]") % version);

            if (is_valid_version(normalize_version(version)))
            {
                auto res = m_file_server_client.sync(path(TMLIBRARY) / version);
                return res.added.size() + res.updated.size();
            }

            return 0;
        }

        size_t sync_templates()
        {
            LOG_CALLSTACK("STISLibraryClient::sync_templates");

            size_t changed = 0;

            for (auto&& dir : {"lcdemgtemplate", "lcdtemplate", "ledemgtemplate", "ledtemplate", "snapshot"})
            {
                auto res = m_file_server_client.sync(path(TMLIBRARY) / dir);
                changed += res.added.size() + res.updated.size();
            }

            return changed;
        }

        void change_sync_interval_for_a_while(size_t new_interval_ms, size_t duration_ms)
        {
            LOG_DEBUG("change_sync_interval_for_a_while(): %s", nvps(new_interval_ms, duration_ms));
            m_sync_interval = milliseconds{new_interval_ms};
            Timer::instance().submit_once(duration_ms, [=]
            {
                m_sync_interval = m_sync_interval_backup;
                LOG_DEBUG("change_sync_interval_for_a_while(): change back to %s", nvps(m_sync_interval));
            });
            m_running.notify_all();
        }

        void change_sync_interval_until(size_t new_interval_ms, size_t check_pred_interval_ms, size_t timeout_ms, std::function<bool()> pred)
        {
            m_sync_interval = milliseconds{new_interval_ms};
            Timer::instance().submit(check_pred_interval_ms, [=, start_time = steady_clock::now()]
            {
                if (pred() || seconds(timeout_ms) < steady_clock::now() - start_time)
                {
                    m_sync_interval = m_sync_interval_backup;
                    SimpleTimer::remove_this_from_timer();
                    LOG_DEBUG("change_sync_interval_until(): change back to %s", nvps(m_sync_interval));
                }
            });
            m_running.notify_all();
        }

        Blob download_library(const std::string& category, std::string version)
        {
            LOG_CALLSTACK(boost::format("STISLibraryClient::download_library[%s:%s]") % category % version);

            if (m_server.has_names() && !m_local_only)
            {
                LOG_CALLSTACK("STISLibraryClient::download_library");
                return m_server.corba_call_return("download_library", std::tie(category, normalize_version(version)));
            }

            // TODO: copy local file or sftp
            return {};
        }

    public: // implementation

        void set_versions_impl(const std::map<std::string, std::string>& versions)
        {
            LOG_CALLSTACK("STISLibraryClient::set_versions_impl");

            if (m_server.has_names() && !m_local_only)
            {
                m_server.corba_call("set_versions", versions); // first write remotely

                if (!m_remote_only)
                {
                    sync_versions(); // then sync back both versions
                    sync_all_versions(); // add all-versions
                }
            }
            else
            {
                m_versions_store.update_versions(versions);
                // NOTE: DO NOT use versions, it may only contains one item, not all of 4
                m_all_versions_store.update_versions(ThisLocation::name(), CurNxtMsgTmpLibVers::from_map(m_versions_store.get_versions()).to_vector());
            }
        }

        void versions_sqlite_pre_query_callback()
        {
            if (!exists(m_root / m_versions_filename))
            {
                LOG_CALLSTACK("STISLibraryClient::versions_sqlite_pre_query_callback");
                m_file_server_client.get(m_versions_filename);
            }
        }

        void all_versions_sqlite_pre_query_callback()
        {
            // TODO: install callback in m_all_versions_store

            if (!exists(m_root / m_all_versions_filename))
            {
                LOG_CALLSTACK("STISLibraryClient::all_versions_sqlite_pre_query_callback");
                m_file_server_client.get(m_all_versions_filename);
            }
        }

        std::string get_server_name()
        {
            return m_server.m_agent_name;
        }

        void parse_options(std::string options)
        {
            LOG_CALLSTACK("STISLibraryClient::parse_options");

            if (m_options == options)
            {
                return;
            }

            m_options = std::move(options);
            LOG_DEBUG("parse_options(): %s", m_options);

            clear();

            options_description desc;
            desc.add_options()
                ("server", value<std::string>())
                ("local-only", value<bool>()->implicit_value(true))
                ("remote-only", value<bool>()->implicit_value(true))
                ("library-server-ior", value<std::string>())("server-ior", value<std::string>())("ior", value<std::string>())
                ("file-server-ior", value<std::string>())
                ("root-dir", value<std::string>())
                ("versions-filename", value<std::string>())
                ("all-versions-filename", value<std::string>())
                ("ad-hoc-message-library-filename", value<std::string>())
                ("sync-all", value<bool>()->implicit_value(true))
                ("sync-versions", value<bool>()->implicit_value(true))
                ("sync-all-versions", value<bool>()->implicit_value(true))
                ("sync-all-predefined-message-libraries", value<bool>()->implicit_value(true))
                ("sync-all-display-template-libraries", value<bool>()->implicit_value(true))
                ("sync-current-next-predefined-message-library", value<bool>()->implicit_value(true))
                ("sync-current-next-display-template-library", value<bool>()->implicit_value(true))
                ("sync-current-next-display-templates", value<bool>()->implicit_value(true))
                ("sync-ad-hoc-message-library", value<bool>()->implicit_value(true))
                ("lock-ad-hoc-message-heartbeat-interval-ms", value<size_t>())
                ("sync-interval-ms", value<size_t>())
                ("client-call-timeout-seconds", value<size_t>()->default_value(10))
                ;

            std::string server;
            std::string ior;
            size_t timeout_seconds = 10;
            bool sync_all = false;

            st::set_value_from_options(m_options, desc)
                (server, "server")
                (m_local_only, "local-only")
                (m_remote_only, "remote-only")
                (ior, {"library-server-ior", "server-ior", "ior"})
                (sync_all, "sync-all")
                (m_sync_versions, "sync-versions")
                (m_sync_all_versions, "sync-all-versions")
                (m_sync_ad_hoc_message_library, "sync-ad-hoc-message-library-message-library")
                (m_sync_current_next_predefined_message_library, "sync-current-next-predefined-message-library")
                (m_sync_current_next_display_template_library, "sync-current-next-display-template-library")
                (m_sync_current_next_display_templates, "sync-current-next-display-templates")
                (m_sync_all_predefined_message_libraries, "sync-all-predefined-message-libraries")
                (m_sync_all_display_template_libraries, "sync-all-display-template-libraries")
                .set<milliseconds, size_t>(m_sync_interval, "sync-interval-ms")
                .set<milliseconds, size_t>(m_sync_interval_backup, "sync-interval-ms")
                .set<milliseconds, size_t>(m_lock_ad_hoc_message_heartbeat_interval, "lock-ad-hoc-message-heartbeat-interval-ms")
                .set<path, std::string>(m_root, "root-dir")
                (m_versions_filename, "versions-filename")
                (m_all_versions_filename, "all-versions-filename")
                (m_ad_hoc_message_library_filename, "ad-hoc-message-library-filename")
                (timeout_seconds, "client-call-timeout-seconds")
                ;

            if (sync_all)
            {
                m_sync_versions = true;
                m_sync_all_versions = true;
                m_sync_ad_hoc_message_library = true;
                m_sync_current_next_predefined_message_library = true;
                m_sync_current_next_display_template_library = true;
                m_sync_current_next_display_templates = true;
                m_sync_all_predefined_message_libraries = true;
                m_sync_all_display_template_libraries = true;
                m_sync_templates = true;
            }

            if (server.size())
            {
                m_server.set_names_and_object_timeout(DAI::get_name_from_string(server), STIS_LIBRARY_SERVANT_NAME, timeout_seconds);
            }

            if (ior.size())
            {
                if (m_server.m_agent_name.empty())
                {
                    m_server.set_names("DummySTISLibraryServer", STIS_LIBRARY_SERVANT_NAME);
                }

                if (boost::iends_with(ior, ".ior")) // ior file
                {
                    ior = st2::file_to_string(ior);
                }

                m_server.assign_object(CorbaUtil::stringToObject<IGenericServantCorbaDef>(ior));
            }

            if (m_server.has_names())
            {
                m_versions_store.remove_all_callbacks();
                m_all_versions_store.remove_all_callbacks();

                if (!m_local_only)
                {
                    // TODO: give me a reason, why do this?
                    m_versions_store.register_pre_query_callback("client", [&] { versions_sqlite_pre_query_callback(); });
                    m_all_versions_store.register_pre_query_callback("client", [&] { all_versions_sqlite_pre_query_callback(); });
                }
            }

            m_root = st2::absoluted(m_root);
            m_file_server_client.parse_options(m_options);
            m_ad_hoc_store.parse_options(m_options);
            m_versions_store.pare_options(m_options);
            m_all_versions_store.pare_options(m_options);
        }

        void initialize()
        {
            std::call_once(m_once, [&]
            {
            });
        }

        void clear()
        {
            m_sync_versions = false;
            m_sync_all_versions = false;
            m_sync_ad_hoc_message_library = false;
            m_sync_current_next_predefined_message_library = false;
            m_sync_current_next_display_template_library = false;
            m_sync_current_next_display_templates = false;
            m_sync_templates = false;
            m_sync_all_predefined_message_libraries = false;
            m_sync_all_display_template_libraries = false;
        }

        std::once_flag m_once;
        SimpleConditionVariable m_running;
        milliseconds m_sync_interval = 1s;
        milliseconds m_sync_interval_backup = m_sync_interval;
        seconds m_sync_all_display_template_libraries_interval = 60s;
        seconds m_sync_all_predefined_message_libraries_interval = 5s;
        seconds m_sync_current_next_display_templates_interval = 5s;
        seconds m_sync_templates_interval = 5s;
        milliseconds m_lock_ad_hoc_message_heartbeat_interval = 1s;
        milliseconds m_lock_ad_hoc_message_heartbeat_timeout = 3s;
        std::string m_options = "uninitialized";
        bool m_sync_versions = false;
        bool m_sync_all_versions = false;
        bool m_sync_ad_hoc_message_library = false;
        bool m_sync_current_next_predefined_message_library = false;
        bool m_sync_current_next_display_template_library = false;
        bool m_sync_current_next_display_templates = false;
        bool m_sync_templates = false;
        bool m_sync_all_predefined_message_libraries = false;
        bool m_sync_all_display_template_libraries = false;
        std::string m_versions_filename = RunParamsEx::expand(RunParamsEx::get_or(RPARAM_STISLIBRARYVERSIONSFILENAME, DEFAULT_STIS_LIBRARY_VERSIONS_FILENAME));
        std::string m_all_versions_filename = RunParamsEx::expand(RunParamsEx::get_or(RPARAM_STISALLLIBRARYVERSIONSFILENAME, DEFAULT_STIS_ALL_LIBRARY_VERSIONS_FILENAME));
        std::string m_ad_hoc_message_library_filename = RunParamsEx::expand(RunParamsEx::get_or(RPARAM_STISADHOCMESSAGELIBRARYFILENAME, DEFAULT_STIS_AD_HOC_MESSAGE_LIBRARY_FILENAME));
        path m_root = st2::absoluted_copy(RunParamsEx::expand(RunParamsEx::get_or(RPARAM_STISROOTDIR, DEFAULT_STIS_ROOT_DIR)));
        //STISAdHocSQLite m_ad_hoc_store;
        //STISLibraryVersionsSQLite m_versions_store;
        //STISAllLibraryVersionsSQLite m_all_versions_store;
        STISVersionsStore m_versions_store;
        STISAllVersionsStore m_all_versions_store;
        STISAdHocStore m_ad_hoc_store;
        SimpleMD5FileManager m_md5_mgr;
        SimpleFileServer::Client m_file_server_client;
        STISLibraryServerNamedObject m_server;
        bool m_local_only = false;
        bool m_remote_only = false;
        bool m_sync_now = false;
        std::shared_ptr<thread_executor> m_executor;
        std::string m_hostname = boost::to_upper_copy(Hostname::getHostname());
    };

    STISLibraryClient& STISLibraryClient::instance()
    {
        return StaticObject<STISLibraryClient>::value();
    }

    STISLibraryClient::STISLibraryClient(std::string options)
        : m_impl(std::make_shared<Impl>(std::move(options)))
    {
    }

    void STISLibraryClient::parse_options(std::string options)
    {
        return m_impl->parse_options(std::move(options));
    }

    bool STISLibraryClient::is_server_online()
    {
        return m_impl->is_server_online();
    }

    std::string STISLibraryClient::get_server_name()
    {
        return m_impl->get_server_name();
    }

    // synchronization

    void STISLibraryClient::start()
    {
        return m_impl->start();
    }

    void STISLibraryClient::stop()
    {
        return m_impl->stop();
    }

    void STISLibraryClient::async_now()
    {
        m_impl->async_now();
    }

    size_t STISLibraryClient::sync_current_message_template_library()
    {
        return m_impl->sync_current_message_template_library();
    }

    size_t STISLibraryClient::sync_current_next_message_template_library()
    {
        return m_impl->sync_current_next_message_template_library();
    }

    bool STISLibraryClient::sync_versions()
    {
        return m_impl->sync_versions();
    }

    bool STISLibraryClient::sync_all_versions()
    {
        return m_impl->sync_all_versions();
    }

    size_t STISLibraryClient::sync_predefined_message_library(std::string version)
    {
        return m_impl->sync_predefined_message_library(std::move(version));
    }

    size_t STISLibraryClient::sync_display_template_library(std::string version)
    {
        return m_impl->sync_display_template_library(std::move(version));
    }

    size_t STISLibraryClient::sync_display_templates(std::string version)
    {
        return m_impl->sync_display_templates(std::move(version));
    }

    size_t STISLibraryClient::sync_templates()
    {
        return m_impl->sync_templates();
    }

    bool STISLibraryClient::sync_ad_hoc_message_library()
    {
        return m_impl->sync_ad_hoc_message_library();
    }

    void STISLibraryClient::change_sync_interval_for_a_while(size_t new_interval_ms, size_t duration_ms)
    {
        m_impl->change_sync_interval_for_a_while(new_interval_ms, duration_ms);
    }

    void STISLibraryClient::change_sync_interval_until(size_t new_interval_ms, size_t check_pred_interval_ms, size_t timeout_ms, std::function<bool()> pred)
    {
        m_impl->change_sync_interval_until(new_interval_ms, check_pred_interval_ms, timeout_ms, std::move(pred));
    }

    // predefined message library

    std::string STISLibraryClient::current_message_library_version()
    {
        return m_impl->current_message_library_version();
    }

    std::string STISLibraryClient::next_message_library_version()
    {
        return m_impl->next_message_library_version();
    }

    std::pair<std::string, std::string> STISLibraryClient::current_next_message_library_versions()
    {
        return m_impl->current_next_message_library_versions();
    }

    void STISLibraryClient::set_current_message_library_version(std::string version)
    {
        return m_impl->set_current_message_library_version(std::move(version));
    }

    void STISLibraryClient::set_next_message_library_version(std::string version)
    {
        return m_impl->set_next_message_library_version(std::move(version));
    }

    void STISLibraryClient::upgrade_message_library_version(std::string version)
    {
        m_impl->upgrade_message_library_version(std::move(version));
    }

    bool STISLibraryClient::has_current_message_library()
    {
        return m_impl->has_current_message_library();
    }

    bool STISLibraryClient::has_next_message_library()
    {
        return m_impl->has_next_message_library();
    }

    bool STISLibraryClient::has_message_library(std::string version)
    {
        return m_impl->has_message_library(std::move(version));
    }

    STSMSGLIB_XML_PTR STISLibraryClient::load_current_message_library_xml()
    {
        return m_impl->load_current_message_library_xml();
    }

    STSMSGLIB_XML_PTR STISLibraryClient::load_next_message_library_xml()
    {
        return m_impl->load_next_message_library_xml();
    }

    STSMSGLIB_XML_PTR STISLibraryClient::load_message_library_xml(std::string version)
    {
        return m_impl->load_message_library_xml(std::move(version));
    }

    IPredefinedMessageLibraryPtr STISLibraryClient::load_message_library()
    {
        return m_impl->load_message_library();
    }

    void STISLibraryClient::add_message_library(std::string version, const Blob& blob)
    {
        return m_impl->add_message_library(std::move(version), blob);
    }

    void STISLibraryClient::remove_message_library(std::string version)
    {
        return m_impl->remove_message_library(std::move(version));
    }

    Blob STISLibraryClient::download_predefined_message_library(std::string version)
    {
        return m_impl->download_predefined_message_library(std::move(version));
    }

    // ad hoc message library

    AdHocMessageMapPtr STISLibraryClient::load_ad_hoc_message_library()
    {
        return m_impl->load_ad_hoc_message_library();
    }

    void STISLibraryClient::set_ad_hoc_message(int key, const std::string& title, const std::string& content)
    {
        m_impl->set_ad_hoc_message(key, title, content);
    }

    void STISLibraryClient::delete_ad_hoc_message(int key)
    {
        m_impl->delete_ad_hoc_message(key);
    }

    AdHocMessageItem STISLibraryClient::get_ad_hoc_message(int key)
    {
        return m_impl->get_ad_hoc_message(key);
    }

    std::pair<std::string, bool> STISLibraryClient::lock_ad_hoc_message(int key)
    {
        return m_impl->lock_ad_hoc_message(key);
    }

    void STISLibraryClient::unlock_ad_hoc_message(int key)
    {
        m_impl->unlock_ad_hoc_message(key);
    }

    // display template library

    std::string STISLibraryClient::current_template_library_version()
    {
        return m_impl->current_template_library_version();
    }

    std::string STISLibraryClient::next_template_library_version()
    {
        return m_impl->next_template_library_version();
    }

    std::pair<std::string, std::string> STISLibraryClient::current_next_template_library_versions()
    {
        return m_impl->current_next_template_library_versions();
    }

    void STISLibraryClient::set_current_template_library_version(std::string version)
    {
        return m_impl->set_current_template_library_version(std::move(version));
    }

    void STISLibraryClient::set_next_template_library_version(std::string version)
    {
        return m_impl->set_next_template_library_version(std::move(version));
    }

    void STISLibraryClient::upgrade_template_library_version(std::string version)
    {
        m_impl->upgrade_template_library_version(std::move(version));
    }

    bool STISLibraryClient::has_current_template_library()
    {
        return m_impl->has_current_template_library();
    }

    bool STISLibraryClient::has_next_template_library()
    {
        return m_impl->has_next_template_library();
    }

    bool STISLibraryClient::has_template_library(std::string version)
    {
        return m_impl->has_template_library(std::move(version));
    }

    bool STISLibraryClient::has_display_templates(std::string version)
    {
        return m_impl->has_display_templates(std::move(version));
    }

    bool STISLibraryClient::has_display_templates()
    {
        return m_impl->has_display_templates();
    }

    STSTMLIB_XML_PTR STISLibraryClient::load_current_template_library_xml()
    {
        return m_impl->load_current_template_library_xml();
    }

    STSTMLIB_XML_PTR STISLibraryClient::load_next_template_library_xml()
    {
        return m_impl->load_next_template_library_xml();
    }

    STSTMLIB_XML_PTR STISLibraryClient::load_template_library_xml(std::string version)
    {
        return m_impl->load_template_library_xml(std::move(version));
    }

    ITemplateLibraryPtr STISLibraryClient::load_template_library()
    {
        return m_impl->load_template_library();
    }

    void STISLibraryClient::add_template_library(std::string version, const Blob& blob)
    {
        return m_impl->add_template_library(std::move(version), blob);
    }

    void STISLibraryClient::remove_template_library(std::string version)
    {
        return m_impl->remove_template_library(std::move(version));
    }

    const std::string& STISLibraryClient::get_lcd_template(const std::string& id)
    {
        return m_impl->get_lcd_template(id);
    }

    const std::string& STISLibraryClient::get_lcd_emergency_template(const std::string& id)
    {
        return m_impl->get_lcd_emergency_template(id);
    }

    const std::string& STISLibraryClient::get_led_template(const std::string& id)
    {
        return m_impl->get_led_template(id);
    }

    const std::string& STISLibraryClient::get_led_emergency_template(const std::string& id)
    {
        return m_impl->get_led_emergency_template(id);
    }

    Blob STISLibraryClient::download_display_template_library(std::string version)
    {
        return m_impl->download_display_template_library(std::move(version));
    }

    std::map<path, Blob> STISLibraryClient::download_templates(const std::vector<path>& excludes)
    {
        return m_impl->download_templates(excludes);
    }

    // both

    void STISLibraryClient::set_versions(const CurNxtMsgTmpLibVers& versions)
    {
        m_impl->set_versions(versions);
    }

    CurNxtMsgTmpLibVers STISLibraryClient::versions()
    {
        return m_impl->versions();
    }

    bool STISLibraryClient::has_invalid_version()
    {
        return m_impl->has_invalid_version();
    }

    bool STISLibraryClient::has_empty_library()
    {
        return m_impl->has_empty_library();
    }

    std::string STISLibraryClient::get_library_version(const std::string& category, const std::string& stage)
    {
        return m_impl->get_library_version(category, stage);
    }

    void STISLibraryClient::set_library_version(const std::string& category, const std::string& stage, std::string version)
    {
        m_impl->set_library_version(category, stage, std::move(version));
    }

    void STISLibraryClient::upgrade_library_version(const std::string& category, std::string version)
    {
        return m_impl->upgrade_library_version(category, std::move(version));
    }

    bool STISLibraryClient::has_library(const std::string& category, std::string version)
    {
        return m_impl->has_library(category, std::move(version));
    }

    Blob STISLibraryClient::get_library(const std::string& category, std::string version)
    {
        return m_impl->get_library(category, std::move(version));
    }

    void STISLibraryClient::add_library(const std::string& category, std::string version, const Blob& blob)
    {
        return m_impl->add_library(category, std::move(version), blob);
    }

    void STISLibraryClient::remove_library(const std::string& category, std::string version)
    {
        return m_impl->remove_library(category, std::move(version));
    }

    // library download

    Blob STISLibraryClient::download_library(const std::string& category, std::string version)
    {
        return m_impl->download_library(category, std::move(version));
    }

    //  status

    AllCurNxtMsgTmpLibVers STISLibraryClient::get_this_station_iscs_stis_library_versions()
    {
        return m_impl->get_this_station_iscs_stis_library_versions();
    }

    AllCurNxtMsgTmpLibVersInfoList STISLibraryClient::get_all_station_iscs_stis_library_versions()
    {
        return m_impl->get_all_station_iscs_stis_library_versions();
    }
}
