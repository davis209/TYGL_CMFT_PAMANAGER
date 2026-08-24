#ifndef RADIO_CALL_STACK_UPDATE_TASK_H
#define RADIO_CALL_STACK_UPDATE_TASK_H
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/RadioCallStackUpdateTask.h $
  * @author  R. van Hugten
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2008/11/28 16:26:01 $
  * Last modified by : $Author: builder $
  *
  * Declares an abstract base class for elements that can appear on the
  * RadioBackgroundProcessor request queue.
  *
  */

#include "app/radio/RadioTetraAgent/src/RadioTask.h"
#include "app/radio/RadioTetraAgent/src/RadioTypes.h"

namespace TA_IRS_App
{
    class RadioTcpServer;
    class IRadioCallback;

    class RadioCallStackUpdateTask
        : public RadioTask
    {
        public:
            /**
             * Constructor to add the call to the stack
             */
            RadioCallStackUpdateTask
                    (   RadioTcpServer * radio,
                        const TA_IRS_App::CallDetailsType& callDetails,
						TA_IRS_App::ECallPriority defaultEmergencyCallPriority = Priority15);


            /**
             *
             */
            ~RadioCallStackUpdateTask();

			void setDefaultEmergencyCallPriority(TA_IRS_App::ECallPriority defaultEmergencyCallPriority);

        protected:
            virtual void perform();

        private:

            ///
            ///
            TA_IRS_App::CallDetailsType m_callDetails;
			TA_IRS_App::ECallPriority m_DefaultEmergencyCallPriority;

    }; // class RadioCallStackUpdateTask
} // namespace TA_IRS_App



#endif // #ifndef RADIO_CALL_STACK_UPDATE_TASK_H
