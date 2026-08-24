/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/SubscriberActivityEventReader.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
///////////////////////////////////////////////////////////
//
//  SubscriberActivityEventReader.cpp
//  Implementation of the Class SubscriberActivityEventReader
//  
///////////////////////////////////////////////////////////

#include "app/radio/RadioTetraAgent/RadioProtocolLibrary/src/SubscriberActivityEventReader.h"

namespace TA_IRS_App
{

    SubscriberActivityEventReader::SubscriberActivityEventReader(RadioEvent& event)
           : RadioEventReader(event,EVENT_SUBSCRIBERACTIVITY,SUBSCRIBERACTIVITY_EVENT_LENGTH)
    {
    }

    SubscriberActivityEventReader::~SubscriberActivityEventReader()
    {
    }

    MonitorReference SubscriberActivityEventReader::getMonitorReference() const
    {
        return getParameterLong(MONITOR_REFERENCE_START);
    }

    CallReference SubscriberActivityEventReader::getCallReference() const
    {
        return ENCODE_REFERENCE( getParameterLong(CALL_REFERENCE_START) ); // limin++ CALL REFERENCE
    }

    std::string SubscriberActivityEventReader::getSubscriberNumber() const
    {
        return getParameterShortString(SUBSCRIBER_NUMBER_START);
    }

    bool SubscriberActivityEventReader::isCallStart() const
    {
        return (0 == getCallStatus());
    }

    bool SubscriberActivityEventReader::isCallEnd() const
    {
        return (1 == getCallStatus());
    }

    unsigned char SubscriberActivityEventReader::getCallStatus() const
    {
        return getParameterChar(CALL_STATUS_START);
    }

    const unsigned int SubscriberActivityEventReader::SUBSCRIBER_NUMBER_START           =  0;
    const unsigned int SubscriberActivityEventReader::MONITOR_REFERENCE_START           =  32;
    const unsigned int SubscriberActivityEventReader::CALL_REFERENCE_START              =  36;
    const unsigned int SubscriberActivityEventReader::CALL_STATUS_START                 =  40;
    const unsigned int SubscriberActivityEventReader::SUBSCRIBERACTIVITY_EVENT_LENGTH   =  41;

}


