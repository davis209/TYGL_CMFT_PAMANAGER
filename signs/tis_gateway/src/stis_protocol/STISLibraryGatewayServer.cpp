#include "pch.h"
#include "STISLibraryGatewayServer.h"
#include "app/signs/common_library/src/stis_protocol/STISLibraryClient.h"
#include "core/utilities/src/CallstackLogger.h"
#include "core/utility/src/base_ex/GenericServantCorbaDef.h"
#include "core/utility/src/base_ex/DebugUtilEx.h"
#include "core/utility/src/base_ex/SimpleSFTP.h"
#include "core/utility/src/base_ex/LocationAccessFactoryEx.h"
#include "core/utility/src/core/StdEx.h"
#include "core/utility/src/core/SimpleConditionVariable.h"
#include "core/utility/src/core/Serialize.h"
#include "core/utility/src/core/StaticObject.h"
#include "core/utility/src/core/FileEx.h"
#include "core/utility/src/core/TagInvoke.h"
#include "core/utility/src/core/MakeNameValuePairsString.h"
#include "core/utility/src/core/algorithm/file_system.h"

#define RPARAM_STISLIBRARYGATEWAYSERVERIORFILEPATH "STISLibraryGatewayServerIorFilePath"

namespace
{
    const std::string DEFAULT_STIS_LIBRARY_GATEWAY_SERVER_IOR_FILENAME = "stis_library_gateway_server.ior";
}

namespace TA_IRS_App::STIS_PROTOCOL::INTERFACES::stislibrarygatewayserver::detail
{
    using namespace std::string_literals;
    using namespace TA_Base_Core;
    using namespace std::chrono;
    using boost::filesystem::path;

    using st::Serialize;
    using st::StaticObject;
    using st::FileEx;
    using st::SimpleConditionVariable;
    using TA_Base_Ex::SimpleSFTP;

    struct STISLibraryGatewayServer::Impl : GenericServantCorbaDef
    {
        using ThisClass = Impl;

        Impl(std::string options = "")
        {
            set_class_name("STISLibraryGatewayServer");
            parse_options(std::move(options));
        }

        void start()
        {
            initialize();
            activate_servant_with_name(STIS_LIBRARY_SERVANT_NAME);

            if (!m_ior.empty())
            {
                FileEx(m_ior).set_content(CorbaUtil::objectToString(m_this)).write();
            }
        }

        void stop()
        {
            deactivate_servant();
        }

        // GenericServantCorbaDef

        BOOST_DESCRIBE_CLASS
        (
            Impl, (GenericServantCorbaDef),
            (
                // on_generic_corba_invoke_return
                download_library,
                download_predefined_message_library,
                download_display_template_library,
                download_templates,
                ), (), ()
        );

        virtual GenericServantCorbaDef::Result on_generic_corba_invoke_return(std::string fn, std::string args) override
        {
            LOG_DEBUG("on_generic_corba_invoke_return(): %s", fn);
            return dispatch_generic_corba_invoke_return(this, std::move(fn), std::move(args));
        }

        // interface implementation

        Blob download_library(const std::string& category, const std::string& version)
        {
            LOG_CALLSTACK(boost::format("STISLibraryGatewayServer::download_library[%s:%s]") % category % version);

            if (ThisLocation::is_not_occ())
            {
                return m_occ_gateway_library_client->download_library(category, version);
            }

            path file_path = m_root_dir;

            if (boost::iequals(category, "message"))
            {
                file_path /= "PMLIBRARY/" + version + "/STSMSGLIB.XML";
            }
            else if (boost::iequals(category, "template"))
            {
                file_path /= "tmlibrary/" + version + "/STSTMLIB.XML";
            }

            LOG_DEBUG("download_library(): begin download %s", file_path);

            SimpleSFTP sftp{m_options};
            if (auto blob = sftp.get(file_path.make_preferred()))
            {
                LOG_DEBUG("download_library(): end download %s", file_path);
                return *blob;
            }

            return {};
        }

        Blob download_predefined_message_library(const std::string& version)
        {
            LOG_CALLSTACK(boost::format("STISLibraryGatewayServer::download_predefined_message_library[%s]") % version);

            SimpleSFTP sftp{m_options};
            if (auto blob = sftp.get(m_root_dir / PMLIBRARY / version / "STSMSGLIB.XML"); blob && blob->size())
            {
                return *blob;
            }

            return {};
        }

        Blob download_display_template_library(const std::string& version)
        {
            LOG_CALLSTACK(boost::format("STISLibraryGatewayServer::download_display_template_library[%s]") % version);

            SimpleSFTP sftp{m_options};
            if (auto blob = sftp.get(m_root_dir / TMLIBRARY / version / "STSTMLIB.XML"); blob && blob->size())
            {
                return *blob;
            }

            return {};
        }

        std::map<path, Blob> download_templates(const std::vector<path>& excludes)
        {
            LOG_CALLSTACK("STISLibraryGatewayServer::download_templates");
            LOG_DEBUG("download_templates(): begin download templates");

            if (ThisLocation::is_not_occ())
            {
                return m_occ_gateway_library_client->download_templates(excludes);
            }

            std::map<path, Blob> res;
            auto top = m_root_dir / TMLIBRARY;
            auto templates = std::vector<path>{"lcdemgtemplate", "lcdtemplate", "ledemgtemplate", "ledtemplate", "snapshot"};
            auto dirs = st::transform_to_vector(templates, [&](auto& dir) { return top / dir; });

            SimpleSFTP sftp{m_options};

            auto files = sftp.recursive_ls_file_names(dirs);
            auto missing = st::remove_copy(files, excludes);

            if (missing.size())
            {
                auto&& [local, remote, download] = std::tie(excludes, files, missing);
                LOGLARGESTRING_DEBUG("download_templates():\n%s\n%s\n%s", nvps(local), nvps(remote), nvps(download));

                for (auto&& [p, data] : sftp.mget(missing))
                {
                    res.emplace(relative(p, top), std::move(*data));
                }
            }

            LOG_DEBUG("download_templates(): end download templates");
            return res;
        }

        void parse_options(std::string options)
        {
            using namespace boost::program_options;

            if (options.empty() || m_options == options)
            {
                return;
            }

            m_options = std::move(options);

            options_description desc;
            desc.add_options()
                ("sftp-root-dir", value<std::string>())("root-dir", value<std::string>())
                ("generate-ior-file", value<std::string>()->implicit_value("default"))
                ;

            st::set_value_from_options(m_options, desc)
                .set<path, std::string>(m_root_dir, {"sftp-root-dir", "root-dir"})
                .set<path, std::string>(m_ior, {"generate-ior-file"})
                ;

            if (m_ior == "default")
            {
                m_ior = st2::absoluted_copy(RunParamsEx::get_or(RPARAM_STISLIBRARYGATEWAYSERVERIORFILEPATH, DEFAULT_STIS_LIBRARY_GATEWAY_SERVER_IOR_FILENAME));
            }

            if (ThisLocation::is_not_occ())
            {
                m_occ_gateway_library_client = std::make_shared<STISLibraryClient>("--server=occ-tis-gateway");
            }

            if (m_root_dir == path("."))
            {
                m_root_dir.clear();
            }

            LOG_DEBUG("parse_options(): root-dir=%s", m_root_dir);
        }

        void initialize()
        {
            std::call_once(m_once, [&]
            {
            });
        }

        std::string m_options;
        std::once_flag m_once;
        path m_root_dir;
        path m_ior;
        STISLibraryClientPtr m_occ_gateway_library_client;
    };

    STISLibraryGatewayServer& STISLibraryGatewayServer::instance()
    {
        return StaticObject<STISLibraryGatewayServer>::value();
    }

    STISLibraryGatewayServer::STISLibraryGatewayServer(std::string options)
        : m_impl(std::make_shared<Impl>(std::move(options)))
    {
    }

    void STISLibraryGatewayServer::start()
    {
        return m_impl->start();
    }

    void STISLibraryGatewayServer::stop()
    {
        return m_impl->stop();
    }

    void STISLibraryGatewayServer::parse_options(std::string options)
    {
        return m_impl->parse_options(std::move(options));
    }

    Blob STISLibraryGatewayServer::download_library(const std::string& category, const std::string& version)
    {
        return m_impl->download_library(category, version);
    }
}
