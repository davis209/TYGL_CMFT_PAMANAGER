/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/stis_manager/src/MainTab.h $
 * @author:  Adam Radics
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/03/05 11:47:34 $
 * Last modified by:  $Author: builder $
 *
 * This creates and manages each of the three main tabs at the top of the app.
 *
 */

#pragma once
#include "DisplayPage.h"
#include "LibraryVersionPage.h"
#include "bus/mfc_extensions/src/tab_dialog_control/TabDialogCtrl.h"

class REBProgressManager;
/////////////////////////////////////////////////////////////////////////////
// MainTab window
namespace TA_IRS_App
{
    class MainTab : public CTabDialogCtrl
    {
    public:

        MainTab();
        virtual ~MainTab();

        //haipeng added for the perfomance issue
        //void init(REBProgressManager & mgr);

        void init();

        //haipeng added for the perfomance issue
        // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(MainTab)

    public:

        virtual BOOL PreTranslateMessage(MSG* pMsg);
        //}}AFX_VIRTUAL

        //Maochun++
        //TD12780
        CDialog* getMainTabPage(UINT iPage);
        //++Maochun
        //TD12780

        //TD 15349
        //zhou yuan++
        DisplayPage& getDisplayPage();
        //++zhou yuan

        LibraryVersionPage& getdownLoadPage();

        static TA_Base_Core::NonReEntrantThreadLockable s_dipalyThreadLock;
        static TA_Base_Core::NonReEntrantThreadLockable s_downLoadPageLock;

    protected:

        //{{AFX_MSG(MainTab)
        // NOTE - the ClassWizard will add and remove member functions here.
        afx_msg void OnPaint();
        afx_msg void OnKickIdle();
        //}}AFX_MSG

        DECLARE_MESSAGE_MAP()

    private:

        DisplayPage m_displayPage;
        LibraryVersionPage m_downloadPage;
    };

    /////////////////////////////////////////////////////////////////////////////

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}
