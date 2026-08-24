/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/SystemErrorEventReader.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Implementation of JoinEventReader
  *
  */
#include "SystemErrorEventReader.h"

namespace TA_IRS_App
{

    SystemErrorEventReader::SystemErrorEventReader(RadioEvent& event)
           : RadioEventReader(event,EVENT_SYSTEM_ERROR,SYSTEM_ERROR_EVENT_LENGTH)
    {
    }

    SystemErrorEventReader::~SystemErrorEventReader()
    {
    }


    int SystemErrorEventReader::getYear() const
    {
        return getParameterShort(TIMESTAMP_YEAR_START_POS);
    }

    int SystemErrorEventReader::getMonth() const
    {
        return getParameterChar(TIMESTAMP_MONTH_START_POS);
    }

    int SystemErrorEventReader::getDay() const
    {
        return getParameterChar(TIMESTAMP_DAY_START_POS);
    }

    int SystemErrorEventReader::getHour() const
    {
        return getParameterChar(TIMESTAMP_HOUR_START_POS);
    }

    int SystemErrorEventReader::getMinute() const
    {
        return getParameterChar(TIMESTAMP_MINUTE_START_POS);
    }

    int SystemErrorEventReader::getSecond() const
    {
        return getParameterChar(TIMESTAMP_SECOND_START_POS);
    }

    int SystemErrorEventReader::getMillisecond() const
    {
        return getParameterShort(TIMESTAMP_MSEC_START_POS);
    }

    std::string SystemErrorEventReader::getTimestamp() const
    {
        char buff[99] = {0};
        int yr = getYear();
        int mo = getMonth();
        int dy = getDay();
        int hr = getHour();
        int mi = getMinute();
        int se = getSecond();
        int ms = getMillisecond();

        // format is "YYYY/MM/DD HH:MM:SS.MSEC"
        sprintf(buff, "%.4d/%0.2d/%0.2d %0.2d:%0.2d:%0.2d.%d", yr,mo,dy,hr,mi,se,ms);

        std::string retval = buff;

        return retval;
    }


    const unsigned int SystemErrorEventReader::SYSTEM_ERROR_EVENT_LENGTH    = 9;
    const unsigned int SystemErrorEventReader::TIMESTAMP_YEAR_START_POS     = 0;
    const unsigned int SystemErrorEventReader::TIMESTAMP_MONTH_START_POS    = 2;
    const unsigned int SystemErrorEventReader::TIMESTAMP_DAY_START_POS      = 3;
    const unsigned int SystemErrorEventReader::TIMESTAMP_HOUR_START_POS     = 4;
    const unsigned int SystemErrorEventReader::TIMESTAMP_MINUTE_START_POS   = 5;
    const unsigned int SystemErrorEventReader::TIMESTAMP_SECOND_START_POS   = 6;
    const unsigned int SystemErrorEventReader::TIMESTAMP_MSEC_START_POS     = 7;

}

