/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/stis_manager/src/MessageTypeTab.h $
 * @author:  Robin Ashcroft
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 *
 * The tab control that holds the
 * predefined, adhoc and template pages.
 *
 */

#pragma once
#include "PredefinedPage.h"
#include "FreeTextPage.h"
#include "TemplatePage.h"
#include "STISManager.h"
#include "bus/mfc_extensions/src/tab_dialog_control/TabDialogCtrl.h"

namespace TA_IRS_App
{
    class CMessageTypeTab : public CTabDialogCtrl
    {
    public:

        using TemplatePtr = TA_Base_Core::TemplatePtr;

        CMessageTypeTab();

        void initAll();

        /**
         * setMessageSelectionListener
         *
         * Sets the object to notify of selection changes
         *
         * @param messageSelectionListener
         */
        void setMessageSelectionListener(IMessageSelectionListener* messageSelectionListener);

        /**
         * getPredefinedMessage
         *
         * Gets the selected predefined message
         *
         * @pre The predefined page must be active
         *      There must be a valid message selected
         */
        TA_Base_Core::PredefinedMessage getPredefinedMessage() const;

        /**
         * getFreeTextMessage
         *
         * Gets all attributes from the free text page
         *
         * @pre The free text page must be active
         */
        FreeTextMessage getFreeTextMessage() const;

        /**
         * getLcdTemplate
         *
         * Gets lcd template from the template page
         *
         * @pre The template page must be active
         */
        TemplatePtr getLcdTemplate() const;

        /**
         * getLedTemplate
         *
         * Gets led template from the template page
         *
         * @pre The template page must be active
         */
        TemplatePtr getLedTemplate() const;

        std::pair<TemplatePtr, TemplatePtr> getLcdLedTemplate() const;

        bool findAndSelectStationMessage(const std::string& messageName);

        CPredefinedPage& predefinedPage();
        CFreeTextPage& adHocPage();
        CTemplatePage& templatePage();

        bool isCurrentPredefinedPage();
        bool isCurrentAdHocPage();
        bool isCurrentTemplatePage();
        bool isLastMessageSelectionPredefined();
        bool isLastMessageSelectionAdHoc();

        bool isPredefinedMessageSelected();
        bool isPredefinedEmergencyMessageSelected();

    protected:

        //TD 16275
        //zhou yuan++
        // override to draw text only; eg, colored text or different font
        virtual void PreSubclassWindow();
        virtual void OnDrawText(CDC& dc, CRect rc, CString sText, BOOL bDisabled);
        virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
        //zhou yuan++

        // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(CMessageTypeTab)

    public:

        //}}AFX_VIRTUAL

        //{{AFX_MSG(CMessageTypeTab)
        afx_msg BOOL OnSelchange(NMHDR* pNMHDR, LRESULT* pResult);
        //}}AFX_MSG

        DECLARE_MESSAGE_MAP()

    private:

        CPredefinedPage m_predefinedPage;
        CFreeTextPage m_freeTextPage;
        CTemplatePage m_templatePage;
        int m_previousSelect = 0;
        int m_lastMessageSelect = -1;

        std::map<int, ITabPage*> m_pages =
        {
            {0, &m_predefinedPage},
            {1, &m_freeTextPage},
            {2, &m_templatePage}
        };

        std::list<int> m_selectionHistory;
    };
}
