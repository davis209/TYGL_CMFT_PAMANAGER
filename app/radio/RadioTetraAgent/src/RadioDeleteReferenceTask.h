#ifndef RADIO_DELETE_REFERENCE_TASK_H
#define RADIO_DELETE_REFERENCE_TASK_H
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/RadioDeleteReferenceTask.h $
  * @author  R. van Hugten
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2008/11/28 16:26:01 $
  * Last modified by : $Author: builder $
  *
  * Declares an 
  *
  */

#include "core/types/src/ta_types.h"
#include "app/radio/RadioTetraAgent/src/RadioTask.h"
#include "app/radio/RadioTetraAgent/src/RadioTypes.h"
#include "app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodDeleteReference.h"

namespace TA_IRS_App
{
    class RadioTcpServer;

    class RadioDeleteReferenceTask
        : public RadioTask
    {
        public:
            /**
             * 
             */
            RadioDeleteReferenceTask(RadioTcpServer * radio, TA_IRS_App::SessionReference sessionRef, ta_uint32 delRef, RadioMethodDeleteReference::ReferenceType refType);


            /**
             *
             */
            ~RadioDeleteReferenceTask();

        protected:
                
            virtual void perform();

        private:

            ///
            ///
            TA_IRS_App::SessionReference m_sessionRef;

            ///
            ///
            ta_uint32 m_deleteRef;

            ///
            ///
            RadioMethodDeleteReference::ReferenceType m_refType;

    }; // class RadioDeleteReferenceTask
} // namespace TA_IRS_App

#endif // #ifndef RADIO_DELETE_REFERENCE_TASK_H
