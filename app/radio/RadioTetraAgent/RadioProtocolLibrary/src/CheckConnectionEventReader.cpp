/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_TIP/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/CheckConnectionEventReader.cpp $
  * @author:  
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: $
  * Last modified by:  $Author: $
  *
  * Implementation of CheckConnection Event Reader
  *
  */

#include "CheckConnectionEventReader.h"

namespace TA_IRS_App
{

    CheckConnectionEventReader::CheckConnectionEventReader(RadioEvent& event)
           : RadioEventReader(event,EVENT_CHECKCONNECTION,CHECKCONNECTION_EVENT_LENGTH)
    {
    }

    CheckConnectionEventReader::~CheckConnectionEventReader()
    {
    }

    const unsigned int CheckConnectionEventReader::CHECKCONNECTION_EVENT_LENGTH   = 0;
} // namespace TA_IRS_App


