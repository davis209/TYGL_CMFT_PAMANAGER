/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/stis_manager/src/InitProgressDlg.h $
 * @author:  Ripple
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 *
 */
#if !defined(AFX_INITPROGRESSDLG_H__BD106266_6FA4_4EAA_B906_346599707DDE__INCLUDED_)
#define AFX_INITPROGRESSDLG_H__BD106266_6FA4_4EAA_B906_346599707DDE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InitProgressDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInitProgressDlg dialog

class CInitProgressDlg : public CDialog
{
    // Construction

public:

    CInitProgressDlg(CWnd* pParent = NULL);   // standard constructor

    // Dialog Data
    //{{AFX_DATA(CInitProgressDlg)
    enum { IDD = IDD_DLG_INITPROGRESS };
    // NOTE: the ClassWizard will add data members here
    //}}AFX_DATA

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CInitProgressDlg)

protected:

    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

    // Implementation

protected:

    // Generated message map functions
    //{{AFX_MSG(CInitProgressDlg)
    // NOTE: the ClassWizard will add member functions here
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INITPROGRESSDLG_H__BD106266_6FA4_4EAA_B906_346599707DDE__INCLUDED_)
