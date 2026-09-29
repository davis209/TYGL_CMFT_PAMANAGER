#pragma once
#include "app/signs/common_library/src/stis_protocol/message_types/MessageTypes.h"
#include "app/signs/common_library/src/stis_protocol/STISLibraryClient.h"
#include <string>
#include <vector>
#include <memory>

namespace TA_IRS_App::STIS_PROTOCOL::INTERFACES::stisstatusserver::detail
{
    using namespace STIS_PROTOCOL::MESSAGE_TYPES;

    struct STISStatusServer
    {
        static STISStatusServer& instance();

        STISStatusServer(std::string options = "");

        void parse_options(std::string options);

        void start();
        void stop();

        // internal-usage

        void sync_with_stis_now();
        void change_sync_with_stis_interval_for_a_while(size_t new_interval_ms, size_t duration_ms);
        void report_status_to_occ();
        std::vector<std::string> get_online_tis_agent_names();
        void wait_for_synchronizing_with_stis_complete();

        struct Impl;
        std::shared_ptr<Impl> m_impl;
    };

    using STISStatusServerPtr = std::shared_ptr<STISStatusServer>;
}

namespace TA_IRS_App::STIS_PROTOCOL::INTERFACES
{
    using stisstatusserver::detail::STISStatusServer;
    using stisstatusserver::detail::STISStatusServerPtr;
}
