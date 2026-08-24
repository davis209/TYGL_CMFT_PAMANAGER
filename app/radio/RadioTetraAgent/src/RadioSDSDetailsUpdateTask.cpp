/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/RadioSDSDetailsUpdateTask.cpp $
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

#include "app/radio/RadioTetraAgent/src/RadioSDSDetailsUpdateTask.h"
#include "app/radio/RadioTetraAgent/src/RadioTcpServer.h"
#include "app/radio/RadioTetraAgent/src/RadioHelper.h"
#include "app/radio/RadioTetraAgent/src/RadioSessionHelper.h"

namespace TA_IRS_App
{

     RadioSDSDetailsUpdateTask::RadioSDSDetailsUpdateTask
    (   RadioTcpServer * radio,
        TA_IRS_App::CallDetailsType * callDetails)
        : RadioTask(radio)
        , m_callDetails(callDetails)
    {
        m_taskName = "RadioSDSDetailsUpdateTask";
    }


    RadioSDSDetailsUpdateTask::~RadioSDSDetailsUpdateTask()
    {
    }

    void RadioSDSDetailsUpdateTask::perform()
    {
        FUNCTION_ENTRY("perform()");
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "SDS Ref:%lu sessionRef:%lu ", m_callDetails->callID, m_callDetails->sessionRef);

        ta_uint32 callRef = m_callDetails->callID;
        ta_uint32 sessRef = m_callDetails->sessionRef;

        RadioSessionHelper * helper = new RadioSessionHelper(sessRef, getRadio());

        try
        {
            QueryKeyValueMap subscriberDetails;
            subscriberDetails[(ta_uint32 )RadioMethodQueryReference::K_NAME] = "";
            subscriberDetails[(ta_uint32 )RadioMethodQueryReference::K_ORGANISATION] = "";
            int num_details = helper->getSubscriberDetails(m_callDetails->callerTSI, subscriberDetails);
            if ( num_details == 2 )
            {
                m_callDetails->callerName      = subscriberDetails[(ta_uint32 )RadioMethodQueryReference::K_NAME];
                m_callDetails->callerLocation  = subscriberDetails[(ta_uint32 )RadioMethodQueryReference::K_ORGANISATION];

                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "SDS callerName: %s, callerLocation: %s", m_callDetails->callerName.c_str(), m_callDetails->callerLocation.c_str());
            }
            else
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Could not retrieve calling subscriber location from the RCS");
            }

        }
        catch (RadioFallbackException rfe)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Radio in Fallback. Will not retrieve subscriber details");

            std::string replacement = helper->getFallbackReplacementString();
            m_callDetails->callerName = replacement;
            m_callDetails->callerLocation = replacement;
        }
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Errors occured while retrieving SDS details. ");
		}

        if (m_callDetails->callerName.empty())
        {
            m_callDetails->callerName = m_callDetails->callerTSI;
        }

        // now that the call details have been populated, add the call to the stack.
        getRadio().addCallToCallStack(*m_callDetails);

        // call stack has a by-value copy of the details now
        delete m_callDetails;
    }
} // namespace TA_IRS_App
