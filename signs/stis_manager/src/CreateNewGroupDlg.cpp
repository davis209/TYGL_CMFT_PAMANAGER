/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/stis_manager/src/CreateNewGroupDlg.cpp $
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

#include "stdafx.h"
#include "resource.h"
#include "CreateNewGroupDlg.h"
#include "PidGroupCombo.h"
#include "core/data_access_interface/tis_agent/src/PidGroupsAccessFactory.h"
#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164 
#include "UserMessages.h"

#ifdef _DEBUG
    #define new DEBUG_NEW
    #undef THIS_FILE
    static char THIS_FILE[] = __FILE__;
#endif

namespace TA_IRS_App
{
    CreateNewGroupDlg::CreateNewGroupDlg(CString windowTitle,
                                         CString textPrompt,
                                         CString defaultName,
                                         CWnd* pParent /*=NULL*/)
        :
        CDialog(CreateNewGroupDlg::IDD, pParent),
        m_windowTitle(windowTitle),
        m_AccessFactoryFailure(false)
    {
        //{{AFX_DATA_INIT(CreateNewGroupDlg)
        m_name = _T(defaultName);
        m_textPrompt = _T(textPrompt);
        //}}AFX_DATA_INIT
    }

    void CreateNewGroupDlg::DoDataExchange(CDataExchange* pDX)
    {
        CDialog::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(CreateNewGroupDlg)
        DDX_Control(pDX, IDC_EDITNAME, m_nameControl);
        DDX_Text(pDX, IDC_EDITNAME, m_name);
        DDV_MaxChars(pDX, m_name, 40);
        DDX_Text(pDX, IDC_STATICTEXT, m_textPrompt);
        //}}AFX_DATA_MAP
    }

    BEGIN_MESSAGE_MAP(CreateNewGroupDlg, CDialog)
        //{{AFX_MSG_MAP(CreateNewGroupDlg)
        ON_WM_SHOWWINDOW()
        ON_BN_CLICKED(ID_CLOSE, OnClose)
        //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    BOOL CreateNewGroupDlg::OnInitDialog()
    {
        CDialog::OnInitDialog();

        // Apply the title
        SetWindowText(m_windowTitle);

        return TRUE;  // return TRUE unless you set the focus to a control
        // EXCEPTION: OCX Property Pages should return FALSE
    }

    void CreateNewGroupDlg::OnOK()
    {
        //You can not save a group with the same name as the one used for
        //the groups specified for a schematic
        CString text;
        m_nameControl.GetWindowText(text);
        std::string nameEntered = text;

        if (nameEntered == PidGroupCombo::HMIGroupName || nameEntered.empty())
        {
            // TD14164 ++
            /*MessageBox( _T("Groups cannot be saved with this name. Please specify another."),
                 _T("Invalid Group Name") );*/
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_070123, "旅客資訊管理器");
            // ++ TD14164

            return; //does not close the dialog
        }

        //tell the user if they are overwriting an exising group
        TA_IRS_Core::PidGroupsAccessFactory& mgr = TA_IRS_Core::PidGroupsAccessFactory::getInstance();

        m_AccessFactoryFailure = false;

        try
        {
            if (mgr.pidGroupExists(nameEntered))
            {
                // TD14164 ++
                /*int responce = MessageBox( _T("A group with the specified name already exists. Do you wish to replace it?"),
                     _T("Group already exists"), MB_YESNO );*/
                TA_Base_Bus::TransActiveMessage userMsg;
                UINT responce = userMsg.showMsgBox(IDS_UW_070029, "旅客資訊管理器");
                // ++ TD14164

                if (responce == IDNO)
                {
                    return;
                }  //does not close the dialog

                m_isModify = true;
            }
        }
        catch (...)
        {
            m_AccessFactoryFailure = true;
        }

        CDialog::OnOK();
    }

    void CreateNewGroupDlg::OnShowWindow(BOOL bShow, UINT nStatus)
    {
        CDialog::OnShowWindow(bShow, nStatus);

        // Select all of default name text
        m_nameControl.SetFocus();
        m_nameControl.SetSel(0, -1);
    }

    void CreateNewGroupDlg::OnClose()
    {
        // Close down the dialog
        CDialog::OnCancel();
    }

} // TA_IRS_App
