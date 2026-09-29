/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source: $File: //depot/4669_T01271350/4669/transactive/app/signs/stis_manager/src/PidListCtrl.h $
 * @author Adam Radics
 * @version $Revision: #1 $
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by: $Author: builder $
 *
 * PidListCtrl defines a list control that displays PIDs.
 */

#pragma once
#include "app/signs/common_library/src/STISUtility.h"
#include "bus/mfc_extensions/src/list_ctrl_selection_without_focus/ListCtrlSelNoFocus.h"
#include "bus/mfc_extensions/src/auto_sort_list_ctrl/AutoSortListCtrl.h"
#include "bus/mfc_extensions/src/auto_sort_list_ctrl/ISortingCallback.h"
#include "bus/mfc_extensions/src/fixed_column_header_ctrl/FixedHeaderCtrl.h"
#include "core/data_access_interface/tis_agent/src/ITrainTimeSchedule.h"

namespace TA_IRS_App
{
    class PidSelectionManager;
}

namespace TA_IRS_App
{
    class PidListCtrl :
        public TA_Base_Bus::ListCtrlSelNoFocus,
        public TA_Base_Bus::ISortingCallback
    {
    public:

        using AutoSortListCtrl = TA_Base_Bus::AutoSortListCtrl;
        using SortInfo = TA_Base_Bus::AutoSortListCtrl::SortInfo;
        using PID = STIS_UTILITY::PID;
        using PIDList = STIS_UTILITY::PIDList;

        // the columns for the list control
        enum PidColumns
        {
            LOCATION = 0,
            PID_NAME = 1
        };

        PidListCtrl() = default;

        /**
         * setPidSelectionManager
         *
         * Sets the object to tell when selection changes
         *
         * @param pidSelectionManager
         */
        void setPidSelectionManager(PidSelectionManager* pidSelectionManager);

        /**
         * sortCallback
         *
         * User defined sorting function
         *
         * @return positive value if item lParam2 before lParam1, negative if lParam1 should
         *      come first
         *
         * @param lParam1, lParam2 user defined values passed in from sort operation (app specific)
         * @param columnIndex the index of the column being sorted
         * @param currentSort defines the type of sorting specified (ie ascending or descending)
         */
        virtual int sortCallback(LPARAM lParam1, LPARAM lParam2, int columnIndex, AutoSortListCtrl::ECurrentSort currentSort);

        PIDList get_selected_pids();

    public:

        // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(PidListCtrl)

    protected:

        virtual void PreSubclassWindow();
        //}}AFX_VIRTUAL

        // Generated message map functions

    protected:

        //{{AFX_MSG(PidListCtrl)
        afx_msg LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
        afx_msg BOOL onItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg BOOL OnNMDblclkPidList(NMHDR* pNMHDR, LRESULT* pResult);
        //}}AFX_MSG

        DECLARE_MESSAGE_MAP()

        /**
         * isSelectable
         *
         * Given the row number, return true if this item
         * should be selectable.
         *
         * TES 880 - individual PIDS are no longer selected for clear requests
         *
         * @param rowNumber the row number.
         *
         * @return true if the item can be selected
         */
        virtual bool isSelectable(int rowNumber);

    private:

        // custom header control
        TA_Base_Bus::FixedHeaderCtrl    m_fixedHeaderCtrl;

        // current sort information
        SortInfo m_sortInfo{2};

        PidSelectionManager* m_pidSelectionManager = nullptr;

        bool m_pidSelected = false;

		CFont m_listfont;
    };

    /////////////////////////////////////////////////////////////////////////////

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}
