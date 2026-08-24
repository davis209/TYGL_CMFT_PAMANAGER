#ifndef RadioStateUpdateBroadcaster_h
#define RadioStateUpdateBroadcaster_h
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/RadioStateUpdateBroadcaster.h $
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
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/StateUpdateMessageSender.h"
#include "core/message/types/RadioTetraAgentStateUpdate_MessageTypes.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "RadioSynchronisationTypes.h"

namespace TA_IRS_App
{

    class RadioStateUpdateBroadcaster
    {
        public:
            
            /**
              * getInstance
              *
              * @return The single instance of RadioStateUpdateBroadcaster
              */
            static RadioStateUpdateBroadcaster* getInstance();
            
            /**
              * Destructor
              */
            ~RadioStateUpdateBroadcaster();
            
            /**
              * configure
              *
              * @param entityKey - The entity to broadcast the messages towards
              * @param agentName - The name of the agent to broadcast towards
              */
            void configure(ta_uint32 entityKey, ta_uint32 locationKey);

            /**
              * sendRadioUpdateCallInRadioCallStack
              *
              * A call has been added or updated in the radio call stack.
              *
              * @param message - The message to be broadcast
              */
            void sendRadioConnectionSwitch(const RadioConnectionSwitch& message);

			/**
              * sendRadioUpdateCallInRadioCallStack
              *
              * A call has been added or updated in the radio call stack.
              *
              * @param message - The message to be broadcast
              */
            void sendRadioUpdateCallInRadioCallStack(const RadioUpdateCallInRadioCallStack& message);

            /**
              * sendRadioRemoveCallFromRadioCallStack
              *
              * A call has been removed from the radio call stack.
              *
              * @param message - The message to be broadcast
              */
            void sendRadioRemoveCallFromRadioCallStack(const RadioRemoveCallFromRadioCallStack& message);

			/**
			  * sendRadioUpdateRadioSession
			  *
			  * A radio session object has been updated
			  *
			  * @param message - The message to be broadcast
			  */
			void sendRadioUpdateRadioSession(const RadioUpdateRadioSession& message);
			
			/**
			  * sendRadioUpdateMonitoredCallProgression
			  *
			  * A radio Monitored Call Progression object has been updated
			  *
			  * @param message - The message to be broadcast
			  */
			void sendRadioUpdateMonitoredCallProgression(const RadioUpdateMonitoredCallProgression& message);

			/**
              * sendRadioRemoveMonitoredSubscriberDetails
              *
              * A call has been removed from the radio.
              *
              * @param message - The message to be broadcast
              */
			void sendRadioRemoveMonitoredSubscriberDetails(const RadioRemoveMonitoredSubscriberDetails& message);

			/**
			  * sendRadioRemoveSessionFromAudioMap
			  *
			  * The AudioMap in SessionAudioMap has been removed
			  *
              * @param message - The message to be broadcast
			  */
			void sendRadioRemoveSessionFromAudioMap( const RadioRemoveSessionFromAudioMap& message);

			/**
			  * sendRadioUpdateAudioEntryInSessionAudioMap
			  *
			  * The AudioEntry Details in SessionAudioMap has been Updated.
			  *
              * @param message - The message to be broadcast
			  */
			 void sendRadioUpdateAudioEntryInSessionAudioMap( 
					const RadioUpdateAudioEntryInSessionAudioMap& message);

			/**
			  * sendRadioUpdateRadioResourceManagerVars
			  *
			  * The RadioResourceManager Variable has been Updated 	
			  *
              * @param message - The message to be broadcast
			  */
			 void sendRadioUpdateRadioResourceManagerVars(
					const RadioUpdateRadioResourceManagerVars& message);

			/**
			  * sendRadioUpdateAuthorisationRequest
			  *
			  * The AuthorisationRequest Details has been Updated	
			  *
              * @param message - The message to be broadcast
			  */
			 void sendRadioUpdateAuthorisationRequest(
					const RadioUpdateAuthorisationRequest& message);

		    /**
			  * sendRadioRemoveAuthorisationRequest
			  *
			  * The AuthorisationRequest Details has been removed
			  *
              * @param message - The message to be broadcast
			  */
			 void sendRadioRemoveAuthorisationRequest(
					const RadioRemoveAuthorisationRequest& message);							

			 /**
			   * sendRadioUpdateCallStackHistory
			   *
			   * The Call Stack History has been Updated
			   *
			   * @param message - The message to be broadcast
			   */
			 void sendRadioUpdateCallStackHistory(
				 const RadioUpdateCallStackHistory& message);

			 /**
			   * sendRadioUpdateCallStackHistory
			   *
			   * The Call Stack Entry has been Updated
			   *
			   * @param message - The message to be broadcast
			   */
			 void sendRadioAddCallStackEntry(
				 const RadioAddCallStackEntry& message);

			 /**
			   * sendRadioDeleteCallStackEntry
			   *
			   * The Call Stack Entry has been Deleted
			   *
			   * @param message - The message to be broadcast
			   */
			 void sendRadioDeleteCallStackEntry(
				 const RadioDeleteCallStackEntry& message);

			 /*
			 * sendRadioSubscriberUpdate
			 *
			 * A subscriber has been added/updated/deleted 
			 */ 
			 void sendRadioSubscriberUpdate(
				const SubscriberStateUpdate& message);

        private:

            /**
              * Constructor
              */
            RadioStateUpdateBroadcaster();
            
            // Single instance of this class
            static RadioStateUpdateBroadcaster* instance;

            static TA_Base_Core::ReEntrantThreadLockable m_singletonLockable;

            TA_Base_Core::StateUpdateMessageSender* m_stateUpdateSender;
            ta_uint32 m_entityKey;
            bool m_configured;
			ta_uint32 m_locationKey;

            TA_Base_Core::ReEntrantThreadLockable m_lockable;
    }; // class RadioStateUpdateBroadcaster
};  // namespace TA_IRS_App

#endif // #ifndef RadioStateUpdateBroadcaster_h
