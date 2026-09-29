/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/stis_manager/src/TemplatePage.h $
 * @author:  Robin Ashcroft
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 *
 * The Template message selection tab
 */

#pragma once
#include "ITabPage.h"
#include "IMessageSelectionListener.h"
#include "core/data_access_interface/tis_agent_4669/src/ITemplateLibrary.h"
#include "bus/mfc_extensions/src/list_ctrl_selection_without_focus/ListCtrlSelNoFocus.h"
#include "bus/signs_4669/tis_agent_access/src/TISAgentAccessFactory.h"
#include "core/naming/src/NamedObject.h"
#include <vector>

using TA_Base_Bus::TISAgentAccessFactory;
using TA_Base_Bus::ListCtrlSelNoFocus;
using TA_Base_Core::Template;

namespace TA_IRS_App
{
    class STISTemplates;   // TD11310 ~ added fwd decl.

    class CTemplatePage : public CDialog, public ITabPage
    {
    public:

        CTemplatePage(CWnd* pParent = NULL);   // standard constructor
        ~CTemplatePage();

        void init();

        /**
         * getLcdTemplate
         *
         * Gets the selected lcd template
         *
         * @return
         */
        TA_Base_Core::TemplatePtr getLcdTemplate() const;
        TA_Base_Core::TemplatePtr getNormalLcdTemplate() const;
        TA_Base_Core::TemplatePtr getEmergencyLcdTemplate() const;

        /**
         * getLedTemplate
         *
         * Gets the selected led template
         *
         * @return
         */
        TA_Base_Core::TemplatePtr getLedTemplate() const;
        TA_Base_Core::TemplatePtr getNormalLedTemplate() const;
        TA_Base_Core::TemplatePtr getEmergencyLedTemplate() const;

        /**
         * windowShown
         *
         * This tab has just been switched to
         */
        void windowShown() override;

        /**
         * setMessageSelectionListener
         *
         * Sets the object to notify of selection changes
         *
         * @param messageSelectionListener
         */
        void setMessageSelectionListener(IMessageSelectionListener* messageSelectionListener);

        //TD 15349
        //zhou yuan++
        bool findAndSelectStationMessage(const std::string& messageName) override;
        //++zhou yuan

        bool isNormalChecked() const;
        bool isEmergencyChecked() const;
        bool isLcdSelected() const;
        bool isLedSelected() const;
        bool isLcdOrLedSelected() const;
        bool isNormalLcdSelected() const;
        bool isNormalLedSelected() const;
        bool isEmergencyLcdSelected() const;
        bool isEmergencyLedSelected() const;
        bool isNormalLcdOrLedSelected() const;
        bool isEmergencyLcdOrLedSelected() const;

    protected:

        // Dialog Data
        //{{AFX_DATA(CTemplatePage)
        enum { IDD = IDD_TEMPLATE };
        ListCtrlSelNoFocus  m_lcdList;
        ListCtrlSelNoFocus  m_ledList;
        CEdit   m_searchLcd;
        CEdit   m_searchLed;
        //}}AFX_DATA

        // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(CTemplatePage)

    protected:

        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        //}}AFX_VIRTUAL

        // Generated message map functions
        //{{AFX_MSG(CTemplatePage)
        virtual BOOL OnInitDialog();
        afx_msg void OnUpdateSearchLcd();
        afx_msg void OnUpdateSearchLed();
        afx_msg void OnClickLcdTemplate(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnClickLedTemplate(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void onItemchangedLcdTemplate(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void onItemchangedLedTemplate(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg LRESULT OnUpdateCurrentSTISVersion(WPARAM wParam, LPARAM lParam);
        afx_msg void OnDestroy();
        afx_msg void OnBnClickedRadioNormalTemplate();
        afx_msg void OnBnClickedRadioEmergencyTemplate();
        afx_msg void onBnClickedLCDDefaultSetting();
        afx_msg void onBnClickedLEDDefaultSetting();
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()

        /**
         * OnOK
         *
         * This method has been implemented to hide accidental calls when
         * the Enter key is pressed. It can be overriden with new behaviour if required.
         */
        virtual afx_msg void OnOK();

        /**
         * OnCancel
         *
         * This method has been implemented to hide accidental calls when
         * the ESC key is pressed. It can be overriden with new behaviour if required.
         */
        virtual afx_msg void OnCancel();

    private:

        //TD 15349
        //zhou yuan++
        bool findAndSelectMessageNameInList(CListCtrl& list, const std::string messageName);
        void clearCurrentSelection(CListCtrl& list);
        //++zhou yuan

        /**
         * hasValidSelection
         *
         * test if a message is selected.
         *
         * @return true if a valid pre-defined message is selected
         */
        bool hasValidLcdSelection() const;
        bool hasValidLedSelection() const;

        /**
         * populateLists
         *
         * Populates the pre-defined message lists.
         *
         */
        void populateLists();

        /**
         * getSelectedMessageData
         *
         * Retrieves the selected pre-defined message.
         *
         *
         * @return The selected pre-defined message (pointer)
         *         Null if nothing valid is selected.
         */
        const Template* getSelectedLcdTemplateData() const;

        /**
         * getSelectedMessageData
         *
         * Retrieves the selected pre-defined message.
         *
         *
         * @return The selected pre-defined message (pointer)
         *         Null if nothing valid is selected.
         */
        const Template* getSelectedLedTemplateData() const;

        void onUpdateSearch();

        // tell this object when message selection changes
        IMessageSelectionListener* m_messageSelectionListener;

        // true if a valid template exists
        bool m_validLcdTemplate = false;
        bool m_validLedTemplate = false;

        STISTemplates* m_stisTemplates;

        int m_selectedRadioButton = 0;      //0: normal, 1: emergency

        CString m_lcdSearchText;
        CString m_ledSearchText;
        int m_default_lcd_index = -1;
        int m_default_led_index = -1;
    };
}
