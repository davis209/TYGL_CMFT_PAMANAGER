/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/RadioSubscriberActivityTask.cpp $
  * @author  R. van Hugten
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2008/11/28 16:26:01 $
  * Last modified by : $Author: builder $
  *
  * Declares an abstract base class for elements that can appear on the
  * RadioBackgroundProcessor request queue.
  *
  */
#include "core/types/src/ta_types.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"

#include "app/radio/RadioTetraAgent/src/Radio.h"
#include "app/radio/RadioTetraAgent/src/RadioHelper.h"
#include "app/radio/RadioTetraAgent/src/RadioTcpServer.h"
#include "app/radio/RadioTetraAgent/src/RadioSessionHelper.h"
#include "app/radio/RadioTetraAgent/src/RadioSubscriberMonitor.h"
#include "app/radio/RadioTetraAgent/src/RadioSubscriberActivityTask.h"
#include "app/radio/RadioTetraAgent/src/RadioSession.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_App
{
    RadioSubscriberActivityTask::RadioSubscriberActivityTask
    (   RadioTcpServer * radio,
        RadioSubscriberMonitor & subscriberMonitor,
        ta_uint32 sessRef,
        ta_uint32 callRef,
        ta_uint32 monitorRef,
        bool isCallStart)
        : RadioTask(radio)
        , m_subscriberMonitor(subscriberMonitor)
        , m_sessionReference(sessRef)
        , m_callReference(callRef)
        , m_monitorReference(monitorRef)
        , m_isCallStart(isCallStart)
    {
        FUNCTION_ENTRY("~RadioSubscriberActivityTask()");
        m_taskName = "RadioSubscriberActivityTask";

        FUNCTION_EXIT;
    }

    RadioSubscriberActivityTask::~RadioSubscriberActivityTask()
    {
        FUNCTION_ENTRY("~RadioSubscriberActivityTask()");
        FUNCTION_EXIT;
    }

    void RadioSubscriberActivityTask::perform()
    {
        FUNCTION_ENTRY("perform()");


        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
                    "SubscriberActivity for Call %lu Session %lu Monitor %lu (%s)",
                    m_callReference, m_sessionReference, m_monitorReference,
                    ((m_isCallStart)?"Call Start":"Call End"));

		ECallSource callSource;
        bool is_known = m_subscriberMonitor.isKnownCall(m_monitorReference, m_callReference);
        EMonitorStatusType monitor_status = m_subscriberMonitor.getCallMonitorStatus(m_monitorReference, m_callReference);
        if ( (!is_known) || (PendingInclude == monitor_status) )
        {
            // if the call is not known or is pending join (in which case its
            // considered new), we need to eek out these details
            MonitoredCallDetailsType * mon_call_details = new MonitoredCallDetailsType;
            mon_call_details->callID          = m_callReference;
            mon_call_details->sessionRef      = m_sessionReference;
            mon_call_details->monitorID       = m_monitorReference;
            if (is_known)
            {
                // legitimately, by design, the only way we have come this far
                // and the call be known is if it is a CALL_END for a Single
                // Call that is about to be turned into a Group Call
                mon_call_details->monitorStatus = Stopped;
            }
            else
            {
                mon_call_details->monitorStatus = (m_isCallStart ? Monitoring : Stopped);
            }

            // If m_isCallStart==false but this is not a known call, this is not
            // good news. We should only get the Call_End subscriberActivity if
            // we already received the Call_Start and got all the details. 
            // Nevertheless, we get everything... but complain first
            if ( (!is_known) && (!m_isCallStart) )
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn,
                            "CALL_END SubscriberActivity for unknown Call %lu Session %lu Monitor %lu",
                            m_callReference, m_sessionReference, m_monitorReference);
            }
			monitor_status = mon_call_details->monitorStatus;

            RadioSessionHelper  helper(m_sessionReference, getRadio());

            // All other MonitoredCallDetails need to be updated
            helper.populateMonitoredCallDetails(mon_call_details);
            helper.populateMonitoredCallSubscriberDetails(mon_call_details);


            m_subscriberMonitor.subscriberActivity(mon_call_details);

			callSource = mon_call_details->callSource;
        }
        else
        {
            if (m_isCallStart)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn,
                            "CALL_START SubscriberActivity for known Call %lu Session %lu Monitor %lu",
                            m_callReference, m_sessionReference, m_monitorReference);
            }

			callSource = m_subscriberMonitor.getCallSource(m_monitorReference, m_callReference);
            m_subscriberMonitor.subscriberActivity(m_monitorReference, m_callReference, m_isCallStart);
		}

		// We will automatically start listening to avoid race condition
		if (monitor_status == Monitoring && 
			callSource == IncomingCall) // Might need to check if call is out going or incomming
		{	// Start listening
			RadioSession* pSession = getRadio().findSession(m_sessionReference);
			if (0 != pSession)
			{
				ta_uint32 newCallID = pSession->startListening(m_callReference);
				LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Started listening to monitored callID %d and new CallID %d", m_callReference, newCallID);
			}
		}

        FUNCTION_EXIT;
	}

} // namespace TA_IRS_App
