#include "stdafx.h"
#include "afxdialogex.h"
#include "PIDControlDlg.h"
#include "UserMessages.h"
#include "app/signs/common_library/src/stis_protocol/STISClient.h"
#include "app/signs/common_library/src/STISAuditMessage.h"
#include "core/utility/src/base_ex/DebugUtilEx.h"
#include "UserMessages.h"

using namespace TA_IRS_App::STIS_PROTOCOL::MESSAGE_TYPES;
using TA_IRS_App::STIS_PROTOCOL::STISClient;

namespace TA_IRS_App
{
    PIDControlDlg::PIDControlDlg(const DestinationList& destinations, CWnd* pParent)
        : CDialogEx(IDD_PID_CONTROL, pParent)
        , m_destinations(destinations)
        , m_controlAction(0)
        , m_scheduleEnabled(FALSE)
    {
    }

    void PIDControlDlg::DoDataExchange(CDataExchange* pDX)
    {
        CDialogEx::DoDataExchange(pDX);
        DDX_Radio(pDX, IDC_RADIO_CONTROL_ALL_OFF, m_controlAction);
        DDX_Check(pDX, IDC_CHECK_SCHEDULE, m_scheduleEnabled);
        DDX_Control(pDX, IDC_EDIT_MONITOR_OFF_TIME, m_editMonitorOffTime);
        DDX_Control(pDX, IDC_EDIT_MONITOR_ON_TIME, m_editMonitorOnTime);
    }

    BEGIN_MESSAGE_MAP(PIDControlDlg, CDialogEx)
        ON_BN_CLICKED(IDC_PID_CONTROL_SEND, OnBnClickedSend)
        ON_BN_CLICKED(IDC_PID_CONTROL_CANCEL, OnBnClickedCancel)
        ON_BN_CLICKED(IDC_CHECK_SCHEDULE, OnBnClickedCheckSchedule)
    END_MESSAGE_MAP()

    BOOL PIDControlDlg::OnInitDialog()
    {
        CDialogEx::OnInitDialog();

        CheckRadioButton(IDC_RADIO_CONTROL_ALL_OFF, IDC_RADIO_CONTROL_ALL_ON, IDC_RADIO_CONTROL_ALL_ON);
        m_controlAction = 1; // Default to Control On
        m_editMonitorOffTime.SetLimitText(4);
        m_editMonitorOnTime.SetLimitText(4);
        enableScheduleControls(FALSE);

        return TRUE;
    }

    void PIDControlDlg::OnBnClickedCheckSchedule()
    {
        UpdateData(TRUE);
        enableScheduleControls(m_scheduleEnabled);
    }

    void PIDControlDlg::enableScheduleControls(bool enable)
    {
        m_editMonitorOffTime.EnableWindow(enable);
        m_editMonitorOnTime.EnableWindow(enable);
        GetDlgItem(IDC_STATIC_MONITOR_OFF)->EnableWindow(enable);
        GetDlgItem(IDC_STATIC_MONITOR_ON)->EnableWindow(enable);

        // When schedule is enabled, disable the on/off radio buttons
        GetDlgItem(IDC_RADIO_CONTROL_ALL_OFF)->EnableWindow(!enable);
        GetDlgItem(IDC_RADIO_CONTROL_MON1_ON)->EnableWindow(!enable);
        GetDlgItem(IDC_RADIO_CONTROL_MON2_ON)->EnableWindow(!enable);
        GetDlgItem(IDC_RADIO_CONTROL_ALL_ON)->EnableWindow(!enable);
    }

    void PIDControlDlg::OnBnClickedSend()
    {
        UpdateData(TRUE);

        if (m_destinations.empty())
        {
            UserMessages::getInstance().displayError(UserMessages::ERROR_NO_PIDS_SELECTED);
            return;
        }

        try
        {
            if (m_scheduleEnabled)
            {
                CString offTime, onTime;
                m_editMonitorOffTime.GetWindowText(offTime);
                m_editMonitorOnTime.GetWindowText(onTime);

                std::string sOffTime((LPCSTR)offTime);
                std::string sOnTime((LPCSTR)onTime);

                if (sOffTime.length() != 4 || sOnTime.length() != 4)
                {
                    UserMessages::getInstance().displayError(UserMessages::ERROR_INVALID_TIME_FORMAT);
                    return;
                }

                int offHH = std::stoi(sOffTime.substr(0, 2));
                int offMM = std::stoi(sOffTime.substr(2, 2));
                int onHH = std::stoi(sOnTime.substr(0, 2));
                int onMM = std::stoi(sOnTime.substr(2, 2));

                if (offHH < 0 || offHH > 23 || offMM < 0 || offMM > 59 ||
                    onHH < 0 || onHH > 23 || onMM < 0 || onMM > 59)
                {
                    UserMessages::getInstance().displayError(UserMessages::ERROR_INVALID_TIME_VALUE);
                    return;
                }

                STISClient::instance().submit_M25_SchedulePIDOnOffTimeSettingRequestList(m_destinations, sOffTime, sOnTime);
                UserMessages::getInstance().displayInfo(UserMessages::INFO_SCHEDULE_PID_SUCCESS);
            }
            else
            {
                // m_controlAction==0 (All Off radio) => turn off; any other => turn on
                bool turnOn = (m_controlAction != 0);
                EPIDControlOn controlOn = turnOn ? EPIDControlOn::ControlOn : EPIDControlOn::NoAction;
                EPIDControlOff controlOff = turnOn ? EPIDControlOff::NoAction : EPIDControlOff::ControlOff;

                STISClient::instance().submit_M21_PIDOnOffControlRequestList(m_destinations, controlOn, controlOff);
                UserMessages::getInstance().displayInfo(UserMessages::INFO_PID_CONTROL_SUCCESS);
            }

            CDialogEx::OnOK();
        }
        catch (...)
        {
            LOG_EXCEPTION("Unknown", "PID Control request failed");
            UserMessages::getInstance().displayError(UserMessages::ERROR_PID_CONTROL_FAILED);
        }
    }

    void PIDControlDlg::OnBnClickedCancel()
    {
        CDialogEx::OnCancel();
    }
}
