#pragma once
#include "core/utility/tests/boost_test/test_utility/FileGuard.h"
#include "core/utility/tests/boost_test/test_utility/DebugLevelGuard.h"
#include "core/utility/src/base_ex/RunParamsEx.h"

namespace TA_IRS_App::STIS_PROTOCOL::TEST
{
    using TA_Base_Ex::RunParamsEx;
    using namespace std::literals;

    static const std::string OCC_TIS_GATEWAY_PORT       = "10001";
    static const std::string DBG_TIS_GATEWAY_PORT       = "10002";
    static const std::string OCC_TIS_AGENT_PORT         = "20001";
    static const std::string DBG_TIS_AGENT_PORT         = "20002";

    static const std::string s_sftp_root                = R"(D:\P4\develop\J155_TIP\J155\code\transactive\app\signs\common_library\tests\boost_test\data\sftp_root\)";
    static const std::string STSMSGLIB                  = s_sftp_root + R"(PMLIBRARY\001\STSMSGLIB.XML)";
    static const std::string STSTMLIB                   = s_sftp_root + R"(tmlibrary\001\STSTMLIB.XML)";

    static const std::string s_db_filename              = "stis_library.sqlite";
    static const std::string s_occ_server_db_filename   = "stis_library.occ.server.sqlite";
    static const std::string s_dbg_server_db_filename   = "stis_library.dbg.server.sqlite";
    static const std::string s_server_db_filename       = "stis_library.server.sqlite";
    static const std::string s_client_db_filename       = "stis_library.client.sqlite";

    static const FileGuard s_guard_1{s_db_filename};
    static const FileGuard s_guard_2{s_occ_server_db_filename};
    static const FileGuard s_guard_3{s_dbg_server_db_filename};
    static const FileGuard s_guard_4{s_server_db_filename};
    static const FileGuard s_guard_5{s_client_db_filename};

    static const DebugLevelGuard s_debuglevel = TA_Base_Core::DebugUtil::DebugDebug;

    struct Fixture
    {
        Fixture()
        {
            TA_Base_Core::gSetRuntimeAssertAction(FAIL_EXCEPTION);

            RunParamsEx::set("DbConnectionDefaultMaxTry", 1);
            RunParamsEx::set("DbConnectionDefaultTimeout", 1);
            RunParamsEx::set(RPARAM_DBPREFIX + "C830G"s, RPARAM_DBONLINE);
            RunParamsEx::set(RPARAM_DBFOLDERPATH, R"(C:\Develop\LocalTest\Database)");
            RunParamsEx::set(RPARAM_DBCONNECTIONFILE, R"(D:\LocalTest\C830G_TIP\ConfigFiles\CSV\OCCConnectionStrings.csv)");

            RunParamsEx::set("OccTisGateway" "NamedObjectRepository", "corbaloc::localhost:" + OCC_TIS_GATEWAY_PORT + "/" "OccTisGateway" "Repo");
            RunParamsEx::set("DbgTisGateway" "NamedObjectRepository", "corbaloc::localhost:" + DBG_TIS_GATEWAY_PORT + "/" "DbgTisGateway" "Repo");
            RunParamsEx::set("OccTisAgent"   "NamedObjectRepository", "corbaloc::localhost:" + OCC_TIS_AGENT_PORT   + "/" "OccTisAgent"   "Repo");
            RunParamsEx::set("DbgTisAgent"   "NamedObjectRepository", "corbaloc::localhost:" + DBG_TIS_AGENT_PORT   + "/" "DbgTisAgent"   "Repo");
        }
    };
}
