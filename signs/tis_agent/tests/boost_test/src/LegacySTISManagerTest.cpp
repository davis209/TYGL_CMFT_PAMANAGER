// test.cpp : Defines the entry point for the console application.
//

#include "pch.h"
#include "app/signs/tis_agent/src/LegacySTISManager.h"
#include "app/signs/tis_agent/src/stis_protocol/STISLibraryServer.h"
#include "app/signs/tis_agent/src/stis_protocol/STISMessageServer.h"
#include "app/signs/common_library/tests/boost_test/src/STISTestConfigurations.h"
#include "app/signs/common_library/src/stis_protocol/simulator/STISSimulator.h"
#include "app/signs/common_library/src/stis_protocol/STISLibrarySQLite.h"
#include "bus/signs_4669/TisManagerIDL/src/ISTISManagerCorbaDef.h"
#include "core/utility/tests/boost_test/test_utility/LogFileGuard.h"
#include "core/utility/tests/boost_test/test_utility/DebugLevelGuard.h"
#include "core/utility/tests/boost_test/test_utility/CorbaUtilGuard.h"
#include "core/utility/tests/boost_test/test_utility/TestCaseProcess.h"
#include "core/data_access_interface/entity_access/src/STISEntityData.h"
#include "core/utility/src/base_ex/DebugUtilEx.h"
#include "core/utility/src/core/StdEx.h"
#include "core/naming/src/Naming.h"
#include "core/naming/src/NamedObject.h"
#include <boost/test/unit_test.hpp>

using namespace TA_IRS_App;
using namespace TA_Base_Bus;
using namespace TA_Base_Core;
using namespace TA_IRS_App::STIS_PROTOCOL;
using namespace TA_IRS_App::STIS_PROTOCOL::SIMULATOR;
using namespace TA_IRS_App::STIS_PROTOCOL::TEST;
using namespace std::literals;

BOOST_AUTO_TEST_SUITE(APP)
BOOST_AUTO_TEST_SUITE(SIGNS)
BOOST_AUTO_TEST_SUITE(TISAGENT)
BOOST_FIXTURE_TEST_SUITE(LegacySTISManagerTest, STIS_PROTOCOL::TEST::Fixture)

BOOST_AUTO_TEST_CASE(test)
{
    LogFileGuard log("APP-SIGNS-TISAGENT-LegacySTISManagerTest-test.log");

    auto corba = std::make_shared<CorbaUtilGuard>("--location-key=1 --entity-name=OccTisAgent --port=" + OCC_TIS_AGENT_PORT);
    Naming::getInstance().init();

    auto simulator = new STISSimulator{"--name=occ"}; // default versions: { 001, 001, 001, 001 }
    simulator->start();
    stdex::sleep_for(1s);

    TestCaseProcess occ_tis_gateway_process("TisGatewayTest", "APP/SIGNS/TISGATEWAY/STISGatewayTest/occ_tis_gateway_process", "--process");
    occ_tis_gateway_process.start();
    stdex::sleep_for(1s);

    STISLibraryServer::instance().start();
    STISLibraryServer::instance().set_current_message_library_version("001");

    LegacySTISManager::instance().start();

    BOOST_SCOPE_EXIT_ALL(&)
    {
        occ_tis_gateway_process.kill();
        LegacySTISManager::instance().stop();
        STISLibraryServer::instance().stop();
        simulator->stop();
        auto db = STISLibrarySQLite::instance_ptr();
        db->close();
    };

    using STISManager = NamedObject<ISTISManagerCorbaDef>;
    STISManager stis("OccTisAgent", "OccSTis");

    try
    {
        auto version = stis->getCurrentSTISMessageLibraryVersion();
        BOOST_TEST(version == 1);

        STISDestinationList destinations;
        destinations.length(1);
        destinations[0].station = CORBA::string_dup("station");
        destinations[0].pids.length(1);
        destinations[0].pids[0] = CORBA::string_dup("01");
        stis->submitPredefinedDisplayRequest(destinations, static_cast<ELibrarySection>(0), 1, 0, stdex::get_time_YYYYMMDDHHMMSS().c_str(), stdex::get_time_YYYYMMDDHHMMSS().c_str(), 0, "session");
    }
    catch (...)
    {
        BOOST_TEST(false);
    }
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
