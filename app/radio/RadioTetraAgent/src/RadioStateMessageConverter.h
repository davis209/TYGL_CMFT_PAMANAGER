#ifndef RadioStateMessageConverter_h
#define RadioStateMessageConverter_h
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/RadioStateMessageConverter.h $
  * @author:  John Dalin
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/03/19 17:00:25 $
  * Last modified by:  $Author: builder $
  *
  * The RadioStateMessageConverter is a class that is used for converting CORBA message
  * structures to normal c-style structures and reverse. This is necessary to reduce coupling between
  * CORBA and the agent.
  */

#include "bus/radio/RadioAgentCorbaDef_4669/idl/src/IRadioCorbaDef.h"
#include "bus/radio/RadioAgentCorbaDef_4669/idl/src/IRadioStateSynchronisationCorbaDef.h"
#include "RadioTypes.h"
#include "RadioSynchronisationTypes.h"

namespace TA_IRS_App
{

    class RadioStateMessageConverter
    {
        public:
            
            // **************************************************************
            // * Corba structure to Normal c-structure conversion functions *
            // **************************************************************
			static void convertAudioEntryCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::AudioEntry& audioEntryCorba,
                AudioEntryType& audioEntry);
			
			static void convertAudioEntryMapTypeCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::AudioEntryMapType& audioEntryMapCorba,
                AudioEntryMapType& audioEntryMap);

			static void convertSessionAudioMapTypeCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::SessionAudioMapType& sessionAudioMapCorba,
                SessionAudioMapType& sessionAudioMap);

			static void convertRadioResourceManagerCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioResourceManagerType& radioResourceManagerCorba,
                RadioResourceManagerType& radioResourceManager);

		
			static void convertAuthorisationRequestTypeCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::AuthorisationRequestType& authorisationRequestCorba,
                AuthorisationRequestType& authorisationRequest);
			
			static void convertAuthorisationManagerCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioAuthorisationManagerType& radioAuthorisationManagerCorba,
                RadioAuthorisationManagerType& radioAuthorisationManager);
            static void convertEPTTStateCorbaToNormal(const TA_Base_Bus::IRadioCorbaDef::EPTTState& pttStateCorba,
                EPTTState& pttState);
            
            static void convertCallParticipantCorbaToNormal(const TA_Base_Bus::IRadioCorbaDef::CallParticipant& callParticipantCorba,
                CallParticipant& callParticipant);            
            
            static void convertECallCategoryCorbaToNormal(const TA_Base_Bus::IRadioCorbaDef::ECallCategory& callCategoryCorba,
                ECallCategory& callCategory);
            
            static void convertECallSourceCorbaToNormal(const TA_Base_Bus::IRadioCorbaDef::ECallSource& callSourceCorba,
                ECallSource &callSource);
            
            static void convertECallTypeCorbaToNormal(const TA_Base_Bus::IRadioCorbaDef::ECallType& callTypeCorba,
                ECallType& callType);
            
            static void convertCallDetailsTypeCorbaToNormal(const TA_Base_Bus::IRadioCorbaDef::CallDetailsType& callDetailsCorba,
                CallDetailsType& callDetails);
            
            static void convertECallStatusTypeCorbaToNormal(const TA_Base_Bus::IRadioCorbaDef::ECallStatusType& callStatusTypeCorba,
                ECallStatusType& callStatusType);

			static void convertRadioCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioType& radioCorba,
                RadioType& radio);

			static void convertTcpServerCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::TcpServerType& radioCorba,
                TcpServerType& radio);
            
            static void convertRadioServantCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioServantType& radioServantCorba,
                RadioServantType& radioServant);
            
			static void convertRadioSessionServantCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioSessionServantType& radioSessionServantCorba,
				RadioSessionServantType& radioSessionServant);

            static void convertRadioTetraAgentCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioTetraAgentType& radioTetraAgentCorba,
                RadioTetraAgentType& radioTetraAgent);
            
            static void convertRadioUpdateCallInRadioCallStackCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioUpdateCallInRadioCallStack& messageCorba,
                RadioUpdateCallInRadioCallStack& message);
            
            static void convertRadioRemoveCallFromRadioCallStackCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioRemoveCallFromRadioCallStack& messageCorba,
                RadioRemoveCallFromRadioCallStack& message);

			static void convertRadioUpdateRadioSessionCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioUpdateRadioSession& messageCorba,
				RadioUpdateRadioSession& message);
			
			static void convertRadioRemoveSessionFromAudioMapCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioRemoveSessionFromAudioMap& messageCorba,
				RadioRemoveSessionFromAudioMap& message);

			static void convertRadioUpdateMonitoredCallProgressionCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioUpdateMonitoredCallProgression& messageCorba,
				RadioUpdateMonitoredCallProgression& message);


			static void convertRadioRemoveMonitoredSubscriberDetailsCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioRemoveMonitoredSubscriberDetails&  messageCorba,
				RadioRemoveMonitoredSubscriberDetails& message);
			
			static void convertRadioUpdateAudioEntryInSessionAudioMapCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioUpdateAudioEntryInSessionAudioMap& messageCorba,
				RadioUpdateAudioEntryInSessionAudioMap& message);

			static void convertRadioUpdateRadioResourceManagerVarsCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioUpdateRadioResourceManagerVars& messageCorba,
				RadioUpdateRadioResourceManagerVars& message);

			static void convertRadioUpdateAuthorisationRequestCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioUpdateAuthorisationRequest& messageCorba,
				RadioUpdateAuthorisationRequest& message);

			static void convertRadioRemoveAuthorisationRequestCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioRemoveAuthorisationRequest& messageCorba,
				RadioRemoveAuthorisationRequest& message);			
			
			static void convertRadioMonitoredCallProgressionCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioUpdateMonitoredCallProgression& messageCorba,
				RadioUpdateMonitoredCallProgression& message);
			static void convertRadioSessionCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioSessionType& radioSessionCorba,
				RadioSessionType& radioSession);

			static void convertESessionStateCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::ESessionState& sessionStateCorba,
				ESessionState& sessionState);
			
			static void convertRadioMonitorManagerTypeCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioMonitorManagerType& radioMonitorCorba,
				RadioMonitorManagerType& radioMonitor);
			
			static void convertMonitoredSubscriberDetailsTypeCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::MonitoredSubscriberDetailsType& radioMonitorSubscriberCorba,
				MonitoredSubscriberDetailsType& radioMonitorSubscriber);

			static void convertRadioConnectionSwitchCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioConnectionSwitchRequest& radioConnectionSwitchRequestCorba,
				RadioConnectionSwitch& radioConnectionSwitch);

			static void convertControlConnectionStatusCorbaToNormal(const TA_Base_Bus::IRadioCorbaDef::EControlConnectionStatus& corbaObject, 
				EControlConnectionStatus& normalObject);

			static void convertRadioUpdateCallStackHistoryCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioUpdateCallStackHistory& messageCorba,
				RadioUpdateCallStackHistory& message);

			static void convertCallStackHistoryCorbaToNormal(const TA_Base_Bus::IRadioCorbaDef::CallStackHistory& callStackHistoryCorba,
				CallStackHistory& callStackHistory);

			static void convertRadioAddCallStackEntryCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioAddCallStackEntry& messageCorba,
				RadioAddCallStackEntry& message);

			static void convertRadioDeleteCallStackEntryCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioDeleteCallStackEntry& messageCorba,
				RadioDeleteCallStackEntry& message);

			static void convertRadioSubscriberUpdateCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::SubscriberStateUpdate& messageCorba,
				SubscriberStateUpdate& message);
            
            // **************************************************************
            // * Normal c-structure to Corba structure conversion functions *
            // **************************************************************

			static void convertAudioEntryNormalToCorba(const AudioEntryType& audioEntry,
                TA_Base_Bus::IRadioStateSynchronisationCorbaDef::AudioEntry& audioEntryCorba);
			
			static void convertAudioEntryMapTypeNormalToCorba(const AudioEntryMapType& audioEntryMap,
                TA_Base_Bus::IRadioStateSynchronisationCorbaDef::AudioEntryMapType& audioEntryMapCorba);

			static void convertSessionAudioMapTypeNormalToCorba(const  SessionAudioMapType& sessionAudioMap,
               TA_Base_Bus::IRadioStateSynchronisationCorbaDef::SessionAudioMapType& sessionAudioMapCorba);

			static void convertRadioResourceManagerNormalToCorba(const RadioResourceManagerType& radioResourceManager,
                TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioResourceManagerType& radioResourceManagerCorba);
			
		
		   	static void convertAuthorisationRequestTypeNormalToCorba(const AuthorisationRequestType& authorisationRequest,
                TA_Base_Bus::IRadioStateSynchronisationCorbaDef::AuthorisationRequestType& authorisationRequestCorba);
			
			static void convertAuthorisationManagerNormalToCorba(const RadioAuthorisationManagerType& radioAuthorisationManager,
                TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioAuthorisationManagerType& radioAuthorisationManagerCorba);

            static void convertEPTTStateNormalToCorba(const EPTTState& pttState,
                TA_Base_Bus::IRadioCorbaDef::EPTTState& pttStateCorba);
            
            static void convertCallParticipantNormalToCorba(const CallParticipant& callParticipant,
                TA_Base_Bus::IRadioCorbaDef::CallParticipant& callParticipantCorba);
            
            static void convertECallCategoryNormalToCorba(const ECallCategory& callCategory,
                TA_Base_Bus::IRadioCorbaDef::ECallCategory& callCategoryCorba);
            
            static void convertECallSourceNormalToCorba(const ECallSource &callSource,
                TA_Base_Bus::IRadioCorbaDef::ECallSource& callSourceCorba);
            
            static void convertECallTypeNormalToCorba(const ECallType& callType,
                TA_Base_Bus::IRadioCorbaDef::ECallType& callTypeCorba);
            
            static void convertCallDetailsTypeNormalToCorba(const CallDetailsType& callDetails,
                TA_Base_Bus::IRadioCorbaDef::CallDetailsType& callDetailsCorba);
            
            static void convertECallStatusTypeNormalToCorba(const ECallStatusType& callStatusType,
                TA_Base_Bus::IRadioCorbaDef::ECallStatusType& callStatusTypeCorba);
            
			static void convertRadioNormalToCorba(const RadioType& radio,
                TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioType& radioCorba);

			static void convertTcpServerNormalToCorba(const TcpServerType& radio,
                TA_Base_Bus::IRadioStateSynchronisationCorbaDef::TcpServerType& radioCorba);
            
            static void convertRadioServantNormalToCorba(const RadioServantType& radioServant,
                TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioServantType& radioServantCorba);
            
			static void convertRadioSessionServantNormalToCorba(const RadioSessionServantType& radioSessionServant,
				TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioSessionServantType& radioSessionServantCorba);

            static void convertRadioTetraAgentNormalToCorba(const RadioTetraAgentType& radioTetraAgent,
                TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioTetraAgentType& radioTetraAgentCorba);

            static void convertRadioUpdateCallInRadioCallStackNormalToCorba(const RadioUpdateCallInRadioCallStack& message,
                TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioUpdateCallInRadioCallStack& messageCorba);
            
            static void convertRadioRemoveCallFromRadioCallStackNormalToCorba(const RadioRemoveCallFromRadioCallStack& message,
                TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioRemoveCallFromRadioCallStack& messageCorba);
            
			static void convertRadioUpdateRadioSessionNormalToCorba(const RadioUpdateRadioSession& message,
				TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioUpdateRadioSession& messageCorba);
				
			static void convertRadioUpdateMonitoredCallProgressionNormalToCorba(const RadioUpdateMonitoredCallProgression& message,
				TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioUpdateMonitoredCallProgression& messageCorba);

			static void convertRadioRemoveMonitoredSubscriberDetailsNormalToCorba(const RadioRemoveMonitoredSubscriberDetails& message, 
				TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioRemoveMonitoredSubscriberDetails&  messageCorba);
			
			static void convertRadioRemoveSessionFromAudioMapNormalToCorba(const 	RadioRemoveSessionFromAudioMap& message,
				TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioRemoveSessionFromAudioMap& messageCorba);

			static void convertRadioUpdateAudioEntryInSessionAudioMapNormalToCorba(const RadioUpdateAudioEntryInSessionAudioMap& message,
				TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioUpdateAudioEntryInSessionAudioMap& messageCorba);
		
			static void convertRadioUpdateRadioResourceManagerVarsNormalToCorba(const RadioUpdateRadioResourceManagerVars& message,
				TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioUpdateRadioResourceManagerVars& messageCorba);
	
			static void convertRadioUpdateAuthorisationRequestNormalToCorba(const RadioUpdateAuthorisationRequest& message,
				TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioUpdateAuthorisationRequest& messageCorba);
	
			static void convertRadioRemoveAuthorisationRequestNormalToCorba(const RadioRemoveAuthorisationRequest& message,
				TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioRemoveAuthorisationRequest& messageCorba);
			static void convertRadioSessionNormalToCorba(const RadioSessionType& radioSession,
				TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioSessionType& radioSessionCorba);

			static void convertESessionStateNormalToCorba(const ESessionState& sessionState,
				TA_Base_Bus::IRadioStateSynchronisationCorbaDef::ESessionState& sessionStateCorba);

			static void convertRadioMonitorManagerTypeNormalToCorba(const RadioMonitorManagerType& radioMonitor ,
				TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioMonitorManagerType& radioMonitorCorba);

			static void convertMonitoredSubscriberDetailsTypeNormalToCorba(const MonitoredSubscriberDetailsType& radioMonitorSubscriber,
				TA_Base_Bus::IRadioStateSynchronisationCorbaDef::MonitoredSubscriberDetailsType& radioMonitorSubscriberCorba);

			static void convertRadioConnectionSwitchNormalToCorba(const RadioConnectionSwitch& message,
				TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioConnectionSwitchRequest& messageCorba);

			static void convertControlConnectionStatusNormalToCorba(const EControlConnectionStatus& normalObject,
				TA_Base_Bus::IRadioCorbaDef::EControlConnectionStatus& corbaObject);

			static void convertRadioUpdateCallStackHistoryNormalToCorba(const RadioUpdateCallStackHistory& message,
				TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioUpdateCallStackHistory& messageCorba);

			static void convertCallStackHistoryNormalToCorba(const CallStackHistory& callStackHistory,
				TA_Base_Bus::IRadioCorbaDef::CallStackHistory& callStackHistoryCorba);

			static void convertRadioAddCallStackEntryNormalToCorba(const RadioAddCallStackEntry& message,
				TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioAddCallStackEntry& messageCorba);

			static void convertRadioDeleteCallStackEntryNormalToCorba(const RadioDeleteCallStackEntry& message,
				TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioDeleteCallStackEntry& messageCorba);

			static void convertRadioSubscriberUpdateNormalToCorba(const SubscriberStateUpdate& message,
				TA_Base_Bus::IRadioStateSynchronisationCorbaDef::SubscriberStateUpdate& messageCorba);

    }; // class RadioStateMessageConverter
};  // namespace TA_IRS_App

#endif // #ifndef RadioStateMessageConverter_h
