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
 * The free text message selection page
 *
 */

#include "stdafx.h"
#include "helperfun.h"
#include "stismanager.h"
#include "FreeTextPage.h"
#include "UserMessages.h"
#include "STISPredefinedMessages.h"
#include "WindowsUtil.h"
#include "app/signs/common_library/src/STISAuditMessage.h"
#include "app/signs/common_library/src/stis_protocol/STISClient.h"
#include "bus/application_types/src/apptypes.h"
#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/UserSettingsException.h"
#include "core/utilities/src/RunParams.h"
#include "core/message/src/NameValuePair.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/TISAudit_MessageTypes.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/utilities/src/CodeConverter.h"
#include "core/utility/src/core/CacheDecorator.h"
#include "core/utility/src/core/MakeNameValuePairsString.h"
#include "core/utility/src/core/algorithm/strings.h"
#include "core/utility/src/core/StdEx.h"
// #include "cots/ssce/sdk/include/sscemfc.hpp"
#include <boost/filesystem.hpp>
#include <iomanip>
#include <boost/format.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using st::make_cached;
using namespace TA_Base_Core;
using namespace TA_Base_Bus;
using namespace TA_IRS_App;
using TA_IRS_App::STIS_PROTOCOL::STISClient;

namespace
{
    const size_t MAX_AD_HOC_MESSAGE_SIZE = 30;
    const char* FREE_TEXT_PREFIX = "Ad Hoc Message ";
    const char* TAG_FREE_TEXT = "FreeText";
    const char* DELIMITER = "0xffff";

    const std::string& make_default_title(int key)
    {
        static auto s_func = st::make_cached([](int key)
        {
            return str(boost::format("%s%02d") % FREE_TEXT_PREFIX % key);
        });

        return s_func(key);
    }

    bool is_supported_character(wchar_t ch)
    {
        static std::wstring s_supported = L" !#$%&'()*+,-./0123456789:;<=>?ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        return s_supported.find(ch) != std::wstring::npos;
    }

    const std::wstring s_supported_characters =
        L"Space character\n"
        L"!#$%&'()*+,-./\n"
        L":;<=>?\n"
        L"0-9 A-Z a-z"
        ;
}

namespace TA_IRS_App
{
    struct CFreeTextPage::EditingMessage
    {
        auto title_content()
        {
            return std::tie(title, content);
        }

        auto title_content() const
        {
            return std::tie(title, content);
        }

        auto new_title_content()
        {
            return std::tie(new_title, new_content);
        }

        auto new_title_content() const
        {
            return std::tie(new_title, new_content);
        }

        auto default_title_content() const
        {
            return std::tie(default_title(), default_content());
        }

        bool is_editing() const
        {
            return key != 0;
        }

        void clear()
        {
            key = 0;
            title.clear();
            content.clear();
            new_title.clear();
            new_content.clear();
        }

        const std::string& default_title() const
        {
            return make_default_title(key);
        }

        const std::string& default_content() const
        {
            static std::string s_empty;
            return s_empty;
        }

        bool changed() const
        {
            return title_content() != new_title_content();
        }

        int key = 0;
        std::string title;
        std::string content;
        std::string new_title;
        std::string new_content;
    };

    CFreeTextPage::CFreeTextPage(CWnd* pParent /*=NULL*/)
        : CDialog(CFreeTextPage::IDD, pParent),
        m_title(std::make_shared<SimpleUnicodeEdit>(this, IDC_MESSAGE_TITLE)),
        m_content(std::make_shared<SimpleUnicodeEdit>(this, IDC_MESSAGE_CONTENT)),
        m_edit(std::make_shared<EditingMessage>())
        //m_userSettingsManager( TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID), STIS_MANAGER_GUI_APPTYPE)
    {
        //{{AFX_DATA_INIT(CFreeTextPage)
        // NOTE: the ClassWizard will add member initialization here
        //}}AFX_DATA_INIT

        // TODO
    }

    CFreeTextPage::~CFreeTextPage()
    {
    }

    void CFreeTextPage::DoDataExchange(CDataExchange* pDX)
    {
        CDialog::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(CFreeTextPage)
        DDX_Control(pDX, IDC_FREETEXT_LIST, m_freeTextList);
        // DDX_Control(pDX, IDC_MESSAGE_TITLE, m_title);
        // DDX_Control(pDX, IDC_MESSAGE_CONTENT, m_content);
        //}}AFX_DATA_MAP
    }

    BEGIN_MESSAGE_MAP(CFreeTextPage, CDialog)
        //{{AFX_MSG_MAP(CFreeTextPage)
        ON_BN_CLICKED(IDC_EDIT, OnEdit)
        ON_EN_CHANGE(IDC_STATION_FREE_TEXT, onChangeFreeText)
        //ON_MESSAGE(WM_UPDATE_CURRENT_STIS_VERSION, OnUpdateCurrentSTISVersion)
        ON_WM_DESTROY()
        ON_BN_CLICKED(IDC_SAVE, OnSave)
        ON_NOTIFY(LVN_ITEMCHANGED, IDC_FREETEXT_LIST, OnSelchangeFreetextList)
        ON_WM_TIMER()
        //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // CFreeTextPage message handlers

    BOOL CFreeTextPage::OnInitDialog()
    {
        CDialog::OnInitDialog();

        m_title->init();
        m_title->set_readonly(TRUE);
        m_content->init();
        m_content->set_readonly(TRUE);

        enable_title_content(FALSE);
        GetDlgItem(IDC_EDIT)->EnableWindow(FALSE);
        GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);

        RECT r;
        m_freeTextList.GetClientRect(&r);
        m_freeTextList.InsertColumn(0, "Free Text Title");
        m_freeTextList.SetColumnWidth(0, r.right - 5);
        m_freeTextList.SetFont(CFont::FromHandle(WindowsUtil::create_font(this)));
        m_freeTextList.SetExtendedStyle(m_freeTextList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);
        m_freeTextList.setScrollBarVisibility(true);

        return TRUE;  // return TRUE unless you set the focus to a control
        // EXCEPTION: OCX Property Pages should return FALSE
    }

    void CFreeTextPage::init()
    {
        m_freeTextList.SendMessage(WM_SETREDRAW, FALSE, 0);
        BOOST_SCOPE_EXIT_ALL(&) { m_freeTextList.SendMessage(WM_SETREDRAW, TRUE, 0); };

        m_pidMessages = build_messages_from_sqlite();

        for (size_t i = 0; i < MAX_AD_HOC_MESSAGE_SIZE; ++i)
        {
            m_freeTextList.InsertItem(i, CodeConverter::UTF8ToANSI(m_pidMessages[i].first).c_str());
        }

        m_title->set_limit_text(MAX_FREE_TEXT_TITLE_CHARACTERS);
        m_content->set_limit_text(MAX_FREE_TEXT_CONTENT_CHARACTERS);

        SetTimer(1, 500, nullptr);
    }

    std::vector<std::pair<std::string, std::string>>& CFreeTextPage::build_messages_from_sqlite()
    {
        static std::vector<std::pair<std::string, std::string>> s_messages;
        auto items = STISClient::instance().load_ad_hoc_message_library();
        static decltype(items) s_items;

        if (items && s_items && *s_items == *items)
        {
            return s_messages;
        }

        s_messages.clear();
        s_items = items;
        decltype(items->begin()) it;

        for (int i = 0; i < MAX_AD_HOC_MESSAGE_SIZE; i++)
        {
            std::string title;
            std::string content;

            if (items && (it = items->find(i + 1)) != items->end())
            {
                std::tie(std::ignore, title, content) = it->second;
            }
            else
            {
                title = make_default_title(i + 1);
            }

            s_messages.emplace_back(std::move(title), std::move(content));
        }

        return s_messages;
    }

    void CFreeTextPage::OnDestroy()
    {
        CDialog::OnDestroy();
    }

    FreeTextMessage CFreeTextPage::getMessage() const
    {
        return {get_title(), get_content()};
    }

    void CFreeTextPage::windowShown()
    {
        onChangeFreeTextImpl(true);
    }

    void CFreeTextPage::onChangeFreeText()
    {
        onChangeFreeTextImpl(false);
    }

    void CFreeTextPage::onChangeFreeTextImpl(bool tabChanged)
    {
        if (!m_messageSelectionListener)
        {
            return;
        }

        auto&& [title, content] = get_title_content();
        // if there is some text there is a valid message
        m_validMessage = title.size() && content.size();
        m_messageSelectionListener->adHocMessageSelected(tabChanged, WindowsUtil::has_selection(m_freeTextList), m_validMessage, title);
    }

    void CFreeTextPage::setMessageSelectionListener(IMessageSelectionListener* messageSelectionListener)
    {
        m_messageSelectionListener = messageSelectionListener;
    }

    void CFreeTextPage::OnOK()
    {
    }

    void CFreeTextPage::OnCancel()
    {
        if (m_edit->is_editing())
        {
            STISClient::instance().unlock_ad_hoc_message(m_edit->key);
            m_edit->clear();
        }
    }

    void CFreeTextPage::OnEdit()
    {
        auto key = WindowsUtil::get_selection(m_freeTextList) + 1;

        if (auto res = STISClient::instance().lock_ad_hoc_message(key); !res.second)
        {
            UserMessages::getInstance().displayInfo(str(boost::format("%s is editing") % res.first));
            return;
        }

        // TODO: save the origin info, then check whether is changed when save

        m_edit->key = key;
        m_edit->title_content() = get_title_content();
        enable_title_content(TRUE);
        GetDlgItem(IDC_EDIT)->EnableWindow(FALSE);
        GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
        m_messageSelectionListener->adHocMessageSelected(false, false, false, "");
    }

    void CFreeTextPage::OnSave()
    {
        // save the text in the message box to
        // the selected free text item

        // get the selected index
        int currSel = WindowsUtil::get_selection(m_freeTextList);
        m_edit->new_title_content() = get_title_content();

        if (m_edit->new_title.empty() && m_edit->new_content.size())
        {
            TransActiveMessage::show(IDS_UE_070124);
            return;
        }

        {
            auto content = st2::utf8_to_wstring(m_edit->new_content);
            boost::remove_erase_if(content, is_supported_character);

            if (content.size())
            {
#if 0
                UserMessages().displayError(L"Unsupported characters: \n" + content);
#else
                UserMessages().displayError(L"Supported characters: \n" + s_supported_characters);
#endif
                return;
            }
        }

        BOOST_SCOPE_EXIT_ALL(&) { m_edit->clear(); };

        try
        {
            if ((m_edit->new_title.empty() && m_edit->new_content.empty()) || (m_edit->new_title_content() == m_edit->default_title_content()))
            {
                STISClient::instance().delete_ad_hoc_message(m_edit->key);
                m_edit->new_title_content() = m_edit->default_title_content();
                m_title->set_window_text_utf8(m_edit->default_title());
            }
            else
            {
                if (m_edit->changed())
                {
                    STISClient::instance().set_ad_hoc_message(m_edit->key, m_edit->new_title, m_edit->new_content);
                }
                else
                {
                    STISClient::instance().unlock_ad_hoc_message(m_edit->key);
                }
            }

            m_pidMessages[currSel].first = m_edit->new_title;
            m_pidMessages[currSel].second = m_edit->new_content;

            STISAuditMessage::saveAdhocMessage(m_edit->new_title, m_edit->new_content);

            // NOTE: will trigger message: LVN_ITEMCHANGED, then m_edit will be cleared
            m_freeTextList.SetItemText(currSel, 0, CodeConverter::UTF8ToANSI(m_edit->new_title).c_str());

            enable_title_content(FALSE);
            GetDlgItem(IDC_SAVE)->EnableWindow(false);
            GetDlgItem(IDC_EDIT)->EnableWindow(true);

            onChangeFreeText();
        }
        catch (const TA_Base_Core::UserSettingsException& use)
        {
            LOG_EXCEPTION("UserSettingsException", use.what());
            TransActiveMessage::show(IDS_UE_070124); // TD14164 ++
        }
        catch (...)
        {
            LOG_EXCEPTION("UnknownException", "Unknown");
        }
    }

    void CFreeTextPage::OnSelchangeFreetextList(NMHDR* pNMHDR, LRESULT* pResult)
    {
        enable_title_content(FALSE);

        if (auto i = WindowsUtil::get_selection(m_freeTextList); i != -1)
        {
            m_title->set_window_text_utf8(m_pidMessages[i].first);
            m_content->set_window_text_utf8(m_pidMessages[i].second);
            GetDlgItem(IDC_EDIT)->EnableWindow(TRUE);
            GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);
        }
        else
        {
            m_title->set_window_text_utf8("");
            m_content->set_window_text_utf8("");
            GetDlgItem(IDC_EDIT)->EnableWindow(FALSE);
            GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);
        }

        onChangeFreeText();
        OnCancel();

        *pResult = 0;
    }

    //TD 15349
    //zhou yuan++
    bool CFreeTextPage::findAndSelectMessageNameInList(CListCtrl& list, const std::string messageName)
    {
        return WindowsUtil::select(list, messageName);
    }

    bool CFreeTextPage::findAndSelectStationMessage(const std::string& messageName)
    {
        if (findAndSelectMessageNameInList(m_freeTextList, messageName))
        {
            //may need not to call the function, because the SetItemState may trigger this function
            OnSelchangeFreetextList(0, 0);
            return true;
        }

        return false;
    }
    //++zhou yuan

    std::string CFreeTextPage::get_title() const
    {
        return m_title->get_window_text_utf8();
    }

    std::string CFreeTextPage::get_content() const
    {
        return m_content->get_window_text_utf8();
    }

    std::pair<std::string, std::string> CFreeTextPage::get_title_content() const
    {
        return {get_title(), get_content()};
    }

    bool CFreeTextPage::hasValidSelection()
    {
        return get_title().size() && get_content().size();
    }

    void CFreeTextPage::enable_title(bool b)
    {
        m_title->set_readonly(!b);
    }

    void CFreeTextPage::enable_content(bool b)
    {
        m_content->set_readonly(!b);
    }

    void CFreeTextPage::enable_title_content(bool b)
    {
        enable_title(b);
        enable_content(b);
    }

    void CFreeTextPage::OnTimer(UINT nIDEvent)
    {
        if (m_edit->is_editing())
        {
            CDialog::OnTimer(nIDEvent);
            return;
        }

        bool changed = false;
        auto cur_sel = WindowsUtil::get_selection(m_freeTextList);
        m_freeTextList.SendMessage(WM_SETREDRAW, FALSE, 0);
        BOOST_SCOPE_EXIT_ALL(&)
        {
            if (changed)
            {
                onChangeFreeText();
            }

            m_freeTextList.SendMessage(WM_SETREDRAW, TRUE, 0);
            CDialog::OnTimer(nIDEvent);
        };

        static std::vector<std::pair<std::string, std::string>> s_messages;

        if (auto messages = build_messages_from_sqlite(); s_messages != messages)
        {
            s_messages = messages;

            for (size_t i = 0; i < MAX_AD_HOC_MESSAGE_SIZE; ++i)
            {
                auto& [title, content] = m_pidMessages[i];
                auto&& [new_title, new_content] = s_messages[i];

                if (title != new_title || content != new_content)
                {
                    // NOTE:
                    // must call m_freeTextList.SetItemText first, or title or content can not be changed
                    if (title != new_title)
                    {
                        m_freeTextList.SetItemText(i, 0, CodeConverter::UTF8ToANSI(new_title).c_str());

                        if (cur_sel == i)
                        {
                            m_title->set_window_text_utf8(new_title);
                        }
                    }

                    if (content != new_content)
                    {
                        if (cur_sel == i)
                        {
                            m_content->set_window_text_utf8(new_content);
                        }
                    }

                    changed = true;
                    m_pidMessages[i] = s_messages[i];
                }
            }
        }
    }
}
