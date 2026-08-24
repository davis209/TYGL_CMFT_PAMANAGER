/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/RadioSessionTask.h $
  * @author:  R. van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Declares an abstract base class for elements that can appear on the
  * RadioBackgroundProcessor request queue.
  *
  */

#ifndef RADIO_SESSION_TASK_H
#define RADIO_SESSION_TASK_H
#include "app/radio/RadioTetraAgent/src/RadioTask.h"

namespace TA_IRS_App
{

    class RadioTcpServer;
    class RadioSessionServant;

    class RadioSessionTask
        : public RadioTask
    {
        public:

            enum RadioSessionTaskType
	        {
		        BeginSession,
		        EndSession
	        };

            /**
             * 
             */
            RadioSessionTask(RadioTcpServer * radio, RadioSessionServant& sessionservant, 
				std::string session, RadioSessionTaskType taskType);


            /**
             *
             */
            ~RadioSessionTask();

        protected:
                
            virtual void perform();
            

        private:

			void beginSession();
			void endSession();

			RadioSessionServant& m_sessionServant;
			std::string m_session;
			RadioSessionTaskType m_taskType;


    }; // class RadioSessionTask
} // namespace TA_IRS_App

#endif // #ifndef RADIO_SESSION_TASK_H
