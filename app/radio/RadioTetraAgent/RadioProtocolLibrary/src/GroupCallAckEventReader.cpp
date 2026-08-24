///////////////////////////////////////////////////////////
//
//  GroupCallAckEventReader.cpp
//  
///////////////////////////////////////////////////////////
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/GroupCallAckEventReader.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Implementation of the class GroupCallAckEventReader
  *
  */

#include "core/types/src/ta_types.h"
#include "app/radio/RadioTetraAgent/RadioProtocolLibrary/src/GroupCallAckEventReader.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_App
{

    GroupCallAckEventReader::GroupCallAckEventReader(RadioEvent& event)
    : RadioEventReader(event,EVENT_GROUPCALLACK)
    {
    }

    GroupCallAckEventReader::~GroupCallAckEventReader()
    {
    }

    CallReference GroupCallAckEventReader::getCallReference() const
    {
        return ENCODE_REFERENCE( getParameterLong(CALL_REFERENCE_POSITION) ); // limin++ CALL REFERENCE
    }

    ta_uint32 GroupCallAckEventReader::getNumberOfOperators() const
    {
        return getParameterLong(NUMBER_OF_OPERATORS_POSITION);
    }

    int GroupCallAckEventReader::getTsiVector(TSIVector & tsiVector) const
    {
        ta_uint32 stringLength = getParameterLong(TSI_VECTOR_STRLEN_POSITION);
        std::string currentTsi = "";

        for (ta_uint32 charPos = 0;charPos < stringLength;charPos++)
        {
			unsigned char currentChar = getParameterChar(TSI_VECTOR_STRING_POSITION + charPos);

            if ( (currentChar>0x7F) || (currentChar=='\0') )
            {
                // Dump these.  The former are obviously incorrect, but have
                // been observed in the wild
                continue;
            }
            if (currentChar == 0x0A)
            {
                if (!currentTsi.empty())
                {
                    tsiVector.push_back(currentTsi);
                    currentTsi = "";
                }
            }
            else
            {
                // Character
                currentTsi.append(1,currentChar);
            }
        } // for loop 

		// Ensure final tsi added
        if (!currentTsi.empty())
        {
            tsiVector.push_back(currentTsi);
        }

        return tsiVector.size();
    }

    const unsigned int GroupCallAckEventReader::CALL_REFERENCE_POSITION      = 0;
    const unsigned int GroupCallAckEventReader::NUMBER_OF_OPERATORS_POSITION = 4;
    const unsigned int GroupCallAckEventReader::TSI_VECTOR_STRLEN_POSITION   = 8;
    const unsigned int GroupCallAckEventReader::TSI_VECTOR_STRING_POSITION   = 12;

} // namespace TA_IRS_App


