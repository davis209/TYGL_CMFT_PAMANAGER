// test.cpp : Defines the entry point for the console application.
//

#include "pch.h"
#include "core/utility/tests/boost_test/test_utility/LogFileGuard.h"
#include "core/utility/tests/boost_test/test_utility/DebugLevelGuard.h"
#include "core/utility/tests/boost_test/test_utility/CorbaUtilGuard.h"
#include "app/signs/common_library/tests/boost_test/src/STISTestConfigurations.h"
#include "app/signs/tis_gateway/src/stis_protocol/STISLibraryServer.h"
#include "app/signs/common_library/src/stis_protocol/STISLibraryClient.h"
#include "core/utility/src/base_ex/DebugUtilEx.h"
#include "core/utility/src/core/FileEx.h"
#include "core/utility/src/core/StdEx.h"
#include "core/naming/src/Naming.h"
#include <boost/test/unit_test.hpp>

using namespace TA_IRS_App;
using namespace TA_Base_Core;
using namespace TA_IRS_App::STIS_PROTOCOL;
using namespace TA_IRS_App::STIS_PROTOCOL::TEST;
using namespace std::literals;
using ta_utility::core::FileEx;

BOOST_AUTO_TEST_SUITE(APP)
BOOST_AUTO_TEST_SUITE(SIGNS)
BOOST_AUTO_TEST_SUITE(TISGATEWAY)
BOOST_FIXTURE_TEST_SUITE(STISLibraryServerTest, STIS_PROTOCOL::TEST::Fixture)

BOOST_AUTO_TEST_CASE(test)
{
    LogFileGuard log("SIGNS-TISGATEWAY-STISLibraryServerTest-test.log");

    auto corba = std::make_shared<CorbaUtilGuard>();
    Naming::getInstance().init();

    STISLibraryServer library_server{str(boost::format("--local-sftp --root-dir=%s") % s_sftp_root)};
    library_server.start();

    STISLibraryClient client{"--remote-only --server=" + RPARAM_ENTITYNAME_v};
    auto blob = client.library_download("message", "001");
    FileEx msg(STSMSGLIB);
    BOOST_TEST((msg.blob() == blob));
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
