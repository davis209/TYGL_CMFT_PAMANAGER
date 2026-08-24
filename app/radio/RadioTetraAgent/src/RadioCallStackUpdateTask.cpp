/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/RadioCallStackUpdateTask.cpp $
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

#include "bus/radio/RadioAgentCorbaDef_4669/idl/src/IRadioCorbaDef.h"

#include "app/radio/RadioTetraAgent/src/RadioCallStackUpdateTask.h"
#include "app/radio/RadioTetraAgent/src/RadioTcpServer.h"
#include "app/radio/RadioTetraAgent/src/RadioHelper.h"
#include "app/radio/RadioTetraAgent/src/RadioSessionHelper.h"
#include "app/radio/RadioTetraAgent/src/RadioSessionException.h"
#include "app/radio/RadioTetraAgent/src/RadioSubscriberMonitor.h"
#include "app/radio/RadioTetraAgent/src/RadioSession.h"

namespace TA_IRS_App
{

    RadioCallStackUpdateTask::RadioCallStackUpdateTask
    (   RadioTcpServer * radio,
        const TA_IRS_App::CallDetailsType& callDetails,
		TA_IRS_App::ECallPriority defaultEmergencyCallPriority)
        : RadioTask(radio)
        , m_callDetails(callDetails)
		, m_DefaultEmergencyCallPriority(defaultEmergencyCallPriority)
    {
        m_taskName = "RadioCallStackUpdateTask";
        
        // add directly to the call stack
        //getRadio().addCallToCallStack(m_callDetails);
    }


    RadioCallStackUpdateTask::~RadioCallStackUpdateTask()
    {
    }

	void RadioCallStackUpdateTask::setDefaultEmergencyCallPriority(TA_IRS_App::ECallPriority defaultEmergencyCallPriority)
	{
		m_DefaultEmergencyCallPriority = defaultEmergencyCallPriority;
	}


    void RadioCallStackUpdateTask::perform()
    {
        FUNCTION_ENTRY("perform()");
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "callRef:%lu sessionRef:%lu ", m_callDetails.callID, m_callDetails.sessionRef);

        ta_uint32 callRef = m_callDetails.callID;
        ta_uint32 sessRef = m_callDetails.sessionRef;

        RadioSessionHelper * helper = new RadioSessionHelper(sessRef, getRadio());

        QueryKeyValueMap callValues;
        callValues[(ta_uint32 )RadioMethodQueryReference::K_PRIORITY] = "";
        callValues[(ta_uint32 )RadioMethodQueryReference::K_CALLING] = "";
        callValues[(ta_uint32 )RadioMethodQueryReference::K_CALLED] = "";
        callValues[(ta_uint32 )RadioMethodQueryReference::K_CALL_TYPE] = "";
		helper->getCallDetails( callRef, callValues );
		// WAS:
        // helper->queryReferenceAsMap( callRef, RadioMethodQueryReference::K_KEY_ALL_DATACALL, callValues);

        //
        // CALL CONNECT TIME not available on un-monitored calls (which this one may very well not be!)
        // however callTime and time fields will be set by addCallToCallStack()

        //
        // Emergency/Priority detail
        // The isEmergency flag is usually cleared by default.  If it is
        // already set, leave it alone, otherwise get the priority from 
        // the RCS TCPServer
        if (!m_callDetails.isEmergency)
        {
            std::string priority_str = RadioHelper::getStdStringValue(callValues, RadioMethodQueryReference::K_PRIORITY);
            int priority = atoi(priority_str.c_str());
            m_callDetails.isEmergency = (priority == static_cast<int> (m_DefaultEmergencyCallPriority));
        }

		std::string call_type_str = RadioHelper::getStdStringValue(callValues, RadioMethodQueryReference::K_CALL_TYPE);
		ECallType call_type = helper->convert2CallType(call_type_str);
		// bool isIndividualCall = (call_type == SingleCall);

        // 
        // We might encounter exceptions with retrieving subscriber details, but
        // we already have the two most important TSIs so might as well stick
        // them in the structure.
        std::string subscriberTSI = RadioHelper::getStdStringValue(callValues, RadioMethodQueryReference::K_CALLING);
        m_callDetails.callerTSI       = subscriberTSI;
        subscriberTSI = RadioHelper::getStdStringValue(callValues, RadioMethodQueryReference::K_CALLED);
        m_callDetails.calledTSI       = subscriberTSI;

        
		// 
		// Sometimes the call is ended before the details are retrieved, so the TSI is null.
		// In that case we don't want the subscriber details anyway.
		if (!(subscriberTSI.empty()))
		{
			// get the caller details from the database
			bool callerDetailsRetrieved = false;
			try
			{
				TA_Base_Core::IRASubscribers* callerDetails = 
					TA_Base_Core::RASubscribersAccessFactory::getInstance().getSubscriberDetails( m_callDetails.callerTSI );

				m_callDetails.callerName = callerDetails->getSubscriberName();
				m_callDetails.callerLocation = callerDetails->getLocation();

				delete callerDetails;

				callerDetailsRetrieved = true;
			}
			catch( TA_Base_Core::TransactiveException& te )
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
					"Caught ... while trying to retrieve subscriber details for %s: %s",
					m_callDetails.callerTSI.c_str(),
					te.what() );
			}
			catch( ... )
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
					"Caught ... while trying to retrieve subscriber details for %s",
					m_callDetails.callerTSI.c_str() );
			}

			if ( false == callerDetailsRetrieved )
			{
				try
				{
					//
					// CALLING Subscriber details

					QueryKeyValueMap subscriberDetails;
					subscriberDetails[(ta_uint32)RadioMethodQueryReference::K_NAME] = "";
					subscriberDetails[(ta_uint32)RadioMethodQueryReference::K_ORGANISATION] = "";
					int num_details = helper->getSubscriberDetails(m_callDetails.callerTSI, subscriberDetails);

					// don't check the number of details we could get, they won't affect the out come of retrieving the values from the map
					// if its important to know that something failed, the helper getSubscriberDetails should log that
					m_callDetails.callerName      = subscriberDetails[(ta_uint32)RadioMethodQueryReference::K_NAME];
					m_callDetails.callerLocation  = subscriberDetails[(ta_uint32)RadioMethodQueryReference::K_ORGANISATION];

					if (m_callDetails.callerName.empty())
					{
						m_callDetails.callerName = m_callDetails.callerTSI;
					}

					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Voice Call callerName:%s callerTSI:%s callerLocation:%s", m_callDetails.callerName.c_str(), m_callDetails.callerTSI.c_str(), m_callDetails.callerLocation.c_str());
				}
				catch (RadioSessionException rse)
				{
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Caught RadioSessionException while trying to retrieve subscriber details: %s", rse.what());
				}
				catch (RadioFallbackException rfe)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Radio in Fallback. Will not retrieve subscriber details");

					std::string replacement = helper->getFallbackReplacementString();
					m_callDetails.callerName = replacement;
					m_callDetails.callerLocation = replacement;
				}
			}
		} // 

		subscriberTSI = RadioHelper::getStdStringValue(callValues, RadioMethodQueryReference::K_CALLED);
		m_callDetails.calledTSI = subscriberTSI;

		// 
		// Sometimes the call is ended before the details are retrieved, so the TSI is null.
		// In that case we don't want the subscriber details anyway.
		if (!(subscriberTSI.empty()))
		{
			// get the called details from the database
			bool calledDetailsRetrieved = false;
			try
			{
				TA_Base_Core::IRASubscribers* calledDetails = 
					TA_Base_Core::RASubscribersAccessFactory::getInstance().getSubscriberDetails( m_callDetails.calledTSI );

				// Check if this TSI is patch 
				if (calledDetails->getSubscriberType() == IRASubscribers::GROUP ||
					calledDetails->getSubscriberType() == IRASubscribers::PATCH_GROUP)
				{
					std::string patchName = TA_Base_Core::RASubscribersAccessFactory::getInstance().getPatchNameFromMemberTSI(m_callDetails.calledTSI);
					if (!patchName.empty())
						m_callDetails.calledName = patchName;
					else
						m_callDetails.calledName = calledDetails->getSubscriberName();
				}
				else
				{
					m_callDetails.calledName = calledDetails->getSubscriberName();
				}
				
				m_callDetails.calledLocation = calledDetails->getLocation();

				delete calledDetails;

				calledDetailsRetrieved = true;
			}
			catch( TA_Base_Core::TransactiveException& te )
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
					"Caught ... while trying to retrieve subscriber details for %s: %s",
					m_callDetails.calledTSI.c_str(),
					te.what() );
			}
			catch( ... )
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
					"Caught ... while trying to retrieve subscriber details for %s",
					m_callDetails.calledTSI.c_str() );
			}


			if ( false == calledDetailsRetrieved )
			{
				try
				{
					//
					// CALLED Subscriber details

					QueryKeyValueMap subscriberDetails;
					subscriberDetails[(ta_uint32)RadioMethodQueryReference::K_NAME] = "";
					subscriberDetails[(ta_uint32)RadioMethodQueryReference::K_ORGANISATION] = "";
					int num_details = helper->getSubscriberDetails(m_callDetails.calledTSI, subscriberDetails);
					m_callDetails.calledName      = subscriberDetails[(ta_uint32)RadioMethodQueryReference::K_NAME];
					m_callDetails.calledLocation  = subscriberDetails[(ta_uint32)RadioMethodQueryReference::K_ORGANISATION];

					if (m_callDetails.calledName.empty())
					{
						m_callDetails.calledName = m_callDetails.calledTSI;
					}

					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Voice Call calledName:%s calledTSI:%s calledLocation:%s", m_callDetails.calledName.c_str(), m_callDetails.calledTSI.c_str(), m_callDetails.calledLocation.c_str());
				}
				catch (RadioSessionException rse)
				{
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Caught RadioSessionException while trying to retrieve subscriber details: %s", rse.what());
				}
				catch (RadioFallbackException rfe)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Radio in Fallback. Will not retrieve subscriber details");

					std::string replacement = helper->getFallbackReplacementString();
					m_callDetails.calledName = replacement;
					m_callDetails.calledLocation = replacement;
				}
			}
		}		

        // the call status and category are generally populated by the generating Radio::function
        // LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CallStackUpdate details, callStatus:%d calledCategory:%d", (int)(m_callDetails->callStatus), (int)(m_callDetails->callCategory));
		RadioSession * session = getRadio().findSession(sessRef);
		if (m_callDetails.callCategory == HalfDuplex && 0 != session)
			m_callDetails.isIncludeCall = session->isIncludeCallITSI(m_callDetails.calledTSI);

        // now that the call details have been populated, add the call to the stack.
        getRadio().addCallToCallStack(m_callDetails);


		if ((m_callDetails.callSource == IncomingCall) && (0 != session))
		{
			// can only do this after adding to the callstack
			if (getRadio().getSubscriberMonitor().isListenCall(callRef))
			{	
				session->answerCall(callRef, "");
			}
			else if (session->isGroupMonitored(m_callDetails.calledTSI))
			{
				session->answerCall(callRef, "", true);
			}
			else if (m_callDetails.isIncludeCall)
			{	// Auto answer here for include call
				LOG3(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Auto answering include call for sessionRef=%d and called TSI=%s, callID=%d",
					sessRef, m_callDetails.calledTSI.c_str(), callRef);

				session->answerCall(callRef, "");

				// Select The call
				session->selectCall(callRef, "");
			}
		} 
        // call stack has a by-value copy of the details now
       
    }
} // namespace TA_IRS_App
