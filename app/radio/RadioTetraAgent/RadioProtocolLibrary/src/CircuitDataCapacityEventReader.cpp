/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/CircuitDataCapacityEventReader.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Implementation of CircuitDataCapacity Event Reader
  *
  */
#include "core/types/src/ta_types.h"
#include "CircuitDataCapacityEventReader.h"

namespace TA_IRS_App
{

    CircuitDataCapacityEventReader::CircuitDataCapacityEventReader(RadioEvent& event)
           : RadioEventReader(event,EVENT_CIRCUITDATACAPACITY,CIRCUITDATACAPACITY_EVENT_LENGTH)
    {
    }

    CircuitDataCapacityEventReader::~CircuitDataCapacityEventReader()
    {
    }

    ta_uint32 CircuitDataCapacityEventReader::getCallReference() const
    {
        return ENCODE_REFERENCE( getParameterLong(CALL_REFERENCE_START) ); // limin++ CALL REFERENCE
    }

    ta_uint32 CircuitDataCapacityEventReader::getFreeBytes() const
    {
        return getParameterLong(FREE_BYTES_START);
    }

    const unsigned int CircuitDataCapacityEventReader::CIRCUITDATACAPACITY_EVENT_LENGTH     = 8;
    const unsigned int CircuitDataCapacityEventReader::CALL_REFERENCE_START             = 0;
    const unsigned int CircuitDataCapacityEventReader::FREE_BYTES_START                 = 4;
} // namespace TA_IRS_App


