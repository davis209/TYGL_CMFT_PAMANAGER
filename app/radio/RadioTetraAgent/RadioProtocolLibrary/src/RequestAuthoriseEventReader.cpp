/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RequestAuthoriseEventReader.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class encapsulates the Radio API RequestAuthorise event.
  *
  */
///////////////////////////////////////////////////////////
//  RequestAuthoriseEventReader.cpp
//  Implementation of the Class RequestAuthoriseEventReader
//  Created on:      29-Nov-2003 09:57:09 AM
///////////////////////////////////////////////////////////

#include "core/types/src/ta_types.h"
#include "RequestAuthoriseEventReader.h"
#include "RadioEvent.h"
#include "RadioProtocol.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_App
{
    const unsigned int RequestAuthoriseEventReader::CAD_REFERENCE_POSITION = 0;
    const unsigned int RequestAuthoriseEventReader::CAD_REASON_POSITION    = 4;

    RequestAuthoriseEventReader::RequestAuthoriseEventReader(RadioEvent& event) : 
                RadioEventReader(event,EVENT_REQUEST_AUTHORISE)
    {
    }


    RequestAuthoriseEventReader::~RequestAuthoriseEventReader()
    {
    }


    ta_uint32 RequestAuthoriseEventReader::getCADReference()
    {
        return getParameterLong(CAD_REFERENCE_POSITION);
    }

    ta_uint32 RequestAuthoriseEventReader::getCADReason()
    {
        return getParameterLong(CAD_REASON_POSITION);
    }

}

