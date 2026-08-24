/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/RadioGroupCallAckTask.cpp $
  * @author  R. van Hugten
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2008/11/28 16:26:01 $
  * Last modified by : $Author: builder $
  *
  * Implementation of the RadioGroupCallAckTask class.
  *
  */
#include "core/types/src/ta_types.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "bus/radio/RadioAgentCorbaDef_4669/idl/src/IRadioCorbaDef.h"

#include "app/radio/RadioTetraAgent/src/RadioGroupCallAckTask.h"
#include "app/radio/RadioTetraAgent/src/RadioTcpServer.h"
#include "app/radio/RadioTetraAgent/src/RadioHelper.h"
#include "app/radio/RadioTetraAgent/src/RadioSessionHelper.h"

namespace TA_IRS_App
{

    RadioGroupCallAckTask::RadioGroupCallAckTask
    (   RadioTcpServer * radio,
        const TA_IRS_App::CallDetailsType& callDetails,
        const TA_IRS_App::GroupCallAckEventReader & eventReader)
        : RadioTask(radio)
        , m_callDetails(callDetails)
    {
        m_taskName = "RadioGroupCallAckTask";
        eventReader.getTsiVector(m_tsiVector);
    }


    RadioGroupCallAckTask::RadioGroupCallAckTask
    (   RadioTcpServer * radio,
        const TA_IRS_App::CallDetailsType& callDetails,
        const TSIVector & tsiVector)
        : RadioTask(radio)
        , m_callDetails(callDetails)
        , m_tsiVector(tsiVector)
    {
        m_taskName = "RadioGroupCallAckTask";
    }

  

    void RadioGroupCallAckTask::perform()
    {
        FUNCTION_ENTRY("perform()");
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "callRef:%lu sessionRef:%lu ", m_callDetails.callID, m_callDetails.sessionRef);

        ta_uint32 callRef = m_callDetails.callID;
        ta_uint32 sessRef = m_callDetails.sessionRef;

        RadioSessionHelper helper(sessRef, getRadio());

        //
        // Emergency/Priority detail
        // The isEmergency flag is usually cleared by default.  If it is
        // already set, leave it alone, otherwise get the priority from 
        // the RCS TCPServer
        if (!m_callDetails.isEmergency)
        {
            QueryKeyValueMap callValues;
            callValues[(ta_uint32 )RadioMethodQueryReference::K_PRIORITY] = "";
            helper.getCallDetails( callRef, callValues );
            
            std::string priority_str = RadioHelper::getStdStringValue(callValues, RadioMethodQueryReference::K_PRIORITY);
            int priority = atoi(priority_str.c_str());
            m_callDetails.isEmergency = (priority == 15);
        }

        try
        {
            //
            // CALLER Subscriber details
            // The caller TSI should be set in RadioSession::makeCall
            QueryKeyValueMap subscriberDetails;
            subscriberDetails[(ta_uint32 )RadioMethodQueryReference::K_NAME] = "";
            subscriberDetails[(ta_uint32 )RadioMethodQueryReference::K_ORGANISATION] = "";
            int num_details = helper.getSubscriberDetails(m_callDetails.callerTSI, subscriberDetails);
            if ( num_details == 2 )
            {
                m_callDetails.callerName      = subscriberDetails[(ta_uint32 )RadioMethodQueryReference::K_NAME];
                m_callDetails.callerLocation  = subscriberDetails[(ta_uint32 )RadioMethodQueryReference::K_ORGANISATION];

                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Call participant details, callerName:%s callerTSI:%s callerLocation:%s", m_callDetails.callerName.c_str(), m_callDetails.callerTSI.c_str(), m_callDetails.callerLocation.c_str());
            }
            else
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Could not retrieve calling subscriber details");
            }


            //
            // CALLED Subscribers' details
            // The caller TSI should be set in RadioSession::makeCall
            subscriberDetails[(ta_uint32 )RadioMethodQueryReference::K_NAME] = "";
            subscriberDetails[(ta_uint32 )RadioMethodQueryReference::K_ORGANISATION] = "";
            num_details = helper.getSubscriberDetails(m_callDetails.calledTSI, subscriberDetails);
            if ( num_details == 2 )
            {
                m_callDetails.calledName      = subscriberDetails[(ta_uint32 )RadioMethodQueryReference::K_NAME];
                m_callDetails.calledLocation  = subscriberDetails[(ta_uint32 )RadioMethodQueryReference::K_ORGANISATION];

                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Call participant details, calledName:%s calledTSI:%s calledLocation:%s", m_callDetails.calledName.c_str(), m_callDetails.calledTSI.c_str(), m_callDetails.calledLocation.c_str());
            }
            else
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Could not retrieve called subscriber details");
            }


            // This being an Acknowledged Group Call, don't even think about Individual/SingleCall stuff
            helper.populateCallParticipantData(m_tsiVector, &m_callDetails);
        }
        catch (RadioFallbackException rfe)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Radio in Fallback. Will not retrieve subscriber details");

            std::string replacement = helper.getFallbackReplacementString();
            m_callDetails.callerName = replacement;
            m_callDetails.callerLocation = replacement;
            m_callDetails.calledName = replacement;
            m_callDetails.calledLocation = replacement;
        }

        // now that the call details have been populated, update the call in the stack.
        getRadio().addCallToCallStack(m_callDetails);
    }
} // namespace TA_IRS_App
