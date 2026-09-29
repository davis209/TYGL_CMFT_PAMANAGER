/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 *
 * Source:    $File: //depot/4669_T01271350/4669/transactive/app/signs/stis_manager/src/TimeControlManager.cpp $
 * @author:   Adam Radics
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 *
 * This handles the time and priority controls
 * on the display page.
 *
 */

#include "StdAfx.h"
#include "TimeControlManager.h"
#include "UserMessages.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_App
{
    TimeControlManager::TimeControlManager(CallbackDateTimeCtrl& startDate,
                                           CallbackDateTimeCtrl& startTime,
                                           CallbackDateTimeCtrl& endDate,
                                           CallbackDateTimeCtrl& endTime)
        : m_startDate(startDate),
          m_startTime(startTime),
          m_endDate(endDate),
          m_endTime(endTime)
    {
        FUNCTION_ENTRY("TimeControlManager::TimeControlManager()");
        // do the initial GUI setup

        m_startDate.setCallback(this);
        m_startTime.setCallback(this);
        m_endDate.setCallback(this);
        m_endTime.setCallback(this);

        FUNCTION_EXIT;
    }

    TimeControlManager::~TimeControlManager()
    {
        m_startDate.setCallback(NULL);
        m_startTime.setCallback(NULL);
        m_endDate.setCallback(NULL);
        m_endTime.setCallback(NULL);
    }

    void TimeControlManager::dateTimeChanged(CallbackDateTimeCtrl* control)
    {
        // this is used to make sure the end time is never before the start time
        // and the end time hasnt passed

        // firstly, get the start, end, and current times
        CTime startTime = getStartDateTime();
        CTime endTime = getEndDateTime();
        CTime currentTime = CTime::GetCurrentTime();

        // if the start time is before the current time, set it to the current time
        if (startTime < currentTime)
        {
            startTime = currentTime;

            m_startDate.SetTime(&startTime);
            m_startTime.SetTime(&startTime);
        }

        // if the end time is before the start time, set it to the start time
        if (endTime < startTime)
        {
            endTime = startTime;

            m_endDate.SetTime(&endTime);
            m_endTime.SetTime(&endTime);
        }
    }

    CTime TimeControlManager::getStartDate() const
    {
        CTime startDate;
        m_startDate.GetTime(startDate);
        return startDate;
    }

    CTime TimeControlManager::getStartTime() const
    {
        CTime startTime;
        m_startTime.GetTime(startTime);
        return startTime;
    }

    std::pair<CTime, CTime> TimeControlManager::getStartDateTimePair() const
    {
        return {getStartDate(), getStartTime()};
    }

    CTime TimeControlManager::getStartDateTime() const
    {
        auto [d, t] = getStartDateTimePair();

        return
        {
            d.GetYear(),
            d.GetMonth(),
            d.GetDay(),
            t.GetHour(),
            t.GetMinute(),
            t.GetSecond()
        };
    }

    std::string TimeControlManager::getStartDateTimeString() const
    {
        return timeToString(getStartDateTime());
    }

    CTime TimeControlManager::getEndDate() const
    {
        CTime endDate;
        m_endDate.GetTime(endDate);
        return endDate;
    }

    CTime TimeControlManager::getEndTime() const
    {
        CTime endTime;
        m_endTime.GetTime(endTime);
        return endTime;
    }

    std::pair<CTime, CTime> TimeControlManager::getEndDateTimePair() const
    {
        return {getEndDate(), getEndTime()};
    }

    CTime TimeControlManager::getEndDateTime() const
    {
        auto [d, t] = getEndDateTimePair();

        return
        {
            d.GetYear(),
            d.GetMonth(),
            d.GetDay(),
            t.GetHour(),
            t.GetMinute(),
            t.GetSecond()
        };
    }

    std::string TimeControlManager::getEndDateTimeString() const
    {
        return timeToString(getEndDateTime());
    }

    bool TimeControlManager::isEnabled() const
    {
        return m_startDate.IsWindowEnabled() &&
               m_startTime.IsWindowEnabled() &&
               m_endDate.IsWindowEnabled() &&
               m_endTime.IsWindowEnabled();
    }

    void TimeControlManager::enable(bool b)
    {
        m_startDate.EnableWindow(b);
        m_startTime.EnableWindow(b);
        m_endDate.EnableWindow(b);
        m_endTime.EnableWindow(b);
    }

    void TimeControlManager::disable()
    {
        enable(false);
    }

    std::tuple<std::string, std::string, std::string> TimeControlManager::validate()
    {
        auto startDateTime = getStartDateTime();
        auto endDateTime = getEndDateTime();
        std::string errmsg;

        if (isEnabled())
        {
            auto current = CTime::GetCurrentTime();

            if (startDateTime > endDateTime)
            {
                errmsg = UserMessages::ERROR_START_TIME_AFTER_END;
            }
            else if (startDateTime == endDateTime)
            {
                errmsg = UserMessages::ERROR_START_TIME_EQUALS_END;
            }

            if (current > endDateTime)
            {
                errmsg = UserMessages::ERROR_END_TIME_PASSED;
            }
            else if (current > startDateTime)
            {
                errmsg = UserMessages::ERROR_START_TIME_BEFORE_CURRENT_TIME;
            }
        }

        return {timeToString(startDateTime), timeToString(endDateTime), errmsg};
    }

    std::string TimeControlManager::timeToString(const CTime& time)
    {
        std::stringstream ss;
        ss << time.GetYear();
        ss << std::setw(2) << std::setfill('0') << time.GetMonth();
        ss << std::setw(2) << std::setfill('0') << time.GetDay();
        ss << std::setw(2) << std::setfill('0') << time.GetHour();
        ss << std::setw(2) << std::setfill('0') << time.GetMinute();
        ss << std::setw(2) << std::setfill('0') << time.GetSecond();
        return ss.str();
    }

    std::string TimeControlManager::currentTimeString()
    {
        return timeToString(CTime::GetCurrentTime());
    }
}
