#pragma once
#include "app/signs/common_library/src/STISUtility.h"

using namespace TA_IRS_App;

struct PidTreeCtrl : CTreeCtrl
{
    using PID = STIS_UTILITY::PID;
    using PIDList = STIS_UTILITY::PIDList;

    PidTreeCtrl();

    void init();

    PIDList get_selection();
    bool is_all_selected() const;
    size_t get_selection_count() const;
    void on_selection_change(std::function<void(const PIDList&)> f);
    void on_double_click(std::function<void(const PID& pid)> f);
    void on_click(std::function<void()> f);

    void set_location_filter(std::vector<std::string> locations);
    void set_area_filter(std::vector<std::string> areas);
    bool apply_location_filter(std::vector<std::string> locations);
    bool apply_area_filter(std::vector<std::string> areas);
    void on_apply_filter();

    // implementation

    void select(HTREEITEM item, bool check = true);
    void select(const PID& pid, bool check = true);
    void select(const PIDList& pids, bool check = true);
    auto lift_select() { return [this](auto& x, bool check) { this->select(x, check); }; }
    void recursive_select(HTREEITEM item, bool check = true);
    void recursive_select(const PID& pid, bool check = true);
    void recursive_select(const PIDList& pids, bool check = true);
    auto lift_recursive_select() { return [this](auto& x, bool check) { this->recursive_select(x, check); }; }
    void recursive_select_and_notify(const PIDList& pids, bool check = true);
    void select_parent(HTREEITEM item, bool check = true);
    void select_children(HTREEITEM item, bool check = true);
    void refresh_selection();
    void on_selection(PIDList pids);
    void remove_pids(const PIDList& pids);

    const PID& get_pid(HTREEITEM item) const;
    HTREEITEM get_item(const PID& pid) const;
    HTREEITEM get_parent(HTREEITEM item) const;
    const std::vector<HTREEITEM>& get_children(HTREEITEM item) const;
    std::vector<HTREEITEM> get_children_recursive_impl(HTREEITEM item) const;
    const std::vector<HTREEITEM>& get_siblings(HTREEITEM item) const;

    bool is_selected(HTREEITEM item) const;
    bool is_selected(const PID& pid) const;
    auto lift_is_selected() const { return [this](auto& x) { return this->is_selected(x); }; }

    bool has_children(HTREEITEM item) const;
    bool has_parent(HTREEITEM item) const;

    bool all_of_children_selected(HTREEITEM item) const;
    bool any_of_children_selected(HTREEITEM item) const;
    bool none_of_children_selected(HTREEITEM item) const;

    bool all_of_siblings_selected(HTREEITEM item) const;
    bool any_of_siblings_selected(HTREEITEM item) const;
    bool none_of_siblings_selected(HTREEITEM item) const;

    bool all_of_selected(HTREEITEM item) const;
    bool any_of_selected(HTREEITEM item) const;
    bool none_of_selected(HTREEITEM item) const;

    DECLARE_MESSAGE_MAP()
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    virtual void PreSubclassWindow() override;

    struct Impl;
    std::shared_ptr<Impl> m_impl;
};
