#include "stdafx.h"
#include "PidTreeCtrl.h"
#include "app/signs/common_library/src/STISUtility.h"
#include "core/utility/src/base_ex/LocationAccessFactoryEx.h"
#include "core/utility/src/core/Map.h"
#include "core/utility/src/core/StdEx.h"
#include "core/utility/src/core/CacheDecorator.h"
#include "core/utilities/src/CodeConverter.h"
#include <boost/signals2.hpp>
#include <vector>

using namespace std::chrono;
using namespace boost::program_options;
using namespace boost::assign;
using namespace boost::hof;
using boost::filesystem::path;
using st::make_cached;
using st::CacheDecorator;
using TA_Base_Ex::Location;
using TA_Base_Ex::ThisLocation;
using namespace TA_Base_Core;
using namespace TA_IRS_App;
using STIS_UTILITY::PID;
using STIS_UTILITY::PIDList;

namespace
{
    struct PidEx
    {
        PID pid;
        bool selected = false;

        static PidEx& npid()
        {
            static PidEx s_pidex;
            return s_pidex;
        };
    };

    using GetItemsCache = CacheDecorator<std::vector<HTREEITEM>, HTREEITEM>;
    using GetItemsCachePtr = std::shared_ptr<GetItemsCache>;
    GetItemsCachePtr s_get_children;
}

BEGIN_MESSAGE_MAP(PidTreeCtrl, CTreeCtrl)
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

struct PidTreeCtrl::Impl
{
    PIDList m_selection;
    PIDList m_selection_in_tree;
    PIDList m_selection_out_tree;
    PIDList m_pids_in_tree;
    PIDList m_pids_out_tree;
    st::map<HTREEITEM, PidEx> m_pids;

    bool m_select_all = false;
    HTREEITEM m_select_all_item = nullptr;

    struct
    {
        std::vector<std::string> locations;
        std::vector<std::string> areas;
    } filter;

    boost::signals2::signal<void()> m_on_click;
    boost::signals2::signal<void(const PID&)> m_on_double_click;
    boost::signals2::signal<void(const PIDList&)> m_on_selection_change;
};

PidTreeCtrl::PidTreeCtrl()
    : m_impl(std::make_shared<Impl>())
{
}

void PidTreeCtrl::PreSubclassWindow()
{
    ModifyStyle(0, TVS_CHECKBOXES);
}

void PidTreeCtrl::init()
{
    m_impl->m_select_all_item = InsertItem("(All)", TVI_ROOT);

    // Helper: Convert UTF-8 to Unicode (wide string) for MFC display
    auto utf8ToWide = [](const std::string& utf8) -> std::wstring {
        if (utf8.empty()) return std::wstring();
        int wlen = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, NULL, 0);
        if (wlen <= 1) return std::wstring();
        std::wstring wstr(wlen - 1, 0);
        MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, &wstr[0], wlen);
        return wstr;
    };

    // Helper: Insert tree item using Unicode directly
    auto insertItemW = [this](const std::wstring& text, HTREEITEM parent) -> HTREEITEM {
        TVINSERTSTRUCTW tvis = {0};
        tvis.hParent = parent;
        tvis.hInsertAfter = TVI_LAST;
        tvis.item.mask = TVIF_TEXT;
        tvis.item.pszText = const_cast<LPWSTR>(text.c_str());
        return (HTREEITEM)::SendMessageW(m_hWnd, TVM_INSERTITEMW, 0, (LPARAM)&tvis);
    };

    for (auto& location : ThisLocation::is_occ() ? Location::get_all_locations_by_order() : Location::get_this_locations())
    {
        if (m_impl->filter.locations.empty() || st::none_of_iequal(m_impl->filter.locations, location->getDisplayName()))
        {
            continue;
        }

        if (auto& pids = (ThisLocation::is_occ() ? PID::get_all_occ(location->getName()) : PID::get_all()); pids.size())
        {
            // Database stores UTF-8, convert to Unicode for display
            std::wstring locationNameW = utf8ToWide(location->getDisplayName());
            auto root = insertItemW(locationNameW, TVI_ROOT);
            m_impl->m_pids.emplace(root, PidEx::npid());

            std::string level;
            HTREEITEM level_item = nullptr;

            for (auto& pid : pids)
            {
                if (m_impl->filter.areas.empty() || st::none_of_iequal(m_impl->filter.areas, pid.level))
                {
                    continue;
                }

                if (pid.level != level)
                {
                    level = pid.level;
                    // Database stores UTF-8, convert to Unicode for display
                    std::wstring levelNameW = utf8ToWide(level);
                    level_item = insertItemW(levelNameW, root);
                    m_impl->m_pids.emplace(level_item, PidEx::npid());
                }

                // Database stores UTF-8, convert to Unicode for display
                std::wstring pidNameW = utf8ToWide(pid.name);
                auto item = insertItemW(pidNameW, level_item);
                m_impl->m_pids.emplace(item, PidEx{pid});
            }
        }
    }

    m_impl->m_pids.push_back_values(m_impl->m_pids_in_tree, std::mem_fn(&PidEx::pid));
    m_impl->m_pids_out_tree = st::remove_copy(PID::get_all(), m_impl->m_pids_in_tree);
}

void PidTreeCtrl::on_apply_filter()
{
    s_get_children.reset();

    m_impl->m_pids.clear();
    m_impl->m_pids_in_tree.clear();
    m_impl->m_pids_out_tree.clear();
    m_impl->m_select_all = false;

    DeleteAllItems();

    init();

    recursive_select(m_impl->m_selection, true);

    m_impl->m_selection_in_tree = get_selection();
    m_impl->m_selection_out_tree = st::remove_copy(m_impl->m_selection, m_impl->m_selection_in_tree);

    Invalidate();
}

void PidTreeCtrl::on_selection_change(std::function<void(const PIDList&)> f)
{
    m_impl->m_on_selection_change.connect(std::move(f));
}

void PidTreeCtrl::on_double_click(std::function<void(const PID& pid)> f)
{
    m_impl->m_on_double_click.connect(std::move(f));
}

void PidTreeCtrl::on_click(std::function<void()> f)
{
    m_impl->m_on_click.connect(std::move(f));
}

void PidTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
    CTreeCtrl::OnLButtonDown(nFlags, point);

    UINT flags = 0;

    if (auto item = HitTest(point, &flags))
    {
        recursive_select(item, GetCheck(item));
        refresh_selection();
    }
    else
    {
        m_impl->m_on_click();
    }
}

void PidTreeCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    UINT flags = 0;

    if (auto item = HitTest(point, &flags))
    {
        if (auto& pid = get_pid(item))
        {
            m_impl->m_on_double_click(pid);
        }

        OnLButtonDown(nFlags, point);
    }
}

void PidTreeCtrl::on_selection(PIDList pids)
{
    if (pids != m_impl->m_selection_in_tree)
    {
        m_impl->m_selection_in_tree = std::move(pids);
        m_impl->m_selection.clear();
        push_back(m_impl->m_selection).range(m_impl->m_selection_in_tree).range(m_impl->m_selection_out_tree);
        m_impl->m_on_selection_change(m_impl->m_selection);
    }
}

void PidTreeCtrl::refresh_selection()
{
    on_selection(get_selection());
}

PIDList PidTreeCtrl::get_selection()
{
    PIDList pids;
    return m_impl->m_pids.push_back_values_if_value(pids, [&](auto& x) { return x.selected && x.pid; }, std::mem_fn(&PidEx::pid));
}

bool PidTreeCtrl::is_all_selected() const
{
    return m_impl->m_pids.all_of_values(std::mem_fn(&PidEx::selected));
}

size_t PidTreeCtrl::get_selection_count() const
{
    return m_impl->m_pids.count_if_value([](auto& x)
    {
        return x.selected && x.pid;
    });
}

HTREEITEM PidTreeCtrl::get_parent(HTREEITEM item) const
{
    return item ? GetParentItem(item) : nullptr;
}

std::vector<HTREEITEM> PidTreeCtrl::get_children_recursive_impl(HTREEITEM item) const
{
    std::vector<HTREEITEM> children;

    if (has_children(item))
    {
        for (auto i = GetChildItem(item); i; i = GetNextSiblingItem(i))
        {
            children.emplace_back(i);

            if (has_children(i))
            {
                push_back(children).range(get_children_recursive_impl(i));
            }
        }
    }

    return children;
}

const std::vector<HTREEITEM>& PidTreeCtrl::get_children(HTREEITEM item) const
{
    if (!s_get_children)
    {
        s_get_children = std::make_shared<GetItemsCache>(make_cached([&](HTREEITEM item)
        {
            return get_children_recursive_impl(item);
        }));
    }

    return (*s_get_children)(item);
}

const std::vector<HTREEITEM>& PidTreeCtrl::get_siblings(HTREEITEM item) const
{
    return get_children(get_parent(item));
}

void PidTreeCtrl::select(HTREEITEM item, bool check)
{
    if (item == m_impl->m_select_all_item)
    {
        SetCheck(m_impl->m_select_all_item, check);
        m_impl->m_select_all = check;
        return;
    }

    if (item && is_selected(item) != check)
    {
        SetCheck(item, check);
        m_impl->m_pids[item].selected = check;
        select(m_impl->m_select_all_item, check && is_all_selected());
    }
}

void PidTreeCtrl::select(const PID& pid, bool check)
{
    select(get_item(pid), check);
}

void PidTreeCtrl::select(const PIDList& pids, bool check)
{
    boost::for_each(pids, check | pipable(lift_select()));
}

void PidTreeCtrl::recursive_select(HTREEITEM item, bool check)
{
    if (item == m_impl->m_select_all_item && check != m_impl->m_select_all)
    {
        select(m_impl->m_select_all_item, check);
        m_impl->m_pids.for_each_key(check | pipable(lift_recursive_select()));
        select(m_impl->m_select_all_item, m_impl->m_pids.size() ? is_all_selected() : check);
        return;
    }

    if (item && is_selected(item) != check)
    {
        select(item, check);

        // recursive-level-down
        boost::for_each(get_children(item), check | pipable(lift_recursive_select()));

        for (auto i = item; i; i = get_parent(i))  // level-up
        {
            if (check == all_of_siblings_selected(i))
            {
                select(get_parent(i), check);
            }
        }
    }
}

void PidTreeCtrl::recursive_select(const PID& pid, bool check)
{
    recursive_select(get_item(pid), check);
}

void PidTreeCtrl::recursive_select(const PIDList& pids, bool check)
{
    boost::for_each(pids, check | pipable(lift_recursive_select()));
}

void PidTreeCtrl::recursive_select_and_notify(const PIDList& pids, bool check)
{
    recursive_select(pids, check);
    refresh_selection();
}

const PID& PidTreeCtrl::get_pid(HTREEITEM item) const
{
    return m_impl->m_pids.get_value_or(item, PidEx::npid()).pid;
}

HTREEITEM PidTreeCtrl::get_item(const PID& pid) const
{
    return m_impl->m_pids.get_key_if_value_or(proj(&PidEx::pid, _ == pid), nullptr);
}

void PidTreeCtrl::select_parent(HTREEITEM item, bool check)
{
    select(get_parent(item), check);
}

void PidTreeCtrl::select_children(HTREEITEM item, bool check)
{
    boost::for_each(get_children(item), [&](auto i) { this->select(i, check); });
}

bool PidTreeCtrl::all_of_children_selected(HTREEITEM item) const
{
    return boost::algorithm::all_of(get_children(item), lift_is_selected()) &&  // recursive
        boost::algorithm::all_of(get_children(item), capture(this)(&PidTreeCtrl::all_of_children_selected));
}

bool PidTreeCtrl::any_of_children_selected(HTREEITEM item) const
{
    return boost::algorithm::any_of(get_children(item), lift_is_selected()) ||
        boost::algorithm::any_of(get_children(item), capture(this)(&PidTreeCtrl::any_of_children_selected));
}

bool PidTreeCtrl::none_of_children_selected(HTREEITEM item) const
{
    return boost::algorithm::none_of(get_children(item), lift_is_selected()) &&
        boost::algorithm::none_of(get_children(item), capture(this)(&PidTreeCtrl::none_of_children_selected));
}

bool PidTreeCtrl::is_selected(HTREEITEM item) const
{
    return item && m_impl->m_pids.get_value_or(item, PidEx::npid()).selected;
}

bool PidTreeCtrl::is_selected(const PID& pid) const
{
    return is_selected(get_item(pid));
}

bool PidTreeCtrl::has_children(HTREEITEM item) const
{
    return item && ItemHasChildren(item);
}

bool PidTreeCtrl::has_parent(HTREEITEM item) const
{
    return item && get_parent(item);
}

bool PidTreeCtrl::all_of_siblings_selected(HTREEITEM item) const
{
    return boost::algorithm::all_of(get_siblings(item), lift_is_selected());
}

bool PidTreeCtrl::any_of_siblings_selected(HTREEITEM item) const
{
    return boost::algorithm::any_of(get_siblings(item), lift_is_selected());
}

bool PidTreeCtrl::none_of_siblings_selected(HTREEITEM item) const
{
    return boost::algorithm::none_of(get_siblings(item), lift_is_selected());
}

bool PidTreeCtrl::all_of_selected(HTREEITEM item) const
{
    return m_impl->m_pids.all_of_values(std::mem_fn(&PidEx::selected));
}

bool PidTreeCtrl::any_of_selected(HTREEITEM item) const
{
    return m_impl->m_pids.any_of_values(std::mem_fn(&PidEx::selected));
}

bool PidTreeCtrl::none_of_selected(HTREEITEM item) const
{
    return m_impl->m_pids.none_of_values(std::mem_fn(&PidEx::selected));
}

bool PidTreeCtrl::apply_location_filter(std::vector<std::string> locations)
{
    if (m_impl->filter.locations != locations)
    {
        m_impl->filter.locations = std::move(locations);
        on_apply_filter();
        return true;
    }

    return false;
}

bool PidTreeCtrl::apply_area_filter(std::vector<std::string> areas)
{
    if (m_impl->filter.areas != areas)
    {
        m_impl->filter.areas = std::move(areas);
        on_apply_filter();
        return true;
    }

    return false;
}

void PidTreeCtrl::set_location_filter(std::vector<std::string> locations)
{
    if (m_impl->filter.locations != locations)
    {
        m_impl->filter.locations = std::move(locations);
    }
}

void PidTreeCtrl::set_area_filter(std::vector<std::string> areas)
{
    if (m_impl->filter.areas != areas)
    {
        m_impl->filter.areas = std::move(areas);
    }
}

void PidTreeCtrl::remove_pids(const PIDList& pids)
{
    st::remove(m_impl->m_selection_out_tree, pids);
    st::remove(m_impl->m_selection, pids);
    recursive_select_and_notify(st::remove_copy(pids, m_impl->m_pids_out_tree), false);
}
