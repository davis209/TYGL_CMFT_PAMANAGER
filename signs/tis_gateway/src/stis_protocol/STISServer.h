#pragma once
#include "app/signs/common_library/src/stis_protocol/message_types/MessageTypes.h"
#include <string>
#include <vector>
#include <memory>

namespace TA_IRS_App::STIS_PROTOCOL::INTERFACES::stisserver::detail
{
    using namespace STIS_PROTOCOL::MESSAGE_TYPES;

    struct STISServer
    {
        static STISServer& instance();

        STISServer(std::string options = "");

        void parse_options(std::string options);

        void start();
        void stop();

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

        void submit_M24_ClearCurrentMessagesRequestByMessageTag(const Destination& dest, std::string msg_tag);
        void submit_M24_ClearCurrentMessagesRequestByMessageTagList(const DestinationList& dests, std::string msg_tag);

        void submit_M25_SchedulePIDOnOffTimeSettingRequest(const Destination& dest, const std::string& monitorOffTime, const std::string& monitorOnTime);
        void submit_M25_SchedulePIDOnOffTimeSettingRequestList(const DestinationList& dests, const std::string& monitorOffTime, const std::string& monitorOnTime);

        A30_StationSTISStatusReport submit_M30_StationSTISStatusRequest(const std::string& station);
        A31_OCCSTISStatusSyncReport submit_M31_OCCSTISStatusSyncRequest(const std::string& destination_occ, const CurNxtMsgTmpLibVers& iscs_versions);
        A32_AllStationStatusDetails submit_M32_AllStationSTISStatusRequest(const std::string& station);
        A33_OCCSTISStatusSyncReport submit_M33_OCCSTISStatusSyncRequest(const std::string& destination_occ, const CurNxtMsgTmpLibVers& iscs_versions);
        A50_CurrentDisplayMessageTemplateReport submit_M50_CurrentDisplayMessageTemplateRequest(const std::string& station, const PID& pid);
        A51_StationScheduledDisplayingMessageTemplateListReport submit_M51_StationScheduledDisplayingMessageTemplateListRequest(const std::string& station);
        A52_StationScheduledDisplayingTemplateListReport submit_M52_StationScheduledDisplayingTemplateListRequest(const std::string& station);
        void submit_M53_RemoveDisplayTemplateByTemplateIDRequest(const Destination& dest, const PredefinedDisplayTemplateList& display_template_list);
        void submit_M53_RemoveDisplayTemplateByTemplateIDRequestList(const DestinationList& dests, const std::vector<PredefinedDisplayTemplateList>& display_template_list_list);
        A70_UpgradePredefinedMessageDisplayTemplateLibraryReport submit_M70_UpgradePredefinedMessageDisplayTemplateLibraryRequest(ELibraryType type, const std::string& version);

        struct Impl;
        std::shared_ptr<Impl> m_impl;
    };
}

namespace TA_IRS_App::STIS_PROTOCOL::INTERFACES
{
    using stisserver::detail::STISServer;
}

namespace TA_IRS_App::STIS_PROTOCOL
{
    using namespace INTERFACES;
}
