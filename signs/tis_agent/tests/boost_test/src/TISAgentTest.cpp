// test.cpp : Defines the entry point for the console application.
//

#include "pch.h"
#include "app/signs/common_library/tests/boost_test/src/STISTestConfigurations.h"
#include "core/utility/tests/boost_test/test_utility/LogFileGuard.h"
#include "core/utility/tests/boost_test/test_utility/DebugLevelGuard.h"
#include "core/utility/tests/boost_test/test_utility/CorbaUtilGuard.h"
#include "core/utility/tests/boost_test/test_utility/TestCaseProcess.h"
#include "app/signs/tis_agent/src/stis_protocol/STISStatusServer.h"
#include "app/signs/tis_agent/src/stis_protocol/STISLibraryServer.h"
#include "app/signs/tis_agent/src/stis_protocol/STISMessageServer.h"
#include "app/signs/tis_agent/src/LegacySTISManager.h"
#include "app/signs/common_library/src/stis_protocol/simulator/STISSimulator.h"
#include "core/utility/src/base_ex/DebugUtilEx.h"
#include "core/utility/src/core/StdEx.h"
#include "core/naming/src/Naming.h"
#include <boost/test/unit_test.hpp>

using namespace TA_IRS_App;
using namespace TA_Base_Core;
using namespace TA_IRS_App::STIS_PROTOCOL;
using namespace TA_IRS_App::STIS_PROTOCOL::SIMULATOR;
using namespace TA_IRS_App::STIS_PROTOCOL::TEST;
using namespace std::literals;
using namespace boost::unit_test;

BOOST_AUTO_TEST_SUITE(APP)
BOOST_AUTO_TEST_SUITE(SIGNS)
BOOST_AUTO_TEST_SUITE(TISAGENT)
BOOST_FIXTURE_TEST_SUITE(TisAgentTest, STIS_PROTOCOL::TEST::Fixture)

BOOST_AUTO_TEST_CASE(test_occ_tisagent_with_occ_gateway)
{
    LogFileGuard log("SIGNS-TISAGENT-STISStatusServerTest-test_occ_tisagent_with_occ_gateway.log");

    auto corba = std::make_shared<CorbaUtilGuard>("--location-key=1 --entity-name=OccTisAgent --port=" + OCC_TIS_AGENT_PORT);
    Naming::getInstance().init();

    STISSimulator simulator{"--name=occ"}; // default versions: { 001, 001, 001, 001 }
    simulator.start();
    stdex::sleep_for(1s);

    TestCaseProcess occ_tis_gateway_process("TisGatewayTest", "APP/SIGNS/TISGATEWAY/STISGatewayTest/occ_tis_gateway_process", "--process");
    occ_tis_gateway_process.start();
    stdex::sleep_for(1s);

    auto& library_server = STISLibraryServer::instance();
    library_server.start();

    STISStatusServer status_server{"--status-sync-interval-seconds=1"};
    status_server.start();
    stdex::sleep_for(2s);

    BOOST_SCOPE_EXIT_ALL(&)
    {
        status_server.stop();
        library_server.stop();
        occ_tis_gateway_process.kill();
        simulator.stop();
        stdex::sleep_for(2s);
    };

    auto versions = status_server.get_library_versions();
    BOOST_TEST(versions.iscs.current_message_library_version == "001");
    BOOST_TEST(versions.stis.current_message_library_version == "001");
    BOOST_TEST(library_server.has_message_library("001"));
    BOOST_TEST(library_server.has_message_library("002") == false);

    simulator.set_current_message_library_version("002");
    stdex::sleep_for(2s);
    versions = status_server.get_library_versions();
    BOOST_TEST(versions.stis.current_message_library_version == "002");
    BOOST_TEST(library_server.has_message_library("002") == true);
}

BOOST_AUTO_TEST_CASE(test_dbg_tis_agent_with_dbg_gateway)
{
    LogFileGuard log("SIGNS-TISAGENT-STISStatusServerTest-test_dbg_tis_agent_with_dbg_gateway.log");

    auto corba = std::make_shared<CorbaUtilGuard>("--location-key=3 --entity-name=DbgTisAgent --port=" + DBG_TIS_AGENT_PORT);
    Naming::getInstance().init();

    STISSimulator simulator{"--name=dbg"}; // default versions: { 001, 001, 001, 001 }
    simulator.start();
    stdex::sleep_for(1s);

    TestCaseProcess dbg_tis_gateway_process("TisGatewayTest", "APP/SIGNS/TISGATEWAY/STISGatewayTest/dbg_tis_gateway_process", "--process");
    dbg_tis_gateway_process.start();
    stdex::sleep_for(1s);

    auto& library_server = STISLibraryServer::instance();
    library_server.start();

    STISStatusServer status_server{"--status-sync-interval-seconds=1"};
    status_server.start();
    stdex::sleep_for(2s);

    BOOST_SCOPE_EXIT_ALL(&)
    {
        status_server.stop();
        library_server.stop();
        dbg_tis_gateway_process.kill();
        simulator.stop();
        stdex::sleep_for(2s);
    };

    auto versions = status_server.get_library_versions();
    BOOST_TEST(versions.stis.current_message_library_version == "001");

    simulator.set_current_message_library_version("002");
    stdex::sleep_for(2s);
    versions = status_server.get_library_versions();
    BOOST_TEST(versions.stis.current_message_library_version == "002");
}

BOOST_AUTO_TEST_CASE(occ_tis_agent_process)
{
    if (!boost::icontains(framework::master_test_suite().argv[1], "--process"))
    {
        BOOST_TEST(true);
        return;
    }

    LogFileGuard log("SIGNS-TISAGENT-STISStatusServerTest-occ_tis_agent_process.log");

    auto corba = std::make_shared<CorbaUtilGuard>("--location-key=1 --entity-name=OccTisAgent --port=" + OCC_TIS_AGENT_PORT);
    Naming::getInstance().init();

    STISLibraryServer::instance().parse_options("--library-upgrade-timeout-seconds=1 --db-filename=" + s_occ_server_db_filename);
    STISLibraryServer::instance().start();
    STISMessageServer::instance().start();

    STISStatusServer::instance().parse_options("--status-sync-interval-seconds=1 --station-versions-sync-interval-seconds=1");
    STISStatusServer::instance().start();

    LegacySTISManager::instance().start();

    stdex::sleep_for(1min);
}

BOOST_AUTO_TEST_CASE(dbg_tis_agent_process)
{
    if (!boost::icontains(framework::master_test_suite().argv[1], "--process"))
    {
        BOOST_TEST(true);
        return;
    }

    LogFileGuard log("SIGNS-TISAGENT-STISStatusServerTest-dbg_tis_agent_process.log");

    auto corba = std::make_shared<CorbaUtilGuard>("--location-key=3 --entity-name=DbgTisAgent --port=" + DBG_TIS_AGENT_PORT);
    Naming::getInstance().init();

    STISLibraryServer::instance().parse_options("--db-filename=" + s_dbg_server_db_filename);
    STISLibraryServer::instance().start();

    STISMessageServer::instance().start();

    STISStatusServer::instance().parse_options("--status-sync-interval-seconds=1");
    STISStatusServer::instance().start();

    LegacySTISManager::instance().start();

    stdex::sleep_for(1min);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
