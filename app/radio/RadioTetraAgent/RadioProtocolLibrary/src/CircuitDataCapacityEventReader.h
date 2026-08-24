/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/CircuitDataCapacityEventReader.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Implementation of CircuitDataCapacity Event Reader
  *
  */


#if !defined(CircuitDataCapacityEventReader_INCLUDED_)
#define CircuitDataCapacityEventReader_INCLUDED_

#include "core/types/src/ta_types.h"
#include "RadioEventReader.h"


namespace TA_IRS_App
{
    class CircuitDataCapacityEventReader : public RadioEventReader
    {
        public:

            CircuitDataCapacityEventReader(RadioEvent & event);
            virtual ~CircuitDataCapacityEventReader();

            ta_uint32 getCallReference() const;
            ta_uint32 getFreeBytes() const;

        private:
            static const unsigned int CIRCUITDATACAPACITY_EVENT_LENGTH;
            static const unsigned int CALL_REFERENCE_START;
            static const unsigned int FREE_BYTES_START;
    };
};

#endif // !defined(CircuitDataCapacityEventReader_INCLUDED_)
