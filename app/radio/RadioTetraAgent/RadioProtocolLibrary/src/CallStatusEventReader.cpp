///////////////////////////////////////////////////////////
//
//  CallStatusEventReader.cpp
//  
///////////////////////////////////////////////////////////
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/CallStatusEventReader.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Implementation of the class CallStatusEventReader
  *
  */

#include "core/types/src/ta_types.h"
#include "CallStatusEventReader.h"
#include "RadioProtocol.h"
#include "RadioEvent.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_App
{

    CallStatusEventReader::CallStatusEventReader(RadioEvent& event)
    : RadioEventReader(event,EVENT_CALL_STATUS)
    {
    }

    CallStatusEventReader::~CallStatusEventReader()
    {
    }

    ta_uint32 CallStatusEventReader::getCallReference() const
    {
        return ENCODE_REFERENCE( getParameterLong(CALL_REFERENCE_POSITION) ); // limin++ CALL REFERENCE
    }

    ta_uint32 CallStatusEventReader::getInfo() const
    {
        return getParameterLong(INFO_POSITION);
    }

    std::string CallStatusEventReader::getDescription() const
    {
        ta_uint32 info = getInfo();
        ta_int32 status = getEvent().getStatus();
        std::string description = "";

        char status_str[16] = {0};
        sprintf(status_str,"%ld",status);
        char info_str[16] = {0};
        sprintf(info_str,"%lu",info);

        switch (status)
        {
            case STATUS_ALERTED:
                description = "The On/Off hook is in alerted state";
                break;
            case STATUS_QUEUED:
                description = "The call is queued on SCN";
                break;
            case STATUS_INTERCEPTED:
                description = "The call has been intercepted from an other application because the called number has limited access";
                break;
            case STATUS_ACCEPTED:
                description = "the application operator had accepted the request to continue the previously intercepted call";
                break;
            case STATUS_REJECTED:
                description = "The call was rejected";
                break;
            case STATUS_CHANGED:
                if (info == INFO_CALL_QUEUED)
                {
                    description = "The call is queued and ";
                }
                description += "One or more call parameters has been changed. Invoke a QueryReference to get further information.";
                break;
            default:
                description  = "ERROR: Call status of ";
                description += status_str;
                description += " not understood.";
                if (info <= 10)
                {
                    description += "\nMight be a WARNING_PREEMPTION (";
                    description += info_str;
                    description += " seconds left)";
                }
                break;
        }

        return description;
    }

    const unsigned int CallStatusEventReader::CALL_REFERENCE_POSITION      = 0;
    const unsigned int CallStatusEventReader::INFO_POSITION = 4;

}


