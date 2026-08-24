/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodSearchSubscribers.cpp $
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
//  RadioMethodSearchSubscribers.cpp
//  Implementation of the Class RadioMethodSearchSubscribers
//  Created on:      29-Nov-2003 09:57:09 AM
///////////////////////////////////////////////////////////

#include "RadioMethodSearchSubscribers.h"
#include "RadioProtocol.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_App
{
    const unsigned int RadioMethodSearchSubscribers::SEARCH_SUBSCRIBERS_BASE_LENGTH    = 1;
    const unsigned int RadioMethodSearchSubscribers::VIEW_PARAMETER_POSITION           = 0;
    const unsigned int RadioMethodSearchSubscribers::CRITEREA_PARAMETER_START_POSITION = 1;

    RadioMethodSearchSubscribers::RadioMethodSearchSubscribers(RadioSessionReference sessionRef) 
                       : RadioMethod(SEARCH_SUBSCRIBERS_BASE_LENGTH,METHOD_SEARCH_SUBSCRIBER,sessionRef)
    {
    }



    RadioMethodSearchSubscribers::~RadioMethodSearchSubscribers()
    {
    }


    void RadioMethodSearchSubscribers::setSearchType(const EView view)
    {
        // Determine if forwarding has been enabled or disabled
        setParameterByte(VIEW_PARAMETER_POSITION,view);
    }

    void RadioMethodSearchSubscribers::setCriteria(const std::string& criteria)
    {
		std::string localCriteria(criteria);
		
		// Always ensure criteria at terminated by "/n" else the radio
		// will get upset and make up some additional random characters
		// to the end of your method
		if ((localCriteria.size()==0)||(localCriteria.at(localCriteria.size()-1)!='\n'))
		{
			localCriteria += "\n";
		}

        // Resize the message - must add 4 for the "length" portion of the string
        setMessageSize(SEARCH_SUBSCRIBERS_BASE_LENGTH + 4 + localCriteria.size());

        // And add the string
        setParameterLongString(CRITEREA_PARAMETER_START_POSITION,localCriteria);
    }

    unsigned char RadioMethodSearchSubscribers::getSerialiseTransactionLevel() const
    {
        return SERIALISATION_LEVEL_2;
    }
}

