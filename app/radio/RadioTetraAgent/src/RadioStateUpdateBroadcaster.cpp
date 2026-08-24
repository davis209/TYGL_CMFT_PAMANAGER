/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/RadioStateUpdateBroadcaster.cpp $
  * @author:  John Dalin
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * The RadioStateUpdateBroadcaster is a class that is used broadcasting state updates
  * for synchronisation between the Radio Tetra Agent that is in control mode and the
  * Radio Tetra Agent that is in monitor mode.
  * mode.
  * This class uses the Singleton design pattern.
  */

#include "core/types/src/ta_types.h"
#include "core/utilities/src/RunParams.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "app/radio/RadioTetraAgent/src/RadioStateUpdateBroadcaster.h"
#include "app/radio/RadioTetraAgent/src/RadioStateMessageConverter.h"

#pragma warning (disable : 4355)

namespace TA_IRS_App
{
    RadioStateUpdateBroadcaster* RadioStateUpdateBroadcaster::instance = NULL;

    TA_Base_Core::ReEntrantThreadLockable RadioStateUpdateBroadcaster::m_singletonLockable;

    RadioStateUpdateBroadcaster* RadioStateUpdateBroadcaster::getInstance()
    {
        if (instance == NULL)
        {
            TA_Base_Core::ThreadGuard guard(m_singletonLockable);

            if (instance == NULL)
            {
                instance = new RadioStateUpdateBroadcaster();
            }
        }

        return instance;
    }
            
    RadioStateUpdateBroadcaster::RadioStateUpdateBroadcaster()
        : m_stateUpdateSender(NULL)
        , m_entityKey(0)
        , m_configured(false)
		, m_locationKey(0)
    {
    }

    RadioStateUpdateBroadcaster::~RadioStateUpdateBroadcaster()
    {
        if (m_stateUpdateSender != NULL)
            delete m_stateUpdateSender;
    }
    
    void RadioStateUpdateBroadcaster::configure(ta_uint32 entityKey, ta_uint32 locationKey)
    {
        TA_Base_Core::ThreadGuard guard(m_lockable);

        m_entityKey = entityKey;
		m_locationKey = locationKey;
        m_configured = true;

		m_stateUpdateSender =
			TA_Base_Core::MessagePublicationManager::getInstance().getStateUpdateMessageSender(
				TA_Base_Core::RadioTetraAgentStateUpdate::Context, m_locationKey, m_locationKey, TA_Base_Core::NsaTypeRegular);
    }

	void RadioStateUpdateBroadcaster::sendRadioConnectionSwitch(const RadioConnectionSwitch& message)
	{
		TA_Base_Core::ThreadGuard guard(m_lockable);
		
        if (m_configured && m_stateUpdateSender != NULL)
        {
            // Create the commsData Any
            CORBA::Any stateUpdateData;
            
            TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioConnectionSwitchRequest corbaMessage;
			
            RadioStateMessageConverter::convertRadioConnectionSwitchNormalToCorba(
                message, corbaMessage);
			
            // Inject your comms info into the Any
            stateUpdateData <<= corbaMessage;        
            
            m_stateUpdateSender->sendStateUpdateMessage(
                TA_Base_Core::RadioTetraAgentStateUpdate::RadioConnectionSwitchRequest, //Message Type
                m_entityKey, // entityKey
                TA_Base_Core::RunParams::getInstance().get( RPARAM_ENTITYNAME ), // agentName
                stateUpdateData); // information about the state update        
        }
	}

    void RadioStateUpdateBroadcaster::sendRadioUpdateCallInRadioCallStack(const RadioUpdateCallInRadioCallStack& message)
    {
        TA_Base_Core::ThreadGuard guard(m_lockable);

        if (m_configured && m_stateUpdateSender != NULL)
        {
            // Create the commsData Any
            CORBA::Any stateUpdateData;
            
            TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioUpdateCallInRadioCallStack corbaMessage;

            RadioStateMessageConverter::convertRadioUpdateCallInRadioCallStackNormalToCorba(
                message, corbaMessage);

            // Inject your comms info into the Any
            stateUpdateData <<= corbaMessage;        
            
            m_stateUpdateSender->sendStateUpdateMessage(
                TA_Base_Core::RadioTetraAgentStateUpdate::RadioUpdateCallInRadioCallStack, //Message Type
                m_entityKey, // entityKey
                TA_Base_Core::RunParams::getInstance().get( RPARAM_ENTITYNAME ), // agentName
                stateUpdateData); // information about the state update        
        }
    }
    
    void RadioStateUpdateBroadcaster::sendRadioRemoveCallFromRadioCallStack(const RadioRemoveCallFromRadioCallStack& message)
    {
        TA_Base_Core::ThreadGuard guard(m_lockable);

        if (m_configured && m_stateUpdateSender != NULL)
        {
            // Create the commsData Any
            CORBA::Any stateUpdateData;
            
            TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioRemoveCallFromRadioCallStack corbaMessage;

            RadioStateMessageConverter::convertRadioRemoveCallFromRadioCallStackNormalToCorba(
                message, corbaMessage);

            // Inject your comms info into the Any
            stateUpdateData <<= corbaMessage;       
            
            m_stateUpdateSender->sendStateUpdateMessage(
                TA_Base_Core::RadioTetraAgentStateUpdate::RadioRemoveCallFromRadioCallStack, //Message Type
                m_entityKey, // entityKey
                TA_Base_Core::RunParams::getInstance().get( RPARAM_ENTITYNAME ), // agentName
                stateUpdateData); // information about the state update 
        }
    }

	void RadioStateUpdateBroadcaster::sendRadioUpdateRadioSession(const RadioUpdateRadioSession& message)
	{
        TA_Base_Core::ThreadGuard guard(m_lockable);

        if (m_configured && m_stateUpdateSender != NULL)
        {
            // Create the commsData Any
            CORBA::Any stateUpdateData;
            
            TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioUpdateRadioSession corbaMessage;

            RadioStateMessageConverter::convertRadioUpdateRadioSessionNormalToCorba(
                message, corbaMessage);

            // Inject your comms info into the Any
            stateUpdateData <<= corbaMessage;       
            
            m_stateUpdateSender->sendStateUpdateMessage(
                TA_Base_Core::RadioTetraAgentStateUpdate::RadioUpdateRadioSession, //Message Type
                m_entityKey, // entityKey
                TA_Base_Core::RunParams::getInstance().get( RPARAM_ENTITYNAME ), // agentName
                stateUpdateData); // information about the state update 
			
        }
	}

	void RadioStateUpdateBroadcaster::sendRadioUpdateMonitoredCallProgression(const RadioUpdateMonitoredCallProgression& message)
	{									  	
        TA_Base_Core::ThreadGuard guard(m_lockable);

        if (m_configured && m_stateUpdateSender != NULL)
        {
            // Create the commsData Any
            CORBA::Any stateUpdateData;
            
            TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioUpdateMonitoredCallProgression corbaMessage;

            RadioStateMessageConverter::convertRadioUpdateMonitoredCallProgressionNormalToCorba(
                message, corbaMessage);

            // Inject your comms info into the Any
            stateUpdateData <<= corbaMessage;       
            
            m_stateUpdateSender->sendStateUpdateMessage(
                TA_Base_Core::RadioTetraAgentStateUpdate::RadioUpdateRadioSession, //Message Type
                m_entityKey, // entityKey
                TA_Base_Core::RunParams::getInstance().get( RPARAM_ENTITYNAME ), // agentName
                stateUpdateData); // information about the state update 
        }
	}

	void RadioStateUpdateBroadcaster::sendRadioRemoveMonitoredSubscriberDetails(const RadioRemoveMonitoredSubscriberDetails& message)
	{									  	
        TA_Base_Core::ThreadGuard guard(m_lockable);

        if (m_configured && m_stateUpdateSender != NULL)
        {
            // Create the commsData Any
            CORBA::Any stateUpdateData;
            
            TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioRemoveMonitoredSubscriberDetails corbaMessage;

            RadioStateMessageConverter::convertRadioRemoveMonitoredSubscriberDetailsNormalToCorba(
                message, corbaMessage);

            // Inject your comms info into the Any
            stateUpdateData <<= corbaMessage;       
            
            m_stateUpdateSender->sendStateUpdateMessage(
                TA_Base_Core::RadioTetraAgentStateUpdate::RadioUpdateRadioSession, //Message Type
                m_entityKey, // entityKey
                TA_Base_Core::RunParams::getInstance().get( RPARAM_ENTITYNAME ), // agentName
                stateUpdateData); // information about the state update 
        }
	}

	void RadioStateUpdateBroadcaster::sendRadioRemoveSessionFromAudioMap( 
				const RadioRemoveSessionFromAudioMap& message)
	{
		
		TA_Base_Core::ThreadGuard guard(m_lockable);
		
        if (m_configured && m_stateUpdateSender != NULL)
        {
            // Create the commsData Any
            CORBA::Any stateUpdateData;
            
            TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioRemoveSessionFromAudioMap corbaMessage;
			
            RadioStateMessageConverter::convertRadioRemoveSessionFromAudioMapNormalToCorba(
                message, corbaMessage);
			
            // Inject your comms info into the Any
            stateUpdateData <<= corbaMessage;       
            
		   m_stateUpdateSender->sendStateUpdateMessage(
                TA_Base_Core::RadioTetraAgentStateUpdate::RadioRemoveSessionFromAudioMap	, //Message Type
                m_entityKey, // entityKey
                TA_Base_Core::RunParams::getInstance().get( RPARAM_ENTITYNAME ), // agentName
                stateUpdateData); // information about the state update
		}
	}
	
	
	void RadioStateUpdateBroadcaster::sendRadioUpdateAudioEntryInSessionAudioMap( 
				const RadioUpdateAudioEntryInSessionAudioMap& message)
	{
		TA_Base_Core::ThreadGuard guard(m_lockable);
		
        if (m_configured && m_stateUpdateSender != NULL)
        {
            // Create the commsData Any
            CORBA::Any stateUpdateData;
            
            TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioUpdateAudioEntryInSessionAudioMap corbaMessage;
			
            RadioStateMessageConverter::convertRadioUpdateAudioEntryInSessionAudioMapNormalToCorba(
                message, corbaMessage);
			
            // Inject your comms info into the Any
            stateUpdateData <<= corbaMessage;       
            
		   m_stateUpdateSender->sendStateUpdateMessage(
                TA_Base_Core::RadioTetraAgentStateUpdate::RadioUpdateAudioEntryInSessionAudioMap, //Message Type
                m_entityKey, // entityKey
                TA_Base_Core::RunParams::getInstance().get( RPARAM_ENTITYNAME ), // agentName
                stateUpdateData); // information about the state update
		}
	}
			 
	 void RadioStateUpdateBroadcaster::sendRadioUpdateRadioResourceManagerVars(
				const RadioUpdateRadioResourceManagerVars& message)
	{
		 TA_Base_Core::ThreadGuard guard(m_lockable);
		
        if (m_configured && m_stateUpdateSender != NULL)
        {
            // Create the commsData Any
            CORBA::Any stateUpdateData;
            
            TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioUpdateRadioResourceManagerVars corbaMessage;
			
            RadioStateMessageConverter::convertRadioUpdateRadioResourceManagerVarsNormalToCorba(
                message, corbaMessage);
			
            // Inject your comms info into the Any
            stateUpdateData <<= corbaMessage;       
            
		   m_stateUpdateSender->sendStateUpdateMessage(
                TA_Base_Core::RadioTetraAgentStateUpdate::RadioUpdateRadioResourceManagerVars, //Message Type
                m_entityKey, // entityKey
                TA_Base_Core::RunParams::getInstance().get( RPARAM_ENTITYNAME ), // agentName
                stateUpdateData); // information about the state update
		}
	}
	 
	 void RadioStateUpdateBroadcaster::sendRadioUpdateAuthorisationRequest(
				const RadioUpdateAuthorisationRequest& message)
	{
		 TA_Base_Core::ThreadGuard guard(m_lockable);
		
        if (m_configured && m_stateUpdateSender != NULL)
        {
            // Create the commsData Any
            CORBA::Any stateUpdateData;
            
            TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioUpdateAuthorisationRequest corbaMessage;
			
            RadioStateMessageConverter::convertRadioUpdateAuthorisationRequestNormalToCorba(
                message, corbaMessage);
			
            // Inject your comms info into the Any
            stateUpdateData <<= corbaMessage;       
            
		   m_stateUpdateSender->sendStateUpdateMessage(
                TA_Base_Core::RadioTetraAgentStateUpdate::RadioUpdateAuthorisationRequest, //Message Type
                m_entityKey, // entityKey
                TA_Base_Core::RunParams::getInstance().get( RPARAM_ENTITYNAME ), // agentName
                stateUpdateData); // information about the state update
		}
	}
	 
	 void RadioStateUpdateBroadcaster::sendRadioRemoveAuthorisationRequest(
				const RadioRemoveAuthorisationRequest& message)
	{
		 TA_Base_Core::ThreadGuard guard(m_lockable);
		
        if (m_configured && m_stateUpdateSender != NULL)
        {
            // Create the commsData Any
            CORBA::Any stateUpdateData;
            
            TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioRemoveAuthorisationRequest corbaMessage;
			
            RadioStateMessageConverter::convertRadioRemoveAuthorisationRequestNormalToCorba(
                message, corbaMessage);
			
            // Inject your comms info into the Any
            stateUpdateData <<= corbaMessage;       
            
		   m_stateUpdateSender->sendStateUpdateMessage(
                TA_Base_Core::RadioTetraAgentStateUpdate::RadioRemoveAuthorisationRequest, //Message Type
                m_entityKey, // entityKey
                TA_Base_Core::RunParams::getInstance().get( RPARAM_ENTITYNAME ), // agentName
                stateUpdateData); // information about the state update
		}
	}

	 void RadioStateUpdateBroadcaster::sendRadioUpdateCallStackHistory(
		 const RadioUpdateCallStackHistory& message)
	 {
		 TA_Base_Core::ThreadGuard guard(m_lockable);

		 if (m_configured && m_stateUpdateSender != NULL)
		 {
			 // Create the commsData Any
			 CORBA::Any stateUpdateData;

			 TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioUpdateCallStackHistory corbaMessage;

			 RadioStateMessageConverter::convertRadioUpdateCallStackHistoryNormalToCorba(
				 message, corbaMessage);

			 // Inject your comms info into the Any
			 stateUpdateData <<= corbaMessage;

			 m_stateUpdateSender->sendStateUpdateMessage(
				 TA_Base_Core::RadioTetraAgentStateUpdate::RadioUpdateCallStackHistory, //Message Type
				 m_entityKey, // entityKey
				 TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME), // agentName
				 stateUpdateData); // information about the state update
		 }
	 }

	 void RadioStateUpdateBroadcaster::sendRadioAddCallStackEntry(
		 const RadioAddCallStackEntry& message)
	 {
		 TA_Base_Core::ThreadGuard guard(m_lockable);

		 if (m_configured && m_stateUpdateSender != NULL)
		 {
			 // Create the commsData Any
			 CORBA::Any stateUpdateData;

			 TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioAddCallStackEntry corbaMessage;

			 RadioStateMessageConverter::convertRadioAddCallStackEntryNormalToCorba(
				 message, corbaMessage);

			 // Inject your comms info into the Any
			 stateUpdateData <<= corbaMessage;

			 m_stateUpdateSender->sendStateUpdateMessage(
				 TA_Base_Core::RadioTetraAgentStateUpdate::RadioAddCallStackEntry, //Message Type
				 m_entityKey, // entityKey
				 TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME), // agentName
				 stateUpdateData); // information about the state update
		 }
	 }

	 void RadioStateUpdateBroadcaster::sendRadioDeleteCallStackEntry(
		 const RadioDeleteCallStackEntry& message)
	 {
		 TA_Base_Core::ThreadGuard guard(m_lockable);

		 if (m_configured && m_stateUpdateSender != NULL)
		 {
			 // Create the commsData Any
			 CORBA::Any stateUpdateData;

			 TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioDeleteCallStackEntry corbaMessage;

			 RadioStateMessageConverter::convertRadioDeleteCallStackEntryNormalToCorba(
				 message, corbaMessage);

			 // Inject your comms info into the Any
			 stateUpdateData <<= corbaMessage;

			 m_stateUpdateSender->sendStateUpdateMessage(
				 TA_Base_Core::RadioTetraAgentStateUpdate::RadioDeleteCallStackEntry, //Message Type
				 m_entityKey, // entityKey
				 TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME), // agentName
				 stateUpdateData); // information about the state update
		 }
	 }

	 void RadioStateUpdateBroadcaster::sendRadioSubscriberUpdate(
		 const SubscriberStateUpdate& message)
	 {
		 TA_Base_Core::ThreadGuard guard(m_lockable);

		 if (m_configured && m_stateUpdateSender != NULL)
		 {
			 // Create the commsData Any
			 CORBA::Any stateUpdateData;

			 TA_Base_Bus::IRadioStateSynchronisationCorbaDef::SubscriberStateUpdate corbaMessage;

			 RadioStateMessageConverter::convertRadioSubscriberUpdateNormalToCorba(
				 message, corbaMessage);

			 // Inject your comms info into the Any
			 stateUpdateData <<= corbaMessage;

			 m_stateUpdateSender->sendStateUpdateMessage(
				 TA_Base_Core::RadioTetraAgentStateUpdate::RadioSubscriberStateUpdate, //Message Type
				 m_entityKey, // entityKey
				 TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME), // agentName
				 stateUpdateData); // information about the state update
		 }
	 }
}



