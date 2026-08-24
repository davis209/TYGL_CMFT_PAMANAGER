/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/CeaseTXEventReader.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class encapsulates the Radio API CeaseTXEventReader event.
  *
  */
///////////////////////////////////////////////////////////
//  CeaseTXEventReader.cpp
//  Implementation of the Class CeaseTXEventReader
//  Created on:      29-Nov-2003 09:57:09 AM
///////////////////////////////////////////////////////////

#include "core/types/src/ta_types.h"
#include "CeaseTXEventReader.h"
#include "RadioEvent.h"
#include "RadioProtocol.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_App
{
    const unsigned int CeaseTXEventReader::CALL_REFERENCE_POSITION = 0;

    CeaseTXEventReader::CeaseTXEventReader(RadioEvent& event) : 
    RadioEventReader(event,EVENT_CEASE_TX)
    {
    }


    CeaseTXEventReader::~CeaseTXEventReader()
    {
    }


    ta_uint32 CeaseTXEventReader::getCallReference()
    {
        return ENCODE_REFERENCE( getParameterLong(CALL_REFERENCE_POSITION) ); // limin++ CALL REFERENCE
    }

}

