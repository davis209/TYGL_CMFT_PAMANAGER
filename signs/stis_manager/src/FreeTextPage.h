/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/stis_manager/src/FreeTextPage.h $
 * @author:  Robin Ashcroft
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2011/03/29 11:49:08 $
 * Last modified by:  $Author: builder $
 *
 * The free text message selection page
 *
 */

#pragma once
#include "ITabPage.h"
#include "IMessageSelectionListener.h"
#include "SimpleUnicodeEdit.h"
#include "bus/mfc_extensions/src/coloured_controls/ColourCombo.h"
#include "bus/mfc_extensions/src/list_ctrl_selection_without_focus/ListCtrlSelNoFocus.h"
#include "bus/user_settings/src/SettingsMgr.h"
#include <vector>
#include <utility>

using TA_Base_Bus::ColourCombo;

namespace TA_IRS_App
{
    struct FreeTextMessage
    {
        std::string messageTitle;
        std::string messageContent;
    };

    class CFreeTextPage : public CDialog, public ITabPage
    {
        using ListCtrlSelNoFocus = TA_Base_Bus::ListCtrlSelNoFocus;

    public:

        CFreeTextPage(CWnd* pParent = NULL);   // standard constructor

        ~CFreeTextPage();

        void init();

        /**
         * getMessage
         *
         * Gets all attributes from the page
         *
         * @return
         */
        FreeTextMessage getMessage() const;

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

        /**
         * timeTypeChanged
         *
         * Gets called when the time type is changed.
         *
         * @param newTimeType  The new time type.
         */
        //virtual void timeTypeChanged(TimeControlManager::TimeType newTimeType);

        //TD 15349
        //zhou yuan++
        bool findAndSelectStationMessage(const std::string& messageName) override;
        //++zhou yuan

        bool hasValidSelection();

    protected:

        // Dialog Data
        //{{AFX_DATA(CFreeTextPage)
        enum { IDD = IDD_FREE_TEXT };
        ListCtrlSelNoFocus m_freeTextList;
        SimpleUnicodeEditPtr m_title;
        SimpleUnicodeEditPtr m_content;

        //}}AFX_DATA

        // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(CFreeTextPage)

    protected:

        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        //}}AFX_VIRTUAL

        // Generated message map functions
        //{{AFX_MSG(CFreeTextPage)
        virtual BOOL OnInitDialog();
        afx_msg void OnEdit();
        afx_msg void onChangeFreeText();
        //afx_msg LRESULT OnUpdateCurrentSTISVersion(WPARAM wParam, LPARAM lParam);
        afx_msg void OnDestroy();
        afx_msg void OnSave();
        afx_msg void OnSelchangeFreetextList(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnTimer(UINT nIDEvent);
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
        //++zhou yuan

        /**
         * updateDisplayTimeState
         *
         * Checks if the display time control should be enabled or disabled, and
         * does so as appropriate.
         */
        void updateDisplayTimeState();
        void onChangeFreeTextImpl(bool tabChanged);

        std::string get_title() const;
        std::string get_content() const;
        std::pair<std::string, std::string> get_title_content() const;
        void enable_title(bool b);
        void enable_content(bool b);
        void enable_title_content(bool b);

        std::vector<std::pair<std::string, std::string>>& build_messages_from_sqlite();

        // tell this object when message selection changes
        IMessageSelectionListener* m_messageSelectionListener = nullptr;

        // true if a valid message exists
        bool m_validMessage = false;

        std::vector<std::pair<std::string, std::string>> m_pidMessages;

        struct EditingMessage;
        std::shared_ptr<EditingMessage> m_edit;
    };
}
