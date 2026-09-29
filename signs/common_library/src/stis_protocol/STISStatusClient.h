#pragma once
#include "CommonDefs.h"
#include "app/signs/common_library/src/stis_protocol/STISLibraryClient.h"
#include "app/signs/common_library/src/stis_protocol/STISAllLibraryVersionsSQLite.h"
#include "app/signs/common_library/src/stis_protocol/message_types/MessageTypes.h"

namespace TA_IRS_App::STIS_PROTOCOL::INTERFACES::stisstatusclient::detail
{
    using namespace TA_IRS_App::STIS_PROTOCOL::IMPL;

    struct StationStatusReport
    {
        std::string entity_name;
        size_t location_key = 0;
        std::string location_name;
        std::string location_display_name;
        bool has_current_message_library = false;
        bool has_next_message_library = false;
        bool has_current_template_library = false;
        bool has_next_template_library = false;
        std::map<std::string, std::vector<std::string>> versions;
        TA_PP_TUPLE_ENABLE_MEMBER_UTILITIES(StationStatusReport, (entity_name, location_key, location_name, location_display_name, has_current_message_library, has_next_message_library, has_current_template_library, has_next_template_library, versions));
    };

    struct STISStatusClient
    {
        static STISStatusClient& instance();

        STISStatusClient(std::string options = "");

        void parse_options(std::string options);

        void station_report_status_to_occ(); // report to server(OCC)

        // internal usage
        void set_client(STISAllLibraryVersionsSQLite& client);
        void set_client(STISLibraryClient& client);

        struct Impl;
        std::shared_ptr<Impl> m_impl;
    };

    using STISStatusClientPtr = std::shared_ptr<STISStatusClient>;
}

namespace TA_IRS_App::STIS_PROTOCOL::INTERFACES
{
    using stisstatusclient::detail::STISStatusClient;
    using stisstatusclient::detail::STISStatusClientPtr;
    using stisstatusclient::detail::StationStatusReport;
}

namespace TA_IRS_App::STIS_PROTOCOL
{
    using namespace INTERFACES;
}
