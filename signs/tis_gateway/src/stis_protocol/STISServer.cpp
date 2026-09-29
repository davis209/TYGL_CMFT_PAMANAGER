#include "pch.h"
#include "STISServer.h"
#include "STISTCPClient.h"
#include "SummaryAlarm.h"
#include "app/signs/tis_gateway/src/Exceptions.h"
#include "app/signs/common_library/src/stis_protocol/message_types/MessageDataTypes.h"
#include "bus/scada/common_library/src/CommonDefs.h"
#include "core/utility/src/base_ex/GenericServantCorbaDef.h"
#include "core/utility/src/base_ex/DebugUtilEx.h"
#include "core/utility/src/base_ex/ThrowException.h"
#include "core/utility/src/base_ex/LocationAccessFactoryEx.h"
#include "core/utility/src/core/StdEx.h"
#include "core/utility/src/core/StaticObject.h"
#include <functional>
#include <thread>

#define RPARAM_DEBUGAMESSAGEDETAILS "DebugAMessageDetails"

namespace TA_IRS_App::STIS_PROTOCOL::INTERFACES::stisserver::detail
{
    using namespace std::literals;
    using st::StaticObject;
    using TA_Base_Ex::ThisLocation;
    using namespace TA_Base_Core;
    using namespace TA_Base_Bus;
    using namespace STIS_PROTOCOL::IMPL;
    using namespace STIS_PROTOCOL::MESSAGE_DATA_TYPES;

    bool s_debug_a_message_details = false;

    template <class T>
    struct Responser
    {
        template <class PACKET_PTR>
        auto operator()(PACKET_PTR response) const
        {
            if (response)
            {
                if (response->template is_message<T>())
                {
                    if (auto a = response->template cast_to_ptr<T>())
                    {
                        if (m_callback)
                        {
                            m_callback(a);
                        }

                        LOGLARGESTRING_DEBUG_IF(s_debug_a_message_details, "\n%s", boost::trim_right_copy(a->dump_details()));
                        return std::move(*a);
                    }
                }
                else if (response->template is_message<A99>())
                {
                    auto nack = response->template cast_to_ptr<A99>();
                    LOG_ERROR("Responser[%s:%s]: A99: %s", T::ID, nack->SQN.str(), nack->Reason.str());
                }
            }

            if (m_callback)
            {
                m_callback({});
            }

            TA_THROW(TA_IRS_Core::TISGatewayException("STIS Communications error"));
        }

        std::function<void(std::shared_ptr<T>)> m_callback;
    };

    template <class T, class PACKET_PTR>
    auto operator|(PACKET_PTR response, const Responser<T>& checker)
    {
        return checker(std::move(response));
    }

    template <class T>
    Responser<T> responser;

    struct STISServer::Impl : GenericServantCorbaDef
    {
        using ThisClass = Impl;

        Impl(std::string options = "")
        {
            set_class_name("STISServer");
            parse_options(std::move(options));
            s_debug_a_message_details = RunParamsEx::is_true(RPARAM_DEBUGAMESSAGEDETAILS);
        }

        void parse_options(std::string options)
        {
            m_tcp.parse_options(std::move(options));
        }

        void start()
        {
            initialize();
            m_tcp.start();
            activate_servant_with_name(STIS_MESSAGE_SERVANT_NAME);
        }

        void stop()
        {
            m_tcp.stop();
            deactivate_servant();
        }

        // GenericServantCorbaDef

        BOOST_DESCRIBE_CLASS
        (
            ThisClass, (GenericServantCorbaDef),
            (
                // on_generic_corba_invoke
                submit_M10_DisplayPredefinedMessageRequest,
                submit_M10_DisplayPredefinedMessageRequestList,
                submit_M11_DisplayAdHocMessageRequest,
                submit_M11_DisplayAdHocMessageRequestList,
                submit_M20_ClearCurrentMessageRequest,
                submit_M20_ClearCurrentMessageRequestList,
                submit_M21_PIDOnOffControlRequest,
                submit_M21_PIDOnOffControlRequestList,
                submit_M22_SendPredefinedDisplayTemplateRequest,
                submit_M22_SendPredefinedDisplayTemplateRequestList,
                submit_M23_RemoveDisplayTemplateRequest,
                submit_M23_RemoveDisplayTemplateRequestList,
                submit_M24_ClearCurrentMessagesRequestByMessageTag,
                submit_M24_ClearCurrentMessagesRequestByMessageTagList,
                submit_M25_SchedulePIDOnOffTimeSettingRequest,
                submit_M25_SchedulePIDOnOffTimeSettingRequestList,
                submit_M53_RemoveDisplayTemplateByTemplateIDRequest,
                submit_M53_RemoveDisplayTemplateByTemplateIDRequestList,
                // on_generic_corba_invoke_return
                submit_M30_StationSTISStatusRequest,
                submit_M31_OCCSTISStatusSyncRequest,
                submit_M32_AllStationSTISStatusRequest,
                submit_M33_OCCSTISStatusSyncRequest,
                submit_M50_CurrentDisplayMessageTemplateRequest,
                submit_M51_StationScheduledDisplayingMessageTemplateListRequest,
                submit_M52_StationScheduledDisplayingTemplateListRequest,
                submit_M70_UpgradePredefinedMessageDisplayTemplateLibraryRequest,
                ), (), ()
        );

        virtual void on_generic_corba_invoke(std::string fn, std::string args) override
        {
            LOG_CORBA("on_generic_corba_invoke(): %s", fn);
            dispatch_generic_corba_invoke(this, std::move(fn), std::move(args));
        }

        virtual GenericServantCorbaDef::Result on_generic_corba_invoke_return(std::string fn, std::string args) override
        {
            LOG_CORBA("on_generic_corba_invoke_return(): %s", fn);
            return dispatch_generic_corba_invoke_return(this, std::move(fn), std::move(args));
        }

        // implementation

        void submit_M10_DisplayPredefinedMessageRequest(const Destination& dest, const PredefinedMessage& msg, std::string session = "")
        {
            m_tcp.send_message(M10{dest, msg})
                | responser<A10>
                ;
        }

        void submit_M10_DisplayPredefinedMessageRequestList(const DestinationList& dests, const PredefinedMessage& msg, std::string session = "")
        {
            for (auto& dest : dests)
            {
                submit_M10_DisplayPredefinedMessageRequest(dest, msg);
            }
        }

        void submit_M11_DisplayAdHocMessageRequest(const Destination& dest, const AdHodMessage& msg, std::string session = "")
        {
            m_tcp.send_message(M11{dest, msg})
                | responser<A10>
                ;
        }

        void submit_M11_DisplayAdHocMessageRequestList(const DestinationList& dests, const AdHodMessage& msg, std::string session = "")
        {
            for (auto& dest : dests)
            {
                submit_M11_DisplayAdHocMessageRequest(dest, msg);
            }
        }

        void submit_M20_ClearCurrentMessageRequest(const Destination& dest, const std::vector<int>& priority, std::string session = "")
        {
            m_tcp.send_message(M20{dest, priority})
                | responser<A20>
                ;
        }

        void submit_M20_ClearCurrentMessageRequestList(const DestinationList& dests, const std::vector<int>& priority, std::string session = "")
        {
            for (auto& dest : dests)
            {
                submit_M20_ClearCurrentMessageRequest(dest, priority);
            }
        }

        void submit_M21_PIDOnOffControlRequest(const Destination& dest, EPIDControlOn on, EPIDControlOff off, std::string session = "")
        {
            m_tcp.send_message(M21{dest, on, off})
                | responser<A21>
                ;
        }

        void submit_M21_PIDOnOffControlRequestList(const DestinationList& dests, EPIDControlOn on, EPIDControlOff off)
        {
            for (auto& dest : dests)
            {
                submit_M21_PIDOnOffControlRequest(dest, on, off);
            }
        }

        void submit_M22_SendPredefinedDisplayTemplateRequest(const Destination& dest, const PredefinedDisplayTemplate& lcd, const PredefinedDisplayTemplate& spare, std::string session = "")
        {
            m_tcp.send_message(M22{dest, lcd, spare})
                | responser<A22>
                ;
        }

        void submit_M22_SendPredefinedDisplayTemplateRequestList(const DestinationList& dests, const PredefinedDisplayTemplate& lcd, const PredefinedDisplayTemplate& spare, std::string session = "")
        {
            for (auto& dest : dests)
            {
                submit_M22_SendPredefinedDisplayTemplateRequest(dest, lcd, spare);
            }
        }

        void submit_M23_RemoveDisplayTemplateRequest(const Destination& dest, EDisplayTemplateType type, std::string session = "")
        {
            m_tcp.send_message(M23{dest, type})
                | responser<A23>
                ;
        }

        void submit_M23_RemoveDisplayTemplateRequestList(const DestinationList& dests, EDisplayTemplateType type, std::string session = "")
        {
            for (auto& dest : dests)
            {
                submit_M23_RemoveDisplayTemplateRequest(dest, type);
            }
        }

        void submit_M24_ClearCurrentMessagesRequestByMessageTag(const Destination& dest, std::string msg_tag, std::string session = "")
        {
            m_tcp.send_message(M24{dest, msg_tag})
                | responser<A24>
                ;
        }

        void submit_M24_ClearCurrentMessagesRequestByMessageTagList(const DestinationList& dests, std::string msg_tag, std::string session = "")
        {
            for (auto& dest : dests)
            {
                submit_M24_ClearCurrentMessagesRequestByMessageTag(dest, msg_tag);
            }
        }

        void submit_M25_SchedulePIDOnOffTimeSettingRequest(const Destination& dest, const std::string& monitorOffTime, const std::string& monitorOnTime, std::string session = "")
        {
            m_tcp.send_message(M25{dest, monitorOffTime, monitorOnTime})
                | responser<A25>
                ;
        }

        void submit_M25_SchedulePIDOnOffTimeSettingRequestList(const DestinationList& dests, const std::string& monitorOffTime, const std::string& monitorOnTime, std::string session = "")
        {
            for (auto& dest : dests)
            {
                submit_M25_SchedulePIDOnOffTimeSettingRequest(dest, monitorOffTime, monitorOnTime);
            }
        }

        A30_StationSTISStatusReport submit_M30_StationSTISStatusRequest(const std::string& station, std::string session = "")
        {
            return m_tcp.send_message(M30{station})
                | responser<A30>
                ;
        }

        A31_OCCSTISStatusSyncReport submit_M31_OCCSTISStatusSyncRequest(const std::string& destination_occ, const CurNxtMsgTmpLibVers& iscs_versions, std::string session = "")
        {
            return m_tcp.send_message(M31{destination_occ, iscs_versions})
                | responser<A31>
                ;
        }

        A32_AllStationStatusDetails submit_M32_AllStationSTISStatusRequest(const std::string& station, std::string session = "")
        {
            return m_tcp.send_message(M32{station})
                | responser<A32>
                ;
        }

        A33_OCCSTISStatusSyncReport submit_M33_OCCSTISStatusSyncRequest(const std::string& destination_occ, const CurNxtMsgTmpLibVers& iscs_versions, std::string session = "")
        {
            return m_tcp.send_message(M33{destination_occ, iscs_versions})
                | responser<A33>
                ;
        }

        A50_CurrentDisplayMessageTemplateReport submit_M50_CurrentDisplayMessageTemplateRequest(const std::string& station, const PID& pid, std::string session = "")
        {
            return m_tcp.send_message(M50{station, pid})
                | responser<A50>
                ;
        }

        A51_StationScheduledDisplayingMessageTemplateListReport submit_M51_StationScheduledDisplayingMessageTemplateListRequest(const std::string& station, std::string session = "")
        {
            return m_tcp.send_message_6(M51{station})
                | responser<A51>
                ;
        }

        A52_StationScheduledDisplayingTemplateListReport submit_M52_StationScheduledDisplayingTemplateListRequest(const std::string& station, std::string session = "")
        {
            return m_tcp.send_message_6(M52{station})
                | responser<A52>
                ;
        }

        void submit_M53_RemoveDisplayTemplateByTemplateIDRequest(const Destination& dest, const PredefinedDisplayTemplateList& display_template_list)
        {
            m_tcp.send_message(M53{dest, display_template_list})
                | responser<A53>
                ;
        }

        void submit_M53_RemoveDisplayTemplateByTemplateIDRequestList(const DestinationList& dests, const std::vector<PredefinedDisplayTemplateList>& display_template_list_list)
        {
            for (auto i = 0; i < dests.size(); ++i)
            {
                submit_M53_RemoveDisplayTemplateByTemplateIDRequest(dests[i], display_template_list_list[i]);
            }
        }

        A70_UpgradePredefinedMessageDisplayTemplateLibraryReport submit_M70_UpgradePredefinedMessageDisplayTemplateLibraryRequest(ELibraryType type, const std::string& version, std::string session = "")
        {
            return m_tcp.send_message(M70{type, version})
                | responser<A70>
                ;
        }

        void initialize()
        {
            std::call_once(m_once, [&]
            {
                if (ThisLocation::is_occ())
                {
                    responser<A33>.m_callback = SummaryAlarm{};
                }
                else
                {
                    responser<A30>.m_callback = SummaryAlarm{};
                }
            });
        }

        std::once_flag m_once;
        STISTCPClient m_tcp;
    };

    STISServer& STISServer::instance()
    {
        return StaticObject<STISServer>::instance();
    }

    STISServer::STISServer(std::string options)
        : m_impl(std::make_shared<Impl>(std::move(options)))
    {
    }

    void STISServer::parse_options(std::string options)
    {
        m_impl->parse_options(std::move(options));
    }

    void STISServer::start()
    {
        m_impl->start();
    }

    void STISServer::stop()
    {
        m_impl->stop();
    }

    void STISServer::submit_M10_DisplayPredefinedMessageRequest(const Destination& dest, const PredefinedMessage& msg)
    {
        m_impl->submit_M10_DisplayPredefinedMessageRequest(dest, msg);
    }

    void STISServer::submit_M10_DisplayPredefinedMessageRequestList(const DestinationList& dests, const PredefinedMessage& msg)
    {
        m_impl->submit_M10_DisplayPredefinedMessageRequestList(dests, msg);
    }

    void STISServer::submit_M11_DisplayAdHocMessageRequest(const Destination& dest, const AdHodMessage& msg)
    {
        m_impl->submit_M11_DisplayAdHocMessageRequest(dest, msg);
    }

    void STISServer::submit_M11_DisplayAdHocMessageRequestList(const DestinationList& dests, const AdHodMessage& msg)
    {
        m_impl->submit_M11_DisplayAdHocMessageRequestList(dests, msg);
    }

    void STISServer::submit_M20_ClearCurrentMessageRequest(const Destination& dest, const std::vector<int>& priority)
    {
        m_impl->submit_M20_ClearCurrentMessageRequest(dest, priority);
    }

    void STISServer::submit_M20_ClearCurrentMessageRequestList(const DestinationList& dests, const std::vector<int>& priority)
    {
        m_impl->submit_M20_ClearCurrentMessageRequestList(dests, priority);
    }

    void STISServer::submit_M21_PIDOnOffControlRequest(const Destination& dest, EPIDControlOn on, EPIDControlOff off)
    {
        return m_impl->submit_M21_PIDOnOffControlRequest(dest, on, off);
    }

    void STISServer::submit_M21_PIDOnOffControlRequestList(const DestinationList& dests, EPIDControlOn on, EPIDControlOff off)
    {
        m_impl->submit_M21_PIDOnOffControlRequestList(dests, on, off);
    }

    void STISServer::submit_M22_SendPredefinedDisplayTemplateRequest(const Destination& dest, const PredefinedDisplayTemplate& lcd, const PredefinedDisplayTemplate& spare)
    {
        m_impl->submit_M22_SendPredefinedDisplayTemplateRequest(dest, lcd, spare);
    }

    void STISServer::submit_M22_SendPredefinedDisplayTemplateRequestList(const DestinationList& dests, const PredefinedDisplayTemplate& lcd, const PredefinedDisplayTemplate& spare)
    {
        m_impl->submit_M22_SendPredefinedDisplayTemplateRequestList(dests, lcd, spare);
    }

    void STISServer::submit_M23_RemoveDisplayTemplateRequest(const Destination& dest, EDisplayTemplateType type)
    {
        m_impl->submit_M23_RemoveDisplayTemplateRequest(dest, type);
    }

    void STISServer::submit_M23_RemoveDisplayTemplateRequestList(const DestinationList& dests, EDisplayTemplateType type)
    {
        m_impl->submit_M23_RemoveDisplayTemplateRequestList(dests, type);
    }

    void STISServer::submit_M24_ClearCurrentMessagesRequestByMessageTag(const Destination& dest, std::string msg_tag)
    {
        m_impl->submit_M24_ClearCurrentMessagesRequestByMessageTag(dest, std::move(msg_tag));
    }

    void STISServer::submit_M24_ClearCurrentMessagesRequestByMessageTagList(const DestinationList& dests, std::string msg_tag)
    {
        m_impl->submit_M24_ClearCurrentMessagesRequestByMessageTagList(dests, std::move(msg_tag));
    }

    void STISServer::submit_M25_SchedulePIDOnOffTimeSettingRequest(const Destination& dest, const std::string& monitorOffTime, const std::string& monitorOnTime)
    {
        m_impl->submit_M25_SchedulePIDOnOffTimeSettingRequest(dest, monitorOffTime, monitorOnTime);
    }

    void STISServer::submit_M25_SchedulePIDOnOffTimeSettingRequestList(const DestinationList& dests, const std::string& monitorOffTime, const std::string& monitorOnTime)
    {
        m_impl->submit_M25_SchedulePIDOnOffTimeSettingRequestList(dests, monitorOffTime, monitorOnTime);
    }

    A30_StationSTISStatusReport STISServer::submit_M30_StationSTISStatusRequest(const std::string& station)
    {
        return m_impl->submit_M30_StationSTISStatusRequest(station);
    }

    A31_OCCSTISStatusSyncReport STISServer::submit_M31_OCCSTISStatusSyncRequest(const std::string& destination_occ, const CurNxtMsgTmpLibVers& iscs_versions)
    {
        return m_impl->submit_M31_OCCSTISStatusSyncRequest(destination_occ, iscs_versions);
    }

    A32_AllStationStatusDetails STISServer::submit_M32_AllStationSTISStatusRequest(const std::string& station)
    {
        return m_impl->submit_M32_AllStationSTISStatusRequest(station);
    }

    A33_OCCSTISStatusSyncReport STISServer::submit_M33_OCCSTISStatusSyncRequest(const std::string& destination_occ, const CurNxtMsgTmpLibVers& iscs_versions)
    {
        return m_impl->submit_M33_OCCSTISStatusSyncRequest(destination_occ, iscs_versions);
    }

    A50_CurrentDisplayMessageTemplateReport STISServer::submit_M50_CurrentDisplayMessageTemplateRequest(const std::string& station, const PID& pid)
    {
        return m_impl->submit_M50_CurrentDisplayMessageTemplateRequest(station, pid);
    }

    A51_StationScheduledDisplayingMessageTemplateListReport STISServer::submit_M51_StationScheduledDisplayingMessageTemplateListRequest(const std::string& station)
    {
        return m_impl->submit_M51_StationScheduledDisplayingMessageTemplateListRequest(station);
    }

    A52_StationScheduledDisplayingTemplateListReport STISServer::submit_M52_StationScheduledDisplayingTemplateListRequest(const std::string& station)
    {
        return m_impl->submit_M52_StationScheduledDisplayingTemplateListRequest(station);
    }

    void STISServer::submit_M53_RemoveDisplayTemplateByTemplateIDRequest(const Destination& dest, const PredefinedDisplayTemplateList& display_template_list)
    {
        m_impl->submit_M53_RemoveDisplayTemplateByTemplateIDRequest(dest, display_template_list);
    }

    void STISServer::submit_M53_RemoveDisplayTemplateByTemplateIDRequestList(const DestinationList& dests, const std::vector<PredefinedDisplayTemplateList>& display_template_list_list)
    {
        m_impl->submit_M53_RemoveDisplayTemplateByTemplateIDRequestList(dests, display_template_list_list);
    }

    A70_UpgradePredefinedMessageDisplayTemplateLibraryReport STISServer::submit_M70_UpgradePredefinedMessageDisplayTemplateLibraryRequest(ELibraryType type, const std::string& version)
    {
        return m_impl->submit_M70_UpgradePredefinedMessageDisplayTemplateLibraryRequest(type, version);
    }
}
