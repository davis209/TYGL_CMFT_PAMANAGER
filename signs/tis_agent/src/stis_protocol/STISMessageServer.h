#pragma once
#include "app/signs/common_library/src/stis_protocol/message_types/MessageTypes.h"
#include <string>
#include <vector>
#include <memory>

namespace TA_IRS_App::STIS_PROTOCOL::INTERFACES::stismessageserver::detail
{
    using namespace STIS_PROTOCOL::MESSAGE_TYPES;

    struct STISMessageServer
    {
        static STISMessageServer& instance();

        STISMessageServer(std::string options = "");

        void parse_options(std::string options);

        void start();
        void stop();

        struct Impl;
        std::shared_ptr<Impl> m_impl;
    };
}

namespace TA_IRS_App::STIS_PROTOCOL::INTERFACES
{
    using stismessageserver::detail::STISMessageServer;
}
