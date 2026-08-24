/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/MonitorSubscriberEventReader.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Implementation of MonitorSubscriber Event Reader
  *
  */


#if !defined(MonitorSubscriberEventReader_INCLUDED_)
#define MonitorSubscriberEventReader_INCLUDED_

#include "core/types/src/ta_types.h"
#include "RadioEventReader.h"


namespace TA_IRS_App
{
    class MonitorSubscriberEventReader : public RadioEventReader
    {
        public:

            MonitorSubscriberEventReader(RadioEvent & event);
            virtual ~MonitorSubscriberEventReader();

            ta_int32 getMonitorReference() const;
            std::string getSubscriberNumber() const;

            enum MonitorRejectReason
            {
                RTT_KO_SERVER_BUSY = 1,
                RTT_KO_INFRASTRUCTURE_PROBLEM = 2,
                RTT_KO_INVALID_REQUEST = 3,
                RTT_KO_NOT_AUTHORIZED = 4,
                RTT_KO_TIMEOUT = 5,
                RTT_KO_REQUEST_NOT_SUPPORTED = 6,
                RTT_KO_SERVER_INTERNAL_ERROR = 7,
                RTT_KO_RESERVED = 8,
                RTT_SERVER_TOO_MANY_TRACE = 9,
                RTT_SERVER_TSI_NOT_RECOGNISED = 10,
                RTT_TERMINATED_BY_CLIENT = 11
            };

            MonitorRejectReason getRejectReason() const;

            std::string getRejectReasonString() const;

        private:
            static const unsigned int MONITORSUBSCRIBER_EVENT_LENGTH;
            static const unsigned int SUBSCRIBER_NUMBER_START;
            static const unsigned int MONITOR_REFERENCE_START;
            static const unsigned int REJECT_REASON_START;
    };
};

#endif // !defined(MonitorSubscriberEventReader_INCLUDED_)
