/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $Source: /cvs/1TS-9999-T/code/transactive/app/radio/RadioTetraAgent/src/Attic/RadioSubscriberMonitor.cpp,v $
  * @author  R. van Hugten
  * @version $Revision: 1.1.2.5 $
  * Last modification : $Date: 2006/12/07 18:24:02 $
  * Last modified by : $Author: robertvh $
  *
  * Implementation of the RadioSubscriberMonitor class
  */
#ifdef __WIN32__
#pragma warning(disable:4503)
#pragma warning(disable:4786)
#endif

#include "core/types/src/ta_types.h"
#include "app/radio/RadioTetraAgent/src/RadioSession.h"
#include "app/radio/RadioTetraAgent/src/RadioTcpServer.h"
#include "app/radio/RadioTetraAgent/src/IRadioCallback.h"
//#include "app/radio/RadioTetraAgent/src/RadioTaskFactory.h"
#include "app/radio/RadioTetraAgent/src/RadioSubscriberMonitor.h"
#include "app/radio/RadioTetraAgent/src/RadioStateUpdateBroadcaster.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

// Forward declarations
using namespace TA_IRS_App;
using TA_Base_Core::DebugUtil;

// //////////////////////////////////////////////////////////////
RadioSubscriberMonitor::RadioSubscriberMonitor(RadioTcpServer& tcpServer)
: m_tcpServer(tcpServer) 
{
	FUNCTION_ENTRY("Constructor");
	m_isInControl = false ;
	FUNCTION_EXIT;
}



// //////////////////////////////////////////////////////////////
void RadioSubscriberMonitor::addMonitor(MonitorReference monitorRef, SessionReference sessionRef, const char * subscriberTsi)
{
	FUNCTION_ENTRY("addMonitor()");

    TA_Base_Core::ThreadGuard guard(m_monitoredLock);

    //
    // if there is already a monitored activity with the given ref, may as well
    // delete it entirely. It is probably an undesired remnant
    MonitoredSubscriberMap::iterator fnd = m_monitoredSubscribers.find(monitorRef);
    if (fnd != m_monitoredSubscribers.end())
    {
        MonitoredSubscriberDetails * fndDetails = (*fnd).second;
        fndDetails->cleanup();
        delete fndDetails;

        // but NOT erase it from the map, as it will be replaced within a
        // few lines of code anyhow.
        // m_monitoredSubscribers.erase(fnd);

        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
                "addMonitor() - deleted pre-existing monitor reference %lu", monitorRef);
    }

    MonitoredSubscriberDetails * monSubDets = new MonitoredSubscriberDetails;
    monSubDets->monitorRef = monitorRef;
    monSubDets->subscriberTsi = subscriberTsi;
	monSubDets->sessionRef = sessionRef;

    m_monitoredSubscribers[monitorRef] = monSubDets;

    //
    // State Update
	sendStateUpdate(monitorRef);


    LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
            "addMonitor() - successfully added monitor reference %lu with status \"Monitoring\"", monitorRef);

	FUNCTION_EXIT;
}

// //////////////////////////////////////////////////////////////
void RadioSubscriberMonitor::removeMonitor(const char * subscriberTsi)
{
	FUNCTION_ENTRY("removeMonitor(const char *)");
    TA_Base_Core::ThreadGuard guard(m_monitoredLock);

    MonitorReference monRef = findMonitor(subscriberTsi);
    if ( 0 != monRef )
    {
        removeMonitor(monRef);
    }
    else
    {
        LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "Could not find Monitor Reference corresponding to %s to remove", subscriberTsi);
    }

    FUNCTION_EXIT;
}


// //////////////////////////////////////////////////////////////
void RadioSubscriberMonitor::removeMonitor(MonitorReference monitorRef)
{
	FUNCTION_ENTRY("removeMonitor(MonitorReference)");

    TA_Base_Core::ThreadGuard guard(m_monitoredLock);

    MonitoredSubscriberMap::iterator fnd = m_monitoredSubscribers.find(monitorRef);
    if (fnd != m_monitoredSubscribers.end())
    {
        // deallocate pointer contents first. All structs within will destroy
        // their own contents as required.
        MonitoredSubscriberDetails * fndDetails = (*fnd).second;
        fndDetails->cleanup();
        delete fndDetails;

        m_monitoredSubscribers.erase(fnd);

        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "removeMonitor() - successfully removed Monitor %lu", monitorRef);
    }
    else
    {
        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "removeMonitor() - could not find Monitor %lu to remove", monitorRef);
    }

    //
    // State Update
	if ( isInControl () ) {
		RadioRemoveMonitoredSubscriberDetails message;
		message.monitorReference = monitorRef;
		message.serverIndex = m_tcpServer.getServerIndex();
		RadioStateUpdateBroadcaster::getInstance()->sendRadioRemoveMonitoredSubscriberDetails(message);
	}

	FUNCTION_EXIT;
}

// //////////////////////////////////////////////////////////////
bool RadioSubscriberMonitor::isMonitored(MonitorReference monitorRef)
{
	FUNCTION_ENTRY("isMonitored()");
    TA_Base_Core::ThreadGuard guard(m_monitoredLock);

    MonitoredSubscriberMap::iterator iter = m_monitoredSubscribers.find(monitorRef);

    if (iter != m_monitoredSubscribers.end())
    {
        // if iter was not the end() then the monitor reference was found and
        // we are indeed monitoring this reference
        FUNCTION_EXIT;
        return true;
    }

    FUNCTION_EXIT;
    return false;
}

// //////////////////////////////////////////////////////////////
MonitorReference RadioSubscriberMonitor::findMonitor(const char * subscriberTsi)
{
	FUNCTION_ENTRY("findMonitor(const char *)");
    TA_Base_Core::ThreadGuard guard(m_monitoredLock);

    MonitorReference monref = 0;

    for (   MonitoredSubscriberMap::iterator iter = m_monitoredSubscribers.begin() ;
            iter != m_monitoredSubscribers.end() ;
            iter++
        )
    {
        MonitoredSubscriberDetails * subscriberDetails = (*iter).second;
        if (subscriberDetails->subscriberTsi == subscriberTsi)
        {
            monref = subscriberDetails->monitorRef;
            break;
        }
    }

    FUNCTION_EXIT;
    return monref;
}

// //////////////////////////////////////////////////////////////
MonitorReference RadioSubscriberMonitor::findMonitor(CallID callId)
{
	FUNCTION_ENTRY("findMonitor(CallID)");
    TA_Base_Core::ThreadGuard guard(m_monitoredLock);

    MonitorReference monref = 0;

    for (   MonitoredSubscriberMap::iterator iter = m_monitoredSubscribers.begin() ;
            iter != m_monitoredSubscribers.end() ;
            iter++
        )
    {
        MonitoredSubscriberDetails * subscriberDetails = (*iter).second;
        bool call_was_found = false;
        /* MonitoredCallInformation & mon_call_info = */
			subscriberDetails->findAnyCall(callId, call_was_found);
        if ( call_was_found )
        {
            monref = subscriberDetails->monitorRef;
            break;
        }
    }

    FUNCTION_EXIT;
    return monref;
}


// //////////////////////////////////////////////////////////////
// This function is called  from RadioSubscriberActivityTask::perform() when this
// class decided that the callId was pre-existing for the given monitorRef.
//
// Also called by the other overloaded subscriberActivity if the call was found
// and its monCallDetails was NULL
//
void RadioSubscriberMonitor::subscriberActivity(MonitorReference monitorRef, CallID callId, bool isCallStart)
{
    FUNCTION_ENTRY("subscriberActivity(MonitoredReference, CallID)");
    LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
                "SubscriberActivity for Monitor %lu Call %lu (%s)",
                monitorRef, callId, (isCallStart?"CALL_START":"CALL_END"));

    TA_Base_Core::ThreadGuard guard(m_monitoredLock);

    MonitoredSubscriberMap::iterator fnd = m_monitoredSubscribers.find(monitorRef);
    if (fnd != m_monitoredSubscribers.end())
    {
        MonitoredSubscriberDetails * fndDetails = (*fnd).second;
        bool call_was_found = false;
        MonitoredCallInformation & mon_call_info = fndDetails->findAnyCall(callId, call_was_found);

        // First check: is the call already in our records
        if ( call_was_found )
        {
            // Just what we expected. Change the monitorStatus, and remove the
            // call if required
            //
            if (PendingInclude != mon_call_info.monitorStatus)
            {
                if (isCallStart)
                {
                    // now we have our joined call info
                    mon_call_info.monitorStatus = Joined;
                    mon_call_info.joinedCall = callId;
                }
                else
                {
                    // ended before it began ? Or maybe while we were still
                    // gathering call details ?
                    mon_call_info.monitorStatus = Stopped;
                }
            }
            else
            {
                mon_call_info.monitorStatus = (isCallStart ? Monitoring : Stopped);
                mon_call_info.monitorCall = callId;
            }

            if (0 == mon_call_info.monitoredCallDetails)
            {
                // this is a bit embarrassing - we have a call info record but
                // not yet populated the details for it... Should we spawn a
                // SubscriberActivityTask to go fetch them ?
                // 
                // This doesn't seem necessary - if we don't have details
                // then there's probably no point telling the gui, as we won't
                // have provided it with anything that its displaying.
                // 
                // Not much to do besides log the problem
                LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Details neither provided nor pre-existing for Call %lu Monitor %lu", callId, monitorRef);
            }
            else
            {
                mon_call_info.monitoredCallDetails->monitorStatus = mon_call_info.monitorStatus;
                LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Found Call %lu for Monitor %lu details to send", callId, monitorRef);

                notifySubscriberActivity(mon_call_info.monitoredCallDetails);
            }

            // in either case, if the call was at its end, we can remove our
            // record of it
            if ( !isCallStart)
            {
                // DELETE traces of this call
                fndDetails->removeCall(callId);
            }
        }
        else
        {
            // specified call not found for the given monitor, even though we
            // must have decided it was there at one stage in the recent past.
            LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Call %lu not found in Monitor %lu", callId, monitorRef);
        }

    }
    else
    {
        //
        // This is actually quite tragic - it is this class that decided that
        // there WAS not just a pre-existing Monitor of this id, but a call also.
        // Where this could have gone wrong will hopefully be shown clearly in
        // the logs
        LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Could not find Monitor %lu for SubscriberActivity", monitorRef);
    }

    FUNCTION_EXIT;
}


// //////////////////////////////////////////////////////////////
// this function is called under only 1 circumstance, from
// RadioSubscriberActivityTask::perform() if this class decided that the callId
// was NOT pre-existing for the given monitorRef. In this case, the monCallDetails
// will have been populated correctly with all the appropriate fields
void RadioSubscriberMonitor::subscriberActivity(MonitoredCallDetailsType * monCallDetails)
{
    FUNCTION_ENTRY("subscriberActivity(MonitoredCallDetailsType *)");
    
    TA_ASSERT( (monCallDetails != NULL) , "MonitoredCallDetails was NULL");

    MonitorReference monitor_ref = monCallDetails->monitorID;
    CallID call_id = monCallDetails->callID;

    LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "SubscriberActivity for MonitoredCallDetails %p (Monitor %lu Call %lu)", monCallDetails, monitor_ref, call_id);

    TA_Base_Core::ThreadGuard guard(m_monitoredLock);

    MonitoredSubscriberMap::iterator fnd = m_monitoredSubscribers.find(monitor_ref);
    if (fnd != m_monitoredSubscribers.end())
    {
        MonitoredSubscriberDetails * fndDetails = (*fnd).second;
        bool call_was_found = false;
        MonitoredCallInformation & mon_call_info = fndDetails->findAnyCall(call_id, call_was_found);

        // First check: is the call already in our records
        if ( ! call_was_found )
        {
            // This is what we expected, we want to add this subscriber activity.
            //
            // For that, we want the MonitoredCallDetails to be non-null,
            // and the callStatus not to be stopped
            if ( monCallDetails != 0 )
            {
                MonitoredCallInformation x_mon_call_info;
                x_mon_call_info.monitorStatus = Monitoring,
                x_mon_call_info.monitorCall = call_id;
                x_mon_call_info.listenCall = 0;
                x_mon_call_info.joinedCall = 0;
                x_mon_call_info.monitoredCallDetails = monCallDetails;
                fndDetails->monitoredCalls.push_back(x_mon_call_info);
                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Successfully added SubscriberActivity Call %lu to Monitor %lu", monCallDetails->callID, monCallDetails->monitorID);
            }
            else
            {
                // no details given, none found => BAD
                LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Details neither provided nor pre-existing for Call %lu Monitor %lu", call_id, monitor_ref);
                FUNCTION_EXIT;
                return;
            }

            notifySubscriberActivity(monCallDetails);
        }
        else
        {
            // we are monitoring this call already, but have received more details.
            // In theory, by design, this circumstance will only occur if a call
            // that has been monitored or listened was ended we are now receiving
            // a Call Start for the Joined/Included call
            // HOWEVER even if this isn't the case, and the monCallDetails is
            // non-NULL, this should be handled by unconditionally replacing
            // the old details.
            //
            if ( monCallDetails != 0 )
            {
                // Just what we expected. Change the monitorStatus, and remove the
                // call if required
                //
                if (PendingInclude != mon_call_info.monitorStatus)
                {
                    // now we have our joined call info
                    mon_call_info.monitorStatus = Joined;
                    mon_call_info.joinedCall = call_id;
                }
                else
                {
                    mon_call_info.monitorStatus = monCallDetails->monitorStatus;
                    mon_call_info.monitorCall = call_id;
                }

                // here is where we unconditionally replace the old details
                delete mon_call_info.monitoredCallDetails;
                mon_call_info.monitoredCallDetails = monCallDetails;

                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Successfully modified details for SubscriberActivity Call %lu Monitor %lu", call_id, monitor_ref);

                notifySubscriberActivity(monCallDetails);
            }
            else
            {
                // no details given. In this case, it is very difficult to know
                // what to do, because we have no idea if the subscriber activity
                // was for a call start or a call end. We assume that we only get
                // either a call start and end from the radio system, so that if
                // we have the details then we already have the start. So end it.
                LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Details neither provided nor pre-existing for Call %lu Monitor %lu", call_id, monitor_ref);

                subscriberActivity(monitor_ref, call_id, false);
            }
        }

    }
    else
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Could not find Monitor %lu for SubscriberActivity", monitor_ref);
    }

    FUNCTION_EXIT;
}

// a bit like findMonitor but searches through all monitored subscribers to match this one
bool RadioSubscriberMonitor::isKnownCall(MonitorReference monitorRef, CallID callId)
{
	FUNCTION_ENTRY("isKnownCall()");
    bool is_known_call = false;

    { // ThreadGuard scope restriction
        TA_Base_Core::ThreadGuard guard(m_monitoredLock);
        MonitoredSubscriberMap::iterator iter = m_monitoredSubscribers.find(monitorRef);

        MonitoredSubscriberDetails * subscriberDetails = (*iter).second;
        
        /* MonitoredCallInformation & mon_call_info = */
        subscriberDetails->findAnyCall(callId, is_known_call);
    }

    FUNCTION_EXIT;
    return is_known_call;
}

// //////////////////////////////////////////////////////////////
bool RadioSubscriberMonitor::isMonitoredCall(CallID callId)
{
	FUNCTION_ENTRY("isMonitoredCall()");

    TA_Base_Core::ThreadGuard guard(m_monitoredLock);

    bool ismoncall = false;

    for (   MonitoredSubscriberMap::iterator iter = m_monitoredSubscribers.begin() ;
            iter != m_monitoredSubscribers.end() ;
            iter++
        )
    {
        MonitoredSubscriberDetails * subscriberDetails = (*iter).second;
        bool call_was_found = false;
        /* MonitoredCallInformation & mon_call_info = */
            subscriberDetails->findMonitoredCall(callId, call_was_found);
        if ( call_was_found )
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
bool RadioSubscriberMonitor::canListenCall(CallID callId)
{
	FUNCTION_ENTRY("canListenCall()");
    FUNCTION_EXIT;
    return isMonitoredCall(callId);
}


// //////////////////////////////////////////////////////////////
bool RadioSubscriberMonitor::startedListenCall(CallID monCallId, CallID listenCallId)
{
    LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "startListenCall(monCallId=%lu, listenCallId=%lu)", monCallId, listenCallId);
    FUNCTION_ENTRY("startedListenCall");

    TA_Base_Core::ThreadGuard guard(m_monitoredLock);

    bool started = false;

    for (   MonitoredSubscriberMap::iterator iter = m_monitoredSubscribers.begin() ;
            iter != m_monitoredSubscribers.end() ;
            iter++
        )
    {
        MonitoredSubscriberDetails * subscriberDetails = (*iter).second;
        bool call_was_found = false;
        MonitoredCallInformation & mon_call_info = subscriberDetails->findMonitoredCall(listenCallId, call_was_found);
        if ( call_was_found )
        {
            mon_call_info.listenCall = listenCallId;
            mon_call_info.monitoredCallDetails->monitorStatus = mon_call_info.monitorStatus = Listening;
            started = true;

            notifySubscriberActivity(mon_call_info.monitoredCallDetails);

            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, 
                        "Listening activated for Monitor %lu Call %lu (Listen Call %lu)",
                        (*iter).first, monCallId, listenCallId );
            // don't break - we want to modify all the monitored subscribers
            // involved in the call
            // break;
        }
    }

    FUNCTION_EXIT;
    return started;
}


// //////////////////////////////////////////////////////////////
bool RadioSubscriberMonitor::isListenCall(CallID callId)
{
	FUNCTION_ENTRY("isListenCall()");

    TA_Base_Core::ThreadGuard guard(m_monitoredLock);

    bool listening = false;

    for (   MonitoredSubscriberMap::iterator iter = m_monitoredSubscribers.begin() ;
            iter != m_monitoredSubscribers.end() ;
            iter++
        )
    {
        MonitoredSubscriberDetails * subscriberDetails = (*iter).second;
        bool call_was_found = false;
        /* MonitoredCallInformation & mon_call_info = */
			subscriberDetails->findListenedCall(callId, call_was_found);
        if ( call_was_found )
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
                    "isListenCall() - found listening Call %lu for Monitor %lu", callId, (*iter).first);
            listening = true;
            break;
        }
    }

    FUNCTION_EXIT;
    return listening;
}


// //////////////////////////////////////////////////////////////
bool RadioSubscriberMonitor::stoppedListenCall(CallID callId)
{
    char logstr[44] = {0};
    sprintf(logstr, "stopListenCall(%lu)", callId);
    FUNCTION_ENTRY(logstr);

    TA_Base_Core::ThreadGuard guard(m_monitoredLock);

    bool stopped = false;

    for (   MonitoredSubscriberMap::iterator iter = m_monitoredSubscribers.begin() ;
            iter != m_monitoredSubscribers.end() ; 
            iter++
        )
    {
        MonitoredSubscriberDetails * subscriberDetails = (*iter).second;
        bool call_was_found = false;
        MonitoredCallInformation & mon_call_info = subscriberDetails->findListenedCall(callId, call_was_found);
        if ( call_was_found )
        {
            mon_call_info.listenCall = 0;
            mon_call_info.monitorStatus = Monitoring;
            if (mon_call_info.monitoredCallDetails != 0)
            {
                mon_call_info.monitoredCallDetails->monitorStatus = Monitoring;
            }
            stopped = true;
            notifySubscriberActivity(mon_call_info.monitoredCallDetails);
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
                    "Stopped listening to Call %lu for Monitor %lu", callId, (*iter).first);
            // don't break - we want to modify all the monitored subscribers
            // involved in the call
            // break;
        }
    }

    FUNCTION_EXIT;
    return stopped;
}


// //////////////////////////////////////////////////////////////
CallID RadioSubscriberMonitor::getJoinCallId(CallID callId)
{
    FUNCTION_ENTRY("getJoinCallId");

    TA_Base_Core::ThreadGuard guard(m_monitoredLock);

    CallID joinId = 0;

    bool call_was_found = false;
    for (   MonitoredSubscriberMap::iterator iter = m_monitoredSubscribers.begin() ;
            iter != m_monitoredSubscribers.end() ;
            iter++
        )
    {
        MonitoredSubscriberDetails * subscriberDetails = (*iter).second;
        MonitoredCallInformation & mon_call_info = subscriberDetails->findJoinableCall(callId, call_was_found);
        if ( call_was_found )
        {
            // whichever way this is done, the monitorCall reference seems to
            // be the valid one to use to join/include
            joinId = mon_call_info.monitorCall;
            LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "JoinCall %lu found for Call %lu for Monitor %lu", joinId, callId, (*iter).first);
            break;
        }
    }

    if ( !call_was_found )
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, 
            "Call %lu NOT found for to retrieve Joinable call ref", callId);
    }


    FUNCTION_EXIT;
    return joinId;
}

bool RadioSubscriberMonitor::designateAsPendingInclude(CallID callId)
{
    FUNCTION_ENTRY("designateAsPendingInclude");
    TA_Base_Core::ThreadGuard guard(m_monitoredLock);

    CallID joinId = 0;
    ta_int32 calls_found = 0;

    for (   MonitoredSubscriberMap::iterator iter = m_monitoredSubscribers.begin() ;
            iter != m_monitoredSubscribers.end() ;
            iter++
        )
    {
        MonitoredSubscriberDetails * subscriberDetails = (*iter).second;
        bool call_was_found = false;
        MonitoredCallInformation & mon_call_info = subscriberDetails->findJoinableCall(callId, call_was_found);
        if ( call_was_found )
        {
            calls_found++;
            mon_call_info.monitorStatus = PendingInclude;
            if (0 != mon_call_info.monitoredCallDetails)
            {
                mon_call_info.monitoredCallDetails->monitorStatus = PendingInclude;
            }
            LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, 
                        "Found Call %lu Monitor %lu now designated as pending Join",
                        callId, (*iter).first);
            // don't break - we want to designate all the monitored subscribers
            // involved in the call
            // break;
        }
    }

    if ( 0 == calls_found )
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Call %lu NOT found for to retrieve joinable call ref", callId);
    }

    FUNCTION_EXIT;
    return (0<calls_found);
}

bool RadioSubscriberMonitor::joinedCall(CallID oldCallId, CallID newCallId, bool included)
{
    FUNCTION_ENTRY("joinedCall");

    LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, 
                "joinedCall(oldCallId=%lu, newCallId=%lu, included=%s)",
                oldCallId, newCallId, (included?"Yes":"No") );


    TA_Base_Core::ThreadGuard guard(m_monitoredLock);

    bool joined = false;

    for (   MonitoredSubscriberMap::iterator iter = m_monitoredSubscribers.begin() ;
            iter != m_monitoredSubscribers.end() ;
            iter++
        )
    {
        MonitoredSubscriberDetails * subscriberDetails = (*iter).second;
        bool call_was_found = false;
        MonitoredCallInformation & mon_call_info = subscriberDetails->findAnyCall(oldCallId, call_was_found);
        if ( call_was_found )
        {
            // 2 subsequent subscriberactivity events will remove the old call
            // and cause a flurry for the new call

            // now update the details in anticipation of the new call
            // This might take a while, possibly should notifySubscriberActivity after doing this - 
            // however then we'd have to upda
            mon_call_info.joinedCall = newCallId;
            mon_call_info.monitoredCallDetails->callID = newCallId;
            // If is Included, it needs to remain as PendingInclude until we get
            // the CALL_START subscriber activity event for the new group call
            if (included)
            {
                mon_call_info.monitoredCallDetails->monitorStatus = mon_call_info.monitorStatus = PendingInclude;
            }
            else
            {
                mon_call_info.monitoredCallDetails->monitorStatus = mon_call_info.monitorStatus = Joined;
            }

            joined = true;

            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, 
                        "%s Call %lu (Listen Call %lu) for Monitor %lu",
                        ((included) ? "Included in" : "Joined"), newCallId, oldCallId, (*iter).first);

            // don't break - we want to modify all the monitored subscribers
            // involved in the call
            // break;
        }
    }

    FUNCTION_EXIT;
    return joined;
}


EMonitorStatusType RadioSubscriberMonitor::getCallMonitorStatus(MonitorReference monitorRef, CallID callId)
{
    FUNCTION_ENTRY("getCallMonitorStatus");

    LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Searching for Monitor %lu Call %lu to retrieve MonitorStatus", monitorRef, callId);

    TA_Base_Core::ThreadGuard guard(m_monitoredLock);

    EMonitorStatusType monitor_status = NotMonitored;
    MonitoredSubscriberMap::iterator fnd = m_monitoredSubscribers.find(monitorRef);
    if (fnd != m_monitoredSubscribers.end())
    {

        MonitoredSubscriberDetails * fndDetails = (*fnd).second;
        monitor_status = fndDetails->getCallMonitorStatus(callId);

        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Monitor %lu Call %lu monitor status %d", monitorRef, callId, (int)monitor_status);
    }
    else
    {
        LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "Could not find Monitor %lu to get Call %lu monitor status", monitorRef, callId);
    }

    FUNCTION_EXIT;
    return monitor_status;
}

ECallSource RadioSubscriberMonitor::getCallSource(MonitorReference monitorRef, CallID callId)
{
	FUNCTION_ENTRY("getCallSource");
	ECallSource callSource;

	TA_Base_Core::ThreadGuard guard(m_monitoredLock);

	MonitoredSubscriberMap::iterator fnd = m_monitoredSubscribers.find(monitorRef);
	if (fnd != m_monitoredSubscribers.end())
	{

		MonitoredSubscriberDetails * fndDetails = (*fnd).second;

		bool bFound = false;
		MonitoredCallInformation& info = fndDetails->findMonitoredCall(callId, bFound);
		if (bFound)
			callSource = info.monitoredCallDetails->callSource;

		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Monitor %lu Call %lu source %d", monitorRef, callId, (int)callSource);
	}
	else
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Could not find Monitor %lu to get Call %lu source", monitorRef, callId);
	}

	FUNCTION_EXIT;
	return callSource;
}

MonitoredSubscriberList RadioSubscriberMonitor::getMonitoredSubscribers(SessionReference sessionRef)
{
	TA_THREADGUARD(m_monitoredLock);
	FUNCTION_ENTRY("getMonitoredSubscribers");

	MonitoredSubscriberList retList;
	MonitoredSubscriberMap::iterator itr = m_monitoredSubscribers.begin();
	for (itr; itr != m_monitoredSubscribers.end(); itr++)
	{
		if (itr->second->sessionRef == sessionRef)
		{
			MonitoredSubscriber monSubscriber;
			monSubscriber.monitorRef = itr->second->monitorRef;
			monSubscriber.sessionRef = itr->second->sessionRef;
			monSubscriber.subscriberTsi = itr->second->subscriberTsi;

			MonitoredCallList::iterator callItr = itr->second->monitoredCalls.begin();

			for (callItr; callItr != itr->second->monitoredCalls.end(); ++callItr)
			{
				MonitoredCallInfo callInfo;
				callInfo.monitorStatus = callItr->monitorStatus;
				callInfo.monitorCall = callItr->monitorCall;
				callInfo.listenCall = callItr->listenCall;
				callInfo.joinedCall = callItr->joinedCall;

				monSubscriber.monCallInfos.push_back(callInfo);
			}

			retList.push_back(monSubscriber);
		}
	}

	FUNCTION_EXIT;
	return retList;
}

void RadioSubscriberMonitor::removeMonitoringbySessionRef(SessionReference sessionRef)
{
	FUNCTION_ENTRY("removeMonitoringbySessionRef");
	TA_THREADGUARD(m_monitoredLock);
	
	LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Remove subscribers Monitoring for session reference %d", sessionRef);
	MonitoredSubscriberMap::iterator itr = m_monitoredSubscribers.begin();
	for(itr; itr!= m_monitoredSubscribers.end(); )
	{
		MonitoredSubscriberDetails * pTemp = itr->second;
		if (pTemp->sessionRef == sessionRef)
		{	// Remove subscriber monitoring due session issue
			LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Remove monitored subscriber with TSI = %s and Monitor ID = %d", pTemp->subscriberTsi.c_str(), pTemp->monitorRef);
			pTemp->cleanup();
			

			// State Update
			if (isInControl()) {
				RadioRemoveMonitoredSubscriberDetails message;
				message.monitorReference = pTemp->monitorRef;
				message.serverIndex = m_tcpServer.getServerIndex();
				RadioStateUpdateBroadcaster::getInstance()->sendRadioRemoveMonitoredSubscriberDetails(message);
			}

			delete pTemp;
			pTemp = 0;

			m_monitoredSubscribers.erase(itr++);
		}
		else
			++itr;
	}

	FUNCTION_EXIT;
}

void RadioSubscriberMonitor::notifySubscriberActivity(MonitoredCallDetailsType * monCallDetails)
{
    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "notifySubscriberActivity temporary watchpoint for Call %lu Monitor %lu", monCallDetails->callID, monCallDetails->monitorID);

    if ( (PendingInclude != monCallDetails->monitorStatus) && (NotMonitored != monCallDetails->monitorStatus) )
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, 
                    "Sending details of subscriber activity (%p) for Monitor %lu for Call %lu",
                    monCallDetails, monCallDetails->monitorID, monCallDetails->callID);
        m_tcpServer.getRadioCallback().onSubscriberActivity(monCallDetails);

    }
    else
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, 
                    "Subscriber activity for Monitor %lu Call %lu is monitorStatus %d, NOT sending",
                    monCallDetails->monitorID, monCallDetails->callID, monCallDetails->monitorStatus);
    }
}






// ///////////////////////////////////
//
// MONITORED SUBSCRIBER DETAILS METHODS
//
// ///////////////////////////////////
RadioSubscriberMonitor::MonitoredCallInformation & RadioSubscriberMonitor::MonitoredSubscriberDetails::findAnyCall(CallID callId, bool & found)
{
    found = false;
    for ( MonitoredCallList::iterator callIter = monitoredCalls.begin() ;
          callIter != monitoredCalls.end() ;
          callIter++
        )
    {
        MonitoredCallInformation & x_mon_call_info = *callIter;
        if ( (x_mon_call_info.monitorCall == callId) || (x_mon_call_info.listenCall == callId) || (x_mon_call_info.joinedCall == callId) )
        {
            found = true;
            return *callIter;
        }
    }

    return NullMonitoredCallInformation;
}


RadioSubscriberMonitor::MonitoredCallInformation & RadioSubscriberMonitor::MonitoredSubscriberDetails::findMonitoredCall(CallID callId, bool & found)
{
    found = false;
    for ( MonitoredCallList::iterator callIter = monitoredCalls.begin() ;
          callIter != monitoredCalls.end() ;
          callIter++
        )
    {
        MonitoredCallInformation & x_mon_call_info = *callIter;
        if ( (x_mon_call_info.monitorCall == callId) && (x_mon_call_info.monitorStatus == Monitoring) )
        {
            found = true;
            return *callIter;
        }
    }

    return NullMonitoredCallInformation;
}


RadioSubscriberMonitor::MonitoredCallInformation & RadioSubscriberMonitor::MonitoredSubscriberDetails::findListenedCall(CallID callId, bool & found)
{
    found = false;
    for ( MonitoredCallList::iterator callIter = monitoredCalls.begin() ;
          callIter != monitoredCalls.end() ;
          callIter++
        )
    {
        MonitoredCallInformation & x_mon_call_info = *callIter;
        if ( ( (x_mon_call_info.listenCall == callId) || (x_mon_call_info.monitorCall == callId) )
                && (x_mon_call_info.monitorStatus == Listening) )
        {
            found = true;
            return *callIter;
        }
    }

    return NullMonitoredCallInformation;
}


RadioSubscriberMonitor::MonitoredCallInformation & RadioSubscriberMonitor::MonitoredSubscriberDetails::findJoinableCall(CallID callId, bool & found)
{
    found = false;
    for ( MonitoredCallList::iterator callIter = monitoredCalls.begin() ;
          callIter != monitoredCalls.end() ;
          callIter++
        )
    {
        MonitoredCallInformation & x_mon_call_info = *callIter;
        if ( ( (x_mon_call_info.listenCall == callId) || (x_mon_call_info.monitorCall == callId) )
          && ( (x_mon_call_info.monitorStatus == Listening) || (x_mon_call_info.monitorStatus == Monitoring) )
           )
        {
            found = true;
            return *callIter;
        }
    }

    return NullMonitoredCallInformation;
}


RadioSubscriberMonitor::MonitoredCallInformation & RadioSubscriberMonitor::MonitoredSubscriberDetails::findJoinedCall(CallID callId, bool & found)
{
    found = false;
    for ( MonitoredCallList::iterator callIter = monitoredCalls.begin() ;
          callIter != monitoredCalls.end() ;
          callIter++
        )
    {
        MonitoredCallInformation & x_mon_call_info = *callIter;
        if ( (x_mon_call_info.joinedCall == callId)
             && ( (x_mon_call_info.monitorStatus == Included)
               || (x_mon_call_info.monitorStatus == Joined)
               || (x_mon_call_info.monitorStatus == PendingInclude) )
           )
        {
            found = true;
            return *callIter;
        }
    }

    return NullMonitoredCallInformation;
}


EMonitorStatusType RadioSubscriberMonitor::MonitoredSubscriberDetails::getCallMonitorStatus(CallID callId)
{
    for ( MonitoredCallList::iterator callIter = monitoredCalls.begin() ;
          callIter != monitoredCalls.end() ;
          callIter++
        )
    {
        MonitoredCallInformation & x_mon_call_info = *callIter;
        if ( (PendingInclude == x_mon_call_info.monitorStatus)
             && ( (x_mon_call_info.monitorCall == callId) // i really can't predict in advance,
                || (x_mon_call_info.listenCall == callId) // which of these will be tested for
                || (x_mon_call_info.joinedCall == callId)
                )
           )
        {
            return x_mon_call_info.monitorStatus;
        }
    }

    return NotMonitored;
}


bool RadioSubscriberMonitor::MonitoredSubscriberDetails::removeCall(CallID callId)
{
    bool found = false;
    for ( MonitoredCallList::iterator callIter = monitoredCalls.begin() ;
          callIter != monitoredCalls.end() ;
          callIter++
        )
    {
        MonitoredCallInformation & x_mon_call_info = *callIter;
        if ( (x_mon_call_info.monitorCall == callId) || (x_mon_call_info.listenCall == callId) || (x_mon_call_info.joinedCall == callId) )
        {
            found = true;
            // only really remove this if we are not awaiting further details
            // this is because we will get a removeCall for the disconnected
            // Individual/Single Call
            if (x_mon_call_info.monitorStatus != PendingInclude)
            {
                delete ( (*callIter).monitoredCallDetails );
                (*callIter).monitoredCallDetails = 0;

                monitoredCalls.erase(callIter--);
            }
            // don't break here, we want to remove this for all subscribers that
            // we may be monitoring that are in this call
			// break;
        }
    }

    return found;
}

void RadioSubscriberMonitor::MonitoredSubscriberDetails::cleanup()
{
    for ( MonitoredCallList::iterator callIter = monitoredCalls.begin() ; callIter != monitoredCalls.end() ; callIter++)
    {
        delete ( (*callIter).monitoredCallDetails );
        (*callIter).monitoredCallDetails = 0;

        // no need to erase the iterated callIter - we're only doing a deep clean
        // here, c++ can handle the shallow stuff
    }
}

RadioSubscriberMonitor::MonitoredSubscriberDetails::MonitoredSubscriberDetails()
: monitorRef(0) { }








// ///////////////////////////////////
//
// MONITORED CALL INFORMATION METHODS
//
// ///////////////////////////////////
RadioSubscriberMonitor::MonitoredCallInformation::~MonitoredCallInformation()
{
}


RadioSubscriberMonitor::MonitoredCallInformation RadioSubscriberMonitor::NullMonitoredCallInformation;


RadioSubscriberMonitor::MonitoredCallInformation::MonitoredCallInformation()
: monitorStatus(NotMonitored)
, monitorCall(0)
, listenCall(0)
, joinedCall(0)
, monitoredCallDetails(NULL)
{ }









// ///////////////////////////////////
//
// STATE UPDATE METHODS
//
// ///////////////////////////////////

void RadioSubscriberMonitor::applyFullState(const RadioMonitorManagerType& state)
{
	FUNCTION_ENTRY("applyFullState");
	
	MonitoredSubscriberDetailsTypeMap::const_iterator it;
	
    {
        TA_Base_Core::ThreadGuard guard(m_monitoredLock);
		
		 for (MonitoredSubscriberMap::iterator iter = m_monitoredSubscribers.begin();iter != m_monitoredSubscribers.end();)
		{
		    MonitoredSubscriberDetails * pfndDetails = (*iter).second;
		    pfndDetails->cleanup();
		    delete pfndDetails;
			pfndDetails = NULL;

		    m_monitoredSubscribers.erase(iter++);
		}

		for (it = state.monitoredSubscriberDetailsMap.begin(); it != state.monitoredSubscriberDetailsMap.end(); it++)
        {
	
			MonitoredSubscriberDetails*  monitoredSubscriberDetails = new MonitoredSubscriberDetails;
			monitoredSubscriberDetails->monitorRef = (it->second).monitorReference;
			m_monitoredSubscribers.insert(std::pair< ta_int32 , MonitoredSubscriberDetails*>(it->first,monitoredSubscriberDetails) );
		}
		
	}

	FUNCTION_EXIT;
}

void RadioSubscriberMonitor::clearFullState()
{
	FUNCTION_ENTRY("clearFullState");
	
	{
		TA_Base_Core::ThreadGuard guard(m_monitoredLock);
	
		for (MonitoredSubscriberMap::iterator iter = m_monitoredSubscribers.begin();iter != m_monitoredSubscribers.end();)
		{
		    MonitoredSubscriberDetails * pfndDetails = (*iter).second;
		    pfndDetails->cleanup();
		    delete pfndDetails;
			pfndDetails = NULL;

		    m_monitoredSubscribers.erase(iter++);
		}
	}

	FUNCTION_EXIT;
}

void RadioSubscriberMonitor::getFullState(RadioMonitorManagerType& state)
{
	FUNCTION_ENTRY("getFullState");
	
	MonitoredSubscriberMap::const_iterator it;
	
	{
		TA_Base_Core::ThreadGuard guard(m_monitoredLock);
		
		// get all the calls from the stack
		state.monitoredSubscriberDetailsMap.clear();
	
		for (it = m_monitoredSubscribers.begin(); it != m_monitoredSubscribers.end(); it++)
		{
			MonitoredSubscriberDetailsType monitoredSubscriberDetailsType;
			monitoredSubscriberDetailsType.monitorReference = (it->second)->monitorRef;
			state.monitoredSubscriberDetailsMap.insert(std::pair< ta_int32 , MonitoredSubscriberDetailsType>(it->first,monitoredSubscriberDetailsType) );

		}
			
	}

	FUNCTION_EXIT;
}


void RadioSubscriberMonitor::sendStateUpdate(MonitorReference	&monitorRef)
{

	if ( isInControl () ) {
		RadioUpdateMonitoredCallProgression message;
		message.monitorReference = monitorRef;
		
		RadioStateUpdateBroadcaster::getInstance()->sendRadioUpdateMonitoredCallProgression(message);
	}
}


void RadioSubscriberMonitor::setToControlMode () 
{
  m_isInControl = true ;
}

	
void RadioSubscriberMonitor::setToMonitorMode () 
{
   m_isInControl = false ;
}


