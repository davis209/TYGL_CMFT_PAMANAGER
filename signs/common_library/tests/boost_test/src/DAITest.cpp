// test.cpp : Defines the entry point for the console application.
//

#include "pch.h"
#include "core/utility/tests/boost_test/test_utility/LogFileGuard.h"
#include "core/utility/tests/boost_test/test_utility/DebugLevelGuard.h"
#include "core/utility/src/base_ex/DAI.h"
#include "core/utility/src/base_ex/DebugUtilEx.h"
#include "core/utility/src/base_ex/RunParamsEx.h"
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

using namespace TA_IRS_App;
using namespace TA_Base_Core;
using namespace TA_Base_Ex;
using namespace std::literals;
using namespace std::string_literals;
using boost::filesystem::path;
using Blob = std::vector<unsigned char>;

namespace
{
    DebugLevelGuard s_debuglevel = DebugUtil::DebugDebug;
}

BOOST_AUTO_TEST_SUITE(SIGNS)
BOOST_AUTO_TEST_SUITE(COMMON)
BOOST_AUTO_TEST_SUITE(DAITest)

BOOST_AUTO_TEST_CASE(test)
{
    LogFileGuard log("SignsCommonLibrary-DAITest-test.log");

    RunParamsEx::set(RPARAM_DBCONNECTIONFILE, R"(D:\LocalTest\C830G_TIP\ConfigFiles\CSV\OCCConnectionStrings.csv)");
    RunParamsEx::set("DbConnectionDefaultMaxTry", 1);
    RunParamsEx::set("DbConnectionDefaultTimeout", 1);
    RunParamsEx::set(RPARAM_LOCATIONKEY, 1);
    RunParamsEx::set(RPARAM_DBPREFIX + "C830G"s, RPARAM_DBONLINE);
    RunParamsEx::set(RPARAM_DBFOLDERPATH, R"(C:\Develop\LocalTest\Database)");

    auto all_tis_agents = DAI::get_all_tis_agent_entities();
    BOOST_TEST(all_tis_agents.size());
    BOOST_TEST(boost::icontains(all_tis_agents[0]->getName(), "TisAgent"));

    auto all_tis_gateways = DAI::get_all_tis_gateway_entities();
    BOOST_TEST(all_tis_gateways.size());
    BOOST_TEST(boost::icontains(all_tis_gateways[0]->getName(), "TisGateway"));

    auto local_tis_agent_name = DAI::get_local_tis_agent_name();
    BOOST_TEST(local_tis_agent_name == "OccTisAgent");

    auto occ_tis_agent_name = DAI::get_occ_tis_agent_name();
    BOOST_TEST(occ_tis_agent_name == "OccTisAgent");

    auto local_tis_gateway_name = DAI::get_local_tis_gateway_name();
    BOOST_TEST(local_tis_gateway_name == "OccTisGateway");

    auto occ_tis_gateway_name = DAI::get_occ_tis_gateway_name();
    BOOST_TEST(occ_tis_gateway_name == "OccTisGateway");
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
