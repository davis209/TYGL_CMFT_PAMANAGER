#pragma once
#include "app/signs/common_library/src/stis_protocol/STISLibraryClient.h"
#include "app/signs/common_library/src/stis_protocol/STISAdHocSQLite.h"
#include "app/signs/common_library/src/stis_protocol/STISLibraryVersionsSQLite.h"
#include "app/signs/common_library/src/stis_protocol/STISAllLibraryVersionsSQLite.h"
#include <boost/filesystem.hpp>
#include <string>
#include <vector>
#include <memory>
#include <mutex>

namespace TA_IRS_App::STIS_PROTOCOL::INTERFACES::stislibraryagentserver::detail
{
    using namespace std::string_literals;
    using boost::filesystem::path;
    using Blob = std::vector<unsigned char>;
    using Tuple = std::tuple<std::string, std::string, std::string, std::string>;
    using namespace TA_IRS_App::STIS_PROTOCOL::IMPL;
    using Lock = std::scoped_lock<std::recursive_mutex>;
    using LockPtr = std::shared_ptr<Lock>;
    using LockPtrList = std::vector<LockPtr>;

    struct STISLibraryAgentServer
    {
        static STISLibraryAgentServer& instance();

        STISLibraryAgentServer(std::string options = "");

        void parse_options(std::string options);

        void start();
        void stop();

        Blob download_library(const std::string& category, const std::string& version);
        std::map<path, Blob> download_templates(const std::vector<path>& excludes = {});

        // internal usage

        STISLibraryClient& client();
        STISLibraryClient& occ_client();
        STISAdHocSQLite& ad_hoc_sqlite();
        STISLibraryVersionsSQLite& versions_sqlite();
        STISAllLibraryVersionsSQLite& all_versions_sqlite();
        void wait_for_library_upgrading_complete();

        struct Impl;
        std::shared_ptr<Impl> m_impl;
    };

    using STISLibraryAgentServerPtr = std::shared_ptr<STISLibraryAgentServer>;
}

namespace TA_IRS_App::STIS_PROTOCOL::INTERFACES
{
    using stislibraryagentserver::detail::STISLibraryAgentServer;
    using stislibraryagentserver::detail::STISLibraryAgentServerPtr;
}
