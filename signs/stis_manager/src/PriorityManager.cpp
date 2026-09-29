/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 *
 * Source:    $File: //depot/4669_T01271350/4669/transactive/app/signs/stis_manager/src/PriorityManager.cpp $
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
#include "PriorityManager.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_App
{
    // This is the biggest repeat interval
    //static const unsigned short MAX_REPEAT_INTERVAL = 999;

    static const DWORD COLOR_RED = RGB(255, 0, 0);

    using TA_Base_Core::DebugUtil;

    PriorityManager::PriorityManager(ColourCombo& priorityCombo)
        : m_priorityCombo(priorityCombo)
    {
        FUNCTION_ENTRY("PriorityManager::PriorityManager()");
        // do the initial GUI setup

        // emergency priorities are red
        m_priorityCombo.mapItemDataToColour(1, COLOR_RED);
        m_priorityCombo.mapItemDataToColour(2, COLOR_RED);
        m_priorityCombo.mapItemDataToColour(3, COLOR_RED);

        // blank everything out to start
        blankAndDisableTimeAndPriority();

        // set the priority to normal only
        setMessagePriorityType(ALL_PRIORITIES);

        FUNCTION_EXIT;
    }

    PriorityManager::~PriorityManager()
    {
    }

    void PriorityManager::setPriority(unsigned short newPriority, bool canChange /* = true */)
    {
        // enable the control only if the priority can be changed
        m_priorityCombo.EnableWindow(canChange);

        int selectionIndex = 0;
        // 0 to 3 emergency . continuous
        /*if ( (newPriority > 0) && (newPriority < 4) )
        {
            selectionIndex = newPriority - 1;
        }
        // 4 to 6 normal . cyclic
        else if ( (newPriority > 3) && (newPriority < 6) )
        {
            selectionIndex = newPriority - 4;
        }
        else if ( newPriority == 6 )
        {
            selectionIndex = newPriority - 1;
        }
        // 7 to 8 normal . cyclic
        else if ( (newPriority > 6) && (newPriority < 9) )
        {
            selectionIndex = newPriority - 5;
        }*/

        if ((newPriority > 0) && (newPriority < 9))
        {
            selectionIndex = newPriority - 1;
        }
        // invalid priority
        else
        {
            // log the error
            LOG_ERROR("Invalid priority given: %d. Check the pre-defined message library, and default TTIS attributes.", newPriority);

            // leave it alone
            return;
        }

        // set the priority
        m_priorityCombo.SetCurSel(selectionIndex);
    }

    void PriorityManager::enablePriority(bool enable)
    {
        m_priorityCombo.EnableWindow(enable);
    }

    void PriorityManager::blankAndDisableTimeAndPriority()
    {
        // select continuous
        //setPriority(1, false);

        // clear priority
        //m_priorityCombo.SetCurSel(-1);
    }

    unsigned short PriorityManager::getPriority() const
    {
        int selected = m_priorityCombo.GetCurSel();
        return static_cast<unsigned short>(m_priorityCombo.GetItemData(selected));
    }

    void PriorityManager::setMessagePriorityType(MessagePriorityType prioritiesEnabled)
    {
        // try to restore the currently selected priority
        // if the new selection has that priotity in it
        int selected = m_priorityCombo.GetCurSel();
        CString selectedString;

        if (selected != CB_ERR)
        {
            m_priorityCombo.GetLBText(selected, selectedString);
        }

        // empty the list
        m_priorityCombo.ResetContent();

        switch (prioritiesEnabled)
        {
            case NORMAL_PRIORITY_ONLY:
                m_priorityCombo.AddString("´¶³q - 4");
                m_priorityCombo.AddString("´¶³q - 5");
                m_priorityCombo.AddString("´¶³q - 6");
                m_priorityCombo.AddString("´¶³q - 7");
                m_priorityCombo.AddString("´¶³q - 8");

                m_priorityCombo.SetItemData(0, 4);
                m_priorityCombo.SetItemData(1, 5);
                m_priorityCombo.SetItemData(2, 7);
                m_priorityCombo.SetItemData(3, 8);
                break;

            case EMERGENCY_PRIORITY_ONLY:
                m_priorityCombo.AddString("ºò«æ - 1");
                m_priorityCombo.AddString("ºò«æ - 2");
                m_priorityCombo.AddString("ºò«æ - 3");

                m_priorityCombo.SetItemData(0, 1);
                m_priorityCombo.SetItemData(1, 2);
                m_priorityCombo.SetItemData(2, 3);
                break;

            case ALL_PRIORITIES:
                m_priorityCombo.AddString("ºò«æ - 1");
                m_priorityCombo.AddString("ºò«æ - 2");
                m_priorityCombo.AddString("ºò«æ - 3");
                m_priorityCombo.AddString("´¶³q - 4");
                m_priorityCombo.AddString("´¶³q - 5");
                m_priorityCombo.AddString("´¶³q - 6");
                m_priorityCombo.AddString("´¶³q - 7");
                m_priorityCombo.AddString("´¶³q - 8");

                m_priorityCombo.SetItemData(0, 1);
                m_priorityCombo.SetItemData(1, 2);
                m_priorityCombo.SetItemData(2, 3);
                m_priorityCombo.SetItemData(3, 4);
                m_priorityCombo.SetItemData(4, 5);
                m_priorityCombo.SetItemData(5, 6);
                m_priorityCombo.SetItemData(6, 7);
                m_priorityCombo.SetItemData(7, 8);
                break;
        }

        // restore the selected priority if possible
        if (selected != CB_ERR)
        {
            selected = m_priorityCombo.FindString(-1, selectedString.GetBuffer(0));
        }

        // if nothing was selected or the previos selection wasnt found
        if (selected == CB_ERR)
        {
            // select 0
            selected = 3;
        }

        m_priorityCombo.SetCurSel(selected);
    }

    bool PriorityManager::isEmergency() const
    {
        return getPriority() < 4;
    }

    bool PriorityManager::isNormal() const
    {
        return 4 <= getPriority();
    }
}
