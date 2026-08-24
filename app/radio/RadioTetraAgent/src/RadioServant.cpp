/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/RadioServant.cpp $
  * @author  A. Parker
  * @version $Revision: #7 $
  * Last modification : $DateTime: 2009/11/25 09:18:00 $
  * Last modified by : $Author: builder $
  *
  * This class implements the radio servant which provides the required servant and radio interfaces
  * The radio interfaces simply delegate to the Radio class to enable it to be unit tested without
  * all the servant requirements.
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif
#include "core/types/src/ta_types.h"
#include <fstream>
#include <iomanip>
#include <ios>
static char * UNKNOWN_EXCEPTION = "Unknown exception";

#include "stdio.h"
#include "Radio.h"
#include "RadioServant.h"
#include "RadioSessionException.h"
#include "RadioSession.h"
#include "RadioStatusSummary.h"
#include "core/naming/src/NamedObject.h"
#include "core/naming/src/Naming.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/message/src/CommsMessageSender.h"
#include "core/message/types/RadioComms_MessageTypes.h"
#include "core/message/types/ControlStationComms_MessageTypes.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/data_access_interface/entity_access/src/IRadioEntityData.h"
#include "core/data_access_interface/entity_access/src/TrainAgentEntityData.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/message/types/RadioAlarms_MessageTypes.h"
#include "core/utilities/src/RunParams.h"
#include "core/process_management/IDL/src/IControlStationCorbaDef.h"
#include "core/message/types/RadioAudit_MessageTypes.h"

#include "bus/radio/RadioAgentCorbaDef_4669/idl/src/IRadioSessionCorbaDef.h"
#include "bus/trains/TrainCorbaProxy/src/TrainAgentProxyFactory.h"
#include "bus/trains/TrainCommonLibrary/src/AgentCommunicationException.h"


#include "app/radio/RadioTetraAgent/src/RadioCorbaTypeConverter.h"
#include "app/radio/RadioTetraAgent/src/RadioSubscriberMonitor.h"
#include "bus/generic_agent/src/GenericAgent.h"

using namespace TA_Base_Core;
using namespace TA_IRS_App;



// Forward declarations

TA_Base_Bus::IRadioCorbaDef::ECallStatusType convertCallStatus(TA_IRS_App::ECallStatusType status);
TA_Base_Bus::IRadioCorbaDef::ECallCategory   convertCallCategory(TA_IRS_App::ECallCategory category);
TA_Base_Bus::IRadioCorbaDef::ECallType       convertCallType(TA_IRS_App::ECallType callType);
TA_Base_Bus::IRadioCorbaDef::ECallSource     convertCallSource(TA_IRS_App::ECallSource source);
TA_Base_Bus::IRadioCorbaDef::EPTTState       convertPTTState(TA_IRS_App::EPTTState pttState);
TA_Base_Bus::IRadioCorbaDef::EOriginType	 convertOriginType(TA_IRS_App::EOriginType originType);
TA_Base_Bus::IRadioCorbaDef::EStackCallType  convertStackCallType(TA_IRS_App::EStackCallType stackCallType);

RadioServant::RadioServant( /*TA_Base_Bus::ItaScada& scada*/ )
    : m_radio(NULL)
    , m_active(false)
    //, m_scada(scada)
	, m_auditMessageSender(NULL)
	, m_flexibleTimeout(100)
{
	FUNCTION_ENTRY("Constructor");
	
	m_serverLinkStatusThread = new RadioServerLinkStatusUpdateThread();

	m_radio = new Radio(m_serverLinkStatusThread, *this/*, m_scada*/);
	
	//m_serverLinkStatusThread->start();//limin

	FUNCTION_EXIT;
}

RadioServant::~RadioServant() 
{
	FUNCTION_ENTRY("Destructor"); 
	deactivateServant();
	delete m_radio;

	m_serverLinkStatusThread->terminateAndWait();
	delete m_serverLinkStatusThread;

	TA_Base_Core::AlarmHelperManager::getInstance().cleanUp();

	// TD13048
	if (m_auditMessageSender != NULL)
	{
		delete m_auditMessageSender;
		m_auditMessageSender = NULL;
	}

	FUNCTION_EXIT;
}

void RadioServant::setEntity(TA_Base_Core::IEntityData& EntityData, TA_Base_Core::RadioGlobalEntityData& globalEntity)
{
	FUNCTION_ENTRY("setEntity");
    LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "setEntity(%s)", EntityData.getDescription().c_str());

    m_objectName      = EntityData.getName();
	m_EntityKey       = EntityData.getKey();
	m_entityLocation  = EntityData.getLocation();
	m_entitySubsystem = EntityData.getSubsystem();
	m_entityTypeKey   = EntityData.getTypeKey();

	TA_Base_Core::IRadioEntityData* entity = dynamic_cast<TA_Base_Core::IRadioEntityData*>(&EntityData);
	TA_ASSERT(entity!=NULL,"Entity data incorrect type.");

    // now read global parameters

	// obtain the asset name
	m_assetName = entity->getAssetName();
	m_radio->setAgentAssetName(m_assetName);

    ta_uint32 xonXoffTimeout = 140;

    try
    {    
        xonXoffTimeout = globalEntity.getXonXoffTimeout();
    }
    catch ( const TA_Base_Core::TransactiveException& e )
    {
        LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "TA_Base_Core::TransactiveException", e.what() );
    }
    
    ta_uint32 circuitDataFramesPerPacket = 1;

    try
    {    
        circuitDataFramesPerPacket = globalEntity.getCircuitDataFramesPerPacket();
    }
    catch ( const TA_Base_Core::TransactiveException& e )
    {
        LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "TA_Base_Core::TransactiveException", e.what() );
    }

    unsigned char serialiseTransactionLevel = 9;
    try
    {
        serialiseTransactionLevel = globalEntity.getSerialiseTransactionLevel();
    }
    catch ( const TA_Base_Core::TransactiveException& e )
    {
        LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "TA_Base_Core::TransactiveException", e.what() );
    }

    unsigned long radioTransactionTimeout = 45;
    try
    {
        radioTransactionTimeout = globalEntity.getRadioTransactionTimeout();
    }
    catch ( const TA_Base_Core::TransactiveException& e )
    {
		LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "TA_Base_Core::TransactiveException", e.what() );
    }

    m_radio->setParameters( entity->getPrimaryTCPServers(),
                            entity->getSecondaryTCPServers(),
							entity->getDefaultVolume(),
                            serialiseTransactionLevel,
                            radioTransactionTimeout,
                            xonXoffTimeout,
                            circuitDataFramesPerPacket );

	m_radio->setEntity(EntityData);

	m_auditMessageSender = TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender(TA_Base_Core::RadioAudit::Context); // TD13048
	TA_ASSERT(m_auditMessageSender!=NULL, "Cannot create audit message sender");

	FUNCTION_EXIT;
}

//****************************
// IEntity Interface Methods *
//****************************

bool RadioServant::isValid(void)
{
	FUNCTION_ENTRY("isValid");
	FUNCTION_EXIT;
	return true;
}

void RadioServant::start(void)
{
	FUNCTION_ENTRY("start");
	m_radio->start();
	activateServantWithName(m_objectName);
	FUNCTION_EXIT;
}

void RadioServant::stop(void)
{
	FUNCTION_ENTRY("stop");
	deactivateServant();
	FUNCTION_EXIT;
}

void RadioServant::update(const class TA_Base_Core::ConfigUpdateDetails &details)
{
	FUNCTION_ENTRY("update");
	FUNCTION_EXIT;
}

void RadioServant::remove(void)
{
	FUNCTION_ENTRY("remove");
	FUNCTION_EXIT;
}

void RadioServant::setToControlMode(void)
{
	FUNCTION_ENTRY("setToControlMode");
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Setting radio servant %s to Control mode",m_objectName.c_str());
	m_active = true;	
	m_radio->setToControlMode();
	FUNCTION_EXIT;
}

void RadioServant::setToMonitorMode(void)
{
	FUNCTION_ENTRY("setToMonitorMode");
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Setting radio servant %s to Monitor mode",m_objectName.c_str());
	m_active = false;
	m_radio->setToMonitorMode();
	FUNCTION_EXIT;
}


void RadioServant::onTerminate(bool peerAvailable)
{
    FUNCTION_ENTRY("setToMonitorMode");
	m_radio->onTerminate(peerAvailable);
	FUNCTION_EXIT;
}


void RadioServant::cancelSendData(const char* ITSI)
{
	TA_Base_Bus::GenericAgent::ensureControlMode();
    try
    {
	    m_radio->getRadioCircuitModeManager()->removeRadioCircuitTransfer( ITSI );
    }
    catch( const RadioCircuitModeException& rce )
    {
        throw TA_Base_Bus::IRadioCorbaDef::dataCallNotInProgressException();
    }
}

// Call Driver Methods

void RadioServant::addCallDriverRequest(unsigned char trainID,const char* trainTSI)
{
	FUNCTION_ENTRY("addCallDriverRequest");
	m_radio->addCallDriverRequest(trainID,trainTSI);
	FUNCTION_EXIT;			
}

void RadioServant::removeCallDriverRequest(unsigned char trainID)
{
	FUNCTION_ENTRY("removeCallDriverRequest");
	m_radio->removeCallDriverRequest(trainID);
	FUNCTION_EXIT;			
}

bool RadioServant::inFallback()
{
    return m_radio->inFallback();
}


void RadioServant::resetConnection()
{
    m_radio->resetConnection();
}

void RadioServant::onCallStatusChange(const TA_IRS_App::CallDetailsType& details)
{
	FUNCTION_ENTRY("onCallStatusChange");

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"onCallStatusChange()begin");

	if (m_active)
	{
		//std::cout << "Sending status update message for call:" << details.callID << std::endl;
		//LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
		//	"Sending status update message for call:  %d", details.callID);

		TA_Base_Bus::IRadioCorbaDef::CallStatusEvent   event;
		CORBA::Any                                data;

		event.callID       = details.callID;
		event.callStatus   = convertCallStatus(details.callStatus);
		event.callCategory = convertCallCategory(details.callCategory);
		event.sessionRef = details.sessionRef;
		event.isInvisibleToGUIApps = false;
		event.consoleKey = details.consoleID;
		event.isInvisibleToGUIApps = details.isInvisibleToGUIApps;
		event.isIncludeCall = details.isIncludeCall;

		data <<= event;

		TA_Base_Core::CommsMessageSender* commsMessageSender =
			TA_Base_Core::MessagePublicationManager::getInstance().getCommsMessageSender( TA_Base_Core::RadioComms::RadioCallStatus, m_entityLocation, m_entityLocation, NsaTypeRegular);
	
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
			"onCallStatusChange()sendCommsMessage(m_EntityKey=%d,m_EntityLocation=%d)",m_EntityKey,m_entityLocation);

		commsMessageSender->sendCommsMessage(TA_Base_Core::RadioComms::RadioCallStatus,
											 m_EntityKey,
											 data,
											 0,
											 m_entityLocation,
											 NULL);
	}
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"onCallStatusChange()end");
	FUNCTION_EXIT;
}

void RadioServant::onTrainEvent(std::string trainTSI,std::vector<unsigned char> messageData)
{
	FUNCTION_ENTRY("onTrainEvent");

	if (m_active)
	{
		//std::cout << "Sending train message notification from train:" << trainTSI << std::endl;
		//LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
		//	"Sending train message notification from train:  %s", trainTSI);


		TA_Base_Bus::IRadioCorbaDef::SDSNotificationEvent   event;
		CORBA::Any                                     data;

		event.sendingITSI = CORBA::string_dup(trainTSI.c_str());
		event.messageData.length(messageData.size());
		for (unsigned int index = 0;index<messageData.size();index++)
		{
			event.messageData[index] = messageData[index];
		}

		data <<= event;

		TA_Base_Core::CommsMessageSender* commsMessageSender =
			TA_Base_Core::MessagePublicationManager::getInstance().getCommsMessageSender( TA_Base_Core::RadioComms::TrainMessage, m_entityLocation, m_entityLocation, NsaTypeRegular);

		commsMessageSender->sendCommsMessage(TA_Base_Core::RadioComms::TrainMessage,
											 m_EntityKey,
											 data,
											 0,                 // subsystem
											 m_entityLocation,  // region
											 NULL);             // filter
	}

	FUNCTION_EXIT;
}

void RadioServant::onTXChange(const TXStateInfo txInfo)
{
	FUNCTION_ENTRY("onTXChange");

	if (m_active)
	{
		TA_Base_Bus::IRadioCorbaDef::PTTStatusNotification   event;
		CORBA::Any                                      data;
		
		switch (txInfo.state)
		{
			case TX_GRANTED:
				event.state = TA_Base_Bus::IRadioCorbaDef::PTT_GRANTED;
				break;
			case TX_QUEUE:
				event.state = TA_Base_Bus::IRadioCorbaDef::PTT_QUEUE;
				break;
			case TX_CEASE:
				event.state = TA_Base_Bus::IRadioCorbaDef::PTT_CEASE;
				break;
			case TX_TIMEOUT:
				event.state = TA_Base_Bus::IRadioCorbaDef::PTT_TIMEOUT;
				break;
			default:
				TA_ASSERT(false,"Unknown TX state.");
		}

		event.callID        = txInfo.callID;
		event.sessionRef    = txInfo.sessionRef;
		event.transactionID = txInfo.transactionID;
		event.userITSI      = CORBA::string_dup(txInfo.userITSI.c_str());
		event.tpiITSI       = CORBA::string_dup(txInfo.tpiITSI.c_str());

		data <<= event;

		TA_Base_Core::CommsMessageSender* commsMessageSender =
			TA_Base_Core::MessagePublicationManager::getInstance().getCommsMessageSender( TA_Base_Core::RadioComms::RadioPTTNotification, m_entityLocation, m_entityLocation, NsaTypeRegular);

		commsMessageSender->sendCommsMessage(TA_Base_Core::RadioComms::RadioPTTNotification,
											 m_EntityKey,
											 data,
											 0,                 // subsystem
											 m_entityLocation,  // region
											 NULL);             // filter
	}

	FUNCTION_EXIT;
}

void RadioServant::onPTTBtnStatus(const PTTBtnStateInfo pttBtnInfo)
{
	FUNCTION_ENTRY("onPTTStatus");
	if (m_active)
	{
		TA_Base_Bus::IRadioCorbaDef::PTTBtnStatusNotification   event;
		CORBA::Any                                      data;

		switch (pttBtnInfo.state)
		{
		case PTT_PRESSED:
			event.state = TA_Base_Bus::IRadioCorbaDef::PTT_PRESSED;
			break;
		case PTT_RELEASED:
			event.state = TA_Base_Bus::IRadioCorbaDef::PTT_RELEASED;
			break;		
		default:
			TA_ASSERT(false,"Unknown PTT Btn state.");
		}

		event.sessionRef = pttBtnInfo.sessionRef;
		event.consoleKey = pttBtnInfo.consoleKey;
		data <<= event;

		TA_Base_Core::CommsMessageSender* commsMessageSender =
			TA_Base_Core::MessagePublicationManager::getInstance().getCommsMessageSender( TA_Base_Core::RadioComms::RadioPTTNotification, m_entityLocation, m_entityLocation, NsaTypeRegular);

		commsMessageSender->sendCommsMessage(TA_Base_Core::RadioComms::RadioPTTNotification,
			m_EntityKey,
			data,
			0,                 // subsystem
			m_entityLocation,  // region
			NULL);             // filter
	}
	FUNCTION_EXIT;
}

void RadioServant::updateGX(std::string sessionId, std::string message, ConsoleID consoleId)
{
	FUNCTION_ENTRY("updateGX");

	if (m_active)
	{
		// Notify the HMI of GX status being set for this session
		TA_Base_Core::IControlStationCorbaDef::TAToGXMessage   event;
		CORBA::Any                                data;
		event.targetSessionID = CORBA::string_dup(sessionId.c_str());
		event.targetSchematicID = CORBA::string_dup("");
		event.sourceEntityName = CORBA::string_dup((TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME)).c_str());
		event.message = CORBA::string_dup(message.c_str());
		event.consoleKey = consoleId;

		data <<= event;

		TA_Base_Core::CommsMessageSender* commsMessageSender =
			TA_Base_Core::MessagePublicationManager::getInstance().getCommsMessageSender( TA_Base_Core::ControlStationComms::TAToGXMessageNotification, m_entityLocation, m_entityLocation, NsaTypeRegular);

		commsMessageSender->sendCommsMessage(TA_Base_Core::ControlStationComms::TAToGXMessageNotification,
											 consoleId,
											 data,
											 0,
											 m_entityLocation,
											 NULL);
	}

	FUNCTION_EXIT;
}

void RadioServant::submitAlarm( const TA_Base_Core::MessageType& type, const TA_Base_Core::DescriptionParameters& dp )
{
    FUNCTION_ENTRY("submitAlarm()");
    TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().submitAlarmWithAsset( type, 
        m_EntityKey, 
        m_entityTypeKey, 
        dp,
        m_objectName,
        m_entityLocation,
        m_entitySubsystem,
		m_assetName,
		TA_Base_Core::AlarmConstants::defaultTime );
    FUNCTION_EXIT;
}

void RadioServant::onRadioFallback(bool isFallback)
{
    FUNCTION_ENTRY("onRadioFallback()");
    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Attempting to inform TrainAgent of Radio Fallback");

    // the train agent doesnt care about radio fallback status changing
    // if the session it uses is not initialised and ready to go,
    // so only tell it if the iscs session is logged in
    RadioSession& iscsRadioSession = m_radio->getISCSRadioSession();

    if ( (false == iscsRadioSession.isLoggedIn()) )
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
            "Not informing TrainAgent of Radio Fallback yet, iscs radio session still not logged in");

        FUNCTION_EXIT;
        //return;
    }

	// initialise m_trainAgentProxy if it isnt already
	if ( !m_trainAgentProxy)
	{
		m_trainAgentProxy = TA_IRS_Bus::TrainAgentProxyFactory::getInstance().createTrainOperationModeCorbaProxy();
    }
	
	try
	{
		m_trainAgentProxy->inFallback(isFallback);
		
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
			"Successfully informed TrainAgent of Radio Fallback" );
	}
	catch ( TA_IRS_Bus::AgentCommunicationException& ex )
	{
		std::ostringstream message;
		message << "While sending isFallback command to local train agent. " << ex.what();
		LOG_EXCEPTION_CATCH(SourceInfo, "AgentCommunicationException", message.str().c_str() );
	}
	catch (...)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "...", "While sending isFallback command to local train agent." );
    }

    FUNCTION_EXIT;
}

void RadioServant::setDirectoryCallback(IRadioDirectoryCallback * directoryCallback)
{
    m_radio->setDirectoryCallback(directoryCallback);
}


void RadioServant::applyFullState(const RadioServantType& state)
{
    m_radio->applyFullState(state.radio);
}

void RadioServant::clearFullState()
{
    m_radio->clearFullState();
}

void RadioServant::getFullState(RadioServantType& state)
{
	state.entityKey = m_EntityKey; // TD9016
    m_radio->getFullState(state.radio);
}

void RadioServant::updateCallInRadioCallStack(const CallDetailsType& callDetails)
{
    m_radio->updateCallInRadioCallStack(callDetails);
}

void RadioServant::updateRadioMonitoredCallProgression(ta_uint32 monitorReference)
{
	this->m_radio->updateRadioMonitoredCallProgression(monitorReference);
}

void RadioServant::removeMonitoredSubscriberDetails(ta_uint32 monitorReference, ta_int32 serverIndex)
{
	this->m_radio->removeMonitoredSubscriberDetails(monitorReference, serverIndex);
}


void RadioServant::removeSessionFromAudioMap(SessionID sessionReference, ta_int32 serverIndex)
{
	
	m_radio->removeSessionFromAudioMap(sessionReference, serverIndex);
}

void RadioServant::updateAudioEntryInSessionMap(SessionID sessionReference, 
												  AudioReference audioReference,
												  const AudioEntryType& audioEntry)
{
	m_radio->updateAudioEntryInSessionMap(sessionReference, audioReference, audioEntry);
}
void RadioServant::updateRadioResourceManagerVars(SessionID sessionReference, CallReference callReference,
												  int currentVolume, ta_int32 serverIndex)
{
	m_radio->updateRadioResourceManagerVars(sessionReference, callReference, currentVolume, serverIndex);
}

void RadioServant::updateAuthorisationRequest(const AuthorisationRequestType& authorisationRequest, ta_int32 serverIndex)
{
	m_radio->updateAuthorisationRequest(authorisationRequest, serverIndex);
}

//***************************
// IRadio interface methods *
//***************************

void RadioServant::removeAuthorisationRequest(CallID callID, ta_int32 serverIndex)
{
	m_radio->removeAuthorisationRequest(callID, serverIndex);
}


TA_Base_Bus::IRadioCorbaDef::CallList* RadioServant::getCallList(ConsoleID consoleId)
{
	FUNCTION_ENTRY("getCallList");

	TA_Base_Bus::GenericAgent::ensureControlMode();
	// Get a list of calls
	TA_IRS_App::CallList  callList;
	m_radio->getCallList(consoleId, callList);

	// Get size of call stack
	CORBA::ULong iCallCount  = callList.size();
	CORBA::ULong iCallNumber = 0;

	// CORBA will delete this object
	TA_Base_Bus::IRadioCorbaDef::CallList* corbaCallList = new TA_Base_Bus::IRadioCorbaDef::CallList();
    corbaCallList->length(iCallCount);

	TA_IRS_App::CallList::iterator it;
	TA_IRS_App::CallDetailsType    callDetails;

	for (it=callList.begin();it!=callList.end();it++)
	{
		if (m_radio->getCallDetailsPartial(*it,consoleId,callDetails))
		{
			(*corbaCallList)[iCallNumber].callID       = callDetails.callID;
			(*corbaCallList)[iCallNumber].sessionRef   = callDetails.sessionRef;
            (*corbaCallList)[iCallNumber].consoleKey   = callDetails.consoleID;
			(*corbaCallList)[iCallNumber].callStatus   = convertCallStatus(callDetails.callStatus);
			(*corbaCallList)[iCallNumber].callCategory = convertCallCategory(callDetails.callCategory);
            (*corbaCallList)[iCallNumber].isInvisibleToGUIApps = callDetails.isInvisibleToGUIApps;
			(*corbaCallList)[iCallNumber].isIncludeCall = callDetails.isIncludeCall;
			iCallNumber++;
		}
	}

	FUNCTION_EXIT;
	return corbaCallList;
}

TA_Base_Bus::IRadioCorbaDef::CallDetailsType* RadioServant::getCallDetails(ta_int32 callID,ConsoleID consoleId) //throw (TA_IRS_App::IRadioCorbaDef::invalidCallID)
{
	FUNCTION_ENTRY("getCallDetails");

	TA_Base_Bus::GenericAgent::ensureControlMode();
	TA_IRS_App::CallDetailsType   callDetails;

	    if (!m_radio->getCallDetails(callID,consoleId,callDetails))
	{
		// Cannot find required call ID so throw an exception
		FUNCTION_EXIT;
		throw TA_Base_Bus::IRadioCorbaDef::invalidCallIDException();  
	}
	
	// CORBA will delete this object
	TA_Base_Bus::IRadioCorbaDef::CallDetailsType* corbaCallDetails = new TA_Base_Bus::IRadioCorbaDef::CallDetailsType();
	
	corbaCallDetails->callID         = callDetails.callID;
	corbaCallDetails->sessionRef     = callDetails.sessionRef;
	corbaCallDetails->time           = callDetails.time;
	corbaCallDetails->callStatus     = convertCallStatus(callDetails.callStatus);
	corbaCallDetails->callCategory   = convertCallCategory(callDetails.callCategory);
	corbaCallDetails->callType       = convertCallType(callDetails.callType);
	corbaCallDetails->callSource     = convertCallSource(callDetails.callSource);;
	corbaCallDetails->callTime       = CORBA::string_dup(callDetails.callTime.c_str());
	corbaCallDetails->callerName     = CORBA::string_dup(callDetails.callerName.c_str());
	corbaCallDetails->callerTSI      = CORBA::string_dup(callDetails.callerTSI.c_str());
	corbaCallDetails->callerLocation = CORBA::string_dup(callDetails.callerLocation.c_str());
	corbaCallDetails->calledName     = CORBA::string_dup(callDetails.calledName.c_str());
	corbaCallDetails->calledTSI      = CORBA::string_dup(callDetails.calledTSI.c_str());
	corbaCallDetails->calledLocation = CORBA::string_dup(callDetails.calledLocation.c_str());
	corbaCallDetails->isEmergency    = callDetails.isEmergency;
	//TD 10234
    corbaCallDetails->isInvisibleToGUIApps = callDetails.isInvisibleToGUIApps;
	corbaCallDetails->isIncludeCall = callDetails.isIncludeCall;

    if (!callDetails.participants.empty())
    {
        int numParticipants = callDetails.participants.size();
        corbaCallDetails->participants.length(numParticipants);

        for ( int pNum=0 ; pNum<numParticipants ; pNum++ )
        {
            corbaCallDetails->participants[pNum].name       = CORBA::string_dup(callDetails.participants[pNum].name.c_str());
            corbaCallDetails->participants[pNum].TSI        = CORBA::string_dup(callDetails.participants[pNum].TSI.c_str());
            corbaCallDetails->participants[pNum].location   = CORBA::string_dup(callDetails.participants[pNum].location.c_str());
            corbaCallDetails->participants[pNum].isAcknowledged = callDetails.participants[pNum].isAcknowledged;
            corbaCallDetails->participants[pNum].pttState   = convertPTTState(callDetails.participants[pNum].pttState);
        }
    }
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "getCallDetails() Got Call Details: ID=%d, console=%lu, Caller=%s (%s), Called=%s (%s)",
                                                                callID, consoleId,
                                                                callDetails.callerName.c_str(),
                                                                callDetails.callerTSI.c_str(),
                                                                callDetails.calledName.c_str(),
                                                                callDetails.calledTSI.c_str());

	// Return the details
	FUNCTION_EXIT;
	return corbaCallDetails;
}

TA_Base_Bus::IRadioCorbaDef::ECallCategory RadioServant::getCallCategory(ta_int32 callID,ConsoleID consoleId) //throw (TA_IRS_App::IRadioCorbaDef::invalidCallID)
{
	FUNCTION_ENTRY("getCallCategory");

	TA_Base_Bus::GenericAgent::ensureControlMode();
	TA_IRS_App::CallDetailsType   callDetails;

	if (!m_radio->getCallDetailsPartial(callID,consoleId,callDetails))
	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "getCallCategory() Failed to locate call ID=%d (console=%lu)", callID, consoleId);
		// Cannot find required call ID so throw an exception
		FUNCTION_EXIT;
		throw TA_Base_Bus::IRadioCorbaDef::invalidCallIDException();  
	}

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "getCallCategory() Got Call Details: ID=%d, console=%lu, Caller=%s (%s), Called=%s (%s)",
                                                                callID, consoleId,
                                                                callDetails.callerName.c_str(),
                                                                callDetails.callerTSI.c_str(),
                                                                callDetails.calledName.c_str(),
                                                                callDetails.calledTSI.c_str());
	FUNCTION_EXIT;
	return convertCallCategory(callDetails.callCategory);  
}

char * RadioServant::getCallText(ta_int32 callID,ConsoleID consoleId)
{
	FUNCTION_ENTRY("getCallText");

	TA_Base_Bus::GenericAgent::ensureControlMode();
	TA_IRS_App::CallDetailsType   callDetails;

	// Find the call in the stack
	if (!m_radio->getCallDetails(callID,consoleId,callDetails))
	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "getCallText() Failed to locate call ID=%d (console=%lu)", callID, consoleId);
		// Cannot find required call ID so throw an exception
		FUNCTION_EXIT;
		throw TA_Base_Bus::IRadioCorbaDef::invalidCallIDException();  
	}

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "getCallText() Got Call Details: ID=%d, console=%lu, Caller=%s (%s), Called=%s (%s)",
                                                                callID, consoleId,
                                                                callDetails.callerName.c_str(),
                                                                callDetails.callerTSI.c_str(),
                                                                callDetails.calledName.c_str(),
                                                                callDetails.calledTSI.c_str());

	FUNCTION_EXIT;			
	return CORBA::string_dup(callDetails.callText.c_str());
}

TA_Base_Bus::IRadioCorbaDef::SelectedCallList*  RadioServant::getList(const char* mft)
{
	FUNCTION_ENTRY("getList");
	TA_Base_Bus::GenericAgent::ensureControlMode();
	TA_Base_Bus::IRadioCorbaDef::SelectedCallList* details;
	try
	{
		details = m_radio->getList(mft);
		//TA_Base_Bus::IRadioCorbaDef::SelectedCallList* corbaCallList = new TA_Base_Bus::IRadioCorbaDef::SelectedCallList();
	}
	catch (...)
	{

	}

	FUNCTION_EXIT;
	return details;
}

TA_Base_Bus::IRadioCorbaDef::CallStackHistoryList*  RadioServant::getCSHList(const char* profile, ta_uint32 locationKey, bool readWrite)
{
	FUNCTION_ENTRY("getCSHList");
	TA_Base_Bus::GenericAgent::ensureControlMode();
	TA_Base_Bus::IRadioCorbaDef::CallStackHistoryList* details;
	try
	{
		details = m_radio->getCSHList(profile, locationKey, readWrite);
	}
	catch (...)
	{

	}

	FUNCTION_EXIT;
	return details;
}

TA_Base_Bus::IRadioCorbaDef::CallStackHistoryList* RadioServant::getCSHListByType(const char* profile, ::CORBA::ULong locationKey, ::TA_Base_Bus::IRadioCorbaDef::EStackCallType callType, ::CORBA::Boolean readWrite)
{
	FUNCTION_ENTRY("getCSHListByType");
	TA_Base_Bus::GenericAgent::ensureControlMode();
	TA_Base_Bus::IRadioCorbaDef::CallStackHistoryList* details;
	try
	{
		details = m_radio->getCSHListByType(profile, locationKey, callType, readWrite);
	}
	catch (...)
	{

	}

	FUNCTION_EXIT;
	return details;
}

void RadioServant::addTetraid(const char* mft, const char* tsiNumber, const char* callId, const char* location)
{
	FUNCTION_ENTRY("addTetraid");
	TA_Base_Bus::GenericAgent::ensureControlMode();

	m_radio->addTetraid(mft, tsiNumber, callId, location);
	FUNCTION_EXIT;
}

void RadioServant::addCallStackEntry(const char* profile, ta_uint32 locationKey, ta_uint32 timestamp, TA_Base_Bus::IRadioCorbaDef::EOriginType originType, TA_Base_Bus::IRadioCorbaDef::EStackCallType callType, const char* identifier, const char* location, bool isEmergency, const char* itsi)
{
	FUNCTION_ENTRY("addCallStackEntry");

	m_radio->addCallStackEntry(profile, locationKey, timestamp, originType, callType, identifier, location, isEmergency, itsi);
	FUNCTION_EXIT;

}

void RadioServant::updateCallStackHistory(const CallStackHistoryList& callStackHistoryList, bool isRetrieved)
{
	FUNCTION_ENTRY("updateCallStackHistory");

	if (isRetrieved)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "updateCallStackHistory() - Call Stack History Data has been retrieved, proceed to add to local db");
		// Retrieved updated CallStackHistoryList from Agent in Control Mode
		//Call to delete existing entries in call stack history list
		m_radio->clearCallStackHistory();

		//Call to insert entries
		for (ta_uint32 index = 0; index < callStackHistoryList.size(); index++)
		{
			TA_Base_Bus::IRadioCorbaDef::EOriginType oType = convertOriginType(callStackHistoryList[index].originType);
			TA_Base_Bus::IRadioCorbaDef::EStackCallType scType = convertStackCallType(callStackHistoryList[index].callType);

			m_radio->addCallStackEntry(
				callStackHistoryList[index].profile.c_str(),
				callStackHistoryList[index].locationKey,
				callStackHistoryList[index].timestamp,
				oType,
				scType,
				callStackHistoryList[index].identifier.c_str(),
				callStackHistoryList[index].location.c_str(),
				callStackHistoryList[index].isEmergency,
				callStackHistoryList[index].tsi.c_str());
		}
	}
	FUNCTION_EXIT;
}

void RadioServant::removeCallStackEntry(ta_uint32 timestamp, const char* tsi)
{
	FUNCTION_ENTRY("removeCallStackEntry");
	m_radio->removeCallStackEntry(timestamp, tsi);
	FUNCTION_EXIT;
}

void RadioServant::sendSDSMessage(const char* trainITSI, const TA_Base_Bus::IRadioCorbaDef::MessageListType& messageList)
{
	FUNCTION_ENTRY("sendSDSMessage");

	TA_ASSERT(false, "using SDS-TL, those code are not useful; in RadioServant::sendSDSMessage");

	FUNCTION_EXIT;			
}

CORBA::Boolean RadioServant::sendTLSDSMessage(const char* trainITSI, const TA_Base_Bus::IRadioCorbaDef::MessageListType& messageList, CORBA::String_out status)
{
	FUNCTION_ENTRY("sendTLSDSMessage");
	TA_Base_Bus::GenericAgent::ensureControlMode();
	// TD12462 In fallback, cannot send SDS to train if audio already established
	if (m_radio->inFallback())
	{
		TA_IRS_App::CallList callList;
		std::string tsi = trainITSI;
		m_radio->getTsiCallList(tsi, true, callList);
		// therefore if ANY calls on the list they are
		if ( callList.size() > 0 )
		{
			char errorstr[256] = {0};
			sprintf(errorstr, "Attempt to send SDS to TSI %.32s with active audio (callId %lu) while in fallback", tsi.c_str(), callList[0]);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, errorstr );
			throw TA_Base_Bus::IRadioSessionCorbaDef::radioAPIException(errorstr);  
		}
	}

	TA_IRS_App::MessageListType messages;
	unsigned int numMessages = messageList.length();

	for (unsigned int messageIndex = 0; messageIndex < numMessages; ++messageIndex)
	{
		TA_IRS_App::MessageTextType messageText;

		unsigned int textLength = messageList[messageIndex].length();

		for (unsigned int textIndex = 0; textIndex < textLength; ++textIndex)
		{
			messageText.push_back( messageList[messageIndex][textIndex] );
		}

		messages.push_back(messageText);
	}


	bool bSendStatus = false;
	try
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Constructed SDS messaging. Going to send for TrainITSI: %s",  trainITSI);
		std::string statusText;
		if (m_radio->sendSDSTLMessage(trainITSI, messages, statusText))
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Send SDS message successfully, statusText: %s", statusText.c_str());
			bSendStatus = true;
		}
		status = CORBA::string_dup(statusText.c_str());

	}
	catch (RadioException rse)
	{
		// pass on via corba back to the TrainAgent or whomever
		throw TA_Base_Bus::IRadioCorbaDef::radioException(rse.what());  
	}

	FUNCTION_EXIT;			
	return bSendStatus;
}

CORBA::Boolean RadioServant::sendUserDefinedSDSMessage(const char* trainITSI, const TA_Base_Bus::IRadioCorbaDef::MessageListType& messageList, CORBA::String_out status)
{
	FUNCTION_ENTRY("sendUserDefinedSDSMessage");
	TA_Base_Bus::GenericAgent::ensureControlMode();
	// TD12462 In fallback, cannot send SDS to train if audio already established
	if (m_radio->inFallback())
	{
		TA_IRS_App::CallList callList;
		std::string tsi = trainITSI;
		m_radio->getTsiCallList(tsi, true, callList);
		// therefore if ANY calls on the list they are
		if ( callList.size() > 0 )
		{
			char errorstr[256] = {0};
			sprintf(errorstr, "Attempt to send User defined SDS to TSI %.32s with active audio (callId %lu) while in fallback", tsi.c_str(), callList[0]);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, errorstr );
			throw TA_Base_Bus::IRadioSessionCorbaDef::radioAPIException(errorstr);  
		}
	}

	TA_IRS_App::MessageListType messages;
	unsigned int numMessages = messageList.length();

	for (unsigned int messageIndex = 0; messageIndex < numMessages; ++messageIndex)
	{
		TA_IRS_App::MessageTextType messageText;

		unsigned int textLength = messageList[messageIndex].length();

		for (unsigned int textIndex = 0; textIndex < textLength; ++textIndex)
		{
			messageText.push_back( messageList[messageIndex][textIndex] );
		}

		messages.push_back(messageText);
	}


	bool bSendStatus = false;
	try
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Constructed User defined SDS messaging. Going to send for TrainITSI: %s",  trainITSI);
		std::string statusText;
		if (m_radio->sendUserDefinedSDSMessage(trainITSI, messages, statusText))
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Send User defined SDS message successfully, statusText: %s", statusText.c_str());
			bSendStatus = true;
		}
		status = CORBA::string_dup(statusText.c_str());

	}
	catch (RadioException rse)
	{
		// pass on via corba back to the TrainAgent or whomever
		throw TA_Base_Bus::IRadioCorbaDef::radioException(rse.what());  
	}

	FUNCTION_EXIT;			
	return bSendStatus;
}

// Circuit data transfers

void RadioServant::sendData(const char* ITSI, const char* IPAddress, const class TA_Base_Bus::IRadioCorbaDef::CircuitDataType& circuitData, bool useXON, const char * sessionId)
{
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Circuit mode data transfer size: %d", circuitData.length() );
	TA_Base_Bus::GenericAgent::ensureControlMode();
    RadioCircuitTransfer::Data data;

    for (ta_uint32 index=0;index < circuitData.length() ;index++)
	{
		data.push_back( circuitData[index] );
	}

    try
    {
	    m_radio->getRadioCircuitModeManager()->addRadioCircuitTransfer( data, ITSI, IPAddress );
    }
    catch(const RadioCircuitModeException& rce )
    {
        throw TA_Base_Bus::IRadioCorbaDef::dataCallInProgressException();
    }
}


void RadioServant::removeCallFromRadioCallStack(CallID callID, ta_uint32 consoleID)
{
    m_radio->removeCallFromRadioCallStack(callID,consoleID);
}

void RadioServant::closeAlarm( const TA_Base_Core::MessageType& type)
{
	TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().closeAlarmAtLocation(type,m_EntityKey,m_entityLocation);
}

TA_Base_Bus::IRadioCorbaDef::PredefinedSDSMessage* RadioServant::getByShortName(const char* shortName)
{
	return m_radio->getByShortName(shortName);
}

TA_Base_Bus::IRadioCorbaDef::PredefinedSDSMessageList* RadioServant::getSDSList(bool readWrite)
{
	return m_radio->getSDSList(readWrite);
}

void RadioServant::deleteMessage(const char* shortName)
{
	m_radio->deleteMessage(shortName);
}

void RadioServant::createMessage(const char* shortName, const char* message, bool isNew)
{
	m_radio->createMessage(shortName, message, isNew);
}

void RadioServant::updateMessage(const char* shortName, const char* message)
{
	m_radio->updateMessage(shortName, message);
}

//***************************
// Private helper functions *
//***************************

TA_Base_Bus::IRadioCorbaDef::ECallStatusType convertCallStatus(TA_IRS_App::ECallStatusType  status)
{
	switch (status)
	{
		case TA_IRS_App::Ringing:
			return TA_Base_Bus::IRadioCorbaDef::Ringing;
		case TA_IRS_App::Connected:
			return TA_Base_Bus::IRadioCorbaDef::Connected;
		case TA_IRS_App::ConnectedGroup:
			return TA_Base_Bus::IRadioCorbaDef::ConnectedGroup;
		case TA_IRS_App::Disconnected:
			return TA_Base_Bus::IRadioCorbaDef::Disconnected;
		case TA_IRS_App::OnHold:
			return TA_Base_Bus::IRadioCorbaDef::OnHold;
		case TA_IRS_App::Deleted:
			return TA_Base_Bus::IRadioCorbaDef::Deleted;
		case TA_IRS_App::SessionUpdate:
			return TA_Base_Bus::IRadioCorbaDef::SessionUpdate;
		default:
			TA_ASSERT(false, "No conversion specified for call status");
	}

    return ((TA_Base_Bus::IRadioCorbaDef::ECallStatusType)-1);
}

TA_Base_Bus::IRadioCorbaDef::ECallCategory convertCallCategory(TA_IRS_App::ECallCategory category)
{
	switch (category)
	{
		case TA_IRS_App::HalfDuplex:
			return TA_Base_Bus::IRadioCorbaDef::HalfDuplex;
		case TA_IRS_App::FullDuplex:
			return TA_Base_Bus::IRadioCorbaDef::FullDuplex;
		case TA_IRS_App::TextMessage:
			return TA_Base_Bus::IRadioCorbaDef::TextMessage;
		case TA_IRS_App::CallDriver:
			return TA_Base_Bus::IRadioCorbaDef::CallDriver;
		default:
			TA_ASSERT(false, "No conversion specified for call status");
	}

    return ((TA_Base_Bus::IRadioCorbaDef::ECallCategory)-1);
}

TA_Base_Bus::IRadioCorbaDef::ECallType convertCallType(TA_IRS_App::ECallType callType)
{
    switch (callType)
    {
        case TA_IRS_App::SingleCall:
            return TA_Base_Bus::IRadioCorbaDef::SingleCall;
            break;
        case TA_IRS_App::GroupAcknowledgeCall:
            return TA_Base_Bus::IRadioCorbaDef::GroupAcknowledgeCall;
            break;
        case TA_IRS_App::GroupUnacknowledgeCall:
            return TA_Base_Bus::IRadioCorbaDef::GroupUnacknowledgeCall;
            break;
        case TA_IRS_App::GroupBroadcastCall:
            return TA_Base_Bus::IRadioCorbaDef::GroupBroadcastCall;
            break;
        default:
			// TA_ASSERT(false, "No conversion specified for call type");
            break;
    }

    // to avoid compiler warnings about not returning a value
    return TA_Base_Bus::IRadioCorbaDef::SingleCall;
}

TA_Base_Bus::IRadioCorbaDef::ECallSource convertCallSource(TA_IRS_App::ECallSource source)
{
	switch (source)
	{
		case TA_IRS_App::IncomingCall:
			return TA_Base_Bus::IRadioCorbaDef::IncomingCall;
		case TA_IRS_App::OutgoingCall:
			return TA_Base_Bus::IRadioCorbaDef::OutgoingCall;
		default:
			TA_ASSERT(false, "No conversion specified for call source");
	}

    return ((TA_Base_Bus::IRadioCorbaDef::ECallSource)-1);
}


TA_Base_Bus::IRadioCorbaDef::EPTTState convertPTTState(TA_IRS_App::EPTTState pttState)
{ 
	switch (pttState)
	{
		case PTT_GRANTED: return TA_Base_Bus::IRadioCorbaDef::PTT_GRANTED;
		case PTT_QUEUE: return TA_Base_Bus::IRadioCorbaDef::PTT_QUEUE;
		case PTT_CEASE: return TA_Base_Bus::IRadioCorbaDef::PTT_CEASE;
		case PTT_TIMEOUT: return TA_Base_Bus::IRadioCorbaDef::PTT_TIMEOUT;
		default:
			TA_ASSERT(false,"Invalid PTT State.");
	}

	return TA_Base_Bus::IRadioCorbaDef::PTT_CEASE;
}


TA_Base_Bus::IRadioCorbaDef::EOriginType convertOriginType(TA_IRS_App::EOriginType originType)
{
	switch (originType)
	{
	case ORIGIN_INVALID: return TA_Base_Bus::IRadioCorbaDef::ORIGIN_INVALID;
	case INBOUND: return TA_Base_Bus::IRadioCorbaDef::INBOUND;
	case OUTBOUND: return TA_Base_Bus::IRadioCorbaDef::OUTBOUND;
	default:
		TA_ASSERT(false, "Invalid OriginType.");
	}

	return TA_Base_Bus::IRadioCorbaDef::ORIGIN_INVALID;
}


TA_Base_Bus::IRadioCorbaDef::EStackCallType convertStackCallType(TA_IRS_App::EStackCallType stackCallType)
{
	switch (stackCallType)
	{
	case CALL_INVALID: return TA_Base_Bus::IRadioCorbaDef::CALL_INVALID;
	case VOICE: return TA_Base_Bus::IRadioCorbaDef::VOICE;
	case CONFERENCE: return TA_Base_Bus::IRadioCorbaDef::CONFERENCE;
	case TEXT: return TA_Base_Bus::IRadioCorbaDef::TEXT;
	case GROUP_ACK: return TA_Base_Bus::IRadioCorbaDef::GROUP_ACK;
	case GROUP_UNACK: return TA_Base_Bus::IRadioCorbaDef::GROUP_UNACK;
	case GROUP_BROADCAST: return TA_Base_Bus::IRadioCorbaDef::GROUP_BROADCAST;
	default:
		TA_ASSERT(false, "Invalid Stack Call Type.");
	}

	return TA_Base_Bus::IRadioCorbaDef::CALL_INVALID;
}

void RadioServant::onCircuitModeActivity(const char * ITSI, bool successful, const char * failReason)
{
	FUNCTION_ENTRY("onCircuitModeActivity");

	if (m_active)
	{
		//std::cout << "Sending circuit mode notification for datacall to itsi " << ITSI << std::endl;
		//LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
		//	"Sending circuit mode notification for datacall to itsi  %s", ITSI);

		TA_Base_Bus::IRadioCorbaDef::SendDataNotification event;
		CORBA::Any data;

		event.ITSI          = CORBA::string_dup(ITSI);
		event.isSuccessful  = successful;
		event.failReason    = CORBA::string_dup(failReason);

		data <<= event;

		TA_Base_Core::CommsMessageSender* commsMessageSender =
			TA_Base_Core::MessagePublicationManager::getInstance().getCommsMessageSender( TA_Base_Core::RadioComms::RadioCircuitModeNotification, m_entityLocation, m_entityLocation, NsaTypeRegular);

		commsMessageSender->sendCommsMessage(TA_Base_Core::RadioComms::RadioCircuitModeNotification,
											 m_EntityKey,
											 data,
											 0,
											 m_entityLocation,
											 NULL);
	}

	FUNCTION_EXIT;

}

void RadioServant::onSubscriberActivity(TA_IRS_App::MonitoredCallDetailsType* subscriberActivityCallDetails)
{
    FUNCTION_ENTRY("onSubscriberActivity()");

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                "Sending RadioMonitor notification of SubscriberActivity (%p) for Monitor %lu Call %lu",
                subscriberActivityCallDetails, subscriberActivityCallDetails->monitorID, subscriberActivityCallDetails->callID);

	TA_Base_Bus::IRadioCorbaDef::MonitoredCallDetailsType subscriberActivityCorbaDetails;

    subscriberActivityCorbaDetails.callID           = subscriberActivityCallDetails->callID;
    subscriberActivityCorbaDetails.sessionRef       = subscriberActivityCallDetails->sessionRef;
    subscriberActivityCorbaDetails.monitorID        = subscriberActivityCallDetails->monitorID;
    subscriberActivityCorbaDetails.callTime         = CORBA::string_dup(subscriberActivityCallDetails->callTime.c_str());
    subscriberActivityCorbaDetails.callerName       = CORBA::string_dup(subscriberActivityCallDetails->callerName.c_str());
    subscriberActivityCorbaDetails.callerTSI        = CORBA::string_dup(subscriberActivityCallDetails->callerTSI.c_str());
    subscriberActivityCorbaDetails.callerLocation   = CORBA::string_dup(subscriberActivityCallDetails->callerLocation.c_str());
    subscriberActivityCorbaDetails.calledName       = CORBA::string_dup(subscriberActivityCallDetails->calledName.c_str());
    subscriberActivityCorbaDetails.calledTSI        = CORBA::string_dup(subscriberActivityCallDetails->calledTSI.c_str());
    subscriberActivityCorbaDetails.calledLocation   = CORBA::string_dup(subscriberActivityCallDetails->calledLocation.c_str());
    subscriberActivityCorbaDetails.callStatus       = RadioCorbaTypeConverter::convertMonitorStatusTypeToCorba(subscriberActivityCallDetails->monitorStatus);
    RadioCorbaTypeConverter::convertECallCategoryNormalToCorba(subscriberActivityCallDetails->callCategory, subscriberActivityCorbaDetails.callCategory);
    RadioCorbaTypeConverter::convertECallTypeNormalToCorba(subscriberActivityCallDetails->callType, subscriberActivityCorbaDetails.callType);
	
    // standard corba notification tricks
    CORBA::Any subscriberActivityCorbaData;
    subscriberActivityCorbaData <<= subscriberActivityCorbaDetails;
    TA_Base_Core::CommsMessageSender * commsMessageSender = 
        TA_Base_Core::MessagePublicationManager::getInstance()
            .getCommsMessageSender( TA_Base_Core::RadioComms::RadioMonitorNotification, m_entityLocation, m_entityLocation, NsaTypeRegular);

    commsMessageSender->sendCommsMessage(TA_Base_Core::RadioComms::RadioMonitorNotification,
                                         m_EntityKey,
                                         subscriberActivityCorbaData,
                                         0,
                                         m_entityLocation);
    FUNCTION_EXIT;
}

char* RadioServant::getOwnerFromPatchMember(const char* TSI)
{
	FUNCTION_ENTRY("getOwnerFromPatchMember");
	TA_Base_Bus::GenericAgent::ensureControlMode();
	return m_radio->getOwnerFromPatchMember(TSI);

	FUNCTION_EXIT;
}

bool RadioServant::getGroupIdsFromSubId(const TA_Base_Bus::IRadioCorbaDef::strList& list, const char* patchrasubs)
{
	FUNCTION_ENTRY("getGroupIdsFromSubId");
	TA_Base_Bus::GenericAgent::ensureControlMode();
	return m_radio->getGroupIdsFromSubId(list, patchrasubs);

	FUNCTION_EXIT;
}

bool RadioServant::getSubNamesFromSubId(const TA_Base_Bus::IRadioCorbaDef::strList& list, const char* TSI)
{
	FUNCTION_ENTRY("getSubNamesFromSubId");
	TA_Base_Bus::GenericAgent::ensureControlMode();
	return m_radio->getSubNamesFromSubId(list, TSI);

	FUNCTION_EXIT;
}

void RadioServant::addGroupToPatch(const char* patchSubTsi, const char* groupSubTsi)
{
	FUNCTION_ENTRY("addGroupToPatch");
	TA_Base_Bus::GenericAgent::ensureControlMode();
	m_radio->addGroupToPatch(patchSubTsi, groupSubTsi);

	FUNCTION_EXIT;
}

void RadioServant::deletePatch(const char* TSI)
{
	FUNCTION_ENTRY("deletePatch");
	TA_Base_Bus::GenericAgent::ensureControlMode();
	m_radio->deletePatch(TSI);


	FUNCTION_EXIT;
}

bool RadioServant::groupPatchInsertDb(const char* displayName, const char* profileName, const char* timestamp)
{
	FUNCTION_ENTRY("groupPatchInsertDb");
	TA_Base_Bus::GenericAgent::ensureControlMode();
	return m_radio->groupPatchInsertDb(displayName, profileName, timestamp);

	FUNCTION_EXIT;
}

char* RadioServant::getOwnerITSI(const char* patchname)
{
	FUNCTION_ENTRY("getOwnerITSI");
	return m_radio->getOwnerITSI(patchname);

	FUNCTION_EXIT;
}


//*************************************************
// IRadioDirectory - Speed Dial Interface Methods *
//*************************************************            
TA_Base_Bus::IRadioCorbaDef::SpeedDialList* RadioServant::getSpeedDialSet(const char* setName)
{
	TA_Base_Bus::GenericAgent::ensureControlMode();
    return m_radio->getSpeedDialSet(setName);
}

void RadioServant::updateSpeedDialSet(const TA_Base_Bus::IRadioCorbaDef::SpeedDialSet& setDetails, const char* sessionID)
{
	TA_Base_Bus::GenericAgent::ensureControlMode();
    m_radio->updateSpeedDialSet(setDetails, sessionID);
}

void RadioServant::updateSpeedDial(const char* setName, const TA_Base_Bus::IRadioCorbaDef::SpeedDialEntry& newEntry, const char* sessionID)
{
	TA_Base_Bus::GenericAgent::ensureControlMode();
    m_radio->updateSpeedDial(setName, newEntry, sessionID);
}

char* RadioServant::getUserSetName(const char* sessionID,TA_Base_Bus::IRadioCorbaDef::EUserSetTypeMatch setTypeMatch)
{
	TA_Base_Bus::GenericAgent::ensureControlMode();
    return CORBA::string_dup(m_radio->getUserSetName(sessionID).c_str());
}

//**********************************
// IRadioSession Interface Methods *
//**********************************
TA_Base_Bus::IRadioCorbaDef::SubscriberList*     RadioServant::getGroupSubscribers()
{
	TA_Base_Bus::GenericAgent::ensureControlMode();
	TA_Base_Bus::IRadioCorbaDef::SubscriberList* list;

	try
	{
		list = m_radio->getGroupSubscribers();    
	}
	catch (TA_Base_Core::TransactiveException& e)
	{
		// Error excuting the command
		throw TA_Base_Bus::IRadioCorbaDef::radioDirectoryError(e.what());  
	}
	catch (...)
	{
		// Error excuting the command
		throw TA_Base_Bus::IRadioCorbaDef::radioDirectoryError(UNKNOWN_EXCEPTION);  
	}

    return list;
}

TA_Base_Bus::IRadioCorbaDef::SubscriberList*     RadioServant::getPatchSubscribers()
{
	TA_Base_Bus::GenericAgent::ensureControlMode();
	TA_Base_Bus::IRadioCorbaDef::SubscriberList* list;

	try
	{
		list = m_radio->getPatchSubscribers();
	}
	catch (TA_Base_Core::TransactiveException& e)
	{
		// Error excuting the command
		throw TA_Base_Bus::IRadioCorbaDef::radioDirectoryError(e.what());  
	}
	catch (...)
	{
		// Error excuting the command
		throw TA_Base_Bus::IRadioCorbaDef::radioDirectoryError(UNKNOWN_EXCEPTION);  
	}

    return list;
}

TA_Base_Bus::IRadioCorbaDef::SubscriberList*     RadioServant::getIndividualSubscribers()
{
	TA_Base_Bus::GenericAgent::ensureControlMode();
	TA_Base_Bus::IRadioCorbaDef::SubscriberList* list;

	try
	{  
		list = m_radio->getIndividualSubscribers();
	}
	catch (TA_Base_Core::TransactiveException& e)
	{
		// Error excuting the command
		throw TA_Base_Bus::IRadioCorbaDef::radioDirectoryError(e.what());  
	}
	catch (...)
	{
		// Error excuting the command
		throw TA_Base_Bus::IRadioCorbaDef::radioDirectoryError(UNKNOWN_EXCEPTION);  
	}

    return list;
}

char* RadioServant::getOCCLastUpdateTime()
{
	TA_Base_Bus::GenericAgent::ensureControlMode();

	return CORBA::string_dup(RASubscribersAccessFactory::getInstance().getLastUpdateTime().c_str());
}

void RadioServant::setLastUpdateTime(std::string time)
{
	RASubscribersAccessFactory::getInstance().setLastUpdateTime(time);
}

TA_Base_Bus::IRadioCorbaDef::SubscriberList*     RadioServant::getAllSubscribers()
{
	TA_Base_Bus::GenericAgent::ensureControlMode();
	TA_Base_Bus::IRadioCorbaDef::SubscriberList* list;

	try
	{
		list = m_radio->getAllSubscribers();
	}
	catch (TA_Base_Core::TransactiveException& e)
	{
		// Error excuting the command
		throw TA_Base_Bus::IRadioCorbaDef::radioDirectoryError(e.what());
	}
	catch (...)
	{
		// Error excuting the command
		throw TA_Base_Bus::IRadioCorbaDef::radioDirectoryError(UNKNOWN_EXCEPTION);
	}

	return list;
}

TA_Base_Bus::IRadioCorbaDef::GroupList*     RadioServant::getAllGroups()
{
	TA_Base_Bus::GenericAgent::ensureControlMode();
	TA_Base_Bus::IRadioCorbaDef::GroupList* list;

	try
	{
		list = m_radio->getAllGroups();
	}
	catch (TA_Base_Core::TransactiveException& e)
	{
		// Error excuting the command
		throw TA_Base_Bus::IRadioCorbaDef::radioDirectoryError(e.what());
	}
	catch (...)
	{
		// Error excuting the command
		throw TA_Base_Bus::IRadioCorbaDef::radioDirectoryError(UNKNOWN_EXCEPTION);
	}

	return list;
}

TA_Base_Bus::IRadioCorbaDef::PatchList*     RadioServant::getAllPatches()
{
	TA_Base_Bus::GenericAgent::ensureControlMode();
	TA_Base_Bus::IRadioCorbaDef::PatchList* list;

	try
	{
		list = m_radio->getAllPatches();
	}
	catch (TA_Base_Core::TransactiveException& e)
	{
		// Error excuting the command
		throw TA_Base_Bus::IRadioCorbaDef::radioDirectoryError(e.what());
	}
	catch (...)
	{
		// Error excuting the command
		throw TA_Base_Bus::IRadioCorbaDef::radioDirectoryError(UNKNOWN_EXCEPTION);
	}

	return list;
}

char* RadioServant::getMessage(const char* shortName)
{
	FUNCTION_ENTRY("getMessage");
	return m_radio->getMessage(shortName);

	FUNCTION_EXIT;
}

void RadioServant::setAllSubscribers(TA_Base_Bus::IRadioCorbaDef::SubscriberList& list)
{
	m_radio->setAllSubscribers(list);
}

void RadioServant::setAllGroups(TA_Base_Bus::IRadioCorbaDef::GroupList& list)
{
	m_radio->setAllGroups(list);
}

void RadioServant::setAllPatches(TA_Base_Bus::IRadioCorbaDef::PatchList& list)
{
	m_radio->setAllPatches(list);
}

void RadioServant::setAllSDS(TA_Base_Bus::IRadioCorbaDef::PredefinedSDSMessageList& list)
{
	m_radio->setAllSDS(list);
}

TA_Base_Bus::IRadioCorbaDef::SubscriberDetails*  RadioServant::getSubscriberDetails(const char* TSI)
{
	TA_Base_Bus::GenericAgent::ensureControlMode();
	if (strlen(TSI)<1)
	{
		throw TA_Base_Bus::IRadioCorbaDef::radioInvalidSubscriberException();  
	}

    TA_Base_Bus::IRadioCorbaDef::SubscriberDetails* details;
		
	try
	{
		details = m_radio->getSubscriberDetails(TSI);
	}
	catch (TA_Base_Core::TransactiveException& e)
	{
		// Error excuting the command
		throw TA_Base_Bus::IRadioCorbaDef::radioDirectoryError(e.what());  
	}
	catch (...)
	{
		// Error excuting the command
		throw TA_Base_Bus::IRadioCorbaDef::radioDirectoryError(UNKNOWN_EXCEPTION);  
	}

    return details;
}


TA_Base_Bus::IRadioCorbaDef::SubscriberList*     RadioServant::getGroupMembers(const char* TSI)
{
	TA_Base_Bus::GenericAgent::ensureControlMode();
	if (strlen(TSI)<1)
	{
		throw TA_Base_Bus::IRadioCorbaDef::radioInvalidSubscriberException();  
	}

    TA_Base_Bus::IRadioCorbaDef::SubscriberList* list;
		
	try
	{
		list = m_radio->getGroupMembers(TSI);
	}
	catch (TA_Base_Core::TransactiveException& e)
	{
		// Error excuting the command
		throw TA_Base_Bus::IRadioCorbaDef::radioDirectoryError(e.what());  
	}
	catch (...)
	{
		// Error excuting the command
		throw TA_Base_Bus::IRadioCorbaDef::radioDirectoryError(UNKNOWN_EXCEPTION);  
	}

    return list;
}

TA_Base_Bus::IRadioCorbaDef::SubscriberList*     RadioServant::getPatchMembers(const char* TSI)
{
	TA_Base_Bus::GenericAgent::ensureControlMode();
	if (strlen(TSI)<1)
	{
		throw TA_Base_Bus::IRadioCorbaDef::radioInvalidSubscriberException();  
	}

    TA_Base_Bus::IRadioCorbaDef::SubscriberList* list;
		
	try
	{
		 list = m_radio->getPatchMembers(TSI);
	}
	catch (TA_Base_Core::TransactiveException& e)
	{
		// Error excuting the command
		throw TA_Base_Bus::IRadioCorbaDef::radioDirectoryError(e.what());  
	}
	catch (...)
	{
		// Error excuting the command
		throw TA_Base_Bus::IRadioCorbaDef::radioDirectoryError(UNKNOWN_EXCEPTION);  
	}

    return list;
}

TA_Base_Bus::IRadioCorbaDef::SubscriberList*     RadioServant::searchSubscriber(const char* searchText)
{
	TA_Base_Bus::GenericAgent::ensureControlMode();
    TA_Base_Bus::IRadioCorbaDef::SubscriberList* list;

	try
	{
		list = m_radio->searchSubscriber(searchText);
	}
	catch (TA_Base_Core::TransactiveException& e)
	{
		// Error excuting the command
		throw TA_Base_Bus::IRadioCorbaDef::radioDirectoryError(e.what());  
	}
	catch (...)
	{
		// Error excuting the command
		throw TA_Base_Bus::IRadioCorbaDef::radioDirectoryError(UNKNOWN_EXCEPTION);  
	}

    return list;
}

void RadioServant::setDataPointLink( TA_Base_Bus::DataPoint* dp )
{
	m_serverLinkStatusThread->setDataPoint(dp);
}

TA_Base_Bus::IRadioCorbaDef::RadioStatusSummary* RadioServant::getRadioStatusSummary()
{
	FUNCTION_ENTRY("getRadioStatusSummary");

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugMsgPub,
		"Received getRadioStatusSummary");

	TA_Base_Bus::IRadioCorbaDef::RadioStatusSummary* summary = new TA_Base_Bus::IRadioCorbaDef::RadioStatusSummary();

	try 
	{
		summary = m_radio->getRadioStatusSummary();
	}
	catch (...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Error trying to getRadioStatusSummary");
	}

	FUNCTION_EXIT;
	return summary;

}

/// *************************************************************
/// Dummy Functions that are used by RadioTetraAgent's RadioServant
/// *************************************************************
ta_int32 RadioServant::getSessionReference(const char * sessionId, const char* sessionName)
{
	FUNCTION_ENTRY("getSessionReference");
	// Dummy Function
	FUNCTION_EXIT;
	return 0;
}

CORBA::Boolean RadioServant::sendTextMessage(const char* destinationTSI,
	const TA_Base_Bus::IRadioSessionCorbaDef::MessageListType& message,
	const char* sessionId,
	CORBA::String_out status,
	const char* sessionName)
{
	FUNCTION_ENTRY("sendTextMessage");
	// Dummy Function
	FUNCTION_EXIT;
	return 0;
}

//CORBA::Boolean RadioServant::sendUserDefinedSDS(const char* destinationTSI, const TA_Base_Bus::IRadioSessionCorbaDef::MessageListType& messageList, const char* sessionId, CORBA::String_out status)
//{
//	FUNCTION_ENTRY("sendUserDefinedSDS");
//
//	LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
//		"Sending user defined text message to '%s'", destinationTSI);
//
//	TA_IRS_App::MessageListType messages;
//	unsigned int numMessages = messageList.length();
//
//	std::string auditText;
//	CORBA::Boolean bStatus = false;
//	std::string strStatus;
//
//	try
//	{
//		// INSERT CORBA_CALL
//	}
//	catch (RadioSessionException& e)
//	{
//		// Error excuting the command
//		throw TA_Base_Bus::IRadioSessionCorbaDef::radioAPIException(e.what());
//	}
//	catch (RadioException& e)
//	{
//		// catch all radio exceptions
//		throw TA_Base_Bus::IRadioSessionCorbaDef::radioAPIException(e.what());
//	}
//
//	FUNCTION_EXIT;
//	return bStatus;
//}

void RadioServant::holdCall(ta_int32 callId, const char * sessionId, const char* sessionName)
{
	FUNCTION_ENTRY("holdCall");
	// Dummy Function
	FUNCTION_EXIT;
}

void RadioServant::resumeCall(ta_int32 callId, const char * sessionId, const char* sessionName)
{
	FUNCTION_ENTRY("resumeCall");
	// Dummy Function
	FUNCTION_EXIT;
}

ta_int32 RadioServant::getLastDirectorySynchronisationTime()
{
	FUNCTION_ENTRY("getLastDirectorySynchronisationTime");
	// Dummy Function
	FUNCTION_EXIT;
	return 0;
}

unsigned short RadioServant::getFlexibleTimeOutValue()
{
	FUNCTION_ENTRY("getLastDirectorySynchronisationTime");
	// Dummy Function
	FUNCTION_EXIT;
	return m_flexibleTimeout;
}

void RadioServant::answerCall(ta_int32 callId, const char* sessionId, const char* sessionName)
{
	FUNCTION_ENTRY("answerCall");
	// Dummy Function
	FUNCTION_EXIT;
}

ta_int32 RadioServant::makeCall(const char * destinationTSI, TA_Base_Bus::IRadioCorbaDef::ECallType calltype, bool isFullDuplex, TA_Base_Bus::IRadioCorbaDef::ECallPriority priority, bool hook, const char * sessionId, bool isInvisibleToGUIApps, ta_int32 locationKey, const char* sessionName)
{
	FUNCTION_ENTRY("makeCall");
	// Dummy Function
	FUNCTION_EXIT;
	return 0;
}

void RadioServant::releaseCall(ta_int32 callID, const char * sessionId, const char* sessionName)
{
	FUNCTION_ENTRY("releaseCall");
	// Dummy Function
	FUNCTION_EXIT;
}

char* RadioServant::makeDynamicGroup(const char* dgnaTSI, const char* dgnaName, const TA_Base_Bus::IRadioSessionCorbaDef::DestinationTSIList& destTSIs, const char * sessionId, const char* sessionName)
{
	FUNCTION_ENTRY("makeDynamicGroup");
	// Dummy Function
	FUNCTION_EXIT;
	return 0;

}

void RadioServant::updateDynamicGroup(const char* dgnaTSI, const char* dgnaName, const ::TA_Base_Bus::IRadioSessionCorbaDef::DestinationTSIList& destTSIs, const char* sessionID, const char* sessionName)
{
	FUNCTION_ENTRY("updateDynamicGroup");
	// Dummy Function
	FUNCTION_EXIT;
}

void RadioServant::deleteDynamicGroup(const char* dgnaTSI, const char* sessionID, const char* sessionName)
{
	FUNCTION_ENTRY("deleteDynamicGroup");
	// Dummy Function
	FUNCTION_EXIT;
}

ta_int32 RadioServant::startMonitoring(const char * tsi, const char * sessionID, const char* sessionName)
{
	FUNCTION_ENTRY("startMonitoring()");
	// Dummy Function
	FUNCTION_EXIT;

	return -1;
}

void RadioServant::stopMonitoring(const char * tsi, const char * sessionID, const char* sessionName)
{
	FUNCTION_ENTRY("stopMonitoring()");
	// Dummy Function
	FUNCTION_EXIT;
}

CORBA::Long RadioServant::oneShotMonitoring(const char* TSI, const char* sessionID, const char* sessionName)
{
	FUNCTION_ENTRY("oneShotMonitoring()");
	// Dummy Function
	FUNCTION_EXIT;
	return 0;
}

TA_Base_Bus::IRadioSessionCorbaDef::MonitoredSubscriberList* RadioServant::getMonitoredSubscribers(const char* sessionID, const char* sessionName)
{
	FUNCTION_ENTRY("getMonitoredSubscribers");
	// Dummy Function
	FUNCTION_EXIT;
	return 0;
}

TA_Base_Bus::IRadioSessionCorbaDef::MonitoredGroupList* RadioServant::getMonitoredGroup(const char* sessionID, const char* sessionName)
{
	FUNCTION_ENTRY("getMonitoredGroup");
	// Dummy Function
	FUNCTION_EXIT;
	return 0;
}

void RadioServant::getActiveCalls(const char * tsi, const char * sessionID, const char* sessionName)
{
	FUNCTION_ENTRY("getActiveCalls()");
	// Dummy Function
	FUNCTION_EXIT;
}

ta_int32 RadioServant::startListening(ta_int32 callId, const char * sessionId, const char* sessionName)
{
	FUNCTION_ENTRY("startListening");
	// Dummy Function
	FUNCTION_EXIT;
	return 0;
}

void RadioServant::stopListening(ta_int32 callId, const char * sessionId, const char* sessionName)
{
	FUNCTION_ENTRY("stopListening()");
	// Dummy Function
	FUNCTION_EXIT;
}


ta_int32 RadioServant::joinCall(ta_int32 callId, const char * sessionId, const char* sessionName)
{
	FUNCTION_ENTRY("joinCall");
	// Dummy Function
	FUNCTION_EXIT;
	return 0;
}

void RadioServant::terminateCall(ta_int32 callId, const char * sessionId, bool bIsMonitored, const char* sessionName)
{
	FUNCTION_ENTRY("terminateCall");
	// Dummy Function
	FUNCTION_EXIT;
}

ta_int32 RadioServant::includeSubscriber(ta_int32 callId, const char * subscriber, const char * sessionId, const char* sessionName)
{
	FUNCTION_ENTRY("includeSubscriber");
	// Dummy Function
	FUNCTION_EXIT;
	return 0;
}

void RadioServant::setCallForwarding(const TA_Base_Bus::IRadioSessionCorbaDef::CallForwardDetails& callForwardDetails, const char * sessionId, const char* sessionName)
{
	FUNCTION_ENTRY("setCallForwarding");
	// Dummy Function
	FUNCTION_EXIT;
}

TA_Base_Bus::IRadioSessionCorbaDef::CallForwardDetails* RadioServant::getCallForwarding(const char* sessionName)
{
	FUNCTION_ENTRY("getCallForwarding");
	// Dummy Function
	FUNCTION_EXIT;
	return  NULL;
}

void RadioServant::selectCall(ta_int32 callId, const char * sessionId, const char* sessionName)
{
	FUNCTION_ENTRY("selectCall");
	// Dummy Function
	FUNCTION_EXIT;
}

void RadioServant::deselectCall(ta_int32 callId, const char * sessionId, const char* sessionName)
{
	FUNCTION_ENTRY("deselectCall");
	// Dummy Function
	FUNCTION_EXIT;
}

void RadioServant::adjustCallVolume(ta_int32 callId, bool increase, const char * sessionId, const char* sessionName)
{
	FUNCTION_ENTRY("adjustCallVolume");
	// Dummy Function
	FUNCTION_EXIT;
}

void RadioServant::sessionLogin(const char* loginName, const char* sessionId, const char* sessionName)
{
	FUNCTION_ENTRY("sessionLogin");
	// Dummy Function
	FUNCTION_EXIT;
}
void RadioServant::createSessionPatch(const TA_Base_Bus::IRadioSessionCorbaDef::PatchList& list, const char* patchName, const char * sessionId, const char* sessionName)
{
	FUNCTION_ENTRY("createSessionPatch");
	// Dummy Function
	FUNCTION_EXIT;
}

void RadioServant::deleteSessionPatch(const char* patchName, const char * sessionId, const char* sessionName)
{
	FUNCTION_ENTRY("deleteSessionPatch");
	// Dummy Function
	FUNCTION_EXIT;
}

void RadioServant::sessionInitialisation(ta_int32 location, ta_int32 console)
{
	FUNCTION_ENTRY("sessionInitialisation");
	// Dummy Function
	FUNCTION_EXIT;
}
::CORBA::Boolean RadioServant::getRadioSessionIsLogin(const char* sessionName)
{
	FUNCTION_ENTRY("getRadioSessionIsLogin");
	// Dummy Function
	FUNCTION_EXIT;
	return false;
}

char* RadioServant::getSessionTsi(const char* sessionName)
{
	FUNCTION_ENTRY("getSessionTsi");
	// Dummy Function
	FUNCTION_EXIT;
	return 0;
}

void RadioServant::demandTX(::CORBA::Short priority, const char* sessionID, ::CORBA::ULong& queuedID, const char* sessionName)
{
	FUNCTION_ENTRY("demandTX");
	// Dummy Function
	FUNCTION_EXIT;
}

void RadioServant::ceaseTX(const char* sessionID, const char* sessionName)
{
	FUNCTION_ENTRY("demandTX");
	// Dummy Function
	FUNCTION_EXIT;
}

void RadioServant::sendCommsMgsNewCallStack(const char* profile, ta_uint32 locationKey, ta_uint32 timestamp, TA_Base_Bus::IRadioCorbaDef::EOriginType originType, TA_Base_Bus::IRadioCorbaDef::EStackCallType callType, const char* identifier, const char* location, bool isEmergency, const char* tsi)
{
	FUNCTION_ENTRY("sendCommsMgsNewCallStack");

	// Send comms message
	TA_Base_Bus::IRadioCorbaDef::CallStackHistory callHist;
	callHist.profile = profile;
	callHist.locationKey = locationKey;
	callHist.timestamp = timestamp;
	callHist.originType = originType;
	callHist.callType = callType;
	callHist.identifier = identifier;
	callHist.location = location;
	callHist.isEmergency = isEmergency;
	callHist.tsi = tsi;

	CORBA::Any                                data;
	data <<= callHist;

	TA_Base_Core::CommsMessageSender* commsMessageSender =
		TA_Base_Core::MessagePublicationManager::getInstance().getCommsMessageSender(TA_Base_Core::RadioComms::RadioCallStatus, m_entityLocation, m_entityLocation, NsaTypeRegular);

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
		"sendCommsMgsNewCallStack()sendCommsMessage(m_EntityKey=%d,m_EntityLocation=%d)", m_EntityKey, m_entityLocation);

	commsMessageSender->sendCommsMessage(TA_Base_Core::RadioComms::RadioCallStackNotification,
		m_EntityKey,
		data,
		0,
		m_entityLocation,
		NULL);

	FUNCTION_EXIT;
}

void RadioServant::sendCommsMgsDeleteCallStack(ta_uint32 timestamp, const std::string tsi)
{
	FUNCTION_ENTRY("sendCommsMgsDeleteCallStack");

	TA_Base_Bus::IRadioCorbaDef::DeleteCallStackHistory delCallHist;
	delCallHist.timestamp = timestamp;
	delCallHist.tsi = tsi.c_str();

	CORBA::Any                                data;
	data <<= delCallHist;

	TA_Base_Core::CommsMessageSender* commsMessageSender =
		TA_Base_Core::MessagePublicationManager::getInstance().getCommsMessageSender(TA_Base_Core::RadioComms::RadioCallStatus, m_entityLocation, m_entityLocation, NsaTypeRegular);

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
		"sendCommsMgsDeleteCallStack()sendCommsMessage(m_EntityKey=%d,m_EntityLocation=%d)", m_EntityKey, m_entityLocation);

	commsMessageSender->sendCommsMessage(TA_Base_Core::RadioComms::RadioCallStackNotification,
		m_EntityKey,
		data,
		0,
		m_entityLocation,
		NULL);

	FUNCTION_EXIT;
}
