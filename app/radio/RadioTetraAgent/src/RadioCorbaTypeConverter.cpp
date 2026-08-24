
#include "RadioCorbaTypeConverter.h"
#include "app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioProtocolLibrary.h"
#include "core/utilities/src/DebugUtil.h" 
#include "core/utilities/src/TAAssert.h"

// TD14696
namespace TA_IRS_App
{


    void RadioCorbaTypeConverter::convertERadioUserTypeCorbaToNormal(const TA_Base_Bus::IRadioSessionCorbaDef::ERadioUserType& radioUserTypeCorba,
                                                                     ERadioUserType& radioUserType)
    {
        switch (radioUserTypeCorba)
        {
        case TA_Base_Bus::IRadioSessionCorbaDef::PORTABLE:
            radioUserType = PORTABLE_RADIO;
            break;
        
        case TA_Base_Bus::IRadioSessionCorbaDef::OPERATOR:
            radioUserType = OPERATOR_RADIO;
            break;
        
        case TA_Base_Bus::IRadioSessionCorbaDef::STATION:
            radioUserType = STATION_RADIO;
            break;
       
        
        default:
            ;
        }
    }
    void RadioCorbaTypeConverter::convertEPTTStateCorbaToNormal(const TA_Base_Bus::IRadioCorbaDef::EPTTState& pttStateCorba,
                                                                   EPTTState& pttState)
    {
        switch (pttStateCorba)
        {
        case TA_Base_Bus::IRadioCorbaDef::PTT_GRANTED:
            pttState = PTT_GRANTED;
            break;
        
        case TA_Base_Bus::IRadioCorbaDef::PTT_QUEUE:
            pttState = PTT_QUEUE;
            break;
        
        case TA_Base_Bus::IRadioCorbaDef::PTT_CEASE:
            pttState = PTT_CEASE;
            break;
        
        case TA_Base_Bus::IRadioCorbaDef::PTT_TIMEOUT:
            pttState = PTT_TIMEOUT;
            break;
        
        default:
            ;
        }
    }

    void RadioCorbaTypeConverter::convertCallParticipantCorbaToNormal(const TA_Base_Bus::IRadioCorbaDef::CallParticipant& callParticipantCorba,
                                                                         CallParticipant& callParticipant)
    {
        callParticipant.isAcknowledged = callParticipantCorba.isAcknowledged;
        callParticipant.location = callParticipantCorba.location;
        callParticipant.name = callParticipantCorba.name;
        convertEPTTStateCorbaToNormal(callParticipantCorba.pttState, callParticipant.pttState);
        callParticipant.TSI = callParticipantCorba.TSI;
    }

    void RadioCorbaTypeConverter::convertECallCategoryCorbaToNormal(const TA_Base_Bus::IRadioCorbaDef::ECallCategory& callCategoryCorba,
                                                                       ECallCategory& callCategory)
    {
        switch (callCategoryCorba)
        {
        case TA_Base_Bus::IRadioCorbaDef::HalfDuplex:
            callCategory = HalfDuplex;
            break;
        
        case TA_Base_Bus::IRadioCorbaDef::FullDuplex:
            callCategory = FullDuplex;
            break;
        
        case TA_Base_Bus::IRadioCorbaDef::TextMessage:
            callCategory = TextMessage;
            break;
        
        case TA_Base_Bus::IRadioCorbaDef::CallDriver:
            callCategory = CallDriver;
            break;
        
        default:
            ;
        }
    }


    void RadioCorbaTypeConverter::convertECallSourceCorbaToNormal(const TA_Base_Bus::IRadioCorbaDef::ECallSource& callSourceCorba,
                                                                     ECallSource &callSource)
    {
        switch (callSourceCorba)
        {
        case TA_Base_Bus::IRadioCorbaDef::IncomingCall:
            callSource = IncomingCall;
            break;
        
        case TA_Base_Bus::IRadioCorbaDef::OutgoingCall:
            callSource = OutgoingCall;
            break;
        
        default:
            ;
        }
    }


    void RadioCorbaTypeConverter::convertECallTypeCorbaToNormal(const TA_Base_Bus::IRadioCorbaDef::ECallType& callTypeCorba,
                                                                   ECallType& callType)
    {
        switch (callTypeCorba)
        {
        case TA_Base_Bus::IRadioCorbaDef::SingleCall:
            callType = SingleCall;
            break;
        
        case TA_Base_Bus::IRadioCorbaDef::GroupUnacknowledgeCall:
            callType = GroupUnacknowledgeCall;
            break;
        
        case TA_Base_Bus::IRadioCorbaDef::GroupAcknowledgeCall:
            callType = GroupAcknowledgeCall;
            break;
        
        case TA_Base_Bus::IRadioCorbaDef::GroupBroadcastCall:
            callType = GroupBroadcastCall;
            break;
        
        case TA_Base_Bus::IRadioCorbaDef::CircuitModeDataCall:
            callType = CircuitModeDataCall;
            break;
        
        default:
            ;
        }
    }


    void RadioCorbaTypeConverter::convertCallDetailsTypeCorbaToNormal(const TA_Base_Bus::IRadioCorbaDef::CallDetailsType& callDetailsCorba,
                                                                         CallDetailsType& callDetails)
    {
        convertECallCategoryCorbaToNormal(callDetailsCorba.callCategory, callDetails.callCategory);
        callDetails.calledLocation = callDetailsCorba.calledLocation;
        callDetails.calledName = callDetailsCorba.calledName;
        callDetails.calledTSI = callDetailsCorba.calledTSI;
        callDetails.callerLocation = callDetailsCorba.callerLocation;
        callDetails.callerName = callDetailsCorba.callerName;
        callDetails.callerTSI = callDetailsCorba.callerTSI;
        callDetails.callID = callDetailsCorba.callID;
        convertECallSourceCorbaToNormal(callDetailsCorba.callSource, callDetails.callSource);
        convertECallStatusTypeCorbaToNormal(callDetailsCorba.callStatus, callDetails.callStatus);
        callDetails.callText = callDetailsCorba.callText;
        callDetails.callTime = callDetailsCorba.callTime;
        convertECallTypeCorbaToNormal(callDetailsCorba.callType, callDetails.callType);
        callDetails.isEmergency = callDetailsCorba.isEmergency;
        //TD 10234
        callDetails.isInvisibleToGUIApps = callDetailsCorba.isInvisibleToGUIApps;
		callDetails.isIncludeCall = callDetailsCorba.isIncludeCall;
    
        // convert all participants in the call
        callDetails.participants.clear();
    
        for (unsigned int i = 0; i < callDetailsCorba.participants.length(); ++i)
        {
            CallParticipant callParticipant;
        
            convertCallParticipantCorbaToNormal(callDetailsCorba.participants[i], callParticipant);
            callDetails.participants.push_back(callParticipant);
        }
    
        callDetails.sessionRef = callDetailsCorba.sessionRef;
        callDetails.time = callDetailsCorba.time;
		callDetails.consoleID = callDetailsCorba.consoleKey;
    }

    void RadioCorbaTypeConverter::convertECallStatusTypeCorbaToNormal(const TA_Base_Bus::IRadioCorbaDef::ECallStatusType& callStatusTypeCorba,
                                                                         ECallStatusType& callStatusType)
    {
        switch (callStatusTypeCorba)
        {
        case TA_Base_Bus::IRadioCorbaDef::Ringing:
            callStatusType = Ringing;
            break;
        
        case TA_Base_Bus::IRadioCorbaDef::Connected:
            callStatusType = Connected;
            break;
        
        case TA_Base_Bus::IRadioCorbaDef::ConnectedGroup:
            callStatusType = ConnectedGroup;
            break;
        
        case TA_Base_Bus::IRadioCorbaDef::Disconnected:
            callStatusType = Disconnected;
            break;
        
        case TA_Base_Bus::IRadioCorbaDef::OnHold:
            callStatusType = OnHold;
            break;
        
        case TA_Base_Bus::IRadioCorbaDef::Deleted:
            callStatusType = Deleted;
            break;
		}
	}

	void RadioCorbaTypeConverter::convertEOriginTypeCorbaToNormal(const TA_Base_Bus::IRadioCorbaDef::EOriginType& corbaObject,
		EOriginType& normalObject)
	{
		switch (corbaObject)
		{
		case TA_Base_Bus::IRadioCorbaDef::ORIGIN_INVALID:
			normalObject = ORIGIN_INVALID;
			break;

		case TA_Base_Bus::IRadioCorbaDef::INBOUND:
			normalObject = INBOUND;
			break;

		case TA_Base_Bus::IRadioCorbaDef::OUTBOUND:
			normalObject = OUTBOUND;
			break;

		default:
			normalObject = ORIGIN_INVALID;
		}
	}

	void RadioCorbaTypeConverter::convertEStackCallTypeCorbaToNormal(const TA_Base_Bus::IRadioCorbaDef::EStackCallType& corbaObject,
		EStackCallType& normalObject)
	{
		switch (corbaObject)
		{
		case TA_Base_Bus::IRadioCorbaDef::CALL_INVALID:
			normalObject = CALL_INVALID;
			break;

		case TA_Base_Bus::IRadioCorbaDef::VOICE:
			normalObject = VOICE;
			break;

		case TA_Base_Bus::IRadioCorbaDef::CONFERENCE:
			normalObject = CONFERENCE;
			break;

		case TA_Base_Bus::IRadioCorbaDef::TEXT:
			normalObject = TEXT;
			break;

		case TA_Base_Bus::IRadioCorbaDef::GROUP_ACK:
			normalObject = GROUP_ACK;
			break;

		case TA_Base_Bus::IRadioCorbaDef::GROUP_UNACK:
			normalObject = GROUP_UNACK;
			break;

		case TA_Base_Bus::IRadioCorbaDef::GROUP_BROADCAST:
			normalObject = GROUP_BROADCAST;
			break;

		default:
			normalObject = CALL_INVALID;
		}
	}

	void RadioCorbaTypeConverter::convertSubscriberNotificationTypeCorbaToNormal(const TA_Base_Bus::IRadioCorbaDef::SubscriberNotificationType& notifyTypeCorba,
		SubscriberNotificationType& notifyType)
	{
		switch (notifyTypeCorba)
		{
		case TA_Base_Bus::IRadioCorbaDef::NEW_SUBSCRIBER:
			notifyType = NEW_SUBSCRIBER;
			break;
		case TA_Base_Bus::IRadioCorbaDef::UPDATE_SUBSCRIBER:
			notifyType = UPDATE_SUBSCRIBER;
			break;
		case TA_Base_Bus::IRadioCorbaDef::DELETE_SUBSCRIBER:
			notifyType = DELETE_SUBSCRIBER;
			break;
		case TA_Base_Bus::IRadioCorbaDef::SCAN_SUBSCRIBERS:
			notifyType = SCAN_SUBSCRIBERS;
			break;
		}
	}

	void RadioCorbaTypeConverter::convertSubscriberTypeCorbaToNormal(const TA_Base_Bus::IRadioCorbaDef::SubscriberType& corbaType,
		SubscriberType& subType)
	{
		switch (corbaType)
		{
		case TA_Base_Bus::IRadioCorbaDef::INDIVIDUAL_SUBSCRIBER:
			subType = INDIVIDUAL_SUBSCRIBER;
			break;
		case TA_Base_Bus::IRadioCorbaDef::GROUP_SUBSCRIBER:
			subType = GROUP_SUBSCRIBER;
			break;
		case TA_Base_Bus::IRadioCorbaDef::PATCH_SUBSCRIBER:
			subType = PATCH_SUBSCRIBER;
			break;
		case TA_Base_Bus::IRadioCorbaDef::DISPATCHER_SUBSCRIBER:
			subType = DISPATCHER_SUBSCRIBER;
			break;
		case TA_Base_Bus::IRadioCorbaDef::MOBILE_SUBSCRIBER:
			subType = MOBILE_SUBSCRIBER;
			break;
		}
	}

    void RadioCorbaTypeConverter::convertERadioUserTypeNormalToCorba(const ERadioUserType& radioUserType,
                                                                        TA_Base_Bus::IRadioSessionCorbaDef::ERadioUserType& radioUserTypeCorba )
    {
        switch (radioUserType)
        {
        case PORTABLE_RADIO:
            radioUserTypeCorba = TA_Base_Bus::IRadioSessionCorbaDef::PORTABLE;
            break;
        
        case OPERATOR_RADIO:
            radioUserTypeCorba = TA_Base_Bus::IRadioSessionCorbaDef::OPERATOR;
            break;
        
        case STATION_RADIO:
            radioUserTypeCorba = TA_Base_Bus::IRadioSessionCorbaDef::STATION;
            break;
       
        
        default:
            ;
        }
    }


    void RadioCorbaTypeConverter::convertEPTTStateNormalToCorba(const EPTTState& pttState,
                                                                   TA_Base_Bus::IRadioCorbaDef::EPTTState& pttStateCorba)
    {
        switch (pttState)
        {
        case PTT_GRANTED:
            pttStateCorba = TA_Base_Bus::IRadioCorbaDef::PTT_GRANTED;
            break;
        
        case PTT_QUEUE:
            pttStateCorba = TA_Base_Bus::IRadioCorbaDef::PTT_QUEUE;
            break;
        
        case PTT_CEASE:
            pttStateCorba = TA_Base_Bus::IRadioCorbaDef::PTT_CEASE;
            break;
        
        case PTT_TIMEOUT:
            pttStateCorba = TA_Base_Bus::IRadioCorbaDef::PTT_TIMEOUT;
            break;
        
        default:
            ;
        }
    }

    void RadioCorbaTypeConverter::convertCallParticipantNormalToCorba(const CallParticipant& callParticipant,
                                                                         TA_Base_Bus::IRadioCorbaDef::CallParticipant& callParticipantCorba)
    {
        callParticipantCorba.isAcknowledged = callParticipant.isAcknowledged;
        callParticipantCorba.location = callParticipant.location.c_str();
        callParticipantCorba.name = callParticipant.name.c_str();
        convertEPTTStateNormalToCorba(callParticipant.pttState, callParticipantCorba.pttState);
        callParticipantCorba.TSI = callParticipant.TSI.c_str();
    }

    void RadioCorbaTypeConverter::convertECallCategoryNormalToCorba(const ECallCategory& callCategory,
                                                                       TA_Base_Bus::IRadioCorbaDef::ECallCategory& callCategoryCorba)
    {
        switch (callCategory)
        {
        case HalfDuplex:
            callCategoryCorba = TA_Base_Bus::IRadioCorbaDef::HalfDuplex;
            break;
        
        case FullDuplex:
            callCategoryCorba = TA_Base_Bus::IRadioCorbaDef::FullDuplex;
            break;
        
        case TextMessage:
            callCategoryCorba = TA_Base_Bus::IRadioCorbaDef::TextMessage;
            break;
        
        case CallDriver:
            callCategoryCorba = TA_Base_Bus::IRadioCorbaDef::CallDriver;
            break;
        
        default:
            ;
        }
    }


    void RadioCorbaTypeConverter::convertECallSourceNormalToCorba(const ECallSource &callSource,
                                                                     TA_Base_Bus::IRadioCorbaDef::ECallSource& callSourceCorba)
    {
        switch (callSource)
        {
        case IncomingCall:
            callSourceCorba = TA_Base_Bus::IRadioCorbaDef::IncomingCall;
            break;
        
        case OutgoingCall:
            callSourceCorba = TA_Base_Bus::IRadioCorbaDef::OutgoingCall;
            break;
        
        default:
            ;
        }
    }


    void RadioCorbaTypeConverter::convertECallTypeNormalToCorba(const ECallType& callType,
                                                                   TA_Base_Bus::IRadioCorbaDef::ECallType& callTypeCorba)
    {
        switch (callType)
        {
        case SingleCall:
            callTypeCorba = TA_Base_Bus::IRadioCorbaDef::SingleCall;
            break;
        
        case GroupUnacknowledgeCall:
            callTypeCorba = TA_Base_Bus::IRadioCorbaDef::GroupUnacknowledgeCall;
            break;
        
        case GroupAcknowledgeCall:
            callTypeCorba = TA_Base_Bus::IRadioCorbaDef::GroupAcknowledgeCall;
            break;
        
        case GroupBroadcastCall:
            callTypeCorba = TA_Base_Bus::IRadioCorbaDef::GroupBroadcastCall;
            break;
        
        case CircuitModeDataCall:
            callTypeCorba = TA_Base_Bus::IRadioCorbaDef::CircuitModeDataCall;
            break;
        
        default:
            ;
        }
    }


    void RadioCorbaTypeConverter::convertCallDetailsTypeNormalToCorba(const CallDetailsType& callDetails,
                                                                         TA_Base_Bus::IRadioCorbaDef::CallDetailsType& callDetailsCorba)
    {
        convertECallCategoryNormalToCorba(callDetails.callCategory, callDetailsCorba.callCategory);
        callDetailsCorba.calledLocation = callDetails.calledLocation.c_str();
        callDetailsCorba.calledName = callDetails.calledName.c_str();
        callDetailsCorba.calledTSI = callDetails.calledTSI.c_str();
        callDetailsCorba.callerLocation = callDetails.callerLocation.c_str();
        callDetailsCorba.callerName = callDetails.callerName.c_str();
        callDetailsCorba.callerTSI = callDetails.callerTSI.c_str();
        callDetailsCorba.callID = callDetails.callID;
        convertECallSourceNormalToCorba(callDetails.callSource, callDetailsCorba.callSource);
        convertECallStatusTypeNormalToCorba(callDetails.callStatus, callDetailsCorba.callStatus);
        callDetailsCorba.callText = callDetails.callText.c_str();
        callDetailsCorba.callTime = callDetails.callTime.c_str();
        convertECallTypeNormalToCorba(callDetails.callType, callDetailsCorba.callType);
        callDetailsCorba.isEmergency = callDetails.isEmergency;
        //TD 10234
        callDetailsCorba.isInvisibleToGUIApps = callDetails.isInvisibleToGUIApps;
		callDetailsCorba.isIncludeCall = callDetails.isIncludeCall;
    
        // convert all participants in the call
        unsigned int i = 0;
    
        callDetailsCorba.participants.length(callDetails.participants.size());
    
        for (CallParticipantList::const_iterator it = callDetails.participants.begin(); it != callDetails.participants.end(); it++)
        {
            convertCallParticipantNormalToCorba(*it, callDetailsCorba.participants[i]);
        }
    
        callDetailsCorba.sessionRef = callDetails.sessionRef;
        callDetailsCorba.time = callDetails.time;
    }

    void RadioCorbaTypeConverter::convertECallStatusTypeNormalToCorba(const ECallStatusType& callStatusType,
                                                                         TA_Base_Bus::IRadioCorbaDef::ECallStatusType& callStatusTypeCorba)
    {
        switch (callStatusType)
        {
        case Ringing:
            callStatusTypeCorba = TA_Base_Bus::IRadioCorbaDef::Ringing;
            break;
        
        case Connected:
            callStatusTypeCorba = TA_Base_Bus::IRadioCorbaDef::Connected;
            break;
        
        case ConnectedGroup:
            callStatusTypeCorba = TA_Base_Bus::IRadioCorbaDef::ConnectedGroup;
            break;
        
        case Disconnected:
            callStatusTypeCorba = TA_Base_Bus::IRadioCorbaDef::Disconnected;
            break;
        
        case OnHold:
            callStatusTypeCorba = TA_Base_Bus::IRadioCorbaDef::OnHold;
            break;
        
        case Deleted:
            callStatusTypeCorba = TA_Base_Bus::IRadioCorbaDef::Deleted;
            break;
        
        case SessionUpdate: //limin++, CL-21121
            callStatusTypeCorba = TA_Base_Bus::IRadioCorbaDef::SessionUpdate;
            break;

        default:
            ;
        }
    }

    void RadioCorbaTypeConverter::convertESessionStateCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::ESessionState& sessionStateCorba,
        ESessionState& sessionState)
    {
        switch (sessionStateCorba)
        {
        case TA_Base_Bus::IRadioStateSynchronisationCorbaDef::SessionUnassigned:
            sessionState = SessionUnassigned;
            break;
            
        case TA_Base_Bus::IRadioStateSynchronisationCorbaDef::SessionAssigned:
            sessionState = SessionAssigned;
            break;
            
        case TA_Base_Bus::IRadioStateSynchronisationCorbaDef::SessionInitialised:
            sessionState = SessionInitialised;
            break;
            
        case TA_Base_Bus::IRadioStateSynchronisationCorbaDef::SessionLoggedIn:
            sessionState = SessionLoggedIn;
            break;
            
        default:
            ;
        }
    }

    void RadioCorbaTypeConverter::convertESessionStateNormalToCorba(const ESessionState& sessionState,
        TA_Base_Bus::IRadioStateSynchronisationCorbaDef::ESessionState& sessionStateCorba)
    {
        switch (sessionState)
        {
        case SessionUnassigned:
            sessionStateCorba = TA_Base_Bus::IRadioStateSynchronisationCorbaDef::SessionUnassigned;
            break;
            
        case SessionAssigned:
            sessionStateCorba = TA_Base_Bus::IRadioStateSynchronisationCorbaDef::SessionAssigned;
            break;
            
        case SessionInitialised:
            sessionStateCorba = TA_Base_Bus::IRadioStateSynchronisationCorbaDef::SessionInitialised;
            break;
            
        case SessionLoggedIn:
            sessionStateCorba = TA_Base_Bus::IRadioStateSynchronisationCorbaDef::SessionLoggedIn;
            break;
            
        default:
            ;
        }
    }
   

    ECallPriority RadioCorbaTypeConverter::convertECallPriorityCorbaToNormal(const TA_Base_Bus::IRadioSessionCorbaDef::ECallPriority& callPriorityCorba)
    {
        FUNCTION_ENTRY("convertECallPriorityCorbaToNormal");    
        
        ECallPriority priority;   

        switch (callPriorityCorba)
        {
            case TA_Base_Bus::IRadioSessionCorbaDef::Priority1:
                priority = Priority1;
                break;
            case TA_Base_Bus::IRadioSessionCorbaDef::Priority2:
                priority = Priority2;
                break;
            case TA_Base_Bus::IRadioSessionCorbaDef::Priority3:
                priority = Priority3;
                break;
            case TA_Base_Bus::IRadioSessionCorbaDef::Priority4:
                priority = Priority4;
                break;
            case TA_Base_Bus::IRadioSessionCorbaDef::Priority5:
                priority = Priority5;
                break;
            case TA_Base_Bus::IRadioSessionCorbaDef::Priority6:
                priority = Priority6;
                break;
            case TA_Base_Bus::IRadioSessionCorbaDef::Priority7:
                priority = Priority7;
                break;
            case TA_Base_Bus::IRadioSessionCorbaDef::Priority8:
                priority = Priority8;
                break;
            case TA_Base_Bus::IRadioSessionCorbaDef::Priority9:
                priority = Priority9;            
                break;
            case TA_Base_Bus::IRadioSessionCorbaDef::Priority10:
                priority = Priority10;
                break;
            case TA_Base_Bus::IRadioSessionCorbaDef::Priority11:
                priority = Priority11;
                break;
            case TA_Base_Bus::IRadioSessionCorbaDef::Priority12:
                priority = Priority12;
                break;
            case TA_Base_Bus::IRadioSessionCorbaDef::Priority13:
                priority = Priority13;
                break;
            case TA_Base_Bus::IRadioSessionCorbaDef::Priority14:
                priority = Priority14;
                break;
            case TA_Base_Bus::IRadioSessionCorbaDef::Priority15:
                priority = Priority15;
                break;
            case TA_Base_Bus::IRadioSessionCorbaDef::DefaultNormalPriority:
                priority = DefaultNormalPriority;
                break;
            case TA_Base_Bus::IRadioSessionCorbaDef::DefaultEmergencyPriority:
                priority = DefaultEmergencyPriority;
                break;
            default:
                TA_ASSERT(false, "Invalid call priority");
        }        
        return priority;    
    }

    TA_Base_Bus::IRadioSessionCorbaDef::ECallPriority RadioCorbaTypeConverter::convertECallPriorityNormalToCorba(const ECallPriority& callPriority)
    {
        FUNCTION_ENTRY("convertECallPriorityNormalToCorba");    
    
        TA_Base_Bus::IRadioSessionCorbaDef::ECallPriority priority;

        switch (callPriority)
        {
        case Priority1:
            priority = TA_Base_Bus::IRadioSessionCorbaDef::Priority1;
            break;
        case Priority2:
            priority = TA_Base_Bus::IRadioSessionCorbaDef::Priority2;
            break;
        case Priority3:
            priority = TA_Base_Bus::IRadioSessionCorbaDef::Priority3;
            break;
        case Priority4:
            priority = TA_Base_Bus::IRadioSessionCorbaDef::Priority4;
            break;
        case Priority5:
            priority = TA_Base_Bus::IRadioSessionCorbaDef::Priority5;
            break;
        case Priority6:
            priority = TA_Base_Bus::IRadioSessionCorbaDef::Priority6;
            break;
        case Priority7:
            priority = TA_Base_Bus::IRadioSessionCorbaDef::Priority7;
            break;
        case Priority8:
            priority = TA_Base_Bus::IRadioSessionCorbaDef::Priority8;
            break;
        case Priority9:
            priority = TA_Base_Bus::IRadioSessionCorbaDef::Priority9;            
            break;
        case Priority10:
            priority = TA_Base_Bus::IRadioSessionCorbaDef::Priority10;
            break;
        case Priority11:
            priority = TA_Base_Bus::IRadioSessionCorbaDef::Priority11;
            break;
        case Priority12:
            priority = TA_Base_Bus::IRadioSessionCorbaDef::Priority12;
            break;
        case Priority13:
            priority = TA_Base_Bus::IRadioSessionCorbaDef::Priority13;
            break;
        case Priority14:
            priority = TA_Base_Bus::IRadioSessionCorbaDef::Priority14;
            break;
        case Priority15:
            priority = TA_Base_Bus::IRadioSessionCorbaDef::Priority15;
            break;
        // DefaultNormalPriority and DefaultEmergencyPriority now handled in
	    // above case values, cause compile error if used
        default:
            TA_ASSERT(false, "Invalid call priority");
        
        }    
        return priority;    
    }
	
    TA_Base_Bus::IRadioCorbaDef::EMonitorStatusType RadioCorbaTypeConverter::convertMonitorStatusTypeToCorba(TA_IRS_App::EMonitorStatusType normalStatusType)
    {
        TA_Base_Bus::IRadioCorbaDef::EMonitorStatusType corbaStatusType = TA_Base_Bus::IRadioCorbaDef::Stopped;
        switch (normalStatusType)
        {
            case Monitoring:
                corbaStatusType = TA_Base_Bus::IRadioCorbaDef::Monitoring;
                break;
            case Listening:
                corbaStatusType = TA_Base_Bus::IRadioCorbaDef::Listening;
                break;
            case Included:
                corbaStatusType = TA_Base_Bus::IRadioCorbaDef::Included;
                break;
            case Joined:
                corbaStatusType = TA_Base_Bus::IRadioCorbaDef::Joined;
                break;
            case Stopped:
            default:
                corbaStatusType = TA_Base_Bus::IRadioCorbaDef::Stopped;
        }
        return corbaStatusType;
    }

	void RadioCorbaTypeConverter::convertEOriginTypeNormalToCorba(const EOriginType& normalObject,
		TA_Base_Bus::IRadioCorbaDef::EOriginType& corbaObject)
	{
		switch (normalObject)
		{
		case ORIGIN_INVALID:
			corbaObject = TA_Base_Bus::IRadioCorbaDef::ORIGIN_INVALID;
			break;

		case INBOUND:
			corbaObject = TA_Base_Bus::IRadioCorbaDef::INBOUND;
			break;

		case OUTBOUND:
			corbaObject = TA_Base_Bus::IRadioCorbaDef::OUTBOUND;
			break;

		default:
			corbaObject = TA_Base_Bus::IRadioCorbaDef::ORIGIN_INVALID;
		}
	}

	void RadioCorbaTypeConverter::convertEStackCallTypeNormalToCorba(const EStackCallType& normalObject,
		TA_Base_Bus::IRadioCorbaDef::EStackCallType& corbaObject)
	{
		switch (normalObject)
		{
		case CALL_INVALID:
			corbaObject = TA_Base_Bus::IRadioCorbaDef::CALL_INVALID;
			break;

		case VOICE:
			corbaObject = TA_Base_Bus::IRadioCorbaDef::VOICE;
			break;

		case CONFERENCE:
			corbaObject = TA_Base_Bus::IRadioCorbaDef::CONFERENCE;
			break;

		case TEXT:
			corbaObject = TA_Base_Bus::IRadioCorbaDef::TEXT;
			break;

		case GROUP_ACK:
			corbaObject = TA_Base_Bus::IRadioCorbaDef::GROUP_ACK;
			break;

		case GROUP_UNACK:
			corbaObject = TA_Base_Bus::IRadioCorbaDef::GROUP_UNACK;
			break;

		case GROUP_BROADCAST:
			corbaObject = TA_Base_Bus::IRadioCorbaDef::GROUP_BROADCAST;
			break;

		default:
			corbaObject = TA_Base_Bus::IRadioCorbaDef::CALL_INVALID;
		}
	}

	void RadioCorbaTypeConverter::convertSubscriberNotificationTypeNormalToCorba(const SubscriberNotificationType& normalObject,
		TA_Base_Bus::IRadioCorbaDef::SubscriberNotificationType& notifyTypeCorba)
	{
		switch (normalObject)
		{
		case NEW_SUBSCRIBER:
			notifyTypeCorba = TA_Base_Bus::IRadioCorbaDef::NEW_SUBSCRIBER;
			break;
		case UPDATE_SUBSCRIBER:
			notifyTypeCorba = TA_Base_Bus::IRadioCorbaDef::UPDATE_SUBSCRIBER;
			break;
		case DELETE_SUBSCRIBER:
			notifyTypeCorba = TA_Base_Bus::IRadioCorbaDef::DELETE_SUBSCRIBER;
			break;
		case SCAN_SUBSCRIBERS:
			notifyTypeCorba = TA_Base_Bus::IRadioCorbaDef::SCAN_SUBSCRIBERS;
			break;
		}
	}

	void RadioCorbaTypeConverter::convertSubscriberTypeNormalToCorba(const SubscriberType& normalObject,
		TA_Base_Bus::IRadioCorbaDef::SubscriberType& corbaObject)
	{
		switch (normalObject)
		{
		case INDIVIDUAL_SUBSCRIBER:
			corbaObject = TA_Base_Bus::IRadioCorbaDef::INDIVIDUAL_SUBSCRIBER;
			break;
		case GROUP_SUBSCRIBER:
			corbaObject = TA_Base_Bus::IRadioCorbaDef::GROUP_SUBSCRIBER;
			break;
		case PATCH_SUBSCRIBER:
			corbaObject = TA_Base_Bus::IRadioCorbaDef::PATCH_SUBSCRIBER;
			break;
		case DISPATCHER_SUBSCRIBER:
			corbaObject = TA_Base_Bus::IRadioCorbaDef::DISPATCHER_SUBSCRIBER;
			break;
		case MOBILE_SUBSCRIBER:
			corbaObject = TA_Base_Bus::IRadioCorbaDef::MOBILE_SUBSCRIBER;
			break;
		}
	}

	TA_Base_Bus::IRadioSessionCorbaDef::EMonitorStatusType RadioCorbaTypeConverter::convertEMonitorStatusTypeToCorba(TA_IRS_App::EMonitorStatusType monStatus)
	{
		TA_Base_Bus::IRadioSessionCorbaDef::EMonitorStatusType retVal;

		switch (monStatus)
		{
		case Monitoring:
			retVal = TA_Base_Bus::IRadioSessionCorbaDef::Monitoring;
			break;
		case Listening:
			break;
			retVal = TA_Base_Bus::IRadioSessionCorbaDef::Listening;
		case PendingInclude:
		case Included:
			retVal = TA_Base_Bus::IRadioSessionCorbaDef::Included;
			break;
		case Joined:
			retVal = TA_Base_Bus::IRadioSessionCorbaDef::Joined;
			break;
		case Stopped:
		case NotMonitored:
			retVal = TA_Base_Bus::IRadioSessionCorbaDef::Stopped;
			break;
		}

		return retVal;
	}
}
