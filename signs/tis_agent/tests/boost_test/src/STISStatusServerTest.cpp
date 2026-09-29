// test.cpp : Defines the entry point for the console application.
//

#include "pch.h"
#include "app/signs/common_library/tests/boost_test/src/STISTestConfigurations.h"
#include "core/utility/tests/boost_test/test_utility/LogFileGuard.h"
#include "core/utility/tests/boost_test/test_utility/CorbaUtilGuard.h"
#include "app/signs/tis_agent/src/stis_protocol/STISStatusServer.h"
#include "app/signs/tis_agent/src/stis_protocol/STISLibraryServer.h"
#include "core/utility/src/base_ex/DebugUtilEx.h"
#include "core/naming/src/Naming.h"
#include <boost/test/unit_test.hpp>

using namespace TA_IRS_App;
using namespace TA_Base_Core;
using namespace TA_IRS_App::STIS_PROTOCOL;
using namespace std::literals;

BOOST_AUTO_TEST_SUITE(APP)
BOOST_AUTO_TEST_SUITE(SIGNS)
BOOST_AUTO_TEST_SUITE(TISAGENT)
BOOST_FIXTURE_TEST_SUITE(STISStatusServerTest, STIS_PROTOCOL::TEST::Fixture)

using TA_Base_Ex::RunParamsEx;

BOOST_AUTO_TEST_CASE(test)
{
    LogFileGuard log("SIGNS-TISAGENT-STISStatusServerTest-test.log");

    auto corba = std::make_shared<CorbaUtilGuard>();
    Naming::getInstance().init();

    STISLibraryServer::instance().start();
    auto& library = STISLibraryServer::instance();
    library.set_current_message_library_version("001");
    library.set_next_message_library_version("001");
    library.set_current_template_library_version("001");
    library.set_next_template_library_version("001");

    STISStatusServer status_server;
    status_server.start();
    stdex::sleep_for(1s);

    auto versions = status_server.get_library_versions();
    BOOST_TEST((versions.iscs.tied() == std::make_tuple("001", "001", "001", "001")));

    auto all_versions = status_server.get_all_library_versions();
    BOOST_TEST((all_versions.size()));
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
