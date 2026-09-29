/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/stis_manager/src/STISManager.h $
 * @author:  Robin Aschcroft
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 *
 *
 */

#if !defined(AFX_STISMANAGER_H__FC9F4F15_195C_4A3F_86A2_136C4447608E__INCLUDED_)
#define AFX_STISMANAGER_H__FC9F4F15_195C_4A3F_86A2_136C4447608E__INCLUDED_

#if _MSC_VER > 1000
    #pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
    #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "bus/generic_gui/src/TransActiveWinApp.h"

//  Maximum number of characters possible for a free text message
#define MAX_FREE_TEXT_TITLE_CHARACTERS 40
#define MAX_FREE_TEXT_CONTENT_CHARACTERS 255

namespace TA_IRS_App
{
    class CSTISManagerApp : public TA_Base_Bus::TransActiveWinApp
    {
    public:

        CSTISManagerApp();

        // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(CSTISManagerApp)
        //}}AFX_VIRTUAL

        // Implementation

        //{{AFX_MSG(CSTISManagerApp)
        // NOTE - the ClassWizard will add and remove member functions here.
        //    DO NOT EDIT what you see in these blocks of generated code !
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

    /////////////////////////////////////////////////////////////////////////////

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // TA_IRS_App

#endif // !defined(AFX_STISMANAGER_H__FC9F4F15_195C_4A3F_86A2_136C4447608E__INCLUDED_)
