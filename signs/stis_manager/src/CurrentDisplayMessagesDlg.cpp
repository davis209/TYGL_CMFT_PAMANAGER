#include "stdafx.h"
#include <afxpriv.h>
#include "afxdialogex.h"
#include "UserMessages.h"
#include "CurrentDisplayMessagesDlg.h"
#include "app/signs/common_library/src/stis_protocol/STISClient.h"
#include "app/signs/common_library/src/STISUtility.h"
#include "core/utility/src/core/fixed_length_data/all.h"
#include "core/utility/src/core/StdEx.h"
#include "core/utility/src/base_ex/RunParamsEx.h"
#include "core/utility/src/base_ex/LocationAccessFactoryEx.h"

#define RPARAM_REFRESH_CURRENT_DISPLAY_MESSAGES_INTERVAL_MS "RefreshCurrentDisplayMessagesIntervalMs"

using namespace std::literals;
using st::fixed_length_data::DigitString;
using TA_Base_Ex::RunParamsEx;
using TA_Base_Ex::LocationEx;
using TA_IRS_App::STIS_PROTOCOL::STISClient;
using TA_IRS_App::STIS_PROTOCOL::Destination;
using TA_IRS_App::STIS_PROTOCOL::CurrentDisplayMessage;
using TA_IRS_App::STIS_PROTOCOL::CurrentDisplayMessageList;
using namespace TA_IRS_App;

// CDM: Current Display Messages

namespace
{
    st::map<std::string, CurrentDisplayMessagesDlgPtr> s_dialogs;

    struct CurrentDisplayMessagesDlgThread : CWinThread
    {
        DECLARE_DYNCREATE(CurrentDisplayMessagesDlgThread);

        virtual BOOL InitInstance() override
        {
            auto dlg = std::make_shared<CurrentDisplayMessagesDlg>(m_report);
            dlg->Create(CurrentDisplayMessagesDlg::IDD, AfxGetApp()->m_pMainWnd);
            m_pMainWnd = dlg.get();
            m_pMainWnd->ShowWindow(SW_SHOW);
            m_pMainWnd->UpdateWindow();
            s_dialogs.emplace(m_report->make_unique_key(), dlg);
            return TRUE;
        }

        ~CurrentDisplayMessagesDlgThread()
        {
            s_dialogs.erase(m_report->make_unique_key());
        }

        A50_CurrentDisplayMessageTemplateReportPtr m_report;
    };

    IMPLEMENT_DYNCREATE(CurrentDisplayMessagesDlgThread, CWinThread)
}

namespace
{
    std::string get_datetime(std::string str)
    {
        boost::trim(str);

        if (str.empty() || boost::algorithm::all_of_equal(str, '0'))
        {
            return "N/A"s;
        }

        return st::get_time_str_from_YYYYMMDDHHMMSS(str);
    }
}

void CurrentDisplayMessagesDlg::show_dialog(A50_CurrentDisplayMessageTemplateReportPtr r)
{
    if (auto dlg = s_dialogs.get_value_optional(r->make_unique_key()))
    {
        (*dlg)->BringWindowToTop();
        return;
    }

    auto t = dynamic_cast<CurrentDisplayMessagesDlgThread*>(AfxBeginThread(RUNTIME_CLASS(CurrentDisplayMessagesDlgThread), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED));
    t->m_report = r;
    t->ResumeThread();
}

CurrentDisplayMessagesDlg::CurrentDisplayMessagesDlg(A50_CurrentDisplayMessageTemplateReportPtr report)
    : CDialogEx(CurrentDisplayMessagesDlg::IDD, AfxGetApp()->m_pMainWnd),
    m_report(report),
    m_message_text(std::make_shared<SimpleUnicodeEdit>(this, IDC_CDM_MESSAGE_TEXT))
{
    m_refresh_interval_ms = RunParamsEx::get_or(RPARAM_REFRESH_CURRENT_DISPLAY_MESSAGES_INTERVAL_MS, m_refresh_interval_ms);
}

void CurrentDisplayMessagesDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CDM_MESSAGES, m_tree);
    DDX_Text(pDX, IDC_CDM_STATION, m_station);
    DDX_Text(pDX, IDC_CDM_PID, m_pid);
    DDX_Text(pDX, IDC_CDM_TEMPLATE_TYPE, m_template_type);
    DDX_Text(pDX, IDC_CDM_TEMPLATE_ID, m_template_id);
    DDX_Text(pDX, IDC_CDM_TEMPLATE_START_TIME, m_template_start_time);
    DDX_Text(pDX, IDC_CDM_TEMPLATE_END_TIME, m_template_end_time);
    DDX_Text(pDX, IDC_CDM_MESSAGE_STATUS, m_message_status);
    DDX_Text(pDX, IDC_CDM_MESSAGE_STATUS, m_message_status);
    // DDX_Text(pDX, IDC_CDM_MESSAGE_TEXT, m_message_text);
}

BEGIN_MESSAGE_MAP(CurrentDisplayMessagesDlg, CDialogEx)
    ON_WM_TIMER()
    ON_WM_SIZE()
    ON_BN_CLICKED(IDC_CDM_CLEAR, &CurrentDisplayMessagesDlg::OnBnClickedCdmClear)
    ON_UPDATE_COMMAND_UI(IDC_CDM_CLEAR, &CurrentDisplayMessagesDlg::OnUpdateButtonClear)
    ON_NOTIFY(TVN_SELCHANGED, IDC_CDM_MESSAGES, &CurrentDisplayMessagesDlg::OnTvnSelchangedCdmMessages)
END_MESSAGE_MAP()

BOOL CurrentDisplayMessagesDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    m_tree.SetMultiSelect(TRUE);

    enable_clear_button(false);

    m_message_text->use_default_font();
    m_message_text->init();
    m_message_text->set_readonly(true);

    CString title;
    GetWindowText(title);
    SetWindowText(str(boost::format("%s - %s:%s") % (const char*)title % LocationEx::to_display_name(m_report->report_station) % STIS_UTILITY::PID::from_station_and_id(m_report->report_station, m_report->report_pid).name).c_str());

    update_messages();
    SetTimer(1, m_refresh_interval_ms, nullptr);

    return TRUE;
}

void CurrentDisplayMessagesDlg::update_messages()
{
    m_station = LocationEx::to_display_name(m_report->report_station).c_str();
    m_pid = STIS_UTILITY::PID::from_station_and_id(m_report->report_station, m_report->report_pid).name.c_str();
    m_template_type = m_connected ? std::to_string((int)m_report->current_display_template.display_template_type).c_str() : "";
    m_template_id = m_connected ? m_report->current_display_template.display_template_id.c_str() : "";
    m_template_start_time = m_connected ? get_datetime(m_report->current_display_template.start_time).c_str() : "";
    m_template_end_time = m_connected ? get_datetime(m_report->current_display_template.end_time).c_str() : "";
    m_message_status = m_connected ? m_report->message_status.c_str() : "";

    m_tree.DeleteAllItems();

    for (auto& message : m_report->current_display_messages)
    {
        auto root = m_tree.InsertItem(boost::trim_copy(message.message_tag).c_str(), TVI_ROOT);
        m_tree.SetItemData(root, (DWORD)&message);

        m_tree.InsertItem(str(boost::format("Priority: %d") % message.message_priority).c_str(), root);
        m_tree.InsertItem(str(boost::format("Start Time: %s") % get_datetime(message.message_start_time)).c_str(), root);
        m_tree.InsertItem(str(boost::format("End Time:   %s") % get_datetime(message.message_end_time)).c_str(), root);
        m_tree.InsertItem(str(boost::format("Message Length: %d") % (message.message_text.size() - 8)).c_str(), root);

        auto languages = STIS_UTILITY::split_to_4_languages_utf8(STIS_UTILITY::transform_4_languages_from_utf16_to_utf8(message.message_text));
        auto& english = languages[0];
        auto text = english.substr(0, 100);
        text += (100 < english.size() ? " ..." : "");
        m_tree.InsertItem(str(boost::format("Message Text: %s") % text).c_str(), root);
    }

    m_tree.SelectItem(nullptr);

    enable_clear_button(false);

    UpdateData(FALSE);
}

void CurrentDisplayMessagesDlg::OnTimer(UINT nIDEvent)
{
    resubmit_m50();
    CDialogEx::OnTimer(nIDEvent);
}

void CurrentDisplayMessagesDlg::OnBnClickedCdmClear()
{
    auto tags = get_selected_tags();
    auto question = str(boost::format(UserMessages::QUESTION_CLEAR_CURRENT_MESSAGES_BY_MESSAGE_TAG) % boost::algorithm::join(tags, "\n"));

    if (UserMessages::getInstance().askQuestionUTF8(question.c_str()) != IDYES)
    {
        return;
    }

    try
    {
        for (auto& tag : tags)
        {
            Destination dest;
            dest.station_id = m_report->report_station;
            dest.pid_list.emplace_back(m_report->report_pid);
            STISClient::instance().submit_M24_ClearCurrentMessagesRequestByMessageTag(dest, tag);
        }

        resubmit_m50();
    }
    catch (...)
    {
    }
}

void CurrentDisplayMessagesDlg::resubmit_m50()
{
    auto station = m_report->report_station;
    auto pid = m_report->report_pid;

    A50_CurrentDisplayMessageTemplateReport report;

    try
    {
        report = STISClient::instance().submit_M50_CurrentDisplayMessageTemplateRequest(station, pid);
        m_connected = true;
    }
    catch (...)
    {
        m_connected = false;
        report.report_station = m_report->report_station;
        report.report_pid = m_report->report_pid;
    }

    if (report != *m_report)
    {
        *m_report = std::move(report);
        update_messages();
    }
}

void CurrentDisplayMessagesDlg::OnUpdateButtonClear(CCmdUI* pCmdUI)
{
    CTreeItemList items;
    m_tree.GetSelectedList(items);
    enable_clear_button(!items.IsEmpty());

    if (m_selected)
    {
        if (auto items = get_selection(); items.size() != 1)
        {
            m_selected = nullptr;
            m_message_text->set_window_text(L"");
        }
    }
}

void CurrentDisplayMessagesDlg::enable_clear_button(bool enable)
{
    GetDlgItem(IDC_CDM_CLEAR)->EnableWindow(enable);
}

HTREEITEM CurrentDisplayMessagesDlg::get_parent(HTREEITEM item)
{
    for (auto t = m_tree.GetParentItem(item); t; t = m_tree.GetParentItem(t))
    {
        item = t;
    }

    return item;
}

std::vector<HTREEITEM> CurrentDisplayMessagesDlg::get_parents(const CTreeItemList& items)
{
    std::vector<HTREEITEM> res;
    auto pos = items.GetHeadPosition();

    while (pos)
    {
        auto item = items.GetNext(pos);
        st::push_back_if_none_of_equal(res, get_parent(item));
    }

    return res;
}

std::vector<HTREEITEM> CurrentDisplayMessagesDlg::get_selection()
{
    CTreeItemList items;
    m_tree.GetSelectedList(items);
    return get_parents(items);
}

std::set<std::string> CurrentDisplayMessagesDlg::get_selected_tags()
{
    return st::transform_to_set(get_selection(), [&](auto item)
    {
        return std::string((const char*)m_tree.GetItemText(item));
    });
}

void CurrentDisplayMessagesDlg::OnCancel()
{
    DestroyWindow();
}

void CurrentDisplayMessagesDlg::on_close()
{
    s_dialogs.for_each_value([](auto& dlg)
    {
        dlg->PostMessage(WM_CLOSE, 0, 0);
    });

    s_dialogs.wait([&] { return s_dialogs.empty(); });
}

void CurrentDisplayMessagesDlg::on_idle_update_cmd_ui()
{
    s_dialogs.for_each_value([](auto& dlg)
    {
        dlg->UpdateDialogControls(dlg.get(), FALSE);
    });
}

void CurrentDisplayMessagesDlg::OnTvnSelchangedCdmMessages(NMHDR* pNMHDR, LRESULT* pResult)
{
    auto pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

    if (auto selected = get_selection(); selected.size() == 1 && m_selected != selected[0])
    {
        m_selected = selected[0];
        auto* message = reinterpret_cast<CurrentDisplayMessage*>(m_tree.GetItemData(m_selected));
        auto languages = STIS_UTILITY::transform_4_languages_from_utf16_to_utf8(message->message_text, "\r\n\r\n");
        m_message_text->set_window_text_utf8(languages);
    }

    *pResult = 0;
}

void CurrentDisplayMessagesDlg::OnSize(UINT nType, int cx, int cy)
{
    CDialogEx::OnSize(nType, cx, cy);
    m_message_text->on_size(nType, cx, cy);
}
