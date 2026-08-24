/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/IncomingCircuitDataEventReader.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Implementation of IncomingCircuitData Event Reader
  *
  */


#if !defined(IncomingCircuitDataEventReader_INCLUDED_)
#define IncomingCircuitDataEventReader_INCLUDED_

#include "core/types/src/ta_types.h"
#include "RadioEventReader.h"
#include <vector>

namespace TA_IRS_App
{
    class IncomingCircuitDataEventReader : public RadioEventReader
    {
        public:

            IncomingCircuitDataEventReader(RadioEvent & event);
            virtual ~IncomingCircuitDataEventReader();

            ta_uint32 getCallReference() const;

            /**
             * Retrieve the data from the event
             *
             * @param data the vector of bytes to copy the data into
             * @return the number of bytes copied
             */
            ta_uint32 getData(std::vector<unsigned char> & data) const;


        private:
            static const unsigned int CALL_REFERENCE_START;
            static const unsigned int DATA_LENGTH_START;
            static const unsigned int DATA_START;
    };
};

#endif // !defined(IncomingCircuitDataEventReader_INCLUDED_)
