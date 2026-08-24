/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/C830G_TIP/830G/transactive/app/radio/RadioTetraAgent/src/RadioStateSynchronisationServant.cpp $
  * @author:  John Dalin
  * @version: $Revision: #16 $
  *
  * Last modification: $DateTime: 2021/11/10 12:13:46 $
  * Last modified by:  $Author: builder $
  *
  * The RadioStateSynchronisationServant is a class that is used for synchronise state data between
  * the Radio Tetra Agent that is in control mode and the Radio Tetra Agent that is in monitor
  * mode.
  */

#if defined(_MSC_VER)
#pragma warning(disable:4503)
#endif // _MSC_VER
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/types/RadioTetraAgentStateUpdate_MessageTypes.h"
#include "core/message/types/RadioComms_MessageTypes.h"
#include "core/utilities/src/RunParams.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/naming/src/NamingMacros.h"
#include "app/radio/RadioTetraAgent/src/RadioStateSynchronisationServant.h"
#include "RadioStateUpdateBroadcaster.h"
#include "RadioStateMessageConverter.h"
#include "bus/generic_agent/src/GenericAgent.h"

#pragma warning (disable : 4355)


//limin++, failover
namespace
{
    template< typename T > class ThreadedMemberFunction : public TA_Base_Core::Thread
    {
        typedef void ( T::* MemberFunction )();
        typedef std::vector< MemberFunction > MemberFunctionList;
        
    public:
        
        static ThreadedMemberFunction& alloc( T& o, MemberFunction mf = NULL )
        {
			TA_Base_Core::ThreadGuard( lock() );
            
            static ThreadedMemberFunction* instance = NULL;
            
            instance = new ThreadedMemberFunction( o, instance );
            
            if ( NULL != mf )
            {
                instance->add( mf );
                
                instance->start();
            }
            
            return *instance;
        }
        
        void add( MemberFunction mf )
        {
            m_members.push_back( mf );
        }
        
    protected:
        
        virtual void run()
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Function Entered: ThreadedMemberFunction, there are [%d] member function(s) to call", m_members.size() );
            
            for ( typename MemberFunctionList::iterator mfIter = m_members.begin(); mfIter != m_members.end(); ++mfIter )
            {
                try
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ThreadedMemberFunction: calling [%d] member function", std::distance( m_members.begin(), mfIter ) );
                    
                    ( m_object.*( *mfIter ) )();
                }
                catch (...)
                {
                    LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "" );
                }
            }
            
            clearPrivious();
            
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Function Exited: ThreadedMemberFunction" );
        }
        
        virtual void terminate(){}
        
    private:
        
        ThreadedMemberFunction( T& o, ThreadedMemberFunction* privious )
            : m_object( o ),
            m_privious( privious )
        {
        }
        
        ThreadedMemberFunction( const ThreadedMemberFunction& );
        ThreadedMemberFunction& operator= ( const ThreadedMemberFunction& );
        
        ~ThreadedMemberFunction()
        {
            terminateAndWait();
        }
        
        void clearPrivious()
        {
            delete m_privious;
            m_privious = NULL;
        }
        
        static TA_Base_Core::NonReEntrantThreadLockable& lock()
        {
            static TA_Base_Core::NonReEntrantThreadLockable lock;
            return lock;
        }
        
    private:
        
        T& m_object;
        MemberFunctionList m_members;
        ThreadedMemberFunction* m_privious;
    };
    
    typedef ThreadedMemberFunction< TA_IRS_App::RadioStateSynchronisationServant > RadioStateSynchronisationServantThreadedMemberFunction;
}
//limin++, failover


namespace TA_IRS_App
{
    
    RadioStateSynchronisationServant::RadioStateSynchronisationServant(IRadioTetraAgentState& radioState,
        TA_Base_Core::RadioStateSynchronisationEntityDataPtr entityData,
		Radio& radio)
        : m_radioTetraAgentState(radioState)
        , m_radioStateUpdateReceiver(radioState)
        , m_radioFullStateReceiver(radioState, *this, m_radioStateUpdateReceiver)
        , m_state(INITIAL)
        , m_subscribed(false)
		, m_monAgentState(false)
		, m_radio(radio)
    {
        FUNCTION_ENTRY("Constructor");

        m_objectName = entityData->getName();
        m_entityKey = entityData->getKey();
        m_entityLocation = entityData->getLocation();
        m_entitySubsystem = entityData->getSubsystem();
        m_entityTypeKey = entityData->getTypeKey();

        // Setup the peer radio agent connection used for synchronisation
        m_peerRadioTetraAgent.setEntityName(m_objectName, false);

        // Let the broadcaster know the entity key
        RadioStateUpdateBroadcaster::getInstance()->configure(m_entityKey, m_entityLocation);

        FUNCTION_EXIT;
    }
    
    RadioStateSynchronisationServant::~RadioStateSynchronisationServant()
    {
        FUNCTION_ENTRY("Destructor");
    
        stop();

        FUNCTION_EXIT;
    }
    
    TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioTetraAgentType* RadioStateSynchronisationServant::requestFullState()
    {
        FUNCTION_ENTRY("requestFullState");

		TA_Base_Bus::GenericAgent::ensureControlMode();
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[CONTROL] Monitor agent is requesting full state" );
        RadioTetraAgentType agentState;
        TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioTetraAgentType* newPeerAgentState =
            new TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioTetraAgentType();

		// TD9016 move getFullState() to RadioStateSynchronisationServant to prevent deadlock in RadioTetraAgent class
        //m_radioTetraAgentState.getFullState(agentState); 
		getFullState(agentState); 
        
		std::ostringstream data;
		data << std::endl
			 << "RadioServant --> Entity: " 
			 << agentState.radioServant.entityKey
			 << std::endl;

		RadioType::const_iterator it = agentState.radioServant.radio.begin();

		for(  ; it != agentState.radioServant.radio.end(); it ++ )
		{
			data << "CallStack " << it->first << ": "
				<< it->second.radioCallStack.size()
				<< " --- Control connection "<< it->first << " is "
				<< static_cast<int>(it->second.controlConnectionStatus) << std::endl;
		}

		for ( unsigned int i =0; i < agentState.radioSessionServants.size(); i++ )
		{
			RadioSessionServantType type = agentState.radioSessionServants[i];
			data << "RadioSessionServent --> Entity: " << type.entityKey << " "
				 << "SessionName: " << type.radioSession.sessionEntityName << " "
				 << "SessionRef: " << type.radioSession.sessionRef << " "
				 << "SessionStatus: " << static_cast<ESessionState>(type.radioSession.sessionStatus) << std::endl;
		}
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Agent's Full State... %s", data.str().c_str() );

        RadioStateMessageConverter::convertRadioTetraAgentNormalToCorba(agentState, *newPeerAgentState);

        return newPeerAgentState;

        FUNCTION_EXIT;
    }
    
    void RadioStateSynchronisationServant::receiveSpecialisedMessage(
        const TA_Base_Core::StateUpdateMessageCorbaDef& message)
    {
        FUNCTION_ENTRY("receiveSpecialisedMessage");
        
        // determine message type then process message
        TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioUpdateCallInRadioCallStack* updateCallInRadioCallStackCorba = 0;
		TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioRemoveCallFromRadioCallStack* removeCallFromRadioCallStackCorba = 0;
		TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioUpdateRadioSession* updateRadioSessionCorba = 0;
		TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioUpdateMonitoredCallProgression* updateMonitoredCallProgressionCorba = 0;	
		TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioRemoveMonitoredSubscriberDetails* removeMonitoredSubscriberDetailsCorba = 0;	
		TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioRemoveSessionFromAudioMap* removeSessionFromAudioMapCorba = 0;
		TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioUpdateAudioEntryInSessionAudioMap* updateAudioEntryInSessionAudioMapCorba = 0;
		TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioUpdateRadioResourceManagerVars* updateRadioResourceManagerVarsCorba = 0;
		TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioUpdateAuthorisationRequest* updateAuthorisationRequestCorba = 0;	
		TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioRemoveAuthorisationRequest* removeAuthorisationRequestCorba = 0;
		TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioConnectionSwitchRequest* connectionSwitchRequest = 0;
		TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioUpdateCallStackHistory* updateCallStackHistory = 0;
		TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioAddCallStackEntry* addCallStackEntry = 0;
		TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioDeleteCallStackEntry* deleteCallStackEntry = 0;
		TA_Base_Bus::IRadioStateSynchronisationCorbaDef::SubscriberStateUpdate* subStateUpdate = 0;

	    if ( 0 != ( message.stateUpdateInfo >>= updateCallInRadioCallStackCorba ) )   // Is this an Update Call In Radio Call Stack message?
		{
            RadioUpdateCallInRadioCallStack* updateCallInRadioCallStack =
                new RadioUpdateCallInRadioCallStack();

            RadioStateMessageConverter::convertRadioUpdateCallInRadioCallStackCorbaToNormal(*updateCallInRadioCallStackCorba,
                                                                *updateCallInRadioCallStack);
            m_radioStateUpdateReceiver.receiveStateUpdateMessage(*updateCallInRadioCallStack);
		}
	    else if ( 0 != ( message.stateUpdateInfo >>= removeCallFromRadioCallStackCorba ) )   // Is this a Remove Call From Radio Call Stack message?
		{
            RadioRemoveCallFromRadioCallStack* removeCallFromRadioCallStack =
                new RadioRemoveCallFromRadioCallStack();
            RadioStateMessageConverter::convertRadioRemoveCallFromRadioCallStackCorbaToNormal(*removeCallFromRadioCallStackCorba,
                                                                  *removeCallFromRadioCallStack);
            m_radioStateUpdateReceiver.receiveStateUpdateMessage(*removeCallFromRadioCallStack);
		}
	    else if ( 0 != ( message.stateUpdateInfo >>= updateRadioSessionCorba ) )   // Is this an Update Radio Session message?
		{
			RadioUpdateRadioSession* updateRadioSession = 
				new RadioUpdateRadioSession();
            RadioStateMessageConverter::convertRadioUpdateRadioSessionCorbaToNormal(*updateRadioSessionCorba,
                                                                  *updateRadioSession);
            m_radioStateUpdateReceiver.receiveStateUpdateMessage(*updateRadioSession);
		}
		else if ( 0 != ( message.stateUpdateInfo >>= updateMonitoredCallProgressionCorba ) )   // Is this an Update Monitored CallProgression message?
		{
			RadioUpdateMonitoredCallProgression* updateMonitoredCallProgression = 
							new RadioUpdateMonitoredCallProgression();
            RadioStateMessageConverter::convertRadioUpdateMonitoredCallProgressionCorbaToNormal(*updateMonitoredCallProgressionCorba,
                                                                  *updateMonitoredCallProgression);
            m_radioStateUpdateReceiver.receiveStateUpdateMessage(*updateMonitoredCallProgression);
		}
		else if ( 0 != ( message.stateUpdateInfo >>= removeMonitoredSubscriberDetailsCorba ) )   // Is this an Update Monitored CallProgression message?
		{
			RadioRemoveMonitoredSubscriberDetails* removeMonitoredSubscriberDetails = 
							new RadioRemoveMonitoredSubscriberDetails();
            RadioStateMessageConverter::convertRadioRemoveMonitoredSubscriberDetailsCorbaToNormal(*removeMonitoredSubscriberDetailsCorba,
                                                                  *removeMonitoredSubscriberDetails);
            m_radioStateUpdateReceiver.receiveStateUpdateMessage(*removeMonitoredSubscriberDetails);
		}
		else if ( 0 != ( message.stateUpdateInfo >>= removeSessionFromAudioMapCorba ) )   // Is this RemoveSessionFromAudioMap Message?
		{
            RadioRemoveSessionFromAudioMap* removeSessionFromAudioMap =
                new RadioRemoveSessionFromAudioMap();
            RadioStateMessageConverter::convertRadioRemoveSessionFromAudioMapCorbaToNormal(*removeSessionFromAudioMapCorba,
                                                                  *removeSessionFromAudioMap);
            m_radioStateUpdateReceiver.receiveStateUpdateMessage(*removeSessionFromAudioMap);
		}
		else if ( 0 != ( message.stateUpdateInfo >>= updateAudioEntryInSessionAudioMapCorba ) )   // Is this UpdateAudioEntryInSessionAudioMap Message?
		{
            RadioUpdateAudioEntryInSessionAudioMap * updateAudioEntryInSessionAudioMap  =
                new RadioUpdateAudioEntryInSessionAudioMap ();
            RadioStateMessageConverter::convertRadioUpdateAudioEntryInSessionAudioMapCorbaToNormal(*updateAudioEntryInSessionAudioMapCorba,
                                                                  *updateAudioEntryInSessionAudioMap );
            m_radioStateUpdateReceiver.receiveStateUpdateMessage(*updateAudioEntryInSessionAudioMap);
		}
		else if ( 0 != ( message.stateUpdateInfo >>= updateRadioResourceManagerVarsCorba ) )   // Is this UpdateRadioResourceManagerVars Message?
		{
            RadioUpdateRadioResourceManagerVars * updateRadioResourceManagerVars  =
                new RadioUpdateRadioResourceManagerVars ();
            RadioStateMessageConverter::convertRadioUpdateRadioResourceManagerVarsCorbaToNormal(*updateRadioResourceManagerVarsCorba,
                                                                  *updateRadioResourceManagerVars );
            m_radioStateUpdateReceiver.receiveStateUpdateMessage(*updateRadioResourceManagerVars);
		}
		else if ( 0 != ( message.stateUpdateInfo >>= updateAuthorisationRequestCorba ) )   // Is this UpdateAuthorisationRequest Message?
		{
            RadioUpdateAuthorisationRequest * updateAuthorisationRequest  =
                new RadioUpdateAuthorisationRequest ();
            RadioStateMessageConverter::convertRadioUpdateAuthorisationRequestCorbaToNormal(*updateAuthorisationRequestCorba,
                                                                  *updateAuthorisationRequest );
            m_radioStateUpdateReceiver.receiveStateUpdateMessage(*updateAuthorisationRequest);
		}
		else if ( 0 != ( message.stateUpdateInfo >>= removeAuthorisationRequestCorba ) )   // Is this Remove AuthorisationRequest Message?
		{
            RadioRemoveAuthorisationRequest * removeAuthorisationRequest  =
                new RadioRemoveAuthorisationRequest();
            RadioStateMessageConverter::convertRadioRemoveAuthorisationRequestCorbaToNormal(*removeAuthorisationRequestCorba,
                                                                  *removeAuthorisationRequest);
            m_radioStateUpdateReceiver.receiveStateUpdateMessage(*removeAuthorisationRequest);
		}
		else if (0 != (message.stateUpdateInfo >>= connectionSwitchRequest))   // Is this Remove AuthorisationRequest Message?
		{
			RadioConnectionSwitch * radioConnectionSwitchUpdate =
				new RadioConnectionSwitch();
			RadioStateMessageConverter::convertRadioConnectionSwitchCorbaToNormal(*connectionSwitchRequest,
				*radioConnectionSwitchUpdate);
			m_radioStateUpdateReceiver.receiveStateUpdateMessage(*radioConnectionSwitchUpdate);
		}
		else if (0 != (message.stateUpdateInfo >>= updateCallStackHistory))   // Is this Update Call Stack History Message?
		{
			RadioUpdateCallStackHistory * radioUpdateCallStackHistory =
				new RadioUpdateCallStackHistory();
			RadioStateMessageConverter::convertRadioUpdateCallStackHistoryCorbaToNormal(*updateCallStackHistory,
				*radioUpdateCallStackHistory);
			m_radioStateUpdateReceiver.receiveStateUpdateMessage(*radioUpdateCallStackHistory);
		}
		else if (0 != (message.stateUpdateInfo >>= addCallStackEntry))   // Is this Add Call Stack Entry Message?
		{
			RadioAddCallStackEntry * radioAddCallStackEntry =
				new RadioAddCallStackEntry();
			RadioStateMessageConverter::convertRadioAddCallStackEntryCorbaToNormal(*addCallStackEntry,
				*radioAddCallStackEntry);
			m_radioStateUpdateReceiver.receiveStateUpdateMessage(*radioAddCallStackEntry);
		}
		else if (0 != (message.stateUpdateInfo >>= deleteCallStackEntry))   // Is this Delete Call Stack Entry Message?
		{
			RadioDeleteCallStackEntry * radioDeleteStackEntry =
				new RadioDeleteCallStackEntry();
			RadioStateMessageConverter::convertRadioDeleteCallStackEntryCorbaToNormal(*deleteCallStackEntry,
				*radioDeleteStackEntry);
			m_radioStateUpdateReceiver.receiveStateUpdateMessage(*radioDeleteStackEntry);
		}
		else if (0 != (message.stateUpdateInfo >>= subStateUpdate))   // Is this Subscriber State Update Message?
		{
			SubscriberStateUpdate * subscriberUpdate =
				new SubscriberStateUpdate();
			RadioStateMessageConverter::convertRadioSubscriberUpdateCorbaToNormal(*subStateUpdate,
				*subscriberUpdate);
			m_radioStateUpdateReceiver.receiveStateUpdateMessage(*subscriberUpdate);
		}

        FUNCTION_EXIT;
    }

	TA_Base_Bus::IRadioStateSynchronisationCorbaDef::PredefinedSDSList* RadioStateSynchronisationServant::getFullSDSList(bool readWrite)
	{
		FUNCTION_ENTRY("getFullSDSList()");
		TA_Base_Bus::GenericAgent::ensureControlMode();

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Retrieving Predefined SDS Messages from Database");


		// Get the full SDS message list from the database
		std::vector<TA_Base_Core::IRAPredefinedSDSMessage*> sdsList = TA_Base_Core::RAPredefinedSDSMessageAccessFactory::getInstance().getList(false);

		// Create the selected call list for returning.
		TA_Base_Bus::IRadioStateSynchronisationCorbaDef::PredefinedSDSList* corbaSDSlist =
			new TA_Base_Bus::IRadioStateSynchronisationCorbaDef::PredefinedSDSList();

		// Set it's length (it's a CORBA sequence, and as such needs its length specifed, because
		// sequences don't automatically increase in size)
		corbaSDSlist->length(sdsList.size());

		// Now put the data from the IRAPredefinedSDSMessage into the PredefinedSDSMessage struct in the CORBA sequence
		for (unsigned int i = 0; i < sdsList.size(); i++)
		{
			(*corbaSDSlist)[i].shortName = sdsList[i]->getShortName().c_str();
			(*corbaSDSlist)[i].message = sdsList[i]->getMessage().c_str();



			// And delete the database member while we're here
			delete sdsList[i];
			sdsList[i] = NULL;
		}
		return corbaSDSlist;
		FUNCTION_EXIT;
	}


	TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioCallStackHistoryList* RadioStateSynchronisationServant::requestCallStackHistory()
	{
		FUNCTION_ENTRY("requestCallStackHistory()");
		TA_Base_Bus::GenericAgent::ensureControlMode();

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Retrieving Call Stack History from Database");

		// Get the selected calls list from the database
		std::vector<TA_Base_Core::IRACallStackHistory*> callStack =
			TA_Base_Core::RACallStackHistoryAccessFactory::getInstance().getFullList();

		TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioCallStackHistoryList* callStackList =
			new TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioCallStackHistoryList();

		// Set it's length (it's a CORBA sequence, and as such needs its length specifed, because
		// sequences don't automatically increase in size)
		callStackList->length(callStack.size());
		// Now put the data from the IRACallStackHistory into the CallStackHistory struct in the CORBA sequence
		for (unsigned int i = 0; i < callStack.size(); i++)
		{
			(*callStackList)[i].profile = callStack[i]->getProfileName().c_str();
			(*callStackList)[i].locationKey = callStack[i]->getLocationKey();
			(*callStackList)[i].identifier = callStack[i]->getIdentifier().c_str();
			(*callStackList)[i].location = callStack[i]->getLocation().c_str();
			(*callStackList)[i].timestamp = callStack[i]->getTimestamp();
			(*callStackList)[i].isEmergency = callStack[i]->isCallEmergency();

			IRACallStackHistory::CallType type = callStack[i]->getCallType();

			switch (type)
			{
			case IRACallStackHistory::CALL_INVALID:
			{
				(*callStackList)[i].callType = TA_Base_Bus::IRadioCorbaDef::CALL_INVALID;
				break;
			}
			case IRACallStackHistory::VOICE:
			{
				(*callStackList)[i].callType = TA_Base_Bus::IRadioCorbaDef::VOICE;
				break;
			}
			case IRACallStackHistory::CONFERENCE:
			{
				(*callStackList)[i].callType = TA_Base_Bus::IRadioCorbaDef::CONFERENCE;
				break;
			}
			case IRACallStackHistory::TEXT:
			{
				(*callStackList)[i].callType = TA_Base_Bus::IRadioCorbaDef::TEXT;
				break;
			}
			case IRACallStackHistory::GROUP_ACK:
			{
				(*callStackList)[i].callType = TA_Base_Bus::IRadioCorbaDef::GROUP_ACK;
				break;
			}
			case IRACallStackHistory::GROUP_UNACK:
			{
				(*callStackList)[i].callType = TA_Base_Bus::IRadioCorbaDef::GROUP_UNACK;
				break;
			}
			case IRACallStackHistory::GROUP_BROADCAST:
			{
				(*callStackList)[i].callType = TA_Base_Bus::IRadioCorbaDef::GROUP_BROADCAST;
				break;
			}
			}

			IRACallStackHistory::OriginType otype = callStack[i]->getOrigin();

			switch (otype)
			{
			case IRACallStackHistory::ORIGIN_INVALID:
			{
				(*callStackList)[i].originType = TA_Base_Bus::IRadioCorbaDef::ORIGIN_INVALID;
				break;
			}
			case IRACallStackHistory::INBOUND:
			{
				(*callStackList)[i].originType = TA_Base_Bus::IRadioCorbaDef::INBOUND;
				break;
			}
			case IRACallStackHistory::OUTBOUND:
			{
				(*callStackList)[i].originType = TA_Base_Bus::IRadioCorbaDef::OUTBOUND;
				break;
			}
			}

			// And delete the database member while we're here
			delete callStack[i];
			callStack[i] = NULL;
		}
		FUNCTION_EXIT;
		return callStackList;
	}

	void RadioStateSynchronisationServant::getPeerSDSList()
	{
		FUNCTION_ENTRY("getPeerSDSList");

		TA_Base_Bus::IRadioStateSynchronisationCorbaDef::PredefinedSDSList_var sdsList;
		try
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Requesting Predefined SDS List ...");
			CORBA_CALL_RETURN(sdsList, m_peerRadioTetraAgent, getFullSDSList, (false));
		}
		catch (...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to get Predefined SDS List");

			FUNCTION_EXIT;
			return;
		}

		if (sdsList->length() > 0)
		{
			TA_Base_Core::RAPredefinedSDSMessageAccessFactory::getInstance().deleteAllMessages();
			for (unsigned int i = 0; i < sdsList->length(); i++)
			{
				m_radio.createMessage(sdsList[i].shortName, sdsList[i].message, true);
			}
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "No Predefined SDS Data retrieved.");
		}

		FUNCTION_EXIT;
	}

	void RadioStateSynchronisationServant::getPeerCallStackHistory()
	{
		FUNCTION_ENTRY("getPeerCallStackHistory");

		TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioCallStackHistoryList_var history;

		try
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Requesting Call Stack History ...");
			CORBA_CALL_RETURN(history, m_peerRadioTetraAgent, requestCallStackHistory, ());

		}
		catch (...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to get Call Stack History");

			FUNCTION_EXIT;
			return;
		}

		if (history->length() > 0)
		{
			m_radio.clearCallStackHistory();
			for (unsigned int entry = 0; entry < history->length(); entry++)
			{
				m_radio.addCallStackEntry(
					history[entry].profile.in(),
					history[entry].locationKey,
					history[entry].timestamp,
					history[entry].originType,
					history[entry].callType,
					history[entry].identifier.in(),
					history[entry].location.in(),
					history[entry].isEmergency,
					history[entry].tsi.in()
				);
			}
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "No Call Stack History Data retrieved.");
		}
		FUNCTION_EXIT;
	}
    
    bool RadioStateSynchronisationServant::getFullPeerState(RadioTetraAgentType& state)
    {
        FUNCTION_ENTRY("getFullState");

        TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioTetraAgentType_var peerAgentState;
        bool successful = false;

        // get state from peer agent
        try
        {
			// TD9016 add more logs
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Requesting Full State...");

			unsigned int retry = 0;

			while ( retry < 3 )
			{
				try
				{
					CORBA_CALL_RETURN( peerAgentState, m_peerRadioTetraAgent, requestFullState, () );
					successful=true;
					break;
				}
				catch ( CORBA::Exception& e)
				{
					LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception while receiving full state",
                                 TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
				}

				retry ++;
			}
            if(!successful)
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Failed to recieve full state!");
				return false;
			}
            // no exception, its a success
            /*successful = true;*/
			std::ostringstream data;

			data << std::endl 
				 << "RadioServant --> Entity: " 
				 << peerAgentState->radioServant.entityKey
				 << std::endl;

			for(  unsigned int i = 0; 
				i< peerAgentState->radioServant.radio.length(); 
				i ++ )
			{
				data << "Callstack " << i << ": " 
					<< peerAgentState->radioServant.radio[i].tcpServer.radioCallStack.length()
					<< " --- Control connection "<< i << " is "
					<< static_cast<int>(peerAgentState->radioServant.radio[i].tcpServer.controlConnectionStatus) << std::endl;
			}
	
			for ( unsigned int j =0; j < peerAgentState->radioSessionServants.length(); j++ )
			{
				TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioSessionServantType type = peerAgentState->radioSessionServants[j];
				data << "RadioSessionServent --> Entity: " << type.entityKey << " "
					 << "SessionName: " << type.radioSession.sessionEntityName << " "
					 << "SessionRef: " << type.radioSession.sessionRef << " "
					 << "SessionStatus: " << static_cast<ESessionState>(type.radioSession.sessionStatus) << std::endl;
			}

			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Received Full State... %s", data.str().c_str() );

//				// TD9016 Cannot apply the state when session refs received is 0
            state.radioSessionServants.clear();
            
            // convert the corba state to c++ state
            RadioStateMessageConverter::convertRadioTetraAgentCorbaToNormal(peerAgentState, state);
        }
        catch ( TA_Base_Core::TransactiveException& e)
	    {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException while receiving full state", e.what() );
	    }
        catch ( CORBA::Exception& e)
	    {
            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception while receiving full state",
                                 TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );
	    }
	    catch (...)
	    {
            LOG_EXCEPTION_CATCH( SourceInfo, "...", "while receiving full state" );
	    }

        FUNCTION_EXIT;
        return successful;
    }
    
    bool RadioStateSynchronisationServant::isValid()
    {
        FUNCTION_ENTRY("isValid");
        FUNCTION_EXIT;

        return true;
    }
    
    void RadioStateSynchronisationServant::start()
    {
        FUNCTION_ENTRY("start");

	    activateServantWithName(m_objectName);

        FUNCTION_EXIT;
    }
    
    void RadioStateSynchronisationServant::stop()
    {
        FUNCTION_ENTRY("stop");
        stopSubscription();
	    deactivateServant();

        FUNCTION_EXIT;
    }
    
    void RadioStateSynchronisationServant::update(const class TA_Base_Core::ConfigUpdateDetails& details)
    {
        FUNCTION_ENTRY("update");
        FUNCTION_EXIT;
    }
    
    void RadioStateSynchronisationServant::remove()
    {
        FUNCTION_ENTRY("remove");
        FUNCTION_EXIT;
    }
    
    void RadioStateSynchronisationServant::setToControlMode()
    {
        FUNCTION_ENTRY("setToControlMode");

        // TD13975 this function doesn't ever exit, causing the agent to be
        // stuck in "Changing To Control". Temporarily disabled to get through
        // FAT Phase1&2.
        //        return;
        //        FUNCTION_EXIT;

        //limin, for failover
        RadioStateSynchronisationServantThreadedMemberFunction::alloc( *this, &RadioStateSynchronisationServant::setToControlModeInternal );

        FUNCTION_EXIT;
    }


    //limin++, for failover
    void RadioStateSynchronisationServant::setToControlModeInternal()
    {
        FUNCTION_ENTRY( "setToControlModeInternal" );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[CONTROL] RadioStateSynchronisationServant::setToControlMode" );

        TA_Base_Core::ThreadGuard guard(m_lockable);

        ServantState oldState = m_state;
        m_state = CONTROL;

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[CONTROL] Set Monitor Agent STATE = FALSE" );
		m_monAgentState = false; //TD15926

        if (oldState == MONITOR)
        {
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[CONTROL] Old state in MONITOR" );
            bool acquired;

            // If still acquiring resume until complete or failure, otherwise continue
            if (m_radioFullStateReceiver.isAcquiring())
            {
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[CONTROL] waitForAcquisition" );
                acquired = m_radioFullStateReceiver.waitForAcquisition();
            }
            else
            {
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[CONTROL] hasAcquired" );
                acquired = m_radioFullStateReceiver.hasAcquired();
            }
            
            // If full state acquired, finish processing single state updates
            // then notify the radio tetra agent state that it is complete
            if (acquired)
            {
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[CONTROL] Full State acquired" );
                stopSubscription();
                m_radioStateUpdateReceiver.waitUntilAllProcessed();
                m_radioStateUpdateReceiver.pauseProcessing();
                m_radioTetraAgentState.notifyStateUpdatesComplete();
            }
            else
            {
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[CONTROL] Full State NOT acquired" );
                // Otherwise clear all state update messages that have been queued
                // and clear the entire state of the radio tetra agent as though
                // it has just started up in Control Mode from the initial state.
                m_radioFullStateReceiver.stopAcquisition();
                stopSubscription();
                m_radioStateUpdateReceiver.pauseProcessing();
                m_radioStateUpdateReceiver.clearStateUpdateMessages();
                m_radioTetraAgentState.clearFullState();
            }
        }

        FUNCTION_EXIT;
    }
    
    void RadioStateSynchronisationServant::setToMonitorMode()
    {
        FUNCTION_ENTRY("setToMonitorMode");

        TA_Base_Core::ThreadGuard guard(m_lockable);

        ServantState oldState = m_state;
        m_state = MONITOR;

        if (oldState == INITIAL)
        {
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[MONITOR] Start from INITIAL State" );
            // Start to receive state update messages from the peer agent
            // that is in Control Mode and acquire that full state
            startSubscription();
            m_radioFullStateReceiver.startAcquisition();

			// Send request  to peer agent in Control mode to retrieve
			// updated call stack history
			getPeerCallStackHistory();
			
			//Send request to peer agent in Control mode to retrieve
			// updated Predefined SDS list
			getPeerSDSList();
        }
        else if (oldState == CONTROL)
        {
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[MONITOR] Start from CONTROL State" );
            // Changing from Control To Monitor mode
            // Full state must be requested from the peer agent
            // as it is not known at this point whether the peer agent has
            // cleared its own state or not.
            m_radioStateUpdateReceiver.clearStateUpdateMessages();
			//clear the state once there is a valid state to apply
            //m_radioTetraAgentState.clearFullState(); 
            startSubscription();
            m_radioFullStateReceiver.startAcquisition();
        }        
		// TD15926
		try
		{
            CORBA_CALL( m_peerRadioTetraAgent, setMonitorAgentState, ( true ) );
		}
		catch (...)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "[MONITOR] Unable to notify control agent" );
		}
		// TD15926

        FUNCTION_EXIT;
    }

    void RadioStateSynchronisationServant::startSubscription()
    {
        FUNCTION_ENTRY("startSubscription");

        if ( false == m_subscribed )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                         "Radio Agent subscribing for state updates" );
            // subscribe to receive state update messages from peer agent
            TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToStateUpdateMessage( 
                TA_Base_Core::RadioTetraAgentStateUpdate::Context,
		        this, TA_Base_Core::RunParams::getInstance().get( RPARAM_ENTITYNAME ), NULL, m_entityLocation, TA_Base_Core::NsaTypeRegular);
            m_subscribed = true;
        }           

        FUNCTION_EXIT;
    }
    
    void RadioStateSynchronisationServant::stopSubscription()
    {
        FUNCTION_ENTRY("stopSubscription");

        if ( true == m_subscribed )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                        "Radio Agent unsubscribing for state updates" );
            // unsubscribe from receiving state update messages from peer agent
            TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
            m_subscribed = false;
        }

        FUNCTION_EXIT;
    }

	//TD15926
	bool RadioStateSynchronisationServant::isPeerAvailable()
	{
		TA_Base_Core::ThreadGuard guard(m_lockable);

		if (m_state == MONITOR)
		{
			try
			{
				// TD15926 notify the controlling agent that 'I' am terminating
                CORBA_CALL( m_peerRadioTetraAgent, setMonitorAgentState, ( false ) );
				return true; // monitor agent cannot detach session
			}
			catch (...)
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "[MONITOR] Unable to notify control agent" );
				return false;
			}
		}
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[CONTROL] isPeerAvailable = %d",m_monAgentState );		
		return m_monAgentState;
	}

	////////////////////////////////////////////////////////////////////////////////
	// *The methods below are added for TD9016 

	/* 
	 * This IDL method implementation is used to notify the agent(control) that the monitor agent is active.
	 */
	void RadioStateSynchronisationServant::setMonitorAgentState(bool state)
	{
		TA_Base_Core::ThreadGuard guard(m_lockable);
		TA_Base_Bus::GenericAgent::ensureControlMode();
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[CONTROL] Set Monitor Agent STATE = %d", state );
		m_monAgentState = state;
	}


	/** 
	 * This function is called when the monitoring agent has retrieved
	 * the full state from the controlling agent
	 */
	void RadioStateSynchronisationServant::applyFullState(const RadioTetraAgentType& state)
	{
		TA_Base_Core::ThreadGuard guard(m_lockable);
        std::vector<RadioSessionServant*> radioSessionServants = m_radioTetraAgentState.getRadioSessionServants();

		m_radioTetraAgentState.clearFullState(); 

        // apply the state on the radio session servant entities
        for ( RadioSessionServantTypeList::const_iterator sessionIt = state.radioSessionServants.begin();
        sessionIt != state.radioSessionServants.end(); ++sessionIt)
        {
            // find the entity with the entity key
            for (std::vector<RadioSessionServant*>::iterator rIt = radioSessionServants.begin();
            rIt != radioSessionServants.end(); rIt++)
            {
                if ( sessionIt->entityKey == (*rIt)->getEntityKey())
                {
                    (*rIt)->applyFullState(*sessionIt);
                    break;
                }
            }
        }

        m_radioTetraAgentState.getRadioServant()->applyFullState(state.radioServant);
	}

	void RadioStateSynchronisationServant::getFullState(RadioTetraAgentType& state)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "RadioServant->getFullState" );
		TA_Base_Core::ThreadGuard guard(m_lockable);

        std::vector<RadioSessionServant*> radioSessionServants = m_radioTetraAgentState.getRadioSessionServants();
        m_radioTetraAgentState.getRadioServant()->getFullState(state.radioServant);

		// get the state of the radio session servant entities
		for (std::vector<RadioSessionServant*>::iterator rIt = radioSessionServants.begin(); 
			  rIt != radioSessionServants.end(); rIt++)
		{
			RadioSessionServantType radioSessionServantData;
			
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "RadioSessionServant->getFullState");
			(*rIt)->getFullState(radioSessionServantData);
			state.radioSessionServants.push_back(radioSessionServantData);			
		}
	}
	//* TD9016

};  // namespace TA_IRS_App
