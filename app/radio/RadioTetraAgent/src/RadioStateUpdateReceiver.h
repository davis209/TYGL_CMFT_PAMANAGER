#ifndef StateUpdateReceiver_h
#define StateUpdateReceiver_h
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/RadioStateUpdateReceiver.h $
  * @author:  John Dalin
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * The StateUpdateReceiver is a class that is used to receive state update messages from
  * from the peer Radio Tetra Agent that is in control mode. It applies these state updates
  * onto the Radio Tetra Agent.
  */

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/Condition.h"
#include "app/radio/RadioTetraAgent/src/IRadioTetraAgentState.h"

#include <queue>

namespace TA_IRS_App
{

    class RadioStateUpdateReceiver : public TA_Base_Core::Thread
    {
        public:
            /**
              * Constructor
              *
              * @param radioState - the radio agent state of this process
              */
            RadioStateUpdateReceiver(IRadioTetraAgentState& radioState);
            
            /**
              * Destructor
              */
            virtual ~RadioStateUpdateReceiver();
            
            /**
              * run
              *
              * The thread run method.
              */
            virtual void run();
            
            /**
              * terminate
              *
              * The thread terminate method.
              */
            virtual void terminate();

            /**
              * resumeProcessing
              *
              * Called to resume processing any state update messages that have
              * been queued in the state update message queue.
              * If already processing the queue, then there will be no effect.
              */
            void resumeProcessing();

            /** 
              * pauseProcessing
              *
              * Called to pause processing any state update messages that have
              * been queued in the state update message queue.
              * If already paused, the there will be no effect.
              * If there is a state update currently being processed it will
              * finish being applied.
              */
            void pauseProcessing();

            /**
              * waitUntilAllProcessed
              *
              * Waits until the StateUpdateReceiver has finished processing all the
              * the state update messages within the queue.
              */
            void waitUntilAllProcessed();

            /** 
              * clearStateUpdateMessages
              * 
              * Clears all the state update messages that are queued.
              * If there is a state update currently being processed it will
              * finish being applied.
              */
            void clearStateUpdateMessages();
            
            /**
              * receiveStateUpdateMessage
              *
              * @param message - The message that is received from the peer agent
              *                  The message should be destroyed once processed.
              */
            void receiveStateUpdateMessage(RadioStateUpdateMessage& message);

        private:
            /**
              * processMessage
              *
              * Processes a message by checking its type then calling the
              * appropriate method that handles it.
              */
            void processMessage(const RadioStateUpdateMessage& message);

			/** 
              * receiveRadioUpdateCallInCallStackMessage
              *
              * @param message - The Radio call details to update or add
              */
            void receiveRadioConnectionSwitchMessage(
                    const RadioConnectionSwitch &message);


            /** 
              * receiveRadioUpdateCallInCallStackMessage
              *
              * @param message - The Radio call details to update or add
              */
            void receiveRadioUpdateCallInRadioCallStackMessage(
                    const RadioUpdateCallInRadioCallStack &message);

            /**
              * receiveRadioRemoveCallInCallStackMessage
              *
              * @param message - The Radio Call ID to remove from the call stack
              */
            void receiveRadioRemoveCallFromRadioCallStackMessage(
                    const RadioRemoveCallFromRadioCallStack &message);

            /**
              * receiveRadioUpdateRadioSessionMessage
              *
              * @param message - The Radio Session data to update
              */
			void receiveRadioUpdateRadioSessionMessage(
					const RadioUpdateRadioSession& message);
			/**
              * receiveRadioMonitoredCallProgressionMessage
              *
              * @param message - The Monitored Call Progression data to update
              */
			void receiveRadioMonitoredCallProgressionMessage(
					const RadioUpdateMonitoredCallProgression& message);
			
			/**
              * receiveRadioRemoveMonitoredSubscriberDetailsMessage
              *
              * @param message - The Radio Monitored Subscriber Detials to remove from the call stack
              */
			void receiveRadioRemoveMonitoredSubscriberDetailsMessage(
					const RadioRemoveMonitoredSubscriberDetails& message);

			/**
			  * receiveRadioRemoveSessionFromAudioMapMessage
			  *
			  * @param message - The SessionReference to remove the AudioMap from SessionAudioMap
			  */
			void receiveRadioRemoveSessionFromAudioMapMessage(
					const RadioRemoveSessionFromAudioMap& message);

			/**
			  * receiveRadioUpdateAudioEntryInSessionAudioMapMessage
			  *
			  * @param message - AudioEntry Detail in SessionAudioMap to Update.	  
			  */
			 void receiveRadioUpdateAudioEntryInSessionAudioMapMessage(
					const RadioUpdateAudioEntryInSessionAudioMap& message);

			/**
			  * receiveRadioUpdateRadioResourceManagerVarsMessage
			  *
			  * @param message - Radio Resource Manager Variable to Update 			  
			  */
			 void receiveRadioUpdateRadioResourceManagerVarsMessage(
					const RadioUpdateRadioResourceManagerVars& message);

		    /**
			  * receiveRadioUpdateAuthorisationRequestMessage
			  *
			  * @param message - The AuthorisationRequest Details to Update			   
			  */
			 void receiveRadioUpdateAuthorisationRequestMessage(
					const RadioUpdateAuthorisationRequest& message);
			 
			/**
			  * receiveRadioRemoveAuthorisationRequestMessage
			  *
			  * @param message - The Radio CallID to remove the AuthorisationRequest Details			   
			  */
			 void receiveRadioRemoveAuthorisationRequestMessage(
					const RadioRemoveAuthorisationRequest& message);

			 /**
			   * receiveRadioUpdateCallStackHistoryMessage
			   *
			   * @param message - The vector of CallStackHistory entries
			   */
			 void receiveRadioUpdateCallStackHistoryMessage(
				 const RadioUpdateCallStackHistory& message);

			 /**
			   * receiveRadioAddCallStackEntryMessage
			   *
			   * @param message - The Call Stack Entry details to be added
			   */
			 void receiveRadioAddCallStackEntryMessage(
				 const RadioAddCallStackEntry& message);

			 void receiveRadioDeleteCallStackEntryMessage(
				 const RadioDeleteCallStackEntry& message);

			 void receiveRadioSubscriberUpdate(
				 const SubscriberStateUpdate& message);

            bool m_terminate; // for terminating the thread

            // Is the StateUpdateReceiver currently processing state update messages?
            bool m_processing;

            std::queue< RadioStateUpdateMessage* > m_stateUpdateMessages;

            IRadioTetraAgentState& m_radioTetraAgentState;

            TA_Base_Core::Condition m_waitUntilAllProcessedCondition;

            TA_Base_Core::Condition m_runCondition;

            TA_Base_Core::ReEntrantThreadLockable m_lockable;

    }; // class StateUpdateReceiver
};  // namespace TA_IRS_App

#endif // #ifndef StateUpdateReceiver_h
