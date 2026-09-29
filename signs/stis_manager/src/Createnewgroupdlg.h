/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/stis_manager/src/Createnewgroupdlg.h $
 * @author:  Jade Lee
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 *
 * Taken from the PA Manager.
 * used for prompting for PID group names.
 *
 */

#pragma once
#include "resource.h"

namespace TA_IRS_App
{
    class CreateNewGroupDlg : public CDialog
    {
        // Construction

    public:

        /**
         * CreateNewGroupDlg (constructor)
         *
         * @param windowTitle
         *
         * @param textPrompt the text displaying just before text
         *          entry panel (ie Please enter a new...)
         *
         * @param initialName the initial text in text entry panel
         *
         */
        CreateNewGroupDlg(CString windowTitle,
                          CString textPrompt,
                          CString initialName,
                          CWnd* pParent = NULL);   // standard constructor

        /**
         * getName
         *
         * @return the name the user entered in dialog box
         *
         */
        std::string getName() const { return m_nameUtf8; }

        bool isModify() { return m_isModify; }

        bool m_AccessFactoryFailure;

        // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(CreateNewGroupDlg)

    protected:

        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        //}}AFX_VIRTUAL

        // Implementation

    protected:

        /**
         * OnCancel
         *
         * This method has been implemented to hide accidental calls when
         * the ESC key is pressed. It can be overriden with new behaviour if required.
         */
        virtual afx_msg void OnCancel() {}

        // Generated message map functions
        //{{AFX_MSG(CreateNewGroupDlg)
        virtual BOOL OnInitDialog();
        virtual void OnOK();
        afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
        afx_msg void OnClose();
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()

    private:

        // Dialog Data
        //{{AFX_DATA(CreateNewGroupDlg)
        enum { IDD = IDD_PID_GROUP_NAME };
        CEdit   m_nameControl;
        CString m_name;
        std::string m_nameUtf8;
        std::wstring m_textPrompt;
        //}}AFX_DATA

        std::wstring m_windowTitle;
        bool m_isModify = false;
    };

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}
