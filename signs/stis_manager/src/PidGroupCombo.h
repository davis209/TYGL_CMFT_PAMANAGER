/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/stis_manager/src/PidGroupCombo.h $
 * @author:  Adam Radics
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 *
 * A custom combo for PID groups.
 */

#pragma once
#include "bus/mfc_extensions/src/coloured_controls/ColourCombo.h"
#include <string>
#include <vector>

namespace TA_IRS_App
{
    class PidSelectionManager;
}

namespace TA_IRS_App
{
    class PidGroupCombo : public TA_Base_Bus::ColourCombo
    {
    public:

        using BaseClass = TA_Base_Bus::ColourCombo;

        static const std::string HMIGroupName;

        PidGroupCombo() = default;

        /**
         * setPidSelectionManager
         *
         * Sets the object to tell when selection changes
         *
         * @param pidSelectionManager
         */
        void setPidSelectionManager(PidSelectionManager* pidSelectionManager);

        /**
         * updateGroupNames
         *
         * Updates the contents of this combo box with the
         * new vector of group names.
         *
         * @param groupNames   The new list of names.
         */
        void updateGroupNames(const std::vector<std::string>& groupNames);

        /**
         * getCurrentGroupName
         *
         * Gets the group name that is currently selected in the list
         *
         * @return The string that is in the combo box.
         */
        std::string getCurrentGroupName();

        /**
         * selectGroupName
         *
         * Sets the given group name  to be the selected group.
         *
         */
        void selectGroupName(std::string groupName);

        /**
         * groupNameIsAnExistingGroup
         *
         * Checks whether the given text is an existing group.
         *
         * This can be used for confirmation dialogs etc.
         *
         * @return Whether the text represents an existing group name.
         */
        bool groupNameIsAnExistingGroup(std::string groupName);

    public:

        // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(PidGroupCombo)
        virtual BOOL onSelectionChange();

    protected:

        virtual void PreSubclassWindow();
        //}}AFX_VIRTUAL

    protected:

        //{{AFX_MSG(PidGroupCombo)
        // NOTE - the ClassWizard will add and remove member functions here.
        //}}AFX_MSG

        DECLARE_MESSAGE_MAP()

    private:

        std::pair<std::string, int> get_current_selection();

        // passes events on
        PidSelectionManager* m_pidSelectionManager = nullptr;
    };
}
