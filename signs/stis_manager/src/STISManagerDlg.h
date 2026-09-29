/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File$
 * @author:  Robin Ashcroft
 * @version: $Revision$
 *
 * Last modification: $DateTime$
 * Last modified by:  $Author$
 *
 * Main Page for STIS manager
 *
 */

#pragma once
#include "resource.h"
#include "MainTab.h"
#include "core/types/src/ta_types.h"
#include "app/signs/stis_manager/src/PIDController.h"
#include "bus/generic_gui/src/TransActiveDialog.h"
#include "core/utilities/src/RunParams.h"

namespace TA_IRS_App
{
    class CSTISManagerDlg : public TA_Base_Bus::TransActiveDialog, public TA_Base_Core::RunParamUser
    {
    public:

        bool ModifyWindowPos();
        CSTISManagerDlg(TA_Base_Bus::IGUIAccess& controlClass); // standard constructor
        ~CSTISManagerDlg();

        bool wantToShow() { return m_wantToShow; };

        /**
         * onRunParamChange
         *
         * Called for a runparam change. selection and deselection.
         *
         */
        void onRunParamChange(const std::string& name, const std::string& value);

        ////haipeng added for the perfomance issue
        void initAll();
        //haipeng added for the perfomance issue

    protected:

        // Dialog Data
        //{{AFX_DATA(CSTISManagerDlg)
        enum { IDD = IDD_STISMANAGER_DIALOG };
        MainTab m_mainTab;
        //}}AFX_DATA

        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(CSTISManagerDlg)

    protected:

        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        //}}AFX_VIRTUAL

        /**
         * OnSetWindowPosition (virtual extension)
         *
         * This method is called whenever GenericGui gets a new window position
         * from control station
         *
         */
        afx_msg LRESULT OnSetWindowPosition(WPARAM wParam, LPARAM lParam) override;
        afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos) override;

        // Generated message map functions
        //{{AFX_MSG(CSTISManagerDlg)
        virtual BOOL OnInitDialog();
        afx_msg void OnPaint();
        afx_msg HCURSOR OnQueryDragIcon();
        afx_msg void OnFileExit();
        afx_msg void OnHelpAbout();
        afx_msg void OnHelpStismanagerhelp();
        afx_msg void OnSize(UINT nType, int cx, int cy);
        afx_msg void OnActivateApp(BOOL bActive, DWORD hTask);
        afx_msg void onHelpButton();
        afx_msg void onBtnClose();
        afx_msg void onClose();
        afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
        afx_msg void OnAppExit();
        afx_msg void OnIdleUpdateCmdUI();
        afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
        afx_msg void OnBnClickedAllMessageViewerButton();
        afx_msg void OnBnClickedAllTemplateViewerButton();
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()

    private:

        /**
         * Attempts to set the schematic into an active state
         *  If fails, application must be automatically closed (for safety reasons)
         *
         */
        void activateSchematic();

        //std::vector<std::string> tokenizeString(std::string theString, const std::string& separatorList );

        HICON m_hIcon;

        // Indicates whether the GUI is meant to be visible at the moment
        bool m_wantToShow;

        // Set to true once the application is first shown
        bool m_initialDisplay;

        PIDController m_pidController;

        bool m_initReady; //haipeng added
        //void offsetWindowPos(CWnd& wnd, ta_int32 x, ta_int32 y);//lkm

        BOOL m_bMaxFlag;    //16350 libo
        BOOL m_bFirstShow;  //16350 libo
    };
}
