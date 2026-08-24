/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/ChangeReferenceEventReader.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class encapsulates the Radio API SendSDS method.
  *
  */
///////////////////////////////////////////////////////////
//  ChangeReferenceEventReader.cpp
//  Implementation of the Class ChangeReferenceEventReader
//  Created on:      29-Nov-2003 09:57:09 AM
///////////////////////////////////////////////////////////

#include "core/types/src/ta_types.h"
#include "ChangeReferenceEventReader.h"
#include "RadioEvent.h"
#include "RadioProtocol.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_App
{
    const unsigned int ChangeReferenceEventReader::SUBSCRIBER_REFERENCE_POSITION    = 0;
    const unsigned int ChangeReferenceEventReader::DETAILS_PARAMETER_START_POSITION = 4;
    const unsigned int ChangeReferenceEventReader::DETAILS_STRING_START_POSITION    = 8;

    ChangeReferenceEventReader::ChangeReferenceEventReader(RadioEvent& event) : 
    RadioEventReader(event,EVENT_CHANGE_REFERENCE)
    {
    }



    ChangeReferenceEventReader::~ChangeReferenceEventReader()
    {
    }


    ta_uint32 ChangeReferenceEventReader::getCallReference()
    {
        return ENCODE_REFERENCE( getParameterLong(SUBSCRIBER_REFERENCE_POSITION) ); // limin++ CALL REFERENCE
    }

    std::string ChangeReferenceEventReader::getFailedKeys()
    {
        // Get the length of the string
        ta_uint32 stringLength = getParameterLong(DETAILS_PARAMETER_START_POSITION);

        // Set up a string to place the characters in.
        std::string failedKeysString("");

        for (unsigned int i = 0;i < stringLength;i++)
        {
            failedKeysString += getParameterChar(DETAILS_STRING_START_POSITION + i);
        }

        return failedKeysString;
    }
}

