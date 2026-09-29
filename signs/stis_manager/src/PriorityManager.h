/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 *
 * Source:    $File: //depot/4669_T01271350/4669/transactive/app/signs/stis_manager/src/PriorityManager.h $
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
#include "bus/mfc_extensions/src/coloured_controls/ColourCombo.h"
#include <vector>

using TA_Base_Bus::ColourCombo;

namespace TA_IRS_App
{
    class PriorityManager
    {
    public:

        /**
         * PriorityManager
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
        PriorityManager(ColourCombo& priorityCombo);

        ~PriorityManager();

        /**
         * getPriority
         *
         * gets the priority from the drop down priority box
         *
         * @return the priority from 1 to 8
         */
        unsigned short getPriority() const;

        /**
         * setPriority
         *
         * Set the default priority and whether the user can change it or if its disabled.
         * The message type will change to cyclic/continuous based on the given priority.
         *
         * @param newPriority  The new priority
         * @param canChange    true if the priority selection should be enabled.
         *
         */
        void setPriority(unsigned short newPriority, bool canChange = true);
        void enablePriority(bool enable = true);

        /**
         * blankAndDisableTimeAndPriority
         *
         * This is called when nothing is selected on the pre-defined page.
         * It sets the message type to continuous, and blanks out the time controls.
         * It blanks out the priority and clears the priority box.
         *
         */
        void blankAndDisableTimeAndPriority();

        bool isEmergency() const;
        bool isNormal() const;

    private:

        PriorityManager();

        enum MessagePriorityType
        {
            NORMAL_PRIORITY_ONLY = 0,
            EMERGENCY_PRIORITY_ONLY = 1,
            ALL_PRIORITIES = 2
        };

        /**
         * setMessagePriorityType
         *
         * Set what priorities are available in the priority selection combo
         *
         * @param prioritiesEnabled    all, normal only, or emergency only.
         */
        void setMessagePriorityType(MessagePriorityType prioritiesEnabled);

        // The display attribute controls
        ColourCombo& m_priorityCombo;
    };
}
