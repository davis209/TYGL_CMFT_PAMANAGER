/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 *
 * Source:    $File: //depot/4669_T01271350/4669/transactive/app/signs/stis_manager/src/TimeControlManager.h $
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

#pragma once
#include "CallbackButton.h"
#include "CallbackDateTimeCtrl.h"
#include <vector>
#include <memory>

namespace TA_IRS_App
{
    class ITimeTypeListener;

    class TimeControlManager : public IDateTimeListener
    {
    public:

        /**
         * TimeControlManager
         *
         * Constructor
         *
         * @param priorityCombo        The priority combo box
         * @param startDate            The start date time control
         * @param startTime            The start time time control
         * @param endDate              The end date time control
         * @param endTime              The end time time control
         *
         */
        TimeControlManager(CallbackDateTimeCtrl& startDate,
                           CallbackDateTimeCtrl& startTime,
                           CallbackDateTimeCtrl& endDate,
                           CallbackDateTimeCtrl& endTime);

        ~TimeControlManager();

        /**
         * getStartDateTime
         *
         * gets the start time
         *
         * @return the start time.
         */
        CTime getStartDate() const;
        CTime getStartTime() const;
        std::pair<CTime, CTime> getStartDateTimePair() const;
        CTime getStartDateTime() const;
        std::string getStartDateTimeString() const;

        /**
         * getEndDateTime
         *
         * gets the end time
         *
         * @return the end time.
         */
        CTime getEndDate() const;
        CTime getEndTime() const;
        CTime getEndDateTime() const;
        std::pair<CTime, CTime> getEndDateTimePair() const;
        std::string getEndDateTimeString() const;

        /**
         * dateTimeChanged
         *
         * Gets called when a date/time is changed.
         *
         * @param control   the control that was changed
         */
        void dateTimeChanged(CallbackDateTimeCtrl* control) override;

        bool isEnabled() const;
        void enable(bool enable = true);
        void disable();

        std::tuple<std::string, std::string, std::string> validate();

        static std::string timeToString(const CTime& time);
        static std::string currentTimeString();

    private:

        TimeControlManager() = delete;

        CallbackDateTimeCtrl& m_startDate;
        CallbackDateTimeCtrl& m_startTime;
        CallbackDateTimeCtrl& m_endDate;
        CallbackDateTimeCtrl& m_endTime;
    };

    using TimeControlManagerPtr = std::shared_ptr<TimeControlManager>;
}
