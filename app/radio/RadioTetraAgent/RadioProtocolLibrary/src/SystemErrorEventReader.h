/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/SystemErrorEventReader.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Implementation of Join Event Reader
  *
  */


#if !defined(SystemErrorEventReader_INCLUDED_)
#define SystemErrorEventReader_INCLUDED_

#include "RadioEventReader.h"


namespace TA_IRS_App
{
    class SystemErrorEventReader : public RadioEventReader
    {
        public:

            SystemErrorEventReader(RadioEvent & event);
            virtual ~SystemErrorEventReader();

            int getYear() const;
            int getMonth() const;
            int getDay() const;
            int getHour() const;
            int getMinute() const;
            int getSecond() const;
            int getMillisecond() const;

            //
            // Just get the timestamp string. TODO: specify format string
            std::string getTimestamp() const; // (const char * strftimeFormat="%Y%m%d%H%M%S");

        private:
            static const unsigned int SYSTEM_ERROR_EVENT_LENGTH;
            static const unsigned int TIMESTAMP_YEAR_START_POS;
            static const unsigned int TIMESTAMP_MONTH_START_POS;
            static const unsigned int TIMESTAMP_DAY_START_POS;
            static const unsigned int TIMESTAMP_HOUR_START_POS;
            static const unsigned int TIMESTAMP_MINUTE_START_POS;
            static const unsigned int TIMESTAMP_SECOND_START_POS;
            static const unsigned int TIMESTAMP_MSEC_START_POS;
    };
};

#endif // !defined(SystemErrorEventReader_INCLUDED_)
