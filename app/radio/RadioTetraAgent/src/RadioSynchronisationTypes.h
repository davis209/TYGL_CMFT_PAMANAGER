#ifndef RadioSynchronisationTypes_h
#define RadioSynchronisationTypes_h

/**
  * The source code in this file is the property of
  * Ripples Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/RadioSynchronisationTypes.h $
  * @author  John Dalin
  * @version $Revision: #2 $
  * Last modification : $DateTime: 2009/03/19 17:00:25 $
  * Last modified by : $Author: builder $
  *
  *
  */

#include "core/types/src/ta_types.h"
#include "app/radio/RadioTetraAgent/src/RadioTypes.h"

#include <vector>
#include <string>
#include <map>
namespace TA_IRS_App
{
	
	enum ESessionState
	{
		SessionUnassigned,
		SessionAssigned,
		SessionInitialised,
		SessionLoggedIn
	};

	enum EControlConnectionStatus
	{
		NoAppliable,
		ConnectToPrimary,
		ConnectToSecondary			
	};
	
	struct RadioSessionType
	{
		ta_uint32 audioLink;
		SessionReference sessionRef;
		std::string sessionEntityName;
		std::string loginName;
        std::string loginPassword;
		std::string subscriberTextRef;
		std::string sessionDGNAITSI;
		bool sessionDGNAInUse;
		ESessionState sessionStatus;
		std::string sessionITSI;
	};


	struct RadioSessionServantType
	{
		ta_uint32 entityKey;
		RadioSessionType radioSession;
	};


	typedef std::vector<RadioSessionServantType> RadioSessionServantTypeList;


    /***********************************
     * Composite Radio State Hierarchy *
     ***********************************/

	struct MonitoredSubscriberDetailsType
	{	   	
		ta_uint32 monitorReference;
		//Future other variables to be added in this structure.
	};

	typedef std::map< ta_int32 , MonitoredSubscriberDetailsType> MonitoredSubscriberDetailsTypeMap;

	struct RadioMonitorManagerType
	{
		MonitoredSubscriberDetailsTypeMap monitoredSubscriberDetailsMap;
	};
	
	struct AudioEntryType
	{
		CallReference callRef;
		bool inUse;
		ta_int32 volume;
	};
	
	typedef std::map<AudioReference, AudioEntryType> AudioEntryMapType;

    struct ConsoleSessionRef

    {

        SessionReference sessionRef;

        ConsoleID consoleKey;


        bool operator<(const ConsoleSessionRef& csRef) const

        {

            if(consoleKey != csRef.consoleKey)

            {

                return (consoleKey < csRef.consoleKey);

            }

            else

            {

                return (sessionRef < csRef.sessionRef);

            }

        };


        bool operator==(const ConsoleSessionRef& csRef) const

        {

            return (consoleKey == csRef.consoleKey &&

                        sessionRef==csRef.sessionRef);

        };


    };


    typedef std::map<ConsoleSessionRef, AudioEntryMapType> SessionAudioMapType;


				
	struct RadioResourceManagerType
	{
		ta_int32 currentAudioVolume;
		SessionAudioMapType sessionAudioMap;
	};

	typedef std::vector<AuthorisationRequestType> AuthorisationRequestTypeList;
	
	struct RadioAuthorisationManagerType
	{
		AuthorisationRequestTypeList radioAuthorisationStack;
	};

    typedef std::vector<CallDetailsType> CallDetailsTypeList;
    
    struct TcpServerType
    {
        CallDetailsTypeList radioCallStack;
		RadioAuthorisationManagerType radioAuthorisationManager;
		RadioResourceManagerType radioResourceManager;
		RadioMonitorManagerType radioMonitorManager;
		EControlConnectionStatus controlConnectionStatus;
    };

	typedef std::map< ServerIndex, TcpServerType> RadioType;
    
    struct RadioServantType
    {
        ta_uint32 entityKey;
        RadioType radio;
    };
    
    struct RadioTetraAgentType
    {
        RadioServantType radioServant;
		RadioSessionServantTypeList radioSessionServants;
  };

	struct RadioCallStackHistory
	{
		std::string		  profile;
		ta_int32		  locationKey;
		time_t			  timestamp;
		EOriginType		  originType;
		EStackCallType	  callType;
		std::string		  identifier;
		std::string		  location;
		bool              isEmergency;
	};
	typedef std::vector<RadioCallStackHistory> RadioCallStackHistoryList;

    /*******************************
     * Radio State Update Messages *
     *******************************/
    struct RadioStateUpdateMessage
    {
        enum Type
        {
            UPDATE_CALL_IN_RADIO_CALL_STACK,
            REMOVE_CALL_FROM_RADIO_CALL_STACK,
			UPDATE_RADIO_SESSION,
			UPDATE_MONITORED_CALL_PROGRESSION,
			REMOVE_MONITORED_SUBSCRIBER_DETAILS,
			REMOVE_SESSION_FROM_AUDIO_MAP,
			UPDATE_AUDIO_ENTRY_IN_SESSION_AUDIO_MAP,
			UPDATE_RADIO_RESOURCE_MANAGER_VARS,
			UPDATE_AUTHORISATION_REQUEST,
			REMOVE_AUTHORISATION_REQUEST,
			CONNECTION_SWITCH,
			UPDATE_CALL_STACK_HISTORY,
			ADD_CALL_STACK_HISTORY,
			RETRIEVE_CALL_STACK_HISTORY,
			DELETE_CALL_STACK_HISTORY,
			SUBSCRIBER_UPDATE
        };

        virtual Type getType() const = 0;
        virtual ~RadioStateUpdateMessage() {}
    };

	struct RadioConnectionSwitch : virtual public RadioStateUpdateMessage
    {
		ta_uint32 TCPServerIndex;
        EControlConnectionStatus controlConnectionStatus;
		
        virtual Type getType() const { return CONNECTION_SWITCH; }
        virtual ~RadioConnectionSwitch() {}
    };

    struct RadioUpdateCallInRadioCallStack : virtual public RadioStateUpdateMessage
    {
        CallDetailsType callDetails;

        virtual Type getType() const { return UPDATE_CALL_IN_RADIO_CALL_STACK; }
        virtual ~RadioUpdateCallInRadioCallStack() {}
    };

    struct RadioRemoveCallFromRadioCallStack : virtual public RadioStateUpdateMessage
    {
        CallID callID;

        ta_uint32 consoleID;
        virtual Type getType() const { return REMOVE_CALL_FROM_RADIO_CALL_STACK; }
        virtual ~RadioRemoveCallFromRadioCallStack() {}
    };

	struct RadioUpdateRadioSession : virtual public RadioStateUpdateMessage
	{
		ta_uint32 servantEntityKey;
		RadioSessionType radioSession;

		virtual Type getType() const { return UPDATE_RADIO_SESSION; }
		virtual ~RadioUpdateRadioSession() {}
	};

	struct RadioUpdateMonitoredCallProgression : virtual public RadioStateUpdateMessage
	{
		ta_uint32   monitorReference;

		virtual Type getType() const { return UPDATE_MONITORED_CALL_PROGRESSION; }
		virtual ~RadioUpdateMonitoredCallProgression() {}
	};
	
	struct RadioRemoveMonitoredSubscriberDetails : virtual public RadioStateUpdateMessage
	{
		ta_uint32   monitorReference;
		ta_int32 	   serverIndex;


		virtual Type getType() const { return REMOVE_MONITORED_SUBSCRIBER_DETAILS; }
		virtual ~RadioRemoveMonitoredSubscriberDetails() {}
	};

	struct RadioRemoveSessionFromAudioMap : virtual public RadioStateUpdateMessage
	{
		SessionID sessionReference;
		ta_int32 serverIndex;

		virtual Type getType() const { return REMOVE_SESSION_FROM_AUDIO_MAP; }
		virtual ~RadioRemoveSessionFromAudioMap() {}
	};
	
	struct RadioUpdateAudioEntryInSessionAudioMap : virtual public RadioStateUpdateMessage
	{
		SessionID sessionReference;
		AudioReference audioReference;
		AudioEntryType audio;

		virtual Type getType() const { return UPDATE_AUDIO_ENTRY_IN_SESSION_AUDIO_MAP; }
		virtual ~RadioUpdateAudioEntryInSessionAudioMap() {}
	};
	
	struct RadioUpdateRadioResourceManagerVars : virtual public RadioStateUpdateMessage
	{
		SessionID sessionReference;
		CallReference callReference;
		ta_int32 currrentVolume;
		ta_int32 serverIndex;

		virtual Type getType() const { return UPDATE_RADIO_RESOURCE_MANAGER_VARS; }
		virtual ~RadioUpdateRadioResourceManagerVars() {}
	};
	
	struct RadioUpdateAuthorisationRequest : virtual public RadioStateUpdateMessage
	{
		AuthorisationRequestType autorisationRequest;
		ta_int32 serverIndex;

		virtual Type getType() const { return UPDATE_AUTHORISATION_REQUEST; }
		virtual ~RadioUpdateAuthorisationRequest() {}
	};
	
	struct RadioRemoveAuthorisationRequest : virtual public RadioStateUpdateMessage
	{
		CallID callID;
		ta_int32 serverIndex;

		virtual Type getType() const { return REMOVE_AUTHORISATION_REQUEST; }
		virtual ~RadioRemoveAuthorisationRequest() {}
	};

	struct RadioUpdateCallStackHistory : virtual public RadioStateUpdateMessage
	{
		CallStackHistoryList callStackHistoryList;
		bool				 isRetrieved;

		virtual Type getType() const { return UPDATE_CALL_STACK_HISTORY; }
		virtual ~RadioUpdateCallStackHistory() {}
	};

	struct RadioAddCallStackEntry : virtual public RadioStateUpdateMessage
	{
		std::string		  profile;
		ta_int32		  locationKey;
		time_t			  timestamp;
		EOriginType		  originType;
		EStackCallType	  callType;
		std::string		  identifier;
		std::string		  location;
		bool              isEmergency;
		std::string		  tsi;

		virtual Type getType() const { return ADD_CALL_STACK_HISTORY; }
		virtual ~RadioAddCallStackEntry() {}
	};

	struct RadioRetrieveCallStackHistory : virtual public RadioStateUpdateMessage
	{
		bool              isRetrieved;

		virtual Type getType() const { return RETRIEVE_CALL_STACK_HISTORY; }
		virtual ~RadioRetrieveCallStackHistory() {}
	};

	struct  RadioDeleteCallStackEntry : virtual public RadioStateUpdateMessage
	{
		time_t              timestamp;
		std::string			tsi;

		virtual Type getType() const { return DELETE_CALL_STACK_HISTORY; }
		virtual ~RadioDeleteCallStackEntry() {}
	};

	struct SubscriberStateUpdate : virtual public RadioStateUpdateMessage
	{
		SubscriberNotificationType    notifyType;
		SubscriberDetails             details;    // Not populated for SCAN_SUBSCRIBERS
		GroupList					  groupList;  // Individual subscribers

		virtual Type getType() const { return SUBSCRIBER_UPDATE; }
		virtual ~SubscriberStateUpdate() {}
	};

    /** requestFullState
      *
      * This method will is used to request the Radio Tetra Agent's full state
      */
    RadioTetraAgentType requestFullState();

	RadioCallStackHistoryList requestCallStackHistory();
    
};  // Namespace TA_IRS_App

#endif
