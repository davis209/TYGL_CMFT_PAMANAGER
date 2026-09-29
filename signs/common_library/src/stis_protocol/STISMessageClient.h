#pragma once
#include "CommonDefs.h"
#include "app/signs/common_library/src/stis_protocol/message_types/MessageTypes.h"

namespace TA_IRS_App::STIS_PROTOCOL::INTERFACES::stismessageclient::detail
{
    struct STISMessageClient
    {
        static STISMessageClient& instance();

        STISMessageClient(std::string options = "");

        void parse_options(std::string options);
        bool is_server_online();
        void set_session_id(std::string sessionId);

        void submit_M10_DisplayPredefinedMessageRequest(const Destination& dest, const PredefinedMessage& msg, std::string sessionId = "");
        void submit_M10_DisplayPredefinedMessageRequestList(const DestinationList& dests, const PredefinedMessage& msg, std::string sessionId = "");

        void submit_M11_DisplayAdHocMessageRequest(const Destination& dest, const AdHodMessage& msg, std::string sessionId = "");
        void submit_M11_DisplayAdHocMessageRequestList(const DestinationList& dests, const AdHodMessage& msg, std::string sessionId = "");

        void submit_M20_ClearCurrentMessageRequest(const Destination& dest, const std::vector<int>& priority, std::string sessionId = "");
        void submit_M20_ClearCurrentMessageRequestList(const DestinationList& dests, const std::vector<int>& priority, std::string sessionId = "");

        void submit_M21_PIDOnOffControlRequest(const Destination& dest, EPIDControlOn on, EPIDControlOff off, std::string sessionId = "");
        void submit_M21_PIDOnOffControlRequestList(const DestinationList& dests, EPIDControlOn on, EPIDControlOff off, std::string sessionId = "");

        void submit_M22_SendPredefinedDisplayTemplateRequest(const Destination& dest, const PredefinedDisplayTemplate& lcd, const PredefinedDisplayTemplate& spare, std::string sessionId = "");
        void submit_M22_SendPredefinedDisplayTemplateRequestList(const DestinationList& dests, const PredefinedDisplayTemplate& lcd, const PredefinedDisplayTemplate& spare, std::string sessionId = "");

        void submit_M23_RemoveDisplayTemplateRequest(const Destination& dest, EDisplayTemplateType type, std::string sessionId = "");
        void submit_M23_RemoveDisplayTemplateRequestList(const DestinationList& dests, EDisplayTemplateType type, std::string sessionId = "");

        void submit_M24_ClearCurrentMessagesRequestByMessageTag(const Destination& dest, std::string msg_tag, std::string sessionId = "");
        void submit_M24_ClearCurrentMessagesRequestByMessageTagList(const DestinationList& dests, std::string msg_tag, std::string sessionId = "");

        void submit_M25_SchedulePIDOnOffTimeSettingRequest(const Destination& dest, const std::string& monitorOffTime, const std::string& monitorOnTime, std::string sessionId = "");
        void submit_M25_SchedulePIDOnOffTimeSettingRequestList(const DestinationList& dests, const std::string& monitorOffTime, const std::string& monitorOnTime, std::string sessionId = "");

        A30_StationSTISStatusReport submit_M30_StationSTISStatusRequest(const std::string& station, std::string sessionId = "");
        A31_OCCSTISStatusSyncReport submit_M31_OCCSTISStatusSyncRequest(const std::string& destination_occ, const CurNxtMsgTmpLibVers& iscs_versions, std::string sessionId = "");
        A32_AllStationStatusDetails submit_M32_AllStationSTISStatusRequest(const std::string& station, std::string sessionId = "");
        A33_OCCSTISStatusSyncReport submit_M33_OCCSTISStatusSyncRequest(const std::string& destination_occ, const CurNxtMsgTmpLibVers& iscs_versions, std::string sessionId = "");
        A50_CurrentDisplayMessageTemplateReport submit_M50_CurrentDisplayMessageTemplateRequest(const std::string& station, const PID& pid, std::string sessionId = "");
        A51_StationScheduledDisplayingMessageTemplateListReport submit_M51_StationScheduledDisplayingMessageTemplateListRequest(const std::string& station, std::string sessionId = "");
        A52_StationScheduledDisplayingTemplateListReport submit_M52_StationScheduledDisplayingTemplateListRequest(const std::string& station, std::string sessionId = "");
        void submit_M53_RemoveDisplayTemplateByTemplateIDRequest(const Destination& dest, const PredefinedDisplayTemplateList& display_template_list, std::string sessionId = "");
        void submit_M53_RemoveDisplayTemplateByTemplateIDRequestList(const DestinationList& dests, const std::vector<PredefinedDisplayTemplateList>& display_template_list_list, std::string sessionId = "");
        A70_UpgradePredefinedMessageDisplayTemplateLibraryReport submit_M70_UpgradePredefinedMessageDisplayTemplateLibraryRequest(ELibraryType type, const std::string& version, std::string sessionId = "");

        struct Impl;
        std::shared_ptr<Impl> m_impl;
    };

    using STISMessageClientPtr = std::shared_ptr<STISMessageClient>;
}

namespace TA_IRS_App::STIS_PROTOCOL::INTERFACES
{
    using stismessageclient::detail::STISMessageClient;
    using stismessageclient::detail::STISMessageClientPtr;
}

namespace TA_IRS_App::STIS_PROTOCOL
{
    using namespace INTERFACES;
}
