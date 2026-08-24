#ifndef RADIO_RESET_CONNECTION_TASK_H
#define RADIO_RESET_CONNECTION_TASK_H
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/RadioResetConnectionTask.h $
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

namespace TA_IRS_App
{
    class RadioTcpServer;

    class RadioResetConnectionTask
        : public RadioTask
    {
        public:
            /**
             * 
             */
            RadioResetConnectionTask(RadioTcpServer * radio);


            /**
             *
             */
            ~RadioResetConnectionTask();

        protected:
            virtual void perform();

        private:


    }; // class RadioResetConnectionTask
} // namespace TA_IRS_App

#endif // #ifndef RADIO_RESET_CONNECTION_TASK_H
