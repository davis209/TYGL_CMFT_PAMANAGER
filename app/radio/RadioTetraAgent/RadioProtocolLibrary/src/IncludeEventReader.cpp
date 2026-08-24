/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/IncludeEventReader.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Implementation of Join Event Reader
  *
  */
#include "core/types/src/ta_types.h"
#include "IncludeEventReader.h"

namespace TA_IRS_App
{

    IncludeEventReader::IncludeEventReader(RadioEvent& event)
           : RadioEventReader(event,EVENT_INCLUDE,INCLUDE_EVENT_LENGTH)
    {
    }

    IncludeEventReader::~IncludeEventReader()
    {
    }

    ta_uint32 IncludeEventReader::getCallReferenceSrc() const
    {
        return getParameterLong(CALL_REFERENCE_SRC_START);
    }

    ta_uint32 IncludeEventReader::getCallReferenceDst() const
    {
        return ENCODE_REFERENCE( getParameterLong(CALL_REFERENCE_DST_START) ); // limin++ CALL REFERENCE
    }

    const unsigned int IncludeEventReader::CALL_REFERENCE_SRC_START = 0;
    const unsigned int IncludeEventReader::CALL_REFERENCE_DST_START = 4;
    const unsigned int IncludeEventReader::INCLUDE_EVENT_LENGTH     = 8;

}

