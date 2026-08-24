/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_TIP/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/DetachSessionEventReader.cpp $
  * @author:  
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: $
  * Last modified by:  $Author: $
  *
  * Implementation of DetachSession Event Reader
  *
  */

#include "app/radio/RadioTetraAgent/RadioProtocolLibrary/src/DetachSessionEventReader.h"

namespace TA_IRS_App
{

    const unsigned int DetachSessionEventReader::DETACHSESSION_EVENT_LENGTH     =  0;

    DetachSessionEventReader::DetachSessionEventReader(RadioEvent& event)
    : RadioEventReader(event,EVENT_DETACH_SESSION) // ,DETACHSESSION_EVENT_LENGTH)
    {
    }

    DetachSessionEventReader::~DetachSessionEventReader()
    {
    }
}


