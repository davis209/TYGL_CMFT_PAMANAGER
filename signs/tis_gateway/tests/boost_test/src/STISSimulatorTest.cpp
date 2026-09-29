// test.cpp : Defines the entry point for the console application.
//

#include "pch.h"
#include "core/utility/tests/boost_test/test_utility/LogFileGuard.h"
#include "core/utility/tests/boost_test/test_utility/DebugLevelGuard.h"
#include "core/utility/tests/boost_test/test_utility/CorbaUtilGuard.h"
#include "app/signs/common_library/tests/boost_test/src/STISTestConfigurations.h"
#include "app/signs/common_library/src/stis_protocol/simulator/STISSimulator.h"
#include "app/signs/tis_gateway/src/stis_protocol/STISServer.h"
#include "core/utility/src/core/StdEx.h"
#include "core/utility/src/base_ex/DebugUtilEx.h"
#include "core/naming/src/Naming.h"
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

    struct Fixture : STIS_PROTOCOL::TEST::Fixture
    {
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
BOOST_FIXTURE_TEST_SUITE(STISSimulatorTest, Fixture)

BOOST_AUTO_TEST_CASE(test)
{
    LogFileGuard log("SIGNS-TISGATEWAY-STISSimulatorTest-test.log");

    CorbaUtilGuard corba;
    Naming::getInstance().init();

    STISSimulator simulator{"--remote"}; // external simulator already running

    if (!simulator.is_online())
    {
        return;
    }

    simulator.exec_system_cmd("TITLE hello simulator, this is tis-gateway");

    STISServer gateway;
    gateway.start();
    stdex::sleep_for(1s);

    try
    {
        auto dest = make_Destination();

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

        {
            ELibraryType type = ELibraryType::PredefinedMessage;
            std::string version = "001";
            gateway.submit_M70_UpgradePredefinedMessageDisplayTemplateLibraryRequest(type, version);
        }
    }
    catch (std::exception& e)
    {
        LOG_ERROR("%s", e);
        BOOST_TEST(false);
    }
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
