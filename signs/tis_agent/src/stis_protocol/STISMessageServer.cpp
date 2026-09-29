#include "pch.h"
#include "STISMessageServer.h"
#include "app/signs/common_library/src/stis_protocol/STISMessageClient.h"
#include "app/signs/common_library/src/STISUtility.h"
#include "app/signs/common_library/src/STISAuditMessage.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/scada/datapoint_library/src/DataPointWriteRequest.h"
#include "core/message/types/TISAudit_MessageTypes.h"
#include "core/utility/src/base_ex/DAI.h"
#include "core/utility/src/base_ex/GenericServantCorbaDef.h"
#include "core/utility/src/base_ex/DebugUtilEx.h"
#include "core/utility/src/core/StaticObject.h"
#include "core/utility/src/base_ex/DataPointUtil.h"
#include "core/utility/src/base_ex/LocationAccessFactoryEx.h"

namespace TA_IRS_App::STIS_PROTOCOL::INTERFACES::stismessageserver::detail
{
    using namespace boost::hof;
    using st::StaticObject;
    using TA_Base_Ex::DataPointUtil;
    using TA_Base_Ex::LocationEx;
    using namespace TA_Base_Core;
    using namespace TA_Base_Bus;

    using STISMessageServerNamedObject = GenericServantCorbaDefNamedObject;

    struct STISMessageServer::Impl : GenericServantCorbaDef
    {
        Impl(std::string options = "")
        {
            set_class_name("STISMessageServer");
            parse_options(std::move(options));
        }

        void start()
        {
            initialize();
            activate_servant_with_name(STIS_MESSAGE_SERVANT_NAME);
        }

        void stop()
        {
            deactivate_servant();
        }

        void parse_options(std::string options)
        {
            m_options = std::move(options);
            m_gateway.set_names_and_object_timeout(DAI::get_local_tis_gateway_name(), STIS_MESSAGE_SERVANT_NAME, 60);
        }

        // forward corba calls

        virtual void generic_invoke(const char* arg, const char* args) override
        {
            m_gateway.corba_call_forward(arg, args);
        }

        virtual char* generic_invoke_return(const char* arg, const char* args) override
        {
            return m_gateway.corba_call_return_forward(arg, args);
        }

        virtual void generic_async(const char* arg, const char* args) override
        {
            m_gateway.corba_async_forward(arg, args);
        }

        // implementations

        void initialize()
        {
            LOG_CALLSTACK("STISMessageServer::initialize");

            std::call_once(m_once, [&]
            {
                DataPointUtil::instance().set_datapoint_write_request_callback(partial(&Impl::datapoint_write_request_callback)(this));
            });
        }

        void datapoint_write_request_callback(DataPointWriteRequest* request)
        {
            LOG_CALLSTACK("STISMessageServer::datapoint_write_request_callback");

            if (auto pid = STIS_UTILITY::PID::from_entity_name(request->getDataPoint()->getDataPointName()))
            {
                auto session = request->getSessionID();

                auto value = request->getValue().getValueAsString();
                auto control_on = boost::iequals(value, "ON") ? EPIDControlOn::ControlOn : EPIDControlOn::NoAction;
                auto control_off = boost::iequals(value, "ON") ? EPIDControlOff::NoAction : EPIDControlOff::ControlOff;

                Destination dest;
                dest.station_id = pid.station;
                dest.pid_list.emplace_back(pid.id);
                m_gateway_message_client.submit_M21_PIDOnOffControlRequest(dest, control_on, control_off, session);

                STISAuditMessage::send(STISAudit::ChangePIDStatus,
                                       {
                                           {"Location", LocationEx::to_display_name(pid.station)},
                                           {"PID", pid.id},
                                           {"on/off", value},
                                       },
                                       session);
            }
        }

        std::once_flag m_once;
        std::string m_options;
        STISMessageServerNamedObject m_gateway;
        STISMessageClient m_gateway_message_client{"--server=local-tis-gateway"};
    };

    STISMessageServer& STISMessageServer::instance()
    {
        return StaticObject<STISMessageServer>::value();
    }

    STISMessageServer::STISMessageServer(std::string options)
        : m_impl(std::make_shared<Impl>(std::move(options)))
    {
    }

    void STISMessageServer::start()
    {
        m_impl->start();
    }

    void STISMessageServer::stop()
    {
        m_impl->stop();
    }

    void STISMessageServer::parse_options(std::string options)
    {
        m_impl->parse_options(options);
    }
}
