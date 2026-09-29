// test.cpp : Defines the entry point for the console application.
//

#include "pch.h"
#include "app/signs/tis_agent/src/stis_protocol/STISLibraryServer.h"
#include "app/signs/tis_agent/src/stis_protocol/STISLibraryServer.inl"
#include "core/utility/tests/boost_test/test_utility/FileGuard.h"
#include "core/utility/tests/boost_test/test_utility/LogFileGuard.h"
#include "core/utility/tests/boost_test/test_utility/DebugLevelGuard.h"
#include "core/utility/tests/boost_test/test_utility/CorbaUtilGuard.h"
#include "app/signs/common_library/tests/boost_test/src/STISTestConfigurations.h"
#include "app/signs/common_library/src/stis_protocol/STISLibrarySQLite.h"
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

namespace
{
    struct MyFixture : Fixture
    {
        MyFixture()
        {
            Naming::getInstance().init();
        }

        CorbaUtilGuard corba;
        FileEx message{STSMSGLIB};
        FileEx templat{STSTMLIB};
    };
}

BOOST_AUTO_TEST_SUITE(APP)
BOOST_AUTO_TEST_SUITE(SIGNS)
BOOST_AUTO_TEST_SUITE(TISAGENT)
BOOST_FIXTURE_TEST_SUITE(STISLibraryServerTest, MyFixture)

using TA_Base_Ex::RunParamsEx;

BOOST_AUTO_TEST_CASE(test)
{
    LogFileGuard log("SIGNS-TISAGENT-STISLibraryServerTest-test.log");

    auto& db = STISLibrarySQLite::instance();

    auto& server = STISLibraryServer::instance();
    server.m_impl->m_db = STISLibrarySQLite::instance_ptr();
    server.m_impl->m_db->open();
    server.m_impl->activate_servant_with_name(STIS_LIBRARY_SERVANT_NAME);

    BOOST_SCOPE_EXIT_ALL(&)
    {
        server.m_impl->deactivate_servant();
        server.m_impl->m_db->close();
    };

    auto& client = STISLibraryClient::instance();
    client.parse_options("--remote-only --server=" + RPARAM_ENTITYNAME_v);

    {
        client.set_current_message_library_version("001");
        client.set_next_message_library_version("001");
        client.set_current_template_library_version("001");
        client.set_next_template_library_version("001");
        BOOST_TEST((server.versions_tuple() == std::make_tuple("001", "001", "001", "001")));
    }

    {
        client.add_message_library("001", message.blob());
        auto blob = client.get_library("message", "001");
        BOOST_TEST(blob == message.blob());
    }

    {
        client.add_template_library("001", templat.blob());
        auto blob = client.get_library("template", "001");
        BOOST_TEST(blob == templat.blob());
    }
}

BOOST_AUTO_TEST_CASE(test_synchronization)
{
    LogFileGuard log("SIGNS-TISAGENT-STISLibraryServerTest-test_synchronization.log");

    auto& db = STISLibrarySQLite::instance();
    db.parse_options("--filename=" + s_server_db_filename);

    auto& server = STISLibraryServer::instance();
    server.m_impl->m_db = STISLibrarySQLite::instance_ptr();
    server.m_impl->m_db->open();
    server.m_impl->activate_servant_with_name(STIS_LIBRARY_SERVANT_NAME);

    BOOST_SCOPE_EXIT_ALL(&)
    {
        server.m_impl->deactivate_servant();
        server.m_impl->m_db->close();
    };

    auto& client = STISLibraryClient{str(boost::format("--use-singleton-local-db=false --sync-all --sync-interval-ms=100 --db-filename=%s --server=%s") % s_client_db_filename % RPARAM_ENTITYNAME_v)};
    client.start();

    {
        server.set_current_message_library_version("001");
        server.set_next_message_library_version("001");
        server.set_current_template_library_version("001");
        server.set_next_template_library_version("001");
        stdex::sleep_for(1s);
        BOOST_TEST((client.versions_tuple() == std::make_tuple("001", "001", "001", "001")));
    }

    {
        server.add_message_library("001", message.blob());
        stdex::sleep_for(1s);
        auto blob = client.get_library("message", "001");
        BOOST_TEST(blob == message.blob());
    }

    {
        server.add_template_library("001", templat.blob());
        stdex::sleep_for(1s);
        auto blob = client.get_library("template", "001");
        BOOST_TEST(blob == templat.blob());
    }
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
