/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File$
 * @author:  San Teo
 * @version: $Revision$
 *
 * Last modification: $DateTime$
 * Last modified by:  $Author$
 *
 * This class captures and performs the following control requests via the run params:
 * 1. Display the current message on a specific PID
 * 2. Turning a PID on/off
 * 3. Lock/unlock a PID.
 *
 */

#include "stdafx.h"
#include "RightsManager.h"
#include "PidSelectionManager.h"
#include "CurrentDisplayMessagesDlg.h"
#include "app/signs/common_library/src/STISAuditMessage.h"
#include "app/signs/common_library/src/STISUtility.h"
#include "core/types/src/ta_types.h"
#include "app/signs/stis_manager/src/helperfun.h"
#include "app/signs/stis_manager/src/PIDController.h"
#include "app/signs/stis_manager/src/GraphworxComms.h"
#include "app/signs/stis_manager/src/UserMessages.h"
#include "app/signs/stis_manager/src/TisAgentInterface.h"
#include "bus/signs_4669/tis_agent_access/src/TISAgentAccessFactory.h"
#include "bus/generic_gui/src/TransactiveMessage.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/TISAudit_MessageTypes.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/naming/src/NamingMacros.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/utility/src/core/StdEx.h"
//#include "boost/tokenizer.hpp"
#include <iomanip>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_Core;
using namespace TA_Base_Bus;
using TA_Base_Bus::TISAgentAccessFactory;
using TA_IRS_App::STIS_PROTOCOL::MESSAGE_TYPES::Destination;
using TA_IRS_App::STIS_PROTOCOL::MESSAGE_TYPES::EPIDControlOn;
using TA_IRS_App::STIS_PROTOCOL::MESSAGE_TYPES::EPIDControlOff;
using TA_IRS_App::STIS_PROTOCOL::MESSAGE_TYPES::A50_CurrentDisplayMessageTemplateReport;
using PID = TA_IRS_App::STIS_UTILITY::PID;

namespace
{
    const std::string RPARAM_GETMESSAGE = "GetMessage";
    const std::string RPARAM_VIEWMESSAGE = "ViewMessage";
    const std::string RPARAM_STATE = "State";
    const std::string RPARAM_LOCK = "Lock";
    const std::string SWITCH_ON_PID = "ON";
    const std::string LOCK_PID = "ON";
    const std::string EMPTY_MESSAGE = "There is no Display message in selected PID";
}

namespace TA_IRS_App
{
    // This macro catches all exceptions and display the suitable user message
#define CATCH_ALL_EXCEPTIONS(action)                                                                                \
        catch (TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException&)                                 \
        {                                                                                                           \
            LOG_EXCEPTION("ISTISManagerCorbaDef::STISServerNotConnectedException", "" );                            \
            UserMessages::getInstance().displayError(                                                               \
                str( format(UserMessages::ERROR_REQUEST_FAILED)                                                     \
                % action % "TIS Agent not connected to STIS Server" ).c_str());                                     \
        }                                                                                                           \
        catch (TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationTimeoutException&)                               \
        {                                                                                                           \
            LOG_EXCEPTION("ISTISManagerCorbaDef::STISCommunicationTimeoutException", "" );                          \
            UserMessages::getInstance().displayError(                                                               \
                str( format(UserMessages::ERROR_REQUEST_FAILED)                                                     \
                % action % "Timed out while waiting for STIS server response" ).c_str());                           \
        }                                                                                                           \
        catch (TA_Base_Bus::ISTISManagerCorbaDef::STISFunctionalityNotAvailableException&)                          \
        {                                                                                                           \
            LOG_EXCEPTION("ISTISManagerCorbaDef::STISFunctionalityNotAvailableException", "" );                     \
            UserMessages::getInstance().displayError(                                                               \
                str( format(UserMessages::ERROR_REQUEST_FAILED)                                                     \
                % action % "Function not available at this location" ).c_str());                                    \
        }                                                                                                           \
        catch (TA_Base_Bus::ISTISManagerCorbaDef::STISInvalidParameterException& ex)                                \
        {                                                                                                           \
            LOG_EXCEPTION("ISTISManagerCorbaDef::STISInvalidParameterException", ex.details.in());                  \
            UserMessages::getInstance().displayError(                                                               \
                str( format(UserMessages::ERROR_REQUEST_FAILED)                                                     \
                % action % ex.details ).c_str());                                                                   \
        }                                                                                                           \
        catch ( const TA_Base_Core::ObjectResolutionException& ore )                                                \
        {                                                                                                           \
            LOG_EXCEPTION("TA_Base_Core::ObjectResolutionException", ore.what() );                                  \
            UserMessages::getInstance().displayError(                                                               \
                str( format(UserMessages::ERROR_REQUEST_FAILED)                                                     \
                % action % "Could not resolve TIS Agent" ).c_str());                                                \
        }                                                                                                           \
        catch ( const CORBA::Exception& ce )                                                                        \
        {                                                                                                           \
            LOG_EXCEPTION("CORBA::Exception",                                                                       \
                CorbaUtil::exceptionToString( ce ) );                                                               \
            UserMessages::getInstance().displayError(                                                               \
                str( format(UserMessages::ERROR_REQUEST_FAILED)                                                     \
                % action % "Could not resolve TIS Agent" ).c_str());                                                \
        }                                                                                                           \
        catch (const TA_Base_Core::DataException& e)                                                                \
        {                                                                                                           \
            LOG_EXCEPTION("Data Exception", e.what());                                                              \
            UserMessages::getInstance().displayError(                                                               \
                str( format(UserMessages::ERROR_REQUEST_FAILED)                                                     \
                % action % "Database is not configured correctly" ).c_str());                                       \
        }                                                                                                           \
        catch (const TA_Base_Core::DatabaseException& e)                                                            \
        {                                                                                                           \
            LOG_EXCEPTION("Database Exception", e.what());                                                          \
            UserMessages::getInstance().displayError(                                                               \
                str( format(UserMessages::ERROR_REQUEST_FAILED)                                                     \
                % action % "Failed to connect to database" ).c_str());                                              \
        }                                                                                                           \
        catch ( ... )                                                                                               \
        {                                                                                                           \
            LOG_EXCEPTION("...", action);                                                                           \
            UserMessages::getInstance().displayError(                                                               \
                str( format(UserMessages::ERROR_REQUEST_FAILED)                                                     \
                % action % "Could not resolve TIS Agent" ).c_str());                                                \
        }
    // end of CATCH_ALL_EXCEPTIONS

    PIDController::PIDController()
    {
    }

    void PIDController::initialise()
    {
        FUNCTION_ENTRY("initialise");

        // Register for the PID selection/deselection
        RunParams::getInstance().registerRunParamUser(this, RPARAM_GETMESSAGE);
        RunParams::getInstance().registerRunParamUser(this, RPARAM_VIEWMESSAGE);
        RunParams::getInstance().registerRunParamUser(this, RPARAM_STATE);
        RunParams::getInstance().registerRunParamUser(this, RPARAM_LOCK);

        for (auto&& param : {RPARAM_GETMESSAGE, RPARAM_VIEWMESSAGE, RPARAM_STATE, RPARAM_LOCK})
        {
            if (auto val = RunParams::getInstance().get(param); val.size())
            {
                onRunParamChange(param, val);
            }
        }

        FUNCTION_EXIT;
    }

    void PIDController::onRunParamChange(const std::string& name, const std::string& value)
    {
        FUNCTION_ENTRY("onRunParamChange");
        std::string runParamReceived("");

        // Break up into the header elements and the PID list
        // The value will be prefixed with Station, XPos, pid, for all three runparams.
        // It will also contain x and y position for GetMessage;
        std::vector<std::string> valueParts = tokenizeString(value, ",");
        std::string station = valueParts[0];
        std::string entityName = valueParts[2];

        // Check for basic validity of GetMessage/State/Lock param,
        // based on the number of items in the value

        if (st::any_of_iequal({RPARAM_GETMESSAGE, RPARAM_VIEWMESSAGE, RPARAM_STATE, RPARAM_LOCK}, name) && valueParts.size() < 3)
        {
            LOG_ERROR("Invalid %s RunParam given to STIS Manager: %s", name, value);
            FUNCTION_EXIT;
            return;
        }

        if (st::any_of_iequal({RPARAM_GETMESSAGE, RPARAM_VIEWMESSAGE}, name))
        {
            getCurrentMessage(station, entityName, std::stoi(valueParts[1]), value);
            FUNCTION_EXIT;
            return;
        }

        if (valueParts.size() < 4)
        {
            LOG_ERROR("Invalid %s RunParam given to STIS Manager: %s", name, value);
            FUNCTION_EXIT;
            return;
        }

        std::string& val = valueParts[3];

        if (boost::iequals(name, RPARAM_STATE))
        {
            changeState(entityName, val.compare(SWITCH_ON_PID) == 0);
        }
        else // RPARAM_LOCK
        {
            lock(entityName, val.compare(LOCK_PID) == 0);

            //TD17783
            // get the location name
            std::stringstream locationKeyStream;
            locationKeyStream << RunParams::getInstance().get(RPARAM_LOCATIONKEY);
            std::uint32_t locationKey = 0;
            locationKeyStream >> locationKey;

            TA_Base_Core::ILocation* location = TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(locationKey);

            // insert it into the map
            std::string locationName = location->getName();

            TA_Base_Core::DescriptionParameters desc;
            TA_Base_Core::NameValuePair pidNameVp("PID", entityName);
            TA_Base_Core::NameValuePair locationVp("location", locationName);
            TA_Base_Core::NameValuePair status("on/off", (val.compare(LOCK_PID) == 0) ? "lock" : "release");

            desc.push_back(&pidNameVp);
            desc.push_back(&locationVp);
            desc.push_back(&status);

            auto auditMessageSender = MessagePublicationManager::getInstance().getAuditMessageSender(TA_Base_Core::TISAudit::Context);

            std::string entityName = RunParams::getInstance().get(RPARAM_ENTITYNAME);
            TA_Base_Core::IEntityData* guiEntity = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityName);
            TA_ASSERT(guiEntity != NULL, "EntityAccessFactory returned a NULL entity and yet did not throw an exception.");

#if 0
            auditMessageSender->sendAuditMessage(TA_Base_Core::TISAudit::STISPidStatusChanged, guiEntity->getKey(),
                                                 desc,
                                                 "", // Further description text
                                                 RunParams::getInstance().get(RPARAM_SESSIONID),
                                                 "", // alarm ID - not required
                                                 "", // incident key - not required
                                                 ""); // event key - not required
#endif

            delete location;
            delete auditMessageSender;
            //TD17783
        }

        FUNCTION_EXIT;
    }

    void PIDController::getCurrentMessage(const std::string& station, const std::string& name, const ta_int32 displayId, const std::string& displayCmd)
    {
        std::string action = "Get current message of " + name + " at " + station;

        try
        {
            // get the current message from agent then get graphworx to display the appropriate dialog
            // using the information that we got from the agent
            auto report = STISClient::instance().submit_M50_CurrentDisplayMessageTemplateRequest(station, PID::from_entity_name(name).id);
            CurrentDisplayMessagesDlg::show_dialog(std::make_shared<decltype(report)>(std::move(report)));
#if 0
            //++libo

            auto content = mesg.message_text;
            LOG_INFO("The message being shown on %s is %s with priority of %d", name, content, mesg.message_priority);
            LOG_INFO_IF(content.size(), "Message content: %s", content);

            if (content.size())
            {
                std::string startTimeDate;
                std::string endTimeDate;

                if (content != EMPTY_MESSAGE)
                {
                    startTimeDate = timeDateString(mesg.message_start_time);
                    endTimeDate = timeDateString(mesg.message_end_time);
                }
                else
                {
                    content = "";
                }

                bool ok = GraphworxComms::getInstance().displayCurrentMessage(displayCmd, content, std::to_string(mesg.message_priority), displayId, startTimeDate, endTimeDate);

                if (!ok)
                {
                    LOG_INFO(action);
                    UserMessages::getInstance().displayError(
                        str(format(UserMessages::ERROR_REQUEST_FAILED)
                            % action % "Failed to display current message").c_str());
                }
            }
            else
            {
                LOG_ERROR("Error while retrieve the current display message. Invalid message from STIS");
            }
#endif
        }

        CATCH_ALL_EXCEPTIONS(action.c_str());
    }

    void PIDController::changeState(const std::string& name, const bool on)
    {
        FUNCTION_ENTRY("changeState");

        //TD17901 Avteam++
        if (!RightsManager::getInstance().canPIDControl())
        {
            TransActiveMessage::show(IDS_UE_020047, "ChangState");
            FUNCTION_EXIT;
            return;
        }

        Destination dest;
        auto pid = PID::from_entity_name(name);
        dest.station_id = pid.station;
        dest.pid_list.emplace_back(pid.id);
        //dest.pid_list = iter->first.c_str();

        std::string action = "Turn ";
        action += (on ? "on " : "off ");
        action += name;

        EPIDControlOn controlOn = on ? EPIDControlOn::ControlOn : EPIDControlOn::NoAction;
        EPIDControlOff controlOff = on ? EPIDControlOff::NoAction : EPIDControlOff::ControlOff;

        try
        {
            STISClient::instance().submit_M21_PIDOnOffControlRequest(dest, controlOn, controlOff);
            //TISAgentAccessFactory::getInstance().getSTISAgentAtLocation(getStationName(name))->submitPIDControlRequest(name.c_str(), on ? TA_Base_Core::TURN_ON : TA_Base_Core::TURN_OFF, RunParams::getInstance().get(RPARAM_SESSIONID).c_str());
            LOG_INFO(action);
            STISAuditMessage::changePIDStatus(dest.station_id, boost::algorithm::join(dest.pid_list, ","), action);
        }

        CATCH_ALL_EXCEPTIONS(action.c_str());

        FUNCTION_EXIT;
    }

    void PIDController::lock(const std::string& name, const bool on)
    {
        FUNCTION_ENTRY("lock");

        //TD17901 Avteam++
        bool canPIDControl = RightsManager::getInstance().canPIDControl();

        if (canPIDControl == FALSE)
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            CString actionName = "鎖定";
            userMsg << actionName;
            userMsg.showMsgBox(IDS_UE_020047, "旅客資訊管理器");
        }
        else
        {
            std::string action = (on ? "鎖定 " : "解鎖 ");
            action += name;

            try
            {
                TISAgentAccessFactory::getInstance().getSTISAgentAtLocation(STIS_UTILITY::PID::from_entity_name(name).station)->setLockStatus(name.c_str(), on, RunParams::getInstance().get(RPARAM_SESSIONID).c_str());
                LOG_INFO(action);
            }

            CATCH_ALL_EXCEPTIONS(action.c_str());
        }

        FUNCTION_EXIT;
    }

    std::string PIDController::timeDateString(const std::string& timeString)
    {
        std::string year = timeString.substr(0, 4);
        std::string month = timeString.substr(4, 2);
        std::string day = timeString.substr(6, 2);
        std::string time = timeString.substr(8, 6);
        std::string date = day + "/" + month + "/" + year;
        time.insert(2, ":");
        time.insert(5, ":");
        time.insert(8, " ");
        return time + date;
    }
}
