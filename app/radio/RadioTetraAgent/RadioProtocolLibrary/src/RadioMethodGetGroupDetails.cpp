/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodGetGroupDetails.cpp $
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
//  RadioMethodGetGroupDetails.cpp
//  Implementation of the Class RadioMethodGetGroupDetails
//  Created on:      29-Nov-2003 09:57:09 AM
///////////////////////////////////////////////////////////

#include "core/types/src/ta_types.h"
#include "RadioMethodGetGroupDetails.h"
#include "RadioProtocol.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "app/radio/RadioTetraAgent/src/CallForwardingDetails.h"

namespace TA_IRS_App
{
    const unsigned int RadioMethodGetGroupDetails::GET_GROUP_DETAILS_BASE_LENGTH     = 8;
    const unsigned int RadioMethodGetGroupDetails::SUBSCRIBER_REFERENCE_PARAMETER_POSITION = 0;
    const unsigned int RadioMethodGetGroupDetails::ROW_COUNT_PARAMETER_POSITION = 4;
    const unsigned int RadioMethodGetGroupDetails::FIELDS_PARAMETER_START_POSITION = 8;

    const unsigned int RadioMethodGetGroupDetails::MAX_ROWS_PER_EVENT = 20;

    RadioMethodGetGroupDetails::RadioMethodGetGroupDetails(RadioSessionReference sessionRef, ta_uint32 subscriberRef)
                       : RadioMethod(GET_GROUP_DETAILS_BASE_LENGTH,METHOD_GET_GROUP_DETAILS,
                       sessionRef)
    {
        setParameterLong(SUBSCRIBER_REFERENCE_PARAMETER_POSITION,subscriberRef);
        setParameterLong(ROW_COUNT_PARAMETER_POSITION,MAX_ROWS_PER_EVENT);
    }
    enum EGetGroupDetailsFieldTerminationStrategy
    {
        newline,
        forcednull,
        nothing
    };


    EGetGroupDetailsFieldTerminationStrategy getFieldTerminationStrategy()
    {
        if ( TA_Base_Core::RunParams::getInstance().isSet("GetGroupDetailsFieldTerminationStrategy") )
        {
            std::string strategystr = TA_Base_Core::RunParams::getInstance().get("GetGroupDetailsFieldTerminationStrategy");
            if ( strategystr == "newline" )
            {
                return newline;
            }
            else if ( strategystr == "forcednull" )
            {
                return forcednull;
            }
            else if ( strategystr == "nothing" )
            {
                return nothing;
            }
            else
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                        "Invalid value for runparam GetGroupDetailsFieldTerminationStrategy, defaulting to \"forcednull\"");
            }
        }

        // else
        return forcednull;
    }


    void RadioMethodGetGroupDetails::setFields(std::vector<std::string> fields)
    {
        std::string fieldsString;
        for (unsigned int i = 0;i < fields.size();i++)
        {
            fieldsString += fields[i];

            if (i != (fields.size() - 1))
            {
                fieldsString += ",";
            }
        }
		// RvH 20050128 pre-Radio iFAT
		// OTE TCPServer/HLR bug means that it isn't so happy about the presence
		// of trailing newlines, nor the absence of a terminating null character
		// fieldsString += "\n";
        // TD16883 oh thats real clever, adding a trailing null. The internal
        // c-string already has one of those, so adding an extra one doesn't
        // help - basic_string::size() ignores/swallows it.
        // For experimentation, the runparam GetGroupDetailsFieldTerminationStrategy
        // is used. To experiment, use
        //
        // RadioTetraAgent --get-group-details-field-termination-strategy=<value>
        //
        // where value can be one of
        // newline  - this is as per TCPSrv Interface Spec 7.3.2.4 . The singular
        //            reason this isn't the hardcoded strategy is that during the
        //            Feb 2005 Radio iFAT in Singapore, it was found that a but in
        //            the TcpSrv/HLR meant that such newlines were actually unacceptable.
        //            Nevertheless, we can never tell when such bugs are fixed,
        //            because we aren't told.
        // forcednull-This is as per advice from Paolo Ramella during that iFAT,
        //            which he in turn obtained from the developers in Italy.
        // nothing  - current strategy. Adds nothing to the end. Seems to work
        //            most of the time, but irritatingly fails (returns -22)
        //            intermittently for no apparent reasons.
        //
        switch ( getFieldTerminationStrategy() )
        {
            case newline:
                {
                    fieldsString += "\n";
                    setMessageSize(GET_GROUP_DETAILS_BASE_LENGTH+fieldsString.size()+4);
                    setParameterLongString(FIELDS_PARAMETER_START_POSITION,fieldsString);
                    break;
                }
            case nothing:
                {
                    setMessageSize(GET_GROUP_DETAILS_BASE_LENGTH+fieldsString.size()+4);
                    setParameterLongString(FIELDS_PARAMETER_START_POSITION,fieldsString);
                    break;
                }
            case forcednull:
            default:
                {
                    // +1 for the null, +4 for the length DWORD
                    setMessageSize(GET_GROUP_DETAILS_BASE_LENGTH+fieldsString.size()+1+4);
                    setParameterLongString(FIELDS_PARAMETER_START_POSITION,fieldsString,true);
                }
        }
    }



    RadioMethodGetGroupDetails::~RadioMethodGetGroupDetails()
    {
    }

    unsigned char RadioMethodGetGroupDetails::getSerialiseTransactionLevel() const
    {
        return SERIALISATION_LEVEL_2;
    }

}

