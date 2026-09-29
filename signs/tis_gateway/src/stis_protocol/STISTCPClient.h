#pragma once
#include "app/signs/common_library/src/stis_protocol/message_types/MessageDataTypes.h"

namespace TA_IRS_App::STIS_PROTOCOL::IMPL::stistcpclient::detail
{
    using namespace MESSAGE_DATA_TYPES;

    struct STISTCPClient
    {
        STISTCPClient(std::string options = "");

        void parse_options(std::string options);

        void start();
        void stop();
        GENERALPACKET_PTR send_message(GENERALPACKET packet);
        GENERALPACKET_6_PTR send_message_6(GENERALPACKET packet);

        struct Impl;
        std::shared_ptr<Impl> m_impl;
    };
}

namespace TA_IRS_App::STIS_PROTOCOL::IMPL
{
    using stistcpclient::detail::STISTCPClient;
}
