#ifndef RADIO_SDS_DETAILS_UPDATE_TASK_H
#define RADIO_SDS_DETAILS_UPDATE_TASK_H
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/RadioSDSDetailsUpdateTask.h $
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

    class RadioSDSDetailsUpdateTask
        : public RadioTask
    {
        public:
            /**
             * Constructor to add the call to the stack
             */
            RadioSDSDetailsUpdateTask
                    (   RadioTcpServer * radio,
                        TA_IRS_App::CallDetailsType * callDetails);


            /**
             *
             */
            ~RadioSDSDetailsUpdateTask();

        protected:
                
                virtual void perform();
            

        private:

            ///
            ///
            TA_IRS_App::CallDetailsType * m_callDetails;

    }; // class RadioSDSDetailsUpdateTask
} // namespace TA_IRS_App

#endif // #ifndef RADIO_SDS_DETAILS_UPDATE_TASK_H
