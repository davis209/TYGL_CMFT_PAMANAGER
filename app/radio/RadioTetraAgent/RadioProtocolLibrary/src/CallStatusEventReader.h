/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/CallStatusEventReader.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Implementation of SubscriberActivity Event Reader
  *
  */


#if !defined(CallStatusEventReader_INCLUDED_)
#define CallStatusEventReader_INCLUDED_

#include "core/types/src/ta_types.h"
#include "RadioEventReader.h"
#include <vector>
#include <string>


namespace TA_IRS_App
{
    class CallStatusEventReader : public RadioEventReader
    {
        public:
            enum
            {
                INFO_CALL_QUEUED  = 1,
                INFO_CALL_NO_INFO = 3
            };


            CallStatusEventReader(RadioEvent & event);
            virtual ~CallStatusEventReader();

            ta_uint32 getCallReference() const;

            /**
             * Retrieves the Info field of the event
             */
            ta_uint32 getInfo() const;

            /**
             * Returns a useful description based on the info and status,
             * according to the description in the SICD.
             */
            std::string getDescription() const;

        private:
            static const unsigned int CALL_REFERENCE_POSITION;
            static const unsigned int INFO_POSITION;
    };
};

#endif // !defined(CallStatusEventReader_INCLUDED_)
