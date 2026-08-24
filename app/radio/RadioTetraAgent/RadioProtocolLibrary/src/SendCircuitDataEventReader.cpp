/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/SendCircuitDataEventReader.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Implementation of SendCircuitData Event Reader
  *
  */
#include "core/types/src/ta_types.h"
#include "SendCircuitDataEventReader.h"

namespace TA_IRS_App
{

    SendCircuitDataEventReader::SendCircuitDataEventReader(RadioEvent& event)
           : RadioEventReader(event,EVENT_SENDCIRCUITDATA,SENDCIRCUITDATA_EVENT_LENGTH)
    {
    }

    SendCircuitDataEventReader::~SendCircuitDataEventReader()
    {
    }

    ta_uint32 SendCircuitDataEventReader::getCallReference() const
    {
        return ENCODE_REFERENCE( getParameterLong(CALL_REFERENCE_START) ); // limin++ CALL REFERENCE
    }

    ta_uint32 SendCircuitDataEventReader::getSentBytes() const
    {
        return getParameterLong(SENT_BYTES_START);
    }

    const unsigned int SendCircuitDataEventReader::SENDCIRCUITDATA_EVENT_LENGTH     = 8;
    const unsigned int SendCircuitDataEventReader::CALL_REFERENCE_START             = 0;
    const unsigned int SendCircuitDataEventReader::SENT_BYTES_START                 = 4;
} // namespace TA_IRS_App


