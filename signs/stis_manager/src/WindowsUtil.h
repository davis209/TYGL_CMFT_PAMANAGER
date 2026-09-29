/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/J155/transactive/app/signs/stis_manager/src/WindowsUtil.h $
 * @author:  Robin Ashcroft
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * The free text message selection page
 *
 */

#include <afx.h>
#include <string>
#include <memory>

namespace TA_IRS_App::windowsutil::detail
{
    struct WindowsUtil
    {
        static HFONT create_font(CWnd* w, std::string name = "Arial Bold", int point_size = 10);
        static HFONT get_transactive_font(CWnd* w);
        static HFONT get_default_font(CWnd* w);

        static int get_selection(CListCtrl& list);
        static std::vector<int> get_all_selection(CListCtrl& list);
        static std::vector<DWORD> get_all_selection_data(CListCtrl& list);
        static size_t delete_selected_items(CListCtrl& list);
        static bool has_selection(CListCtrl& list);
        static bool select(CListCtrl& list, const std::string& str);

        static HWND find_main_window(size_t pid);
        static bool is_main_window(HWND hwnd);
        static HWND find_main_window_recursive(size_t ppid);
        static void WindowsUtil::activate_window(HWND hwnd);
        static void WindowsUtil::activate_window(CWnd& wnd);
        static void WindowsUtil::show_window(HWND hwnd);
        static void WindowsUtil::hide_window(HWND hwnd);
        static void WindowsUtil::show_window(CWnd& w);
        static void WindowsUtil::hide_window(CWnd& w);

        static std::vector<size_t> get_child_process_ids(size_t ppid);
        static std::vector<size_t> get_child_process_ids_recursive(size_t ppid);
        static size_t get_parent_process_id(size_t pid);
        static std::string get_program(size_t pid);
        static size_t get_root_process_id(size_t pid);
        static size_t get_root_process_id_for_the_same_program(size_t pid);
        static void kill_process(size_t pid, size_t exit_code = 0);
        static void kill_process_tree(size_t ppid, size_t exit_code = 0);
        static size_t get_pid_by_tcp_port(unsigned int port);
        static size_t get_pid_by_tcp_port(std::string port);
    };
}

namespace TA_IRS_App
{
    using windowsutil::detail::WindowsUtil;
}
