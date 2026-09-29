/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/J155/transactive/app/signs/stis_manager/src/DisplayPage.h $
 * @author:  Adam Radics
 * @version: $Revision: #4 $
 *
 * Last modification: $DateTime: 2026/05/19 17:14:59 $
 * Last modified by:  $Author: anusuya $
 *
 * The Station display page
 */

#pragma once
#include "Resource.h"
#include "MessageTypeTab.h"
#include "PidSelectionManager.h"
#include "TimeControlManager.h"
#include "PriorityManager.h"
#include "IMessageSelectionListener.h"
#include "SimpleUnicodeStatic.h"
#include "PidTreeCtrl.h"
#include "SimpleTreeFilterCtrl.h"
#include "SimpleColorStatic.h"
#include "app/signs/common_library/src/stis_protocol/message_types/MessageTypes.h"
#include "bus/signs_4669/tis_agent_access/src/TISAgentAccessFactory.h"
#include "bus/mfc_extensions/src/tree_ctrl_multi_sel/MltiTree.h"
#include <vector>
#include <list>
#include <map>
#include <string>
#include <tuple>
#include <variant>

#define PREDEFINED_ITEM 0
#define FREE_TEXT_ITEM 1
#define TEMPLATE_ITEM 2

#define NORMAL_PRIORITY 0
#define EMERGENCY_PRIORITY 1
#define ALL_PRIORITY 2

using TA_Base_Bus::CMultiTree;
using TA_Base_Bus::CTreeItemList;
using TA_Base_Bus::TISAgentAccessFactory;
using TA_IRS_App::STIS_PROTOCOL::MESSAGE_TYPES::EDisplayTemplateType;
class REBProgressManager;

namespace TA_IRS_App
{
    class STISPredefinedMessages;   // TD11310 ~ added

    using PredefinedAdHocMessage = std::variant<STIS_PROTOCOL::PredefinedMessage, STIS_PROTOCOL::AdHodMessage>;

    class DisplayPage : public CDialog, public IPidSelectionListener, public IMessageSelectionListener
    {
    public:

        using PredefinedDisplayTemplate = STIS_PROTOCOL::PredefinedDisplayTemplate;
        using AdHodMessage = STIS_PROTOCOL::AdHodMessage;

        DisplayPage(CWnd* pParent = NULL);  // standard constructor
        ~DisplayPage();

        void initAll(/*REBProgressManager & mgr*/);

        /**
         * pidSelectionChanged
         *
         * Gets called when the PID selection is changed.
         *
         * @param pidsInList   true if at least one PID is in the list
         * @param pidSelectionExists   true if one PID in the list is selected
         */
        void pidSelectionChanged(bool pidsInList, bool pidSelectionExists) override;

        /**
         * predefinedMessageSelected
         *
         * Called when the predefined message tab is selected.
         * Sets whether a message is selected and if so the details needed to
         * populate certain fields on the display page.
         *
         * This is also called when a message is selected/deselected.
         *
         * @param tabSwitched          only true when the tab has just been switched to
         * @param validMessageSelected
         * @param priority
         * @param repeatInterval
         *
         */
        void predefinedMessageSelected(bool tabSwitched,
                                       bool validMessageSelected,
                                       const char* title = "",
                                       unsigned short priority = 0) override;

        /**
         * adHocMessageSelected
         *
         * Called when the ad hoc message tab is selected.
         * Sets whether a message has been entered.
         *
         * This is also called when a message is types/cleared.
         *
         * @param tabSwitched          only true when the tab has just been switched to
         * @param validMessageEntered
         * @param repeatInterval       only used to set the default when the tab is switched
         */
        void adHocMessageSelected(bool tabSwitched, bool hasSelection, bool validMessageEntered, std::string title) override;

        /**
         * templateSelected
         *
         * Called when the template tab is selected.
         * Sets whether a template is selected and if so the details needed to
         * populate certain fields on the display page.
         *
         * This is also called when a template is selected/deselected.
         *
         * @param tabSwitched          only true when the tab has just been switched to
         * @param validTemplateSelected
         */
        void templateSelected(bool tabSwitched, bool validLcdTemplateSelected, bool validLedTemplateSelected, TemplatePtr lcdTmp, TemplatePtr ledTmp) override;

        void predefinedMessageSelectChanged(const std::string description);
        void templateSelectChanged(const Template& tmp);

        bool findAndSelectStationMessage(const std::string& messageName);

        std::string getSelectedMessage();

    protected:

        // Dialog Data
        //{{AFX_DATA(DisplayPage)
        enum { IDD = IDD_DISPLAY_PAGE };
        CButton m_displayMessageButton;
        CButton m_displayTemplateButton;
        CButton m_displayAllButton;
        CButton m_clearNormalMessageButton;
        CButton m_clearEmergencyMessageAndTemplateButton;
        CButton m_clearNormalTemplateButton;
        CButton m_clearAllButton;
        CMessageTypeTab m_messageTypeTab;
        ColourCombo m_priority;
        //CallbackButton    m_radioTimed;
        //CallbackButton    m_radioContinuous;
        CallbackDateTimeCtrl m_templateStartDate;
        CallbackDateTimeCtrl m_templateStartTime;
        CallbackDateTimeCtrl m_templateEndDate;
        CallbackDateTimeCtrl m_templateEndTime;
        CallbackDateTimeCtrl m_messageStartDate;
        CallbackDateTimeCtrl m_messageStartTime;
        CallbackDateTimeCtrl m_messageEndDate;
        CallbackDateTimeCtrl m_messageEndTime;
        //CDateTimeCtrl m_repeatInterval;
        CStatic m_startTimeLabel;
        //CStatic   m_repeatLabel;
        CStatic m_endTimeLabel;
        PidListCtrl m_PIDList;
        PidTreeCtrl m_PIDTree;
        SimpleTreeFilterCtrl m_location_filter;
        SimpleTreeFilterCtrl m_area_filter;
        PidGroupCombo   m_pidGroupCombo;
        CallbackButton  m_deleteGroupButton;
        CallbackButton  m_saveGroupButton;

        SimpleColorStatic m_lcdTemplate;
        SimpleColorStatic m_ledTemplate;
        SimpleUnicodeStaticPtr m_selectedMessage;
        int m_selectedMessageType = -1;
        //}}AFX_DATA

        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(DisplayPage)

    protected:

        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        //}}AFX_VIRTUAL

        // Implementation

    protected:

        HICON m_hIcon;

        // Generated message map functions
        //{{AFX_MSG(DisplayPage)
        virtual BOOL OnInitDialog();
        afx_msg void onDisplayMessage();
        afx_msg void onDisplayTemplate();
        afx_msg void onDisplayAll();
        afx_msg void onClearAll();
        afx_msg void onClearNormalMessage();
        afx_msg void onClearEmergencyMessageAndTemplate();
        afx_msg void onClearNormalTemplate();
        afx_msg void onLcdPreview();
        afx_msg void onLedPreview();
        afx_msg void OnDestroy();
        afx_msg LRESULT onRightsChanged(WPARAM wParam, LPARAM lParam);
        afx_msg void OnKickIdle();
        afx_msg void onUpdateLcdLabel(CCmdUI* pCmdUI);
        afx_msg void onUpdateLedLabel(CCmdUI* pCmdUI);
        afx_msg void onUpdatePriority(CCmdUI* pCmdUI);
        afx_msg void OnBnClickedSplitLocation();
        afx_msg void OnBnClickedSplitArea();
        afx_msg void OnNMKillfocusPidLocationFilterTree(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnNMKillfocusPidAreaFilterTree(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
        afx_msg void OnRemoveSelectedPID();
        afx_msg void OnBnDropDownSplitLocation(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnBnDropDownSplitArea(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
        afx_msg void OnLvnKeydownPidList(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void onPIDControl();
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()

        /**
         * OnOK
         *
         * This method has been implemented to hide accidental calls when
         * the Enter key is pressed. It can be overriden with new behaviour if required.
         */
        virtual afx_msg void OnOK() override;

        /**
         * OnCancel
         *
         * This method has been implemented to hide accidental calls when
         * the ESC key is pressed. It can be overriden with new behaviour if required.
         */
        virtual afx_msg void OnCancel() override;

        virtual BOOL PreTranslateMessage(MSG* pMsg) override;

    private:

        /**
         * submitClearMessageRequest
         *
         * Submits a clear request
         *
         * @param clearType    The name eg All, Normal etc - for UI messages
         * @param lowerPriority    the lower clear priority
         * @param upperPriority    the upper clear priority
         *
         */
        bool submitClearMessageRequest(std::string clearType, int lowerPriority, int upperPriority, bool ask_question = true, bool display_info = true);
        std::vector<int> make_priority_list(int lowerPriority, int upperPriority);
        bool submitClearTemplateRequest(std::list<EDisplayTemplateType> typeList, std::string clearType, bool ask_question = true, bool display_info = true);
        void OnSelchangePriorityList();
        void updateControls();
        bool validateAllStartEndDateTime();
        bool validateMessageStartEndDateTime();
        bool validateTemplateStartEndDateTime();
        bool validateStartEndDateTime(TimeControlManagerPtr t);
        std::tuple<PredefinedDisplayTemplate, PredefinedDisplayTemplate, std::string> constructSelectedDisplayTemplate();
        std::tuple<PredefinedAdHocMessage, std::string, std::string> constructSelectedMessage();
        bool isLcdSelected();
        bool isLedSelected();
        bool isEmergencyLcdOrLedSelected();
        bool isEmergencyLcdSelected();
        bool isEmergencyLedSelected();
        std::string getMessageStartTime();
        std::string getMessageEndTime();
        std::string getTemplateStartTime();
        std::string getTemplateEndTime();
        DestinationList getDestinations();
        bool isEmergencyPredefinedMessageSelected();
        bool isEmergencyAdHocMessageSelected();
        bool isEmergencyMessageSelected();
        const char* get_default_lcd_led_template();

        void close_and_apply_pid_location_filter();
        void close_and_apply_pid_area_filter();
        void close_and_apply_pid_filters();

    private:

        bool m_pidsInList = false;
        bool m_pidSelected = false;
        bool m_pidLcdSelected = false;
        bool m_pidLedSelected = false;
        bool m_validPredefinedMessageSelected = false;
        bool m_validAdhocMessageSelected = false;
        bool m_canDisplay = false;
        bool m_canClear = false;
        bool m_canModifyPidGroups = false;

        // all PID and PID group logic is encapsulated here
        PidSelectionManager* m_pidSelectionManager = nullptr;

        // all time control logic is encapsulated here
        TimeControlManagerPtr m_templateTimeControlManager;
        TimeControlManagerPtr m_messageTimeControlManager;
        PriorityManager* m_priorityManager = nullptr;
        STISPredefinedMessages* m_stisPredefinedMessages = nullptr;    // TD11310 ~ added

        struct AdHocSelectionInfo
        {
            int priority = 4;
        };

        AdHocSelectionInfo m_adhocInfo;
        CToolTipCtrl m_location_filter_tooltip;
        CToolTipCtrl m_area_filter_tooltip;
    };
}
