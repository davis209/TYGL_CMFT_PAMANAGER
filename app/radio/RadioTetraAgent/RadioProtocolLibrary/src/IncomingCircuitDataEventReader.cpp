/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/IncomingCircuitDataEventReader.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Implementation of IncomingCircuitData Event Reader
  *
  */
#include "core/types/src/ta_types.h"
#include "IncomingCircuitDataEventReader.h"

namespace TA_IRS_App
{

    IncomingCircuitDataEventReader::IncomingCircuitDataEventReader(RadioEvent& event)
           : RadioEventReader(event,EVENT_INCOMINGCIRCUITDATA)
    {
    }

    IncomingCircuitDataEventReader::~IncomingCircuitDataEventReader()
    {
    }

    ta_uint32 IncomingCircuitDataEventReader::getCallReference() const
    {
        return ENCODE_REFERENCE( getParameterLong(CALL_REFERENCE_START) ); // limin++ CALL REFERENCE
    }

    ta_uint32 IncomingCircuitDataEventReader::getData(std::vector<unsigned char> & data) const
    {
        ta_uint32 datalen = getParameterLong(DATA_LENGTH_START);
        for ( unsigned int i=0 ; i<datalen ; i++ )
        {
            data.push_back(getParameterChar(DATA_START + i));
        }
        return datalen;
    }

    const unsigned int IncomingCircuitDataEventReader::CALL_REFERENCE_START             = 0;
    const unsigned int IncomingCircuitDataEventReader::DATA_LENGTH_START                = 4;
    const unsigned int IncomingCircuitDataEventReader::DATA_START                       = 8;
} // namespace TA_IRS_App


