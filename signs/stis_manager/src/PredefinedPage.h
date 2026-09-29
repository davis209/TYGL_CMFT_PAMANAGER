/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/stis_manager/src/PredefinedPage.h $
 * @author:  Robin Ashcroft
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 *
 * The predefined message selection tab
 */

#pragma once
#include "ITabPage.h"
#include "IMessageSelectionListener.h"
#include "SimpleUnicodeEdit.h"
#include "core/data_access_interface/tis_agent_4669/src/IPredefinedMessageLibrary.h"
#include "bus/mfc_extensions/src/list_ctrl_selection_without_focus/ListCtrlSelNoFocus.h"
#include "bus/signs_4669/tis_agent_access/src/TISAgentAccessFactory.h"
#include "core/naming/src/NamedObject.h"
#include <vector>

using TA_Base_Bus::TISAgentAccessFactory;
using TA_Base_Bus::ListCtrlSelNoFocus;

namespace TA_IRS_App
{
    class STISPredefinedMessages;   // TD11310 ~ added fwd decl.

    class CPredefinedPage : public CDialog, public ITabPage
    {
    public:

        CPredefinedPage(CWnd* pParent = NULL);   // standard constructor
        ~CPredefinedPage();

        void init();

        /**
         * getMessage
         *
         * Gets the selected message
         *
         * @return
         */
        TA_Base_Core::PredefinedMessage getMessage() const;

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

        /**
         * hasValidSelection
         *
         * test if a message is selected.
         *
         * @return true if a valid pre-defined message is selected
         */
        bool hasValidSelection() const;

        bool isEmergencySelected() const;

    protected:

        // Dialog Data
        //{{AFX_DATA(CPredefinedPage)
        enum { IDD = IDD_PREDEFINED };
        ListCtrlSelNoFocus  m_normalList;
        ListCtrlSelNoFocus  m_emergencyList;
        //CEdit m_languages;
        CEdit   m_searchNormal;
        CEdit   m_searchEmergency;
        //CEdit m_predefinedContent;
        //}}AFX_DATA

        // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(CPredefinedPage)

    protected:

        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        //}}AFX_VIRTUAL

        // Generated message map functions
        //{{AFX_MSG(CPredefinedPage)
        virtual BOOL OnInitDialog();
        afx_msg void OnUpdateSearchEmergency();
        afx_msg void OnUpdateSearchNormal();
        afx_msg void OnClickEmergencyPredefined(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnClickNormalPredefined(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void onItemchangedNormalPredefined(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void onItemchangedEmergencyPredefined(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg LRESULT OnUpdateCurrentSTISVersion(WPARAM wParam, LPARAM lParam);
        afx_msg void OnDestroy();
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
        const TA_Base_Core::PredefinedMessage* getSelectedMessageData() const;

        /**
         * updatePreviewArea
         *
         * updates the message preview area with the text from the selected message.
         * also updates the languages box.
         *
         */
        void updatePreviewArea();

        // tell this object when message selection changes
        IMessageSelectionListener* m_messageSelectionListener;

        // true if a valid message exists
        bool m_validMessage;

        STISPredefinedMessages* m_stisPredefinedMessages;
        std::string m_currentSelection;

        SimpleUnicodeEditPtr m_predefinedContent;
    };

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}
