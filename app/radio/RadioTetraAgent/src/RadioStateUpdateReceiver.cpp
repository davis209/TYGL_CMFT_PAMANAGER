/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/RadioStateUpdateReceiver.cpp $
  * @author  John Dalin
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2008/11/28 16:26:01 $
  * Last modified by : $Author: builder $
  *
  * The RadioStateUpdateReceiver is a class that is used to receive state update messages from
  * from the peer Radio Tetra Agent that is in control mode. It applies these state updates
  * onto the Radio Tetra Agent.
  *
  */

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/radio/RadioTetraAgent/src/RadioStateUpdateReceiver.h"

namespace TA_IRS_App
{
    RadioStateUpdateReceiver::RadioStateUpdateReceiver(IRadioTetraAgentState& radioState)
        : m_terminate(false)
        , m_processing(false)
        , m_radioTetraAgentState(radioState)
    {
        FUNCTION_ENTRY("Constructor");
        
        start();

        FUNCTION_EXIT;
    }
    
    RadioStateUpdateReceiver::~RadioStateUpdateReceiver()
    {
        FUNCTION_ENTRY("Destructor");

        terminateAndWait();

        FUNCTION_EXIT;
    }
    
    void RadioStateUpdateReceiver::run()
    {
        FUNCTION_ENTRY("run");

        while (!m_terminate)
        {
            // Check if processing and there are messages
            RadioStateUpdateMessage* currentMessage = 0;
            bool processingAndHasMessages;

            {
                TA_Base_Core::ThreadGuard guard(m_lockable);

                processingAndHasMessages = m_processing && !m_stateUpdateMessages.empty();

                if (processingAndHasMessages)
                {
                    currentMessage = m_stateUpdateMessages.front();
                    m_stateUpdateMessages.pop();
                }
            }

            if (!processingAndHasMessages)
            {
                // Wait until resume processing, new messages or terminate
                m_runCondition.wait();
                continue;
            }
            else
            {
                // Process the next message on the queue
                processMessage(*currentMessage);
                delete currentMessage;
                m_waitUntilAllProcessedCondition.broadcast();
            }
        }

        FUNCTION_EXIT;
    }
    
    void RadioStateUpdateReceiver::terminate()
    {
        FUNCTION_ENTRY("terminate");

        TA_Base_Core::ThreadGuard guard(m_lockable);

        // Terminate the thread, stop subscription to the peer agent, pause procesing
        // and clear any state update messages that are queued.
        m_terminate = true;
        pauseProcessing();
        clearStateUpdateMessages();
        m_runCondition.signal();
        m_waitUntilAllProcessedCondition.broadcast();

        FUNCTION_EXIT;
    }
    
    void RadioStateUpdateReceiver::resumeProcessing()
    {
        FUNCTION_ENTRY("resumeProcessing");

        TA_Base_Core::ThreadGuard guard(m_lockable);

        if (!m_processing)
        {
            m_processing = true;
            m_runCondition.signal();
        }

        FUNCTION_EXIT;
    }
    
    void RadioStateUpdateReceiver::pauseProcessing()
    {
        FUNCTION_ENTRY("pauseProcessing");

        TA_Base_Core::ThreadGuard guard(m_lockable);

        if (m_processing)
        {
            m_processing = false;
        }

        FUNCTION_EXIT;
    }
    
    void RadioStateUpdateReceiver::waitUntilAllProcessed()
    {
        FUNCTION_ENTRY("waitUntilAllProcessed");

        // Check that thread not terminated to continue
        if (!m_terminate)
        {
            bool empty;

            {
                TA_Base_Core::ThreadGuard guard(m_lockable);

                empty = m_stateUpdateMessages.empty();
            }

            // wait if there are still messages to be processed and not terminated
            while (!empty && !m_terminate)
            {
                m_waitUntilAllProcessedCondition.wait();

                {
                    TA_Base_Core::ThreadGuard guard(m_lockable);

                    empty = m_stateUpdateMessages.empty();
                }
            }
        }

        FUNCTION_EXIT;
    }
    
    void RadioStateUpdateReceiver::clearStateUpdateMessages()
    {
        FUNCTION_ENTRY("clearStateUpdateMessages");
        
        TA_Base_Core::ThreadGuard guard(m_lockable);
        
        unsigned int queueSize = m_stateUpdateMessages.size();
        
        // Remove all state update messages from the queue
        for( unsigned int i = 0; i < queueSize; ++i )
        {
            RadioStateUpdateMessage* currentMessage = m_stateUpdateMessages.front();
            m_stateUpdateMessages.pop();
            delete currentMessage;
        }
        
        m_waitUntilAllProcessedCondition.broadcast();
        
        FUNCTION_EXIT;
    }
    
    void RadioStateUpdateReceiver::receiveStateUpdateMessage(RadioStateUpdateMessage& message)
    {
        FUNCTION_ENTRY("receiveStateUpdateMessage");

        TA_Base_Core::ThreadGuard guard(m_lockable);

        // Insert into the state update message into the queue to be processed
        m_stateUpdateMessages.push(&message);
        m_runCondition.signal();

        FUNCTION_EXIT;
    }

    void RadioStateUpdateReceiver::processMessage(const RadioStateUpdateMessage& message)
    {
        FUNCTION_ENTRY("processMessage");

        try
        {   // TD9016 Add logs for all state updates 
            switch (message.getType())
            {
            case RadioStateUpdateMessage::UPDATE_CALL_IN_RADIO_CALL_STACK:
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "RadioStateUpdateMessage::UPDATE_CALL_IN_RADIO_CALL_STACK" );
                receiveRadioUpdateCallInRadioCallStackMessage(dynamic_cast<const RadioUpdateCallInRadioCallStack&> (message));
                break;
            case RadioStateUpdateMessage::REMOVE_CALL_FROM_RADIO_CALL_STACK:
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "RadioStateUpdateMessage::REMOVE_CALL_FROM_RADIO_CALL_STACK" );
                receiveRadioRemoveCallFromRadioCallStackMessage(dynamic_cast<const RadioRemoveCallFromRadioCallStack&> (message));
                break;
			case RadioStateUpdateMessage::UPDATE_RADIO_SESSION:
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "RadioStateUpdateMessage::UPDATE_RADIO_SESSION" );
                receiveRadioUpdateRadioSessionMessage(dynamic_cast<const RadioUpdateRadioSession&> (message));
                break;
			case RadioStateUpdateMessage::UPDATE_MONITORED_CALL_PROGRESSION:
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "RadioStateUpdateMessage::UPDATE_MONITORED_CALL_PROGRESSION" );
                receiveRadioMonitoredCallProgressionMessage(dynamic_cast<const RadioUpdateMonitoredCallProgression&> (message));
                break;	
			case RadioStateUpdateMessage::REMOVE_MONITORED_SUBSCRIBER_DETAILS:
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "RadioStateUpdateMessage::REMOVE_MONITORED_SUBSCRIBER_DETAILS" );
				receiveRadioRemoveMonitoredSubscriberDetailsMessage(dynamic_cast<const RadioRemoveMonitoredSubscriberDetails&> (message));
				break;
			case RadioStateUpdateMessage::REMOVE_SESSION_FROM_AUDIO_MAP:
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "RadioStateUpdateMessage::REMOVE_SESSION_FROM_AUDIO_MAP" );
                receiveRadioRemoveSessionFromAudioMapMessage(dynamic_cast<const RadioRemoveSessionFromAudioMap&> (message));
                break;
            case RadioStateUpdateMessage::UPDATE_AUDIO_ENTRY_IN_SESSION_AUDIO_MAP:
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "RadioStateUpdateMessage::UPDATE_AUDIO_ENTRY_IN_SESSION_AUDIO_MAP" );
                receiveRadioUpdateAudioEntryInSessionAudioMapMessage(dynamic_cast<const RadioUpdateAudioEntryInSessionAudioMap&> (message));
                break;
			case RadioStateUpdateMessage::UPDATE_RADIO_RESOURCE_MANAGER_VARS:
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "RadioStateUpdateMessage::UPDATE_AUDIO_ENTRY_IN_SESSION_AUDIO_MAP" );
                receiveRadioUpdateRadioResourceManagerVarsMessage(dynamic_cast<const RadioUpdateRadioResourceManagerVars&> (message));
                break;
			case RadioStateUpdateMessage::UPDATE_AUTHORISATION_REQUEST:
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "RadioStateUpdateMessage::UPDATE_AUTHORISATION_REQUEST" );
                receiveRadioUpdateAuthorisationRequestMessage(dynamic_cast<const RadioUpdateAuthorisationRequest&> (message));
                break;	
			case RadioStateUpdateMessage::REMOVE_AUTHORISATION_REQUEST:
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "RadioStateUpdateMessage::REMOVE_AUTHORISATION_REQUEST" );
				receiveRadioRemoveAuthorisationRequestMessage(dynamic_cast<const RadioRemoveAuthorisationRequest&> (message));
				break;
			case RadioStateUpdateMessage::CONNECTION_SWITCH:
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "RadioStateUpdateMessage::CONNECTION_SWITCH" );
				receiveRadioConnectionSwitchMessage(dynamic_cast<const RadioConnectionSwitch&> (message));
				break;
			case RadioStateUpdateMessage::UPDATE_CALL_STACK_HISTORY:
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "RadioStateUpdateMessage::UPDATE_CALL_STACK_HISTORY");
				receiveRadioUpdateCallStackHistoryMessage(dynamic_cast<const RadioUpdateCallStackHistory&> (message));
				break;
			case RadioStateUpdateMessage::ADD_CALL_STACK_HISTORY:
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "RadioStateUpdateMessage::ADD_CALL_STACK_HISTORY");
				receiveRadioAddCallStackEntryMessage(dynamic_cast<const RadioAddCallStackEntry&> (message));
				break;
			case RadioStateUpdateMessage::DELETE_CALL_STACK_HISTORY:
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "RadioStateUpdateMessage::DELETE_CALL_STACK_HISTORY");
				receiveRadioDeleteCallStackEntryMessage(dynamic_cast<const RadioDeleteCallStackEntry&> (message));
				break;
			case RadioStateUpdateMessage::SUBSCRIBER_UPDATE:
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "RadioStateUpdateMessage::SUBSCRIBER_UPDATE");
				receiveRadioSubscriberUpdate(dynamic_cast<const SubscriberStateUpdate&> (message));
				break;
            default:
                // should not occur
                ;
            }
        }
        catch (std::bad_cast)
        {
        }
		catch ( const TA_Base_Core::TransactiveException& e )
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "TA_Base_Core::TransactiveException", e.what() );
		}

        FUNCTION_EXIT;
    }

    void RadioStateUpdateReceiver::receiveRadioUpdateCallInRadioCallStackMessage(
                    const RadioUpdateCallInRadioCallStack &message)
    {
        m_radioTetraAgentState.updateCallInRadioCallStack(message.callDetails);
    }

	void RadioStateUpdateReceiver::receiveRadioConnectionSwitchMessage( const RadioConnectionSwitch &message)
	{
		m_radioTetraAgentState.updateConnectionSwitch(message.controlConnectionStatus, message.TCPServerIndex);
	}

    void RadioStateUpdateReceiver::receiveRadioRemoveCallFromRadioCallStackMessage(
                    const RadioRemoveCallFromRadioCallStack &message)
    {
        m_radioTetraAgentState.removeCallFromRadioCallStack(message.callID, message.consoleID);
    }

	void RadioStateUpdateReceiver::receiveRadioUpdateRadioSessionMessage(
		const RadioUpdateRadioSession& message)
	{
		m_radioTetraAgentState.updateRadioSession(message.servantEntityKey,
												  message.radioSession);
	}
	void RadioStateUpdateReceiver::receiveRadioMonitoredCallProgressionMessage(
		const RadioUpdateMonitoredCallProgression& message)
	{
		m_radioTetraAgentState.updateRadioMonitoredCallProgression(message.monitorReference);
	}
	
	void RadioStateUpdateReceiver::receiveRadioRemoveMonitoredSubscriberDetailsMessage(
		const RadioRemoveMonitoredSubscriberDetails& message)
	{
		m_radioTetraAgentState.removeMonitoredSubscriberDetails(message.monitorReference, message.serverIndex);
												  
	}

	void RadioStateUpdateReceiver:: receiveRadioRemoveSessionFromAudioMapMessage(
		const RadioRemoveSessionFromAudioMap& message)
	{
			m_radioTetraAgentState.removeSessionFromAudioMap(message.sessionReference, message.serverIndex);
	}
	void RadioStateUpdateReceiver::	receiveRadioUpdateAudioEntryInSessionAudioMapMessage(
		const RadioUpdateAudioEntryInSessionAudioMap& message)
	{
			m_radioTetraAgentState.updateAudioEntryInSessionMap(message.sessionReference,
																message.audioReference,
																message.audio);
	}
	void RadioStateUpdateReceiver::	receiveRadioUpdateRadioResourceManagerVarsMessage(
		const RadioUpdateRadioResourceManagerVars& message)
	{
			m_radioTetraAgentState.updateRadioResourceManagerVars(message.sessionReference, message.callReference, 
																	message.currrentVolume, message.serverIndex);
	}
	void RadioStateUpdateReceiver::	receiveRadioUpdateAuthorisationRequestMessage(
		const RadioUpdateAuthorisationRequest& message)
	{
			m_radioTetraAgentState.updateAuthorisationRequest(message.autorisationRequest, message.serverIndex);
	}
	void RadioStateUpdateReceiver::	receiveRadioRemoveAuthorisationRequestMessage(
		const RadioRemoveAuthorisationRequest& message)
	{
			m_radioTetraAgentState.removeAuthorisationRequest(message.callID, message.serverIndex);
	}

	void RadioStateUpdateReceiver::receiveRadioUpdateCallStackHistoryMessage(
		const RadioUpdateCallStackHistory& message)
	{
		m_radioTetraAgentState.updateCallStackHistory(message.callStackHistoryList, message.isRetrieved);
	}

	void RadioStateUpdateReceiver::receiveRadioAddCallStackEntryMessage(
		const RadioAddCallStackEntry& message)
	{
		m_radioTetraAgentState.addCallStackEntry(message.profile, message.locationKey, message.timestamp, message.originType, message.callType, message.identifier, message.location, message.isEmergency, message.tsi);
	}

	void RadioStateUpdateReceiver::receiveRadioDeleteCallStackEntryMessage(
		const RadioDeleteCallStackEntry& message)
	{
		m_radioTetraAgentState.deleteCallStackEntry(message.timestamp, message.tsi);
	}

	void RadioStateUpdateReceiver::receiveRadioSubscriberUpdate(
		const SubscriberStateUpdate& message)
	{	
		m_radioTetraAgentState.updateSubscriber(message);
	}
} // namespace TA_IRS_App
