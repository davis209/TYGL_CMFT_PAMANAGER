#ifndef RADIO_SUBSCRIBER_ACTIVITY_TASK_H
#define RADIO_SUBSCRIBER_ACTIVITY_TASK_H
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/RadioSubscriberActivityTask.h $
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
#include "app/radio/RadioTetraAgent/src/RadioTypes.h"
#include "app/radio/RadioTetraAgent/src/RadioTask.h"

namespace TA_IRS_App
{
    class RadioTcpServer;
    class RadioSubscriberMonitor;

    class RadioSubscriberActivityTask
        : public RadioTask
    {
        public:
            /**
             *
             */
            RadioSubscriberActivityTask
                    (   RadioTcpServer * radio,
                        RadioSubscriberMonitor & subscriberMonitor,
                        ta_uint32 sessRef,
                        ta_uint32 callRef,
                        ta_uint32 monitorRef,
                        bool isCallStart);

            /**
             *
             */
            ~RadioSubscriberActivityTask();

        protected:
                
            virtual void perform();
            

        private:

            ///
            ///
            RadioSubscriberMonitor & m_subscriberMonitor;

            ///
            ///
            ta_uint32 m_sessionReference;

            ///
            ///
            ta_uint32 m_callReference;

            ///
            ///
            ta_uint32 m_monitorReference;

            ///
            ///
            bool m_isCallStart;
    }; // class RadioSubscriberActivityTask
} // namespace TA_IRS_App

#endif // #ifndef RADIO_SUBSCRIBER_ACTIVITY_TASK_H
