#pragma once
#include <string>
#include <vector>

struct SimpleTreeFilterCtrl : CTreeCtrl
{
    SimpleTreeFilterCtrl();

    // iterface

    void init(const std::vector<std::string>& texts, bool check = true);
    bool update(const std::vector<std::string>& texts);
    std::vector<std::string> get_selection() const;
    void select_all(bool check);
    bool is_all_selected() const;
    size_t get_selection_count() const;
    void on_selection_change(std::function<void(const std::vector<std::string>&)> f);
    std::string get_title() const;
    std::string get_tooltip() const;

    // implementation

    // hide base functions
    BOOL ShowWindow(int nCmdShow);

    void select(HTREEITEM item, bool check);
    void select(const std::string& text, bool check);
    auto lift_select() { return [this](auto& x, bool check) { this->select(x, check); }; };
    void on_selection(std::vector<std::string> texts);
    void refresh_selection();
    void reselect();
    bool is_selected(HTREEITEM item) const;
    HTREEITEM get_item(const std::string& text) const;
    std::string get_text(HTREEITEM item) const;

    DECLARE_MESSAGE_MAP()
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnTimer(UINT nIDEvent);
    virtual void PreSubclassWindow() override;

    struct Impl;
    std::shared_ptr<Impl> m_impl;
};
