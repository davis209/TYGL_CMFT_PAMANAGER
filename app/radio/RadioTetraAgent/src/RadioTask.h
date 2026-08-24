#ifndef RADIO_TASK_H
#define RADIO_TASK_H
/**
  * The source code in this file is the property of
  * MI Services Group and is not for redistribution
  * in any form.
  *
  * Source : $Source: /cvs/1TS-9999-T/code/transactive/app/radio/RadioTetraAgent/src/Attic/RadioTask.h,v $
  * @author  R. van Hugten
  * @version $Revision: #1 $
  * Last modification : $Date: 2008/11/28 $
  * Last modified by : $Author: builder $
  *
  * Declares a base class for elements that can appear on the
  * RadioBackgroundProcessor request queue.
  *
  */
#include "core/threads/src/IWorkItem.h"

#include <string>

namespace TA_IRS_App
{
    class RadioTcpServer;

    class RadioTask : public TA_Base_Core::IWorkItem
    {
        public:
            /**
             * Creates the task with a pointer to the TCP server this
             * task is associated with
             */
            RadioTask(RadioTcpServer * radio);

            /**
             * Destructor
             */
            virtual ~RadioTask();

            virtual void executeWorkItem();
        protected:
            virtual void perform() = 0;

            /**
             * Gets a reference to the TCP server this task is associated with
             */
            RadioTcpServer & getRadio();

            /**
             * Gets a reference to the TCP server this task is associated with
             */
            const RadioTcpServer & getRadio() const;

        private:

            /** a reference to the TCP server this task is associated with */
            RadioTcpServer &     m_radio;
        protected:

            std::string m_taskName;

    }; // class RadioTask

    typedef boost::shared_ptr<RadioTask> RadioTaskPtr;

} // namespace TA_IRS_App

#endif // #ifndef RADIO_TASK_H
