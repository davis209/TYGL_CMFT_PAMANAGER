/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source: $File: //depot/4669_T01271350/4669/transactive/app/signs/stis_manager/src/PidListCtrl.cpp $
 * @author Adam Radics
 * @version $Revision: #1 $
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by: $Author: builder $
 *
 * PidListCtrl defines a list control that displays PIDs.
 */

#include "stdafx.h"
#include "resource.h"
#include "PidListCtrl.h"
#include "PidSelectionManager.h"
#include "WindowsUtil.h"
#include "CurrentDisplayMessagesDlg.h"
#include "UserMessages.h"
#include "app/signs/common_library/src/stis_protocol/STISClient.h"
#include "core/utility/src/base_ex/LocationAccessFactoryEx.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utility/src/core/algorithm/strings.h"
#include "core/utility/src/core/StdEx.h"
#include <algorithm>

using namespace TA_Base_Core;
using TA_Base_Ex::Location;
using TA_IRS_App::STIS_PROTOCOL::STISClient;
using PID = TA_IRS_App::STIS_UTILITY::PID;

namespace TA_IRS_App
{
    using STIS_UTILITY::PID;
    using STIS_UTILITY::PIDList;

    BEGIN_MESSAGE_MAP(PidListCtrl, TA_Base_Bus::ListCtrlSelNoFocus)
        //{{AFX_MSG_MAP(PidListCtrl)
        ON_NOTIFY_REFLECT_EX(LVN_ITEMCHANGED, onItemchanged)
        ON_NOTIFY_REFLECT_EX(NM_DBLCLK, OnNMDblclkPidList)
        //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    void PidListCtrl::PreSubclassWindow()
    {
        CListCtrl::PreSubclassWindow();

		m_listfont.CreatePointFont(110, "MS Shell Dlg");
		this->SetFont(&m_listfont, TRUE);
		this->GetHeaderCtrl()->SetFont(&m_listfont, TRUE);
	
        SetExtendedStyle(GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);

        m_fixedHeaderCtrl.subclassHeader(GetHeaderCtrl());

        InsertColumn(LOCATION, "¦aÂI"); //TD17909 AVteam
        SetColumnWidth(LOCATION, 80);  // 67
        m_sortInfo.defineSortingSemantic(LOCATION, AutoSortListCtrl::BY_CALLBACK);

        InsertColumn(PID_NAME, "PID");
        SetColumnWidth(PID_NAME, 150);  // 195
        m_sortInfo.defineSortingSemantic(PID_NAME, AutoSortListCtrl::BY_STRING);

        // sort callback for item data
        m_sortInfo.setCallback(this);
        // Sort columns based on label
        m_sortInfo.setCurrentSort(LOCATION, AutoSortListCtrl::ASCENDING);
        // Make label column the active sorting column
        m_sortInfo.activateSort(LOCATION);

        setScrollBarVisibility(true, false);

        m_pidSelected = false;

        setNonSelectableGray(false);
    }

    void PidListCtrl::setPidSelectionManager(PidSelectionManager* pidSelectionManager)
    {
        m_pidSelectionManager = pidSelectionManager;
    }

    bool PidListCtrl::isSelectable(int rowNumber)
    {
        return true;
    }

    BOOL PidListCtrl::onItemchanged(NMHDR* pNMHDR, LRESULT* pResult)
    {
        NM_LISTVIEW* pNMListView = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);

        // call the parent class handler first
        // if it returns true - this has been blocked.
        if (ListCtrlSelNoFocus::OnItemchanged(pNMHDR, pResult) == TRUE)
        {
            return TRUE;
        }

        *pResult = 0;

        // Only interested in state changes (ie selection)
        if (pNMListView->uChanged != LVIF_STATE)
        {
            return FALSE;
        }

        // if there is something managing events
        if (m_pidSelectionManager != NULL)
        {
            // if the status has changed
            if (m_pidSelected != (GetFirstSelectedItemPosition() != NULL))
            {
                m_pidSelected = (GetFirstSelectedItemPosition() != NULL);

                // update the selection manager
                m_pidSelectionManager->pidSelectionChanged();
            }
        }

        // this allows the owning window to handle this as well
        return FALSE;
    }

    // for sorting
    LRESULT PidListCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
    {
        AutoSortListCtrl::WindowProc(message, wParam, lParam, *this, m_sortInfo);
        return ListCtrlSelNoFocus::WindowProc(message, wParam, lParam);
    }

    int PidListCtrl::sortCallback(LPARAM lParam1, LPARAM lParam2, int columnIndex, AutoSortListCtrl::ECurrentSort currentSort)
    {
        auto& pid1 = STIS_UTILITY::PID::from_entity_key(lParam1);
        auto& pid2 = STIS_UTILITY::PID::from_entity_key(lParam2);

        bool lt = false;
        bool gt = false;

        if (columnIndex == 0)
        {
            lt = std::tie(pid1.extra.location_order, pid1.extra.id) < std::tie(pid2.extra.location_order, pid2.extra.id);
            gt = std::tie(pid1.extra.location_order, pid1.extra.id) > std::tie(pid2.extra.location_order, pid2.extra.id);
        }
        else
        {
            lt = std::tie(pid1.extra.id, pid1.extra.location_order) < std::tie(pid2.extra.id, pid2.extra.location_order);
            gt = std::tie(pid1.extra.id, pid1.extra.location_order) > std::tie(pid2.extra.id, pid2.extra.location_order);
        }

        if (currentSort == AutoSortListCtrl::ASCENDING)
        {
            return lt ? -1 : (gt ? 1 : 0);
        }
        else
        {
            return lt ? 1 : (gt ? -1 : 0);
        }

#if 0
        // simple number sort

        int first;
        int second;
        int result = 0; //TD17909 AVteam

        if (currentSort == AutoSortListCtrl::DESCENDING)
        {
            first = lParam2;
            second = lParam1;
        }
        else
        {
            // ascending or unknown - which will default to ascending
            first = lParam1;
            second = lParam2;
        }

        //TD17909 AVteam++
        result = first - second;

        if (result == 0)
        {
            m_sortInfo.setCurrentSort(PID_NAME, AutoSortListCtrl::ASCENDING);

            m_sortInfo.activateSort(PID_NAME);
        }

        return result;
        //TD17909 ++AVteam
#endif
    }

    PIDList PidListCtrl::get_selected_pids()
    {
        return PID::from_entity_keys(WindowsUtil::get_all_selection_data(*this));
    }

    BOOL PidListCtrl::OnNMDblclkPidList(NMHDR* pNMHDR, LRESULT* pResult)
    {
        if (auto item = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR); item->iItem != -1)
        {
            if (auto& pid = PID::from_entity_key(this->GetItemData(item->iItem)))
            {
                try
                {
                    auto report = STISClient::instance().submit_M50_CurrentDisplayMessageTemplateRequest(pid.station, pid.id);
                    CurrentDisplayMessagesDlg::show_dialog(std::make_shared<decltype(report)>(std::move(report)));
                }
                catch (std::exception& e)
                {
                    UserMessages::getInstance().displayError(st2::format("Failed to get current display message and template for %s[%s]: %s", Location::to_display_name(pid.station), pid.name, e.what()));
                }
            }
        }

        *pResult = 0;
        return FALSE;
    }
}
