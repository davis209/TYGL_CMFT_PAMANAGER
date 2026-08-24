/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/SendCircuitDataEventReader.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Implementation of SendCircuitData Event Reader
  *
  */


#if !defined(SendCircuitDataEventReader_INCLUDED_)
#define SendCircuitDataEventReader_INCLUDED_

#include "core/types/src/ta_types.h"
#include "RadioEventReader.h"


namespace TA_IRS_App
{
    class SendCircuitDataEventReader : public RadioEventReader
    {
        public:

            SendCircuitDataEventReader(RadioEvent & event);
            virtual ~SendCircuitDataEventReader();

            ta_uint32 getCallReference() const;
            ta_uint32 getSentBytes() const;

        private:
            static const unsigned int SENDCIRCUITDATA_EVENT_LENGTH;
            static const unsigned int CALL_REFERENCE_START;
            static const unsigned int SENT_BYTES_START;
    };
};

#endif // !defined(SendCircuitDataEventReader_INCLUDED_)
