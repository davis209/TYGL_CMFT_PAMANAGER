#ifndef RadioGroupCallAckTask_H
#define RadioGroupCallAckTask_H
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/RadioGroupCallAckTask.h $
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
// for TSIVecor typedef
#include "app/radio/RadioTetraAgent/RadioProtocolLibrary/src/GroupCallAckEventReader.h"

namespace TA_IRS_App
{
    class RadioTcpServer;
    class IRadioCallback;

    class RadioGroupCallAckTask
        : public RadioTask
    {
        public:
            /**
             * Constructor to add the call to the stack
             */
            RadioGroupCallAckTask
                    (   RadioTcpServer * radio,
                        const TA_IRS_App::CallDetailsType& callDetails,
                        const TA_IRS_App::GroupCallAckEventReader & eventReader);


			RadioGroupCallAckTask
					(   RadioTcpServer * radio,
						const TA_IRS_App::CallDetailsType& callDetails,
						const TSIVector & tsiVector);

        protected:
                
            virtual void perform();
            

        private:

            ///
            ///
            TA_IRS_App::CallDetailsType m_callDetails;

            ///
            ///
            std::vector< std::string > m_tsiVector;
    }; // class RadioGroupCallAckTask
} // namespace TA_IRS_App

#endif // #ifndef RadioGroupCallAckTask_H
