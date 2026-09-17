#include "pch.h"
#include "FireAbortMonitor.h"
#include "bus/pa/common/src/Utility.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/scada/datapoint_library/src/DataPointWriteRequest.h"
#include "core/utility/src/base_ex/EntityAccessFactoryEx.h"
#include "core/utility/src/base_ex/LocationAccessFactoryEx.h"
#include "core/utility/src/base_ex/DescriptionParametersEx.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/src/CommsMessageSender.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/StateUpdateMessageSender.h"
#include "core/message/types/PAAgentAlarms_MessageTypes.h"
#include "core/message/types/PAAgentAudit_MessageTypes.h"
#include "core/message/types/PAAgentComms_MessageTypes.h"
#include "core/message/types/PAAgentStateUpdate_MessageTypes.h"
#include "core/utility/src/base_ex/GenericServantCorbaDef.h"
#include "core/utility/src/base_ex/DebugUtilEx.h"
#include "core/utility/src/base_ex/ThrowException.h"
#include "core/utility/src/base_ex/DataPointUtil.h"
#include "core/utility/src/base_ex/LocationAccessFactoryEx.h"
#include "core/utility/src/base_ex/RunParamsEx.h"
#include "core/utility/src/core/StdEx.h"
#include "core/utility/src/base_ex/DAI.h"
#include "core/utility/src/core/StaticObject.h"
#include "core/utility/src/core/Map.h"
#include "core/utility/src/core/SimpleConditionVariable.h"
#include "core/utility/src/core/algorithm/strings.h"
#include "core/utilities/src/CallstackLogger.h"
#include <boost/hof.hpp>
#include <chrono>

namespace TA_IRS_App
{
    using namespace boost::hof;
    using namespace std::literals;
    using namespace std::chrono;
    using st::SimpleConditionVariable;
    using st::StaticObject;
    using TA_Base_Ex::DataPointUtil;
    using TA_Base_Ex::Location;
    using TA_Base_Ex::ThisLocation;
    using TA_Base_Ex::RunParamsEx;
    using TA_Base_Ex::ThisEntityData;
    using TA_Base_Ex::DescriptionParametersEx;
    using namespace TA_Base_Core;
    using namespace TA_Base_Bus;
    using namespace boost::program_options;

    using PAGatewayServerNamedObject = GenericServantCorbaDefNamedObject;

    struct FireAbortMonitor::Impl
    {
        Impl(std::string options = "")
        {
            parse_options(std::move(options));
        }

        ~Impl()
        {
            stop();
        }

        void parse_options(std::string options)
        {
            if (options == m_options)
            {
                return;
            }

            m_options = std::move(options);

            options_description desc;
            desc.add_options()
                ("client-call-timeout-seconds", value<size_t>()->default_value(5))
                ;

            stdex::set_value_from_options(m_options, desc)
                (m_timeout, "client-call-timeout-seconds")
                ;
        }

        void start()
        {
            initialize();
        }

        void stop()
        {
        }

        void initialize()
        {
            LOG_CALLSTACK("FireAbortMonitor::initialize");

            std::call_once(m_once, [&]
            {
                if (ThisLocation::is_occ())
                {
                    for (auto dp : DataPointUtil::instance().get_datapoints_if_istarts_and_icontains_with("OCC_", "dioFIRE-Abort"))
                    {
                        LOG_DEBUG("initialize(): add fire-abort datapoint %s", dp->getDataPointName());
                        auto location = Utility::get_location_name_from_datapoint_name(dp->getDataPointName());
                        m_fire_abort_datapoints.emplace(location, dp);
                        m_servers[location].set_names_and_object_timeout(DAI::get_station_pa_gateway_name(location), Utility::PAGATEWAY_SERVANT_NAME, m_timeout);
                    }
                }
                else
                {
                    auto location = ThisLocation::name();
                    auto datapoint_name = st2::format("%s.PA.FIRE.FIRE.dioFIRE-Abort", location);
                    DataPoint* dp = DataPointUtil::instance().get_datapoint_if_istarts_width(datapoint_name);;
                    m_fire_abort_datapoints[location] = dp;
                    m_servers[location].set_names_and_object_timeout(DAI::get_station_pa_gateway_name(location), Utility::PAGATEWAY_SERVANT_NAME, m_timeout);
                    LOG_DEBUG_IF(dp, "initialize(): add fire-abort datapoint %s", dp->getDataPointName());
                }

                DataPointUtil::instance().set_datapoint_write_request_callback(partial(&Impl::datapoint_write_request_callback)(this));
                m_audit_sender.reset(MessagePublicationManager::getInstance().getAuditMessageSender(PAAgentAudit::Context));
            });
        }

        void datapoint_write_request_callback(DataPointWriteRequest* request)
        {
            LOG_CALLSTACK("STISMessageServer::datapoint_write_request_callback");

            auto location = Utility::get_location_name_from_datapoint_name(request->getDataPoint()->getDataPointName());
            LOG_DEBUG("datapoint_write_request_callback(): location=%s, datapoint=%s, value=%d", location, request->getDataPoint()->getDataPointName(), request->getValue().getBoolean());

            if (auto dp = m_fire_abort_datapoints.get_value(location); dp->getDataPointName() == request->getDataPoint()->getDataPointName())
            {
                try
                {
                    LOG_DEBUG("datapoint_write_request_callback(): calling fireAbort to %s PaGateway for %s", location, dp->getDataPointName());
                    m_servers[location].corba_call("fireAbort", request->getSessionID());
                    submit_audit_message(PAAgentAudit::PaAbortFireCountdown, {"LocationName", Location::to_display_name(location)}, request->getSessionID());
                }
                catch (...)
                {
                    LOG_ERROR("datapoint_write_request_callback(): failed to call fireAbort() to %s PaGateway for %s", location, location, dp->getDataPointName());
                }
            }
        }

        void submit_audit_message(const MessageType& messageType, const DescriptionParametersEx& parameters, const std::string& sessionId)
        {
            LOG_INFO("submit_audit_message(): auditEvent - %s", messageType.getTypeName());

            bool submitted = false;
            std::string reason;

            try
            {
                m_audit_sender->sendAuditMessage(messageType,                            // Message Type
                                                 ThisEntityData::get()->getParent(),
                                                 parameters,                             // Description
                                                 "",                                     // Additional details
                                                 sessionId,                              // SessionID if applicable
                                                 "",                                     // AlarmID if an alarm associated with event
                                                 "",                                     // IncidentKey if incident associated with event
                                                 "");                                    // EventID of a parent event, used to link events

                submitted = true;
            }
            catch (TA_Base_Core::TransactiveException& ex)
            {
                reason = ex.what();
            }
            catch (...)
            {
                reason = "Unknown reason.";
            }

            LOG_ERROR_IF_NOT(submitted, "submit_audit_message(): Failed to submit audit message %s. %s\nParameters are: %s, SessionId=%s", messageType.getTypeKey(), reason, parameters.dump(), sessionId);
        }

        std::once_flag m_once;
        std::string m_options = "uninitialized";
        st::map<std::string, DataPoint*, st::CompareNoCase> m_fire_abort_datapoints;
        st::map<std::string, PAGatewayServerNamedObject, st::CompareNoCase> m_servers;
        size_t m_timeout = 5;
        AuditMessageSenderPtr m_audit_sender;
    };

    FireAbortMonitor& FireAbortMonitor::instance()
    {
        return StaticObject<FireAbortMonitor>::instance();
    }

    FireAbortMonitor::FireAbortMonitor(std::string options)
        : m_impl(std::make_shared<Impl>(std::move(options)))
    {
    }

    void FireAbortMonitor::parse_options(std::string options)
    {
        m_impl->parse_options(std::move(options));
    }

    void FireAbortMonitor::start()
    {
        m_impl->start();
    }

    void FireAbortMonitor::stop()
    {
        m_impl->stop();
    }
}
