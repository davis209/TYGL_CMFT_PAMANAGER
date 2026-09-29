#include "stdafx.h"
#include "SimpleTreeFilterCtrl.h"
#include "app/signs/common_library/src/STISUtility.h"
#include "core/utility/src/base_ex/LocationAccessFactoryEx.h"
#include "core/utility/src/core/Map.h"
#include "core/utility/src/core/StdEx.h"
#include "core/utility/src/core/CacheDecorator.h"
#include <boost/signals2.hpp>
#include "helperfun.h" // for convertUtf8ToUtf16le

using namespace std::chrono;
using namespace std::literals;
using namespace boost::program_options;
using namespace boost::assign;
using namespace boost::hof;
using namespace boost::adaptors;
using boost::filesystem::path;
using st::make_cached;
using st::CacheDecorator;
using TA_Base_Ex::LocationEx;
using TA_Base_Ex::ThisLocation;
using namespace TA_Base_Core;
using namespace TA_IRS_App;
using STIS_UTILITY::PID;
using STIS_UTILITY::PIDList;

namespace
{
    struct Text
    {
        std::string text;
        bool selected = false;

        static Text& ntext()
        {
            static Text s_text;
            return s_text;
        }
    };
}

BEGIN_MESSAGE_MAP(SimpleTreeFilterCtrl, CTreeCtrl)
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_TIMER()
END_MESSAGE_MAP()

struct SimpleTreeFilterCtrl::Impl
{
    std::vector<std::string> m_selection;
    st::map<HTREEITEM, Text> m_texts;
    std::vector<std::string> m_all;

    bool m_select_all = false;
    HTREEITEM m_select_all_item = nullptr;
    boost::signals2::signal<void(const std::vector<std::string>&)> m_on_selection_change;
};

SimpleTreeFilterCtrl::SimpleTreeFilterCtrl()
    : m_impl(std::make_shared<Impl>())
{
}

void SimpleTreeFilterCtrl::PreSubclassWindow()
{
    ModifyStyle(0, TVS_CHECKBOXES);
}

BOOL SimpleTreeFilterCtrl::ShowWindow(int nCmdShow)
{
    auto res = CTreeCtrl::ShowWindow(nCmdShow);

    if (nCmdShow == SW_SHOW)
    {
        SetTimer(1, 0, nullptr);
    }

    return res;
}

void SimpleTreeFilterCtrl::init(const std::vector<std::string>& texts, bool check)
{
    m_impl->m_all = texts;

    {
        TVINSERTSTRUCTW tvis = {};
        tvis.hParent = TVI_ROOT;
        tvis.hInsertAfter = TVI_FIRST;
        tvis.item.mask = TVIF_TEXT;
        std::wstring allText = L"(All)";
        tvis.item.pszText = const_cast<LPWSTR>(allText.c_str());

        m_impl->m_select_all_item = reinterpret_cast<HTREEITEM>(
            ::SendMessageW(m_hWnd, TVM_INSERTITEMW, 0, reinterpret_cast<LPARAM>(&tvis))
        );
    }

    select(m_impl->m_select_all_item, check);

    for (const auto& text : texts)
    {
        std::wstring wtext = convertUtf8ToUtf16le(text);

        TVINSERTSTRUCTW tvis = {};
        tvis.hParent = TVI_ROOT;
        tvis.hInsertAfter = TVI_LAST;
        tvis.item.mask = TVIF_TEXT;
        tvis.item.pszText = const_cast<LPWSTR>(wtext.c_str());

        HTREEITEM root = reinterpret_cast<HTREEITEM>(
            ::SendMessageW(m_hWnd, TVM_INSERTITEMW, 0, reinterpret_cast<LPARAM>(&tvis))
        );

        m_impl->m_texts.emplace(root, Text{text});
        select(root, check);
    }
}

bool SimpleTreeFilterCtrl::update(const std::vector<std::string>& texts)
{
    if (texts != m_impl->m_all)
    {
        refresh_selection();

        auto select_all = m_impl->m_select_all;
        auto selection = select_all ? texts : m_impl->m_selection;
        m_impl->m_selection.clear();
        m_impl->m_texts.clear();

        DeleteAllItems();

        init(texts, false);

        for (auto& text : selection)
        {
            select(text, true);
        }

        select(m_impl->m_select_all_item, select_all);

        refresh_selection();
        return true;
    }

    return false;
}

void SimpleTreeFilterCtrl::on_selection_change(std::function<void(const std::vector<std::string>&)> f)
{
    m_impl->m_on_selection_change.connect(std::move(f));
}

void SimpleTreeFilterCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
    CTreeCtrl::OnLButtonDown(nFlags, point);

    UINT flags = 0;

    if (auto item = HitTest(point, &flags))
    {
        auto check = GetCheck(item);
        item == m_impl->m_select_all_item
            ? select_all(check)
            : select(item, check)
            ;
        refresh_selection();
    }
}

void SimpleTreeFilterCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    UINT flags = 0;

    if (auto item = HitTest(point, &flags))
    {
        OnLButtonDown(nFlags, point);
    }
}

bool SimpleTreeFilterCtrl::is_selected(HTREEITEM item) const
{
    return m_impl->m_texts.get_value_or(item, Text::ntext()).selected;
}

void SimpleTreeFilterCtrl::select(HTREEITEM item, bool check)
{
    if (item == m_impl->m_select_all_item)
    {
        if (m_impl->m_select_all != check)
        {
            SetCheck(m_impl->m_select_all_item, check);
            m_impl->m_select_all = check;
        }

        return;
    }

    if (item && is_selected(item) != check)
    {
        SetCheck(item, check);
        m_impl->m_texts[item].selected = check;
        select(m_impl->m_select_all_item, m_impl->m_texts.size() ? is_all_selected() : check);
    }
}

void SimpleTreeFilterCtrl::select(const std::string& text, bool check)
{
    select(get_item(text), check);
}

void SimpleTreeFilterCtrl::select_all(bool check)
{
    if (is_selected(m_impl->m_select_all_item) != check)
    {
        m_impl->m_select_all = check;
        SetCheck(m_impl->m_select_all_item, check);
    }

    m_impl->m_texts.for_each_key(check | pipable(lift_select()));
}

bool SimpleTreeFilterCtrl::is_all_selected() const
{
    return m_impl->m_texts.all_of_values(std::mem_fn(&Text::selected));
}

size_t SimpleTreeFilterCtrl::get_selection_count() const
{
    return m_impl->m_texts.count_if_value(std::mem_fn(&Text::selected));
}

void SimpleTreeFilterCtrl::on_selection(std::vector<std::string> texts)
{
    if (texts != m_impl->m_selection)
    {
        m_impl->m_selection = std::move(texts);
        m_impl->m_on_selection_change(m_impl->m_selection);
    }
}

void SimpleTreeFilterCtrl::refresh_selection()
{
    on_selection(get_selection());
}

std::vector<std::string> SimpleTreeFilterCtrl::get_selection() const
{
    std::vector<std::string> texts;
    return m_impl->m_texts.push_back_values_if_value(texts, std::mem_fn(&Text::selected), std::mem_fn(&Text::text));
}

HTREEITEM SimpleTreeFilterCtrl::get_item(const std::string& text) const
{
    return m_impl->m_texts.get_key_if_value_or(proj(&Text::text, _ == text), nullptr);
}

std::string SimpleTreeFilterCtrl::get_text(HTREEITEM item) const
{
    return m_impl->m_texts.get_value_or(item, Text::ntext()).text;
}

std::string SimpleTreeFilterCtrl::get_title() const
{
    if (is_all_selected())
    {
        return "All";
    }

    auto count = get_selection_count();
    return count == 1
        ? get_selection()[0]
        : str(boost::format("%d / %d") % count % m_impl->m_texts.size())
        ;
}

std::string SimpleTreeFilterCtrl::get_tooltip() const
{
    if (is_all_selected())
    {
        return "All";
    }

    auto selection = get_selection();
    return selection.size() == 1
        ? selection[0]
        : st::join(selection, ",")
        ;
}

void SimpleTreeFilterCtrl::OnTimer(UINT nIDEvent)
{
    if (1 == nIDEvent)
    {
        reselect();
        SetFocus();
        KillTimer(nIDEvent);
    }

    CTreeCtrl::OnTimer(nIDEvent);
}

void SimpleTreeFilterCtrl::reselect()
{
    for (auto&& [item, text] : m_impl->m_texts)
    {
        if (text.selected)
        {
            SetCheck(item);
        }
    }

    if (m_impl->m_select_all)
    {
        SetCheck(m_impl->m_select_all_item);
    }
}
