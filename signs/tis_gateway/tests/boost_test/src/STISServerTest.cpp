// test.cpp : Defines the entry point for the console application.
//

#include "pch.h"
#include "core/utility/tests/boost_test/test_utility/LogFileGuard.h"
#include "core/utility/tests/boost_test/test_utility/DebugLevelGuard.h"
#include "core/utility/tests/boost_test/test_utility/CorbaUtilGuard.h"
#include "app/signs/tis_gateway/src/stis_protocol/STISServer.h"
#include "app/signs/common_library/src/stis_protocol/simulator/STISSimulator.h"
#include "core/utility/src/base_ex/DebugUtilEx.h"
#include "core/utility/src/core/StdEx.h"
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

using namespace TA_IRS_App;
using namespace TA_Base_Core;
using namespace TA_IRS_App::STIS_PROTOCOL;
using namespace TA_IRS_App::STIS_PROTOCOL::SIMULATOR;
using namespace std::literals;

namespace
{
    DebugLevelGuard s_debuglevel = DebugUtil::DebugDebug;

    struct Fixture
    {
        Fixture()
        {
            simulator.start();
            gateway.start();
            stdex::sleep_for(100ms);
        }

        CorbaUtilGuard corba;
        STISServer gateway;
        STISSimulator simulator;
    };

    Destination make_Destination()
    {
        Destination dest;
        dest.system_id = "TEL";
        dest.station_id = "OCC"; // "TE01", "ASTN"
        dest.pid_list.emplace_back("101");
        // dest.number_of_pids = dest.pid_list.size();
        return dest;
    }

    PredefinedMessage make_PredefinedMessage()
    {
        PredefinedMessage msg;
        msg.message_tag = "tag";
        msg.start_time = stdex::get_time_YYYYMMDDHHMMSS();
        msg.end_time = stdex::get_time_YYYYMMDDHHMMSS();
        msg.priority = 0;
        return msg;
    }

    AdHodMessage make_AdHodMessage()
    {
        AdHodMessage msg;
        msg.message_tag = "tag";
        msg.start_time = stdex::get_time_YYYYMMDDHHMMSS();
        msg.end_time = stdex::get_time_YYYYMMDDHHMMSS();
        msg.priority = 0;
        msg.message_text = "hello, world";
        // msg.message_length = msg.message_text.size();
        return msg;
    }

    PredefinedDisplayTemplate make_PredefinedDisplayTemplate(EDisplayTemplateType type = EDisplayTemplateType::Default)
    {
        PredefinedDisplayTemplate pdt;
        pdt.display_template_type = type;
        pdt.display_template_id = "001";
        pdt.start_time = stdex::get_time_YYYYMMDDHHMMSS();
        pdt.end_time = stdex::get_time_YYYYMMDDHHMMSS();
        return pdt;
    }

    CurNxtMsgTmpLibVers make_CurNxtMsgTmpLibVers()
    {
        CurNxtMsgTmpLibVers iscs_versions;
        iscs_versions.tied() = std::tie("001", "001", "001", "001");
        return iscs_versions;
    }
}

BOOST_AUTO_TEST_SUITE(APP)
BOOST_AUTO_TEST_SUITE(SIGNS)
BOOST_AUTO_TEST_SUITE(TISGATEWAY)
BOOST_FIXTURE_TEST_SUITE(STISServerTest, Fixture)

BOOST_AUTO_TEST_CASE(test)
{
    LogFileGuard log("SIGNS-TISGATEWAY-STISServerTest-test.log");

    auto dest = make_Destination();

    try
    {
        {
            auto msg = make_PredefinedMessage();
            gateway.submit_M10_DisplayPredefinedMessageRequest(dest, msg);
        }

        {
            auto msg = make_AdHodMessage();
            gateway.submit_M11_DisplayAdHocMessageRequest(dest, msg);
        }

        {
            std::vector<int> priority = {0, 0, 0, 0, 0, 0, 0, 0};
            gateway.submit_M20_ClearCurrentMessageRequest(dest, priority);
        }

        {
            EPIDControlOn control_on = EPIDControlOn::NoAction;
            EPIDControlOff control_off = EPIDControlOff::NoAction;
            gateway.submit_M21_PIDOnOffControlRequest(dest, control_on, control_off);
        }

        {
            auto lcd = make_PredefinedDisplayTemplate(EDisplayTemplateType::LCDNormal);
            auto led = make_PredefinedDisplayTemplate(EDisplayTemplateType::Spare);
            gateway.submit_M22_SendPredefinedDisplayTemplateRequest(dest, lcd, led);
        }

        {
            auto type = EDisplayTemplateType::LCDNormal;
            gateway.submit_M23_RemoveDisplayTemplateRequest(dest, type);
        }

        {
            std::string station = "OCC";
            gateway.submit_M30_StationSTISStatusRequest(station);
        }

        {
            std::string destination_occ = "OCC";
            auto iscs_versions = make_CurNxtMsgTmpLibVers();
            gateway.submit_M31_OCCSTISStatusSyncRequest(destination_occ, iscs_versions);
        }

        {
            std::string station = "OCC";
            gateway.submit_M32_AllStationSTISStatusRequest(station);
        }

        {
            std::string station = "OCC";
            std::string pid = "101";
            gateway.submit_M50_CurrentDisplayMessageTemplateRequest(station, pid);
        }

        {
            ELibraryType type = ELibraryType::PredefinedMessage;
            std::string version = "002";
            gateway.submit_M70_UpgradePredefinedMessageDisplayTemplateLibraryRequest(type, version);
        }
    }
    catch (std::exception& e)
    {
        LOG_ERROR("%s", e);
        BOOST_TEST(false);
    }
}

BOOST_AUTO_TEST_CASE(test_M10)
{
    LogFileGuard log("SIGNS-TISGATEWAY-STISServerTest-test_M10.log");

    auto dest = make_Destination();
    auto msg = make_PredefinedMessage();

    try
    {
        gateway.submit_M10_DisplayPredefinedMessageRequest(dest, msg);
    }
    catch (std::exception& e)
    {
        LOG_ERROR("%s", e);
        BOOST_TEST(false);
    }
}

BOOST_AUTO_TEST_CASE(test_M11)
{
    LogFileGuard log("SIGNS-TISGATEWAY-STISServerTest-test_M11.log");

    auto dest = make_Destination();
    auto msg = make_AdHodMessage();

    try
    {
        gateway.submit_M11_DisplayAdHocMessageRequest(dest, msg);
    }
    catch (std::exception& e)
    {
        LOG_ERROR("%s", e);
        BOOST_TEST(false);
    }
}

BOOST_AUTO_TEST_CASE(test_M20)
{
    LogFileGuard log("SIGNS-TISGATEWAY-STISServerTest-test_M20.log");

    auto dest = make_Destination();
    std::vector<int> priority = {0, 0, 0, 0, 0, 0, 0, 0};

    try
    {
        gateway.submit_M20_ClearCurrentMessageRequest(dest, priority);
    }
    catch (std::exception& e)
    {
        LOG_ERROR("%s", e);
        BOOST_TEST(false);
    }
}

BOOST_AUTO_TEST_CASE(test_M21)
{
    LogFileGuard log("SIGNS-TISGATEWAY-STISServerTest-test_M21.log");

    auto dest = make_Destination();
    EPIDControlOn control_on = EPIDControlOn::AllOff;
    EPIDControlOff control_off = EPIDControlOff::Reserved;

    try
    {
        gateway.submit_M21_PIDOnOffControlRequest(dest, control_on, control_off);
    }
    catch (std::exception& e)
    {
        LOG_ERROR("%s", e);
        BOOST_TEST(false);
    }
}

BOOST_AUTO_TEST_CASE(test_M22)
{
    LogFileGuard log("SIGNS-TISGATEWAY-STISServerTest-test_M22.log");

    auto dest = make_Destination();
    auto lcd = make_PredefinedDisplayTemplate(EDisplayTemplateType::LCDNormal);
    auto led = make_PredefinedDisplayTemplate(EDisplayTemplateType::Spare);

    try
    {
        gateway.submit_M22_SendPredefinedDisplayTemplateRequest(dest, lcd, led);
    }
    catch (std::exception& e)
    {
        LOG_ERROR("%s", e);
        BOOST_TEST(false);
    }
}

BOOST_AUTO_TEST_CASE(test_M23)
{
    LogFileGuard log("SIGNS-TISGATEWAY-STISServerTest-test_M23.log");

    auto dest = make_Destination();
    EDisplayTemplateType type = EDisplayTemplateType::LCDNormal;

    try
    {
        gateway.submit_M23_RemoveDisplayTemplateRequest(dest, type);
    }
    catch (std::exception& e)
    {
        LOG_ERROR("%s", e);
        BOOST_TEST(false);
    }
}

BOOST_AUTO_TEST_CASE(test_M30)
{
    LogFileGuard log("SIGNS-TISGATEWAY-STISServerTest-test_M30.log");

    std::string station = "STN";

    try
    {
        auto a30 = gateway.submit_M30_StationSTISStatusRequest(station);
    }
    catch (std::exception& e)
    {
        LOG_ERROR("%s", e);
        BOOST_TEST(false);
    }

    auto dump_pids = [](auto & pids) { return stdex::join_transformed("\n", pids, [](auto & pair) { return str(boost::format("%s: %d") % pair.first % static_cast<int>(pair.second)); }); };

    try
    {
        simulator.set_connection_link_status(static_cast<int>(EConnectionLinkStatus::Alarm));
        simulator.set_current_message_library_version("002");
        auto a30 = gateway.submit_M30_StationSTISStatusRequest(station);
        BOOST_TEST((a30.connection_link_status == EConnectionLinkStatus::Alarm));
        BOOST_TEST((a30.versions.current_message_library_version == "002"));
        LOG_INFO("test_M30(): pid_status_list:\n%s", dump_pids(a30.pid_status_list));

        simulator.set_pid_status("201", static_cast<int>(EPIDStatus::On));
        a30 = gateway.submit_M30_StationSTISStatusRequest(station);
        auto it = boost::find_if(a30.pid_status_list, [&](auto & pidstatus) { return pidstatus.first == "201"; });
        BOOST_TEST((it != a30.pid_status_list.end() && it->second == EPIDStatus::On));
        LOG_INFO("test_M30(): pid_status_list:\n%s", dump_pids(a30.pid_status_list));
    }
    catch (std::exception& e)
    {
        LOG_ERROR("%s", e);
        BOOST_TEST(false);
    }
}

BOOST_AUTO_TEST_CASE(test_M31)
{
    LogFileGuard log("SIGNS-TISGATEWAY-STISServerTest-test_M31.log");

    std::string destination_occ = "OCC";
    auto iscs_versions = make_CurNxtMsgTmpLibVers();

    try
    {
        auto a31 = gateway.submit_M31_OCCSTISStatusSyncRequest(destination_occ, iscs_versions);
    }
    catch (std::exception& e)
    {
        LOG_ERROR("%s", e);
        BOOST_TEST(false);
    }

    try
    {
        simulator.set_connection_link_status(static_cast<int>(EConnectionLinkStatus::Alarm));
        simulator.set_occ_server_status(static_cast<int>(ESTISOCCServerStatus::Alarm));
        simulator.set_current_message_library_version("002");
        auto a31 = gateway.submit_M31_OCCSTISStatusSyncRequest(destination_occ, iscs_versions);
        BOOST_TEST((a31.connection_link_status == EConnectionLinkStatus::Alarm));
        BOOST_TEST((a31.stis_occ_server_status == ESTISOCCServerStatus::Alarm));
        BOOST_TEST((a31.versions.current_message_library_version == "002"));
    }
    catch (std::exception& e)
    {
        LOG_ERROR("%s", e);
        BOOST_TEST(false);
    }
}

BOOST_AUTO_TEST_CASE(test_M32)
{
    LogFileGuard log("SIGNS-TISGATEWAY-STISServerTest-test_M32.log");

    std::string station = "OCC";

    try
    {
        auto a32 = gateway.submit_M32_AllStationSTISStatusRequest(station);
    }
    catch (std::exception& e)
    {
        LOG_ERROR("%s", e);
        BOOST_TEST(false);
    }
}

BOOST_AUTO_TEST_CASE(test_M50)
{
    LogFileGuard log("SIGNS-TISGATEWAY-STISServerTest-test_M50.log");

    std::string station = "OCC";
    std::string pid = "101";

    try
    {
        auto a50 = gateway.submit_M50_CurrentDisplayMessageTemplateRequest(station, pid);
        BOOST_TEST((a50.report_pid == "101"));
    }
    catch (std::exception& e)
    {
        LOG_ERROR("%s", e);
        BOOST_TEST(false);
    }
}

BOOST_AUTO_TEST_CASE(test_M70)
{
    LogFileGuard log("SIGNS-TISGATEWAY-STISServerTest-test_M70.log");

    ELibraryType type = ELibraryType::PredefinedMessage;
    std::string version = "002";

    try
    {
        auto a70 = gateway.submit_M70_UpgradePredefinedMessageDisplayTemplateLibraryRequest(type, version);
        BOOST_TEST((a70.type == ELibraryType::PredefinedMessage));
        BOOST_TEST((a70.version == "002"));
    }
    catch (std::exception& e)
    {
        LOG_ERROR("%s", e);
        BOOST_TEST(false);
    }
}

BOOST_AUTO_TEST_CASE(test_A99)
{
    LogFileGuard log("SIGNS-TISGATEWAY-STISServerTest-test_A99.log");
    simulator.response_nack(true, 1);
    BOOST_CHECK_THROW(gateway.submit_M30_StationSTISStatusRequest("OCC"), std::exception);
    simulator.response_nack(false, 0);
    BOOST_CHECK_NO_THROW(gateway.submit_M30_StationSTISStatusRequest("OCC"));
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
