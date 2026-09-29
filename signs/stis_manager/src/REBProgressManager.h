/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/stis_manager/src/REBProgressManager.h $
 * @author:  Ripple
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 *
 */
#if !defined(AFX_REBPROGRESSDIALOG_H__6A18D7F2_AE8A_11D3_86DC_0008C773CB7F__INCLUDED_)
#define AFX_REBPROGRESSDIALOG_H__6A18D7F2_AE8A_11D3_86DC_0008C773CB7F__INCLUDED_

#if _MSC_VER > 1000
    #pragma once
#endif // _MSC_VER > 1000
// REBProgressDialog.h : header file
//

#include <afxcmn.h> // for CProgressCtrl
#include "resource.h"

#define REBPROGRESSMAXSTRING 100
#define REBPROGRESSMAXSTATIC 3

typedef struct tagREBPROGRESSDATA
{
    TCHAR cCaption[REBPROGRESSMAXSTRING + 1];
    TCHAR cAbortText[REBPROGRESSMAXSTRING + 1];
    TCHAR cStaticText[REBPROGRESSMAXSTATIC][REBPROGRESSMAXSTRING + 1];
    BOOL  bCancelEnabled;
    BOOL  bProgressEnabled;
    BOOL  bVisible;
    int   nProgress;
} REBPROGRESSDATA;

void REBInitializeProgressData(REBPROGRESSDATA* pData);
void REBCopyProgressData(REBPROGRESSDATA* pDataDest, REBPROGRESSDATA* pDataSource);

class REBProgressDialog; // forward declaration
class REBProgressThread; // forward declaration

/////////////////////////////////////////////////////////////////////////////
// REBProgressManager

class REBProgressManager
{
public:

    REBProgressManager();
    virtual ~REBProgressManager();

    void EndProgressDialog();
    void BeginProgressDialog();
    BOOL IsInited() { return m_bInited; }
    BOOL IsProgressDialogActive() { return (m_pThread != NULL); }
    void Exit();
    void init(HWND hwndParent);

    BOOL GetVisible();
    void SetVisible(BOOL bNewVal);
    LPCTSTR GetAbortText();
    void SetAbortText(LPCTSTR pszText);
    BOOL IsProgressEnabled();
    BOOL IsCancelEnabled();
    void SetUserAbortFlag(BOOL bNewVal);
    BOOL GetUserAbortFlag();
    void EnableProgress(BOOL bEnable);
    void EnableCancel(BOOL bEnable);
    int GetProgress();
    void SetProgress(int nVal);
    LPCTSTR GetCaption();
    void SetCaption(LPCTSTR pszCaption);
    LPCTSTR GetStaticText(int nIndex);
    void SetStaticText(int nIndex, LPCTSTR pszText);

    CWnd* getProgressWnd();

protected:

    HWND  m_hwndParent;
    BOOL m_bInited;
    REBProgressThread* m_pThread;
    BOOL m_bVisible;

protected:

    void PreAccess(int nCritSec);
    void PostAccess(int nCritSec);
    void NotifyChange();
    void WaitForProgressDialog();
    void CheckAndPump();
};

/////////////////////////////////////////////////////////////////////////////
// REBProgressThread

class REBProgressThread : public CWinThread
{
public:

    DECLARE_DYNCREATE(REBProgressThread)
    REBProgressThread(HWND hwndParent);
    REBProgressThread(); // constructor required by DECLARE_DYNCREATE macro (not used)
    ~REBProgressThread();
    virtual BOOL InitInstance();
    virtual int ExitInstance();

protected:

    HWND  m_hwndParent;
};

/////////////////////////////////////////////////////////////////////////////
// REBProgressDialog dialog

class REBProgressDialog : public CDialog
{
public:

    // Construction

public:

    REBProgressDialog(CWnd* pParent = NULL);   // standard constructor

    // Dialog Data
    //{{AFX_DATA(REBProgressDialog)
    enum { IDD = IDD_DLG_INITPROGRESS };
    CProgressCtrl   m_ctrlProgress;
    CString m_csText1;
    CString m_csText2;
    CString m_csText3;
    CButton m_buttonCancel;
    //}}AFX_DATA

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(REBProgressDialog)

protected:

    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

    // Implementation

protected:

    void HandleUserAbort();
    void RefreshData(BOOL bInit = FALSE);
    CString m_csAbortText;
    BOOL m_bUserAbortFlag;
    BOOL m_bProgressEnabled;
    BOOL m_bCancelEnabled;
    BOOL m_bVisible;
    int m_nProgress;
    CString m_csCaption;
    REBPROGRESSDATA m_TempData;

    // Generated message map functions
    //{{AFX_MSG(REBProgressDialog)
    virtual void OnCancel();
    virtual BOOL OnInitDialog();
    afx_msg void OnDestroy();
    //}}AFX_MSG
    afx_msg LRESULT OnRefresh(WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REBPROGRESSDIALOG_H__6A18D7F2_AE8A_11D3_86DC_0008C773CB7F__INCLUDED_)
