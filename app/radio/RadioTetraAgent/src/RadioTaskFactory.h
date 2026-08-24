#ifndef RADIO_TASK_FACTORY_H
#define RADIO_TASK_FACTORY_H
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/RadioTaskFactory.h $
  * @author  R. van Hugten
  * @version $Revision: #3 $
  * Last modification : $DateTime: 2009/09/23 13:09:47 $
  * Last modified by : $Author: builder $
  *
  * Declares an abstract base class for elements that can appear on the
  * RadioBackgroundProcessor request queue.
  *

  */

#include "core/types/src/ta_types.h"
#include "bus/radio/RadioAgentCorbaDef_4669/idl/src/IRadioCorbaDef.h"
#include "app/radio/RadioTetraAgent/src/RadioTypes.h"
#include "app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodDeleteReference.h"
#include "app/radio/RadioTetraAgent/src/RadioSession.h"
#include "app/radio/RadioTetraAgent/src/RadioTask.h"



namespace TA_IRS_App
{
    class Radio;
    class RadioTcpServer;
    class IRadioCallback;
    class GroupCallAckEventReader;
	class RadioSubscriberMonitor;
	class RadioSessionServant;

    class RadioTaskFactory
    {
        public:
            /**
             *
             */
            static RadioTaskPtr createSubscriberActivityTask(RadioTcpServer * radio, RadioSubscriberMonitor & subscriberMonitor, ta_uint32 sessRef, ta_uint32 callRef, ta_uint32 monitorRef, bool isCallStart);


            /**
             *
             */
            static RadioTaskPtr createCallStackUpdateTask(RadioTcpServer * radio, const TA_IRS_App::CallDetailsType& callDetails);

            /**
             *
             */
            static RadioTaskPtr createCallStackUpdateTask(RadioTcpServer * radio, const TA_IRS_App::CallDetailsType& callDetails, TA_IRS_App::ECallPriority defaultEmergencyCallPriority);

            /**
             *
             */
            static RadioTaskPtr createSDSDetailsUpdateTask(RadioTcpServer * radio, TA_IRS_App::CallDetailsType * callDetails);

            /**
             *
             */
            static RadioTaskPtr createGroupCallAckTask(RadioTcpServer * radio, const TA_IRS_App::CallDetailsType& callDetails, const TA_IRS_App::GroupCallAckEventReader & eventReader);


            /**
             *
             */
			static RadioTaskPtr createGroupCallAckTask(RadioTcpServer * radio, const TA_IRS_App::CallDetailsType& callDetails, const std::vector< std::string > & tsiVector);


            /**
             *
             */
			static RadioTaskPtr createResetConnectionTask(RadioTcpServer * radio);

			/**
             *
             */
			static RadioTaskPtr createResetSessionTask(RadioTcpServer * radio, ta_uint32 sessionRef);


            /**
             *
             */
			static RadioTaskPtr createResetSocketTask(RadioTcpServer * radio);

            /**
             *
             */
            static RadioTaskPtr createDeleteReferenceTask(RadioTcpServer * radio, TA_IRS_App::SessionReference sessionRef, ta_uint32 delRef, RadioMethodDeleteReference::ReferenceType refType);


            static RadioTaskPtr createRadioSessionBeginTask(RadioTcpServer * radio, RadioSessionServant& sessionservant, const std::string& session);

			
            static RadioTaskPtr createRadioSessionEndTask(RadioTcpServer * radio, RadioSessionServant& sessionservant, const std::string& session);


        private:
            /** Hidden constructor: not intended for instantiation */
            RadioTaskFactory();

            /** Hidden destructor: not intended for instantiation */
            ~RadioTaskFactory();
    }; // class RadioTaskFactory

} // namespace TA_IRS_App

#endif // #ifndef RADIO_TASK_FACTORY_H
