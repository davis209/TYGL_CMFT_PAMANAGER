#include "pch.h"
#include "STISStatusClient.h"
#include "STISMessageClient.h"
#include "CommonDefs.h"
#include "STISAllLibraryVersionsSQLite.h"
#include "core/utilities/src/CallstackLogger.h"
#include "core/utility/src/core/StaticObject.h"
#include "core/utility/src/base_ex/DAI.h"
#include "core/utility/src/base_ex/DebugUtilEx.h"
#include "core/utility/src/base_ex/RunParamsEx.h"
#include "core/utility/src/base_ex/LocationAccessFactoryEx.h"
#include "core/utility/src/core/StdEx.h"
#include "core/utility/src/base_ex/GenericServantCorbaDef.h"

using namespace std::string_literals;
using namespace boost::program_options;
using boost::filesystem::path;
using st::StaticObject;
using namespace TA_Base_Ex;
using namespace TA_Base_Core;
using namespace TA_IRS_App::STIS_PROTOCOL::IMPL;
using STISStatusServerNamedObject = GenericServantCorbaDefNamedObject;

namespace TA_IRS_App::STIS_PROTOCOL::INTERFACES::stisstatusclient::detail
{
    struct STISStatusClient::Impl
    {
        Impl(std::string options = "")
        {
            parse_options(std::move(options));
        }

        AllCurNxtMsgTmpLibVers get_this_station_iscs_stis_library_versions()
        {
            return {};
        }

        AllCurNxtMsgTmpLibVersInfoList get_all_station_iscs_stis_library_versions()
        {
            return {};
        }

        void station_report_status_to_occ()
        {
            LOG_CALLSTACK("STISStatusClient::station_report_status_to_occ");
            static std::string s_entity_name = RPARAM_ENTITYNAME_v;
            StationStatusReport report;
            report.entity_name = s_entity_name;
            report.location_key = ThisLocation::key();
            report.location_name = ThisLocation::name();
            report.location_display_name = ThisLocation::display_name();
            report.has_current_message_library = m_library_client->has_current_message_library();
            report.has_next_message_library = m_library_client->has_next_message_library();
            report.has_current_template_library = m_library_client->has_current_template_library();
            report.has_next_template_library = m_library_client->has_next_template_library();
            report.versions = m_all_library_versions_sqlite->get_versions();
            m_server.corba_call("station_report_status_to_occ", report);
        }

    public: // implementation

        void set_client(STISLibraryClient& client)
        {
            m_library_client = &client;
        }

        void set_client(STISAllLibraryVersionsSQLite& client)
        {
            m_all_library_versions_sqlite = &client;
        }

        void parse_options(std::string options)
        {
            LOG_CALLSTACK("STISStatusClient::parse_options");

            if (m_options == options)
            {
                return;
            }

            m_options = std::move(options);

            options_description desc;
            desc.add_options()
                ("server", value<std::string>()->default_value("local-tis-agent"))
                ("client-call-timeout-seconds", value<size_t>()->default_value(10))
                ;

            std::string server;
            size_t timeout = 10;

            st::set_value_from_options(m_options, desc)
                (server, "server")
                (timeout, "client-call-timeout-seconds")
                ;

            m_server.set_names_and_object_timeout(DAI::get_name_from_string(server), STIS_STATUS_SERVANT_NAME, timeout);
        }

        std::string m_options;
        STISStatusServerNamedObject m_server;
        STISLibraryClient* m_library_client = &STISLibraryClient::instance();
        STISAllLibraryVersionsSQLite* m_all_library_versions_sqlite = &STISAllLibraryVersionsSQLite::instance();
    };

    STISStatusClient& STISStatusClient::instance()
    {
        return StaticObject<STISStatusClient>::value();
    }

    STISStatusClient::STISStatusClient(std::string options)
        : m_impl(std::make_shared<Impl>(std::move(options)))
    {
    }

    void STISStatusClient::parse_options(std::string options)
    {
        m_impl->parse_options(std::move(options));
    }

    void STISStatusClient::station_report_status_to_occ()
    {
        m_impl->station_report_status_to_occ();
    }

    void STISStatusClient::set_client(STISAllLibraryVersionsSQLite& client)
    {
        m_impl->set_client(client);
    }

    void STISStatusClient::set_client(STISLibraryClient& client)
    {
        m_impl->set_client(client);
    }
}
