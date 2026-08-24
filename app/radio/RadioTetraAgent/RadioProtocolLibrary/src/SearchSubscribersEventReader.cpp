/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/SearchSubscribersEventReader.cpp $
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
//  SearchSubscribersEventReader.cpp
//  Implementation of the Class SearchSubscribersEventReader
//  Created on:      29-Nov-2003 09:57:09 AM
///////////////////////////////////////////////////////////

#include "core/types/src/ta_types.h"
#include "SearchSubscribersEventReader.h"
#include "RadioEvent.h"
#include "RadioProtocol.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_App
{
    const unsigned int SearchSubscribersEventReader::SUBSCRIBER_REFERENCE_RESULT_PARAMETER_POSITION        =   0;

    SearchSubscribersEventReader::SearchSubscribersEventReader(RadioEvent& event) : 
    RadioEventReader(event,EVENT_SEARCH_SUBSCRIBER)
    {
    }



    SearchSubscribersEventReader::~SearchSubscribersEventReader()
    {
    }


    const ta_uint32 SearchSubscribersEventReader::getSubscriberReferenceResultSet()
    {
        return getParameterLong(SUBSCRIBER_REFERENCE_RESULT_PARAMETER_POSITION);
    }
}

