/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/stis_manager/src/TemplatePage.cpp $
 * @author:  Robin Ashcroft
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 *
 * The Template message selection tab
 */

#include "stdafx.h"
#include "stismanager.h"
#include "TemplatePage.h"
#include "STISTemplates.h"
#include "DisplayPageInfo.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"

#include "core/data_access_interface/entity_access/src/IConsole.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/entity_access/src/TISAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/STISEntityData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::TemplatePtr;

namespace TA_IRS_App
{
    CTemplatePage::CTemplatePage(CWnd* pParent /*=NULL*/)
        : CDialog(CTemplatePage::IDD, pParent),
        m_messageSelectionListener(NULL)
    {
        //{{AFX_DATA_INIT(CTemplatePage)
        //}}AFX_DATA_INIT
    }

    CTemplatePage::~CTemplatePage()
    {
        // no longer accepting updates
        //m_stisTemplates->deregisterCurrentVersionUser(this);
    }

    void CTemplatePage::DoDataExchange(CDataExchange* pDX)
    {
        CDialog::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(CTemplatePage)
        DDX_Control(pDX, IDC_LCD_TEMPLATE, m_lcdList);
        DDX_Control(pDX, IDC_LED_TEMPLATE, m_ledList);
        DDX_Control(pDX, IDC_SEARCH_LCD, m_searchLcd);
        DDX_Control(pDX, IDC_SEARCH_LED, m_searchLed);
        //}}AFX_DATA_MAP
    }

    BEGIN_MESSAGE_MAP(CTemplatePage, CDialog)
        //{{AFX_MSG_MAP(CTemplatePage)
        ON_WM_SHOWWINDOW()
        ON_WM_DESTROY()
        ON_EN_UPDATE(IDC_SEARCH_LCD, OnUpdateSearchLcd)
        ON_EN_UPDATE(IDC_SEARCH_LED, OnUpdateSearchLed)
        ON_NOTIFY(NM_CLICK, IDC_LCD_TEMPLATE, OnClickLcdTemplate)
        ON_NOTIFY(NM_CLICK, IDC_LED_TEMPLATE, OnClickLedTemplate)
        ON_NOTIFY(LVN_ITEMCHANGED, IDC_LCD_TEMPLATE, onItemchangedLcdTemplate)
        ON_NOTIFY(LVN_ITEMCHANGED, IDC_LED_TEMPLATE, onItemchangedLedTemplate)
        ON_MESSAGE(WM_UPDATE_CURRENT_STIS_TEMPLATE_VERSION, OnUpdateCurrentSTISVersion)
        ON_BN_CLICKED(IDC_RADIO_NORMAL_TEMPLATE, &CTemplatePage::OnBnClickedRadioNormalTemplate)
        ON_BN_CLICKED(IDC_RADIO_EMERGENCY_TEMPLATE, &CTemplatePage::OnBnClickedRadioEmergencyTemplate)
        ON_BN_CLICKED(IDC_LCD_DEFAULT_SETTING, &CTemplatePage::onBnClickedLCDDefaultSetting)
        ON_BN_CLICKED(IDC_LED_DEFAULT_SETTING, &CTemplatePage::onBnClickedLEDDefaultSetting)
        //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // CTemplatePage message handlers

    BOOL CTemplatePage::OnInitDialog()
    {
        CDialog::OnInitDialog();
        return TRUE;  // return TRUE unless you set the focus to a control
        // EXCEPTION: OCX Property Pages should return FALSE
    }

    void CTemplatePage::init()
    {
        m_stisTemplates = STISTemplates::getInstance();
        // extended styles (including infotips)
        // column widths for message titles
        m_lcdList.SetExtendedStyle(m_lcdList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);
        m_ledList.SetExtendedStyle(m_ledList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);

        RECT rect;
        m_lcdList.GetClientRect(&rect);
        m_lcdList.InsertColumn(0, "Message Name");
        m_lcdList.SetColumnWidth(0, rect.right);
        m_ledList.GetClientRect(&rect);
        m_ledList.InsertColumn(0, "Message Name");
        m_ledList.SetColumnWidth(0, rect.right);

        // load the pre-defined messages into the lists
        populateLists();

        // register with the pre-defined message singleton to be updated of a change
        m_stisTemplates->registerCurrentVersionUser(this);

        m_lcdList.setScrollBarVisibility(true);
        m_ledList.setScrollBarVisibility(true);

        CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_NORMAL_TEMPLATE);
        pButton->SetCheck(true);
    }

    void CTemplatePage::OnDestroy()
    {
        CDialog::OnDestroy();
    }

    void CTemplatePage::windowShown()
    {
        if (m_messageSelectionListener)
        {
            m_validLcdTemplate = hasValidLcdSelection();
            m_validLedTemplate = hasValidLedSelection();
            m_messageSelectionListener->templateSelected(true, m_validLcdTemplate, m_validLedTemplate, getLcdTemplate(), getLedTemplate());
        }
    }

    void CTemplatePage::setMessageSelectionListener(IMessageSelectionListener* messageSelectionListener)
    {
        m_messageSelectionListener = messageSelectionListener;
    }

    LRESULT CTemplatePage::OnUpdateCurrentSTISVersion(WPARAM wParam, LPARAM lParam)
    {
        // rebuild the lists
        populateLists();

        // update all the stuff to do with selected items in lists
        //updatePreviewArea();

        return 0;
    }

    bool CTemplatePage::hasValidLcdSelection() const
    {
        FUNCTION_ENTRY("CTemplatePage::hasValidLcdSelection");
        FUNCTION_EXIT;
        return getSelectedLcdTemplateData();
    }

    bool CTemplatePage::hasValidLedSelection() const
    {
        LOG_FUNCTION_ENTRY(SourceInfo, "CTemplatePage::hasValidLedSelection");
        FUNCTION_EXIT;
        return getSelectedLedTemplateData();
    }

    TemplatePtr CTemplatePage::getLcdTemplate() const
    {
        LOG_FUNCTION_ENTRY(SourceInfo, "CTemplatePage::getLcdTemplate");

        if (auto p = getSelectedLcdTemplateData())
        {
            FUNCTION_EXIT;
            return std::make_shared<Template>(*p);
        }

        FUNCTION_EXIT;
        return {};
    }

    TemplatePtr CTemplatePage::getNormalLcdTemplate() const
    {
        if (isNormalChecked())
        {
            return getLcdTemplate();
        }

        return {};
    }

    TemplatePtr CTemplatePage::getEmergencyLcdTemplate() const
    {
        if (isEmergencyChecked())
        {
            return getLcdTemplate();
        }

        return {};
    }

    TemplatePtr CTemplatePage::getLedTemplate() const
    {
        LOG_FUNCTION_ENTRY(SourceInfo, "CTemplatePage::getLedTemplate");

        if (auto p = getSelectedLedTemplateData())
        {
            FUNCTION_EXIT;
            return std::make_shared<Template>(*p);
        }

        FUNCTION_EXIT;
        return {};
    }

    TemplatePtr CTemplatePage::getNormalLedTemplate() const
    {
        if (isNormalChecked())
        {
            return getLedTemplate();
        }

        return {};
    }

    TemplatePtr CTemplatePage::getEmergencyLedTemplate() const
    {
        if (isEmergencyChecked())
        {
            return getLedTemplate();
        }

        return {};
    }

    void CTemplatePage::populateLists()
    {
        LOG_FUNCTION_ENTRY(SourceInfo, "Template::populateLists");

        // clear the list of pre-defined messages
        m_lcdList.DeleteAllItems();
        m_ledList.DeleteAllItems();

        std::vector<Template> lcdTemplates;
        std::vector<Template> ledTemplates;

        if (m_selectedRadioButton == 0)
        {
            lcdTemplates = m_stisTemplates->getNormalLcdTemplates();
            ledTemplates = m_stisTemplates->getNormalLedTemplates();
        }
        else
        {
            lcdTemplates = m_stisTemplates->getEmergencyLcdTemplates();
            ledTemplates = m_stisTemplates->getEmergencyLedTemplates();
        }

        int count = 0;

        for (auto it = lcdTemplates.begin(); it != lcdTemplates.end(); it++)
        {
            // add the string to the list
            int index = m_lcdList.InsertItem(count, it->description.c_str());
            m_lcdList.SetItemData(index, static_cast<DWORD>(it->templateID));

            if (it->templateID == 0)
            {
                m_default_lcd_index = index;
            }

            if (m_selectedRadioButton == 1)
            {
                m_lcdList.setItemColour(count, COLORREF(RGB(255, 0, 0)), ListCtrlSelNoFocus::I_INDEX);
            }
            else
            {
                m_lcdList.setItemColour(count, COLORREF(RGB(0, 0, 0)), ListCtrlSelNoFocus::I_INDEX);
            }

            count++;
        }

        count = 0;

        for (auto it = ledTemplates.begin(); it != ledTemplates.end(); it++)
        {
            // add the string to the list
            int index = m_ledList.InsertItem(count, it->description.c_str());
            m_ledList.SetItemData(index, static_cast<DWORD>(it->templateID));

            if (it->templateID == 0)
            {
                m_default_led_index = index;
            }

            // set the emergency colour to red
            if (m_selectedRadioButton == 1)
            {
                m_ledList.setItemColour(count, COLORREF(RGB(255, 0, 0)), ListCtrlSelNoFocus::I_INDEX);
            }
            else
            {
                m_ledList.setItemColour(count, COLORREF(RGB(0, 0, 0)), ListCtrlSelNoFocus::I_INDEX);
            }

            count++;
        }

        if (m_messageSelectionListener != NULL)
        {
            m_validLcdTemplate = hasValidLcdSelection();
            m_validLedTemplate = hasValidLedSelection();
            m_messageSelectionListener->templateSelected(true, m_validLcdTemplate, m_validLedTemplate, getLcdTemplate(), getLedTemplate());
        }

        LOG_FUNCTION_EXIT(SourceInfo, "Template::populateLists");
    }

    const Template* CTemplatePage::getSelectedLcdTemplateData() const
    {
        LOG_FUNCTION_ENTRY(SourceInfo, "CTemplatePage::getSelectedLcdMessageData");
        const Template* res = NULL;

        // get the selected index for the normal priority list
        int lcdSelected = -1;
        POSITION currSel = m_lcdList.GetFirstSelectedItemPosition();

        // if something is selected
        if (currSel != NULL)
        {
            // get the selected index
            lcdSelected = m_lcdList.GetNextSelectedItem(currSel);
        }

        // if a normal message is selected
        if (lcdSelected > -1)
        {
            // get the data for the selected item
            unsigned short itemData = static_cast<unsigned short>(m_lcdList.GetItemData(lcdSelected));

            // find the appropriate template
            res = m_selectedRadioButton == 0
                ? m_stisTemplates->getNormalLcdTemplateById(itemData)
                : m_stisTemplates->getEmergencyLcdTemplateById(itemData)
                ;

          // anything selected should point to a valid message
            TA_ASSERT(res != NULL, "The selected lcd template is not a valid template");
        }

        // otherwise, return NULL

        LOG_FUNCTION_EXIT(SourceInfo, "CTemplatePage::getSelectedLcdMessageData");
        return res;
    }

    const Template* CTemplatePage::getSelectedLedTemplateData() const
    {
        LOG_FUNCTION_ENTRY(SourceInfo, "CTemplatePage::getSelectedLedMessageData");
        const Template* res = NULL;

        // get the index for the emergency priority list
        int ledSelected = -1;
        POSITION currSel = m_ledList.GetFirstSelectedItemPosition();

        // if something is selected
        if (currSel != NULL)
        {
            // get the selected index
            ledSelected = m_ledList.GetNextSelectedItem(currSel);
        }

        if (ledSelected > -1)
        {
            // get the text for the selected item
            unsigned short itemData = static_cast<unsigned short>(m_ledList.GetItemData(ledSelected));

            // find the appropriate template
            res = m_selectedRadioButton == 0
                ? m_stisTemplates->getNormalLedTemplateById(itemData)
                : m_stisTemplates->getEmergencyLedTemplateById(itemData)
                ;

          // anything selected should point to a valid message
            TA_ASSERT(res != NULL, "The selected led template is not a valid template");
        }

        // otherwise, return NULL

        LOG_FUNCTION_EXIT(SourceInfo, "CTemplatePage::getSelectedLedMessageData");
        return res;
    }

    void CTemplatePage::onItemchangedLcdTemplate(NMHDR* pNMHDR, LRESULT* pResult)
    {
        NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

        // Only interested in state changes
        if (pNMListView->uChanged == LVIF_STATE)
        {
            // if it is being selected
            if (((pNMListView->uNewState & LVIS_SELECTED) == LVIS_SELECTED) &&
                ((pNMListView->uOldState & LVIS_SELECTED) == 0))
            {
#if 0
                // remove_pids led messages
                m_ledList.SetItemState(-1, 0, LVIS_SELECTED);
#endif

                if (m_messageSelectionListener)
                {
                    m_validLcdTemplate = hasValidLcdSelection();
                    m_messageSelectionListener->templateSelected(true, m_validLcdTemplate, m_validLedTemplate, getLcdTemplate(), getLedTemplate());
                }
            }
            else
            {
                m_validLcdTemplate = false;
                m_messageSelectionListener->templateSelected(true, m_validLcdTemplate, m_validLedTemplate, {}, getLedTemplate());
            }
        }

        *pResult = 0;
    }

    void CTemplatePage::OnClickLcdTemplate(NMHDR* pNMHDR, LRESULT* pResult)
    {
#if 0
        // remove_pids led template
        m_ledList.SetItemState(-1, 0, LVIS_SELECTED);
#endif

        if (pResult != NULL)
        {
            *pResult = 0;
        }
    }

    void CTemplatePage::onItemchangedLedTemplate(NMHDR* pNMHDR, LRESULT* pResult)
    {
        NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

        // Only interested in state changes
        if (pNMListView->uChanged == LVIF_STATE)
        {
            // if it is being selected
            if (((pNMListView->uNewState & LVIS_SELECTED) == LVIS_SELECTED) &&
                ((pNMListView->uOldState & LVIS_SELECTED) == 0))
            {
#if 0
                // Deselect the lcd
                m_lcdList.SetItemState(-1, 0, LVIS_SELECTED);
#endif

                // update the display
                //updatePreviewArea();

                if (m_messageSelectionListener)
                {
                    m_validLedTemplate = hasValidLedSelection();
                    m_messageSelectionListener->templateSelected(true, m_validLcdTemplate, m_validLedTemplate, getLcdTemplate(), getLedTemplate());
                }
            }
            else
            {
                m_validLedTemplate = false;
                m_messageSelectionListener->templateSelected(true, m_validLcdTemplate, m_validLedTemplate, getLcdTemplate(), {});
            }
        }

        *pResult = 0;
    }

    void CTemplatePage::OnClickLedTemplate(NMHDR* pNMHDR, LRESULT* pResult)
    {
#if 0
        // Deselect the lcd listbox
        m_lcdList.SetItemState(-1, 0, LVIS_SELECTED);
#endif

        if (pResult != NULL)
        {
            *pResult = 0;
        }
    }

    void CTemplatePage::OnUpdateSearchLcd()
    {
        // get the text to search for from the text box.
        m_searchLcd.GetWindowText(m_lcdSearchText);
        m_lcdSearchText.MakeLower();

        // rebuild the list based on the search text
        m_lcdList.DeleteAllItems();

        std::vector<TA_Base_Core::Template> templates = m_selectedRadioButton == 0 ?
            m_stisTemplates->getNormalLcdTemplates() : m_stisTemplates->getEmergencyLcdTemplates();

        int count = 0;
        m_default_lcd_index = -1;

        for (auto it = templates.begin(); it != templates.end(); it++)
        {
            CString messageString(it->description.c_str());
            messageString.MakeLower();

            // if the string matches
            if (messageString.Find(m_lcdSearchText) != -1)
            {
                // add the string to the list
                int index = m_lcdList.InsertItem(count, it->description.c_str());
                m_lcdList.SetItemData(index, static_cast<DWORD>(it->templateID));

                if (it->templateID == 0)
                {
                    m_default_lcd_index = index;
                }

                count++;
            }
        }

        // remove_pids item and set window text
        m_ledList.SetItemState(-1, 0, LVIS_SELECTED);
        m_lcdList.SetItemState(-1, 0, LVIS_SELECTED);
        //updatePreviewArea();
    }

    void CTemplatePage::OnUpdateSearchLed()
    {
        m_searchLed.GetWindowText(m_ledSearchText);
        m_ledSearchText.MakeLower();

        // rebuild the list based on the string
        m_ledList.DeleteAllItems();

        auto templates = m_selectedRadioButton == 0 ? m_stisTemplates->getNormalLedTemplates() : m_stisTemplates->getEmergencyLedTemplates();

        int count = 0;
        m_default_led_index = -1;

        for (auto it = templates.begin(); it != templates.end(); it++)
        {
            CString messageString(it->description.c_str());
            messageString.MakeLower();

            // if the string matches
            if (messageString.Find(m_ledSearchText) != -1)
            {
                // add the string to the list
                int index = m_ledList.InsertItem(count, it->description.c_str());
                m_ledList.SetItemData(index, static_cast<DWORD>(it->templateID));

                if (it->templateID == 0)
                {
                    m_default_led_index = index;
                }
#if 0
                // set the emergency colour to red
                m_ledList.setItemColour(count,
                                        COLORREF(RGB(255, 0, 0)),
                                        ListCtrlSelNoFocus::I_INDEX);
#endif
                count++;
            }
        }

        // remove_pids item and set window text
        m_ledList.SetItemState(-1, 0, LVIS_SELECTED);
        m_lcdList.SetItemState(-1, 0, LVIS_SELECTED);
        //updatePreviewArea();
    }

    void CTemplatePage::OnOK()
    {
    }

    void CTemplatePage::OnCancel()
    {
    }

    //TD 15349
    //zhou yuan++
    bool CTemplatePage::findAndSelectMessageNameInList(CListCtrl& list, const std::string messageName)
    {
        bool ret = false;

        int num = list.GetItemCount();

        for (int i = 0; i < num; ++i)
        {
            CString msgContent = list.GetItemText(i, 0);

            if (stricmp(msgContent, messageName.c_str()) == 0)
            {
                clearCurrentSelection(list);
                list.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
                ret = true;
                break;
            }
        }

        return ret;
    }

    void CTemplatePage::clearCurrentSelection(CListCtrl& list)
    {
        list.SetItemState(-1, 0, LVIS_SELECTED);
    }

    bool CTemplatePage::findAndSelectStationMessage(const std::string& messageName)
    {
        bool ret = false;

        if (findAndSelectMessageNameInList(m_lcdList, messageName))
        {
            OnClickLedTemplate(0, 0);
            ret = true;
        }
        else if (findAndSelectMessageNameInList(m_ledList, messageName))
        {
            OnClickLcdTemplate(0, 0);
            ret = true;
        }

        return ret;
    }

    void CTemplatePage::OnBnClickedRadioNormalTemplate()
    {
        CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_NORMAL_TEMPLATE);
        int newState = pButton->GetCheck();

        if (newState == BST_CHECKED && m_selectedRadioButton == 1)
        {
            m_selectedRadioButton = 0;
            populateLists();
        }

        onUpdateSearch();
    }

    void CTemplatePage::OnBnClickedRadioEmergencyTemplate()
    {
        CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_EMERGENCY_TEMPLATE);
        int newState = pButton->GetCheck();

        if (newState == BST_CHECKED && m_selectedRadioButton == 0)
        {
            m_selectedRadioButton = 1;
            populateLists();
        }

        onUpdateSearch();
    }

    void CTemplatePage::onUpdateSearch()
    {
        if (m_lcdSearchText.GetLength())
        {
            OnUpdateSearchLcd();
        }

        if (m_ledSearchText.GetLength())
        {
            OnUpdateSearchLed();
        }
    }

    void CTemplatePage::onBnClickedLCDDefaultSetting()
    {
        m_lcdList.SetItemState(-1, 0, LVIS_SELECTED);
    }

    void CTemplatePage::onBnClickedLEDDefaultSetting()
    {
        m_ledList.SetItemState(-1, 0, LVIS_SELECTED);
    }

    bool CTemplatePage::isNormalChecked() const
    {
        return m_selectedRadioButton == 0;
    }

    bool CTemplatePage::isEmergencyChecked() const
    {
        return m_selectedRadioButton == 1;
    }

    bool CTemplatePage::isLcdSelected() const
    {
        return m_lcdList.GetSelectedCount();
    }

    bool CTemplatePage::isLedSelected() const
    {
        return m_ledList.GetSelectedCount();
    }

    bool CTemplatePage::isLcdOrLedSelected() const
    {
        return isLcdSelected() || isLedSelected();
    }

    bool CTemplatePage::isNormalLcdOrLedSelected() const
    {
        return isLcdOrLedSelected() && isNormalChecked();
    }

    bool CTemplatePage::isEmergencyLcdOrLedSelected() const
    {
        return isLcdOrLedSelected() && isEmergencyChecked();
    }

    bool CTemplatePage::isNormalLcdSelected() const
    {
        return isLcdSelected() && isNormalChecked();
    }

    bool CTemplatePage::isNormalLedSelected() const
    {
        return isLedSelected() && isNormalChecked();
    }

    bool CTemplatePage::isEmergencyLcdSelected() const
    {
        return isLcdSelected() && isEmergencyChecked();
    }

    bool CTemplatePage::isEmergencyLedSelected() const
    {
        return isLedSelected() && isEmergencyChecked();
    }
}
