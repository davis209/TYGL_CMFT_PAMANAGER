/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_TIP/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/KeepAliveEventReader.cpp $
  * @author:  
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: $
  * Last modified by:  $Author: $
  *
  * Implementation of KeepAlive Event Reader
  *
  */

#include "core/types/src/ta_types.h"
#include "KeepAliveEventReader.h"

namespace TA_IRS_App
{

    KeepAliveEventReader::KeepAliveEventReader(RadioEvent& event)
           : RadioEventReader(event,EVENT_KEEPALIVE,KEEPALIVE_EVENT_LENGTH)
    {
    }

    KeepAliveEventReader::~KeepAliveEventReader()
    {
    }

	ta_uint32 KeepAliveEventReader::getEventCounter()
	{
		return getParameterLong(0);
	}

    const unsigned int KeepAliveEventReader::KEEPALIVE_EVENT_LENGTH   = 4;
} // namespace TA_IRS_App


