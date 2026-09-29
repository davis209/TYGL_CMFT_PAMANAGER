#pragma once
#include "afxdialogex.h"
#include "resource.h"
#include "SimpleUnicodeEdit.h"
#include "app/signs/common_library/src/stis_protocol/message_types/MessageTypes.h"
#include "bus/mfc_extensions/src/tree_ctrl_multi_sel/MltiTree.h"
#include "core/utility/src/core/Map.h"

using TA_Base_Bus::CMultiTree;
using TA_Base_Bus::CTreeItemList;
using TA_IRS_App::STIS_PROTOCOL::A50_CurrentDisplayMessageTemplateReport;
using TA_IRS_App::STIS_PROTOCOL::A50_CurrentDisplayMessageTemplateReportPtr;
using TA_IRS_App::SimpleUnicodeEdit;
using TA_IRS_App::SimpleUnicodeEditPtr;

class CurrentDisplayMessagesDlg;
using CurrentDisplayMessagesDlgPtr = std::shared_ptr<CurrentDisplayMessagesDlg>;

class CurrentDisplayMessagesDlg : public CDialogEx
{
public:

    CurrentDisplayMessagesDlg(A50_CurrentDisplayMessageTemplateReportPtr report);

    static void show_dialog(A50_CurrentDisplayMessageTemplateReportPtr report);
    static void on_close();
    static void on_idle_update_cmd_ui();

    enum { IDD = IDD_CURRENT_DISPLAY_MESSAGES };

protected:

    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
    virtual void OnCancel();

    afx_msg void OnTimer(UINT nIDEvent);
    afx_msg void OnBnClickedCdmClear();
    afx_msg void OnUpdateButtonClear(CCmdUI* pCmdUI);
    afx_msg void OnTvnSelchangedCdmMessages(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnSize(UINT nType, int cx, int cy);

    DECLARE_MESSAGE_MAP()

public:

    void update_messages();
    void resubmit_m50();
    void enable_clear_button(bool enable);
    HTREEITEM get_parent(HTREEITEM item);
    std::vector<HTREEITEM> get_parents(const CTreeItemList& items);
    std::vector<HTREEITEM> get_selection();
    std::set<std::string> get_selected_tags();

    CString m_station;
    CString m_pid;
    CString m_message_status;
    CString m_template_type;
    CString m_template_id;
    CString m_template_start_time;
    CString m_template_end_time;
    SimpleUnicodeEditPtr m_message_text;
    CMultiTree m_tree;
    size_t m_refresh_interval_ms = 5000;
    A50_CurrentDisplayMessageTemplateReportPtr m_report;
    HTREEITEM m_selected = nullptr;
    bool m_connected = true;
};
