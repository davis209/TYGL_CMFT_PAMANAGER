/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/RadioMonitorManager.cpp $
  * @author  R. van Hugten
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2008/11/28 16:26:01 $
  * Last modified by : $Author: builder $
  *
  * Implementation of the RadioMonitorManager class
  */
#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "core/types/src/ta_types.h"
#include "app/radio/RadioTetraAgent/src/Radio.h"
#include "app/radio/RadioTetraAgent/src/RadioMonitorManager.h"
#include "app/radio/RadioTetraAgent/src/RadioTaskFactory.h"
#include "app/radio/RadioTetraAgent/src/IRadioCallback.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "app/radio/RadioTetraAgent/src/RadioStateUpdateBroadcaster.h"

// Forward declarations
using namespace TA_IRS_App;


// //////////////////////////////////////////////////////////////
RadioMonitorManager::RadioMonitorManager(Radio & radio, IRadioCallback& callback, RadioBackgroundProcessor * bgProcessor)
: m_radio(radio) 
, m_radioCallback(callback)
{
	FUNCTION_ENTRY("Constructor");
	FUNCTION_EXIT;
}


// //////////////////////////////////////////////////////////////
void RadioMonitorManager::addMonitor(MonitorReference monitorRef)
{
	FUNCTION_ENTRY("addMonitor()");

    TA_Base_Core::ThreadGuard guard(m_monitoredLock);

    //
    // if there is already a monitored activity with the given ref,
    // may as well delete it entirely.
    MonitoredActivityMap::iterator fnd = m_monitoredActivities.find(monitorRef);
    if (fnd != m_monitoredActivities.end())
    {
        MonitoredSubscriberDetails * fndDetails = (*fnd).second;
        delete fndDetails;

        // but NOT erase it from the map, as it will be replaced within a
        // few lines of code anyhow.
        // m_monitoredActivities.erase(fnd);

        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
                "addMonitor() - successfully deleted pre-existing monitor reference %d", monitorRef);
    }

    MonitoredSubscriberDetails * monSubDets = new MonitoredSubscriberDetails;
    monSubDets->monitorRef = monitorRef;

    m_monitoredActivities[monitorRef] = monSubDets;

	sendStateUpdate(monitorRef);


    LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
            "addMonitor() - successfully added monitor reference 0x%X with status \"Monitoring\"", monitorRef);

	FUNCTION_EXIT;
}

// //////////////////////////////////////////////////////////////
void RadioMonitorManager::removeMonitor(MonitorReference monitorRef)
{
	FUNCTION_ENTRY("removeMonitor()");

    TA_Base_Core::ThreadGuard guard(m_monitoredLock);

    MonitoredActivityMap::iterator fnd = m_monitoredActivities.find(monitorRef);
    if (fnd != m_monitoredActivities.end())
    {
        MonitoredSubscriberDetails * fndDetails = (*fnd).second;

        // the list of calls and the structs within that list are all by
        // value, so we shouldn't need to explicitly erase them.
        // Except of course for the newly added SubscriberActivityDetails
        fndDetails->cleanup();

        delete fndDetails;

        m_monitoredActivities.erase(fnd);

        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
                "removeMonitor() - successfully removed monitoring for reference %d", monitorRef);
    }
    else
    {
        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
                "removeMonitor() - could not find monitor reference %d to remove", monitorRef);
    }

	RadioRemoveMonitoredSubscriberDetails message;
	message.monitorReference = monitorRef;
	RadioStateUpdateBroadcaster::getInstance()->sendRadioRemoveMonitoredSubscriberDetails(message);

	FUNCTION_EXIT;
}

// //////////////////////////////////////////////////////////////
bool RadioMonitorManager::isMonitored(MonitorReference monitorRef)
{
	FUNCTION_ENTRY("isMonitored()");
    TA_Base_Core::ThreadGuard guard(m_monitoredLock);

    MonitoredActivityMap::iterator iter = m_monitoredActivities.find(monitorRef);

    // return (iter != m_monitoredActivities.end());
    if (iter != m_monitoredActivities.end())
    {
        FUNCTION_EXIT;
        return true;
    }

    FUNCTION_EXIT;
    // if iter was not the end() then the monitor reference was found and
    // we are indeed monitoring this reference
    return false;
}


// //////////////////////////////////////////////////////////////
MonitorReference RadioMonitorManager::findMonitor(CallID callId)
{
	FUNCTION_ENTRY("findMonitor()");
    TA_Base_Core::ThreadGuard guard(m_monitoredLock);

    MonitorReference monref = 0;

    for (   MonitoredActivityMap::iterator iter = m_monitoredActivities.begin() ;
            iter != m_monitoredActivities.end() ;
            iter++
        )
    {
        MonitoredSubscriberDetails * subscriberDetails = (*iter).second;
        bool mcpFound = false;
        /* MonitoredCallProgression & theMcp = */
			subscriberDetails->findAnyCall(callId, mcpFound);
        if ( mcpFound )
        {
            monref = subscriberDetails->monitorRef;
            break;
        }
    }

    FUNCTION_EXIT;
    return monref;
}


// //////////////////////////////////////////////////////////////
void RadioMonitorManager::subscriberActivity
(   MonitorReference monitorRef,
    CallID callId,
    TA_Base_Bus::IRadioCorbaDef::MonitoredCallDetailsType * monCallDetails
)
{
    char logstr[64];
    sprintf(logstr, "subscriberActivity(monitorRef=%lu, callId=%lu)", monitorRef, callId);
    FUNCTION_ENTRY(logstr);

    TA_Base_Core::ThreadGuard guard(m_monitoredLock);

    MonitoredActivityMap::iterator fnd = m_monitoredActivities.find(monitorRef);
    if (fnd != m_monitoredActivities.end())
    {
        MonitoredSubscriberDetails * fndDetails = (*fnd).second;
        bool mcpFound = false;
        MonitoredCallProgression & theMcp = fndDetails->findAnyCall(callId, mcpFound);

        // First check: is the call already in our records
        if ( ! mcpFound )
        {
            // If not, we probably want to add this subscriber activity.
            //
            // For that, we want the MonitoredCallDetails to be non-null,
            // and the callStatus not to be stopped
            if ( ( monCallDetails != 0 ) && ( monCallDetails->callStatus != TA_Base_Bus::IRadioCorbaDef::Stopped ) )
            {
                MonitoredCallProgression x_mcp;
                x_mcp.monitorStatus = Monitoring,
                x_mcp.monitorCall = callId;
                x_mcp.listenCall = 0;
                x_mcp.joinedCall = 0;
                x_mcp.monitoredCallDetails = monCallDetails;
                fndDetails->monitoredCalls.push_back(x_mcp);
                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
                        "subscriberActivity() - successfully added subscriber activity call=0x%X to monitored reference=0x%X", callId, monitorRef);
            }
            // at this stage, either the call details were null...
            else if (monCallDetails == 0)
            {
                // which is bad, as it means we have been sent notice of a
                // SubscriberActivity that we don't have but which subscriber
                // we do know about.  This is no cause for alarm or assertion
                // however
                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
                        "subscriberActivity() - Encountered unknown call=0x%X for monitored reference=0x%X with no details", callId, monitorRef);
                FUNCTION_EXIT;
                return;
            }
            // or the callStatus was Stopped,
            else
            {
                // in which case we can just send the finished subscriberActivity
                // on up, as the RadioMonitor GUI probably knows what to do with it.
                m_radioCallback.onSubscriberActivity(*monCallDetails);
                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn, 
                        "subscriberActivity() - Encountered 'Stopped' subscriber activity for unknown call=0x%X for monitored reference=0x%X", callId, monitorRef);
				// oh, and delete the call
				// fndDetails->removeCall(callId);
            }

            m_radioCallback.onSubscriberActivity(*monCallDetails);
        }
        else
        {
            // if we are monitoring this call, what we now do depends on the
            // given call details. If they are null, we'll try to get them.
            TA_Base_Bus::IRadioCorbaDef::MonitoredCallDetailsType * useDetails = 0;
            if ( monCallDetails == 0 )
            {
                if ( theMcp.monitoredCallDetails != 0 )
                {
                    useDetails = theMcp.monitoredCallDetails;
                }
                else
                {
                    // with no details given and none found, there isn't a lot
                    // we can do!
                    LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
                            "subscriberActivity() - found monitored call=0x%X for monitored reference=0x%X with no details given or pre-existing", callId, monitorRef);

                    FUNCTION_EXIT;
                    return;
                }
            }
            else // ( monCallDetails != 0 )
            {
                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
                        "subscriberActivity() - found pre-existing subscriber activity for monitored reference=0x%X call=0x%X (using newer details)", monitorRef, callId);

                // out with the old
                //delete theMcp.monitoredCallDetails;
                // in with the new
                theMcp.monitoredCallDetails = monCallDetails;
                // use the new
                useDetails = monCallDetails;
            }

            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
                    "subscriberActivity() - sending new call details of subscriber activity for monitored reference=0x%X for call=0x%X", monitorRef, callId);


            m_radioCallback.onSubscriberActivity(*useDetails);
        }

    }
    else
    {
        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
                "subscriberActivity() - could not find monitor reference=0x%X for subscriber activity", monitorRef);
    }

    FUNCTION_EXIT;
}

// //////////////////////////////////////////////////////////////
bool RadioMonitorManager::isMonitoredCall(CallID callId)
{
	FUNCTION_ENTRY("isMonitoredCall()");

    TA_Base_Core::ThreadGuard guard(m_monitoredLock);

    bool ismoncall = false;

    for (   MonitoredActivityMap::iterator iter = m_monitoredActivities.begin() ;
            iter != m_monitoredActivities.end() ;
            iter++
        )
    {
        MonitoredSubscriberDetails * subscriberDetails = (*iter).second;
        bool mcpFound = false;
        /* MonitoredCallProgression & theMcp = */
            subscriberDetails->findMonitoredCall(callId, mcpFound);
        if ( mcpFound )
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
                    "isMonitoredCall() - found call");
            ismoncall = true;
            break;
        }
    }

    FUNCTION_EXIT;
    return ismoncall;
}


// //////////////////////////////////////////////////////////////
bool RadioMonitorManager::canListenCall(CallID callId)
{
	FUNCTION_ENTRY("canListenCall()");
    FUNCTION_EXIT;
    return isMonitoredCall(callId);
}


// //////////////////////////////////////////////////////////////
bool RadioMonitorManager::startListenCall(CallID monCallId, CallID listenCallId)
{
    char logstr[64];
    sprintf(logstr, "startListenCall(monCallId=%lu, listenCallId=%lu)", monCallId, listenCallId);
    FUNCTION_ENTRY(logstr);

    TA_Base_Core::ThreadGuard guard(m_monitoredLock);

    bool started = false;

    for (   MonitoredActivityMap::iterator iter = m_monitoredActivities.begin() ;
            iter != m_monitoredActivities.end() ;
            iter++
        )
    {
        MonitoredSubscriberDetails * subscriberDetails = (*iter).second;
        bool mcpFound = false;
        MonitoredCallProgression & theMcp = subscriberDetails->findMonitoredCall(listenCallId, mcpFound);
        if ( mcpFound )
        {
            theMcp.listenCall = listenCallId;
            theMcp.monitorStatus = Listening;
            theMcp.monitoredCallDetails->callStatus = TA_Base_Bus::IRadioCorbaDef::Listening;
            started = true;
            subscriberActivity((*iter).first, listenCallId, theMcp.monitoredCallDetails);
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
                    "startListenCall() - listening activated for monitored call=0x%X (listen call=0x%X) monitor reference=0x%X ", monCallId, listenCallId, (*iter).first);
            break;
        }
    }

    FUNCTION_EXIT;
    return started;
}


// //////////////////////////////////////////////////////////////
bool RadioMonitorManager::isListenCall(CallID callId)
{
	FUNCTION_ENTRY("isListenCall()");

    TA_Base_Core::ThreadGuard guard(m_monitoredLock);

    bool listening = false;

    for (   MonitoredActivityMap::iterator iter = m_monitoredActivities.begin() ;
            iter != m_monitoredActivities.end() ;
            iter++
        )
    {
        MonitoredSubscriberDetails * subscriberDetails = (*iter).second;
        bool mcpFound = false;
        /* MonitoredCallProgression & theMcp = */
			subscriberDetails->findListenedCall(callId, mcpFound);
        if ( mcpFound )
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
                    "isListenCall() - found listening call=0x%X for monitor reference=0x%X", callId, (*iter).first);
            listening = true;
            break;
        }
    }

    FUNCTION_EXIT;
    return listening;
}


// //////////////////////////////////////////////////////////////
bool RadioMonitorManager::stopListenCall(CallID callId)
{
    char logstr[44];
    sprintf(logstr, "stopListenCall(%lu)", callId);
    FUNCTION_ENTRY(logstr);

    TA_Base_Core::ThreadGuard guard(m_monitoredLock);

    bool stopped = false;

    for (   MonitoredActivityMap::iterator iter = m_monitoredActivities.begin() ;
            iter != m_monitoredActivities.end() ; 
            iter++
        )
    {
        MonitoredSubscriberDetails * subscriberDetails = (*iter).second;
        bool mcpFound = false;
        MonitoredCallProgression & theMcp = subscriberDetails->findListenedCall(callId, mcpFound);
        if ( mcpFound )
        {
            theMcp.listenCall = 0;
            theMcp.monitorStatus = Monitoring;
            if (theMcp.monitoredCallDetails != 0)
            {
                theMcp.monitoredCallDetails->callStatus = TA_Base_Bus::IRadioCorbaDef::Monitoring;
            }
            stopped = true;
            subscriberActivity((*iter).first, callId, theMcp.monitoredCallDetails);
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
                    "stopListenCall() - stopped listening to call=0x%X for monitor reference=0x%X", callId, (*iter).first);
            break;
        }
    }

    FUNCTION_EXIT;
    return stopped;
}


// //////////////////////////////////////////////////////////////
CallID RadioMonitorManager::getJoinCallId(CallID callId)
{
    char logstr[64];
    sprintf(logstr, "getJoinCallId(callId=%lu)", callId);
    FUNCTION_ENTRY(logstr);

    TA_Base_Core::ThreadGuard guard(m_monitoredLock);

    CallID joinId = 0;

    for (   MonitoredActivityMap::iterator iter = m_monitoredActivities.begin() ;
            iter != m_monitoredActivities.end() ;
            iter++
        )
    {
        MonitoredSubscriberDetails * subscriberDetails = (*iter).second;
        bool mcpFound = false;
        MonitoredCallProgression & theMcp = subscriberDetails->findJoinableCall(callId, mcpFound);
        if ( mcpFound )
        {
            joinId = theMcp.monitorCall;
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
                    "stopListenCall() - joinCall=0x%X found for call=0x%X for monitor reference=0x%X", joinId, callId, (*iter).first);
            break;
        }
    }

    FUNCTION_EXIT;
    return joinId;
}


// //////////////////////////////////////////////////////////////
bool RadioMonitorManager::joinCall(CallID oldCallId, CallID newCallId, bool included)
{
    char logstr[88];
    sprintf(logstr, "joinCall(oldCallId=%lu, newCallId=%lu, included=%d)", oldCallId, newCallId, included);
    FUNCTION_ENTRY(logstr);

    TA_Base_Core::ThreadGuard guard(m_monitoredLock);

    bool joined = false;
    LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
            "joinCall() - searching %d monitored subscribers for call=0x%X",
            m_monitoredActivities.size(), oldCallId);


    for (   MonitoredActivityMap::iterator iter = m_monitoredActivities.begin() ;
            iter != m_monitoredActivities.end() ;
            iter++
        )
    {
        MonitoredSubscriberDetails * subscriberDetails = (*iter).second;
        bool mcpFound = false;
        MonitoredCallProgression & theMcp = subscriberDetails->findAnyCall(oldCallId, mcpFound);
        if ( mcpFound )
        {
            // first, kill the old entry in either CallList in the RadioMonitor
            theMcp.monitoredCallDetails->callStatus = TA_Base_Bus::IRadioCorbaDef::Stopped;
            subscriberActivity((*iter).first, oldCallId, theMcp.monitoredCallDetails);

            theMcp.joinedCall = newCallId;
            theMcp.monitoredCallDetails->callID = newCallId;
            if (included)
            {
                theMcp.monitorStatus = Included;
                theMcp.monitoredCallDetails->callStatus = TA_Base_Bus::IRadioCorbaDef::Included;
            }
            else
            {
                theMcp.monitorStatus = Joined;
                theMcp.monitoredCallDetails->callStatus = TA_Base_Bus::IRadioCorbaDef::Joined;
            }

            joined = true;

            // need to discover new participants
            m_radio.addBackgroundTask(RadioTaskFactory::createSubscriberActivityTask(&(m_radio.getISCSRadioSession().getTcpServer()),m_radioCallback,theMcp.monitoredCallDetails));

            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
                    "joinCall() - %s call=0x%X (listen=0x%X) for monitor reference=0x%X",
                    ((included) ? "Included in" : "Joined"), newCallId, oldCallId, (*iter).first);

            break;
        }
    }

    FUNCTION_EXIT;
    return joined;
}

// //////////////////////////////////////////////////////////////
RadioMonitorManager::MonitoredCallProgression & RadioMonitorManager::MonitoredSubscriberDetails::findAnyCall(CallID callId, bool & found)
{
    found = false;
    for ( MonitoredCallList::iterator callIter = monitoredCalls.begin() ;
          callIter != monitoredCalls.end() ;
          callIter++
        )
    {
        MonitoredCallProgression & x_mcp = *callIter;
        if ( (x_mcp.monitorCall == callId) || (x_mcp.listenCall == callId) || (x_mcp.joinedCall == callId) )
        {
            found = true;
            return *callIter;
        }
    }

    return NullMonitoredCallProgression;
}

// //////////////////////////////////////////////////////////////
RadioMonitorManager::MonitoredCallProgression & RadioMonitorManager::MonitoredSubscriberDetails::findMonitoredCall(CallID callId, bool & found)
{
    found = false;
    for ( MonitoredCallList::iterator callIter = monitoredCalls.begin() ;
          callIter != monitoredCalls.end() ;
          callIter++
        )
    {
        MonitoredCallProgression & x_mcp = *callIter;
        if ( (x_mcp.monitorCall == callId) && (x_mcp.monitorStatus == Monitoring) )
        {
            found = true;
            return *callIter;
        }
    }

    return NullMonitoredCallProgression;
}

// //////////////////////////////////////////////////////////////
RadioMonitorManager::MonitoredCallProgression & RadioMonitorManager::MonitoredSubscriberDetails::findListenedCall(CallID callId, bool & found)
{
    found = false;
    for ( MonitoredCallList::iterator callIter = monitoredCalls.begin() ;
          callIter != monitoredCalls.end() ;
          callIter++
        )
    {
        MonitoredCallProgression & x_mcp = *callIter;
        if ( ( (x_mcp.listenCall == callId) || (x_mcp.monitorCall == callId) )
                && (x_mcp.monitorStatus == Listening) )
        {
            found = true;
            return *callIter;
        }
    }

    return NullMonitoredCallProgression;
}

// //////////////////////////////////////////////////////////////
RadioMonitorManager::MonitoredCallProgression & RadioMonitorManager::MonitoredSubscriberDetails::findJoinableCall(CallID callId, bool & found)
{
    found = false;
    for ( MonitoredCallList::iterator callIter = monitoredCalls.begin() ;
          callIter != monitoredCalls.end() ;
          callIter++
        )
    {
        MonitoredCallProgression & x_mcp = *callIter;
        if ( ( (x_mcp.listenCall == callId) || (x_mcp.monitorCall == callId) )
          && ( (x_mcp.monitorStatus == Listening) || (x_mcp.monitorStatus == Monitoring) )
           )
        {
            found = true;
            return *callIter;
        }
    }

    return NullMonitoredCallProgression;
}

// //////////////////////////////////////////////////////////////
RadioMonitorManager::MonitoredCallProgression & RadioMonitorManager::MonitoredSubscriberDetails::findJoinedCall(CallID callId, bool & found)
{
    found = false;
    for ( MonitoredCallList::iterator callIter = monitoredCalls.begin() ;
          callIter != monitoredCalls.end() ;
          callIter++
        )
    {
        MonitoredCallProgression & x_mcp = *callIter;
        if ( (x_mcp.joinedCall == callId)
             && ( (x_mcp.monitorStatus == Included) || ( x_mcp.monitorStatus == Joined ) )
           )
        {
            found = true;
            return *callIter;
        }
    }

    return NullMonitoredCallProgression;
}

bool RadioMonitorManager::MonitoredSubscriberDetails::removeCall(CallID callId)
{
    bool found = false;
    for ( MonitoredCallList::iterator callIter = monitoredCalls.begin() ;
          callIter != monitoredCalls.end() ;
          callIter++
        )
    {
        MonitoredCallProgression & x_mcp = *callIter;
        if ( (x_mcp.monitorCall == callId) || (x_mcp.listenCall == callId) || (x_mcp.joinedCall == callId) )
        {
            found = true;
            monitoredCalls.erase(callIter);
			break;
        }
    }

    return found;
}

// //////////////////////////////////////////////////////////////
void RadioMonitorManager::MonitoredSubscriberDetails::cleanup()
{
    for ( MonitoredCallList::iterator callIter = monitoredCalls.begin() ;
          callIter != monitoredCalls.end() ;
          callIter++
        )
    {
        MonitoredCallProgression & x_mcp = *callIter;

        delete x_mcp.monitoredCallDetails;
        x_mcp.monitoredCallDetails = 0;
    }
}

RadioMonitorManager::MonitoredCallProgression RadioMonitorManager::NullMonitoredCallProgression = {Stopped,0,0,0,0};



void RadioMonitorManager::applyFullState(const RadioMonitorManagerType& state)
{
	FUNCTION_ENTRY("applyFullState");
	
	MonitoredSubscriberDetailsTypeMap::const_iterator it;
	
    {
        TA_Base_Core::ThreadGuard guard(m_monitoredLock);
		
		 for (MonitoredActivityMap::iterator iter = m_monitoredActivities.begin();iter != m_monitoredActivities.end();iter++)
		{
		    MonitoredSubscriberDetails * pfndDetails = (*iter).second;
		    pfndDetails->cleanup();
		    delete pfndDetails;
			pfndDetails = NULL;

		    m_monitoredActivities.erase(iter--);
		}

		for (it = state.monitoredSubscriberDetailsMap.begin(); it != state.monitoredSubscriberDetailsMap.end(); it++)
        {
	
			MonitoredSubscriberDetails*  monitoredSubscriberDetails = new MonitoredSubscriberDetails;
			monitoredSubscriberDetails->monitorRef = (it->second).monitorReference;
			m_monitoredActivities.insert(std::pair< ta_int32 , MonitoredSubscriberDetails*>(it->first,monitoredSubscriberDetails) );
		}
		
	}

	FUNCTION_EXIT;
}

void RadioMonitorManager::clearFullState()
{
	FUNCTION_ENTRY("clearFullState");
	
	{
		TA_Base_Core::ThreadGuard guard(m_monitoredLock);
	
		for (MonitoredActivityMap::iterator iter = m_monitoredActivities.begin();iter != m_monitoredActivities.end();iter++)
		{
		    MonitoredSubscriberDetails * pfndDetails = (*iter).second;
		    pfndDetails->cleanup();
		    delete pfndDetails;
			pfndDetails = NULL;

		    m_monitoredActivities.erase(iter--);
		}
	}

	FUNCTION_EXIT;
}

void RadioMonitorManager::getFullState(RadioMonitorManagerType& state)
{
	FUNCTION_ENTRY("getFullState");
	
	MonitoredActivityMap::const_iterator it;
	
	{
		TA_Base_Core::ThreadGuard guard(m_monitoredLock);
		
		// get all the calls from the stack
		state.monitoredSubscriberDetailsMap.clear();
	
		for (it = m_monitoredActivities.begin(); it != m_monitoredActivities.end(); it++)
		{
			MonitoredSubscriberDetailsType monitoredSubscriberDetailsType;
			monitoredSubscriberDetailsType.monitorReference = (it->second)->monitorRef;
			state.monitoredSubscriberDetailsMap.insert(std::pair< ta_int32 , MonitoredSubscriberDetailsType>(it->first,monitoredSubscriberDetailsType) );

		}
			
	}

	FUNCTION_EXIT;
}


void RadioMonitorManager::sendStateUpdate(MonitorReference	&monitorRef)
{

	RadioUpdateMonitoredCallProgression message;
	message.monitorReference = monitorRef;
	
	RadioStateUpdateBroadcaster::getInstance()->sendRadioUpdateMonitoredCallProgression(message);
}


