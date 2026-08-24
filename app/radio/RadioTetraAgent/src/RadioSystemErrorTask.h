#ifndef RADIO_SYSTEMERROR_TASK_H
#define RADIO_SYSTEMERROR_TASK_H
/**
  * The source code in this file is the property of
  * MI Services Group and is not for redistribution
  * in any form.
  *
  * Source : $Source: /cvs/1TS-9999-T/code/transactive/app/radio/RadioTetraAgent/src/Attic/RadioSystemErrorTask.h,v $
  * @author  R. van Hugten
  * @version $Revision: 
  * Last modification : $Date: 2007/10/12 12:52:24 $
  * Last modified by : $Author: robertvh $
  *
  * Declares an abstract base class for elements that can appear on the
  * RadioBackgroundProcessor request queue.
  *
  */

#include "app/radio/RadioTetraAgent/src/RadioTask.h"
#include "app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioEvent.h"

namespace TA_IRS_App
{

    class RadioTcpServer;
    class RadioSessionServant;

    class RadioSystemErrorTask
        : public RadioTask
    {
        public:

            /**
             * 
             */
            RadioSystemErrorTask(RadioTcpServer * radio, const TA_IRS_App::RadioEvent & event);

            /**
             *
             */
            ~RadioSystemErrorTask();

        protected:
                
            virtual void perform();
            

        private:

            TA_IRS_App::RadioEvent & m_event;

    }; // class RadioSystemErrorTask
} // namespace TA_IRS_App

#endif // #ifndef RADIO_SYSTEMERROR_TASK_H
