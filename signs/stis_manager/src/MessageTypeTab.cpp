/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/C830G_TIP/830G/transactive/app/signs/stis_manager/src/MessageTypeTab.cpp $
 * @author:  Robin Ashcroft
 * @version: $Revision: #5 $
 *
 * Last modification: $DateTime: 2021/04/19 21:14:55 $
 * Last modified by:  $Author: limin.zhu $
 *
 * The tab control that holds the
 * predefined, adhoc and template pages.
 *
 */

#include "stdafx.h"
#include "STISManagerDlg.h"
#include "stismanager.h"
#include "MessageTypeTab.h"
#include "core/utility/src/base_ex/LocationAccessFactoryEx.h"
#include "core/types/src/ta_types.h"
#include <sstream>

#ifdef _DEBUG
    #define new DEBUG_NEW
    #undef THIS_FILE
    static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Ex::ThisLocation;
using TA_Base_Core::TemplatePtr;

namespace TA_IRS_App
{
    CMessageTypeTab::CMessageTypeTab()
        : CTabDialogCtrl()
    {
        AddPage(m_predefinedPage, IDD_PREDEFINED, CString("¹w¿ý"));
        AddPage(m_freeTextPage, IDD_FREE_TEXT, CString("¤â°Ê"));
        AddPage(m_templatePage, IDD_TEMPLATE, CString("¼ÒªO"));
        m_selectionHistory.push_front(0);
    }

    BEGIN_MESSAGE_MAP(CMessageTypeTab, CTabDialogCtrl)
        //{{AFX_MSG_MAP(CMessageTypeTab)
        ON_NOTIFY_REFLECT_EX(TCN_SELCHANGE, OnSelchange)
        //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    void CMessageTypeTab::initAll()
    {
        m_predefinedPage.init();
        m_freeTextPage.init();
        m_templatePage.init();
    }

    void CMessageTypeTab::setMessageSelectionListener(IMessageSelectionListener* messageSelectionListener)
    {
        // pass it to child pages
        m_predefinedPage.setMessageSelectionListener(messageSelectionListener);
        m_freeTextPage.setMessageSelectionListener(messageSelectionListener);
        m_templatePage.setMessageSelectionListener(messageSelectionListener);

        // call windowShown on the active one to update the status on the main dialog

        if (messageSelectionListener)
        {
            if (auto page = dynamic_cast<ITabPage*>(getActiveDialog()))
            {
                page->windowShown();
            }
        }
    }

    BOOL CMessageTypeTab::OnSelchange(NMHDR* pNMHDR, LRESULT* pResult)
    {
        // change tab
        BOOL res = CTabDialogCtrl::OnSelchange(pNMHDR, pResult);
        m_selectionHistory.push_front(GetCurSel());

        while (m_pages.size() < m_selectionHistory.size())
        {
            m_selectionHistory.pop_back();
        }

        if (auto page = dynamic_cast<ITabPage*>(getActiveDialog()))
        {
            page->windowShown();
            m_previousSelect = GetCurSel();
        }
        else
        {
            //in station, the ratis part should be disable
            this->SetCurSel(static_cast<int>(m_previousSelect));
        }

        return res;
    }

    TA_Base_Core::PredefinedMessage CMessageTypeTab::getPredefinedMessage() const
    {
        return m_predefinedPage.getMessage();
    }

    FreeTextMessage CMessageTypeTab::getFreeTextMessage() const
    {
        return m_freeTextPage.getMessage();
    }

    TemplatePtr CMessageTypeTab::getLcdTemplate() const
    {
        return m_templatePage.getLcdTemplate();
    }

    TemplatePtr CMessageTypeTab::getLedTemplate() const
    {
        return m_templatePage.getLedTemplate();
    }

    std::pair<TemplatePtr, TemplatePtr> CMessageTypeTab::getLcdLedTemplate() const
    {
        return {getLcdTemplate(), getLedTemplate()};
    }

    bool CMessageTypeTab::findAndSelectStationMessage(const std::string& messageName)
    {
        for (auto && [i, page] : m_pages)
        {
            if (page->findAndSelectStationMessage(messageName))
            {
                this->SetCurSel(i);
                page->windowShown();
                return true;
            }
        }

        return false;
    }

    //////////////////
    // Draw tab text. You can override to use different color/font.
    //
    void CMessageTypeTab::OnDrawText(CDC& dc, CRect rc, CString sText, BOOL bDisabled)
    {
        if (bDisabled)
        {
            // Create a shadow effect by first drawing the text in light highlight colour
            // at a slight south-east offset.
            CRect shadowRc(rc);
            shadowRc += CPoint(1, 1);
            dc.SetTextColor(::GetSysColor(COLOR_3DHILIGHT));
            dc.DrawText(sText, &shadowRc, DT_CENTER | DT_VCENTER);
        }

        // Draw the text in normal or disabled text colour
        dc.SetTextColor(::GetSysColor(bDisabled ? COLOR_GRAYTEXT : COLOR_BTNTEXT));
        dc.DrawText(sText, &rc, DT_CENTER | DT_VCENTER);
    }

    //////////////////
    // Draw the tab: mimic SysTabControl32, except use gray if tab is disabled
    //
    void CMessageTypeTab::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
    {
        DRAWITEMSTRUCT& ds = *lpDrawItemStruct;

        int iItem = ds.itemID;

        // Get tab item info
        char text[128];
        TCITEM tci;
        tci.mask = TCIF_TEXT;
        tci.pszText = text;
        tci.cchTextMax = sizeof(text);
        GetItem(iItem, &tci);

        // use draw item DC
        CDC dc;
        dc.Attach(ds.hDC);

        // draw text background
        CRect bkRc = ds.rcItem;
        bkRc.top += ::GetSystemMetrics(SM_CYEDGE);
        dc.SetBkMode(TRANSPARENT);
        dc.FillSolidRect(bkRc, ::GetSysColor(COLOR_BTNFACE));

        // calculate text rectangle
        CRect rc = ds.rcItem;
        rc += CPoint(0, 3);

        // draw the text
#if 0

        if (iItem == 2 && ThisLocation::is_not_occ())
        {
            OnDrawText(dc, rc, text, true);
        }
        else
#endif
        {
            OnDrawText(dc, rc, text, false);
        }

        dc.Detach();
    }

    void CMessageTypeTab::PreSubclassWindow()
    {
        // Perform normal stuff
        CTabDialogCtrl::PreSubclassWindow();

        ModifyStyle(0, TCS_OWNERDRAWFIXED);
    }

    CPredefinedPage& CMessageTypeTab::predefinedPage()
    {
        return m_predefinedPage;
    }

    CFreeTextPage& CMessageTypeTab::adHocPage()
    {
        return m_freeTextPage;
    }

    CTemplatePage& CMessageTypeTab::templatePage()
    {
        return m_templatePage;
    }

    bool CMessageTypeTab::isCurrentPredefinedPage()
    {
        return GetCurSel() == 0;
    }

    bool CMessageTypeTab::isCurrentAdHocPage()
    {
        return GetCurSel() == 1;
    }

    bool CMessageTypeTab::isCurrentTemplatePage()
    {
        return GetCurSel() == 2;
    }

    bool CMessageTypeTab::isLastMessageSelectionPredefined()
    {
        auto t = m_selectionHistory;
        t.remove(2);
        return t.size() && t.front() == 0;
    }

    bool CMessageTypeTab::isLastMessageSelectionAdHoc()
    {
        auto t = m_selectionHistory;
        t.remove(2);
        return t.size() && t.front() == 1;
    }

    bool CMessageTypeTab::isPredefinedMessageSelected()
    {
        return (isCurrentPredefinedPage() || isLastMessageSelectionPredefined()) && m_predefinedPage.hasValidSelection();
    }

    bool CMessageTypeTab::isPredefinedEmergencyMessageSelected()
    {
        return isPredefinedMessageSelected() && getPredefinedMessage().priority < 4;
    }
}
