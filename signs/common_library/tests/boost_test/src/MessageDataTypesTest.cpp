// test.cpp : Defines the entry point for the console application.
//

#include "pch.h"
#include "app/signs/common_library/src/stis_protocol/message_types/MessageDataTypes.h"
#include "core/utility/src/base_ex/DebugUtilEx.h"
#include "core/utility/src/core/StdEx.h"
#include <boost/test/unit_test.hpp>

// using namespace ta_utility::core;
using namespace TA_IRS_App::STIS_PROTOCOL;
using namespace TA_IRS_App::STIS_PROTOCOL::MESSAGE_TYPES;
using namespace TA_IRS_App::STIS_PROTOCOL::MESSAGE_DATA_TYPES;
using namespace std::string_literals;
using namespace std::rel_ops;
using boost::filesystem::path;
namespace stdex = ta_utility::core::stdex;

namespace
{
    auto make_MSGDEST()
    {
        MSGDEST m;
        m.SystemID = "TEL"; // Thomson East-coast Line (TEL)
        m.StationID = "OCC"; // TE01 ASTN
        m.PIDList.emplace_back("101");
        return m;
    }

    auto make_Destination()
    {
        Destination dest;
        dest.system_id = "TEL";
        dest.station_id = "OCC";
        dest.pid_list.emplace_back("101");
        return dest;
    }

    auto make_DISPLAYEFFECT()
    {
        DISPLAYEFFECT m;
        m.DisplayMode = EDisplayMode::ScrollLeft;
        m.Speed = ESpeed::Medium;
        m.RepeatInterval = 1;
        m.DisplayTime = 1;
        m.Alignment = EAlignment::Centered;
        m.FontSize = 1;
        m.FontFamily = 1;
        m.FontColor = 1;
        m.BackgroundColor = 1;
        return m;
    }

    auto make_DisplayEffect()
    {
        DisplayEffect x;
        x.display_mode = EDisplayMode::ScrollLeft;
        x.speed = ESpeed::Medium;
        x.repeat_interval = 1;
        x.display_time = 1;
        x.alignment = EAlignment::Centered;
        x.font_size = 1;
        x.font_family = 1;
        x.font_color = 1;
        x.background_color = 1;
        return x;
    }

    auto make_MSGPRE()
    {
        MSGPRE m;
        m.MessageTag = "[-MSG--TAG-]";
        m.StartTime = stdex::get_time_YYYYMMDDHHMMSS();
        m.EndTime = stdex::get_time_YYYYMMDDHHMMSS();
        m.Priority = 1;
        m.Spare = 1;
        m.LCDEmergencyDisplayTemplateType = ELCDEmergencyDisplayTemplateType::Default;
        m.LCDEmergencyDisplayTemplateID = "001";
        m.LEDEmergencyDisplayTemplateType = ELEDEmergencyDisplayTemplateType::Default;
        m.LEDEmergencyDisplayTemplateID = "001";
        return m;
    }

    auto make_PredefinedMessage()
    {
        PredefinedMessage msg;
        msg.message_tag = "[-MSG--TAG-]";
        msg.start_time = stdex::get_time_YYYYMMDDHHMMSS();
        msg.end_time = stdex::get_time_YYYYMMDDHHMMSS();
        msg.priority = 1;
        msg.spare = 1;
        msg.lcd_emergency_display_template_type = ELCDEmergencyDisplayTemplateType::Default;
        msg.lcd_emergency_display_template_id = "001";
        msg.led_emergency_display_template_type = ELEDEmergencyDisplayTemplateType::Default;
        msg.led_emergency_display_template_id = "001";
        return msg;
    }

    auto make_MSGFREE()
    {
        MSGFREE m;
        m.MessageTag = "[-MSG--TAG-]";
        m.StartTime = stdex::get_time_YYYYMMDDHHMMSS();
        m.EndTime = stdex::get_time_YYYYMMDDHHMMSS();
        m.Priority = 1;
        m.LCDEmergencyDisplayTemplateType = ELCDEmergencyDisplayTemplateType::Default;
        m.LCDEmergencyDisplayTemplateID = "001";
        m.LEDEmergencyDisplayTemplateType = ELEDEmergencyDisplayTemplateType::Default;
        m.LEDEmergencyDisplayTemplateID = "001";
        m.MessageText = "hello, world";
        return m;
    }

    auto make_AdHodMessage()
    {
        AdHodMessage msg;
        msg.message_tag = "[-MSG--TAG-]";
        msg.start_time = stdex::get_time_YYYYMMDDHHMMSS();
        msg.end_time = stdex::get_time_YYYYMMDDHHMMSS();
        msg.priority = 1;
        msg.lcd_emergency_display_template_type = ELCDEmergencyDisplayTemplateType::Default;
        msg.lcd_emergency_display_template_id = "001";
        msg.led_emergency_display_template_type = ELEDEmergencyDisplayTemplateType::Default;
        msg.led_emergency_display_template_id = "001";
        msg.message_text = "hello, world";
        return msg;
    }

    auto make_TLPRE(EDisplayTemplateType type = EDisplayTemplateType::Default)
    {
        TLPRE m;
        m.DisplayTemplateType = type;
        m.DisplayTemplateID = "001";
        m.StartTime = stdex::get_time_YYYYMMDDHHMMSS();
        m.EndTime = stdex::get_time_YYYYMMDDHHMMSS();
        return m;
    }

    auto make_PredefinedDisplayTemplate(EDisplayTemplateType type = EDisplayTemplateType::Default)
    {
        PredefinedDisplayTemplate msg;
        msg.display_template_type = type;
        msg.display_template_id = "001";
        msg.start_time = stdex::get_time_YYYYMMDDHHMMSS();
        msg.end_time = stdex::get_time_YYYYMMDDHHMMSS();
        return msg;
    }

    auto make_STATIONSTATUSDETAIL()
    {
        STATIONSTATUSDETAIL m;
        m.ReportStation = "OCC";
        m.ConnectionLinkStatus = EConnectionLinkStatus::Normal;
        m.versions_tied() = std::make_tuple("001", "001", "001", "001");
        // m.NumberOfPIDs = 1;
        m.PIDStatusList.emplace_back("001", EPIDStatus::Off);
        return m;
    }

    auto make_A30_StationSTISStatusReport()
    {
        A30_StationSTISStatusReport a;
        a.report_station = "OCC";
        a.connection_link_status = EConnectionLinkStatus::Normal;
        a.versions.tied() = std::make_tuple("001", "001", "001", "001");
        a.pid_status_list.emplace_back("001", EPIDStatus::Off);
        return a;
    }
}

BOOST_AUTO_TEST_SUITE(SIGNS)
BOOST_AUTO_TEST_SUITE(COMMON)
BOOST_AUTO_TEST_SUITE(PROTOCOL)
BOOST_AUTO_TEST_SUITE(MessageDataTypesTest)

BOOST_AUTO_TEST_CASE(test_MSGDEST)
{
    MSGDEST m = make_MSGDEST();
    LOG_INFO("%s", m.dump());

    std::stringstream ss;
    ss << m;
    LOG_INFO("%s", ss.str());

    MSGDEST m2;
    ss >> m2;
    LOG_INFO("%s", m2.dump());
    BOOST_TEST((m2 == m));

    m2.SystemID = "42";
    BOOST_TEST((m != m2));

    {
        Destination dest = make_Destination();
        MSGDEST m3(dest);
        BOOST_TEST((m3 == m));

        MSGDEST m4;
        m4 = dest;
        BOOST_TEST((m4 == m));
    }
}

BOOST_AUTO_TEST_CASE(test_DISPLAYEFFECT)
{
    DISPLAYEFFECT m = make_DISPLAYEFFECT();
    LOG_INFO("%s", m.dump());

    std::stringstream ss;
    ss << m;
    LOG_INFO("%s", ss.str());

    DISPLAYEFFECT m2;
    ss >> m2;
    LOG_INFO("%s", m2.dump());
    BOOST_TEST((m == m2));

    m2.DisplayMode = EDisplayMode::Flashing;
    BOOST_TEST((m != m2));

    {
        DisplayEffect rhs = make_DisplayEffect();

        DISPLAYEFFECT m3(rhs);
        BOOST_TEST((m3 == m));

        DISPLAYEFFECT m4;
        m4 = rhs;
        BOOST_TEST((m4 == m));
    }
}

#if 0
BOOST_AUTO_TEST_CASE(test_LEDFONT)
{
    LEDFONT m;
    m.FontSize = 1;
    m.FontColor = 1;
    LOG_INFO("%s", m.dump());

    std::stringstream ss;
    ss << m;
    LOG_INFO("%s", ss.str());

    LEDFONT m2;
    ss >> m2;
    LOG_INFO("%s", m2.dump());
    BOOST_TEST((m == m2));

    m.FontSize = 2;
    BOOST_TEST((m != m2));
}
#endif

BOOST_AUTO_TEST_CASE(test_MSGPRE)
{
    MSGPRE m = make_MSGPRE();
    LOG_INFO("%s", m.dump());

    std::stringstream ss;
    ss << m;
    LOG_INFO("%s", ss.str());

    MSGPRE m2;
    ss >> m2;
    LOG_INFO("%s", m2.dump());
    BOOST_TEST((m2 == m));

    m2.Priority = 2;
    BOOST_TEST((m2 != m));

    {
        PredefinedMessage msg = make_PredefinedMessage();

        MSGPRE m3(msg);
        BOOST_TEST((m3 == m));

        MSGPRE m4;
        m4 = msg;
        BOOST_TEST((m4 == m));
    }
}

BOOST_AUTO_TEST_CASE(test_MSGFREE)
{
    MSGFREE m = make_MSGFREE();
    LOG_INFO("%s", m.dump());

    std::stringstream ss;
    ss << m;
    LOG_INFO("%s", ss.str());

    MSGFREE m2;
    ss >> m2;
    LOG_INFO("%s", m2.dump());
    BOOST_TEST((m == m2));

    m2.Priority = 2;
    BOOST_TEST((m != m2));

    {
        AdHodMessage rhs = make_AdHodMessage();

        MSGFREE m3(rhs);
        BOOST_TEST((m3 == m));

        MSGFREE m4;
        m4 = rhs;
        BOOST_TEST((m4 == m));
    }
}

BOOST_AUTO_TEST_CASE(test_TLPRE)
{
    TLPRE m = make_TLPRE();
    LOG_INFO("%s", m.dump());

    std::stringstream ss;
    ss << m;
    LOG_INFO("%s", ss.str());

    TLPRE m2;
    ss >> m2;
    LOG_INFO("%s", m2.dump());
    BOOST_TEST((m == m2));

    m2.DisplayTemplateType = EDisplayTemplateType::LCDNormal;
    LOG_INFO("%s", m.dump());
    BOOST_TEST((m != m2));

    {
        PredefinedDisplayTemplate rhs = make_PredefinedDisplayTemplate();

        TLPRE m3(rhs);
        BOOST_TEST((m3 == m));

        TLPRE m4;
        m4 = rhs;
        BOOST_TEST((m4 == m));
    }
}

BOOST_AUTO_TEST_CASE(test_STATIONSTATUSDETAIL)
{
    STATIONSTATUSDETAIL m;
    m.ReportStation = "OCC";
    m.ConnectionLinkStatus = EConnectionLinkStatus::Normal;
    m.versions_tied() = std::make_tuple("001", "001", "001", "001");
    // m.NumberOfPIDs = 1;
    m.PIDStatusList.emplace_back("001", EPIDStatus::Off);
    LOG_INFO("%s", m.dump());

    std::stringstream ss;
    ss << m;
    LOG_INFO("%s", ss.str());

    STATIONSTATUSDETAIL m2;
    ss >> m2;
    LOG_INFO("%s", m2.dump());
    BOOST_TEST((m == m2));

    m2.ConnectionLinkStatus = EConnectionLinkStatus::Alarm;
    BOOST_TEST((m != m2));

    {
        A30_StationSTISStatusReport rhs;
        rhs.report_station = "OCC";
        rhs.connection_link_status = EConnectionLinkStatus::Normal;
        rhs.versions.tied() = std::make_tuple("001", "001", "001", "001");
        rhs.pid_status_list.emplace_back("001", EPIDStatus::Off);

        STATIONSTATUSDETAIL m3(rhs);
        BOOST_TEST((m3 == m));

        STATIONSTATUSDETAIL m4;
        m4 = rhs;
        BOOST_TEST((m4 == m));
    }
}

BOOST_AUTO_TEST_CASE(test_ALLSTATIONSTATUSDETAILS)
{
    ALLSTATIONSTATUSDETAILS a;
    STATIONSTATUSDETAIL m;
    m.ReportStation = "OCC";
    m.ConnectionLinkStatus = EConnectionLinkStatus::Normal;
    m.versions_tied() = std::make_tuple("001", "001", "001", "001");
    // m.NumberOfPIDs = 1;
    m.PIDStatusList.emplace_back("001", EPIDStatus::Off);
    a.emplace_back(std::move(m));
    LOG_INFO("%s", a[0].dump());

    std::stringstream ss;
    ss << a;
    LOG_INFO("%s", ss.str());

    ALLSTATIONSTATUSDETAILS a2;
    ss >> a2;
    LOG_INFO("%s", a2[0].dump());
    BOOST_TEST((a == a2));

    a[0].ConnectionLinkStatus = EConnectionLinkStatus::Alarm;
    BOOST_TEST((a != a2));
}

BOOST_AUTO_TEST_CASE(test_M10)
{
    M10 m;
    m.SQN = 1;
    m.Timestamp = stdex::get_time_YYYYMMDDHHMMSS();
    m.Destination = make_MSGDEST();
    m.Message = make_MSGPRE();
    m.DataLength = m.datasize();

    std::stringstream ss;
    ss << m;
    LOG_INFO("\nM10: %s", ss.str());

    M10 m2;
    ss >> m2;
    LOG_INFO("\nM10: %s", m2.dump());
    BOOST_TEST((m2 == m));

    m2.Message.Priority = 2;
    BOOST_TEST((m2 != m));

    {
        Destination dest = make_Destination();
        PredefinedMessage msg = make_PredefinedMessage();

        M10 m3;
        m3.SQN = 1;
        m3.Timestamp = stdex::get_time_YYYYMMDDHHMMSS();
        m3.set(dest, msg);
        BOOST_TEST((m3 == m));
    }
}

BOOST_AUTO_TEST_CASE(test_A10)
{
    A10 a;
    a.SQN = 1;
    a.Timestamp = stdex::get_time_YYYYMMDDHHMMSS();
    a.ReportStation = "OCC";
    a.DataLength = a.datasize();
    LOG_INFO("%s", a.dump());

    std::stringstream ss;
    ss << a;
    LOG_INFO("\nA10: %s", ss.str());

    A10 a2;
    ss >> a2;
    LOG_INFO("\nA10: %s", a2.dump());
    BOOST_TEST((a2 == a));

    a2.SQN = 2;
    BOOST_TEST((a2 != a));
}

BOOST_AUTO_TEST_CASE(test_M11)
{
    M11 m;
    m.SQN = 1;
    m.Timestamp = stdex::get_time_YYYYMMDDHHMMSS();
    m.Destination = make_MSGDEST();
    m.Message = make_MSGFREE();
    m.DataLength = m.datasize();
    LOG_INFO("%s", m.dump());

    std::stringstream ss;
    ss << m;
    LOG_INFO("\nM11: %s", ss.str());

    M11 m2;
    ss >> m2;
    LOG_INFO("\nM11: %s", m2.dump());
    BOOST_TEST((m == m2));

    m2.SQN = 2;
    BOOST_TEST((m2 != m));

    {
        Destination dest = make_Destination();
        AdHodMessage msg = make_AdHodMessage();

        M11 m3;
        m3.SQN = 1;
        m3.Timestamp = stdex::get_time_YYYYMMDDHHMMSS();
        m3.set(dest, msg);
        BOOST_TEST((m3 == m));
    }
}

BOOST_AUTO_TEST_CASE(test_M20)
{
    M20 m;
    m.SQN = 1;
    m.Timestamp = stdex::get_time_YYYYMMDDHHMMSS();
    m.Destination = make_MSGDEST();
    m.Priority1 = 1;
    m.Priority2 = 2;
    m.Priority3 = 3;
    m.Priority4 = 4;
    m.Priority5 = 5;
    m.Priority6 = 6;
    m.Priority7 = 7;
    m.Priority8 = 8;
    m.DataLength = m.datasize();
    LOG_INFO("%s", m.dump());

    std::stringstream ss;
    ss << m;
    LOG_INFO("\nM20: %s", ss.str());

    M20 m2;
    ss >> m2;
    LOG_INFO("\nM20: %s", m2.dump());
    BOOST_TEST((m == m2));

    {
        Destination dest = make_Destination();
        std::vector<int> priority = {1, 2, 3, 4, 5, 6, 7, 8};

        M20 m3;
        m3.SQN = 1;
        m3.Timestamp = stdex::get_time_YYYYMMDDHHMMSS();
        m3.set(dest, priority);
        BOOST_TEST((m3 == m));
    }
}

BOOST_AUTO_TEST_CASE(test_A20)
{
    A20 a;
    a.SQN = 1;
    a.Timestamp = stdex::get_time_YYYYMMDDHHMMSS();
    a.ReportStation = "OCC";
    a.DataLength = a.datasize();

    std::stringstream ss;
    ss << a;
    LOG_INFO("\nA20: %s", ss.str());

    A20 a2;
    ss >> a2;
    LOG_INFO("\nA20: %s", a2.dump());
    BOOST_TEST((a == a2));
}

BOOST_AUTO_TEST_CASE(test_M22)
{
    M22 m;
    m.SQN = 1;
    m.Timestamp = stdex::get_time_YYYYMMDDHHMMSS();
    m.Destination = make_MSGDEST();
    m.LCDDisplayTemplate = make_TLPRE(EDisplayTemplateType::LCDNormal);
    m.Spare = make_TLPRE(EDisplayTemplateType::Spare);
    m.DataLength = m.datasize();
    LOG_INFO("%s", m.dump());

    std::stringstream ss;
    ss << m;
    LOG_INFO("\nM22: %s", ss.str());

    M22 m2;
    ss >> m2;
    LOG_INFO("\nM22: %s", m2.dump());
    BOOST_TEST((m == m2));

    {
        Destination dest = make_Destination();
        PredefinedDisplayTemplate lcd = make_PredefinedDisplayTemplate(EDisplayTemplateType::LCDNormal);
        PredefinedDisplayTemplate led = make_PredefinedDisplayTemplate(EDisplayTemplateType::Spare);

        M22 m3;
        m3.SQN = 1;
        m3.Timestamp = stdex::get_time_YYYYMMDDHHMMSS();
        m3.set(dest, lcd, led);
        BOOST_TEST((m3 == m));
    }
}

BOOST_AUTO_TEST_CASE(test_A22)
{
    A22 a;
    a.SQN = 1;
    a.Timestamp = stdex::get_time_YYYYMMDDHHMMSS();
    a.ReportStation = "OCC";
    a.DataLength = a.datasize();
    LOG_INFO("%s", a.dump());

    std::stringstream ss;
    ss << a;
    LOG_INFO("\nA22: %s", ss.str());

    A22 a2;
    ss >> a2;
    LOG_INFO("\nA22: %s", a2.dump());
    BOOST_TEST((a == a2));
}

BOOST_AUTO_TEST_CASE(test_M23)
{
    M23 m;
    m.SQN = 1;
    m.Timestamp = stdex::get_time_YYYYMMDDHHMMSS();
    m.Destination = make_MSGDEST();
    m.DataLength = m.datasize();
    LOG_INFO("%s", m.dump());

    std::stringstream ss;
    ss << m;
    LOG_INFO("\nM23: %s", ss.str());

    M23 m2;
    ss >> m2;
    LOG_INFO("\nM23: %s", m2.dump());
    BOOST_TEST((m == m2));

    {
        Destination dest = make_Destination();

        M23 m3;
        m3.SQN = 1;
        m3.Timestamp = stdex::get_time_YYYYMMDDHHMMSS();
        m3.set(dest, EDisplayTemplateType::Default);
        BOOST_TEST((m3 == m));
    }
}

BOOST_AUTO_TEST_CASE(test_A23)
{
    A23 a;
    a.SQN = 1;
    a.Timestamp = stdex::get_time_YYYYMMDDHHMMSS();
    a.ReportStation = "OCC";
    a.DataLength = a.datasize();
    LOG_INFO("%s", a.dump());

    std::stringstream ss;
    ss << a;
    LOG_INFO("\nA23: %s", ss.str());

    A23 a2;
    ss >> a2;
    LOG_INFO("\nA23: %s", a2.dump());
    BOOST_TEST((a == a2));
}

BOOST_AUTO_TEST_CASE(test_M21)
{
    M21 m;
    m.SQN = 1;
    m.Timestamp = stdex::get_time_YYYYMMDDHHMMSS();
    m.Destination = make_MSGDEST();
    m.ControlOn = EPIDControlOn::NoAction;
    m.ControlOff = EPIDControlOff::NoAction;
    m.DataLength = m.datasize();
    LOG_INFO("%s", m.dump());

    std::stringstream ss;
    ss << m;
    LOG_INFO("\nM21: %s", ss.str());

    M21 m2;
    ss >> m2;
    LOG_INFO("\nM21: %s", m2.dump());
    BOOST_TEST((m == m2));

    {
        Destination dest = make_Destination();

        M21 m3;
        m3.SQN = 1;
        m3.Timestamp = stdex::get_time_YYYYMMDDHHMMSS();
        m3.set(dest, EPIDControlOn::NoAction, EPIDControlOff::NoAction);
        BOOST_TEST((m3 == m));
    }
}

BOOST_AUTO_TEST_CASE(test_A21)
{
    A21 a;
    a.SQN = 1;
    a.Timestamp = stdex::get_time_YYYYMMDDHHMMSS();
    a.ReportStation = "OCC";
    a.DataLength = a.datasize();
    LOG_INFO("%s", a.dump());

    std::stringstream ss;
    ss << a;
    LOG_INFO("\nA21: %s", ss.str());

    A21 a2;
    ss >> a2;
    LOG_INFO("\nA21: %s", a2.dump());
    BOOST_TEST((a == a2));
}

BOOST_AUTO_TEST_CASE(test_M30)
{
    M30 m;
    m.SQN = 1;
    m.Timestamp = stdex::get_time_YYYYMMDDHHMMSS();
    m.DestinationStation = "OCC";
    m.DataLength = m.datasize();
    LOG_INFO("%s", m.dump());

    std::stringstream ss;
    ss << m;
    LOG_INFO("\nM30: %s", ss.str());

    M30 m2;
    ss >> m2;
    LOG_INFO("\nM30: %s", m2.dump());
    BOOST_TEST((m == m2));

    {
        M30 m3;
        m3.SQN = 1;
        m3.Timestamp = stdex::get_time_YYYYMMDDHHMMSS();
        m3.set("OCC");
        BOOST_TEST((m3 == m));
    }
}

BOOST_AUTO_TEST_CASE(test_A30)
{
    A30 a;
    a.SQN = 1;
    a.Timestamp = stdex::get_time_YYYYMMDDHHMMSS();
    a.ReportStation = "OCC";
    a.ConnectionLinkStatus = 1;
    a.versions_tied() = std::make_tuple("001", "001", "001", "001");
    a.PIDStatusList.emplace_back("101", EPIDStatus::Off);
    a.DataLength = a.datasize();
    LOG_INFO("%s", a.dump());

    std::stringstream ss;
    ss << a;
    LOG_INFO("\nA30: %s", ss.str());

    A30 a2;
    ss >> a2;
    LOG_INFO("\nA30: %s", a2.dump());
    BOOST_TEST((a == a2));
}

BOOST_AUTO_TEST_CASE(test_M32)
{
    M32 m;
    m.SQN = 1;
    m.Timestamp = stdex::get_time_YYYYMMDDHHMMSS();
    m.DataLength = m.datasize();
    LOG_INFO("%s", m.dump());

    std::stringstream ss;
    ss << m;
    LOG_INFO("\nM32: %s", ss.str());

    M32 m2;
    ss >> m2;
    LOG_INFO("\nM32: %s", m2.dump());
    BOOST_TEST((m == m2));

    {
        M32 m3;
        m3.SQN = 1;
        m3.Timestamp = stdex::get_time_YYYYMMDDHHMMSS();
        m3.set("OCC");
        BOOST_TEST((m3 == m));
    }
}

BOOST_AUTO_TEST_CASE(test_A32)
{
    A32 a;
    a.SQN = 1;
    a.Timestamp = stdex::get_time_YYYYMMDDHHMMSS();

    STATIONSTATUSDETAIL ssd;
    ssd.ReportStation = "OCC";
    ssd.ConnectionLinkStatus = 1;
    ssd.versions_tied() = std::make_tuple("001", "001", "001", "001");
    ssd.PIDStatusList.emplace_back("101", EPIDStatus::Off);
    a.AllStationStatusDetails.emplace_back(std::move(ssd));
    // a.DataLength = a.datasize();
    LOG_INFO("%s", a.dump());

    std::stringstream ss;
    ss << a;
    LOG_INFO("\nA32: %s", ss.str());

    A32 a2;
    ss >> a2;
    LOG_INFO("\nA32: %s", a2.dump());
    BOOST_TEST((a == a2));
}

BOOST_AUTO_TEST_CASE(test_M50)
{
    M50 m;
    m.SQN = 1;
    m.Timestamp = stdex::get_time_YYYYMMDDHHMMSS();
    m.DestinationStation = "OCC";
    m.DestinationPID = "101";
    m.DataLength = m.datasize();
    LOG_INFO("%s", m.dump());

    std::stringstream ss;
    ss << m;
    LOG_INFO("\nM50: %s", ss.str());

    M50 m2;
    ss >> m2;
    LOG_INFO("\nM50: %s", m2.dump());
    BOOST_TEST((m == m2));

    {
        M50 m3;
        m3.SQN = 1;
        m3.Timestamp = stdex::get_time_YYYYMMDDHHMMSS();
        m3.set("OCC", "101");
        BOOST_TEST((m3 == m));
    }
}

BOOST_AUTO_TEST_CASE(test_A50)
{
    A50 a;
    a.SQN = 1;
    a.Timestamp = stdex::get_time_YYYYMMDDHHMMSS();
    a.ReportStation = "OCC";
    a.ReportPID = "101";
    a.CurrentDiaplayTemplate.DisplayTemplateType = EDisplayTemplateType::Default;
    a.CurrentDiaplayTemplate.DisplayTemplateID = "001";
    a.CurrentDiaplayTemplate.StartTime = stdex::get_time_YYYYMMDDHHMMSS();
    a.CurrentDiaplayTemplate.EndTime = stdex::get_time_YYYYMMDDHHMMSS();
    a.MessageStatus = "00";
    a.MessageTag = "[-MSG--TAG-]";
    a.MessageStartTime = stdex::get_time_YYYYMMDDHHMMSS();
    a.MessageEndTime = stdex::get_time_YYYYMMDDHHMMSS();
    a.MessagePriority = 1;
    a.MessageText = "hello, world";
    a.DataLength = a.datasize();
    LOG_INFO("%s", a.dump());

    std::stringstream ss;
    ss << a;
    LOG_INFO("\nA50: %s", ss.str());

    A50 a2;
    ss >> a2;
    LOG_INFO("\nA50: %s", a2.dump());
    BOOST_TEST((a == a2));
}

BOOST_AUTO_TEST_CASE(test_M31)
{
    M31 m;
    m.SQN = 1;
    m.Timestamp = stdex::get_time_YYYYMMDDHHMMSS();
    m.DestinationOCC = "OCC";
    m.versions_tied() = std::make_tuple("001", "001", "001", "001");
    m.DataLength = m.datasize();
    LOG_INFO("%s", m.dump());

    std::stringstream ss;
    ss << m;
    LOG_INFO("\nM31: %s", ss.str());

    M31 m2;
    ss >> m2;
    LOG_INFO("\nM31: %s", m2.dump());
    BOOST_TEST((m == m2));

    {
        M31 m3;
        m3.SQN = 1;
        m3.Timestamp = stdex::get_time_YYYYMMDDHHMMSS();
        m3.set("OCC", {"001"s, "001"s, "001"s, "001"s});
        BOOST_TEST((m3 == m));
    }
}

BOOST_AUTO_TEST_CASE(test_A31)
{
    A31 a;
    a.SQN = 1;
    a.Timestamp = stdex::get_time_YYYYMMDDHHMMSS();
    a.ConnectionLinkStatus = EConnectionLinkStatus::Normal;
    a.STISOCCServerStatus = ESTISOCCServerStatus::Normal;
    a.versions_tied() = std::make_tuple("001"s, "001"s, "001"s, "001"s);
    a.DataLength = a.datasize();
    LOG_INFO("%s", a.dump());

    std::stringstream ss;
    ss << a;
    LOG_INFO("\nA31: %s", ss.str());

    A31 a2;
    ss >> a2;
    LOG_INFO("\nA31: %s", a2.dump());
    BOOST_TEST((a == a2));
}

BOOST_AUTO_TEST_CASE(test_M70)
{
    M70 m;
    m.SQN = 1;
    m.Timestamp = stdex::get_time_YYYYMMDDHHMMSS();
    m.Type = ELibraryType::PredefinedMessage;
    m.Version = "001";
    m.DataLength = m.datasize();
    LOG_INFO("%s", m.dump());

    std::stringstream ss;
    ss << m;
    LOG_INFO("\nM70: %s", ss.str());

    M70 m2;
    ss >> m2;
    LOG_INFO("\nM70: %s", m2.dump());
    BOOST_TEST((m == m2));

    {
        M70 m3;
        m3.SQN = 1;
        m3.Timestamp = stdex::get_time_YYYYMMDDHHMMSS();
        m3.set(ELibraryType::PredefinedMessage, "001");
        BOOST_TEST((m3 == m));
    }
}

BOOST_AUTO_TEST_CASE(test_A70)
{
    A70 a;
    a.SQN = 1;
    a.Timestamp = stdex::get_time_YYYYMMDDHHMMSS();
    a.Type = ELibraryType::PredefinedMessage;
    a.Version = "001";
    a.DataLength = a.datasize();
    LOG_INFO("%s", a.dump());

    std::stringstream ss;
    ss << a;
    LOG_INFO("\nA70: %s", ss.str());

    A70 a2;
    ss >> a2;
    LOG_INFO("\nA70: %s", a2.dump());
    BOOST_TEST((a == a2));
}

BOOST_AUTO_TEST_CASE(test_A99)
{
    A99 a;
    a.SQN = 1;
    a.Timestamp = stdex::get_time_YYYYMMDDHHMMSS();
    a.Reason = 1;
    a.DataLength = a.datasize();
    LOG_INFO("%s", a.dump());

    std::stringstream ss;
    ss << a;
    LOG_INFO("\nA99: %s", ss.str());

    A99 a2;
    ss >> a2;
    LOG_INFO("\nA99: %s", a2.dump());
    BOOST_TEST((a == a2));
}

BOOST_AUTO_TEST_CASE(test_GENERALPACKET)
{
    A99 a;
    a.SQN = 1;
    a.Timestamp = stdex::get_time_YYYYMMDDHHMMSS();
    a.Reason = 1;
    a.DataLength = a.datasize();
    LOG_INFO("%s", a.dump());

    std::stringstream ss;
    ss << a;

    GENERALPACKET packet;
    ss >> packet;

    BOOST_TEST((packet.SQN == 1));
    BOOST_TEST((packet.Timestamp == stdex::get_time_YYYYMMDDHHMMSS()));
    BOOST_TEST((packet.MessageID == "A99"s));
}

#if 0
BOOST_AUTO_TEST_CASE(test_SequenceGenerator)
{
    auto sqn = s_sequence_generator();
    BOOST_TEST(sqn == 1);

    for (int i = 2; i <= 9999; ++i)
    {
        sqn = s_sequence_generator();
        BOOST_TEST(sqn == i);
    }

    BOOST_TEST(s_sequence_generator() == 1);
}
#endif

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
