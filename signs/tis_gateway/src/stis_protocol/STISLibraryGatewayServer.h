#pragma once
#include <string>
#include <vector>
#include <memory>

namespace TA_IRS_App::STIS_PROTOCOL::INTERFACES::stislibrarygatewayserver::detail
{
    using Blob = std::vector<unsigned char>;

    struct STISLibraryGatewayServer
    {
        static STISLibraryGatewayServer& instance();

        STISLibraryGatewayServer(std::string options = "");

        void parse_options(std::string options);

        void start();
        void stop();
        Blob download_library(const std::string& category, const std::string& version);

        struct Impl;
        std::shared_ptr<Impl> m_impl;
    };

    using STISLibraryGatewayServerPtr = std::shared_ptr<STISLibraryGatewayServer>;
}

namespace TA_IRS_App::STIS_PROTOCOL::INTERFACES
{
    using stislibrarygatewayserver::detail::STISLibraryGatewayServer;
    using stislibrarygatewayserver::detail::STISLibraryGatewayServerPtr;
}
