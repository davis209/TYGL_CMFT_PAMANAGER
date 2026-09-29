#pragma once
#include "core/utility/src/core/preprocessor/tuple.h"
#include <boost/describe.hpp>
#include <string>
#include <vector>
#include <memory>
#include <tuple>
#include <map>

namespace TA_IRS_App::STIS_PROTOCOL::MESSAGE_TYPES::HELPERS
{
    using namespace std::string_literals;

    inline bool is_valid_version(const std::string& version)
    {
        return version.size() == 3 && version != "000";
    }

    inline bool is_invalid_version(const std::string& version)
    {
        return !is_valid_version(version);
    }

    inline std::string& normalize_version(std::string& version)
    {
        if (version.size() != 3)
        {
            std::string v(3, '0');

            if (3 < version.size())
            {
                version = version.substr(0, 3);
            }

            std::copy_backward(version.begin(), version.end(), v.end());
            version = std::move(v);
        }

        return version;
    }

    inline std::string normalize_version_copy(std::string version)
    {
        return normalize_version(version);
    }

    inline std::string normalize_version(std::string&& version)
    {
        return normalize_version_copy(std::move(version));
    }

    struct CurrentNextMessageTemplateLibraryVersions
    {
        using ThisClass = CurrentNextMessageTemplateLibraryVersions;
        using Tuple = std::tuple<std::string, std::string, std::string, std::string>;
        using TupleRef = std::tuple<std::string&, std::string&, std::string&, std::string&>;

        mutable std::string current_message_library_version = "000";
        mutable std::string next_message_library_version = "000";
        mutable std::string current_template_library_version = "000";
        mutable std::string next_template_library_version = "000";

        TA_PP_TUPLE_ENABLE_MEMBER_UTILITIES(CurrentNextMessageTemplateLibraryVersions, (current_message_library_version,
                                                                                        next_message_library_version,
                                                                                        current_template_library_version,
                                                                                        next_template_library_version));

        std::pair<std::string, std::string> current_next_message_library_versions() const
        {
            this->normalize();
            return {current_message_library_version, next_message_library_version};
        }

        std::pair<std::string, std::string> current_next_template_library_versions() const
        {
            this->normalize();
            return {current_template_library_version, next_template_library_version};
        }

        auto category_versions_tuple()
        {
            this->normalize();
            return std::make_tuple(std::make_tuple("message"s, "current"s, current_message_library_version),
                                   std::make_tuple("message"s, "next"s, next_message_library_version),
                                   std::make_tuple("template"s, "current"s, current_template_library_version),
                                   std::make_tuple("template"s, "next"s, next_template_library_version));
        }

        std::vector<std::string> to_vector() const
        {
            this->normalize();
            return
            {
                current_message_library_version,
                next_message_library_version,
                current_template_library_version,
                next_template_library_version
            };
        }

        std::map<std::string, std::string> to_map() const
        {
            this->normalize();
            return
            {
                {"current-message", current_message_library_version},
                {"next-message", next_message_library_version},
                {"current-template", current_template_library_version},
                {"next-template", next_template_library_version},
            };
        }

        CurrentNextMessageTemplateLibraryVersions() = default;

        CurrentNextMessageTemplateLibraryVersions(const std::string& current_message_library_version,
                                                  const std::string& next_message_library_version,
                                                  const std::string& current_template_library_version,
                                                  const std::string& next_template_library_version)
        {
            tied() = std::tie
            (
                current_message_library_version,
                next_message_library_version,
                current_template_library_version,
                next_template_library_version
            );
            normalize();
        }

        template <class T>
        CurrentNextMessageTemplateLibraryVersions(T tuple)
        {
            tied() = std::move(tuple);
            normalize();
        }

        CurrentNextMessageTemplateLibraryVersions& clear()
        {
            std::apply([&](auto && ... args) { ((args = "000"), ...); }, tied());
            return *this;
        }

        bool empty() const
        {
            this->normalize();
            return tied() == std::tie("000", "000", "000", "000");
        }

        const CurrentNextMessageTemplateLibraryVersions& normalize() const
        {
            return const_cast<CurrentNextMessageTemplateLibraryVersions*>(this)->normalize();
        }

        CurrentNextMessageTemplateLibraryVersions& normalize()
        {
            std::apply([&](auto && ... args) { (normalize_version(args), ...); }, tied());
            return *this;
        }

        CurrentNextMessageTemplateLibraryVersions& operator=(const std::vector<std::string>& v)
        {
            return this->assign(v, 0);
        }

        CurrentNextMessageTemplateLibraryVersions& assign(const std::vector<std::string>& v, size_t offset = 0)
        {
            if (4 <= (v.size() - offset))
            {
                this->tied() = std::tie(v[offset + 0], v[offset + 1], v[offset + 2], v[offset + 3]);
            }

            return this->normalize();
        }

        static CurrentNextMessageTemplateLibraryVersions from_vector(const std::vector<std::string>& v, size_t offset = 0)
        {
            return CurrentNextMessageTemplateLibraryVersions{}.assign(v, offset);
        }

        static CurrentNextMessageTemplateLibraryVersions from_map(const std::map<std::string, std::string>& versions)
        {
            auto& m = const_cast<std::map<std::string, std::string>&>(versions);
            CurrentNextMessageTemplateLibraryVersions v;
            v.tied() = {m["current-message"], m["next-message"], m["current-template"], m["next-template"]};
            return v.normalize();
        }
    };

    BOOST_DESCRIBE_STRUCT(CurrentNextMessageTemplateLibraryVersions, (), (current_message_library_version,
                                                                          next_message_library_version,
                                                                          current_template_library_version,
                                                                          next_template_library_version));

    using CurNxtMsgTmpLibVers = CurrentNextMessageTemplateLibraryVersions;
    using CNMTVersions = CurrentNextMessageTemplateLibraryVersions;

    struct ISCSSTISCurrentNextMessageTemplateLibraryVersions
    {
        CurNxtMsgTmpLibVers iscs;
        CurNxtMsgTmpLibVers stis;
        TA_PP_TUPLE_ENABLE_MEMBER_UTILITIES(ISCSSTISCurrentNextMessageTemplateLibraryVersions, (iscs, stis));

        void clear()
        {
            iscs.clear();
            stis.clear();
        }

        bool empty() const
        {
            return iscs.empty() && stis.empty();
        }

        std::vector<std::string> to_vector() const
        {
            auto v = iscs.to_vector();
            auto v2 = stis.to_vector();
            v.insert(v.end(), v2.begin(), v2.end());
            return v;
        }

        ISCSSTISCurrentNextMessageTemplateLibraryVersions& assign(const std::vector<std::string>& v, size_t offset = 0)
        {
            if (8 <= (v.size() - offset))
            {
                iscs.assign(v, offset + 0);
                stis.assign(v, offset + 4);
            }

            return *this;
        }

        ISCSSTISCurrentNextMessageTemplateLibraryVersions& operator=(const std::vector<std::string>& v)
        {
            return this->assign(v);
        }

        static ISCSSTISCurrentNextMessageTemplateLibraryVersions from_vector(const std::vector<std::string>& v, size_t offset = 0)
        {
            return ISCSSTISCurrentNextMessageTemplateLibraryVersions{}.assign(v, offset);
        }
    };

    BOOST_DESCRIBE_STRUCT(ISCSSTISCurrentNextMessageTemplateLibraryVersions, (), (iscs, stis));

    using AllCurNxtMsgTmpLibVers = ISCSSTISCurrentNextMessageTemplateLibraryVersions;

    struct ISCSSTISCurrentNextMessageTemplateLibraryVersionsWithLocationInfo
    {
        std::uint32_t location_key;
        std::string location_name;
        AllCurNxtMsgTmpLibVers versions;
        TA_PP_TUPLE_ENABLE_MEMBER_UTILITIES(ISCSSTISCurrentNextMessageTemplateLibraryVersionsWithLocationInfo, (location_key, location_name, versions));
    };

    BOOST_DESCRIBE_STRUCT(ISCSSTISCurrentNextMessageTemplateLibraryVersionsWithLocationInfo, (), (location_key, location_name, versions));

    using AllCurNxtMsgTmpLibVersInfo = ISCSSTISCurrentNextMessageTemplateLibraryVersionsWithLocationInfo;
    using ISCSSTISCurrentNextMessageTemplateLibraryVersionsWithLocationInfoList = std::vector<AllCurNxtMsgTmpLibVersInfo>;
    using AllCurNxtMsgTmpLibVersInfoList = ISCSSTISCurrentNextMessageTemplateLibraryVersionsWithLocationInfoList;
}

namespace TA_IRS_App::STIS_PROTOCOL::MESSAGE_TYPES
{
    using namespace HELPERS;

    static inline const std::string STIS_MESSAGE_SERVANT_NAME = "STIS_SERVER-STIS";
    static inline const std::string STIS_LIBRARY_SERVANT_NAME = "STIS_SERVER-LIBRARY";
    static inline const std::string STIS_STATUS_SERVANT_NAME = "STIS_SERVER-STATUS";
    static inline const std::string STIS_DISPLAY_TEMPLATE_SERVANT_NAME = "STIS_SERVER-DISPLAY_TEMPLATE";

    using PID = std::string;
    using PIDList = std::vector<PID>;

    struct Destination
    {
        std::string system_id = "TYG"; // ¡°JRL¡± ¨C Message to TEL STIS.
        std::string station_id; // e.g. ¡°TE01, TE02¡±) for STIS. ¡°ASTN¡±¡ªfor all PIDs in all stations
        // int number_of_pids = 0; // '000'-for all PIDs in the specified station
        PIDList pid_list;

        static Destination from_location_pid(std::string location, std::string pid)
        {
            Destination d;
            d.station_id = location;
            d.pid_list.emplace_back(pid);
            return d;
        }

        static Destination from_location_pids(std::string location, PIDList pids)
        {
            Destination d;
            d.station_id = location;
            d.pid_list = pids;
            return d;
        }

        TA_PP_TUPLE_ENABLE_MEMBER_UTILITIES(Destination, (system_id, station_id, /*number_of_pids, */pid_list));
    };

    BOOST_DESCRIBE_STRUCT(Destination, (), (system_id, station_id, /*number_of_pids, */pid_list));

    using DestinationList = std::vector<Destination>;

    enum class EDisplayMode : char
    {
        InstantOn = '0',
        ScrollLeft = '1',  // default
        ScrollRight = '2',
        RollUp = '3',
        RollDown = '4',
        WipeLeft = '5',
        WipeRight = '6',
        LeftMultipleWiping = '7',
        RightMultipleWiping = '8',
        CurtainOpening = '9',
        CurtainClosing = 'A',
        Random = 'B',
        CentreSpreadFromCenterToTwoEnds = 'C',
        TwoEndsSpread = 'D',
        Flashing = 'E',
        DroppingEffect = 'G',
        ColourChange = 'H'
    };

    enum class ESpeed
    {
        NonScrolling,
        Slow,
        Medium, // default
        Fast
    };

    enum class EAlignment
    {
        LeftAligned,
        Centered, // default
        RightAligned
    };

    struct DisplayEffect
    {
        EDisplayMode display_mode = EDisplayMode::ScrollLeft;
        ESpeed speed = ESpeed::Medium;
        int repeat_interval = 0; // Seconds. '0000' for continuous display during the period between start time and end time,
        int display_time = 0;
        EAlignment alignment = EAlignment::Centered;
        int font_size = 0; //'0'-'8'
        int font_family = 0; // '0'-'3'
        int font_color = 0; // '0'-'8'
        int background_color = 0; // '0'-'8'
        TA_PP_TUPLE_ENABLE_MEMBER_UTILITIES(DisplayEffect, (display_mode, speed, repeat_interval, display_time, alignment, font_size, font_family, font_color, background_color));
    };

    BOOST_DESCRIBE_STRUCT(DisplayEffect, (), (display_mode, speed, repeat_interval, display_time, alignment, font_size, font_family, font_color, background_color));

    enum class ETemplateType
    {
        Default,
        EmergencyLcd,
        EmergencyLed,
        NormalLcd,
        NormalLed
    };

    struct PredefinedMessage
    {
        std::string message_tag;
        std::string start_time;
        std::string end_time;
        int priority = 0; // 0: the predefined priority will be used. 1-3: Emergency Priority; 4-8: Normal Priority
        int spare = 0;
        ETemplateType lcd_emergency_display_template_type = ETemplateType::Default;
        std::string lcd_emergency_display_template_id = "000";
        ETemplateType led_emergency_display_template_type = ETemplateType::Default;
        std::string led_emergency_display_template_id = "000";
        TA_PP_TUPLE_ENABLE_MEMBER_UTILITIES(PredefinedMessage, (message_tag, start_time, end_time, priority, spare,
                                                                lcd_emergency_display_template_type,
                                                                lcd_emergency_display_template_id,
                                                                led_emergency_display_template_type,
                                                                led_emergency_display_template_id));
    };

    BOOST_DESCRIBE_STRUCT(PredefinedMessage, (), (message_tag, start_time, end_time, priority, spare,
                                                  lcd_emergency_display_template_type,
                                                  lcd_emergency_display_template_id,
                                                  led_emergency_display_template_type,
                                                  led_emergency_display_template_id));

    struct AdHodMessage
    {
        std::string message_tag;
        std::string start_time;
        std::string end_time;
        int priority = 1; // Priority 1-3: Emergency Priority; 4-8: Normal Priority.
        ETemplateType lcd_emergency_display_template_type = ETemplateType::Default;
        std::string lcd_emergency_display_template_id;
        ETemplateType led_emergency_display_template_type = ETemplateType::Default;
        std::string led_emergency_display_template_id;
        // int message_length = 0;
#if 0
        std::string message_text;
#else
        std::vector<unsigned char> message_text;  // utf16-le
#endif
        TA_PP_TUPLE_ENABLE_MEMBER_UTILITIES(AdHodMessage, (message_tag, start_time, end_time, priority,
                                                           lcd_emergency_display_template_type,
                                                           lcd_emergency_display_template_id,
                                                           led_emergency_display_template_type,
                                                           led_emergency_display_template_id,
                                                           /*message_length,*/
                                                           message_text));

        std::string get_message_text_utf8() const;
    };

    BOOST_DESCRIBE_STRUCT(AdHodMessage, (), (message_tag, start_time, end_time, priority,
                                             lcd_emergency_display_template_type,
                                             lcd_emergency_display_template_id,
                                             led_emergency_display_template_type,
                                             led_emergency_display_template_id,
                                             /*message_length,*/
                                             message_text));

    enum class EDisplayTemplateType
    {
        Default,        // '0' - All PIDs Normal Display template
        LCDEmergency,   // '1' - LCD Display Emergency template (cannot be removed)
        LEDEmergency,   // '2' - Reserved (no LED for TYGL CMFT)
        LCDNormal,      // '3' - LCD Display Normal template
        Spare           // '4' - Reserved for future
    };

    BOOST_DESCRIBE_ENUM(EDisplayTemplateType, Default, LCDEmergency, LEDEmergency, LCDNormal, Spare);

    struct PredefinedDisplayTemplate
    {
        EDisplayTemplateType display_template_type = EDisplayTemplateType::Default;
        std::string display_template_id = "000"; // '000': The current display template will remain. And the corresponding 'Display template type' shall be set to 0.
        std::string start_time = std::string(14, '0');
        std::string end_time = std::string(14, '0');
        TA_PP_TUPLE_ENABLE_MEMBER_UTILITIES(PredefinedDisplayTemplate, (display_template_type, display_template_id, start_time, end_time));
    };

    BOOST_DESCRIBE_STRUCT(PredefinedDisplayTemplate, (), (display_template_type, display_template_id, start_time, end_time));

    using PredefinedDisplayTemplateList = std::vector<PredefinedDisplayTemplate>;
    using ScheduledDisplayTemplateList = PredefinedDisplayTemplateList;

    enum class EPIDControlOn
    {
        NoAction,           // '0' - No action
        ControlOn           // '1' - Control on
    };

    enum class EPIDControlOff
    {
        NoAction,           // '0' - No action
        ControlOff          // '1' - Control off
    };

    enum class EConnectionLinkStatus
    {
        Normal,
        Alarm
    };

    enum class EPIDStatus
    {
        Off,
        On,
        Alarm
    };

    enum class ELANConnectionLinkStatus
    {
        Normal,
        LAN_1_Alarm,  // SW1 & FW1
        LAN_2_Alarm,  // SW2 & FW2
        LAN_1_LAN_2_Alarm
    };

    enum class EAlarmSummary
    {
        Normal,
        Minor,
        Major,
        MajorMinor
    };

    inline bool is_minor(EAlarmSummary v)
    {
        return EAlarmSummary::Minor == v || EAlarmSummary::MajorMinor == v;
    }

    inline bool is_major(EAlarmSummary v)
    {
        return EAlarmSummary::Major == v || EAlarmSummary::MajorMinor == v;
    }

    inline std::string to_message_status(EPIDStatus status)
    {
        switch (status)
        {
        case EPIDStatus::On:
            return "01";

        default:
        case EPIDStatus::Off:
        case EPIDStatus::Alarm:
            return "00";
        }
    }

    using PIDStatus = std::pair<PID, EPIDStatus>;
    using PIDStatusList = std::vector<PIDStatus>;

    struct A30_StationSTISStatusReport
    {
        std::string report_station;
        ELANConnectionLinkStatus lan_connection_link_status = ELANConnectionLinkStatus::Normal;
        EAlarmSummary alarm_summary = EAlarmSummary::Normal;
        int spare = 0;
        CurNxtMsgTmpLibVers versions;
        // int number_of_pids = 0;
        PIDStatusList pid_status_list;
        TA_PP_TUPLE_ENABLE_MEMBER_UTILITIES(A30_StationSTISStatusReport,
                                            (report_station, lan_connection_link_status, alarm_summary, spare,
                                             versions.current_message_library_version,
                                             versions.next_message_library_version,
                                             versions.current_template_library_version,
                                             versions.next_template_library_version,
                                             /*number_of_pids,*/
                                             pid_status_list));
    };

    BOOST_DESCRIBE_STRUCT(A30_StationSTISStatusReport, (), (report_station, lan_connection_link_status, alarm_summary, spare, versions, pid_status_list));

    using StationStatusDetails = A30_StationSTISStatusReport;
    using A32_AllStationStatusDetails = std::vector<StationStatusDetails>;

    struct CurrentDisplayMessage
    {
        std::string message_tag;
        std::string message_start_time;
        std::string message_end_time;
        int message_priority = 1;  // 1-8
        // int message_length = 0;
#if 0
        std::string message_text;
#else
        std::vector<unsigned char> message_text;  // utf16-le
#endif
        TA_PP_TUPLE_ENABLE_MEMBER_UTILITIES(CurrentDisplayMessage,
                                            (message_tag, message_start_time, message_end_time, message_priority, /*message_length, */message_text));

        std::string get_message_text_utf8() const;
    };

    BOOST_DESCRIBE_STRUCT(CurrentDisplayMessage, (), (message_tag, message_start_time, message_end_time, message_priority, /*message_length, */message_text));

    using CurrentDisplayMessageList = std::vector<CurrentDisplayMessage>;

    struct A50_CurrentDisplayMessageTemplateReport
    {
        std::string report_station;
        std::string report_pid;
        PredefinedDisplayTemplate current_display_template;
        std::string message_status;
        CurrentDisplayMessageList current_display_messages;
        TA_PP_TUPLE_ENABLE_MEMBER_UTILITIES(A50_CurrentDisplayMessageTemplateReport,
                                            (report_station, report_pid, current_display_template, message_status, current_display_messages));

        std::string make_unique_key() const
        {
            return report_station + ":" + report_pid;
        }

        static auto from(std::string station, std::string pid)
        {
            A50_CurrentDisplayMessageTemplateReport r;
            r.report_station = station;
            r.report_pid = pid;
            return r;
        }
    };

    BOOST_DESCRIBE_STRUCT(A50_CurrentDisplayMessageTemplateReport, (), (report_station, report_pid, current_display_template, message_status, current_display_messages));

    using A50_CurrentDisplayMessageTemplateReportPtr = std::shared_ptr<A50_CurrentDisplayMessageTemplateReport>;

    using ScheduledDisplayMessage = CurrentDisplayMessage;
    using ScheduledDisplayMessageList = std::vector<ScheduledDisplayMessage>;

    struct ScheduledDisplayMessageTemplateForPid
    {
        std::string report_station;
        std::string report_pid;
        PredefinedDisplayTemplate current_display_template;
        std::string message_status;
        ScheduledDisplayMessageList scheduled_display_messages;
        TA_PP_TUPLE_ENABLE_MEMBER_UTILITIES(ScheduledDisplayMessageTemplateForPid,
                                            (report_station, report_pid, current_display_template, message_status, scheduled_display_messages));
    };

    BOOST_DESCRIBE_STRUCT(ScheduledDisplayMessageTemplateForPid, (), (report_station, report_pid, current_display_template, message_status, scheduled_display_messages));

    using ScheduledDisplayMessageTemplateForPidList = std::vector<ScheduledDisplayMessageTemplateForPid>;

    struct A51_StationScheduledDisplayingMessageTemplateListReport
    {
        ScheduledDisplayMessageTemplateForPidList scheduled_display_message_for_pids;
        TA_PP_TUPLE_ENABLE_MEMBER_UTILITIES(A51_StationScheduledDisplayingMessageTemplateListReport, (scheduled_display_message_for_pids));
    };

    BOOST_DESCRIBE_STRUCT(A51_StationScheduledDisplayingMessageTemplateListReport, (), (scheduled_display_message_for_pids));

    using A51_StationScheduledDisplayingMessageTemplateListReportPtr = std::shared_ptr<A51_StationScheduledDisplayingMessageTemplateListReport>;

    struct ScheduledDisplayTemplateListForPid
    {
        std::string report_station;
        std::string report_pid;
        ScheduledDisplayTemplateList scheduled_display_template_list;
        TA_PP_TUPLE_ENABLE_MEMBER_UTILITIES(ScheduledDisplayTemplateListForPid, (report_station, report_pid, scheduled_display_template_list));
    };

    BOOST_DESCRIBE_STRUCT(ScheduledDisplayTemplateListForPid, (), (report_station, report_pid, scheduled_display_template_list));

    using ScheduledDisplayTemplateListForPidList = std::vector<ScheduledDisplayTemplateListForPid>;

    struct A52_StationScheduledDisplayingTemplateListReport
    {
        ScheduledDisplayTemplateListForPidList scheduled_display_template_list_for_pids;
        TA_PP_TUPLE_ENABLE_MEMBER_UTILITIES(A52_StationScheduledDisplayingTemplateListReport, (scheduled_display_template_list_for_pids));
    };

    BOOST_DESCRIBE_STRUCT(A52_StationScheduledDisplayingTemplateListReport, (), (scheduled_display_template_list_for_pids));

    using A52_StationScheduledDisplayingTemplateListReportPtr = std::shared_ptr<A52_StationScheduledDisplayingTemplateListReport>;

    enum class ESTISOCCServerStatus
    {
        Normal,
        Alarm
    };

    struct A31_OCCSTISStatusSyncReport
    {
        EConnectionLinkStatus connection_link_status = EConnectionLinkStatus::Normal;
        int spare = 0;
        ESTISOCCServerStatus stis_occ_server_status = ESTISOCCServerStatus::Normal;
        CurNxtMsgTmpLibVers versions;
        TA_PP_TUPLE_ENABLE_MEMBER_UTILITIES(A31_OCCSTISStatusSyncReport,
                                            (connection_link_status, spare, stis_occ_server_status,
                                             versions.current_message_library_version,
                                             versions.next_message_library_version,
                                             versions.current_template_library_version,
                                             versions.next_template_library_version));
    };

    BOOST_DESCRIBE_STRUCT(A31_OCCSTISStatusSyncReport, (), (connection_link_status, spare, stis_occ_server_status, versions));

    struct A33_OCCSTISStatusSyncReport
    {
        ELANConnectionLinkStatus lan_connection_link_status = ELANConnectionLinkStatus::Normal;
        EAlarmSummary alarm_summary = EAlarmSummary::Normal;
        int spare = 0;
        CurNxtMsgTmpLibVers versions;
        TA_PP_TUPLE_ENABLE_MEMBER_UTILITIES(A33_OCCSTISStatusSyncReport,
                                            (lan_connection_link_status, alarm_summary, spare,
                                             versions.current_message_library_version,
                                             versions.next_message_library_version,
                                             versions.current_template_library_version,
                                             versions.next_template_library_version));
    };

    BOOST_DESCRIBE_STRUCT(A33_OCCSTISStatusSyncReport, (), (lan_connection_link_status, alarm_summary, spare, versions));

    enum class ELibraryType
    {
        PredefinedMessage,
        DisplayTemplate
    };

    struct A70_UpgradePredefinedMessageDisplayTemplateLibraryReport
    {
        ELibraryType type = ELibraryType::PredefinedMessage;
        std::string version;
        TA_PP_TUPLE_ENABLE_MEMBER_UTILITIES(A70_UpgradePredefinedMessageDisplayTemplateLibraryReport, (type, version));
    };

    BOOST_DESCRIBE_STRUCT(A70_UpgradePredefinedMessageDisplayTemplateLibraryReport, (), (type, version));
}

namespace TA_IRS_App::STIS_PROTOCOL
{
    using namespace MESSAGE_TYPES;
}
