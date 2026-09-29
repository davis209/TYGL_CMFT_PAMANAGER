// test.cpp : Defines the entry point for the console application.
//

#include "pch.h"
#include "core/utility/tests/boost_test/test_utility/LogFileGuard.h"
#include "core/utility/tests/boost_test/test_utility/DebugLevelGuard.h"
#include "core/utility/tests/boost_test/test_utility/CorbaUtilGuard.h"
#include "app/signs/common_library/tests/boost_test/src/STISTestConfigurations.h"
#include "app/signs/tis_gateway/src/stis_protocol/STISServer.h"
#include "app/signs/tis_gateway/src/stis_protocol/STISLibraryServer.h"
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
BOOST_AUTO_TEST_SUITE(TISGATEWAY)
BOOST_FIXTURE_TEST_SUITE(STISGatewayTest, STIS_PROTOCOL::TEST::Fixture)

BOOST_AUTO_TEST_CASE(occ_tis_gateway_process)
{
    if (!boost::icontains(framework::master_test_suite().argv[1], "--process"))
    {
        BOOST_TEST(true);
        return;
    }

    LogFileGuard log("SIGNS-TISGATEWAY-STISGatewayTest-occ_tis_gateway_process.log", DebugUtil::DebugCORBA);

    auto corba = std::make_shared<CorbaUtilGuard>("--location-key=1 --entity-name=OccTisGateway --port=" + OCC_TIS_GATEWAY_PORT);
    Naming::getInstance().init();

    STISLibraryServer library_server{str(boost::format("--local-sftp --root-dir=%s") % s_sftp_root)};
    library_server.start();

    STISServer stis_server{"--ip=localhost --port=" + STISSimulator::name_to_port("occ")};
    stis_server.start();

    stdex::sleep_for(1min);
}

BOOST_AUTO_TEST_CASE(dbg_tis_gateway_process)
{
    if (!boost::icontains(framework::master_test_suite().argv[1], "--process"))
    {
        BOOST_TEST(true);
        return;
    }

    LogFileGuard log("SIGNS-TISGATEWAY-STISGatewayTest-dbg_tis_gateway_process.log", DebugUtil::DebugCORBA);

    auto corba = std::make_shared<CorbaUtilGuard>("--location-key=3 --entity-name=DbgTisGateway --port=" + DBG_TIS_GATEWAY_PORT);
    Naming::getInstance().init();

    STISServer stis_server{"--ip=localhost --port=" + STISSimulator::name_to_port("dbg")};
    stis_server.start();

    stdex::sleep_for(1min);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
