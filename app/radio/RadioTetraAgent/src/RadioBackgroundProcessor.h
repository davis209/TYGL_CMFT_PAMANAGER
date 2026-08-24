#ifndef RadioBackgroundProcessor_H
#define RadioBackgroundProcessor_H
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/RadioBackgroundProcessor.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * The RadioBackgroundProcessor is a class that allows for performing
  * RadioTasks in a side thread off the main line of execution.  This is to
  * enable, for instance, asynchonous processing of unsolicited events that
  * require further information from the TcpServer 
  */

#include "app/radio/RadioTetraAgent/src/RadioTask.h"

namespace TA_Base_Core
{
	class ThreadPoolSingletonManager;
}

namespace TA_IRS_App
{

    class Radio;

    class RadioBackgroundProcessor
    {
        public:
            /**
             * 
             */
            RadioBackgroundProcessor(Radio * radio);
            
            /**
             * 
             */
            virtual ~RadioBackgroundProcessor();
            
            
            /**
             * Adds the references from the given SubscriberActivityEvent to the task queue
             *
             * @param task a derived RadioTask as created by the RadioTaskFactory
             */
            void addTask(RadioTaskPtr task);

        private:

            Radio& m_radio;

            TA_Base_Core::ThreadPoolSingletonManager* m_threadPool;

    }; // class RadioBackgroundProcessor 
};  // namespace TA_IRS_App

#endif // #ifndef RadioBackgroundProcessor_H
