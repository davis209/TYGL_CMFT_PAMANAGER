/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/MonitorSubscriberEventReader.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Implementation of MonitorSubscriber Event Reader
  *
  */
#include "core/types/src/ta_types.h"
#include "MonitorSubscriberEventReader.h"

#include <sstream>

namespace TA_IRS_App
{

    MonitorSubscriberEventReader::MonitorSubscriberEventReader(RadioEvent& event)
           : RadioEventReader(event,EVENT_MONITORSUBSCRIBER,MONITORSUBSCRIBER_EVENT_LENGTH)
    {
    }

    MonitorSubscriberEventReader::~MonitorSubscriberEventReader()
    {
    }

    ta_int32 MonitorSubscriberEventReader::getMonitorReference() const
    {
        return getParameterLong(MONITOR_REFERENCE_START);
    }


    std::string MonitorSubscriberEventReader::getSubscriberNumber() const
    {
        return getParameterShortString(SUBSCRIBER_NUMBER_START);
    }

    MonitorSubscriberEventReader::MonitorRejectReason MonitorSubscriberEventReader::getRejectReason() const
    {
        return (MonitorRejectReason)(getParameterChar(REJECT_REASON_START));
    }

    std::string MonitorSubscriberEventReader::getRejectReasonString() const
    {
        switch (getRejectReason())
        {
            case STATUS_OK:
                return "STATUS_OK";
            case RTT_KO_SERVER_BUSY:
                return "RTT_KO_SERVER_BUSY";
            case RTT_KO_INFRASTRUCTURE_PROBLEM:
                return "RTT_KO_INFRASTRUCTURE_PROBLEM";
            case RTT_KO_INVALID_REQUEST:
                return "RTT_KO_INVALID_REQUEST";
            case RTT_KO_NOT_AUTHORIZED:
                return "RTT_KO_NOT_AUTHORIZED";
            case RTT_KO_TIMEOUT:
                return "RTT_KO_TIMEOUT";
            case RTT_KO_REQUEST_NOT_SUPPORTED:
                return "RTT_KO_REQUEST_NOT_SUPPORTED";
            case RTT_KO_SERVER_INTERNAL_ERROR:
                return "RTT_KO_SERVER_INTERNAL_ERROR";
            case RTT_KO_RESERVED:
                return "RTT_KO_RESERVED";
            case RTT_SERVER_TOO_MANY_TRACE:
                return "RTT_SERVER_TOO_MANY_TRACE";
            case RTT_SERVER_TSI_NOT_RECOGNISED:
                return "RTT_SERVER_TSI_NOT_RECOGNISED";
            case RTT_TERMINATED_BY_CLIENT:
                return "RTT_TERMINATED_BY_CLIENT";
            default:
                std::ostringstream s;
                s << "UNDOCUMENTED (0x" << std::hex << (int)getRejectReason() << ")";
                return s.str();
        }

        return "UNDOCUMENTED";
    };


    const unsigned int MonitorSubscriberEventReader::SUBSCRIBER_NUMBER_START           =  0;
    const unsigned int MonitorSubscriberEventReader::MONITOR_REFERENCE_START           =  32;
    const unsigned int MonitorSubscriberEventReader::REJECT_REASON_START               =  36;
    const unsigned int MonitorSubscriberEventReader::MONITORSUBSCRIBER_EVENT_LENGTH    =  37;

}


