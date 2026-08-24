/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/ConvertToDBTimeEventReader.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Implementation of Convert to DB Time Event Reader
  *
  */

#include "core/types/src/ta_types.h"
#include "app/radio/RadioTetraAgent/RadioProtocolLibrary/src/ConvertToDBTimeEventReader.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_App
{

    const unsigned int ConvertToDBTimeEventReader::DATETIME_START_POSITION          =  0;


    ConvertToDBTimeEventReader::ConvertToDBTimeEventReader(RadioEvent& event)
						    : RadioEventReader(event,EVENT_CONVERT_TO_DB_TIME)
    {
    }

    ConvertToDBTimeEventReader::~ConvertToDBTimeEventReader()
    {
    }


    const std::string ConvertToDBTimeEventReader::getDBTimeString() const
    {
        // Get the length of the string
        ta_uint32 stringLength = getParameterLong(DATETIME_START_POSITION);

        // Set up a string to place the characters in.
        std::string dateTimeString("");

        for (unsigned int i = 0;i < stringLength;i++)
        {
            unsigned char dateTimeUChar = getParameterChar(DATETIME_START_POSITION + 4 + i);
            // need to not include the trailing zero
            if ( (i>=0) && (dateTimeUChar>0) )
            {
                dateTimeString += getParameterChar(DATETIME_START_POSITION + 4 + i);
            }
        }

        return dateTimeString;
    }
}


