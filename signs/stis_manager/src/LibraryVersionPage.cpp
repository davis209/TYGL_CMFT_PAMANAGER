/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File$
 * @author:  Adam Radics
 * @version: $Revision$
 *
 * Last modification: $DateTime$
 * Last modified by:  $Author$
 *
 * This page shows the library versions at each location.
 */

#include "stdafx.h"
#include "core/types/src/ta_types.h"
#include "LibraryVersionPage.h"
#include "RightsManager.h"
#include "STISPredefinedMessages.h"
#include "STISTemplates.h"
#include "LibraryVersionMonitor.h"
#include "UserMessages.h"
#include "REBProgressManager.h"
#include "MainTab.h"
#include "TisAgentInterface.h"
#include "MainTab.h"
#include "app/signs/common_library/src/STISAuditMessage.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/CodeConverter.h"
#include "core/utility/src/core/fixed_length_data/all.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/corba/src/CorbaUtil.h"
#include "bus/signs_4669/tis_agent_access/src/TISAgentAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/naming/src/NamingMacros.h"
#include "bus/mfc_extensions/src/list_ctrl_selection_without_focus/ListCtrlSelNoFocus.h"
//#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include <iomanip>
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std::literals;
using namespace TA_Base_Core;
using TA_Base_Ex::ThisLocation;
using TA_Base_Bus::ListCtrlSelNoFocus;
using st::fixed_length_data::DigitString;

namespace
{
    auto get_version = [](int version)
    {
        if (0 < version)
        {
            return DigitString<3>(version).str();
        }
        else
        {
            return "N/A"s;
        }
    };
}

namespace TA_IRS_App
{
    LibraryVersionPage::LibraryVersionPage(CWnd* pParent /*=NULL*/)
        : CDialog(LibraryVersionPage::IDD, pParent),
        m_canUpgradeISCS(false),
        m_currentMessageLibraryVersion(0),
        m_nextMessageLibraryVersion(0),
        m_messageLibrariesAreSynchronised(false)
    {
        FUNCTION_ENTRY("LibraryVersionPage");

        // {{AFX_DATA_INIT(LibraryVersionPage)
        // NOTE: the ClassWizard will add member initialization here
        // }}AFX_DATA_INIT

        FUNCTION_EXIT;
    }

    LibraryVersionPage::~LibraryVersionPage()
    {
        TA_Base_Core::ThreadGuard guard2(MainTab::s_downLoadPageLock);

        m_stisPredefinedMessages->deregisterCurrentVersionUser(this);
        m_stisPredefinedMessages->deregisterNextVersionUser(this);
        m_stisPredefinedMessages->deregisterLibrarySynchronisedUser(this);

        m_stisTemplates->deregisterCurrentVersionUser(this);
        m_stisTemplates->deregisterNextVersionUser(this);
        m_stisTemplates->deregisterLibrarySynchronisedUser(this);

        LibraryVersionMonitor::instance().deregisterForChanges(this);
        LibraryVersionMonitor::instance().terminateAndWait();

        RightsManager::getInstance().deregisterForRightsChanges(this);

        // TD11310 ~ added
        //TA_Base_Bus::ScadaProxyFactory::removeInstance();        //must remove all instance before remove proxy callback class, or it will crash when exit sometimes
        //TA_Base_Bus::ScadaProxyFactory::removeInstance();
        //LibraryVersionMonitor::instance().removeInstance();
        //m_stisPredefinedMessages->removeInstance();
    }

    void LibraryVersionPage::DoDataExchange(CDataExchange* pDX)
    {
        FUNCTION_ENTRY("DoDataExchange");

        CDialog::DoDataExchange(pDX);
        // {{AFX_DATA_MAP(LibraryVersionPage)
        DDX_Control(pDX, IDC_VERSION_LIST_SUBHEADER1, m_subHeader1);
        DDX_Control(pDX, IDC_VERSION_LIST_MAIN_HEADER1, m_mainHeader1);
        DDX_Control(pDX, IDC_UPGRADE_ISCS_MESSAGE, m_upgradeISCS);
        DDX_Control(pDX, IDC_STATION_MESSAGE_VERSION_LIST, m_stationVersionList1);
        DDX_Control(pDX, IDC_ISCS__MESSAGE_NEXT_VERSION, m_nextISCSMessageVersion);
        DDX_Control(pDX, IDC_ISCS__MESSAGE_CURRENT_VERSION, m_currentISCSMessageVersion);
        DDX_Control(pDX, IDC_VERSION_LIST_SUBHEADER2, m_subHeader2);
        DDX_Control(pDX, IDC_VERSION_LIST_MAIN_HEADER2, m_mainHeader2);
        DDX_Control(pDX, IDC_UPGRADE_ISCS_TEMPLATE, m_upgradeISCS2);
        DDX_Control(pDX, IDC_STATION_TEMPLATE_VERSION_LIST, m_stationVersionList2);
        DDX_Control(pDX, IDC_ISCS_TEMPLATE_NEXT_VERSION, m_nextISCSTemplateVersion);
        DDX_Control(pDX, IDC_ISCS_TEMPLATE_CURRENT_VERSION, m_currentISCSTemplateVersion);
        // }}AFX_DATA_MAP

        FUNCTION_EXIT;
    }

    BEGIN_MESSAGE_MAP(LibraryVersionPage, CDialog)
        // {{AFX_MSG_MAP(LibraryVersionPage)
        ON_WM_DESTROY()
        ON_BN_CLICKED(IDC_UPGRADE_ISCS_MESSAGE, OnUpgradeIscsMessage)
        ON_MESSAGE(WM_UPDATE_MESSAGE_LIBRARY_VERSION, OnMessageLibraryVersionChanged)
        ON_MESSAGE(WM_UPDATE_CURRENT_STIS_VERSION, OnCurrentMessageLibraryVersionChanged)
        ON_MESSAGE(WM_UPDATE_NEXT_STIS_VERSION, OnNextMessageLibraryVersionChanged)
        ON_MESSAGE(WM_UPDATE_LIBRARY_SYNCHRONISED, OnOCCMessageLibrariesSynchronisedChanged)
        ON_BN_CLICKED(IDC_UPGRADE_ISCS_TEMPLATE, OnUpgradeIscsTemplate)
        ON_MESSAGE(WM_UPDATE_TEMPLATE_LIBRARY_VERSION, OnTemplateLibraryVersionChanged)
        ON_MESSAGE(WM_UPDATE_CURRENT_STIS_TEMPLATE_VERSION, OnCurrentTemplateLibraryVersionChanged)
        ON_MESSAGE(WM_UPDATE_NEXT_STIS_TEMPLATE_VERSION, OnNextTemplateLibraryVersionChanged)
        ON_MESSAGE(WM_UPDATE_TEMPLATE_LIBRARY_SYNCHRONISED, OnOCCTemplateLibrariesSynchronisedChanged)
        // }}AFX_MSG_MAP
    END_MESSAGE_MAP()

    BOOL LibraryVersionPage::OnInitDialog()
    {
        FUNCTION_ENTRY("OnInitDialog");

        CDialog::OnInitDialog();

        FUNCTION_EXIT;
        return TRUE;  // return TRUE unless you set the focus to a control
        // EXCEPTION: OCX Property Pages should return FALSE
    }

    void LibraryVersionPage::init()
    {
        FUNCTION_ENTRY("init");

        // TD1131 ~ added
        LibraryVersionMonitor::instance().start();
        m_stisPredefinedMessages = STISPredefinedMessages::getInstance();
        m_stisTemplates = STISTemplates::getInstance();
        m_canUpgradeISCS = RightsManager::getInstance().canUpgradeISCS();
        RightsManager::getInstance().registerForRightsChanges(this);

        m_currentMessageLibraryVersion = m_stisPredefinedMessages->getCurrentMessageLibraryVersion();
        m_nextMessageLibraryVersion = m_stisPredefinedMessages->getNextMessageLibraryVersion();
        m_messageLibrariesAreSynchronised = m_stisPredefinedMessages->getMessageLibrarySynchronised();
        m_stisPredefinedMessages->registerCurrentVersionUser(this);
        m_stisPredefinedMessages->registerNextVersionUser(this);
        m_stisPredefinedMessages->registerLibrarySynchronisedUser(this);

        m_currentTemplateLibraryVersion = m_stisTemplates->getCurrentTemplateLibraryVersion();
        m_nextTemplateLibraryVersion = m_stisTemplates->getNextTemplateLibraryVersion();
        m_templateLibrariesAreSynchronised = m_stisTemplates->getTemplateLibrarySynchronised();
        m_stisTemplates->registerCurrentVersionUser(this);
        m_stisTemplates->registerNextVersionUser(this);
        m_stisTemplates->registerLibrarySynchronisedUser(this);

        // Register to receive library version datapoint updates
        LibraryVersionMonitor::instance().registerForChanges(this);

		m_listfont.CreatePointFont(110, "MS Shell Dlg");
        // Setup the version lists - create the column headings etc
        setupLists1();
        setupLists2();

        // populate the initial data into the lists
        populateMessagePageData();
        populateTemplatePageData();

        // Set the initial state of the upgrade button
        enableUpgradeIscsMessageButton();
        enableUpgradeIscsTemplateButton();

        FUNCTION_EXIT;
    }

    void LibraryVersionPage::OnDestroy()
    {
        FUNCTION_ENTRY("OnDestroy");

        CDialog::OnDestroy();

        FUNCTION_EXIT;
    }

    LRESULT LibraryVersionPage::onRightsChanged(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY("onRightsChanged");

        // the rights have changed - re check them
        m_canUpgradeISCS = RightsManager::getInstance().canUpgradeISCS();

        // re-enable button
        enableUpgradeIscsMessageButton();
        enableUpgradeIscsTemplateButton();

        FUNCTION_EXIT;
        return 0;
    }

    void LibraryVersionPage::setupLists1()
    {
        FUNCTION_ENTRY("setupLists1");
			
		m_mainHeader1.SetFont(&m_listfont,TRUE);
		m_mainHeader1.GetHeaderCtrl()->SetFont(&m_listfont, TRUE);	
		m_subHeader1.SetFont(&m_listfont,TRUE);
		m_subHeader1.GetHeaderCtrl()->SetFont(&m_listfont, TRUE);
		
        // set the column headers
        RECT rect;
        m_mainHeader1.GetClientRect(&rect);

        int stationHeaderSize = 80;
        int currentIscsHeaderSize = 150;
        int currentStisHeaderSize = 150;
        int nextIscsHeaderSize = 150;
        int nextStisHeaderSize = 150;
        int spareHeaderSize = rect.right - (stationHeaderSize +
                                            currentIscsHeaderSize +
                                            currentStisHeaderSize +
                                            nextIscsHeaderSize +
                                            nextStisHeaderSize);

        // Fancy pants headers
        m_mainHeader1.setScrollBarVisibility(false, false);
        m_mainHeader1.InsertColumn(0, "", LVCFMT_CENTER);
        m_mainHeader1.InsertColumn(1, "", LVCFMT_CENTER);
        m_mainHeader1.InsertColumn(2, "", LVCFMT_CENTER);
        m_mainHeader1.InsertColumn(3, "", LVCFMT_CENTER);
        m_mainHeader1.SetColumnWidth(0, stationHeaderSize);
        m_mainHeader1.SetColumnWidth(1, currentIscsHeaderSize + currentStisHeaderSize);
        m_mainHeader1.SetColumnWidth(2, nextIscsHeaderSize + nextStisHeaderSize);
        m_mainHeader1.SetColumnWidth(3, spareHeaderSize);
        m_mainHeader1.InsertItem(0, "");
        m_mainHeader1.SetItemText(0, 1, "ヘ玡癟セ");
        m_mainHeader1.SetItemText(0, 2, "癟セ");
        m_mainHeader1.setItemColour(0, RGB(255, 255, 255),
                                    ListCtrlSelNoFocus::I_INDEX);
        m_mainHeader1.setItemColour(0, RGB(0, 0, 0),
                                    ListCtrlSelNoFocus::I_INDEX,
                                    ListCtrlSelNoFocus::CT_BACKGROUND);

        m_subHeader1.setScrollBarVisibility(false, false);
        m_subHeader1.InsertColumn(Station, "", LVCFMT_CENTER);
        m_subHeader1.InsertColumn(CurrentISCS, "", LVCFMT_CENTER);
        m_subHeader1.InsertColumn(CurrentSTIS, "", LVCFMT_CENTER);
        m_subHeader1.InsertColumn(NextISCS, "", LVCFMT_CENTER);
        m_subHeader1.InsertColumn(NextSTIS, "", LVCFMT_CENTER);
        m_subHeader1.InsertColumn(Spare, "", LVCFMT_CENTER);
        m_subHeader1.InsertItem(0, "翴");
		m_subHeader1.SetItemText(0, CurrentISCS, "侯菏北╰参");
		m_subHeader1.SetItemText(0, CurrentSTIS, "戈癟╰参");
		m_subHeader1.SetItemText(0, NextISCS, "侯菏北╰参");
		m_subHeader1.SetItemText(0, NextSTIS, "戈癟╰参");
        m_subHeader1.setItemColour(0, RGB(255, 255, 255),
                                   ListCtrlSelNoFocus::I_INDEX);
        m_subHeader1.setItemColour(0, RGB(0, 0, 0),
                                   ListCtrlSelNoFocus::I_INDEX,
                                   ListCtrlSelNoFocus::CT_BACKGROUND);

        m_subHeader1.SetColumnWidth(Station, stationHeaderSize);
        m_subHeader1.SetColumnWidth(CurrentISCS, currentIscsHeaderSize);
        m_subHeader1.SetColumnWidth(CurrentSTIS, currentStisHeaderSize);
        m_subHeader1.SetColumnWidth(NextISCS, nextIscsHeaderSize);
        m_subHeader1.SetColumnWidth(NextSTIS, nextStisHeaderSize);
        m_subHeader1.SetColumnWidth(Spare, spareHeaderSize);

        m_stationVersionList1.InsertColumn(Station, "");
        m_stationVersionList1.InsertColumn(CurrentISCS, "", LVCFMT_CENTER);
        m_stationVersionList1.InsertColumn(CurrentSTIS, "", LVCFMT_CENTER);
        m_stationVersionList1.InsertColumn(NextISCS, "", LVCFMT_CENTER);
        m_stationVersionList1.InsertColumn(NextSTIS, "", LVCFMT_CENTER);
        m_stationVersionList1.InsertColumn(Spare, "");

        m_stationVersionList1.SetColumnWidth(Station, stationHeaderSize);
        m_stationVersionList1.SetColumnWidth(CurrentISCS, currentIscsHeaderSize);
        m_stationVersionList1.SetColumnWidth(CurrentSTIS, currentStisHeaderSize);
        m_stationVersionList1.SetColumnWidth(NextISCS, nextIscsHeaderSize);
        m_stationVersionList1.SetColumnWidth(NextSTIS, nextStisHeaderSize);
        m_stationVersionList1.SetColumnWidth(Spare, spareHeaderSize);
        m_stationVersionList1.setSelectable(true);

        FUNCTION_EXIT;
    }

    void LibraryVersionPage::setupLists2()
    {
        FUNCTION_ENTRY("setupLists2");

		m_mainHeader2.SetFont(&m_listfont,TRUE);
		m_mainHeader2.GetHeaderCtrl()->SetFont(&m_listfont, TRUE);	
		m_subHeader2.SetFont(&m_listfont,TRUE);
		m_subHeader2.GetHeaderCtrl()->SetFont(&m_listfont, TRUE);
		
        // set the column headers
        RECT rect;
        m_mainHeader2.GetClientRect(&rect);

        int stationHeaderSize = 80;
        int currentIscsHeaderSize = 150;
        int currentStisHeaderSize = 150;
        int nextIscsHeaderSize = 150;
        int nextStisHeaderSize = 150;
        int spareHeaderSize = rect.right - (stationHeaderSize +
                                            currentIscsHeaderSize +
                                            currentStisHeaderSize +
                                            nextIscsHeaderSize +
                                            nextStisHeaderSize);

        // Fancy pants headers
        m_mainHeader2.setScrollBarVisibility(false, false);
        m_mainHeader2.InsertColumn(0, "", LVCFMT_CENTER);
        m_mainHeader2.InsertColumn(1, "", LVCFMT_CENTER);
        m_mainHeader2.InsertColumn(2, "", LVCFMT_CENTER);
        m_mainHeader2.InsertColumn(3, "", LVCFMT_CENTER);
        m_mainHeader2.SetColumnWidth(0, stationHeaderSize);
        m_mainHeader2.SetColumnWidth(1, currentIscsHeaderSize + currentStisHeaderSize);
        m_mainHeader2.SetColumnWidth(2, nextIscsHeaderSize + nextStisHeaderSize);
        m_mainHeader2.SetColumnWidth(3, spareHeaderSize);
        m_mainHeader2.InsertItem(0, "");
		m_mainHeader2.SetItemText(0, 1, "ヘ玡家狾セ");
		m_mainHeader2.SetItemText(0, 2, "家狾セ");
        m_mainHeader2.setItemColour(0, RGB(255, 255, 255),
                                    ListCtrlSelNoFocus::I_INDEX);
        m_mainHeader2.setItemColour(0, RGB(0, 0, 0),
                                    ListCtrlSelNoFocus::I_INDEX,
                                    ListCtrlSelNoFocus::CT_BACKGROUND);

        m_subHeader2.setScrollBarVisibility(false, false);
        m_subHeader2.InsertColumn(Station, "", LVCFMT_CENTER);
        m_subHeader2.InsertColumn(CurrentISCS, "", LVCFMT_CENTER);
        m_subHeader2.InsertColumn(CurrentSTIS, "", LVCFMT_CENTER);
        m_subHeader2.InsertColumn(NextISCS, "", LVCFMT_CENTER);
        m_subHeader2.InsertColumn(NextSTIS, "", LVCFMT_CENTER);
        m_subHeader2.InsertColumn(Spare, "", LVCFMT_CENTER);
		m_subHeader2.InsertItem(0, "翴");
		m_subHeader2.SetItemText(0, CurrentISCS, "侯菏北╰参");
		m_subHeader2.SetItemText(0, CurrentSTIS, "戈癟╰参");
		m_subHeader2.SetItemText(0, NextISCS, "侯菏北╰参");
		m_subHeader2.SetItemText(0, NextSTIS, "戈癟╰参");
        m_subHeader2.setItemColour(0, RGB(255, 255, 255),
                                   ListCtrlSelNoFocus::I_INDEX);
        m_subHeader2.setItemColour(0, RGB(0, 0, 0),
                                   ListCtrlSelNoFocus::I_INDEX,
                                   ListCtrlSelNoFocus::CT_BACKGROUND);

        m_subHeader2.SetColumnWidth(Station, stationHeaderSize);
        m_subHeader2.SetColumnWidth(CurrentISCS, currentIscsHeaderSize);
        m_subHeader2.SetColumnWidth(CurrentSTIS, currentStisHeaderSize);
        m_subHeader2.SetColumnWidth(NextISCS, nextIscsHeaderSize);
        m_subHeader2.SetColumnWidth(NextSTIS, nextStisHeaderSize);
        m_subHeader2.SetColumnWidth(Spare, spareHeaderSize);

        m_stationVersionList2.InsertColumn(Station, "");
        m_stationVersionList2.InsertColumn(CurrentISCS, "", LVCFMT_CENTER);
        m_stationVersionList2.InsertColumn(CurrentSTIS, "", LVCFMT_CENTER);
        m_stationVersionList2.InsertColumn(NextISCS, "", LVCFMT_CENTER);
        m_stationVersionList2.InsertColumn(NextSTIS, "", LVCFMT_CENTER);
        m_stationVersionList2.InsertColumn(Spare, "");

        m_stationVersionList2.SetColumnWidth(Station, stationHeaderSize);
        m_stationVersionList2.SetColumnWidth(CurrentISCS, currentIscsHeaderSize);
        m_stationVersionList2.SetColumnWidth(CurrentSTIS, currentStisHeaderSize);
        m_stationVersionList2.SetColumnWidth(NextISCS, nextIscsHeaderSize);
        m_stationVersionList2.SetColumnWidth(NextSTIS, nextStisHeaderSize);
        m_stationVersionList2.SetColumnWidth(Spare, spareHeaderSize);
        m_stationVersionList2.setSelectable(true);

        FUNCTION_EXIT;
    }

    void LibraryVersionPage::populateMessagePageData()
    {
        FUNCTION_ENTRY("populateMessagePageData");

        TA_THREADGUARD(m_messageVersionInfoLock);

        CString str;
        str.Format("%03d", m_currentMessageLibraryVersion);
        m_currentISCSMessageVersion.SetWindowText(str);

        str.Format("%03d", m_nextMessageLibraryVersion);
        m_nextISCSMessageVersion.SetWindowText(str);

        CString currentISCSVersionString;
        CString nextISCSVersionString;
        CString currentSTISVersionString;
        CString nextSTISVersionString;

        m_stationVersionList1.DeleteAllItems();

        try
        {
            int count = 0;

            // Helper: Convert UTF-8 to Unicode (wide string) for MFC display
            auto utf8ToWide = [](const std::string& utf8) -> std::wstring {
                if (utf8.empty()) return std::wstring();
                int wlen = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, NULL, 0);
                if (wlen <= 1) return std::wstring();
                std::wstring wstr(wlen - 1, 0);  // -1 to exclude null terminator from string size
                MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, &wstr[0], wlen);
                return wstr;
            };

            // Helper: Insert item using Unicode directly (works regardless of system code page)
            auto insertItemW = [](CListCtrl& list, int index, const std::wstring& text) -> int {
                LVITEMW lvi = {0};
                lvi.mask = LVIF_TEXT;
                lvi.iItem = index;
                lvi.iSubItem = 0;
                lvi.pszText = const_cast<LPWSTR>(text.c_str());
                return (int)::SendMessageW(list.m_hWnd, LVM_INSERTITEMW, 0, (LPARAM)&lvi);
            };

            for (auto&& [_, info] : LibraryVersionMonitor::instance().getAllMessageLibraryVersions())
            {
                LOG_DEBUG("location_name raw = %s (len=%d)", info.name.c_str(), info.name.length());
                
                // Log first few bytes in hex to verify encoding
                std::string hexBytes;
                for (size_t i = 0; i < std::min(info.name.length(), (size_t)20); ++i) {
                    char buf[8];
                    sprintf(buf, "%02X ", (unsigned char)info.name[i]);
                    hexBytes += buf;
                }
                LOG_DEBUG("location_name hex bytes: %s", hexBytes.c_str());

                // Database stores UTF-8, convert to Unicode for display
                std::wstring locationNameW = utf8ToWide(info.name);
                LOG_DEBUG("location_name wide len = %d", locationNameW.length());

                int result = insertItemW(m_stationVersionList1, count, locationNameW);
                LOG_DEBUG("InsertItemW result = %d", result);
                // If the location name is occ, then show the location name as OCCA in list.
                //m_stationVersionList1.InsertItem(count, info.name.c_str());
                currentISCSVersionString = get_version(info.currentISCSLibraryVersionKey).c_str();
                currentSTISVersionString = get_version(info.currentSTISLibraryVersionKey).c_str();
                nextISCSVersionString = get_version(info.nextISCSLibraryVersionKey).c_str();
                nextSTISVersionString = get_version(info.nextSTISLibraryVersionKey).c_str();

                m_stationVersionList1.SetItemText(count, CurrentISCS, currentISCSVersionString);
                m_stationVersionList1.SetItemText(count, CurrentSTIS, currentSTISVersionString);
                m_stationVersionList1.SetItemText(count, NextISCS, nextISCSVersionString);
                m_stationVersionList1.SetItemText(count, NextSTIS, nextSTISVersionString);
                m_stationVersionList1.SetItemData(count, info.locationKey);

                // Display the OCC points in red
                if (boost::iequals(info.name, "NDOCC"))
                {
                    m_stationVersionList1.setItemColour(info.locationKey,
                                                        RGB(255, 0, 0),
                                                        ListCtrlSelNoFocus::I_ITEMDATA);
                }
                else // otherwise display in black
                {
                    m_stationVersionList1.setItemColour(info.locationKey,
                                                        RGB(0, 0, 0),
                                                        ListCtrlSelNoFocus::I_ITEMDATA);
                }

                count++;
            }
        }
        catch (...)
        {
            LOG_EXCEPTION("Unknown", "LibraryVersionPage::populateMessagePageData() Unknown Exception");
        }

        FUNCTION_EXIT;
    }

    void LibraryVersionPage::populateTemplatePageData()
    {
        FUNCTION_ENTRY("populateTemplatePageData");

        TA_THREADGUARD(m_templateVersionInfoLock);

        CString str;
        str.Format("%03d", m_currentTemplateLibraryVersion);
        m_currentISCSTemplateVersion.SetWindowText(str);

        str.Format("%03d", m_nextTemplateLibraryVersion);
        m_nextISCSTemplateVersion.SetWindowText(str);

        CString currentISCSVersionString;
        CString nextISCSVersionString;
        CString currentSTISVersionString;
        CString nextSTISVersionString;

        m_stationVersionList2.DeleteAllItems();

        try
        {
            int count = 0;

            // Helper: Convert UTF-8 to Unicode (wide string) for MFC display
            auto utf8ToWide = [](const std::string& utf8) -> std::wstring {
                if (utf8.empty()) return std::wstring();
                int wlen = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, NULL, 0);
                if (wlen <= 1) return std::wstring();
                std::wstring wstr(wlen - 1, 0);
                MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, &wstr[0], wlen);
                return wstr;
            };

            // Helper: Insert item using Unicode directly
            auto insertItemW = [](CListCtrl& list, int index, const std::wstring& text) -> int {
                LVITEMW lvi = {0};
                lvi.mask = LVIF_TEXT;
                lvi.iItem = index;
                lvi.iSubItem = 0;
                lvi.pszText = const_cast<LPWSTR>(text.c_str());
                return (int)::SendMessageW(list.m_hWnd, LVM_INSERTITEMW, 0, (LPARAM)&lvi);
            };

            for (auto&& [_, info] : LibraryVersionMonitor::instance().getAllTemplateLibraryVersions())
            {
                LOG_DEBUG("location_name raw = %s", info.name.c_str());

                // Database stores UTF-8, convert to Unicode for display
                std::wstring locationNameW = utf8ToWide(info.name);

                insertItemW(m_stationVersionList2, count, locationNameW);
                // If the location name is occ, then show the location name as OCCA in list.
                //m_stationVersionList2.InsertItem(count, info.name.c_str());
                currentISCSVersionString = get_version(info.currentISCSLibraryVersionKey).c_str();
                currentSTISVersionString = get_version(info.currentSTISLibraryVersionKey).c_str();
                nextISCSVersionString = get_version(info.nextISCSLibraryVersionKey).c_str();
                nextSTISVersionString = get_version(info.nextSTISLibraryVersionKey).c_str();

                m_stationVersionList2.SetItemText(count, CurrentISCS, currentISCSVersionString);
                m_stationVersionList2.SetItemText(count, CurrentSTIS, currentSTISVersionString);
                m_stationVersionList2.SetItemText(count, NextISCS, nextISCSVersionString);
                m_stationVersionList2.SetItemText(count, NextSTIS, nextSTISVersionString);
                m_stationVersionList2.SetItemData(count, info.locationKey);

                // Display the OCC points in red
                if (boost::iequals(info.name, "OCC"))
                {
                    m_stationVersionList2.setItemColour(info.locationKey,
                                                        RGB(255, 0, 0),
                                                        ListCtrlSelNoFocus::I_ITEMDATA);
                }
                else // otherwise display in black
                {
                    m_stationVersionList2.setItemColour(info.locationKey,
                                                        RGB(0, 0, 0),
                                                        ListCtrlSelNoFocus::I_ITEMDATA);
                }

                count++;
            }
        }
        catch (...)
        {
            LOG_EXCEPTION("Unknown", "LibraryVersionPage::populateTemplatePageData() Unknown Exception");
        }

        FUNCTION_EXIT;
    }

    void LibraryVersionPage::enableUpgradeIscsMessageButton()
    {
        FUNCTION_ENTRY("enableUpgradeIscsMessageButton");

        // enable the upgrade ISCS message button if
        // - the libraries are synchronised across the stations
        // - The library hasnt been upgraded yet
        bool enableButton = (ThisLocation::is_occ() &&
                             m_canUpgradeISCS == true &&
                             //m_messageLibrariesAreSynchronised == true &&
                             (m_currentMessageLibraryVersion != m_nextMessageLibraryVersion));

        GetDlgItem(IDC_UPGRADE_ISCS_MESSAGE)->EnableWindow(enableButton);

        FUNCTION_EXIT;
    }

    void LibraryVersionPage::enableUpgradeIscsTemplateButton()
    {
        FUNCTION_ENTRY("enableUpgradeIscsTemplateButton");

        // enable the upgrade ISCS template button if
        // - the libraries are synchronised across the stations
        // - The library hasnt been upgraded yet
        bool enableButton = (ThisLocation::is_occ() &&
                             m_canUpgradeISCS == true &&
                             //m_templateLibrariesAreSynchronised == true &&
                             (m_currentTemplateLibraryVersion != m_nextTemplateLibraryVersion));

        GetDlgItem(IDC_UPGRADE_ISCS_TEMPLATE)->EnableWindow(enableButton);

        FUNCTION_EXIT;
    }

    void LibraryVersionPage::OnUpgradeIscsMessage()
    {
        FUNCTION_ENTRY("OnUpgradeIscsMessage");

        std::string sessionId = RunParams::getInstance().get(RPARAM_SESSIONID);

        int ret = UserMessages::getInstance().askQuestion(UserMessages::QUESTION_UPGRADE_LIBRARY);

        if (ret != IDYES)
        {
            FUNCTION_EXIT;
            return;
        }

        unsigned short version = static_cast<unsigned short>(m_nextMessageLibraryVersion);

        try
        {
            STISClient::instance().upgrade_message_library_version(std::to_string(version));
#if 1
            STISClient::instance().change_sync_interval_for_a_while(100, 10000);
            LibraryVersionMonitor::instance().change_sync_interval_for_a_while(100, 10000);
            STISAuditMessage::upgradeMessageLibrary(DigitString<3>{m_nextMessageLibraryVersion});
#else
            STISClient::instance().change_sync_interval_until(500, 100, 15 * 1000, [=, version = STISClient::instance().next_message_library_version()]
                                                              {
                                                                  return STISClient::instance().current_message_library_version() == version;
                                                              });
#endif
        }
        catch (const ObjectResolutionException& ore)
        {
            LOG_EXCEPTION("ObjectResolutionException", ore.what());

            UserMessages::getInstance().displayError(
                str(format(UserMessages::ERROR_REQUEST_FAILED)
                    % "Upgrade STIS predefined message library" % "Could not resolve TIS Agent").c_str());
        }
        catch (const CORBA::Exception& ce)
        {
            LOG_EXCEPTION("CORBA::Exception", CorbaUtil::exceptionToString(ce));

            UserMessages::getInstance().displayError(
                str(format(UserMessages::ERROR_REQUEST_FAILED)
                    % "Upgrade STIS predefined message library" % "Could not resolve TIS Agent").c_str());
        }
        catch (...)
        {
            LOG_EXCEPTION("...", "While attempting to upgrade the STIS predefined message library");

            UserMessages::getInstance().displayError(
                str(format(UserMessages::ERROR_REQUEST_FAILED)
                    % "Upgrade STIS predefined message library" % "Could not resolve TIS Agent").c_str());
        }

        FUNCTION_EXIT;
    }

    void LibraryVersionPage::OnUpgradeIscsTemplate()
    {
        FUNCTION_ENTRY("OnUpgradeIscsTemplate");

        std::string sessionId = RunParams::getInstance().get(RPARAM_SESSIONID);

        int ret = UserMessages::getInstance().askQuestion(UserMessages::QUESTION_UPGRADE_LIBRARY);

        if (ret != IDYES)
        {
            FUNCTION_EXIT;
            return;
        }

        unsigned short version = static_cast<unsigned short>(m_nextTemplateLibraryVersion);

        try
        {
            STISClient::instance().upgrade_template_library_version(std::to_string(version));
#if 1
            STISClient::instance().change_sync_interval_for_a_while(100, 10000);
            LibraryVersionMonitor::instance().change_sync_interval_for_a_while(100, 10000);
            STISAuditMessage::upgradeTemplateLibrary(DigitString<3>{m_nextTemplateLibraryVersion});
#else
            STISClient::instance().change_sync_interval_until(500, 100, 15 * 1000, [=, version = STISClient::instance().next_template_library_version()]
                                                              {
                                                                  return STISClient::instance().current_template_library_version() == version;
                                                              });
#endif
        }
        catch (const ObjectResolutionException& ore)
        {
            LOG_EXCEPTION("ObjectResolutionException", ore.what());

            UserMessages::getInstance().displayError(
                str(format(UserMessages::ERROR_REQUEST_FAILED)
                    % "Upgrade STIS predefined template library" % "Could not resolve TIS Agent").c_str());
        }
        catch (const CORBA::Exception& ce)
        {
            LOG_EXCEPTION("CORBA::Exception", CorbaUtil::exceptionToString(ce));

            UserMessages::getInstance().displayError(
                str(format(UserMessages::ERROR_REQUEST_FAILED)
                    % "Upgrade STIS predefined template library" % "Could not resolve TIS Agent").c_str());
        }
        catch (...)
        {
            LOG_EXCEPTION("...", "While attempting to upgrade the STIS predefined template library");

            UserMessages::getInstance().displayError(
                str(format(UserMessages::ERROR_REQUEST_FAILED)
                    % "Upgrade STIS predefined template library" % "Could not resolve TIS Agent").c_str());
        }

        FUNCTION_EXIT;
    }

    void LibraryVersionPage::OnOK()
    {
        FUNCTION_ENTRY("OnOK");
        FUNCTION_EXIT;
    }

    void LibraryVersionPage::OnCancel()
    {
        FUNCTION_ENTRY("OnCancel");
        FUNCTION_EXIT;
    }

    BOOL LibraryVersionPage::PreTranslateMessage(MSG* pMsg)
    {
        FUNCTION_ENTRY("PreTranslateMessage");

        switch (pMsg->message)
        {
        case WM_KEYDOWN:
        {
            switch (pMsg->wParam)
            {
            case VK_ESCAPE:
            {
                FUNCTION_EXIT;
                return TRUE;
            }
            break;

            case VK_RETURN:
            {
                FUNCTION_EXIT;
                return TRUE;
            }
            break;

            default:
            {
                // do nothing
            }
            break;
            }
        }
        break;

        default:
        {
            // do nothing
        }
        }

        FUNCTION_EXIT;
        return CDialog::PreTranslateMessage(pMsg);
    }

    LRESULT LibraryVersionPage::OnMessageLibraryVersionChanged(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY("OnMessageLibraryVersionChanged");

        TA_THREADGUARD(m_messageVersionInfoLock);

        LOG_INFO("Received library version datapoint change");

        populateMessagePageData();

        FUNCTION_EXIT;
        return 0;
    }

    // Only the OCC should have registered for these
    LRESULT LibraryVersionPage::OnCurrentMessageLibraryVersionChanged(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY("OnCurrentMessageLibraryVersionChanged");

        TA_THREADGUARD(m_messageVersionInfoLock);

        m_currentMessageLibraryVersion = m_stisPredefinedMessages->getCurrentMessageLibraryVersion();

        CString str;
        str.Format("%03d", m_currentMessageLibraryVersion);
        m_currentISCSMessageVersion.SetWindowText(str);

        enableUpgradeIscsMessageButton();

        FUNCTION_EXIT;
        return 0;
    }

    LRESULT LibraryVersionPage::OnNextMessageLibraryVersionChanged(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY("OnNextMessageLibraryVersionChanged");

        TA_THREADGUARD(m_messageVersionInfoLock);

        m_nextMessageLibraryVersion = m_stisPredefinedMessages->getNextMessageLibraryVersion();

        CString str;
        str.Format("%03d", m_nextMessageLibraryVersion);
        m_nextISCSMessageVersion.SetWindowText(str);

        enableUpgradeIscsMessageButton();

        FUNCTION_EXIT;
        return 0;
    }

    LRESULT LibraryVersionPage::OnOCCMessageLibrariesSynchronisedChanged(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY("OnOCCMessageLibrariesSynchronisedChanged");

        m_messageLibrariesAreSynchronised = m_stisPredefinedMessages->getMessageLibrarySynchronised();

        // If rights and synchronisation permit...
        enableUpgradeIscsMessageButton();

        FUNCTION_EXIT;
        return 0;
    }

    LRESULT LibraryVersionPage::OnTemplateLibraryVersionChanged(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY("OnTemplateLibraryVersionChanged");

        TA_THREADGUARD(m_templateVersionInfoLock);

        LOG_INFO("Received template library version change");

        populateTemplatePageData();

        FUNCTION_EXIT;
        return 0;
    }

    // Only the OCC should have registered for these
    LRESULT LibraryVersionPage::OnCurrentTemplateLibraryVersionChanged(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY("OnCurrentTemplateLibraryVersionChanged");

        TA_THREADGUARD(m_templateVersionInfoLock);

        m_currentTemplateLibraryVersion = m_stisTemplates->getCurrentTemplateLibraryVersion();

        CString str;
        str.Format("%03d", m_currentTemplateLibraryVersion);
        m_currentISCSTemplateVersion.SetWindowText(str);

        enableUpgradeIscsTemplateButton();

        FUNCTION_EXIT;
        return 0;
    }

    LRESULT LibraryVersionPage::OnNextTemplateLibraryVersionChanged(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY("OnNextTemplateLibraryVersionChanged");

        TA_THREADGUARD(m_templateVersionInfoLock);

        m_nextTemplateLibraryVersion = m_stisTemplates->getNextTemplateLibraryVersion();

        CString str;
        str.Format("%03d", m_nextTemplateLibraryVersion);
        m_nextISCSTemplateVersion.SetWindowText(str);

        enableUpgradeIscsTemplateButton();

        FUNCTION_EXIT;
        return 0;
    }

    LRESULT LibraryVersionPage::OnOCCTemplateLibrariesSynchronisedChanged(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY("OnOCCTemplateLibrariesSynchronisedChanged");

        m_templateLibrariesAreSynchronised = m_stisTemplates->getTemplateLibrarySynchronised();

        // If rights and synchronisation permit...
        enableUpgradeIscsTemplateButton();

        FUNCTION_EXIT;
        return 0;
    }
} // TA_IRS_App
