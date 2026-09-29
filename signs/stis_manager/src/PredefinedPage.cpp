/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File$
 * @author:  Robin Ashcroft
 * @version: $Revision$
 *
 * Last modification: $DateTime$
 * Last modified by:  $Author$
 *
 * The predefined message selection tab
 */

#include "stdafx.h"
#include "helperfun.h"
#include "stismanager.h"
#include "PredefinedPage.h"
#include "STISPredefinedMessages.h"
#include "WindowsUtil.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/IConsole.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/entity_access/src/TISAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/STISEntityData.h"
#include "core/utility/src/core/StdEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::PredefinedMessage;

namespace
{
    static bool s_internal_event = false;
}

namespace TA_IRS_App
{
    CPredefinedPage::CPredefinedPage(CWnd* pParent /*=NULL*/)
        : CDialog(CPredefinedPage::IDD, pParent),
        m_messageSelectionListener(NULL), // TD11310 ~ added
        m_predefinedContent(std::make_shared<SimpleUnicodeEdit>(this, IDC_PREDEFINED_CONTENT))

    {
        //{{AFX_DATA_INIT(CPredefinedPage)
        //}}AFX_DATA_INIT
    }

    CPredefinedPage::~CPredefinedPage()
    {
        // no longer accepting updates
        m_stisPredefinedMessages->deregisterCurrentVersionUser(this);
        //m_stisPredefinedMessages->removeInstance();  // TD11310 ~ added
    }

    void CPredefinedPage::DoDataExchange(CDataExchange* pDX)
    {
        CDialog::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(CPredefinedPage)
        DDX_Control(pDX, IDC_NORMAL_PREDEFINED, m_normalList);
        DDX_Control(pDX, IDC_EMERGENCY_PREDEFINED, m_emergencyList);
        //DDX_Control(pDX, IDC_LANGUAGES, m_languages);
        DDX_Control(pDX, IDC_SEARCH_NORMAL, m_searchNormal);
        DDX_Control(pDX, IDC_SEARCH_EMERGENCY, m_searchEmergency);
        //DDX_Control(pDX, IDC_PREDEFINED_CONTENT, m_predefinedContent);
        //}}AFX_DATA_MAP
    }

    BEGIN_MESSAGE_MAP(CPredefinedPage, CDialog)
        //{{AFX_MSG_MAP(CPredefinedPage)
        ON_WM_SHOWWINDOW()
        ON_EN_UPDATE(IDC_SEARCH_EMERGENCY, OnUpdateSearchEmergency)
        ON_EN_UPDATE(IDC_SEARCH_NORMAL, OnUpdateSearchNormal)
        ON_NOTIFY(NM_CLICK, IDC_EMERGENCY_PREDEFINED, OnClickEmergencyPredefined)
        ON_NOTIFY(NM_CLICK, IDC_NORMAL_PREDEFINED, OnClickNormalPredefined)
        ON_NOTIFY(LVN_ITEMCHANGED, IDC_NORMAL_PREDEFINED, onItemchangedNormalPredefined)
        ON_NOTIFY(LVN_ITEMCHANGED, IDC_EMERGENCY_PREDEFINED, onItemchangedEmergencyPredefined)
        ON_MESSAGE(WM_UPDATE_CURRENT_STIS_VERSION, OnUpdateCurrentSTISVersion)
        ON_WM_DESTROY()
        //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // CPredefinedPage message handlers

    BOOL CPredefinedPage::OnInitDialog()
    {
        CDialog::OnInitDialog();
        m_predefinedContent->init();
        m_predefinedContent->set_readonly(TRUE);
        return TRUE;  // return TRUE unless you set the focus to a control
        // EXCEPTION: OCX Property Pages should return FALSE
    }

    void CPredefinedPage::init()
    {
        m_stisPredefinedMessages = STISPredefinedMessages::getInstance();
        // extended styles (including infotips)
        // column widths for message titles
        m_emergencyList.SetExtendedStyle(m_emergencyList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);
        m_normalList.SetExtendedStyle(m_normalList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);

        RECT rect;
        m_emergencyList.GetClientRect(&rect);
        m_emergencyList.InsertColumn(0, "Message Name");
        m_emergencyList.SetColumnWidth(0, rect.right);
        m_normalList.GetClientRect(&rect);
        m_normalList.InsertColumn(0, "Message Name");
        m_normalList.SetColumnWidth(0, rect.right);

        // load the pre-defined messages into the lists
        populateLists();

        // register with the pre-defined message singleton to be updated of a change
        m_stisPredefinedMessages->registerCurrentVersionUser(this);

        // set the available languages box to empty
        //m_languages.SetWindowText("");

        m_emergencyList.setScrollBarVisibility(true);
        m_normalList.setScrollBarVisibility(true);
    }

    void CPredefinedPage::OnDestroy()
    {
        CDialog::OnDestroy();
    }

    void CPredefinedPage::windowShown()
    {
        if (m_messageSelectionListener != NULL)
        {
            m_validMessage = hasValidSelection();

            if (m_validMessage)
            {
                PredefinedMessage message = getMessage();
                m_messageSelectionListener->predefinedMessageSelected(true, m_validMessage, message.description.c_str(), message.priority);
            }
            else
            {
                m_messageSelectionListener->predefinedMessageSelected(true, m_validMessage);
            }
        }
    }

    void CPredefinedPage::setMessageSelectionListener(IMessageSelectionListener* messageSelectionListener)
    {
        m_messageSelectionListener = messageSelectionListener;
    }

    LRESULT CPredefinedPage::OnUpdateCurrentSTISVersion(WPARAM wParam, LPARAM lParam)
    {
        // rebuild the lists
        populateLists();

        // update all the stuff to do with selected items in lists
        updatePreviewArea();

        return 0;
    }

    bool CPredefinedPage::hasValidSelection() const
    {
        FUNCTION_ENTRY("CPredefinedPage::hasValidSelection");
        FUNCTION_EXIT;
        return getSelectedMessageData();
    }

    TA_Base_Core::PredefinedMessage CPredefinedPage::getMessage() const
    {
        FUNCTION_ENTRY("CPredefinedPage::getMessage");
        auto msg = getSelectedMessageData();
        TA_ASSERT(msg, "CPredefinedPage::getMessage() called when there is no valid message selected. Call hasValidSelection() first.");
        FUNCTION_EXIT;
        return *msg;
    }

    void CPredefinedPage::populateLists()
    {
        LOG_FUNCTION_ENTRY(SourceInfo, "PredefinedMessage::populateLists");

        m_normalList.SendMessage(WM_SETREDRAW, FALSE, 0);
        m_emergencyList.SendMessage(WM_SETREDRAW, FALSE, 0);
        BOOST_SCOPE_EXIT_ALL(&)
        {
            m_normalList.SendMessage(WM_SETREDRAW, TRUE, 0);
            m_emergencyList.SendMessage(WM_SETREDRAW, TRUE, 0);
        };

        // clear the list of pre-defined messages
        m_normalList.DeleteAllItems();
        m_emergencyList.DeleteAllItems();

        int count = 0;

        for (auto&& msg : m_stisPredefinedMessages->getNormalMessages())
        {
            // add the string to the list
            int index = m_normalList.InsertItem(count, msg.description.c_str());
            m_normalList.SetItemData(index, static_cast<DWORD>(msg.messageTag));
            count++;
        }

        count = 0;

        for (auto&& msg : m_stisPredefinedMessages->getEmergencyMessages())
        {
            // add the string to the list
            int index = m_emergencyList.InsertItem(count, msg.description.c_str());
            m_emergencyList.SetItemData(index, static_cast<DWORD>(msg.messageTag));

            // set the emergency colour to red
            m_emergencyList.setItemColour(count, COLORREF(RGB(255, 0, 0)), ListCtrlSelNoFocus::I_INDEX);
            count++;
        }

        LOG_FUNCTION_EXIT(SourceInfo, "PredefinedMessage::populateLists");
    }

    const TA_Base_Core::PredefinedMessage* CPredefinedPage::getSelectedMessageData() const
    {
        LOG_FUNCTION_ENTRY(SourceInfo, "CPredefinedPage::getSelectedMessageData");
        const PredefinedMessage* message = NULL;

        // get the selected index for the normal priority list
        int normalSelected = -1;
        POSITION currSel = m_normalList.GetFirstSelectedItemPosition();

        // if something is selected
        if (currSel != NULL)
        {
            // get the selected index
            normalSelected = m_normalList.GetNextSelectedItem(currSel);
        }

        // get the index for the emergency priority list
        currSel = NULL;
        int emergencySelected = -1;
        currSel = m_emergencyList.GetFirstSelectedItemPosition();

        // if something is selected
        if (currSel != NULL)
        {
            // get the selected index
            emergencySelected = m_emergencyList.GetNextSelectedItem(currSel);
        }

        // if a normal message is selected
        if (normalSelected > -1)
        {
            // get the data for the selected item
            unsigned short itemData = static_cast<unsigned short>(m_normalList.GetItemData(normalSelected));

            // find the appropriate message
            message = m_stisPredefinedMessages->getNormalMessageById(itemData);

            // anything selected should point to a valid message
            TA_ASSERT(message, "The selected message is not a valid message");
        }
        // otherwise if an emergency message is selected
        else if (emergencySelected > -1)
        {
            // get the text for the selected item
            unsigned short itemData = static_cast<unsigned short>(m_emergencyList.GetItemData(emergencySelected));

            // find the appropriate message
            message = m_stisPredefinedMessages->getEmergencyMessageById(itemData);

            // anything selected should point to a valid message
            TA_ASSERT(message, "The selected message is not a valid message");
        }

        // otherwise, return NULL

        LOG_FUNCTION_EXIT(SourceInfo, "CPredefinedPage::getSelectedMessageData");
        return message;
    }

    /**
     * updatePreviewArea
     *
     * updates the message preview area with the text from the selected message.
     * also updates the languages box.
     *
     */
    void CPredefinedPage::updatePreviewArea()
    {
        // get the selected message
        if (auto message = getSelectedMessageData())
        {
            // set the message text
            m_predefinedContent->set_window_text_utf8(message->message);

            // notify the owning window
            if (m_messageSelectionListener != NULL)
            {
                m_validMessage = true;
                m_messageSelectionListener->predefinedMessageSelected(false, m_validMessage, message->description.c_str(), message->priority);
                m_messageSelectionListener->predefinedMessageSelectChanged(message->description);
            }
        }
        else
        {
            // clear the text in the preview window
            m_predefinedContent->set_window_text(L"");

            // notify the owning window
            if (m_messageSelectionListener != NULL)
            {
                m_validMessage = false;
                m_messageSelectionListener->predefinedMessageSelected(false, m_validMessage);
                m_messageSelectionListener->predefinedMessageSelectChanged("");
            }
        }
    }

#if 0
    void CPredefinedPage::onItemchangedNormalPredefined(NMHDR* pNMHDR, LRESULT* pResult)
    {
        NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

        // Only interested in state changes
        if (pNMListView->uChanged == LVIF_STATE)
        {
            // if it is being selected
            if (((pNMListView->uNewState & LVIS_SELECTED) == LVIS_SELECTED) &&
                ((pNMListView->uOldState & LVIS_SELECTED) == 0))
            {
                // remove_pids emergency messages
                // m_emergencyList.SetItemState(-1, 0, LVIS_SELECTED);

                // update the display
                updatePreviewArea();
            }
        }

        *pResult = 0;
    }

    void CPredefinedPage::OnClickNormalPredefined(NMHDR* pNMHDR, LRESULT* pResult)
    {
        // remove_pids emergency messages
        m_emergencyList.SetItemState(-1, 0, LVIS_SELECTED);

        // if nothing is selected then this was effectively a deselection click
        // update the preview area
        POSITION currSel = m_normalList.GetFirstSelectedItemPosition();

        // if nothing is selected
        if (currSel == NULL)
        {
            // update the display
            updatePreviewArea();
        }

        if (pResult != NULL)
        {
            *pResult = 0;
        }
    }

    void CPredefinedPage::onItemchangedEmergencyPredefined(NMHDR* pNMHDR, LRESULT* pResult)
    {
        NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

        // Only interested in state changes
        if (pNMListView->uChanged == LVIF_STATE)
        {
            // if it is being selected
            if (((pNMListView->uNewState & LVIS_SELECTED) == LVIS_SELECTED) &&
                ((pNMListView->uOldState & LVIS_SELECTED) == 0))
            {
                // Deselect the normal priority listbox
                m_normalList.SetItemState(-1, 0, LVIS_SELECTED);

                // update the display
                updatePreviewArea();
            }
        }

        *pResult = 0;
    }

    void CPredefinedPage::OnClickEmergencyPredefined(NMHDR* pNMHDR, LRESULT* pResult)
    {
        // Deselect the normal priority listbox
        m_normalList.SetItemState(-1, 0, LVIS_SELECTED);

        // if nothing is selected then this was effectively a deselection click
        // update the preview area

        // get the index for the emergency priority list
        POSITION currSel = m_emergencyList.GetFirstSelectedItemPosition();

        // if something is selected
        if (currSel == NULL)
        {
            // update the display
            updatePreviewArea();
        }

        if (pResult != NULL)
        {
            *pResult = 0;
        }
    }
#else

    void CPredefinedPage::onItemchangedNormalPredefined(NMHDR* pNMHDR, LRESULT* pResult)
    {
        if (auto pNMListView = (NM_LISTVIEW*)pNMHDR; pNMListView->uChanged == LVIF_STATE)
        {
            if (!s_internal_event)
            {
                s_internal_event = true;
                BOOST_SCOPE_EXIT_ALL() { s_internal_event = false; };
                m_emergencyList.SetItemState(-1, 0, LVIS_SELECTED);
                updatePreviewArea();
            }
        }

        *pResult = 0;
    }

    void CPredefinedPage::OnClickNormalPredefined(NMHDR* pNMHDR, LRESULT* pResult)
    {
        updatePreviewArea();
        *pResult = 0;
    }

    void CPredefinedPage::onItemchangedEmergencyPredefined(NMHDR* pNMHDR, LRESULT* pResult)
    {
        if (auto pNMListView = (NM_LISTVIEW*)pNMHDR; pNMListView->uChanged == LVIF_STATE)
        {
            if (!s_internal_event)
            {
                s_internal_event = true;
                BOOST_SCOPE_EXIT_ALL() { s_internal_event = false; };
                m_normalList.SetItemState(-1, 0, LVIS_SELECTED);
                updatePreviewArea();
            }
        }

        *pResult = 0;
    }

    void CPredefinedPage::OnClickEmergencyPredefined(NMHDR* pNMHDR, LRESULT* pResult)
    {
        updatePreviewArea();
        *pResult = 0;
    }
#endif

    void CPredefinedPage::OnUpdateSearchNormal()
    {
        // get the text to search for from the text box.
        CString searchText;
        m_searchNormal.GetWindowText(searchText);
        searchText.MakeLower();

        // rebuild the list based on the search text
        m_normalList.DeleteAllItems();

        // get the STIS message library
        // the singleton below that retrieves the pre-defined messages will
        // only throw exceptions the first time it is used.

        int count = 0;

        for (auto&& msg : m_stisPredefinedMessages->getNormalMessages())
        {
            CString messageString(msg.description.c_str());
            messageString.MakeLower();

            // if the string matches
            if (messageString.Find(searchText) != -1)
            {
                // add the string to the list
                int index = m_normalList.InsertItem(count, msg.description.c_str());
                m_normalList.SetItemData(index, static_cast<DWORD>(msg.messageTag));
                count++;
            }
        }

        // remove_pids item and set window text
        m_emergencyList.SetItemState(-1, 0, LVIS_SELECTED);
        m_normalList.SetItemState(-1, 0, LVIS_SELECTED);
        updatePreviewArea();
    }

    void CPredefinedPage::OnUpdateSearchEmergency()
    {
        CString searchText;
        m_searchEmergency.GetWindowText(searchText);
        searchText.MakeLower();

        // rebuild the list based on the string
        m_emergencyList.DeleteAllItems();

        // Load the predefined message library
        // the singleton below that retrieves the pre-defined messages will
        // only throw exceptions the first time it is used.
        int count = 0;

        for (auto&& msg : m_stisPredefinedMessages->getEmergencyMessages())
        {
            CString messageString(msg.description.c_str());
            messageString.MakeLower();

            // if the string matches
            if (messageString.Find(searchText) != -1)
            {
                // add the string to the list
                int index = m_emergencyList.InsertItem(count, msg.description.c_str());
                m_emergencyList.SetItemData(index, static_cast<DWORD>(msg.messageTag));
                // set the emergency colour to red
                m_emergencyList.setItemColour(count,
                                              COLORREF(RGB(255, 0, 0)),
                                              ListCtrlSelNoFocus::I_INDEX);
                count++;
            }
        }

        // remove_pids item and set window text
        m_emergencyList.SetItemState(-1, 0, LVIS_SELECTED);
        m_normalList.SetItemState(-1, 0, LVIS_SELECTED);
        updatePreviewArea();
    }

    void CPredefinedPage::OnOK()
    {
    }

    void CPredefinedPage::OnCancel()
    {
    }

    //TD 15349
    //zhou yuan++
    bool CPredefinedPage::findAndSelectMessageNameInList(CListCtrl& list, const std::string messageName)
    {
        return WindowsUtil::select(list, messageName);
    }

    void CPredefinedPage::clearCurrentSelection(CListCtrl& list)
    {
        list.SetItemState(-1, 0, LVIS_SELECTED);
    }

    bool CPredefinedPage::findAndSelectStationMessage(const std::string& messageName)
    {
        bool ret = false;

        if (findAndSelectMessageNameInList(m_emergencyList, messageName))
        {
            OnClickEmergencyPredefined(0, 0);
            ret = true;
        }
        else if (findAndSelectMessageNameInList(m_normalList, messageName))
        {
            OnClickNormalPredefined(0, 0);
            ret = true;
        }

        return ret;
    }
    //++zhou yuan

    bool CPredefinedPage::isEmergencySelected() const
    {
        return m_emergencyList.GetFirstSelectedItemPosition();
    }
}
