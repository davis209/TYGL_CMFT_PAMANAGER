// test.cpp : Defines the entry point for the console application.
//

#include "pch.h"
#include "core/utility/tests/boost_test/test_utility/FileGuard.h"
#include "core/utility/tests/boost_test/test_utility/LogFileGuard.h"
#include "core/utility/tests/boost_test/test_utility/DebugLevelGuard.h"
#include "core/utility/tests/boost_test/test_utility/CorbaUtilGuard.h"
#include "core/utility/tests/boost_test/test_utility/TestCaseProcess.h"
#include "app/signs/common_library/tests/boost_test/src/STISTestConfigurations.h"
#include "app/signs/common_library/src/stis_protocol/STISClient.h"
#include "app/signs/common_library/src/stis_protocol/simulator/STISSimulator.h"
#include "core/utility/src/base_ex/DebugUtilEx.h"
#include "core/utility/src/core/StdEx.h"
#include "core/utility/src/core/SimpleConditionVariable.h"
#include "core/naming/src/Naming.h"
#include <boost/test/unit_test.hpp>

using namespace TA_IRS_App;
using namespace TA_Base_Core;
using namespace TA_IRS_App::STIS_PROTOCOL;
using namespace TA_IRS_App::STIS_PROTOCOL::TEST;
using namespace TA_IRS_App::STIS_PROTOCOL::SIMULATOR;
using namespace boost::unit_test;
using namespace std::literals;
using namespace std::chrono;
using ta_utility::core::SimpleConditionVariable;

namespace
{
    using TA_IRS_App::STIS_PROTOCOL::MESSAGE_TYPES::PredefinedMessage;

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

BOOST_AUTO_TEST_SUITE(SIGNS)
BOOST_AUTO_TEST_SUITE(COMMON)
BOOST_AUTO_TEST_SUITE(PROTOCOL)
BOOST_FIXTURE_TEST_SUITE(STISClientTest, STIS_PROTOCOL::TEST::Fixture)

BOOST_AUTO_TEST_CASE(test_occ)
{
    LogFileGuard log("SIGNS-COMMON-PROTOCOL-STISClientTest-test_occ.log");

    auto corba = std::make_shared<CorbaUtilGuard>("--location-key=1 --entity-name=OccSTISManager --port=0");
    Naming::getInstance().init();

    STISSimulator occ_stis_simulator{"--name=occ"}; // default versions: { 001, 001, 001, 001 }
    occ_stis_simulator.start();
    STISSimulator dbg_stis_simulator{"--name=dbg"}; // default versions: { 001, 001, 001, 001 }
    dbg_stis_simulator.start();

    stdex::sleep_for(1s);

    TestCaseProcess occ_tis_gateway_process("TisGatewayTest", "APP/SIGNS/TISGATEWAY/STISGatewayTest/occ_tis_gateway_process", "--process");
    occ_tis_gateway_process.start();
    TestCaseProcess occ_tis_agent_process("TisAgentTest", "APP/SIGNS/TISAGENT/TisAgentTest/occ_tis_agent_process", "--process");
    occ_tis_agent_process.start();

    TestCaseProcess dbg_tis_gateway_process("TisGatewayTest", "APP/SIGNS/TISGATEWAY/STISGatewayTest/dbg_tis_gateway_process", "--process");
    dbg_tis_gateway_process.start();
    TestCaseProcess dbg_tis_agent_process("TisAgentTest", "APP/SIGNS/TISAGENT/TisAgentTest/dbg_tis_agent_process", "--process");
    dbg_tis_agent_process.start();

    stdex::sleep_for(3s);

    STISClient client{"--sync-all --sync-interval-ms=100 --server=occ-tis-agent --db-filename=" + s_client_db_filename};
    client.start();

    BOOST_SCOPE_EXIT_ALL(&)
    {
        client.stop();
        occ_tis_agent_process.kill();
        occ_tis_gateway_process.kill();
        dbg_tis_agent_process.kill();
        dbg_tis_gateway_process.kill();
        occ_stis_simulator.stop();
        dbg_stis_simulator.stop();
        stdex::sleep_for(2s);
    };

    {
        BOOST_TEST((client.current_message_library_version() == "001"));
        BOOST_TEST((client.has_message_library("001")));

        auto versions = client.get_library_versions();
        BOOST_TEST((versions.iscs.current_message_library_version == "001"));
        BOOST_TEST((versions.stis.current_message_library_version == "001"));

        auto msgxml = client.load_current_message_library_xml();
        BOOST_TEST((msgxml->PredefMsgLib.Version == "001"));
    }

    occ_stis_simulator.set_current_message_library_version("002");
    dbg_stis_simulator.set_current_message_library_version("002");
    stdex::sleep_for(3s);

    {
        BOOST_TEST((client.has_message_library("002")));

        auto versions = client.get_all_library_versions();
        BOOST_TEST((versions.size() == 2));
        BOOST_TEST((versions[0].location_name == "OCC"));
        BOOST_TEST((versions[0].versions.iscs.current_message_library_version == "001"));
        BOOST_TEST((versions[0].versions.stis.current_message_library_version == "002"));
        BOOST_TEST((versions[1].location_name == "DBG"));
        BOOST_TEST((versions[1].versions.iscs.current_message_library_version == "001"));
        BOOST_TEST((versions[1].versions.stis.current_message_library_version == "002"));
    }

    client.upgrade_message_library_version("002");
    dbg_stis_simulator.set_current_message_library_version("002");
    stdex::sleep_for(3s);

    {
        BOOST_TEST((client.current_message_library_version() == "002"));

        auto versions = client.get_all_library_versions();
        BOOST_TEST((versions.size() == 2));
        BOOST_TEST((versions[0].location_name == "OCC"));
        BOOST_TEST((versions[0].versions.iscs.current_message_library_version == "002"));
        BOOST_TEST((versions[0].versions.stis.current_message_library_version == "002"));
        BOOST_TEST((versions[1].location_name == "DBG"));
        BOOST_TEST((versions[1].versions.iscs.current_message_library_version == "002"));
        BOOST_TEST((versions[1].versions.stis.current_message_library_version == "002"));

        auto msgxml = client.load_current_message_library_xml();
        BOOST_TEST((msgxml->PredefMsgLib.Version == "002"));
    }

    try
    {
        auto dest = make_Destination();

        {
            auto msg = make_PredefinedMessage();
            client.submit_M10_DisplayPredefinedMessageRequest(dest, msg);
        }

        {
            auto msg = make_AdHodMessage();
            client.submit_M11_DisplayAdHocMessageRequest(dest, msg);
        }

        {
            std::vector<int> priority = {0, 0, 0, 0, 0, 0, 0, 0};
            client.submit_M20_ClearCurrentMessageRequest(dest, priority);
        }

        {
            EPIDControlOn control_on = EPIDControlOn::AllOff;
            EPIDControlOff control_off = EPIDControlOff::Reserved;
            client.submit_M21_PIDOnOffControlRequest(dest, control_on, control_off);
        }

        {
            auto lcd = make_PredefinedDisplayTemplate(EDisplayTemplateType::LCDNormal);
            auto led = make_PredefinedDisplayTemplate(EDisplayTemplateType::Spare);
            client.submit_M22_SendPredefinedDisplayTemplateRequest(dest, lcd, led);
        }

        {
            auto type = EDisplayTemplateType::LCDNormal;
            client.submit_M23_RemoveDisplayTemplateRequest(dest, type);
        }

        {
            std::string station = "OCC";
            auto a30 = client.submit_M30_StationSTISStatusRequest(station);
            BOOST_TEST((a30.connection_link_status == EConnectionLinkStatus::Normal));

            occ_stis_simulator.set_connection_link_status(static_cast<int>(EConnectionLinkStatus::Alarm));
            BOOST_SCOPE_EXIT_ALL(&) { occ_stis_simulator.set_connection_link_status(static_cast<int>(EConnectionLinkStatus::Normal)); };
            a30 = client.submit_M30_StationSTISStatusRequest(station);
            BOOST_TEST((a30.connection_link_status == EConnectionLinkStatus::Alarm));
        }

        {
            std::string destination_occ = "OCC";
            auto iscs_versions = make_CurNxtMsgTmpLibVers();
            auto a31 = client.submit_M31_OCCSTISStatusSyncRequest(destination_occ, iscs_versions);
            BOOST_TEST((a31.connection_link_status == EConnectionLinkStatus::Normal));
            BOOST_TEST((a31.stis_occ_server_status == ESTISOCCServerStatus::Normal));

            occ_stis_simulator.set_connection_link_status(static_cast<int>(EConnectionLinkStatus::Alarm));
            occ_stis_simulator.set_occ_server_status(static_cast<int>(ESTISOCCServerStatus::Alarm));
            BOOST_SCOPE_EXIT_ALL(&)
            {
                occ_stis_simulator.set_connection_link_status(static_cast<int>(EConnectionLinkStatus::Normal));
                occ_stis_simulator.set_occ_server_status(static_cast<int>(ESTISOCCServerStatus::Normal));
            };
            a31 = client.submit_M31_OCCSTISStatusSyncRequest(destination_occ, iscs_versions);
            BOOST_TEST((a31.connection_link_status == EConnectionLinkStatus::Alarm));
            BOOST_TEST((a31.stis_occ_server_status == ESTISOCCServerStatus::Alarm));
        }

        {
            std::string station = "OCC";
            client.submit_M32_AllStationSTISStatusRequest(station);
        }

        {
            std::string station = "OCC";
            std::string pid = "101";
            auto m50 = client.submit_M50_CurrentDisplayMessageTemplateRequest(station, pid);
            BOOST_TEST((m50.report_pid == "101"));
        }

        {
            ELibraryType type = ELibraryType::PredefinedMessage;
            std::string version = "002";
            auto a70 = client.submit_M70_UpgradePredefinedMessageDisplayTemplateLibraryRequest(type, version);
            BOOST_TEST((a70.tied() == std::make_tuple(ELibraryType::PredefinedMessage, "002"s)));
        }
    }
    catch (std::exception& e)
    {
        LOG_ERROR("%s", e);
        BOOST_TEST(false);
    }
}

BOOST_AUTO_TEST_CASE(test_occ_external_simulator)
{
    LogFileGuard log("SIGNS-COMMON-PROTOCOL-STISClientTest-test_occ.log");

    auto corba = std::make_shared<CorbaUtilGuard>("--location-key=1 --entity-name=OccSTISManager --port=0");
    Naming::getInstance().init();

    STISSimulator simulator{"--remote --name=occ"};

    if (!simulator.is_online())
    {
        return;
    }

    TestCaseProcess occ_tis_gateway_process("TisGatewayTest", "APP/SIGNS/TISGATEWAY/STISGatewayTest/occ_tis_gateway_process", "--process");
    occ_tis_gateway_process.start();
    TestCaseProcess occ_tis_agent_process("TisAgentTest", "APP/SIGNS/TISAGENT/TisAgentTest/occ_tis_agent_process", "--process");
    occ_tis_agent_process.start();

    stdex::sleep_for(3s);

    STISClient client{"--sync-all --sync-interval-ms=100 --server=occ-tis-agent --db-filename=" + s_client_db_filename};
    client.start();

    BOOST_SCOPE_EXIT_ALL(&)
    {
        simulator.reset_all_to_default(); // rollback
        client.stop();
        occ_tis_agent_process.kill();
        occ_tis_gateway_process.kill();
        stdex::sleep_for(2s);
    };

    {
        BOOST_TEST((client.current_message_library_version() == "001"));
        BOOST_TEST((client.has_message_library("001")));

        auto versions = client.get_library_versions();
        BOOST_TEST((versions.iscs.current_message_library_version == "001"));
        BOOST_TEST((versions.stis.current_message_library_version == "001"));

        auto msgxml = client.load_current_message_library_xml();
        BOOST_TEST((msgxml && (msgxml->PredefMsgLib.Version == "001")));
    }

    try
    {
        auto dest = make_Destination();

        {
            auto msg = make_PredefinedMessage();
            client.submit_M10_DisplayPredefinedMessageRequest(dest, msg);
        }

        {
            auto msg = make_AdHodMessage();
            client.submit_M11_DisplayAdHocMessageRequest(dest, msg);
        }

        {
            std::vector<int> priority = {0, 0, 0, 0, 0, 0, 0, 0};
            client.submit_M20_ClearCurrentMessageRequest(dest, priority);
        }

        {
            EPIDControlOn control_on = EPIDControlOn::NoAction;
            EPIDControlOff control_off = EPIDControlOff::NoAction;
            client.submit_M21_PIDOnOffControlRequest(dest, control_on, control_off);
        }

        {
            auto lcd = make_PredefinedDisplayTemplate(EDisplayTemplateType::LCDNormal);
            auto led = make_PredefinedDisplayTemplate(EDisplayTemplateType::Spare);
            client.submit_M22_SendPredefinedDisplayTemplateRequest(dest, lcd, led);
        }

        {
            auto type = EDisplayTemplateType::LCDNormal;
            client.submit_M23_RemoveDisplayTemplateRequest(dest, type);
        }

        {
            std::string station = "OCC";
            auto a30 = client.submit_M30_StationSTISStatusRequest(station);
            BOOST_TEST((a30.connection_link_status == EConnectionLinkStatus::Normal));
        }

        {
            std::string destination_occ = "OCC";
            auto iscs_versions = make_CurNxtMsgTmpLibVers();
            auto a31 = client.submit_M31_OCCSTISStatusSyncRequest(destination_occ, iscs_versions);
            BOOST_TEST((a31.connection_link_status == EConnectionLinkStatus::Normal));
            BOOST_TEST((a31.stis_occ_server_status == ESTISOCCServerStatus::Normal));
        }

        {
            std::string station = "OCC";
            client.submit_M32_AllStationSTISStatusRequest(station);
        }

        {
            std::string station = "OCC";
            std::string pid = "101";
            auto m50 = client.submit_M50_CurrentDisplayMessageTemplateRequest(station, pid);
            BOOST_TEST((m50.report_pid == "101"));
        }

        {
            ELibraryType type = ELibraryType::PredefinedMessage;
            std::string version = "002";
            auto a70 = client.submit_M70_UpgradePredefinedMessageDisplayTemplateLibraryRequest(type, version);
            BOOST_TEST((a70.tied() == std::make_tuple(ELibraryType::PredefinedMessage, "002"s)));
        }
    }
    catch (std::exception& e)
    {
        LOG_ERROR("%s", e);
        BOOST_TEST(false);
    }
}

#if 0
BOOST_AUTO_TEST_CASE(test_occ_external_simulator_interactive)
{
    LogFileGuard log("SIGNS-COMMON-PROTOCOL-STISClientTest-test_occ_external_simulator_interactive.log");

    auto corba = std::make_shared<CorbaUtilGuard>("--location-key=1 --entity-name=OccSTISManager --port=0");
    Naming::getInstance().init();

    TestCaseProcess occ_tis_gateway_process("TisGatewayTest", "APP/SIGNS/TISGATEWAY/STISGatewayTest/occ_tis_gateway_process", "--process");
    occ_tis_gateway_process.start();
    TestCaseProcess occ_tis_agent_process("TisAgentTest", "APP/SIGNS/TISAGENT/TisAgentTest/occ_tis_agent_process", "--process");
    occ_tis_agent_process.start();

    stdex::sleep_for(3s);

    STISClient client{"--sync-all --sync-interval-ms=100 --server=occ-tis-agent --db-filename=" + s_client_db_filename};
    client.start();

    BOOST_SCOPE_EXIT_ALL(&)
    {
        client.stop();
        occ_tis_agent_process.kill();
        occ_tis_gateway_process.kill();
        stdex::sleep_for(2s);
    };

    try
    {
        for (std::string line; std::getline(std::cin, line);)
        {
            boost::trim(line);

            if (stdex::icontains_any(line, {"quit", "exit"}) || stdex::any_of_iequal({"q"}, line))
            {
                break;
            }

            if (line.empty())
            {
                auto versions = client.get_library_versions();
                std::cout << boost::format("                                  ISCS STIS") << std::endl;
                std::cout << boost::format("current message library version:  %s, %s") %  versions.iscs.current_message_library_version  %  versions.stis.current_message_library_version  << std::endl;
                std::cout << boost::format("next message library version:     %s, %s") %  versions.iscs.next_message_library_version     %  versions.stis.next_message_library_version     << std::endl;
                std::cout << boost::format("current template library version: %s, %s") %  versions.iscs.current_template_library_version %  versions.stis.current_template_library_version << std::endl;
                std::cout << boost::format("next template library version:    %s, %s") %  versions.iscs.next_template_library_version    %  versions.stis.next_template_library_version    << std::endl;
            }
        }

        BOOST_TEST(true);
    }
    catch (std::exception& e)
    {
        LOG_ERROR("%s", e);
        BOOST_TEST(false);
    }
}
#endif

namespace test_occ_observer_namespace
{
    struct Observer : ISTISLibraryObserver
    {
        virtual void on_any_change() override
        {
            m_cv = true;
        }

        template <class... Args>
        auto wait_for(Args&& ... args)
        {
            auto res = m_cv.wait_for(std::forward<Args>(args)...);
            m_cv = false;
            return res;
        }

        SimpleConditionVariable m_cv;
    };
}
BOOST_AUTO_TEST_CASE(test_occ_observer)
{
    LogFileGuard log("SIGNS-COMMON-PROTOCOL-STISClientTest-test_occ.log");
    using namespace test_occ_observer_namespace;

    auto corba = std::make_shared<CorbaUtilGuard>("--location-key=1 --entity-name=OccSTISManager --port=0");
    Naming::getInstance().init();

    STISSimulator occ_stis_simulator{"--name=occ"}; // default versions: { 001, 001, 001, 001 }
    occ_stis_simulator.start();
    STISSimulator dbg_stis_simulator{"--name=dbg --sync-with-occ-simulator=false"}; // default versions: { 001, 001, 001, 001 }
    dbg_stis_simulator.start();

    TestCaseProcess occ_tis_gateway_process("TisGatewayTest", "APP/SIGNS/TISGATEWAY/STISGatewayTest/occ_tis_gateway_process", "--process");
    occ_tis_gateway_process.start();
    TestCaseProcess occ_tis_agent_process("TisAgentTest", "APP/SIGNS/TISAGENT/TisAgentTest/occ_tis_agent_process", "--process");
    occ_tis_agent_process.start();

    TestCaseProcess dbg_tis_gateway_process("TisGatewayTest", "APP/SIGNS/TISGATEWAY/STISGatewayTest/dbg_tis_gateway_process", "--process");
    dbg_tis_gateway_process.start();
    TestCaseProcess dbg_tis_agent_process("TisAgentTest", "APP/SIGNS/TISAGENT/TisAgentTest/dbg_tis_agent_process", "--process");
    dbg_tis_agent_process.start();

    auto observer = std::make_shared<Observer>();

    STISClient client{"--sync-all --sync-interval-ms=100 --server=occ-tis-agent --db-filename=" + s_client_db_filename};
    client.set_observer(observer);
    client.start();

    BOOST_SCOPE_EXIT_ALL(&)
    {
        client.stop();
        occ_tis_agent_process.kill();
        occ_tis_gateway_process.kill();
        dbg_tis_agent_process.kill();
        dbg_tis_gateway_process.kill();
        occ_stis_simulator.stop();
        dbg_stis_simulator.stop();
        stdex::sleep_for(2s);
    };

    BOOST_TEST((observer->wait_for(5s, [&]
    {
        auto b1 = (client.current_message_library_version() == "001");
        auto b2 = client.has_message_library("001");

        auto versions = client.get_library_versions();
        auto b3 = ((versions.iscs.current_message_library_version == "001"));
        auto b4 = ((versions.stis.current_message_library_version == "001"));
        return b1 && b2 && b3 && b4;
    })));

    occ_stis_simulator.set_current_message_library_version("002");
    dbg_stis_simulator.set_current_message_library_version("002");
    BOOST_TEST((observer->wait_for(5s, [&]
    {
        auto b1 = (client.current_message_library_version() == "001");
        auto b2 = client.has_message_library("002");

        auto versions = client.get_all_library_versions();
        auto b3 = ((versions.size() == 2));
        auto b4 = ((versions[0].location_name == "OCC"));
        auto b5 = ((versions[0].versions.iscs.current_message_library_version == "001"));
        auto b6 = ((versions[0].versions.stis.current_message_library_version == "002"));
        auto b7 = ((versions[1].location_name == "DBG"));
        auto b8 = ((versions[1].versions.iscs.current_message_library_version == "001"));
        auto b9 = ((versions[1].versions.stis.current_message_library_version == "002"));
        return b1 && b2 && b3 && b4 && b5 && b6 && b7 && b8 && b9;
    })));

    client.upgrade_message_library_version("002");
    dbg_stis_simulator.set_current_message_library_version("002");

    BOOST_TEST((observer->wait_for(5s, [&]
    {
        auto versions = client.get_all_library_versions();
        auto b1 = ((versions.size() == 2));
        auto b2 = ((versions[0].location_name == "OCC"));
        auto b3 = ((versions[0].versions.iscs.current_message_library_version == "002"));
        auto b4 = ((versions[0].versions.stis.current_message_library_version == "002"));
        auto b5 = ((versions[1].location_name == "DBG"));
        auto b6 = ((versions[1].versions.iscs.current_message_library_version == "002"));
        auto b7 = ((versions[1].versions.stis.current_message_library_version == "002"));
        return b1 && b2 && b3 && b4 && b5 && b6 && b7;
    })));
}

BOOST_AUTO_TEST_CASE(test_dbg)
{
    LogFileGuard log("SIGNS-COMMON-PROTOCOL-STISClientTest-test_dbg.log");

    auto corba = std::make_shared<CorbaUtilGuard>("--location-key=3 --entity-name=DbgSTISManager --port=0");
    Naming::getInstance().init();

    STISSimulator dbg_stis_simulator{"--name=dbg"}; // default versions: { 001, 001, 001, 001 }
    dbg_stis_simulator.start();
    stdex::sleep_for(1s);

    TestCaseProcess dbg_tis_gateway_process("TisGatewayTest", "APP/SIGNS/TISGATEWAY/STISGatewayTest/dbg_tis_gateway_process", "--process");
    dbg_tis_gateway_process.start();
    stdex::sleep_for(1s);

    TestCaseProcess dbg_tis_agent_process("TisAgentTest", "APP/SIGNS/TISAGENT/TisAgentTest/dbg_tis_agent_process", "--process");
    dbg_tis_agent_process.start();
    stdex::sleep_for(3s);

    STISClient client{"--sync-all --sync-interval-ms=100 --server=local-tis-agent --db-filename=" + s_client_db_filename};
    client.start();

    BOOST_SCOPE_EXIT_ALL(&)
    {
        client.stop();
        dbg_tis_agent_process.kill();
        dbg_tis_gateway_process.kill();
        dbg_stis_simulator.stop();
        stdex::sleep_for(2s);
    };

    auto versions = client.get_library_versions();
    BOOST_TEST((versions.stis.current_message_library_version == "001"));

    dbg_stis_simulator.set_current_message_library_version("002");
    stdex::sleep_for(2s);

    {
        auto versions = client.get_library_versions();
        BOOST_TEST((versions.stis.current_message_library_version == "002"));
    }
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
