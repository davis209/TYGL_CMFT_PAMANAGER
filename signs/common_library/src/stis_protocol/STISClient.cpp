#include "pch.h"
#include "STISClient.h"
#include "core/utility/src/core/StaticObject.h"

namespace TA_IRS_App::STIS_PROTOCOL::INTERFACES::stisclient::detail
{
    using st::StaticObject;

    STISClient& STISClient::instance()
    {
        return StaticObject<STISClient>::value();
    }

    STISClient::STISClient(std::string options)
    {
        parse_options(std::move(options));
    }

    void STISClient::parse_options(std::string options)
    {
        m_library.parse_options(options);
        m_status.parse_options(options);
        m_message.parse_options(options);
    }

    // library synchronize

    void STISClient::start()
    {
        return m_library.start();
    }

    void STISClient::stop()
    {
        return m_library.stop();
    }

    void STISClient::async_now()
    {
        m_library.async_now();
    }

    size_t STISClient::sync_current_message_template_library()
    {
        return m_library.sync_current_message_template_library();
    }

    size_t STISClient::sync_current_next_message_template_library()
    {
        return m_library.sync_current_next_message_template_library();
    }

    bool STISClient::sync_versions()
    {
        return m_library.sync_versions();
    }

    bool STISClient::sync_all_versions()
    {
        return m_library.sync_all_versions();
    }

    bool STISClient::sync_ad_hoc_message_library()
    {
        return m_library.sync_ad_hoc_message_library();
    }

    size_t STISClient::sync_predefined_message_library(std::string version)
    {
        return m_library.sync_predefined_message_library(std::move(version));
    }

    size_t STISClient::sync_display_template_library(std::string version)
    {
        return m_library.sync_display_template_library(std::move(version));
    }

    size_t STISClient::sync_display_templates(std::string version)
    {
        return m_library.sync_display_templates(std::move(version));
    }

    size_t STISClient::sync_templates()
    {
        return m_library.sync_templates();
    }

    void STISClient::change_sync_interval_for_a_while(size_t new_interval_ms, size_t duration_ms)
    {
        m_library.change_sync_interval_for_a_while(new_interval_ms, duration_ms);
    }

    void STISClient::change_sync_interval_until(size_t new_interval_ms, size_t check_pred_interval_ms, size_t timeout_ms, std::function<bool()> pred)
    {
        m_library.change_sync_interval_until(new_interval_ms, check_pred_interval_ms, timeout_ms, std::move(pred));
    }

    // predefined message library

    std::string STISClient::current_message_library_version()
    {
        return m_library.current_message_library_version();
    }

    std::string STISClient::next_message_library_version()
    {
        return m_library.next_message_library_version();
    }

    std::pair<std::string, std::string> STISClient::current_next_message_library_versions()
    {
        return m_library.current_next_message_library_versions();
    }

    void STISClient::set_current_message_library_version(std::string version)
    {
        return m_library.set_current_message_library_version(normalize_version(version));
    }

    void STISClient::set_next_message_library_version(std::string version)
    {
        return m_library.set_next_message_library_version(normalize_version(version));
    }

    void STISClient::upgrade_message_library_version(std::string version)
    {
        return m_library.upgrade_message_library_version(normalize_version(version));
    }

    bool STISClient::has_current_message_library()
    {
        return m_library.has_current_message_library();
    }

    bool STISClient::has_next_message_library()
    {
        return m_library.has_next_message_library();
    }

    bool STISClient::has_message_library(std::string version)
    {
        return m_library.has_message_library(version);
    }

    STSMSGLIB_XML_PTR STISClient::load_current_message_library_xml()
    {
        return m_library.load_current_message_library_xml();
    }

    STSMSGLIB_XML_PTR STISClient::load_next_message_library_xml()
    {
        return m_library.load_next_message_library_xml();
    }

    STSMSGLIB_XML_PTR STISClient::load_message_library_xml(std::string version)
    {
        return m_library.load_message_library_xml(version);
    }

    IPredefinedMessageLibraryPtr STISClient::load_message_library()
    {
        return m_library.load_message_library();
    }

    void STISClient::add_message_library(std::string version, const Blob& blob)
    {
        return m_library.add_message_library(version, blob);
    }

    void STISClient::remove_message_library(std::string version)
    {
        return m_library.remove_message_library(version);
    }

    Blob STISClient::download_predefined_message_library(std::string version)
    {
        return m_library.download_predefined_message_library(version);
    }

    // ad hoc message library

    AdHocMessageMapPtr STISClient::load_ad_hoc_message_library()
    {
        return m_library.load_ad_hoc_message_library();
    }

    void STISClient::set_ad_hoc_message(int key, const std::string& title, const std::string& content)
    {
        m_library.set_ad_hoc_message(key, title, content);
    }

    void STISClient::delete_ad_hoc_message(int key)
    {
        m_library.delete_ad_hoc_message(key);
    }

    AdHocMessageItem STISClient::get_ad_hoc_message(int key)
    {
        return m_library.get_ad_hoc_message(key);
    }

    std::pair<std::string, bool> STISClient::lock_ad_hoc_message(int key)
    {
        return m_library.lock_ad_hoc_message(key);
    }

    void STISClient::unlock_ad_hoc_message(int key)
    {
        m_library.unlock_ad_hoc_message(key);
    }

    // display template library

    std::string STISClient::current_template_library_version()
    {
        return m_library.current_template_library_version();
    }

    std::string STISClient::next_template_library_version()
    {
        return m_library.next_template_library_version();
    }

    std::pair<std::string, std::string> STISClient::current_next_template_library_versions()
    {
        return m_library.current_next_template_library_versions();
    }

    void STISClient::set_current_template_library_version(std::string version)
    {
        return m_library.set_current_template_library_version(version);
    }

    void STISClient::set_next_template_library_version(std::string version)
    {
        return m_library.set_next_template_library_version(version);
    }

    void STISClient::upgrade_template_library_version(std::string version)
    {
        return m_library.upgrade_template_library_version(version);
    }

    bool STISClient::has_current_template_library()
    {
        return m_library.has_current_template_library();
    }

    bool STISClient::has_next_template_library()
    {
        return m_library.has_next_template_library();
    }

    bool STISClient::has_template_library(std::string version)
    {
        return m_library.has_template_library(version);
    }

    STSTMLIB_XML_PTR STISClient::load_current_template_library_xml()
    {
        return m_library.load_current_template_library_xml();
    }

    STSTMLIB_XML_PTR STISClient::load_next_template_library_xml()
    {
        return m_library.load_next_template_library_xml();
    }

    STSTMLIB_XML_PTR STISClient::load_template_library_xml(std::string version)
    {
        return m_library.load_template_library_xml(version);
    }

    ITemplateLibraryPtr STISClient::load_template_library()
    {
        return m_library.load_template_library();
    }

    void STISClient::add_template_library(std::string version, const Blob& blob)
    {
        return m_library.add_template_library(version, blob);
    }

    void STISClient::remove_template_library(std::string version)
    {
        return m_library.remove_template_library(version);
    }

    // display template

    const std::string& STISClient::get_lcd_template(const std::string& id)
    {
        return m_library.get_lcd_template(id);
    }

    const std::string& STISClient::get_lcd_emergency_template(const std::string& id)
    {
        return m_library.get_lcd_emergency_template(id);
    }

    const std::string& STISClient::get_led_template(const std::string& id)
    {
        return m_library.get_led_template(id);
    }

    const std::string& STISClient::get_led_emergency_template(const std::string& id)
    {
        return m_library.get_led_emergency_template(id);
    }

    Blob STISClient::download_display_template_library(std::string version)
    {
        return m_library.download_display_template_library(version);
    }

    std::map<path, Blob> STISClient::download_templates(const std::vector<path>& excludes)
    {
        return m_library.download_templates(excludes);
    }

    // both

    std::string STISClient::get_library_version(const std::string& category, std::string version)
    {
        return m_library.get_library_version(category, version);
    }

    void STISClient::set_library_version(const std::string& category, std::string version, const std::string& number)
    {
        m_library.set_library_version(category, version, number);
    }

    void STISClient::upgrade_library_version(const std::string& category, std::string version)
    {
        return m_library.upgrade_library_version(category, version);
    }

    bool STISClient::has_library(const std::string& category, std::string version)
    {
        return m_library.has_library(category, version);
    }

    Blob STISClient::get_library(const std::string& category, std::string version)
    {
        return m_library.get_library(category, version);
    }

    void STISClient::add_library(const std::string& category, std::string version, const Blob& blob)
    {
        return m_library.add_library(category, version, blob);
    }

    void STISClient::remove_library(const std::string& category, std::string version)
    {
        return m_library.remove_library(category, version);
    }

    // library download

    Blob STISClient::download_library(const std::string& category, std::string version)
    {
        return m_library.download_library(category, version);
    }

    // status

    AllCurNxtMsgTmpLibVers STISClient::get_this_station_iscs_stis_library_versions()
    {
        return m_library.get_this_station_iscs_stis_library_versions();
    }

    AllCurNxtMsgTmpLibVersInfoList STISClient::get_all_station_iscs_stis_library_versions()
    {
        return m_library.get_all_station_iscs_stis_library_versions();
    }

    // message

    void STISClient::submit_M10_DisplayPredefinedMessageRequest(const Destination& dest, const PredefinedMessage& msg)
    {
        m_message.submit_M10_DisplayPredefinedMessageRequest(dest, msg);
    }

    void STISClient::submit_M10_DisplayPredefinedMessageRequestList(const DestinationList& dests, const PredefinedMessage& msg)
    {
        m_message.submit_M10_DisplayPredefinedMessageRequestList(dests, msg);
    }

    void STISClient::submit_M11_DisplayAdHocMessageRequest(const Destination& dest, const AdHodMessage& msg)
    {
        m_message.submit_M11_DisplayAdHocMessageRequest(dest, msg);
    }

    void STISClient::submit_M11_DisplayAdHocMessageRequestList(const DestinationList& dests, const AdHodMessage& msg)
    {
        m_message.submit_M11_DisplayAdHocMessageRequestList(dests, msg);
    }

    void STISClient::submit_M20_ClearCurrentMessageRequest(const Destination& dest, const std::vector<int>& priority)
    {
        m_message.submit_M20_ClearCurrentMessageRequest(dest, priority);
    }

    void STISClient::submit_M20_ClearCurrentMessageRequestList(const DestinationList& dests, const std::vector<int>& priority)
    {
        m_message.submit_M20_ClearCurrentMessageRequestList(dests, priority);
    }

    void STISClient::submit_M21_PIDOnOffControlRequest(const Destination& dest, EPIDControlOn on, EPIDControlOff off)
    {
        return m_message.submit_M21_PIDOnOffControlRequest(dest, on, off);
    }

    void STISClient::submit_M22_SendPredefinedDisplayTemplateRequest(const Destination& dest, const PredefinedDisplayTemplate& lcd, const PredefinedDisplayTemplate& spare)
    {
        m_message.submit_M22_SendPredefinedDisplayTemplateRequest(dest, lcd, spare);
    }

    void STISClient::submit_M22_SendPredefinedDisplayTemplateRequestList(const DestinationList& dests, const PredefinedDisplayTemplate& lcd, const PredefinedDisplayTemplate& spare)
    {
        m_message.submit_M22_SendPredefinedDisplayTemplateRequestList(dests, lcd, spare);
    }

    void STISClient::submit_M23_RemoveDisplayTemplateRequest(const Destination& dest, EDisplayTemplateType type)
    {
        m_message.submit_M23_RemoveDisplayTemplateRequest(dest, type);
    }

    void STISClient::submit_M23_RemoveDisplayTemplateRequestList(const DestinationList& dests, EDisplayTemplateType type)
    {
        m_message.submit_M23_RemoveDisplayTemplateRequestList(dests, type);
    }

    void STISClient::submit_M24_ClearCurrentMessagesRequestByMessageTag(const Destination& dest, std::string msg_tag, std::string sessionId)
    {
        m_message.submit_M24_ClearCurrentMessagesRequestByMessageTag(dest, std::move(msg_tag), std::move(sessionId));
    }

    void STISClient::submit_M24_ClearCurrentMessagesRequestByMessageTagList(const DestinationList& dests, std::string msg_tag, std::string sessionId)
    {
        m_message.submit_M24_ClearCurrentMessagesRequestByMessageTagList(dests, std::move(msg_tag), std::move(sessionId));
    }

    void STISClient::submit_M25_SchedulePIDOnOffTimeSettingRequest(const Destination& dest, const std::string& monitorOffTime, const std::string& monitorOnTime)
    {
        m_message.submit_M25_SchedulePIDOnOffTimeSettingRequest(dest, monitorOffTime, monitorOnTime);
    }

    void STISClient::submit_M25_SchedulePIDOnOffTimeSettingRequestList(const DestinationList& dests, const std::string& monitorOffTime, const std::string& monitorOnTime)
    {
        m_message.submit_M25_SchedulePIDOnOffTimeSettingRequestList(dests, monitorOffTime, monitorOnTime);
    }

    void STISClient::submit_M21_PIDOnOffControlRequestList(const DestinationList& dests, EPIDControlOn on, EPIDControlOff off)
    {
        m_message.submit_M21_PIDOnOffControlRequestList(dests, on, off);
    }

    A30_StationSTISStatusReport STISClient::submit_M30_StationSTISStatusRequest(const std::string& station)
    {
        return m_message.submit_M30_StationSTISStatusRequest(station);
    }

    A31_OCCSTISStatusSyncReport STISClient::submit_M31_OCCSTISStatusSyncRequest(const std::string& destination_occ, const CurNxtMsgTmpLibVers& iscs_versions)
    {
        return m_message.submit_M31_OCCSTISStatusSyncRequest(destination_occ, iscs_versions);
    }

    A32_AllStationStatusDetails STISClient::submit_M32_AllStationSTISStatusRequest(const std::string& station)
    {
        return m_message.submit_M32_AllStationSTISStatusRequest(station);
    }

    A33_OCCSTISStatusSyncReport STISClient::submit_M33_OCCSTISStatusSyncRequest(const std::string& destination_occ, const CurNxtMsgTmpLibVers& iscs_versions)
    {
        return m_message.submit_M33_OCCSTISStatusSyncRequest(destination_occ, iscs_versions);
    }

    A50_CurrentDisplayMessageTemplateReport STISClient::submit_M50_CurrentDisplayMessageTemplateRequest(const std::string& station, const PID& pid)
    {
        return m_message.submit_M50_CurrentDisplayMessageTemplateRequest(station, pid);
    }

    A51_StationScheduledDisplayingMessageTemplateListReport STISClient::submit_M51_StationScheduledDisplayingMessageTemplateListRequest(const std::string& station, std::string sessionId)
    {
        return m_message.submit_M51_StationScheduledDisplayingMessageTemplateListRequest(station);
    }

    A52_StationScheduledDisplayingTemplateListReport STISClient::submit_M52_StationScheduledDisplayingTemplateListRequest(const std::string& station, std::string sessionId)
    {
        return m_message.submit_M52_StationScheduledDisplayingTemplateListRequest(station);
    }

    void STISClient::submit_M53_RemoveDisplayTemplateByTemplateIDRequest(const Destination& dest, const PredefinedDisplayTemplateList& display_template_list, std::string sessionId)
    {
        m_message.submit_M53_RemoveDisplayTemplateByTemplateIDRequest(dest, display_template_list, std::move(sessionId));
    }

    void STISClient::submit_M53_RemoveDisplayTemplateByTemplateIDRequestList(const DestinationList& dests, const std::vector<PredefinedDisplayTemplateList>& display_template_list_list, std::string sessionId)
    {
        m_message.submit_M53_RemoveDisplayTemplateByTemplateIDRequestList(dests, display_template_list_list, std::move(sessionId));
    }

    A70_UpgradePredefinedMessageDisplayTemplateLibraryReport STISClient::submit_M70_UpgradePredefinedMessageDisplayTemplateLibraryRequest(ELibraryType type, std::string version)
    {
        return m_message.submit_M70_UpgradePredefinedMessageDisplayTemplateLibraryRequest(type, version);
    }
}
