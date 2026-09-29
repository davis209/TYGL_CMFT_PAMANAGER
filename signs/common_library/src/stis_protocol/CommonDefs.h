#pragma once
#include <string>

#define RPARAM_STISROOTDIR                      "STISRootDir"
#define RPARAM_STISLIBRARYVERSIONSFILENAME      "STISLibraryVersionsFileName"
#define RPARAM_STISALLLIBRARYVERSIONSFILENAME   "STISAllLibraryVersionsFileName"
#define RPARAM_STISADHOCMESSAGELIBRARYFILENAME  "STISAdHocMessageLibraryFileName"

namespace
{
    const std::string DEFAULT_STIS_CLIENT_ROOT_DIR = R"(C:\transActive\config\database\stis)";
    const std::string DEFAULT_STIS_SERVER_ROOT_DIR = R"(/u01/transactive/data/stis)";

#ifdef WIN32
    const std::string DEFAULT_STIS_ROOT_DIR = DEFAULT_STIS_CLIENT_ROOT_DIR;
#else
    const std::string DEFAULT_STIS_ROOT_DIR = DEFAULT_STIS_SERVER_ROOT_DIR;
#endif

    const std::string DEFAULT_STIS_LIBRARY_VERSIONS_FILENAME = "stis_library_versions.sqlite";
    const std::string DEFAULT_STIS_ALL_LIBRARY_VERSIONS_FILENAME = "stis_all_library_versions.sqlite";
    const std::string DEFAULT_STIS_AD_HOC_MESSAGE_LIBRARY_FILENAME = "stis_ad_hoc_message_library.sqlite";

    const std::string PMLIBRARY = "PMLIBRARY";
    const std::string TMLIBRARY = "tmlibrary";
    const std::string SNAPSHOT = "snapshot";
}
