#include "pch.h"
#include "STISMessageClient.h"
#include "core/utility/src/base_ex/DAI.h"
#include "core/utility/src/base_ex/GenericServantCorbaDef.h"
#include "core/utility/src/base_ex/DebugUtilEx.h"
#include "core/utility/src/base_ex/RunParamsEx.h"
#include "core/utility/src/core/StdEx.h"

using namespace TA_Base_Ex;
using namespace TA_Base_Core;
using namespace std::string_literals;
using namespace boost::program_options;

using STISMessageServerNamedObject = GenericServantCorbaDefNamedObject;

namespace TA_IRS_App::STIS_PROTOCOL::INTERFACES::stismessageclient::detail
{
    struct STISMessageClient::Impl
    {
        Impl(std::string options)
        {
            parse_options(std::move(options));
        }

        bool is_server_online()
        {
            return m_server.is_online();
        }

        void set_session_id(std::string sessionId)
        {
            m_session = std::move(sessionId);
        }

        void submit_M10_DisplayPredefinedMessageRequest(const Destination& dest, const PredefinedMessage& msg, std::string sessionId)
        {
            auto args = std::tie(dest, msg, get_session_id(sessionId));
            m_server.corba_call("submit_M10_DisplayPredefinedMessageRequest", args);
        }

        void submit_M10_DisplayPredefinedMessageRequestList(const DestinationList& dests, const PredefinedMessage& msg, std::string sessionId)
        {
            auto args = std::tie(dests, msg, get_session_id(sessionId));
            m_server.corba_call("submit_M10_DisplayPredefinedMessageRequestList", args);
        }

        void submit_M11_DisplayAdHocMessageRequest(const Destination& dest, const AdHodMessage& msg, std::string sessionId)
        {
            auto args = std::tie(dest, msg, get_session_id(sessionId));
            m_server.corba_call("submit_M11_DisplayAdHocMessageRequest", args);
        }

        void submit_M11_DisplayAdHocMessageRequestList(const DestinationList& dests, const AdHodMessage& msg, std::string sessionId)
        {
            auto args = std::tie(dests, msg, get_session_id(sessionId));
            m_server.corba_call("submit_M11_DisplayAdHocMessageRequestList", args);
        }

        void submit_M20_ClearCurrentMessageRequest(const Destination& dest, const std::vector<int>& priority, std::string sessionId)
        {
            auto args = std::tie(dest, priority, get_session_id(sessionId));
            m_server.corba_call("submit_M20_ClearCurrentMessageRequest", args);
        }

        void submit_M20_ClearCurrentMessageRequestList(const DestinationList& dests, const std::vector<int>& priority, std::string sessionId)
        {
            auto args = std::tie(dests, priority, get_session_id(sessionId));
            m_server.corba_call("submit_M20_ClearCurrentMessageRequestList", args);
        }

        void submit_M22_SendPredefinedDisplayTemplateRequest(const Destination& dest, const PredefinedDisplayTemplate& lcd, const PredefinedDisplayTemplate& spare, std::string sessionId)
        {
            auto args = std::tie(dest, lcd, spare, get_session_id(sessionId));
            m_server.corba_call("submit_M22_SendPredefinedDisplayTemplateRequest", args);
        }

        void submit_M22_SendPredefinedDisplayTemplateRequestList(const DestinationList& dests, const PredefinedDisplayTemplate& lcd, const PredefinedDisplayTemplate& spare, std::string sessionId)
        {
            auto args = std::tie(dests, lcd, spare, get_session_id(sessionId));
            m_server.corba_call("submit_M22_SendPredefinedDisplayTemplateRequestList", args);
        }

        void submit_M23_RemoveDisplayTemplateRequest(const Destination& dest, EDisplayTemplateType type, std::string sessionId)
        {
            auto args = std::tie(dest, type, get_session_id(sessionId));
            m_server.corba_call("submit_M23_RemoveDisplayTemplateRequest", args);
        }

        void submit_M23_RemoveDisplayTemplateRequestList(const DestinationList& dests, EDisplayTemplateType type, std::string sessionId)
        {
            auto args = std::tie(dests, type, get_session_id(sessionId));
            m_server.corba_call("submit_M23_RemoveDisplayTemplateRequestList", args);
        }

        void submit_M24_ClearCurrentMessagesRequestByMessageTag(const Destination& dest, std::string msg_tag, std::string sessionId)
        {
            auto args = std::tie(dest, msg_tag, get_session_id(sessionId));
            m_server.corba_call("submit_M24_ClearCurrentMessagesRequestByMessageTag", args);
        }

        void submit_M24_ClearCurrentMessagesRequestByMessageTagList(const DestinationList& dests, std::string msg_tag, std::string sessionId)
        {
            auto args = std::tie(dests, msg_tag, get_session_id(sessionId));
            m_server.corba_call("submit_M24_ClearCurrentMessagesRequestByMessageTagList", args);
        }

        void submit_M25_SchedulePIDOnOffTimeSettingRequest(const Destination& dest, const std::string& monitorOffTime, const std::string& monitorOnTime, std::string sessionId)
        {
            auto args = std::tie(dest, monitorOffTime, monitorOnTime, get_session_id(sessionId));
            m_server.corba_call("submit_M25_SchedulePIDOnOffTimeSettingRequest", args);
        }

        void submit_M25_SchedulePIDOnOffTimeSettingRequestList(const DestinationList& dests, const std::string& monitorOffTime, const std::string& monitorOnTime, std::string sessionId)
        {
            auto args = std::tie(dests, monitorOffTime, monitorOnTime, get_session_id(sessionId));
            m_server.corba_call("submit_M25_SchedulePIDOnOffTimeSettingRequestList", args);
        }

        void submit_M21_PIDOnOffControlRequest(const Destination& dest, EPIDControlOn on, EPIDControlOff off, std::string sessionId)
        {
            auto args = std::tie(dest, on, off, get_session_id(sessionId));
            m_server.corba_call("submit_M21_PIDOnOffControlRequest", args);
        }

        void submit_M21_PIDOnOffControlRequestList(const DestinationList& dests, EPIDControlOn on, EPIDControlOff off, std::string sessionId)
        {
            auto args = std::tie(dests, on, off, get_session_id(sessionId));
            m_server.corba_call("submit_M21_PIDOnOffControlRequestList", args);
        }

        A30_StationSTISStatusReport submit_M30_StationSTISStatusRequest(const std::string& station, std::string sessionId)
        {
            auto args = std::tie(station, get_session_id(sessionId));
            return m_server.corba_call_return("submit_M30_StationSTISStatusRequest", args);
        }

        A31_OCCSTISStatusSyncReport submit_M31_OCCSTISStatusSyncRequest(const std::string& destination_occ, const CurNxtMsgTmpLibVers& iscs_versions, std::string sessionId)
        {
            auto args = std::tie(destination_occ, iscs_versions, get_session_id(sessionId));
            return m_server.corba_call_return("submit_M31_OCCSTISStatusSyncRequest", args);
        }

        A32_AllStationStatusDetails submit_M32_AllStationSTISStatusRequest(const std::string& station, std::string sessionId)
        {
            auto args = std::tie(station, get_session_id(sessionId));
            return m_server.corba_call_return("submit_M32_AllStationSTISStatusRequest", args);
        }

        A33_OCCSTISStatusSyncReport submit_M33_OCCSTISStatusSyncRequest(const std::string& destination_occ, const CurNxtMsgTmpLibVers& iscs_versions, std::string sessionId)
        {
            auto args = std::tie(destination_occ, iscs_versions, get_session_id(sessionId));
            return m_server.corba_call_return("submit_M33_OCCSTISStatusSyncRequest", args);
        }

        A50_CurrentDisplayMessageTemplateReport submit_M50_CurrentDisplayMessageTemplateRequest(const std::string& station, const PID& pid, std::string sessionId)
        {
            auto args = std::tie(station, pid, get_session_id(sessionId));
            return m_server.corba_call_return("submit_M50_CurrentDisplayMessageTemplateRequest", args);
        }

        A51_StationScheduledDisplayingMessageTemplateListReport submit_M51_StationScheduledDisplayingMessageTemplateListRequest(const std::string& station, std::string sessionId)
        {
            auto args = std::tie(station, get_session_id(sessionId));
            return m_server.corba_call_return("submit_M51_StationScheduledDisplayingMessageTemplateListRequest", args);
        }

        A52_StationScheduledDisplayingTemplateListReport submit_M52_StationScheduledDisplayingTemplateListRequest(const std::string& station, std::string sessionId)
        {
            auto args = std::tie(station, get_session_id(sessionId));
            return m_server.corba_call_return("submit_M52_StationScheduledDisplayingTemplateListRequest", args);
        }

        void submit_M53_RemoveDisplayTemplateByTemplateIDRequest(const Destination& dest, const PredefinedDisplayTemplateList& display_template_list, std::string sessionId)
        {
            auto args = std::tie(dest, display_template_list, get_session_id(sessionId));
            m_server.corba_call("submit_M53_RemoveDisplayTemplateByTemplateIDRequest", args);
        }

        void submit_M53_RemoveDisplayTemplateByTemplateIDRequestList(const DestinationList& dests, const std::vector<PredefinedDisplayTemplateList>& display_template_list_list, std::string sessionId)
        {
            auto args = std::tie(dests, display_template_list_list, get_session_id(sessionId));
            m_server.corba_call("submit_M53_RemoveDisplayTemplateByTemplateIDRequestList", args);
        }

        A70_UpgradePredefinedMessageDisplayTemplateLibraryReport submit_M70_UpgradePredefinedMessageDisplayTemplateLibraryRequest(ELibraryType type, const std::string& version, std::string sessionId)
        {
            auto args = std::tie(type, version, get_session_id(sessionId));
            return m_server.corba_call_return("submit_M70_UpgradePredefinedMessageDisplayTemplateLibraryRequest", args);
        }

    public: // implementation

        void parse_options(std::string options)
        {
            if (m_options == options)
            {
                return;
            }

            m_options = std::move(options);

            options_description desc;
            desc.add_options()
                ("server", value<std::string>()->default_value("local-tis-agent"))
                ("session-id", value<std::string>())
                ("client-call-timeout-seconds", value<size_t>()->default_value(60))
                ;

            std::string server;
            size_t timeout = 60;

            st::set_value_from_options(m_options, desc)
                (server, "server")
                (timeout, "client-call-timeout-seconds")
                (m_session, "session-id")
                ;

            m_server.set_names_and_object_timeout(DAI::get_name_from_string(server), STIS_MESSAGE_SERVANT_NAME, timeout);
        }

        const std::string& get_session_id(std::string& sessionId) const
        {
            return sessionId.size() ? sessionId : m_session;
        }

        std::string m_options = "uninitialized";
        std::string m_session = RPARAM_SESSIONID_v;
        STISMessageServerNamedObject m_server;
    };

    STISMessageClient& STISMessageClient::instance()
    {
        static auto s_instance = new STISMessageClient{};
        return *s_instance;
    }

    STISMessageClient::STISMessageClient(std::string options)
        : m_impl(std::make_shared<Impl>(std::move(options)))
    {
    }

    void STISMessageClient::parse_options(std::string options)
    {
        return m_impl->parse_options(std::move(options));
    }

    bool STISMessageClient::is_server_online()
    {
        return m_impl->is_server_online();
    }

    void STISMessageClient::set_session_id(std::string sessionId)
    {
        return m_impl->set_session_id(std::move(sessionId));
    }

    void STISMessageClient::submit_M10_DisplayPredefinedMessageRequest(const Destination& dest, const PredefinedMessage& msg, std::string sessionId)
    {
        m_impl->submit_M10_DisplayPredefinedMessageRequest(dest, msg, std::move(sessionId));
    }

    void STISMessageClient::submit_M10_DisplayPredefinedMessageRequestList(const DestinationList& dests, const PredefinedMessage& msg, std::string sessionId)
    {
        m_impl->submit_M10_DisplayPredefinedMessageRequestList(dests, msg, std::move(sessionId));
    }

    void STISMessageClient::submit_M11_DisplayAdHocMessageRequest(const Destination& dest, const AdHodMessage& msg, std::string sessionId)
    {
        m_impl->submit_M11_DisplayAdHocMessageRequest(dest, msg, std::move(sessionId));
    }

    void STISMessageClient::submit_M11_DisplayAdHocMessageRequestList(const DestinationList& dests, const AdHodMessage& msg, std::string sessionId)
    {
        m_impl->submit_M11_DisplayAdHocMessageRequestList(dests, msg, std::move(sessionId));
    }

    void STISMessageClient::submit_M20_ClearCurrentMessageRequest(const Destination& dest, const std::vector<int>& priority, std::string sessionId)
    {
        m_impl->submit_M20_ClearCurrentMessageRequest(dest, priority, std::move(sessionId));
    }

    void STISMessageClient::submit_M20_ClearCurrentMessageRequestList(const DestinationList& dests, const std::vector<int>& priority, std::string sessionId)
    {
        m_impl->submit_M20_ClearCurrentMessageRequestList(dests, priority, std::move(sessionId));
    }

    void STISMessageClient::submit_M21_PIDOnOffControlRequest(const Destination& dest, EPIDControlOn on, EPIDControlOff off, std::string sessionId)
    {
        return m_impl->submit_M21_PIDOnOffControlRequest(dest, on, off, std::move(sessionId));
    }

    void STISMessageClient::submit_M21_PIDOnOffControlRequestList(const DestinationList& dests, EPIDControlOn on, EPIDControlOff off, std::string sessionId)
    {
        m_impl->submit_M21_PIDOnOffControlRequestList(dests, on, off, std::move(sessionId));
    }

    void STISMessageClient::submit_M22_SendPredefinedDisplayTemplateRequest(const Destination& dest, const PredefinedDisplayTemplate& lcd, const PredefinedDisplayTemplate& led, std::string sessionId)
    {
        m_impl->submit_M22_SendPredefinedDisplayTemplateRequest(dest, lcd, led, std::move(sessionId));
    }

    void STISMessageClient::submit_M22_SendPredefinedDisplayTemplateRequestList(const DestinationList& dests, const PredefinedDisplayTemplate& lcd, const PredefinedDisplayTemplate& led, std::string sessionId)
    {
        m_impl->submit_M22_SendPredefinedDisplayTemplateRequestList(dests, lcd, led, std::move(sessionId));
    }

    void STISMessageClient::submit_M23_RemoveDisplayTemplateRequest(const Destination& dest, EDisplayTemplateType type, std::string sessionId)
    {
        m_impl->submit_M23_RemoveDisplayTemplateRequest(dest, type, std::move(sessionId));
    }

    void STISMessageClient::submit_M23_RemoveDisplayTemplateRequestList(const DestinationList& dests, EDisplayTemplateType type, std::string sessionId)
    {
        m_impl->submit_M23_RemoveDisplayTemplateRequestList(dests, type, std::move(sessionId));
    }

    void STISMessageClient::submit_M24_ClearCurrentMessagesRequestByMessageTag(const Destination& dest, std::string msg_tag, std::string sessionId)
    {
        m_impl->submit_M24_ClearCurrentMessagesRequestByMessageTag(dest, std::move(msg_tag), std::move(sessionId));
    }

    void STISMessageClient::submit_M24_ClearCurrentMessagesRequestByMessageTagList(const DestinationList& dests, std::string msg_tag, std::string sessionId)
    {
        m_impl->submit_M24_ClearCurrentMessagesRequestByMessageTagList(dests, std::move(msg_tag), std::move(sessionId));
    }

    void STISMessageClient::submit_M25_SchedulePIDOnOffTimeSettingRequest(const Destination& dest, const std::string& monitorOffTime, const std::string& monitorOnTime, std::string sessionId)
    {
        m_impl->submit_M25_SchedulePIDOnOffTimeSettingRequest(dest, monitorOffTime, monitorOnTime, std::move(sessionId));
    }

    void STISMessageClient::submit_M25_SchedulePIDOnOffTimeSettingRequestList(const DestinationList& dests, const std::string& monitorOffTime, const std::string& monitorOnTime, std::string sessionId)
    {
        m_impl->submit_M25_SchedulePIDOnOffTimeSettingRequestList(dests, monitorOffTime, monitorOnTime, std::move(sessionId));
    }

    A30_StationSTISStatusReport STISMessageClient::submit_M30_StationSTISStatusRequest(const std::string& station, std::string sessionId)
    {
        return m_impl->submit_M30_StationSTISStatusRequest(station, std::move(sessionId));
    }

    A31_OCCSTISStatusSyncReport STISMessageClient::submit_M31_OCCSTISStatusSyncRequest(const std::string& destination_occ, const CurNxtMsgTmpLibVers& iscs_versions, std::string sessionId)
    {
        return m_impl->submit_M31_OCCSTISStatusSyncRequest(destination_occ, iscs_versions, std::move(sessionId));
    }

    A32_AllStationStatusDetails STISMessageClient::submit_M32_AllStationSTISStatusRequest(const std::string& station, std::string sessionId)
    {
        return m_impl->submit_M32_AllStationSTISStatusRequest(station, std::move(sessionId));
    }

    A33_OCCSTISStatusSyncReport STISMessageClient::submit_M33_OCCSTISStatusSyncRequest(const std::string& destination_occ, const CurNxtMsgTmpLibVers& iscs_versions, std::string sessionId)
    {
        return m_impl->submit_M33_OCCSTISStatusSyncRequest(destination_occ, iscs_versions, std::move(sessionId));
    }

    A50_CurrentDisplayMessageTemplateReport STISMessageClient::submit_M50_CurrentDisplayMessageTemplateRequest(const std::string& station, const PID& pid, std::string sessionId)
    {
        return m_impl->submit_M50_CurrentDisplayMessageTemplateRequest(station, pid, std::move(sessionId));
    }

    A51_StationScheduledDisplayingMessageTemplateListReport STISMessageClient::submit_M51_StationScheduledDisplayingMessageTemplateListRequest(const std::string& station, std::string sessionId)
    {
        return m_impl->submit_M51_StationScheduledDisplayingMessageTemplateListRequest(station, std::move(sessionId));
    }

    A52_StationScheduledDisplayingTemplateListReport STISMessageClient::submit_M52_StationScheduledDisplayingTemplateListRequest(const std::string& station, std::string sessionId)
    {
        return m_impl->submit_M52_StationScheduledDisplayingTemplateListRequest(station, std::move(sessionId));
    }

    void STISMessageClient::submit_M53_RemoveDisplayTemplateByTemplateIDRequest(const Destination& dest, const PredefinedDisplayTemplateList& display_template_list, std::string sessionId)
    {
        m_impl->submit_M53_RemoveDisplayTemplateByTemplateIDRequest(dest, display_template_list, std::move(sessionId));
    }

    void STISMessageClient::submit_M53_RemoveDisplayTemplateByTemplateIDRequestList(const DestinationList& dests, const std::vector<PredefinedDisplayTemplateList>& display_template_list_list, std::string sessionId)
    {
        m_impl->submit_M53_RemoveDisplayTemplateByTemplateIDRequestList(dests, display_template_list_list, std::move(sessionId));
    }

    A70_UpgradePredefinedMessageDisplayTemplateLibraryReport STISMessageClient::submit_M70_UpgradePredefinedMessageDisplayTemplateLibraryRequest(ELibraryType type, const std::string& version, std::string sessionId)
    {
        return m_impl->submit_M70_UpgradePredefinedMessageDisplayTemplateLibraryRequest(type, version, std::move(sessionId));
    }
}
