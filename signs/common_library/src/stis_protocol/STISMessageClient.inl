#include "STISMessageClient.h"
#include "core/utility/src/base_ex/DAI.h"
#include "core/utility/src/base_ex/GenericServantCorbaDef.h"
#include "core/utility/src/base_ex/DebugUtilEx.h"
#include "core/utility/src/base_ex/RunParamsEx.h"
#include "core/utility/src/core/StdEx.h"

namespace TA_IRS_App::STIS_PROTOCOL::INTERFACES::stismessageclient::detail
{
    using namespace TA_Base_Ex;
    using namespace TA_Base_Core;
    using namespace std::string_literals;
    using namespace boost::program_options;

    using STISMessageServerNamedObject = GenericServantCorbaDefNamedObject;

    struct STISMessageClient::Impl
    {
        Impl(std::string options = "default")
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

        A32_AllStationStatusDetails submit_M32_AllStationSTISStatusRequest(const std::string& station, std::string sessionId)
        {
            auto args = std::tie(station, get_session_id(sessionId));
            return m_server.corba_call_return("submit_M32_AllStationSTISStatusRequest", args);
        }

        A50_CurrentDisplayMessageTemplateReport submit_M50_CurrentDisplayMessageTemplateRequest(const std::string& station, const PID& pid, std::string sessionId)
        {
            auto args = std::tie(station, pid, get_session_id(sessionId));
            return m_server.corba_call_return("submit_M50_CurrentDisplayMessageTemplateRequest", args);
        }

        A31_OCCSTISStatusSyncReport submit_M31_OCCSTISStatusSyncRequest(const std::string& destination_occ, const CurNxtMsgTmpLibVers& iscs_versions, std::string sessionId)
        {
            auto args = std::tie(destination_occ, iscs_versions, get_session_id(sessionId));
            return m_server.corba_call_return("submit_M31_OCCSTISStatusSyncRequest", args);
        }

        A70_UpgradePredefinedMessageDisplayTemplateLibraryReport submit_M70_UpgradePredefinedMessageDisplayTemplateLibraryRequest(ELibraryType type, const std::string& version, std::string sessionId)
        {
            auto args = std::tie(type, version, get_session_id(sessionId));
            return m_server.corba_call_return("submit_M70_UpgradePredefinedMessageDisplayTemplateLibraryRequest", args);
        }

    public: // implementation

        void parse_options(std::string options)
        {
            if (!m_server.empty() && (m_options == options))
            {
                return;
            }

            m_options = std::move(options);

            options_description desc;
            desc.add_options()
            ("server", value<std::string>()->default_value("local-tis-agent"))
            ("client-call-timeout-seconds", value<size_t>()->default_value(2))
            ("session-id", value<std::string>()->default_value(RPARAM_SESSIONID_v))
            ;

            std::string server;
            size_t timeout = 2;

            stdex::set_value_from_options(m_options, desc)
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

        std::string m_options;
        std::string m_session;
        STISMessageServerNamedObject m_server;
    };
}
