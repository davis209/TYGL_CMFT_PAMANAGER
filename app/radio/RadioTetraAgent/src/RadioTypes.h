#ifndef RADIOTYPES_H
#define RADIOTYPES_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/RadioTypes.h $
  * @author  A. Parker
  * @version $Revision: #2 $
  * Last modification : $DateTime: 2009/05/18 10:25:59 $
  * Last modified by : $Author: builder $
  *
  *
  */

#include "core/types/src/ta_types.h"
#include <string>
#include <vector>
#include <map>
#include "app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioProtocol.h"

namespace TA_IRS_App
{
    // TODO: ta_int32 CallID DEPRECATED, replace with CallReference, ensure
    //       that no unsigned/signed incompatibilities will arise
    typedef CallReference        CallID;
    // DEPRECATED: use SessionReference from RadioProtocol.h
    typedef SessionReference SessionID;
    // DEPRECATED: use AudioReference from RadioProtocol.h
    // typedef ta_uint32         AudioReference;
    // DEPRECATED: use CallReference from RadioProtocol.h
    // typedef     ta_uint32         CallReference;
    // DEPRECATED: use CallReference from RadioProtocol.h
    // typedef ta_uint32         CADReference;

    typedef ta_uint32         ConsoleID;
    typedef ta_int32                 ServerIndex;
    typedef std::pair< SessionID, ServerIndex > SessionServerIndex;

    typedef std::map< SessionServerIndex, ta_uint32 > SessionServerToConsoleMap;
    typedef std::map< ta_uint32 , SessionServerIndex > ConsoleToSessionServerMap;

    // this had to go - there are too many variables and parameters as exactly
    // the same identifier to have a typedef. Previously it was not unseen to
    // have an argument like function(TSI TSI);
    //
    // Also, this typedef was being used in *very* few places, too few to justify
    // keeping.
    // typedef std::string TSI;


	enum ECallStatusType
	{
		Ringing,
		Connected,
		ConnectedGroup,   // Group details now available.
		Disconnected,
		OnHold,
		Deleted,          // Only sent by status updates
        SessionUpdate    //    "   "   "   "   "   "

	};

	enum ECallCategory
	{
		HalfDuplex,
		FullDuplex,
		TextMessage,
		CallDriver
	};

    enum EMonitorStatusType
    {
        Monitoring      = 1,
        Listening       = 2,
        PendingInclude  = 3, // not used to send a notification
        Included        = 4,
        Joined          = 5,
        Stopped         = 6,
        NotMonitored    = 0 // used as error condition : not for notification use
    };

	enum ECallSource
	{
		IncomingCall,
		OutgoingCall
	};

	enum ECallType
	{
		SingleCall,
		GroupUnacknowledgeCall,
		GroupAcknowledgeCall,
		GroupBroadcastCall,
		CircuitModeDataCall

	};

    enum EPTTState
    {
        PTT_GRANTED,
        PTT_QUEUE,
        PTT_CEASE,
        PTT_TIMEOUT
    };

    struct CallParticipant
    {
        std::string name;
        std::string TSI;
        std::string location;

        bool   		isAcknowledged;	// Defines 'Not Connected' state, if false
        EPTTState	pttState;		// Current ptt state, if available (only required if isAcknowledged is true)
    };

    typedef std::vector<CallParticipant> CallParticipantList;

	struct CallDetailsType
	{
		CallID			callID;
		SessionReference sessionRef;
        ta_uint32    consoleID;
		time_t			time;
		std::string		callTime;
		std::string		callerName;
		std::string		callerTSI;
		std::string		callerLocation;
		std::string		calledName;
		std::string		calledTSI;
		std::string		calledLocation;
		ECallStatusType	callStatus;
		ECallCategory	callCategory;
		ECallType		callType;
		ECallSource		callSource;
		std::string		callText;
        bool            isEmergency;
		//TD 10234
        bool            isInvisibleToGUIApps;
		bool			isIncludeCall;
        CallParticipantList participants;
	};

	enum ERadioUserType {PORTABLE_RADIO,OPERATOR_RADIO,STATION_RADIO,DLTS_TELEPHONY,EPAX_TELEPHONY};

    struct MonitoredCallDetailsType
        : public CallDetailsType
    {
        MonitorReference   monitorID;
        EMonitorStatusType monitorStatus;
    };

	typedef std::vector<MonitoredCallDetailsType> MonitoredCallDetailsList;

	struct AuthorisationRequestType
	{
		CADReference		callID;
		SessionReference	sessionRef;
		ERadioUserType		sourceType;
		ERadioUserType		destinationType;
		std::string			callerName;     //sourceUserName;
		std::string         calledName;     //destinationUserName;
		std::string			callerTSI;     //sourceUser;
		std::string			calledTSI;     //DestinationUser;
		std::string			callerLocation;	 //for showing in the callbanner
		std::string			calledLocation;
		time_t				time;		   // time
	};

	enum EAction {NEW_AUTHORISATION,ACCEPTED_AUTHORISATION,REJECTED_AUTHORISATION,CANCELLED_AUTHORISATION};

    enum EFallbackReason
    {
        fbSessionLogin,
        fbUnsolicited
    };

    enum ECallPriority
    {
        Priority0 = 0,
        Priority1 = 1,
        Priority2 = 2,
        Priority3 = 3,
        Priority4 = 4,
        Priority5 = 5,
        Priority6 = 6,
        Priority7 = 7,
        Priority8 = 8,
        Priority9 = 9,
        Priority10 = 10,
        Priority11 = 11,
        Priority12 = 12,
        Priority13 = 13,
        Priority14 = 14,
        Priority15 = 15,
        DefaultNormalPriority = Priority1,   // Indicates the default normal call priority
        DefaultEmergencyPriority = Priority15 // Indicates the default emergency call priority
    };

	
	enum GxUpdateType
	{
		SessionStatusOK,
		SessionNotLogin,
		SessionBadUser,
		LinkBroken,
		LinkResume
    };

	typedef std::vector<CallID>						CallList; 
	typedef std::vector<CallDetailsType>			CallStack; 
	typedef std::vector<AuthorisationRequestType>	AuthorisationStack; 

	// TD17183 Group SDS Support
	enum ESdsServiceSelection
	{
		Individual,   // Service selection is SDSTL_INDIVIDUAL
		Group,        // Service selection is SDSTL_GROUP
		Unknown       // Service selection is determined by querying radio system
		// for subscriber type
	};

	enum EOriginType
	{
		ORIGIN_INVALID,
		INBOUND,
		OUTBOUND
	};

	enum EStackCallType
	{
		CALL_INVALID,
		VOICE,
		CONFERENCE,
		TEXT,
		GROUP_ACK,
		GROUP_UNACK,
		GROUP_BROADCAST,
		MISSED,
		FAILED,
		ENDED
	};

	struct CallStackHistory
	{
		std::string	      profile;
		ta_uint32		  locationKey;
		time_t			  timestamp;
		EOriginType		  originType;
		EStackCallType	  callType;
		std::string	      identifier;
		std::string	      location;
		bool	          isEmergency;
		std::string		  tsi;
	};

	typedef std::vector<CallStackHistory> CallStackHistoryList;

	struct GroupDetails
	{
		std::string       groupTSI;
		std::string       memberTSI;
	};
	typedef std::vector<GroupDetails> GroupList;

	struct PatchDetails
	{
		std::string       patchTSI;
		std::string       groupTSI;
	};
	typedef std::vector<PatchDetails> PatchList;

	typedef std::vector<unsigned char> MessageTextType;
	typedef std::vector<MessageTextType> MessageListType;

	enum SubscriberNotificationType
	{
		NEW_SUBSCRIBER,
		UPDATE_SUBSCRIBER,
		DELETE_SUBSCRIBER,
		SCAN_SUBSCRIBERS       // Sent out to notify clients that a synchronisation scan has completed.
	};

	enum SubscriberType
	{
		INDIVIDUAL_SUBSCRIBER,
		GROUP_SUBSCRIBER,
		PATCH_SUBSCRIBER,
		DISPATCHER_SUBSCRIBER,
		MOBILE_SUBSCRIBER
	};

	struct SubscriberDetails
	{
		std::string      TSI;
		std::string      subscriberName;
		std::string	     locationName;
		ta_uint32	     locationKey;
		std::string      ownerTSI;        // Only populated for patch subscribers.
		SubscriberType   type;
	};

	struct MonitoredCallInfo
	{
		EMonitorStatusType	monitorStatus;
		CallID				monitorCall;
		CallID				listenCall;
		CallID				joinedCall;
	};

	typedef std::vector<MonitoredCallInfo> MonitoredCallInfoList;

	struct MonitoredSubscriber
	{
		MonitorReference	monitorRef;
		SessionReference	sessionRef;
		std::string         subscriberTsi;

		MonitoredCallInfoList monCallInfos;
	};


	typedef std::vector<MonitoredSubscriber> MonitoredSubscriberList;

	struct MonitoredGroup
	{
		MonitorReference	monitorRef;
		std::string groupTsi;
		CallID callID;
	};

	typedef std::vector<MonitoredGroup> MonitoredGroupList;

};  // Namespace TA_IRS_App

#endif
