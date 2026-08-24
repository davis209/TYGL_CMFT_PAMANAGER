/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/AttachToGroupEventReader.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Implementation of AttachToGroup Event Reader
  *
  */
#include "AttachToGroupEventReader.h"

namespace TA_IRS_App
{

    AttachToGroupEventReader::AttachToGroupEventReader(RadioEvent& event)
           : RadioEventReader(event,EVENT_ATTACHTOGROUP,ATTACHTOGROUP_EVENT_LENGTH)
    {
    }

    std::string AttachToGroupEventReader::getGTSI() const
    {
        return getParameterShortString(GROUP_TSI_START);
    }

    const unsigned int AttachToGroupEventReader::GROUP_TSI_START            =  0;
    const unsigned int AttachToGroupEventReader::ATTACHTOGROUP_EVENT_LENGTH = 32;

}


