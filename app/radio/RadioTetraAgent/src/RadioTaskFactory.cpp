/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/RadioTaskFactory.cpp $
  * @author  R. van Hugten
  * @version $Revision: #3 $
  * Last modification : $DateTime: 2009/09/23 13:09:47 $
  * Last modified by : $Author: builder $
  *
  * Declares a class for instantiating objects that can be placed on the
  * RadioBackgroundProcessor request queue.
  *
  */

#include "core/types/src/ta_types.h"
#include "app/radio/RadioTetraAgent/src/RadioTaskFactory.h"
#include "app/radio/RadioTetraAgent/src/RadioTask.h"
#include "app/radio/RadioTetraAgent/src/RadioSubscriberActivityTask.h"
#include "app/radio/RadioTetraAgent/src/RadioCallStackUpdateTask.h"
#include "app/radio/RadioTetraAgent/src/RadioSDSDetailsUpdateTask.h"
#include "app/radio/RadioTetraAgent/src/RadioGroupCallAckTask.h"
#include "app/radio/RadioTetraAgent/src/RadioResetConnectionTask.h"
#include "app/radio/RadioTetraAgent/src/RadioResetSessionTask.h"
#include "app/radio/RadioTetraAgent/src/RadioResetSocketTask.h"
#include "app/radio/RadioTetraAgent/src/RadioDeleteReferenceTask.h"
#include "app/radio/RadioTetraAgent/src/RadioSessionTask.h"

namespace TA_IRS_App
{
    RadioTaskPtr RadioTaskFactory::createSubscriberActivityTask
    (   RadioTcpServer * radio,
        RadioSubscriberMonitor & subscriberMonitor,
        ta_uint32 sessRef,
        ta_uint32 callRef,
        ta_uint32 monitorRef,
        bool isCallStart)
    {
	    RadioTaskPtr newOwn(new RadioSubscriberActivityTask(radio, subscriberMonitor, sessRef, callRef, monitorRef, isCallStart));

        return newOwn;
    }


    RadioTaskPtr RadioTaskFactory::createCallStackUpdateTask
    (   RadioTcpServer * radio,
        const TA_IRS_App::CallDetailsType& callDetails)
    {
        RadioTaskPtr newOwn(new RadioCallStackUpdateTask(radio, callDetails));
        return newOwn;
    }
	
	RadioTaskPtr RadioTaskFactory::createCallStackUpdateTask
	(	RadioTcpServer * radio, 
		const TA_IRS_App::CallDetailsType& callDetails, 
		TA_IRS_App::ECallPriority defaultEmergencyCallPriority)
	{
        RadioTaskPtr newOwn(new RadioCallStackUpdateTask(radio, callDetails, defaultEmergencyCallPriority));
        return newOwn;
	}


    RadioTaskPtr RadioTaskFactory::createSDSDetailsUpdateTask
    (   RadioTcpServer * radio,
        TA_IRS_App::CallDetailsType * callDetails)
    {
        RadioTaskPtr newOwn(new RadioSDSDetailsUpdateTask(radio, callDetails));
        return newOwn;
    }



    RadioTaskPtr RadioTaskFactory::createGroupCallAckTask
    (   RadioTcpServer * radio,
        const TA_IRS_App::CallDetailsType& callDetails,
        const TA_IRS_App::GroupCallAckEventReader & eventReader)
    {
        RadioTaskPtr newOwn(new RadioGroupCallAckTask(radio, callDetails, eventReader));
        return newOwn;
    }


    RadioTaskPtr RadioTaskFactory::createGroupCallAckTask
    (   RadioTcpServer * radio,
        const TA_IRS_App::CallDetailsType& callDetails,
        const std::vector< std::string > & tsiVector)
    {
        RadioTaskPtr newOwn(new RadioGroupCallAckTask(radio, callDetails, tsiVector));
        return newOwn;
    }


    RadioTaskPtr RadioTaskFactory::createResetConnectionTask(RadioTcpServer * radio)
    {
        RadioTaskPtr newOwn(new RadioResetConnectionTask(radio));
        return newOwn;
    }

	RadioTaskPtr RadioTaskFactory::createResetSessionTask(RadioTcpServer * radio, ta_uint32 sessionRef)
	{
		RadioTaskPtr newOwn(new RadioResetSessionTask(radio, sessionRef));
		return newOwn;
	}

	RadioTaskPtr RadioTaskFactory::createResetSocketTask(RadioTcpServer * radio)
	{
		RadioTaskPtr newOwn(new RadioResetSocketTask(radio));
		return newOwn;
	}

    RadioTaskPtr RadioTaskFactory::createDeleteReferenceTask
    (   RadioTcpServer * radio,
        TA_IRS_App::SessionReference sessionRef,
        ta_uint32 delRef,
        RadioMethodDeleteReference::ReferenceType refType)
    {
        RadioTaskPtr newOwn(new RadioDeleteReferenceTask(radio, sessionRef, delRef, refType));
        return newOwn;
    }

    RadioTaskPtr RadioTaskFactory::createRadioSessionBeginTask
        (RadioTcpServer * radio, RadioSessionServant& sessionservant, const std::string& session)
    {
        RadioTaskPtr newOwn(new RadioSessionTask(radio, sessionservant, session, RadioSessionTask::BeginSession));
        return newOwn;
    }

    RadioTaskPtr RadioTaskFactory::createRadioSessionEndTask
        (RadioTcpServer * radio, RadioSessionServant& sessionservant, const std::string& session)
    {
        RadioTaskPtr newOwn(new RadioSessionTask(radio, sessionservant, session, RadioSessionTask::EndSession));
        return newOwn;
    }

} // namespace TA_IRS_App
