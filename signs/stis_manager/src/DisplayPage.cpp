/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File$
 * @author:  Adam Radics
 * @version: $Revision$
 *
 * Last modification: $DateTime$
 * Last modified by:  $Author$
 *
 * The Station display page
 */

#include "stdafx.h"
#include "STISManager.h"
#include "STISManagerDlg.h"
#include "DisplayPage.h"
#include "PIDControlDlg.h"
#include "UserMessages.h"
#include "STISPredefinedMessages.h"
#include "RightsManager.h"
#include "REBProgressManager.h"
#include "TisAgentInterface.h"
#include "WindowsUtil.h"
#include "TemplatePreviewProcess.h"
#include "app/signs/common_library/src/STISAuditMessage.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/TISAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/STISEntityData.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/EntityTypeException.h"
#include "core/naming/src/NamingMacros.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/utility/src/base_ex/CorbaUtilEx.h"
#include "core/utility/src/core/StdEx.h"
#include "core/utility/src/core/fixed_length_data/all.h"
#include "core/utilities/src/CodeConverter.h"
#include "core/utility/src/core/FileEx.h"
#include "core/utility/src/core/CacheDecorator.h"
#include "core/utility/src/core/algorithm/strings.h"
#include <boost/tokenizer.hpp>
#include <boost/scope_exit.hpp>
#include <iomanip>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std::chrono;
using namespace boost::program_options;
using boost::filesystem::path;

using st::FileEx;
using st::FileExPtr;
using st::make_cached;
using st::fixed_length_data::DigitString;
using TA_Base_Ex::CorbaUtilEx;
using namespace TA_Base_Core;
using namespace TA_IRS_App;
using namespace STIS_PROTOCOL;
using TemplatePreview = TemplatePreviewProcess;

namespace
{
    enum class ESubmitMessageToSTIS
    {
        NotSubmit,
        Success,
        Failed
    };

    const DWORD COLOR_RED = RGB(255, 0, 0);
    const COLORREF EMERGENCY_MESSAGE_TEXT_COLOR = RGB(255, 0, 0);
    const COLORREF NORMAL_MESSAGE_TEXT_COLOR = RGB(0, 0, 0);
    thread_local DestinationList sl_destinations;
    thread_local ESubmitMessageToSTIS sl_submit_message_to_stis = ESubmitMessageToSTIS::NotSubmit;  // sl: static thread-local

    bool is_success(ESubmitMessageToSTIS e)
    {
        return ESubmitMessageToSTIS::Success == e;
    }

    bool is_submit_message_to_stis_success()
    {
        return is_success(sl_submit_message_to_stis);
    }

    template <class F>
    bool try_submit_message_to_stis_or_show_error(std::string request, F&& f)
	{
		std::string errMsg;

		try
		{
			std::invoke(f);
			return is_success(sl_submit_message_to_stis = ESubmitMessageToSTIS::Success);
		}
		catch (const TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationTimeoutException&)
		{
			LOG_EXCEPTION("ISTISManagerCorbaDef::STISCommunicationTimeoutException", "");
			errMsg = "等待旅客資訊系統回應逾時";
		}
		catch (const TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationFailureException&)
		{
			LOG_EXCEPTION("ISTISManagerCorbaDef::STISCommunicationFailureException", "");
			errMsg = "旅客資訊系統通訊錯誤";
		}
		catch (const TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException&)
		{
			LOG_EXCEPTION("ISTISManagerCorbaDef::STISServerNotConnectedException", "");
			errMsg = "旅客資訊服務未連線至旅客資訊系統";
		}
		catch (const TA_Base_Bus::ISTISManagerCorbaDef::STISInvalidParameterException& e)
		{
			LOG_EXCEPTION("ISTISManagerCorbaDef::STISInvalidParameterException", e.details.in());
			errMsg = e.details.in();
		}
		catch (const TA_Base_Core::ObjectResolutionException& e)
		{
			LOG_EXCEPTION("TA_Base_Core::ObjectResolutionException", e.what());
			errMsg = "無法連線至旅客資訊服務";
		}
		catch (const CORBA::Exception& e)
		{
			LOG_EXCEPTION("CORBA::Exception", CorbaUtilEx::to_string(e));
			errMsg = "無法連線至旅客資訊服務";
		}
		catch (const TransactiveException& e)
		{
			LOG_EXCEPTION("TransactiveException", e.what());
			errMsg = e.what();
		}
		catch (const std::exception& e)
		{
			LOG_EXCEPTION("std::exception", e.what());
			errMsg = e.what();
		}
		catch (...)
		{
			LOG_EXCEPTION("...", "While submitting Free Text Display request");
			errMsg = "無法連線至旅客資訊服務";
		}

		UserMessages::getInstance().displayError(
			str(boost::format(UserMessages::ERROR_REQUEST_FAILED) % request % errMsg));

		return is_success(sl_submit_message_to_stis = ESubmitMessageToSTIS::Failed);
	}

    struct SplitButtonDropDownDelayer
    {
        bool delay()
        {
            if (steady_clock::now() - m_timestamp < m_delay)
            {
                return true;
            }

            m_timestamp = steady_clock::now();
            return false;
        }

        void update()
        {
            m_timestamp = steady_clock::now();
        }

        operator bool()
        {
            return delay();
        }

        steady_clock::duration m_delay = 200ms;
        steady_clock::time_point m_timestamp = steady_clock::now() - 1s;
    };

    SplitButtonDropDownDelayer s_location_split_button_dropdown_delayer;
    SplitButtonDropDownDelayer s_area_split_button_dropdown_delayer;
}

namespace TA_IRS_App
{
    // This is the biggest repeat interval
    //static const unsigned short MAX_REPEAT_INTERVAL = 999;

    DisplayPage::DisplayPage(CWnd* pParent /*=NULL*/)
        : CDialog(DisplayPage::IDD, pParent),
        m_selectedMessage(std::make_shared<SimpleUnicodeStatic>(this, IDC_SELECTED_MESSAGE))
    {
        FUNCTION_ENTRY("DisplayPage");
        // {{AFX_DATA_INIT(DisplayPage)
        // }}AFX_DATA_INIT
        FUNCTION_EXIT;
    }

    DisplayPage::~DisplayPage()
    {
        FUNCTION_ENTRY("~DisplayPage");

        ThreadGuard guard1(MainTab::s_dipalyThreadLock);

        RightsManager::getInstance().deregisterForRightsChanges(this);

        m_messageTypeTab.setMessageSelectionListener(NULL);

        // clean up PID selection
        delete m_pidSelectionManager;
        m_pidSelectionManager = NULL;

        // clean up time controls
        m_templateTimeControlManager.reset();
        m_messageTimeControlManager.reset();

        delete m_priorityManager;
        m_priorityManager = NULL;

        //m_stisPredefinedMessages->removeInstance();  // TD11310 ~ added

        FUNCTION_EXIT;
    }

    void DisplayPage::DoDataExchange(CDataExchange* pDX)
    {
        FUNCTION_ENTRY("DoDataExchange");

        CDialog::DoDataExchange(pDX);
        // {{AFX_DATA_MAP(DisplayPage)
        DDX_Control(pDX, IDC_DISPLAY_MESSAGE, m_displayMessageButton);
        DDX_Control(pDX, IDC_DISPLAY_TEMPLATE, m_displayTemplateButton);
        DDX_Control(pDX, IDC_DISPLAY_ALL, m_displayAllButton);
        DDX_Control(pDX, IDC_CLEAR_NORMAL_MESSAGE, m_clearNormalMessageButton);
        DDX_Control(pDX, IDC_CLEAR_EMERGENCY_MESSAGE_AND_TEMPLATE, m_clearEmergencyMessageAndTemplateButton);
        DDX_Control(pDX, IDC_CLEAR_NORMAL_TEMPLATE, m_clearNormalTemplateButton);
        DDX_Control(pDX, IDC_CLEAR_ALL, m_clearAllButton);
        DDX_Control(pDX, IDC_MESSAGE_TYPE_TAB, m_messageTypeTab);
        DDX_Control(pDX, IDC_PRIORITY, m_priority);
        //DDX_Control(pDX, IDC_RADIO_TIMED, m_radioTimed);
        //DDX_Control(pDX, IDC_RADIO_CONTINUOUS, m_radioContinuous);
        DDX_Control(pDX, IDC_START_DATE1, m_templateStartDate);
        DDX_Control(pDX, IDC_START_TIME1, m_templateStartTime);
        DDX_Control(pDX, IDC_END_DATE1, m_templateEndDate);
        DDX_Control(pDX, IDC_END_TIME1, m_templateEndTime);
        DDX_Control(pDX, IDC_START_DATE2, m_messageStartDate);
        DDX_Control(pDX, IDC_START_TIME2, m_messageStartTime);
        DDX_Control(pDX, IDC_END_DATE2, m_messageEndDate);
        DDX_Control(pDX, IDC_END_TIME2, m_messageEndTime);
        //DDX_Control(pDX, IDC_REPEAT_INTERVAL, m_repeatInterval);
        DDX_Control(pDX, IDC_START_TIME_LABEL, m_startTimeLabel);
        //DDX_Control(pDX, IDC_REPEAT_LABEL, m_repeatLabel);
        DDX_Control(pDX, IDC_END_TIME_LABEL, m_endTimeLabel);
        DDX_Control(pDX, IDC_PID_LIST, m_PIDList);
        DDX_Control(pDX, IDC_PID_TREE, m_PIDTree);
        DDX_Control(pDX, IDC_PID_LOCATION_FILTER_TREE, m_location_filter);
        DDX_Control(pDX, IDC_PID_AREA_FILTER_TREE, m_area_filter);
        DDX_Control(pDX, IDC_STN_PID_GROUP_COMBO, m_pidGroupCombo);
        DDX_Control(pDX, IDC_DELETE_GROUP, m_deleteGroupButton);
        DDX_Control(pDX, IDC_SAVE_GROUP, m_saveGroupButton);
        DDX_Control(pDX, IDC_LCD_LABEL, m_lcdTemplate);
        DDX_Control(pDX, IDC_LED_LABEL, m_ledTemplate);
        // DDX_Control(pDX, IDC_SELECTED_MESSAGE, m_selectedMessage);
        // }}AFX_DATA_MAP

        FUNCTION_EXIT;
    }

    BEGIN_MESSAGE_MAP(DisplayPage, CDialog)
        // {{AFX_MSG_MAP(DisplayPage)
        ON_WM_DESTROY()
        ON_WM_CONTEXTMENU()
        ON_WM_LBUTTONDOWN()
        ON_BN_CLICKED(IDC_DISPLAY_MESSAGE, onDisplayMessage)
        ON_BN_CLICKED(IDC_DISPLAY_TEMPLATE, onDisplayTemplate)
        ON_BN_CLICKED(IDC_DISPLAY_ALL, onDisplayAll)
        ON_BN_CLICKED(IDC_CLEAR_ALL, onClearAll)
        ON_BN_CLICKED(IDC_CLEAR_NORMAL_MESSAGE, onClearNormalMessage)
        ON_BN_CLICKED(IDC_CLEAR_EMERGENCY_MESSAGE_AND_TEMPLATE, onClearEmergencyMessageAndTemplate)
        ON_BN_CLICKED(IDC_CLEAR_NORMAL_TEMPLATE, onClearNormalTemplate)
        ON_BN_CLICKED(IDC_LCD_PREVIEW, onLcdPreview)
        ON_BN_CLICKED(IDC_LED_PREVIEW, onLedPreview)
        ON_LBN_SELCHANGE(IDC_PRIORITY, OnSelchangePriorityList)
        ON_MESSAGE(WM_UPDATE_RIGHTS, onRightsChanged)
        ON_MESSAGE_VOID(WM_KICKIDLE, OnKickIdle)
        ON_UPDATE_COMMAND_UI(IDC_LCD_LABEL, onUpdateLcdLabel)
        ON_UPDATE_COMMAND_UI(IDC_LED_LABEL, onUpdateLedLabel)
        ON_UPDATE_COMMAND_UI(IDC_PRIORITY, onUpdatePriority)
        ON_BN_CLICKED(IDC_SPLIT_LOCATION, OnBnClickedSplitLocation)
        ON_BN_CLICKED(IDC_SPLIT_AREA, OnBnClickedSplitArea)
        ON_NOTIFY(NM_KILLFOCUS, IDC_PID_LOCATION_FILTER_TREE, OnNMKillfocusPidLocationFilterTree)
        ON_NOTIFY(NM_KILLFOCUS, IDC_PID_AREA_FILTER_TREE, OnNMKillfocusPidAreaFilterTree)
        ON_NOTIFY(BCN_DROPDOWN, IDC_SPLIT_LOCATION, OnBnDropDownSplitLocation)
        ON_NOTIFY(BCN_DROPDOWN, IDC_SPLIT_AREA, OnBnDropDownSplitArea)
        ON_COMMAND(ID_SELECTEDPID_REMOVE, OnRemoveSelectedPID)
        ON_NOTIFY(LVN_KEYDOWN, IDC_PID_LIST, OnLvnKeydownPidList)
        ON_BN_CLICKED(IDC_PID_CONTROL_BUTTON, onPIDControl)
        // }}AFX_MSG_MAP
    END_MESSAGE_MAP()

    BOOL DisplayPage::OnInitDialog()
    {
        FUNCTION_ENTRY("OnInitDialog");

        CDialog::OnInitDialog();

        // set up PID selection
        m_pidSelectionManager = new PidSelectionManager(m_PIDList,
                                                        m_PIDTree,
                                                        m_location_filter,
                                                        m_area_filter,
                                                        m_pidGroupCombo,
                                                        m_saveGroupButton,
                                                        m_deleteGroupButton,
                                                        *this);

        // set up the time controls
        try
        {
            LOG_INFO("Attempting to create new TimeControlManager");

            m_templateTimeControlManager = std::make_shared<TimeControlManager>(m_templateStartDate, m_templateStartTime, m_templateEndDate, m_templateEndTime);
            m_messageTimeControlManager = std::make_shared<TimeControlManager>(m_messageStartDate, m_messageStartTime, m_messageEndDate, m_messageEndTime);

            m_priorityManager = new PriorityManager(m_priority);
            //m_messageTypeTab.getTimeTypeLisener());
        }
        catch (...)
        {
            LOG_INFO("Caught an unknown exception while creating new TimeControlManager");
        }

        // emergency priorities are red
        m_priority.mapItemDataToColour(1, COLOR_RED);
        m_priority.mapItemDataToColour(2, COLOR_RED);
        m_priority.mapItemDataToColour(3, COLOR_RED);

        m_clearNormalMessageButton.EnableWindow(FALSE);
        m_clearEmergencyMessageAndTemplateButton.EnableWindow(FALSE);
        m_clearNormalTemplateButton.EnableWindow(FALSE);
        m_clearAllButton.EnableWindow(FALSE);

        // set the repeat interval range (0 to 255 minutes)
        /*CTime lowerTime(1971, 1, 1, 0, 0, 0, 0);
        // set the repeat interval
        unsigned short hours = MAX_REPEAT_INTERVAL / 60;
        unsigned short minutes = MAX_REPEAT_INTERVAL - (hours * 60);
        CTime temp(1971, 1, 1, hours, minutes, 0, 0);
        CTime upperTime(1971, 1, 1, hours, minutes, 0, 0);*/
        //m_repeatInterval.SetRange(&lowerTime, &upperTime);

        // set date time control formate
        m_templateStartDate.SetFormat(_T(" dd-MMM-yyyy"));
        m_templateStartTime.SetFormat(_T(" HH:mm:ss"));
        m_templateEndDate.SetFormat(_T(" dd-MMM-yyyy"));
        m_templateEndTime.SetFormat(_T(" HH:mm:ss"));
        m_messageStartDate.SetFormat(_T(" dd-MMM-yyyy"));
        m_messageStartTime.SetFormat(_T(" HH:mm:ss"));
        m_messageEndDate.SetFormat(_T(" dd-MMM-yyyy"));
        m_messageEndTime.SetFormat(_T(" HH:mm:ss"));

        m_selectedMessage->init();

        EnableToolTips(TRUE);

        m_area_filter_tooltip.Create(this);
        m_area_filter_tooltip.AddTool(GetDlgItem(IDC_SPLIT_AREA), "All");
        m_area_filter_tooltip.Activate(TRUE);

        if (ThisLocation::is_occ())
        {
            m_location_filter_tooltip.Create(this);
            m_location_filter_tooltip.AddTool(GetDlgItem(IDC_SPLIT_LOCATION), "All");
            m_location_filter_tooltip.Activate(TRUE);
        }
        else
        {
            GetDlgItem(IDC_SPLIT_LOCATION)->EnableWindow(FALSE);
            GetDlgItem(IDC_SPLIT_LOCATION)->SetWindowText(ThisLocation::display_name().c_str());
        }

        m_area_filter_tooltip.Create(this);
        m_area_filter_tooltip.AddTool(GetDlgItem(IDC_SPLIT_AREA), "All");
        m_area_filter_tooltip.Activate(TRUE);

        FUNCTION_EXIT;
        return TRUE;  // return TRUE  unless you set the focus to a control
    }

    void DisplayPage::OnKickIdle()
    {
        UpdateDialogControls(this, FALSE);
    }

    void DisplayPage::initAll(/*REBProgressManager & mgr*/)
    {
        FUNCTION_ENTRY("initAll");

        //mgr.SetStaticText(0, "Initializing the Display Page: get rights");
        // access control
        m_stisPredefinedMessages = STISPredefinedMessages::getInstance();   // TD11310
        //mgr.SetProgress(10);
        m_canClear = RightsManager::getInstance().canClear();
        m_canModifyPidGroups = RightsManager::getInstance().canModifyPIDGroups();
        //mgr.SetProgress(20);
        RightsManager::getInstance().registerForRightsChanges(this);

        //mgr.SetProgress(30);
        m_pidSelectionManager->init();

        m_pidSelectionManager->setPermissions(m_canModifyPidGroups);

        //mgr.SetStaticText(0, "Initializing the Display Page: populate the message types");

        //mgr.SetProgress(40);
        m_messageTypeTab.setMessageSelectionListener(this);

        m_messageTypeTab.initAll();

        Invalidate();

        FUNCTION_EXIT;
    }

    void DisplayPage::OnDestroy()
    {
        FUNCTION_ENTRY("OnDestroy");

        CDialog::OnDestroy();
        TemplatePreview::remove();

        FUNCTION_EXIT;
    }

    void DisplayPage::pidSelectionChanged(bool pidsInList, bool pidSelectionExists)
    {
        FUNCTION_ENTRY("pidSelectionChanged");

        // enable buttons based on PID selection
        m_pidsInList = pidsInList;
        m_pidSelected = pidSelectionExists;
        std::tie(m_pidLcdSelected, m_pidLedSelected) = m_pidSelectionManager->is_lcd_led_selected();

        // TES 880 - this was changed from (m_pidSelected && m_canClear)
        // because a clear request now goes to all PIDs in the list
        // you can no longer select individual items in the PID list

#if 0
        m_clearNormalMessageButton.EnableWindow(m_pidsInList && m_canClear);
        m_clearEmergencyMessageAndTemplateButton.EnableWindow(m_pidsInList && m_canClear);
        m_clearNormalTemplateButton.EnableWindow(m_pidsInList && m_canClear);
        m_clearAllButton.EnableWindow(m_pidsInList && m_canClear);

        m_displayMessageButton.EnableWindow(m_pidsInList && m_validMessageSelected && m_canDisplay);
        m_displayTemplateButton.EnableWindow(m_pidsInList && m_validTemplateSelected && m_canDisplay);
        m_displayAllButton.EnableWindow(m_pidsInList && m_validMessageSelected && m_validTemplateSelected && m_canDisplay);
#endif

        updateControls();
        FUNCTION_EXIT;
    }

    void DisplayPage::predefinedMessageSelected(bool tabSwitched,
                                                bool validMessageSelected,
                                                const char* title,
                                                unsigned short priority)
    {
        FUNCTION_ENTRY("predefinedMessageSelected");

        if ((tabSwitched && validMessageSelected) || !tabSwitched)
        {
            m_selectedMessageType = PREDEFINED_ITEM;
            m_validPredefinedMessageSelected = validMessageSelected;

            // set up the time controls
            if (tabSwitched)
            {
                // check access control
                m_canDisplay = RightsManager::getInstance().canDisplayPredefined();
            }

            if (m_validPredefinedMessageSelected)
            {
                m_priorityManager->setPriority(priority, false);
            }
            else
            {
                m_priorityManager->blankAndDisableTimeAndPriority();

                // default adhoc attributes
                m_priorityManager->setPriority(priority, false);
            }

            // m_displayMessageButton.EnableWindow(m_pidsInList && m_validPredefinedMessageSelected && m_canDisplay);
            m_selectedMessage->set_window_text_utf8(m_validPredefinedMessageSelected ? title : "");
            updateControls();
        }

        FUNCTION_EXIT;
    }

    void DisplayPage::adHocMessageSelected(bool tabSwitched, bool hasSelection, bool validMessageEntered, std::string title)
    {
        FUNCTION_ENTRY("adHocMessageSelected");

        if ((tabSwitched && hasSelection) || !tabSwitched)
        {
            m_selectedMessageType = FREE_TEXT_ITEM;
            m_validAdhocMessageSelected = validMessageEntered;

            //m_displayMessageButton.EnableWindow(m_pidsInList && m_validAdhocMessageSelected && m_canDisplay);
            m_selectedMessage->set_window_text_utf8(m_validAdhocMessageSelected ? title.c_str() : "");
#if 0
            m_priorityManager->setPriority(m_adhocInfo.priority, true);
#else
            m_priorityManager->setPriority(m_adhocInfo.priority, validMessageEntered && title.size());
#endif
            updateControls();
        }

        FUNCTION_EXIT;
    }

    void DisplayPage::templateSelected(bool tabSwitched, bool validLcdTemplateSelected, bool validLedTemplateSelected, TemplatePtr lcd, TemplatePtr led)
    {
        FUNCTION_ENTRY("templateSelected");

        //TA_ASSERT(m_timeControlManager != NULL, "Time Control manager cant be null");

        // set up the time controls
        if (tabSwitched)
        {
            // check access control
            m_canDisplay = RightsManager::getInstance().canDisplayTemplate();
        }

        // m_displayTemplateButton.EnableWindow(m_pidsInList && m_validTemplateSelected && m_canDisplay);

        m_lcdTemplate.SetWindowText(lcd ? lcd->description.c_str() : "");
        m_ledTemplate.SetWindowText(led ? led->description.c_str() : "");

        updateControls();

        FUNCTION_EXIT;
    }

    void DisplayPage::predefinedMessageSelectChanged(const std::string description)
    {
        FUNCTION_ENTRY("predefinedMessageSelected");

        m_selectedMessage->set_window_text_utf8(description);

        FUNCTION_EXIT;
    }

    void DisplayPage::templateSelectChanged(const Template& tmp)
    {
        FUNCTION_ENTRY("predefinedMessageSelected");

        if (tmp.templateType == 1 && tmp.templateType == 2)
        {
            m_lcdTemplate.SetWindowText(tmp.description.c_str());
        }
        else if (tmp.templateType == 3 && tmp.templateType == 4)
        {
            m_ledTemplate.SetWindowText(tmp.description.c_str());
        }

        if (tmp.templateType == 1 && tmp.templateType == 3)
        {
            m_templateTimeControlManager->enable(true);
        }
        else if (tmp.templateType == 2 && tmp.templateType == 4)
        {
            m_templateTimeControlManager->enable(false);
        }

        FUNCTION_EXIT;
    }

    LRESULT DisplayPage::onRightsChanged(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY("onRightsChanged");

        // the rights have changed - re check them
        m_canClear = RightsManager::getInstance().canClear();
        m_canModifyPidGroups = RightsManager::getInstance().canModifyPIDGroups();

        LOG_INFO("Calling setPermissions() from onRightsChanged");
        m_pidSelectionManager->setPermissions(m_canModifyPidGroups);

        LOG_INFO("Calling setPermissions() successfully");
        // cause a state change to trigger one of the above functions
        m_messageTypeTab.setMessageSelectionListener(this);

        updateControls();

        FUNCTION_EXIT;
        return 0;
    }

    void DisplayPage::onClearAll()
    {
        FUNCTION_ENTRY("onClearAll");

        std::string confirmationMessage = str(boost::format(UserMessages::QUESTION_CLEAR) % "全部");

        // Display the confirmation dialog
        if (UserMessages::getInstance().askQuestionUTF8(confirmationMessage.c_str()) != IDYES)
        {
            sl_submit_message_to_stis = ESubmitMessageToSTIS::NotSubmit;
            FUNCTION_EXIT;
            return;
        }

        // TES 880 - now a destination list for all pids in list
        // m_pidSelectionManager->populateClearDestination( destination );
        sl_destinations = getDestinations();
        std::vector<int> priority(8, 1);
        //std::string clearType = "Clear All";
		std::string clearType = "清除全部";

        try_submit_message_to_stis_or_show_error(clearType, [&]
        {
            STISClient::instance().submit_M20_ClearCurrentMessageRequestList(sl_destinations, priority);
            STISClient::instance().submit_M23_RemoveDisplayTemplateRequestList(sl_destinations, EDisplayTemplateType::Default);
            STISAuditMessage::clearNormalTemplate(sl_destinations);
            STISAuditMessage::clearNormalMessage(sl_destinations);
            STISAuditMessage::clearEmergencyMessageAndTemplate(sl_destinations);
            UserMessages::getInstance().displayInfo(str(boost::format(UserMessages::INFO_REQUEST_SUCCESSFUL) % clearType));
        });

        FUNCTION_EXIT;
    }

    void DisplayPage::onClearNormalMessage()
    {
        FUNCTION_ENTRY("onClearNormalMessage");

        //submitClearMessageRequest("Clear Normal Message", 4, 8);
		submitClearMessageRequest("清除普通訊息", 4, 8);
        STISAuditMessage::clearNormalMessageIf(is_submit_message_to_stis_success(), sl_destinations);

        FUNCTION_EXIT;
    }

    void DisplayPage::onClearEmergencyMessageAndTemplate()
    {
        FUNCTION_ENTRY("onClearEmergencyMessageAndTemplate");

        auto clear_type = "清除緊急訊息和緊急模板";
        auto question = str(boost::format(UserMessages::QUESTION_CLEAR) % clear_type);

        if (UserMessages::getInstance().askQuestionUTF8(question.c_str()) != IDYES)
        {
            sl_submit_message_to_stis = ESubmitMessageToSTIS::NotSubmit;
            FUNCTION_EXIT;
            return;
        }

        if (submitClearMessageRequest("Clear Emergency Message", 1, 3, false, false))
        {
#if 0       // no need clear emergency template
            if (submitClearTemplateRequest({EDisplayTemplateType::LCDEmergency, EDisplayTemplateType::LEDEmergency}, "清除緊急模板", false, false))
#endif
            {
                STISAuditMessage::clearEmergencyMessageAndTemplate(sl_destinations);
                UserMessages::getInstance().displayInfo(str(boost::format(UserMessages::INFO_REQUEST_SUCCESSFUL) % clear_type));
            }
        }

        FUNCTION_EXIT;
    }

    void DisplayPage::onClearNormalTemplate()
    {
        FUNCTION_ENTRY("onClearNormalTemplate");

        //submitClearTemplateRequest({EDisplayTemplateType::LCDNormal, EDisplayTemplateType::LEDNormal}, "Clear Normal Template");
		submitClearTemplateRequest({EDisplayTemplateType::LCDNormal}, "清除普通模板");
        STISAuditMessage::clearNormalTemplateIf(is_submit_message_to_stis_success(), sl_destinations);

        FUNCTION_EXIT;
    }

    bool DisplayPage::submitClearMessageRequest(std::string clearType, int lowerPriority, int upperPriority, bool ask_question, bool display_info)
    {
        FUNCTION_ENTRY("submitClearMessageRequest");

        auto question = str(boost::format(UserMessages::QUESTION_CLEAR) % clearType);

        if (ask_question)
        {
            if (UserMessages::getInstance().askQuestionUTF8(question.c_str()) != IDYES)
            {
                FUNCTION_EXIT;
                return is_success(sl_submit_message_to_stis = ESubmitMessageToSTIS::NotSubmit);
            }
        }

        // TES 880 - now a destination list for all pids in list
        sl_destinations = getDestinations();
        auto priority = make_priority_list(lowerPriority, upperPriority);

        FUNCTION_EXIT;
        return try_submit_message_to_stis_or_show_error(clearType, [&]
        {
            STISClient::instance().submit_M20_ClearCurrentMessageRequestList(sl_destinations, priority);
            UserMessages::getInstance().displayInfo_if(display_info, str(boost::format(UserMessages::INFO_REQUEST_SUCCESSFUL) % clearType));
        });
    }

    bool DisplayPage::submitClearTemplateRequest(std::list<EDisplayTemplateType> typeList, std::string clearType, bool ask_question, bool display_info)
    {
        FUNCTION_ENTRY("submitClearTemplateRequest");

        auto question = str(boost::format(UserMessages::QUESTION_CLEAR) % clearType);

        if (ask_question)
        {
            // Display the confirmation dialog
            if (UserMessages::getInstance().askQuestionUTF8(question.c_str()) != IDYES)
            {
                FUNCTION_EXIT;
                return is_success(sl_submit_message_to_stis = ESubmitMessageToSTIS::NotSubmit);
            }
        }

        sl_destinations = getDestinations();

        FUNCTION_EXIT;
        return try_submit_message_to_stis_or_show_error(clearType, [&]
        {
            for (auto type : typeList)
            {
                STISClient::instance().submit_M23_RemoveDisplayTemplateRequestList(sl_destinations, type);
            }

            UserMessages::getInstance().displayInfo_if(display_info, str(boost::format(UserMessages::INFO_REQUEST_SUCCESSFUL) % clearType));
        });
    }

    void DisplayPage::OnOK()
    {
        FUNCTION_ENTRY("OnOK");
        FUNCTION_EXIT;
    }

    void DisplayPage::OnCancel()
    {
        FUNCTION_ENTRY("OnCancel");
        FUNCTION_EXIT;
    }

    BOOL DisplayPage::PreTranslateMessage(MSG* pMsg)
    {
        m_location_filter_tooltip.RelayEvent(pMsg);
        m_area_filter_tooltip.RelayEvent(pMsg);
        // m_location_filter.onPreTranslateMessage(pMsg);
        return CDialog::PreTranslateMessage(pMsg);
    }

    bool DisplayPage::findAndSelectStationMessage(const std::string& messageName)
    {
        FUNCTION_ENTRY("findAndSelectStationMessage");
        FUNCTION_EXIT;
        return m_messageTypeTab.findAndSelectStationMessage(messageName);
    }

    std::vector<int> DisplayPage::make_priority_list(int lowerPriority, int upperPriority)
    {
        static auto s_func = make_cached([](int lower, int upper)
        {
            std::vector<int> priority(8, 0);
            std::fill_n(next(priority.begin(), lower - 1), upper - lower + 1, 1);
            return priority;
        });

        return s_func(lowerPriority, upperPriority);
    }

    void DisplayPage::onDisplayMessage()
    {
        FUNCTION_ENTRY("onDisplayMessage");

        CWaitCursor cursor;

        if (!validateMessageStartEndDateTime())
        {
            FUNCTION_EXIT;
            return;
        }

        sl_destinations = getDestinations();
        TA_ASSERT(sl_destinations.size(), "The display button should not be active if there are no PIDs in the list");

        auto&& [msg, type, info] = constructSelectedMessage();

        std::string confirmationMessage = str(boost::format(UserMessages::QUESTION_DISPLAY) % type % info);

        if (UserMessages::getInstance().askQuestionUTF8(confirmationMessage) != IDYES)
        {
            FUNCTION_EXIT;
            sl_submit_message_to_stis = ESubmitMessageToSTIS::NotSubmit;
            return;
        }

        try_submit_message_to_stis_or_show_error(type, [&]
        {
            if (std::holds_alternative<STIS_PROTOCOL::PredefinedMessage>(msg))
            {
                auto& msg_pre = std::get<STIS_PROTOCOL::PredefinedMessage>(msg);
                STISClient::instance().submit_M10_DisplayPredefinedMessageRequestList(sl_destinations, msg_pre);
                STISAuditMessage::sendPredefinedMessage(sl_destinations, std::to_string(msg_pre.priority), msg_pre.start_time, msg_pre.end_time);
            }
            else
            {
                auto& msg_adhoc = std::get<STIS_PROTOCOL::AdHodMessage>(msg);
                STISClient::instance().submit_M11_DisplayAdHocMessageRequestList(sl_destinations, msg_adhoc);
                STISAuditMessage::sendAdhocMessage(sl_destinations, std::to_string(msg_adhoc.priority), msg_adhoc.start_time, msg_adhoc.end_time);
            }

            UserMessages::getInstance().displayInfo(str(boost::format(UserMessages::INFO_REQUEST_SUCCESSFUL) % type));
        });

        FUNCTION_EXIT;
    }

    void DisplayPage::onDisplayTemplate()
    {
        FUNCTION_ENTRY("onDisplayTemplate");

        CWaitCursor cursor;

        if (!validateTemplateStartEndDateTime())
        {
            FUNCTION_EXIT;
            return;
        }

        sl_destinations = getDestinations();
        TA_ASSERT(sl_destinations.size(), "The display button should not be active if there are no PIDs in the list");

        auto&& [lcd, led, info] = constructSelectedDisplayTemplate();
        auto question = str(boost::format(UserMessages::QUESTION_DISPLAY_TEMPLATE) % info);

        if (UserMessages::getInstance().askQuestionUTF8(question.c_str()) != IDYES)
        {
            sl_submit_message_to_stis = ESubmitMessageToSTIS::NotSubmit;
            FUNCTION_EXIT;
            return;
        }

        try_submit_message_to_stis_or_show_error("顯示模板", [&]
        {
            STISClient::instance().submit_M22_SendPredefinedDisplayTemplateRequestList(sl_destinations, lcd, led);
            UserMessages::getInstance().displayInfo(str(boost::format(UserMessages::INFO_REQUEST_SUCCESSFUL) % "顯示模板"));
            STISAuditMessage::sendPredefinedTemplate(sl_destinations, isEmergencyLcdOrLedSelected(), getTemplateStartTime(), getTemplateEndTime());
        });

        FUNCTION_EXIT;
    }

    void DisplayPage::onDisplayAll()
    {
        FUNCTION_ENTRY("onDisplayAll");

        CWaitCursor cursor;

        if (!validateAllStartEndDateTime())
        {
            sl_submit_message_to_stis = ESubmitMessageToSTIS::NotSubmit;
            FUNCTION_EXIT;
            return;
        }

        sl_destinations = getDestinations();
        TA_ASSERT(sl_destinations.size(), "The display button should not be active if there are no PIDs in the list");

        auto&& [msg, type, info1] = constructSelectedMessage();
        auto&& [lcd, led, info2] = constructSelectedDisplayTemplate();
        std::string confirmationMessage = str(boost::format(UserMessages::QUESTION_DISPLAY_MESSAGE_TEMPLATE) % info1 % info2);

        if (UserMessages::getInstance().askQuestionUTF8(confirmationMessage.c_str()) != IDYES)
        {
            sl_submit_message_to_stis = ESubmitMessageToSTIS::NotSubmit;
            FUNCTION_EXIT;
            return;
        }

        try_submit_message_to_stis_or_show_error(type + ", 顯示模板", [&]
        {
            if (std::holds_alternative<STIS_PROTOCOL::PredefinedMessage>(msg))
            {
                auto& msg_pre = std::get<STIS_PROTOCOL::PredefinedMessage>(msg);
                STISClient::instance().submit_M10_DisplayPredefinedMessageRequestList(sl_destinations, msg_pre);
                STISAuditMessage::sendPredefinedMessage(sl_destinations, std::to_string(msg_pre.priority), msg_pre.start_time, msg_pre.end_time);
            }
            else
            {
                auto& msg_adhoc = std::get<STIS_PROTOCOL::AdHodMessage>(msg);
                STISClient::instance().submit_M11_DisplayAdHocMessageRequestList(sl_destinations, msg_adhoc);
                STISAuditMessage::sendAdhocMessage(sl_destinations, std::to_string(msg_adhoc.priority), msg_adhoc.start_time, msg_adhoc.end_time);
            }

            if (!m_priorityManager->isEmergency())
            {
                STISClient::instance().submit_M22_SendPredefinedDisplayTemplateRequestList(sl_destinations, lcd, led);
            }

            UserMessages::getInstance().displayInfo(str(boost::format(UserMessages::INFO_REQUEST_SUCCESSFUL) % (type + ", 顯示模板")));
            STISAuditMessage::sendPredefinedTemplate(sl_destinations, isEmergencyLcdOrLedSelected() || isEmergencyMessageSelected(), getTemplateStartTime(), getTemplateEndTime());
        });

        FUNCTION_EXIT;
    }

    std::tuple<PredefinedDisplayTemplate, PredefinedDisplayTemplate, std::string> DisplayPage::constructSelectedDisplayTemplate()
    {
        std::stringstream ss;
        PredefinedDisplayTemplate lcd;
        PredefinedDisplayTemplate led; // Spare - reserved for future (no LED for TYGL CMFT)

        auto&& [lcdTemp, ledTemp] = m_messageTypeTab.getLcdLedTemplate();

        if (lcdTemp)
        {
            lcd.display_template_type = static_cast<EDisplayTemplateType>(lcdTemp->templateType);
            lcd.display_template_id = DigitString<3>{lcdTemp->templateID};
            lcd.start_time = m_templateTimeControlManager->getStartDateTimeString();
            lcd.end_time = m_templateTimeControlManager->getEndDateTimeString();
            ss << "LCD: " << lcdTemp->description;
        }
        else
        {
            ss << "LCD: " << get_default_lcd_led_template();
        }

        return {lcd, led, ss.str()};
    }

    std::tuple<PredefinedAdHocMessage, std::string, std::string> DisplayPage::constructSelectedMessage()
    {
        auto fill_template = [&](auto& res)
        {
            if (m_priorityManager->isEmergency())
            {
                auto&& [lcd, led] = m_messageTypeTab.getLcdLedTemplate();

                if (lcd && m_messageTypeTab.templatePage().isEmergencyLcdSelected())
                {
                    res.lcd_emergency_display_template_id = DigitString<3>{lcd->templateID};
                    res.lcd_emergency_display_template_type = ETemplateType::EmergencyLcd;
                }

                if (led && m_messageTypeTab.templatePage().isEmergencyLedSelected())
                {
                    res.led_emergency_display_template_id = DigitString<3>{led->templateID};
                    res.led_emergency_display_template_type = ETemplateType::EmergencyLed;
                }
            }
        };

        auto [startTime, endTime, _] = m_messageTimeControlManager->validate();

        if (m_messageTypeTab.isLastMessageSelectionPredefined() && m_messageTypeTab.predefinedPage().hasValidSelection())
        {
            STIS_PROTOCOL::PredefinedMessage res;
            auto msg = m_messageTypeTab.getPredefinedMessage();
            res.message_tag = DigitString<4>{msg.messageTag};
            res.priority = m_priorityManager->getPriority();
            res.start_time = startTime;
            res.end_time = endTime;
            fill_template(res);
            return {res, "預定義訊息", msg.message};
        }
        else if (m_messageTypeTab.isLastMessageSelectionAdHoc() && m_messageTypeTab.adHocPage().hasValidSelection())
        {
            AdHodMessage res;
            auto msg = m_messageTypeTab.getFreeTextMessage();
#if 0
            res.message_text = msg.messageContent;
#else
            auto utf8 = STIS_UTILITY::join_4_languages_utf8({msg.messageContent});
            res.message_text = STIS_UTILITY::transform_4_languages_from_utf8_to_utf16(utf8);
#endif
            res.priority = m_priorityManager->getPriority();
            res.start_time = startTime;
            res.end_time = endTime;
            res.message_tag = st::get_time_YYYYMMDDHHMMSS().substr(2);
            fill_template(res);
            return {res, "臨時訊息", msg.messageContent};
        }

        return {};
    }

    void DisplayPage::OnSelchangePriorityList()
    {
        if (m_messageTypeTab.GetCurSel() == 1) // adhoc
        {
            m_adhocInfo.priority = m_priorityManager->getPriority();
        }

        updateControls();
    }

    /**
     * The emergency template is only applicable to emergency message display.
     * An emergency message has to be selected followed by an emergency template.
     * J155 shall ensure that the emergency template must be send together with emergency message from the MFT GUI.
     *
     * NOTE:
     * - message-type must mach template-type
     * - for emergench message, template should send with message, can not be send separately
     */
    void DisplayPage::updateControls()
    {
        struct PidLcd { enum { No_, Yes, No = 0 }; };
        struct PidLed { enum { No_, Yes }; };
        struct Message { enum { No_______, Normal___, Emergency, No = 0, Normal = 1 }; };
        struct TemplateType { enum { Normal___, Emergency, Normal = 0 }; };
        struct TemplateLcd { enum { No_, Yes, No = 0 }; };
        struct TemplateLed { enum { No_, Yes, No = 0 }; };

        const std::size_t MessageButton = 0x01;
        const std::size_t TemplateButton = 0x02;
        const std::size_t AllButton = 0x04;
        const std::size_t MessageDateTime = 0x08;
        const std::size_t TemplateDateTime = 0x10;
        const std::size_t LcdPrevewButton = 0x20;
        const std::size_t LedPrevewButton = 0x40;

        using TemplateLedArray = std::array<size_t, 2>;
        using TemplateLcdLedArray = std::array<TemplateLedArray, 2>;
        using TemplateTypeLcdLedArray = std::array<TemplateLcdLedArray, 2>;
        using MessageTemplateTypeLcdLedArray = std::array<TemplateTypeLcdLedArray, 3>;
        using PidLedMessageTemplateTypeLcdLedArray = std::array<MessageTemplateTypeLcdLedArray, 2>;
        using PidLcdPidLedMessageTemplateTypeLcdLedArray = std::array<PidLedMessageTemplateTypeLcdLedArray, 2>;
        using Tensor = PidLcdPidLedMessageTemplateTypeLcdLedArray;

        static auto s_tensor = boost::hof::eval([&]
        {
            auto t = Tensor{};

            t[PidLcd::No_][PidLed::No_][Message::No_______][TemplateType::Normal___][TemplateLcd::No_][TemplateLed::No_] = 0;
            t[PidLcd::No_][PidLed::No_][Message::No_______][TemplateType::Normal___][TemplateLcd::No_][TemplateLed::Yes] = 0;
            t[PidLcd::No_][PidLed::No_][Message::No_______][TemplateType::Normal___][TemplateLcd::Yes][TemplateLed::No_] = 0;
            t[PidLcd::No_][PidLed::No_][Message::No_______][TemplateType::Normal___][TemplateLcd::Yes][TemplateLed::Yes] = 0;
            t[PidLcd::No_][PidLed::No_][Message::No_______][TemplateType::Emergency][TemplateLcd::No_][TemplateLed::No_] = 0;
            t[PidLcd::No_][PidLed::No_][Message::No_______][TemplateType::Emergency][TemplateLcd::No_][TemplateLed::Yes] = 0;
            t[PidLcd::No_][PidLed::No_][Message::No_______][TemplateType::Emergency][TemplateLcd::Yes][TemplateLed::No_] = 0;
            t[PidLcd::No_][PidLed::No_][Message::No_______][TemplateType::Emergency][TemplateLcd::Yes][TemplateLed::Yes] = 0;
            t[PidLcd::No_][PidLed::No_][Message::Normal___][TemplateType::Normal___][TemplateLcd::No_][TemplateLed::No_] = 0;
            t[PidLcd::No_][PidLed::No_][Message::Normal___][TemplateType::Normal___][TemplateLcd::No_][TemplateLed::Yes] = 0;
            t[PidLcd::No_][PidLed::No_][Message::Normal___][TemplateType::Normal___][TemplateLcd::Yes][TemplateLed::No_] = 0;
            t[PidLcd::No_][PidLed::No_][Message::Normal___][TemplateType::Normal___][TemplateLcd::Yes][TemplateLed::Yes] = 0;
            t[PidLcd::No_][PidLed::No_][Message::Normal___][TemplateType::Emergency][TemplateLcd::No_][TemplateLed::No_] = 0;
            t[PidLcd::No_][PidLed::No_][Message::Normal___][TemplateType::Emergency][TemplateLcd::No_][TemplateLed::Yes] = 0;
            t[PidLcd::No_][PidLed::No_][Message::Normal___][TemplateType::Emergency][TemplateLcd::Yes][TemplateLed::No_] = 0;
            t[PidLcd::No_][PidLed::No_][Message::Normal___][TemplateType::Emergency][TemplateLcd::Yes][TemplateLed::Yes] = 0;
            t[PidLcd::No_][PidLed::No_][Message::Emergency][TemplateType::Normal___][TemplateLcd::No_][TemplateLed::No_] = 0;
            t[PidLcd::No_][PidLed::No_][Message::Emergency][TemplateType::Normal___][TemplateLcd::No_][TemplateLed::Yes] = 0;
            t[PidLcd::No_][PidLed::No_][Message::Emergency][TemplateType::Normal___][TemplateLcd::Yes][TemplateLed::No_] = 0;
            t[PidLcd::No_][PidLed::No_][Message::Emergency][TemplateType::Normal___][TemplateLcd::Yes][TemplateLed::Yes] = 0;
            t[PidLcd::No_][PidLed::No_][Message::Emergency][TemplateType::Emergency][TemplateLcd::No_][TemplateLed::No_] = 0;
            t[PidLcd::No_][PidLed::No_][Message::Emergency][TemplateType::Emergency][TemplateLcd::No_][TemplateLed::Yes] = 0;
            t[PidLcd::No_][PidLed::No_][Message::Emergency][TemplateType::Emergency][TemplateLcd::Yes][TemplateLed::No_] = 0;
            t[PidLcd::No_][PidLed::No_][Message::Emergency][TemplateType::Emergency][TemplateLcd::Yes][TemplateLed::Yes] = 0;

            t[PidLcd::No_][PidLed::Yes][Message::No_______][TemplateType::Normal___][TemplateLcd::No_][TemplateLed::No_] = 0;
            t[PidLcd::No_][PidLed::Yes][Message::No_______][TemplateType::Normal___][TemplateLcd::No_][TemplateLed::Yes] = TemplateButton | TemplateDateTime | LedPrevewButton;
            t[PidLcd::No_][PidLed::Yes][Message::No_______][TemplateType::Normal___][TemplateLcd::Yes][TemplateLed::No_] = 0;
            t[PidLcd::No_][PidLed::Yes][Message::No_______][TemplateType::Normal___][TemplateLcd::Yes][TemplateLed::Yes] = TemplateButton | TemplateDateTime | LedPrevewButton;
            t[PidLcd::No_][PidLed::Yes][Message::No_______][TemplateType::Emergency][TemplateLcd::No_][TemplateLed::No_] = 0;
            t[PidLcd::No_][PidLed::Yes][Message::No_______][TemplateType::Emergency][TemplateLcd::No_][TemplateLed::Yes] = LedPrevewButton;
            t[PidLcd::No_][PidLed::Yes][Message::No_______][TemplateType::Emergency][TemplateLcd::Yes][TemplateLed::No_] = 0;
            t[PidLcd::No_][PidLed::Yes][Message::No_______][TemplateType::Emergency][TemplateLcd::Yes][TemplateLed::Yes] = LedPrevewButton;
            t[PidLcd::No_][PidLed::Yes][Message::Normal___][TemplateType::Normal___][TemplateLcd::No_][TemplateLed::No_] = MessageButton | MessageDateTime;
            t[PidLcd::No_][PidLed::Yes][Message::Normal___][TemplateType::Normal___][TemplateLcd::No_][TemplateLed::Yes] = MessageButton | MessageDateTime | TemplateButton | TemplateDateTime | LedPrevewButton | AllButton;
            t[PidLcd::No_][PidLed::Yes][Message::Normal___][TemplateType::Normal___][TemplateLcd::Yes][TemplateLed::No_] = MessageButton | MessageDateTime;
            t[PidLcd::No_][PidLed::Yes][Message::Normal___][TemplateType::Normal___][TemplateLcd::Yes][TemplateLed::Yes] = MessageButton | MessageDateTime | TemplateButton | TemplateDateTime | LedPrevewButton | AllButton;
            t[PidLcd::No_][PidLed::Yes][Message::Normal___][TemplateType::Emergency][TemplateLcd::No_][TemplateLed::No_] = 0;
            t[PidLcd::No_][PidLed::Yes][Message::Normal___][TemplateType::Emergency][TemplateLcd::No_][TemplateLed::Yes] = LedPrevewButton;
            t[PidLcd::No_][PidLed::Yes][Message::Normal___][TemplateType::Emergency][TemplateLcd::Yes][TemplateLed::No_] = 0;
            t[PidLcd::No_][PidLed::Yes][Message::Normal___][TemplateType::Emergency][TemplateLcd::Yes][TemplateLed::Yes] = LedPrevewButton;
            t[PidLcd::No_][PidLed::Yes][Message::Emergency][TemplateType::Normal___][TemplateLcd::No_][TemplateLed::No_] = MessageButton;
            t[PidLcd::No_][PidLed::Yes][Message::Emergency][TemplateType::Normal___][TemplateLcd::No_][TemplateLed::Yes] = LedPrevewButton;
            t[PidLcd::No_][PidLed::Yes][Message::Emergency][TemplateType::Normal___][TemplateLcd::Yes][TemplateLed::No_] = 0;
            t[PidLcd::No_][PidLed::Yes][Message::Emergency][TemplateType::Normal___][TemplateLcd::Yes][TemplateLed::Yes] = LedPrevewButton;
            t[PidLcd::No_][PidLed::Yes][Message::Emergency][TemplateType::Emergency][TemplateLcd::No_][TemplateLed::No_] = MessageButton;
            t[PidLcd::No_][PidLed::Yes][Message::Emergency][TemplateType::Emergency][TemplateLcd::No_][TemplateLed::Yes] = MessageButton | LedPrevewButton | AllButton;
            t[PidLcd::No_][PidLed::Yes][Message::Emergency][TemplateType::Emergency][TemplateLcd::Yes][TemplateLed::No_] = MessageButton;
            t[PidLcd::No_][PidLed::Yes][Message::Emergency][TemplateType::Emergency][TemplateLcd::Yes][TemplateLed::Yes] = MessageButton | LedPrevewButton | AllButton;

            t[PidLcd::Yes][PidLed::No_][Message::No_______][TemplateType::Normal___][TemplateLcd::No_][TemplateLed::No_] = 0;
            t[PidLcd::Yes][PidLed::No_][Message::No_______][TemplateType::Normal___][TemplateLcd::No_][TemplateLed::Yes] = 0;
            t[PidLcd::Yes][PidLed::No_][Message::No_______][TemplateType::Normal___][TemplateLcd::Yes][TemplateLed::No_] = TemplateButton | TemplateDateTime | LcdPrevewButton;
            t[PidLcd::Yes][PidLed::No_][Message::No_______][TemplateType::Normal___][TemplateLcd::Yes][TemplateLed::Yes] = TemplateButton | TemplateDateTime | LcdPrevewButton;
            t[PidLcd::Yes][PidLed::No_][Message::No_______][TemplateType::Emergency][TemplateLcd::No_][TemplateLed::No_] = 0;
            t[PidLcd::Yes][PidLed::No_][Message::No_______][TemplateType::Emergency][TemplateLcd::No_][TemplateLed::Yes] = 0;
            t[PidLcd::Yes][PidLed::No_][Message::No_______][TemplateType::Emergency][TemplateLcd::Yes][TemplateLed::No_] = LcdPrevewButton;
            t[PidLcd::Yes][PidLed::No_][Message::No_______][TemplateType::Emergency][TemplateLcd::Yes][TemplateLed::Yes] = LcdPrevewButton;
            t[PidLcd::Yes][PidLed::No_][Message::Normal___][TemplateType::Normal___][TemplateLcd::No_][TemplateLed::No_] = MessageButton | MessageDateTime;
            t[PidLcd::Yes][PidLed::No_][Message::Normal___][TemplateType::Normal___][TemplateLcd::No_][TemplateLed::Yes] = MessageButton | MessageDateTime;
            t[PidLcd::Yes][PidLed::No_][Message::Normal___][TemplateType::Normal___][TemplateLcd::Yes][TemplateLed::No_] = MessageButton | MessageDateTime | TemplateButton | TemplateDateTime | LcdPrevewButton | AllButton;
            t[PidLcd::Yes][PidLed::No_][Message::Normal___][TemplateType::Normal___][TemplateLcd::Yes][TemplateLed::Yes] = MessageButton | MessageDateTime | TemplateButton | TemplateDateTime | LcdPrevewButton | AllButton;
            t[PidLcd::Yes][PidLed::No_][Message::Normal___][TemplateType::Emergency][TemplateLcd::No_][TemplateLed::No_] = 0;
            t[PidLcd::Yes][PidLed::No_][Message::Normal___][TemplateType::Emergency][TemplateLcd::No_][TemplateLed::Yes] = 0;
            t[PidLcd::Yes][PidLed::No_][Message::Normal___][TemplateType::Emergency][TemplateLcd::Yes][TemplateLed::No_] = LcdPrevewButton;
            t[PidLcd::Yes][PidLed::No_][Message::Normal___][TemplateType::Emergency][TemplateLcd::Yes][TemplateLed::Yes] = LcdPrevewButton;
            t[PidLcd::Yes][PidLed::No_][Message::Emergency][TemplateType::Normal___][TemplateLcd::No_][TemplateLed::No_] = MessageButton;
            t[PidLcd::Yes][PidLed::No_][Message::Emergency][TemplateType::Normal___][TemplateLcd::No_][TemplateLed::Yes] = 0;
            t[PidLcd::Yes][PidLed::No_][Message::Emergency][TemplateType::Normal___][TemplateLcd::Yes][TemplateLed::No_] = LcdPrevewButton;
            t[PidLcd::Yes][PidLed::No_][Message::Emergency][TemplateType::Normal___][TemplateLcd::Yes][TemplateLed::Yes] = LcdPrevewButton;
            t[PidLcd::Yes][PidLed::No_][Message::Emergency][TemplateType::Emergency][TemplateLcd::No_][TemplateLed::No_] = MessageButton;
            t[PidLcd::Yes][PidLed::No_][Message::Emergency][TemplateType::Emergency][TemplateLcd::No_][TemplateLed::Yes] = MessageButton;
            t[PidLcd::Yes][PidLed::No_][Message::Emergency][TemplateType::Emergency][TemplateLcd::Yes][TemplateLed::No_] = MessageButton | LcdPrevewButton | AllButton;
            t[PidLcd::Yes][PidLed::No_][Message::Emergency][TemplateType::Emergency][TemplateLcd::Yes][TemplateLed::Yes] = MessageButton | LcdPrevewButton | AllButton;

            t[PidLcd::Yes][PidLed::Yes][Message::No_______][TemplateType::Normal___][TemplateLcd::No_][TemplateLed::No_] = 0;
            t[PidLcd::Yes][PidLed::Yes][Message::No_______][TemplateType::Normal___][TemplateLcd::No_][TemplateLed::Yes] = LedPrevewButton;
            t[PidLcd::Yes][PidLed::Yes][Message::No_______][TemplateType::Normal___][TemplateLcd::Yes][TemplateLed::No_] = LcdPrevewButton;
            t[PidLcd::Yes][PidLed::Yes][Message::No_______][TemplateType::Normal___][TemplateLcd::Yes][TemplateLed::Yes] = TemplateButton | TemplateDateTime | LcdPrevewButton | LedPrevewButton;
            t[PidLcd::Yes][PidLed::Yes][Message::No_______][TemplateType::Emergency][TemplateLcd::No_][TemplateLed::No_] = 0;
            t[PidLcd::Yes][PidLed::Yes][Message::No_______][TemplateType::Emergency][TemplateLcd::No_][TemplateLed::Yes] = LedPrevewButton;
            t[PidLcd::Yes][PidLed::Yes][Message::No_______][TemplateType::Emergency][TemplateLcd::Yes][TemplateLed::No_] = LcdPrevewButton;
            t[PidLcd::Yes][PidLed::Yes][Message::No_______][TemplateType::Emergency][TemplateLcd::Yes][TemplateLed::Yes] = LcdPrevewButton | LedPrevewButton;
            t[PidLcd::Yes][PidLed::Yes][Message::Normal___][TemplateType::Normal___][TemplateLcd::No_][TemplateLed::No_] = MessageButton | MessageDateTime;
            t[PidLcd::Yes][PidLed::Yes][Message::Normal___][TemplateType::Normal___][TemplateLcd::No_][TemplateLed::Yes] = MessageButton | MessageDateTime | LedPrevewButton;
            t[PidLcd::Yes][PidLed::Yes][Message::Normal___][TemplateType::Normal___][TemplateLcd::Yes][TemplateLed::No_] = MessageButton | MessageDateTime | LcdPrevewButton;
            t[PidLcd::Yes][PidLed::Yes][Message::Normal___][TemplateType::Normal___][TemplateLcd::Yes][TemplateLed::Yes] = MessageButton | MessageDateTime | TemplateButton | TemplateDateTime | LcdPrevewButton | LedPrevewButton | AllButton;
            t[PidLcd::Yes][PidLed::Yes][Message::Normal___][TemplateType::Emergency][TemplateLcd::No_][TemplateLed::No_] = 0;
            t[PidLcd::Yes][PidLed::Yes][Message::Normal___][TemplateType::Emergency][TemplateLcd::No_][TemplateLed::Yes] = LedPrevewButton;
            t[PidLcd::Yes][PidLed::Yes][Message::Normal___][TemplateType::Emergency][TemplateLcd::Yes][TemplateLed::No_] = LcdPrevewButton;
            t[PidLcd::Yes][PidLed::Yes][Message::Normal___][TemplateType::Emergency][TemplateLcd::Yes][TemplateLed::Yes] = LcdPrevewButton | LedPrevewButton;
            t[PidLcd::Yes][PidLed::Yes][Message::Emergency][TemplateType::Normal___][TemplateLcd::No_][TemplateLed::No_] = MessageButton;
            t[PidLcd::Yes][PidLed::Yes][Message::Emergency][TemplateType::Normal___][TemplateLcd::No_][TemplateLed::Yes] = LedPrevewButton;
            t[PidLcd::Yes][PidLed::Yes][Message::Emergency][TemplateType::Normal___][TemplateLcd::Yes][TemplateLed::No_] = LcdPrevewButton;
            t[PidLcd::Yes][PidLed::Yes][Message::Emergency][TemplateType::Normal___][TemplateLcd::Yes][TemplateLed::Yes] = LcdPrevewButton | LedPrevewButton;
            t[PidLcd::Yes][PidLed::Yes][Message::Emergency][TemplateType::Emergency][TemplateLcd::No_][TemplateLed::No_] = MessageButton;
            t[PidLcd::Yes][PidLed::Yes][Message::Emergency][TemplateType::Emergency][TemplateLcd::No_][TemplateLed::Yes] = MessageButton | LedPrevewButton | AllButton;
            t[PidLcd::Yes][PidLed::Yes][Message::Emergency][TemplateType::Emergency][TemplateLcd::Yes][TemplateLed::No_] = MessageButton | LcdPrevewButton | AllButton;
            t[PidLcd::Yes][PidLed::Yes][Message::Emergency][TemplateType::Emergency][TemplateLcd::Yes][TemplateLed::Yes] = MessageButton | LcdPrevewButton | LedPrevewButton | AllButton;

            return t;
        });

        auto msg = getSelectedMessage();
        bool message_selected = msg.size();
        bool is_emergency = m_priorityManager->isEmergency();
        auto message_type = (message_selected ? (is_emergency ? Message::Emergency : Message::Normal) : Message::No);

        bool template_priority = m_messageTypeTab.templatePage().isEmergencyChecked();
        bool lcd_selected = m_messageTypeTab.templatePage().isLcdSelected();
        bool led_selected = m_messageTypeTab.templatePage().isLedSelected();

        auto res = s_tensor[m_pidLcdSelected][m_pidLedSelected][message_type][template_priority][lcd_selected][led_selected];

        auto message_button_enabled = (res & MessageButton) && m_canDisplay && m_pidsInList;
        auto message_datetime_enabled = res & MessageDateTime;
        auto template_button_enabled = (res & TemplateButton) && m_canDisplay && m_pidsInList;
        auto template_datetime_enabled = res & TemplateDateTime;
        auto all_button_enabled = (res & AllButton) && m_canDisplay && m_pidsInList;
        auto lcd_preview_enabled = res & LcdPrevewButton;
        auto led_preview_enabled = res & LedPrevewButton;

        m_clearNormalMessageButton.EnableWindow(m_pidsInList && m_canClear);
        m_clearEmergencyMessageAndTemplateButton.EnableWindow(m_pidsInList && m_canClear);
        m_clearNormalTemplateButton.EnableWindow(m_pidsInList && m_canClear);
        m_clearAllButton.EnableWindow(m_pidsInList && m_canClear);

        m_displayMessageButton.EnableWindow(message_button_enabled);
        m_displayTemplateButton.EnableWindow(template_button_enabled);
        m_displayAllButton.EnableWindow(all_button_enabled);

        m_messageTimeControlManager->enable(message_datetime_enabled);
        m_templateTimeControlManager->enable(template_datetime_enabled);

        GetDlgItem(IDC_LCD_PREVIEW)->EnableWindow(lcd_preview_enabled);
        GetDlgItem(IDC_LED_PREVIEW)->EnableWindow(led_preview_enabled);

        m_selectedMessage->set_text_color(is_emergency ? EMERGENCY_MESSAGE_TEXT_COLOR : NORMAL_MESSAGE_TEXT_COLOR);
        m_lcdTemplate.set_text_color(isEmergencyLcdSelected() || (!isLcdSelected() && isEmergencyMessageSelected()) ? EMERGENCY_MESSAGE_TEXT_COLOR : NORMAL_MESSAGE_TEXT_COLOR);
        m_ledTemplate.set_text_color(isEmergencyLedSelected() || (!isLedSelected() && isEmergencyMessageSelected()) ? EMERGENCY_MESSAGE_TEXT_COLOR : NORMAL_MESSAGE_TEXT_COLOR);
    }

    std::string DisplayPage::getSelectedMessage()
    {
        return m_selectedMessage->get_window_text_utf8();
    }

    bool DisplayPage::validateAllStartEndDateTime()
    {
        return validateMessageStartEndDateTime() && validateTemplateStartEndDateTime();
    }

    bool DisplayPage::validateStartEndDateTime(TimeControlManagerPtr t)
    {
        if (auto [startTime, endTime, errmsg] = t->validate(); errmsg.size())
        {
            UserMessages::getInstance().displayError(errmsg);
            return false;
        }

        return true;
    }

    bool DisplayPage::validateMessageStartEndDateTime()
    {
        return validateStartEndDateTime(m_messageTimeControlManager);
    }

    bool DisplayPage::validateTemplateStartEndDateTime()
    {
        return validateStartEndDateTime(m_templateTimeControlManager);
    }

    void DisplayPage::onLcdPreview()
    {
        std::string id;

        if (auto t = m_messageTypeTab.templatePage().getLcdTemplate())
        {
            std::string id2 = DigitString<3>(t->templateID);

            if (m_messageTypeTab.templatePage().isEmergencyChecked())
            {
                id = "EMG1" + id2;
            }
            else
            {
                id = "TMP3" + id2;
            }
        }
        else if (m_messageTypeTab.isPredefinedEmergencyMessageSelected())
        {
            auto msg = m_messageTypeTab.predefinedPage().getMessage();
            id = "EMG1"s + DigitString<3>(msg.lcdEmgTemplate).str();
        }

        TemplatePreview::instance().preview(id);
    }

    void DisplayPage::onLedPreview()
    {
        std::string id;

        if (auto t = m_messageTypeTab.templatePage().getLedTemplate())
        {
            std::string id2 = DigitString<3>(t->templateID);

            if (m_messageTypeTab.templatePage().isEmergencyChecked())
            {
                id = "EMG2" + id2;
            }
            else
            {
                id = "TMP4" + id2;
            }
        }
        else if (m_messageTypeTab.isPredefinedEmergencyMessageSelected())
        {
            auto msg = m_messageTypeTab.predefinedPage().getMessage();
            id = "EMG2"s + DigitString<3>(msg.ledEmgTemplate).str();
        }

        TemplatePreview::instance().preview(id);
    }

    bool DisplayPage::isEmergencyLcdOrLedSelected()
    {
        return m_messageTypeTab.templatePage().isEmergencyLcdOrLedSelected();
    }

    bool DisplayPage::isEmergencyLcdSelected()
    {
        return m_messageTypeTab.templatePage().isEmergencyLcdSelected();
    }

    bool DisplayPage::isEmergencyLedSelected()
    {
        return m_messageTypeTab.templatePage().isEmergencyLedSelected();
    }

    std::string DisplayPage::getMessageStartTime()
    {
        return m_messageTimeControlManager->getStartDateTimeString();
    }

    std::string DisplayPage::getMessageEndTime()
    {
        return m_messageTimeControlManager->getEndDateTimeString();
    }

    std::string DisplayPage::getTemplateStartTime()
    {
        return m_templateTimeControlManager->getStartDateTimeString();
    }

    std::string DisplayPage::getTemplateEndTime()
    {
        return m_templateTimeControlManager->getEndDateTimeString();
    }

    DestinationList DisplayPage::getDestinations()
    {
        return m_pidSelectionManager->populateDisplayDestination();
    }

    bool DisplayPage::isLcdSelected()
    {
        return m_messageTypeTab.templatePage().isLcdSelected();
    }

    bool DisplayPage::isLedSelected()
    {
        return m_messageTypeTab.templatePage().isLedSelected();
    }

    bool DisplayPage::isEmergencyPredefinedMessageSelected()
    {
        return m_messageTypeTab.predefinedPage().isEmergencySelected() && m_priorityManager->isEmergency();
    }

    bool DisplayPage::isEmergencyAdHocMessageSelected()
    {
        return m_messageTypeTab.adHocPage().hasValidSelection() && m_priorityManager->isEmergency();
    }

    bool DisplayPage::isEmergencyMessageSelected()
    {
        return isEmergencyPredefinedMessageSelected() || isEmergencyAdHocMessageSelected();
    }

    void DisplayPage::onUpdateLcdLabel(CCmdUI* pCmdUI)
    {
        if (!isLcdSelected())
        {
            pCmdUI->SetText(get_default_lcd_led_template());
        }
    }

    void DisplayPage::onUpdateLedLabel(CCmdUI* pCmdUI)
    {
        if (!isLedSelected())
        {
            pCmdUI->SetText(get_default_lcd_led_template());
        }
    }

    void DisplayPage::onUpdatePriority(CCmdUI* pCmdUI)
    {
        if (m_messageTypeTab.isCurrentTemplatePage())
        {
            pCmdUI->Enable(FALSE);
        }
    }

    const char* DisplayPage::get_default_lcd_led_template()
    {
        return (m_validPredefinedMessageSelected || m_validAdhocMessageSelected) && m_priorityManager->isEmergency()
            ? "默認"
            : "預定"
            ;
    }

    void DisplayPage::OnBnClickedSplitLocation()
    {
        close_and_apply_pid_filters();
    }

    void DisplayPage::OnBnClickedSplitArea()
    {
        close_and_apply_pid_filters();
    }

    void DisplayPage::OnNMKillfocusPidLocationFilterTree(NMHDR* pNMHDR, LRESULT* pResult)
    {
        if (m_location_filter.IsWindowVisible())
        {
            close_and_apply_pid_location_filter();
            m_PIDTree.EnableWindow(TRUE);
            s_location_split_button_dropdown_delayer.update();
        }

        *pResult = 0;
    }

    void DisplayPage::OnNMKillfocusPidAreaFilterTree(NMHDR* pNMHDR, LRESULT* pResult)
    {
        if (m_area_filter.IsWindowVisible())
        {
            close_and_apply_pid_area_filter();
            m_PIDTree.EnableWindow(TRUE);
            s_area_split_button_dropdown_delayer.update();
        }

        *pResult = 0;
    }

    void DisplayPage::OnContextMenu(CWnd* pWnd, CPoint point)
    {
        if (pWnd == &m_PIDList)
        {
            CMenu context;
            context.LoadMenu(IDR_MENU2);
            auto menu = context.GetSubMenu(0);
            context.EnableMenuItem(ID_SELECTEDPID_REMOVE, m_PIDList.GetSelectedCount() ? MF_ENABLED : MF_GRAYED);
            menu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, this);
        }

        CDialog::OnContextMenu(pWnd, point);
    }

    void DisplayPage::OnLvnKeydownPidList(NMHDR* pNMHDR, LRESULT* pResult)
    {
        LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);

        if (pLVKeyDow->wVKey == VK_DELETE)
        {
            OnRemoveSelectedPID();
        }

        *pResult = 0;
    }

    void DisplayPage::OnRemoveSelectedPID()
    {
        if (m_PIDList.GetSelectedCount())
        {
            auto pids = m_PIDList.get_selected_pids();
            WindowsUtil::delete_selected_items(m_PIDList);
            m_PIDTree.remove_pids(pids);
        }
    }

    void DisplayPage::close_and_apply_pid_filters()
    {
        close_and_apply_pid_location_filter();
        close_and_apply_pid_area_filter();

        if (!m_PIDTree.IsWindowEnabled())
        {
            m_PIDTree.EnableWindow(TRUE);
        }
    }

    void DisplayPage::close_and_apply_pid_location_filter()
    {
        if (m_location_filter.IsWindowVisible())
        {
            m_location_filter.ShowWindow(SW_HIDE);

            if (m_PIDTree.apply_location_filter(m_location_filter.get_selection()))
            {
                const std::wstring locationTitle =
                    CodeConverter::UTF8ToUnicode(m_location_filter.get_title());
                ::SetWindowTextW(GetDlgItem(IDC_SPLIT_LOCATION)->GetSafeHwnd(),
                                 locationTitle.c_str());
                m_location_filter_tooltip.UpdateTipText(m_location_filter.get_tooltip().c_str(), GetDlgItem(IDC_SPLIT_LOCATION));

                // TODO: update areas
#if 0
                if (m_area_filter.update(STIS_UTILITY::PID::get_areas(m_location_filter.get_selection())))
                {
                    GetDlgItem(IDC_SPLIT_AREA)->SetWindowText(m_area_filter.get_title().c_str());
                    m_area_filter_tooltip.UpdateTipText(m_area_filter.get_tooltip().c_str(), GetDlgItem(IDC_SPLIT_AREA));
                }
#endif
            }
        }
    }

    void DisplayPage::close_and_apply_pid_area_filter()
    {
        if (m_area_filter.IsWindowVisible())
        {
            m_area_filter.ShowWindow(SW_HIDE);

            if (m_PIDTree.apply_area_filter(m_area_filter.get_selection()))
            {
                GetDlgItem(IDC_SPLIT_AREA)->SetWindowText(m_area_filter.get_title().c_str());
                m_area_filter_tooltip.UpdateTipText(m_area_filter.get_tooltip().c_str(), GetDlgItem(IDC_SPLIT_AREA));
            }
        }
    }

    void DisplayPage::OnBnDropDownSplitLocation(NMHDR* pNMHDR, LRESULT* pResult)
    {
        if (s_location_split_button_dropdown_delayer)
        {
            return;
        }

        auto pDropDown = reinterpret_cast<LPNMBCDROPDOWN>(pNMHDR);

        if (!m_location_filter.IsWindowVisible())
        {
            close_and_apply_pid_area_filter();
            m_location_filter.ShowWindow(SW_SHOW);
            m_PIDTree.EnableWindow(FALSE);
        }
        else
        {
            close_and_apply_pid_location_filter();
            m_PIDTree.EnableWindow(TRUE);
        }

        *pResult = 0;
    }

    void DisplayPage::OnBnDropDownSplitArea(NMHDR* pNMHDR, LRESULT* pResult)
    {
        if (s_area_split_button_dropdown_delayer)
        {
            return;
        }

        auto pDropDown = reinterpret_cast<LPNMBCDROPDOWN>(pNMHDR);

        if (!m_area_filter.IsWindowVisible())
        {
            close_and_apply_pid_location_filter();
            m_area_filter.ShowWindow(SW_SHOW);
            m_PIDTree.EnableWindow(FALSE);
        }
        else
        {
            close_and_apply_pid_area_filter();
            m_PIDTree.EnableWindow(TRUE);
        }

        *pResult = 0;
    }

    void DisplayPage::OnLButtonDown(UINT nFlags, CPoint point)
    {
        close_and_apply_pid_filters();
        __super::OnLButtonDown(nFlags, point);
    }

    void DisplayPage::onPIDControl()
    {
        auto destinations = getDestinations();

        if (destinations.empty())
        {
            UserMessages::getInstance().displayError("未選擇任何PID。請至少選擇一個PID。");
            return;
        }

        PIDControlDlg dlg(destinations, this);
        dlg.DoModal();
    }
}
