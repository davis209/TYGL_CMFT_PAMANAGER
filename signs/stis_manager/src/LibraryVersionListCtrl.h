/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source: $File: //depot/4669_T01271350/4669/transactive/app/signs/stis_manager/src/LibraryVersionListCtrl.h $
 * @author Adam Radics
 * @version $Revision: #1 $
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by: $Author: builder $
 *
 * This list is formatted to display library versions.
 */

#pragma once
#include "bus/mfc_extensions/src/list_ctrl_selection_without_focus/ListCtrlSelNoFocus.h"

namespace TA_IRS_App
{
    class LibraryVersionListCtrl : public TA_Base_Bus::ListCtrlSelNoFocus
    {
    public:

        LibraryVersionListCtrl();
        virtual ~LibraryVersionListCtrl();

        void setSelectable(bool selectble);

    public:

        // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(LibraryVersionListCtrl)

    protected:

        virtual void PreSubclassWindow();
        //}}AFX_VIRTUAL

        // Generated message map functions

    protected:

        //{{AFX_MSG(LibraryVersionListCtrl)
        //}}AFX_MSG

        DECLARE_MESSAGE_MAP()

        virtual bool isSelectable(int rowNumber) override;

    private:

        bool m_selectble = false;
    };

    /////////////////////////////////////////////////////////////////////////////

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}
