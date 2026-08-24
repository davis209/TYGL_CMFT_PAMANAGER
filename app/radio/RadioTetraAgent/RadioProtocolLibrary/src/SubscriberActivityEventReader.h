/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/SubscriberActivityEventReader.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Implementation of SubscriberActivity Event Reader
  *
  */


#if !defined(SubscriberActivityEventReader_INCLUDED_)
#define SubscriberActivityEventReader_INCLUDED_

#include "app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioEventReader.h"
#include "app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioProtocol.h"

namespace TA_IRS_App
{
    class SubscriberActivityEventReader : public RadioEventReader
    {
        public:
            SubscriberActivityEventReader(RadioEvent & event);
            virtual ~SubscriberActivityEventReader();

            MonitorReference getMonitorReference() const;
            CallReference getCallReference() const;
            std::string getSubscriberNumber() const;
            bool isCallStart() const;
            bool isCallEnd() const;
            // DEPRECATED: use isCallEnd() or isCallStart() instead
            unsigned char getCallStatus() const;

        private:
            static const unsigned int SUBSCRIBERACTIVITY_EVENT_LENGTH;
            static const unsigned int SUBSCRIBER_NUMBER_START;
            static const unsigned int MONITOR_REFERENCE_START;
            static const unsigned int CALL_REFERENCE_START;
            static const unsigned int CALL_STATUS_START;
    };
};

#endif // !defined(SubscriberActivityEventReader_INCLUDED_)
