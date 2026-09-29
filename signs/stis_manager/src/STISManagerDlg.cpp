/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File$
 * @author:  Ripple
 * @version: $Revision$
 *
 * Last modification: $DateTime$
 * Last modified by:  $Author$
 *
 */
// STISManagerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "helperfun.h"
#include "STISManager.h"
#include "STISManagerDlg.h"
#include "UserMessages.h"
#include "REBProgressManager.h"
#include "TemplatePreviewProcess.h"
#include "PythonServer.h"
#include "MessageViewerProcess.h"
#include "TemplateViewerProcess.h"
#include "CurrentDisplayMessagesDlg.h"
#include "MonitorInfo.h"
#include "app/signs/common_library/src/stis_protocol/STISClient.h"
#include "app/signs/common_library/src/stis_protocol/message_types/MessageTypes.h"
//#include "app/signs/STISManager/src/RunParamListener.h"
#include "app/signs/stis_manager/src/GraphworxComms.h"
#include "app/signs/stis_manager/src/DisplayPage.h"
#include "bus/generic_gui/src/HelpLauncher.h"
#include "bus/generic_gui/src/GenericGuiConstants.h"
#include "bus/generic_gui/src/AppLauncher.h"    //zhongjie++
#include "core/data_access_interface/entity_access/src/IConsole.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/src/Location.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/TISAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/STISEntityData.h"
#include "core/exceptions/src/ApplicationException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/EntityTypeException.h"
#include "core/utility/src/base_ex/RunParamsEx.h"
#include "core/utility/src/base_ex/DebugUtilEx.h"
#include "core/utility/src/core/MakeNameValuePairsString.h"
#include <boost/tokenizer.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SM_CXVIRTUALSCREEN      78      //zhongjie++
#define MINVALUE                2400    //zhongjie++
#define NUMSCREENS              3       //zhongjie++

#define RPARAM_PRELOADTEMPLATEPREVIEWER "PreloadTemplatePreviewer"
#define RPARAM_NOMODIFYINITIALWINDOWSPOS "NoModifyInitialWindowsPos"
#define RPARAM_NOMINMAXINFO "NoMinMaxInfo"
#define RPARAM_NOWINDOWSPOSCHANGING "NoWindowsPosChanging"
#define RPARAM_CLIENTCALLTIMEOUTSECONDS "ClientCallTimeoutSeconds"

using namespace TA_Base_Core;
using namespace TA_Base_Bus;
using TA_Base_Bus::TransActiveDialog;
using TA_Base_Ex::RunParamsEx;
using TA_IRS_App::STIS_PROTOCOL::STISClient;
using TA_IRS_App::STIS_PROTOCOL::A50_CurrentDisplayMessageTemplateReport;

namespace
{
    std::string SELECTED_STIS_MSG_TAG = "SelectedSTISMessage";
}

namespace TA_IRS_App
{
    CSTISManagerDlg::CSTISManagerDlg(TA_Base_Bus::IGUIAccess& controlClass)
        : TransActiveDialog(controlClass, CSTISManagerDlg::IDD, NULL),
        m_initialDisplay(false), m_wantToShow(false), m_initReady(false)
    {
        //{{AFX_DATA_INIT(CSTISManagerDlg)
        //}}AFX_DATA_INIT
        // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
        m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
        UserMessages::getInstance().setParent(this);

        if (RunParamsEx::isTrue(RPARAM_PRELOADTEMPLATEPREVIEWER))
        {
            TemplatePreviewProcess::instance().launch_and_hide_async();
        }

        PythonServer::instance().start();  // start server
    }

    CSTISManagerDlg::~CSTISManagerDlg()
    {
        onClose();
    }

    void CSTISManagerDlg::DoDataExchange(CDataExchange* pDX)
    {
        TransActiveDialog::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(CSTISManagerDlg)
        DDX_Control(pDX, IDC_MAINPAGE_TAB, m_mainTab);
        //}}AFX_DATA_MAP
    }

    BEGIN_MESSAGE_MAP(CSTISManagerDlg, TransActiveDialog)
        //{{AFX_MSG_MAP(CSTISManagerDlg)
        ON_WM_PAINT()
        ON_WM_QUERYDRAGICON()
        ON_WM_SIZE()
        ON_WM_ACTIVATEAPP()
        ON_WM_SHOWWINDOW()
        ON_WM_CLOSE()
        ON_WM_GETMINMAXINFO()
        ON_COMMAND(ID_FILE_EXIT, OnFileExit)
        ON_COMMAND(ID_HELP_ABOUTSTISMANAGER, OnHelpAbout)
        ON_COMMAND(ID_HELP_STISMANAGERHELP, OnHelpStismanagerhelp)
        ON_BN_CLICKED(IDC_HELP_BUTTON, onHelpButton)
        ON_BN_CLICKED(IDC_CLOSE, onBtnClose)
        ON_REGISTERED_MESSAGE(TA_GenericGui::WM_SET_WINDOW_POSITION, OnSetWindowPosition)
        ON_COMMAND(ID_APP_EXIT, OnAppExit)
        ON_MESSAGE_VOID(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
        ON_BN_CLICKED(IDC_ALL_MESSAGE_VIEWER_BUTTON, &CSTISManagerDlg::OnBnClickedAllMessageViewerButton)
        ON_BN_CLICKED(IDC_ALL_TEMPLATE_VIEWER_BUTTON, &CSTISManagerDlg::OnBnClickedAllTemplateViewerButton)
      //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    BOOL CSTISManagerDlg::OnInitDialog()
    {
#if 0
        CRect windowSize;
        GetWindowRect(&windowSize);
        TA_Base_Bus::ResizingProperties properties;
        properties.canMaximise = false;
        properties.maxHeight = -1;
        properties.maxWidth = -1;
        properties.minHeight = windowSize.bottom - windowSize.top;
        properties.minWidth = windowSize.right - windowSize.left;
        setResizingProperties(properties);
#else
        CRect windowSize;
        GetWindowRect(&windowSize);
        TA_Base_Bus::ResizingProperties properties;
        properties.canMaximise = true;
        properties.maxHeight = -1;
        properties.maxWidth = -1;
        properties.minHeight = -1;
        properties.minWidth = -1;
        setResizingProperties(properties);
#endif

        TransActiveDialog::OnInitDialog();
        // Set the icon for this dialog.  The framework does this automatically
        //  when the application's main window is not a dialog
        SetIcon(m_hIcon, TRUE);         // Set big icon
        SetIcon(m_hIcon, FALSE);        // Set small icon

        //Maochun++
        //TD12780
        CDialog* displayDlg = m_mainTab.getMainTabPage(0);
        CDialog* versionDlg = m_mainTab.getMainTabPage(1);

        WINDOWPOS w{};
        w.x = windowSize.left + 20;
        w.y = windowSize.top + 30;
        w.cx = windowSize.Width() - 42;
        w.cy = windowSize.Height() - 133;
        displayDlg->SetWindowPos(NULL, w.x, w.y, w.cx, w.cy, SWP_NOZORDER);
        versionDlg->SetWindowPos(NULL, w.x, w.y, w.cx, w.cy, SWP_NOZORDER);
        LOG_DEBUG("OnInitDialog(): displayDlg->SetWindowPos=%s", w);

        ModifyWindowPos();

        std::string cmdline(GetCommandLine());

        if (cmdline.find("select") != std::string::npos)
        {
            ShowWindow(SW_MINIMIZE);
        }

        return TRUE;  // return TRUE  unless you set the focus to a control
    }

    void CSTISManagerDlg::OnShowWindow(BOOL bShow, UINT nStatus)
    {
        TransActiveDialog::OnShowWindow(bShow, nStatus);
    }

    void CSTISManagerDlg::initAll()
    {
        LOG_CALLSTACK("CSTISManagerDlg::initAll");

        STISClient::instance().parse_options(str(boost::format
        ("--root-dir=%s --server=local-tis-agent --sync-all --client-call-timeout-seconds=%d"
        ) % RunParamsEx::expand(RunParamsEx::get_or(RPARAM_STISROOTDIR, DEFAULT_STIS_ROOT_DIR)) % RunParamsEx::get_or(RPARAM_CLIENTCALLTIMEOUTSECONDS, 60)));
#if 0
        STISClient::instance().sync_all();
#else
        STISClient::instance().sync_versions();
        STISClient::instance().sync_all_versions();
        STISClient::instance().sync_current_message_template_library();
#endif
        STISClient::instance().start();

        //show the progress dialog
        // TODO: Add extra validation here
        //REBProgressManager mgr;

        //mgr.init(GetDesktopWindow()->m_hWnd); //
        //mgr.EnableProgress(false);
        //mgr.SetVisible(false);
        //mgr.EnableCancel(FALSE);
        //mgr.BeginProgressDialog();
        //UserMessages::getInstance().setParent(mgr.getProgressWnd());
        //mgr.SetCaption("STIS manager initializing...");

        //mgr.SetProgress(1);
        //mgr.SetStaticText(0, "Initializing the subsystem pages...");

        // Hide the GUI by default initially
        m_wantToShow = false;
        //ShowWindow(SW_MINIMIZE);//to show

        m_mainTab.init();

        // unsuppress messages
        UserMessages::getInstance().setMessageSuppression(false);

        // Register for --display runparams here as well as
        // the PIDSelectionManager
        // This object has to handle the restoration of the app from minimisation
        RunParams::getInstance().registerRunParamUser(this, "Display");
        RunParams::getInstance().registerRunParamUser(this, SELECTED_STIS_MSG_TAG);

        // If the STIS Manager has crashed, pressing the 'Launch STIS Manager'
        // will start the app, as opposed to just sending a --display=xxx,n,SHOW
        // so the SHOW run param will be on the startup list, so it needs
        // to be processed here

        //16350 libo
        m_bFirstShow = true;
        std::string preselectMsg = RunParams::getInstance().get(SELECTED_STIS_MSG_TAG);
        m_bMaxFlag = !preselectMsg.empty();

        //16350 libo

        std::string displayValue = RunParams::getInstance().get("Display");

        if (displayValue.size())
        {
            onRunParamChange("Display", displayValue);
        }

        //TD 15349
        //zhou yuan++
        //  std::string preselectMsg = RunParams::getInstance().get(SELECTED_STIS_MSG_TAG.c_str());
        if (preselectMsg.size())
        {
            onRunParamChange(SELECTED_STIS_MSG_TAG, preselectMsg);
        }

        //++zhou yuan

        m_pidController.initialise();
        //mgr.SetProgress(100);
        m_initReady = true;
        //UserMessages::getInstance().setParent(this);
    }

    // If you add a minimize button to your dialog, you will need the code below
    //  to draw the icon.  For MFC applications using the document/view model,
    //  this is automatically done for you by the framework.
    void CSTISManagerDlg::OnPaint()
    {
        if (IsIconic())
        {
            CPaintDC dc(this); // device context for painting

            SendMessage(WM_ICONERASEBKGND, (WPARAM)dc.GetSafeHdc(), 0);

            // Center icon in client rectangle
            int cxIcon = GetSystemMetrics(SM_CXICON);
            int cyIcon = GetSystemMetrics(SM_CYICON);
            CRect rect;
            GetClientRect(&rect);
            int x = (rect.Width() - cxIcon + 1) / 2;
            int y = (rect.Height() - cyIcon + 1) / 2;

            // Draw the icon
            dc.DrawIcon(x, y, m_hIcon);
        }
        else
        {
            TransActiveDialog::OnPaint();
        }
    }

    // The system calls this to obtain the cursor to display while the user drags
    //  the minimized window.
    HCURSOR CSTISManagerDlg::OnQueryDragIcon()
    {
        return (HCURSOR)m_hIcon;
    }

    void CSTISManagerDlg::OnFileExit()
    {
        //TD18095, jianghp, to fix the performance of showing manager application
        DestroyWindow();
    }

    void CSTISManagerDlg::OnHelpAbout()
    {
        TA_Base_Bus::HelpLauncher::getInstance().displayAboutBox();
    }

    void CSTISManagerDlg::OnSize(UINT nType, int cx, int cy)
    {
        // If the 'Launch...' button has been pressed
        // only then should the app be
        if ((m_wantToShow) && (nType == SIZE_RESTORED))
        {
            TransActiveDialog::OnSize(nType, cx, cy);
        }
        else
        {
            if (nType == SIZE_MINIMIZED)  // Minimise
            {
                m_wantToShow = false;  // make sure the window doesn't pop up on PID selections now it is minimised
                return;
            }
        }
    }

    void CSTISManagerDlg::OnActivateApp(BOOL bActive, DWORD hTask)
    {
        if (!bActive)
        {
            // De-activation, no longer want to show the application when it's de-activated
            if (m_initialDisplay)
            {
                m_wantToShow = false;

                // suppress messages
                UserMessages::getInstance().setMessageSuppression(true);
            }
        }

        TransActiveDialog::OnActivateApp(bActive, hTask);
    }

    LRESULT CSTISManagerDlg::OnSetWindowPosition(WPARAM wParam, LPARAM lParam)
    {
        // from the PA manager
        // TES #720
        // don't show it if we don't want it to be shown
        if (lParam == TA_GenericGui::NO_REPOSITION)
        {
            return 0;
        }

        //TD16102
        if (lParam == TA_GenericGui::FOCUS)
        {
            return TransActiveDialog::OnSetWindowPosition(wParam, lParam);
        }
        //TD16102

        else
        {
            return -1;
        }

        if (!m_wantToShow && lParam == TA_GenericGui::REPOSITION)
        {
            //return TransActiveDialog::OnSetWindowPosition(wParam, TA_GenericGui::REPOSITION_NO_RESTORE);
            //TransActiveDialog::OnSetWindowPosition(wParam, TA_GenericGui::REPOSITION_NO_RESTORE);
            if (m_bFirstShow)
            {
                return TransActiveDialog::OnSetWindowPosition(wParam, TA_GenericGui::MINIMISE);
            }
        }
        else if (m_wantToShow && lParam == TA_GenericGui::REPOSITION && m_bMaxFlag)
        {
            m_bMaxFlag = false;
            return TransActiveDialog::OnSetWindowPosition(wParam, TA_GenericGui::REPOSITION_NO_RESTORE);
        }

        m_bFirstShow = false;

        {
            ACE_GUARD_RETURN(ACE_Thread_Mutex, ace_mon, PidSelectionManager::Tag_mutex, -1);

            if (PidSelectionManager::DONTGOFRONT_TAG == 0)
            {
                return TransActiveDialog::OnSetWindowPosition(wParam, lParam);
            }

            --PidSelectionManager::DONTGOFRONT_TAG;
        }
        return 0;
        // TD16350,16285 - STIS Manager overlap screen on LED View Current message
        // remove else statement
        /* else
        {
            return TransActiveDialog::OnSetWindowPosition(wParam, lParam);
        } */

        //return TransActiveDialog::OnSetWindowPosition(wParam, lParam);
    }

    void CSTISManagerDlg::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos)
    {
        if (RunParamsEx::isTrue(RPARAM_NOWINDOWSPOSCHANGING))
        {
            CDialog::OnWindowPosChanging(lpwndpos);
            return;
        }

        TransActiveDialog::OnWindowPosChanging(lpwndpos);
    }

    void CSTISManagerDlg::OnHelpStismanagerhelp()
    {
        TA_Base_Bus::HelpLauncher::getInstance().displayHelp();
    }

    void CSTISManagerDlg::onHelpButton()
    {
        TA_Base_Bus::HelpLauncher::getInstance().displayHelp();
    }

    void CSTISManagerDlg::onClose()
    {
        //TD18095, jianghp, to fix the performance of showing manager application
        CurrentDisplayMessagesDlg::on_close();
        STISClient::instance().stop();
        TemplatePreviewProcess::remove();
        MessageViewerProcess::remove();
        TemplateViewerProcess::remove();
        ::_exit(0);
        DestroyWindow();
    }

    void CSTISManagerDlg::onBtnClose()
    {
        onClose();
    }

    void CSTISManagerDlg::activateSchematic()
    {
        /*
        bool schematicUpdated = false;

        // Now we've finished loading, we may activate the graphworx display we're operating for
        try
        {
            //std::string schematicId = RunParamListener::getInstance().getSchematicIdentifier();
            std::string schematicId;

            schematicUpdated = TA_IRS_App::GraphworxComms::getInstance().activateGraphworxDisplay(schematicId);
        }
        catch (const TA_Base_Core::ValueNotSetException& e)
        {
            LOG_EXCEPTION("ValueNotSetException", e.what());
        }
        catch (...)
        {
            // Silently log any error communicating with the actual schematic, may still run
            LOG_EXCEPTION("Unknown", "While activating schematic");
        }
        */
        /*
            #ifdef _DEBUG
                // Perform an override if a debug session
                if (CachedConfig::getInstance()->getSessionId().compare("debug") == 0)
                {
                    schematicUpdated = true;
                }
            #endif

                if (!schematicUpdated)
                {
                    //PaErrorHandler::displayModalError(PaErrorHandler::ERROR_SCHEMATIC_COMMS);
                    //PostQuitMessage(0);
                }
        */
    }

    void CSTISManagerDlg::onRunParamChange(const std::string& name, const std::string& value)
    {
        std::vector<std::string> valueParts = tokenizeString(value, ",");

        if ((name.compare("Display") == 0) && (valueParts[2].compare("SHOW") == 0))
        {
            m_wantToShow = true;

            if (::IsWindow(m_hWnd))
            {
                ShowWindow(SW_RESTORE);
                SetForegroundWindow();
            }
        }

        if ((name.compare(SELECTED_STIS_MSG_TAG) == 0) && !(value.empty()))
        {
            m_bMaxFlag = true;
            DisplayPage& displayPage = m_mainTab.getDisplayPage();
            displayPage.findAndSelectStationMessage(boost::replace_all_copy(value, "_", " "));
        }
    }

    //++zhou yuan
    void CSTISManagerDlg::OnAppExit()
    {
        onClose();
        DestroyWindow();
    }

    bool CSTISManagerDlg::ModifyWindowPos() //zhongjie++ this function to modify the STisManager Dialog position in three monitor
    {
        if (RunParamsEx::isTrue(RPARAM_NOMODIFYINITIALWINDOWSPOS))
        {
            return true;
        }

        CPoint pt;
        GetCursorPos(&pt);
        RECT boundary;

        try
        {
            boundary = AppLauncher::getInstance().getRect(TA_GenericGui::SCREEN_CURRENT,
                                                          TA_GenericGui::AREA_SCHEMATIC,
                                                          pt.x);
        }
        catch (const TA_Base_Core::ApplicationException& ex)
        {
            LOG_EXCEPTION("TA_Base_Core::ApplicationException", ex.what());
            return true;
        }

        auto totalScreenWidth = MonitorInfo::instance().m_totalScreenWidth;
        auto numScreens = MonitorInfo::instance().getNumberOfMonitors();
        auto eachScreenWidth = MonitorInfo::instance().getEachScreenWidth();

#if 0
        currentScreen = x_pos / (screenWidth / numScreens);
        left = currentScreen * (screenWidth / numScreens) + 1;
        width = screenWidth / numScreens;
        top = boundary.top;
        height = boundary.bottom - boundary.top;
#else
        auto currentScreen = pt.x / eachScreenWidth;
        auto left = currentScreen * eachScreenWidth - 8;
        auto top = boundary.top;
        auto width = eachScreenWidth + 16;  // 16 = 8 + 8
        auto height = boundary.bottom - boundary.top + 12;
#endif
        this->MoveWindow(left, top, width, height, NULL);
        LOG_DEBUG("CSTISManagerDlg::ModifyWindowPos(): %s", nvps(left, top, width, height, numScreens, eachScreenWidth, totalScreenWidth));
        return true;
    }

    void CSTISManagerDlg::OnIdleUpdateCmdUI()
    {
        SendMessageToDescendants(WM_KICKIDLE, 0, 0, FALSE, FALSE);
        CurrentDisplayMessagesDlg::on_idle_update_cmd_ui();
    }

    void CSTISManagerDlg::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI)
    {
        CTADialog::OnGetMinMaxInfo(lpMMI);

        if (RunParamsEx::isTrue(RPARAM_NOMINMAXINFO))
        {
            return;
        }

        MINMAXINFO backup = *lpMMI;

        TransActiveDialog::OnGetMinMaxInfo(lpMMI);

        lpMMI->ptReserved = backup.ptReserved;
        lpMMI->ptMaxPosition = backup.ptMaxPosition;  // the max position is (-8, -8), NOT (0, 0)
        lpMMI->ptMaxSize.x = backup.ptMaxSize.x;
        lpMMI->ptMaxSize.y += 12;
        lpMMI->ptMinTrackSize.x = backup.ptMinTrackSize.x;
        lpMMI->ptMaxTrackSize.x = backup.ptMaxTrackSize.x;
        lpMMI->ptMaxTrackSize.y += 12;
        LOG_DEBUG("CSTISManagerDlg::OnGetMinMaxInfo(): %s", nvps(lpMMI->ptMaxPosition, lpMMI->ptMaxSize, lpMMI->ptMinTrackSize, lpMMI->ptMaxTrackSize));
    }

    void CSTISManagerDlg::OnBnClickedAllMessageViewerButton()
    {
        MessageViewerProcess::instance().launch();
    }

    void CSTISManagerDlg::OnBnClickedAllTemplateViewerButton()
    {
        TemplateViewerProcess::instance().launch();
    }
}
