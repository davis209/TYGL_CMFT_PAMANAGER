#pragma once
#include "STISLibraryClient.h"
#include "STISStatusClient.h"
#include "STISMessageClient.h"

namespace TA_IRS_App::STIS_PROTOCOL::INTERFACES::stisclient::detail
{
    using namespace std::string_literals;
    using boost::filesystem::path;
    using namespace TA_Base_Core;
    using Blob = std::vector<unsigned char>;
    using AdHocMessageItem = std::tuple<int, std::string, std::string>;
    using AdHocMessageMap = std::map<int, AdHocMessageItem>;
    using AdHocMessageMapPtr = std::shared_ptr<AdHocMessageMap>;

    struct STISClient
    {
        static STISClient& instance();

        STISClient(std::string options = "");

        void parse_options(std::string options);

        // synchronize

        void start();
        void stop();
        void async_now();
        bool sync_versions();
        bool sync_all_versions();
        bool sync_ad_hoc_message_library();
        size_t sync_predefined_message_library(std::string version);
        size_t sync_display_template_library(std::string version);
        size_t sync_display_templates(std::string version);
        size_t sync_templates();
        size_t sync_current_message_template_library();
        size_t sync_current_next_message_template_library();
        void change_sync_interval_for_a_while(size_t new_interval_ms, size_t duration_ms);
        void change_sync_interval_until(size_t new_interval_ms, size_t check_pred_interval_ms, size_t timeout_ms, std::function<bool()> pred);

        // predefined message library

        std::string current_message_library_version();
        std::string next_message_library_version();
        std::pair<std::string, std::string> current_next_message_library_versions();

        void set_current_message_library_version(std::string version);
        void set_next_message_library_version(std::string version);
        void upgrade_message_library_version(std::string version);

        bool has_current_message_library();
        bool has_next_message_library();
        bool has_message_library(std::string version);

        STSMSGLIB_XML_PTR load_current_message_library_xml();
        STSMSGLIB_XML_PTR load_next_message_library_xml();
        STSMSGLIB_XML_PTR load_message_library_xml(std::string version);
        IPredefinedMessageLibraryPtr load_message_library();

        void add_message_library(std::string version, const Blob& blob);
        void remove_message_library(std::string version);

        Blob download_predefined_message_library(std::string version);

        // ad hoc message library

        AdHocMessageMapPtr load_ad_hoc_message_library();
        void set_ad_hoc_message(int key, const std::string& title, const std::string& content);
        void delete_ad_hoc_message(int key);
        AdHocMessageItem get_ad_hoc_message(int key);
        std::pair<std::string, bool> lock_ad_hoc_message(int key);
        void unlock_ad_hoc_message(int key);

        // display template library

        std::string current_template_library_version();
        std::string next_template_library_version();
        std::pair<std::string, std::string> current_next_template_library_versions();

        void set_current_template_library_version(std::string version);
        void set_next_template_library_version(std::string version);
        void upgrade_template_library_version(std::string version);

        bool has_current_template_library();
        bool has_next_template_library();
        bool has_template_library(std::string version);

        STSTMLIB_XML_PTR load_current_template_library_xml();
        STSTMLIB_XML_PTR load_next_template_library_xml();
        STSTMLIB_XML_PTR load_template_library_xml(std::string version);
        ITemplateLibraryPtr load_template_library();

        void add_template_library(std::string version, const Blob& blob);
        void remove_template_library(std::string version);

        // display template

        const std::string& get_lcd_template(const std::string& id);
        const std::string& get_lcd_emergency_template(const std::string& id);
        const std::string& get_led_template(const std::string& id);
        const std::string& get_led_emergency_template(const std::string& id);

        Blob download_display_template_library(std::string version);
        std::map<path, Blob> download_templates(const std::vector<path>& excludes = {});

        // all library

        std::string get_library_version(const std::string& category, std::string version);
        void set_library_version(const std::string& category, std::string version, const std::string& number);
        void upgrade_library_version(const std::string& category, std::string version);
        bool has_library(const std::string& category, std::string version);
        void add_library(const std::string& category, std::string version, const Blob& blob);
        Blob get_library(const std::string& category, std::string version);
        void remove_library(const std::string& category, std::string version);

        auto versions_tuple()
        {
            return std::tuple_cat(current_next_message_library_versions(), current_next_template_library_versions());
        }

        auto category_versions_tuple()
        {
            return std::make_tuple(std::make_tuple("message"s, "current"s, current_message_library_version()),
                                   std::make_tuple("message"s, "next"s, next_message_library_version()),
                                   std::make_tuple("template"s, "current"s, current_template_library_version()),
                                   std::make_tuple("template"s, "next"s, next_template_library_version()));
        }

        // library download

        Blob download_library(const std::string& category, std::string version);

        // status

        AllCurNxtMsgTmpLibVers get_this_station_iscs_stis_library_versions();
        AllCurNxtMsgTmpLibVersInfoList get_all_station_iscs_stis_library_versions();

        // message

        void submit_M10_DisplayPredefinedMessageRequest(const Destination& dest, const PredefinedMessage& msg);
        void submit_M10_DisplayPredefinedMessageRequestList(const DestinationList& dests, const PredefinedMessage& msg);

        void submit_M11_DisplayAdHocMessageRequest(const Destination& dest, const AdHodMessage& msg);
        void submit_M11_DisplayAdHocMessageRequestList(const DestinationList& dests, const AdHodMessage& msg);

        void submit_M20_ClearCurrentMessageRequest(const Destination& dest, const std::vector<int>& priority);
        void submit_M20_ClearCurrentMessageRequestList(const DestinationList& dests, const std::vector<int>& priority);

        void submit_M21_PIDOnOffControlRequest(const Destination& dest, EPIDControlOn on, EPIDControlOff off);
        void submit_M21_PIDOnOffControlRequestList(const DestinationList& dests, EPIDControlOn on, EPIDControlOff off);

        void submit_M22_SendPredefinedDisplayTemplateRequest(const Destination& dest, const PredefinedDisplayTemplate& lcd, const PredefinedDisplayTemplate& spare = {});
        void submit_M22_SendPredefinedDisplayTemplateRequestList(const DestinationList& dests, const PredefinedDisplayTemplate& lcd, const PredefinedDisplayTemplate& spare = {});

        void submit_M23_RemoveDisplayTemplateRequest(const Destination& dest, EDisplayTemplateType type);
        void submit_M23_RemoveDisplayTemplateRequestList(const DestinationList& dests, EDisplayTemplateType type);

        void submit_M24_ClearCurrentMessagesRequestByMessageTag(const Destination& dest, std::string msg_tag, std::string sessionId = "");
        void submit_M24_ClearCurrentMessagesRequestByMessageTagList(const DestinationList& dests, std::string msg_tag, std::string sessionId = "");

        void submit_M25_SchedulePIDOnOffTimeSettingRequest(const Destination& dest, const std::string& monitorOffTime, const std::string& monitorOnTime);
        void submit_M25_SchedulePIDOnOffTimeSettingRequestList(const DestinationList& dests, const std::string& monitorOffTime, const std::string& monitorOnTime);

        A30_StationSTISStatusReport submit_M30_StationSTISStatusRequest(const std::string& station);
        A31_OCCSTISStatusSyncReport submit_M31_OCCSTISStatusSyncRequest(const std::string& destination_occ, const CurNxtMsgTmpLibVers& iscs_versions);
        A32_AllStationStatusDetails submit_M32_AllStationSTISStatusRequest(const std::string& station);
        A33_OCCSTISStatusSyncReport submit_M33_OCCSTISStatusSyncRequest(const std::string& destination_occ, const CurNxtMsgTmpLibVers& iscs_versions);
        A50_CurrentDisplayMessageTemplateReport submit_M50_CurrentDisplayMessageTemplateRequest(const std::string& station, const PID& pid);
        A51_StationScheduledDisplayingMessageTemplateListReport submit_M51_StationScheduledDisplayingMessageTemplateListRequest(const std::string& station, std::string sessionId = "");
        A52_StationScheduledDisplayingTemplateListReport submit_M52_StationScheduledDisplayingTemplateListRequest(const std::string& station, std::string sessionId = "");
        void submit_M53_RemoveDisplayTemplateByTemplateIDRequest(const Destination& dest, const PredefinedDisplayTemplateList& display_template_list, std::string sessionId = "");
        void submit_M53_RemoveDisplayTemplateByTemplateIDRequestList(const DestinationList& dests, const std::vector<PredefinedDisplayTemplateList>& display_template_list_list, std::string sessionId = "");
        A70_UpgradePredefinedMessageDisplayTemplateLibraryReport submit_M70_UpgradePredefinedMessageDisplayTemplateLibraryRequest(ELibraryType type, std::string version);

        STISLibraryClient m_library;
        STISStatusClient m_status;
        STISMessageClient m_message;
    };
}

namespace TA_IRS_App::STIS_PROTOCOL::INTERFACES
{
    using TA_IRS_App::STIS_PROTOCOL::INTERFACES::stisclient::detail::STISClient;
}

namespace TA_IRS_App::STIS_PROTOCOL
{
    using namespace INTERFACES;
}
