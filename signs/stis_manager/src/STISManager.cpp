/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/stis_manager/src/STISManager.cpp $
 * @author:  Robin Aschcroft
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 *
 *
 */

#include "stdafx.h"
#include "STISManager.h"
#include "STISManagerGUI.h"

#ifdef _DEBUG
    #define new DEBUG_NEW
    #undef THIS_FILE
    static char THIS_FILE[] = __FILE__;
#endif

namespace TA_IRS_App
{
    BEGIN_MESSAGE_MAP(CSTISManagerApp,  TA_Base_Bus::TransActiveWinApp)
        //{{AFX_MSG_MAP(CSTISManagerApp)
        // NOTE - the ClassWizard will add and remove mapping macros here.
        //    DO NOT EDIT what you see in these blocks of generated code!
        //}}AFX_MSG
        ON_COMMAND(ID_HELP, CWinApp::OnHelp)
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // CSTISManagerApp construction

    CSTISManagerApp::CSTISManagerApp()
        : TA_Base_Bus::TransActiveWinApp(new CSTISManagerGUI(), "旅客資訊管理器")
    {
        // TODO: add construction code here,
        // Place all significant initialization in InitInstance
    }

    /////////////////////////////////////////////////////////////////////////////
    // The one and only CSTISManagerApp object

    CSTISManagerApp theApp;

    /////////////////////////////////////////////////////////////////////////////
    // CSTISManagerApp initialization

    /*BOOL CSTISManagerApp::InitInstance()
    {
        // Standard initialization
        // If you are not using these features and wish to reduce the size
        //  of your final executable, you should remove from the following
        //  the specific initialization routines you do not need.

    #ifdef _AFXDLL
        Enable3dControls();         // Call this when using MFC in a shared DLL
    #else
        Enable3dControlsStatic();   // Call this when linking to MFC statically
    #endif

        CSTISManagerDlg dlg;
        m_pMainWnd = &dlg;
        int nResponse = dlg.DoModal();
        if (nResponse == IDOK)
        {
            // TODO: Place code here to handle when the dialog is
            //  dismissed with OK
        }
        else if (nResponse == IDCANCEL)
        {
            // TODO: Place code here to handle when the dialog is
            //  dismissed with Cancel
        }

        // Since the dialog has been closed, return FALSE so that we exit the
        //  application, rather than start the application's message pump.
        return FALSE;
    } */

} // TA_IRS_App
