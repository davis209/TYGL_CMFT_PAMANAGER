#pragma once
#include "afxdialogex.h"
#include "resource.h"
#include "app/signs/common_library/src/stis_protocol/message_types/MessageTypes.h"

using TA_IRS_App::STIS_PROTOCOL::MESSAGE_TYPES::DestinationList;

namespace TA_IRS_App
{
    class PIDControlDlg : public CDialogEx
    {
    public:

        PIDControlDlg(const DestinationList& destinations, CWnd* pParent = nullptr);

        enum { IDD = IDD_PID_CONTROL };

    protected:

        virtual void DoDataExchange(CDataExchange* pDX);
        virtual BOOL OnInitDialog();

        afx_msg void OnBnClickedSend();
        afx_msg void OnBnClickedCancel();
        afx_msg void OnBnClickedCheckSchedule();

        DECLARE_MESSAGE_MAP()

    private:

        void enableScheduleControls(bool enable);

        DestinationList m_destinations;
        int m_controlAction;        // 0 = All Off, 1 = Monitor1 On, 2 = Monitor2 On, 3 = All On
        BOOL m_scheduleEnabled;
        CEdit m_editMonitorOffTime;
        CEdit m_editMonitorOnTime;
    };
}
