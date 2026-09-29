#include "StdAfx.h"
#include "WindowsUtil.h"
#include "core/utility/src/base_ex/DebugUtilEx.h"
#include "core/utility/src/core/MakeNameValuePairsString.h"
#include <boost/scope_exit.hpp>
#include <boost/assign.hpp>
#include <string>
#include <memory>
#include <tlhelp32.h>

using boost::filesystem::path;

namespace
{
    bool s_debug = false;

    template <class F>
    void for_each_process(F func)
    {
        auto snap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        BOOST_SCOPE_EXIT_ALL(&) { ::CloseHandle(snap); };

        if (snap == INVALID_HANDLE_VALUE)
        {
            return;
        }

        PROCESSENTRY32 pe{};
        pe.dwSize = sizeof(pe);
        auto process = ::Process32First(snap, &pe);

        while (process)
        {
            func(pe);
            process = Process32Next(snap, &pe);
        }
    }

    template <class F>
    std::optional<PROCESSENTRY32> find_process(F pred)
    {
        auto snap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        BOOST_SCOPE_EXIT_ALL(&) { ::CloseHandle(snap); };

        if (snap == INVALID_HANDLE_VALUE)
        {
            return std::nullopt;
        }

        PROCESSENTRY32 pe{};
        pe.dwSize = sizeof(pe);
        auto process = ::Process32First(snap, &pe);

        while (process)
        {
            if (pred(pe))
            {
                return pe;
            }

            process = Process32Next(snap, &pe);
        }

        return std::nullopt;
    }
}

namespace TA_IRS_App::windowsutil::detail
{
    HFONT WindowsUtil::create_font(CWnd* w, std::string name, int point_size)
    {
        auto pDC = w->GetDC();
        BOOST_SCOPE_EXIT_ALL(&) { w->ReleaseDC(pDC); };
        LOGFONT lf;
        std::memset(&lf, 0, sizeof(LOGFONT));
        lf.lfHeight = -::MulDiv(point_size, ::GetDeviceCaps(pDC->m_hDC, LOGPIXELSY), 72);
        lf.lfCharSet = DEFAULT_CHARSET;
        std::strcpy(lf.lfFaceName, name.c_str());
        return ::CreateFontIndirect(&lf);
    }

    HFONT WindowsUtil::get_transactive_font(CWnd* w)
    {
        return create_font(w, "Arial Bold", 10);
    }

    HFONT WindowsUtil::get_default_font(CWnd* w)
    {
        return create_font(w, "MS Shell Dlg", 8);
    }

    int WindowsUtil::get_selection(CListCtrl& list)
    {
        auto pos = list.GetFirstSelectedItemPosition();
        return pos ? list.GetNextSelectedItem(pos) : -1;
    }

    bool WindowsUtil::has_selection(CListCtrl& list)
    {
        return get_selection(list) != -1;
    }

    bool WindowsUtil::select(CListCtrl& list, const std::string& str)
    {
        for (int i = 0, count = list.GetItemCount(); i < count; ++i)
        {
            auto text = list.GetItemText(i, 0);

            if (boost::iequals((const char*)text, str.c_str()))
            {
                list.SetItemState(-1, 0, LVIS_SELECTED);
                list.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
                return true;
            }
        }

        return false;
    }

    std::vector<int> WindowsUtil::get_all_selection(CListCtrl& list)
    {
        std::vector<int> res;

        for (auto pos = list.GetFirstSelectedItemPosition(); pos;)
        {
            res.emplace_back(list.GetNextSelectedItem(pos));
        }

        return res;
    }

    std::vector<DWORD> WindowsUtil::get_all_selection_data(CListCtrl& list)
    {
        std::vector<unsigned long> res;

        for (auto i : get_all_selection(list))
        {
            res.emplace_back(list.GetItemData(i));
        }

        return res;
    }

    size_t WindowsUtil::delete_selected_items(CListCtrl& list)
    {
        auto selection = get_all_selection(list);

        for (auto i : boost::reverse(selection))
        {
            list.DeleteItem(i);
        }

        return selection.size();
    }

    bool WindowsUtil::is_main_window(HWND hwnd)
    {
        return ::GetWindow(hwnd, GW_OWNER) == nullptr && ::IsWindowVisible(hwnd);
    }

    HWND WindowsUtil::find_main_window(size_t pid)
    {
        struct Data
        {
            size_t pid = 0;
            HWND hwnd = nullptr;
        };

        struct Callback
        {
            static BOOL CALLBACK enum_windows_proc(HWND hwnd, LPARAM lParam)
            {
                auto& data = *reinterpret_cast<Data*>(lParam);
                DWORD pid = 0;
                ::GetWindowThreadProcessId(hwnd, &pid);

                if (pid == data.pid && WindowsUtil::is_main_window(hwnd))
                {
                    data.hwnd = hwnd;
                    return FALSE;
                }

                return TRUE;
            }
        };

        Data data{pid, nullptr};
        ::EnumWindows(&Callback::enum_windows_proc, (LPARAM)&data);
        return data.hwnd;
    }

    HWND WindowsUtil::find_main_window_recursive(size_t ppid)
    {
        for (auto pid : get_child_process_ids_recursive(ppid))
        {
            if (auto hwnd = find_main_window(pid))
            {
                return hwnd;
            }
        }

        return nullptr;
    }

    std::vector<size_t> WindowsUtil::get_child_process_ids(size_t ppid)
    {
        std::vector<size_t> pids;

        for_each_process([&](PROCESSENTRY32& pe)
        {
            if (pe.th32ParentProcessID == ppid)
            {
                pids.emplace_back(pe.th32ProcessID);
            }
        });

        return pids;
    }

    std::vector<size_t> WindowsUtil::get_child_process_ids_recursive(size_t ppid)
    {
        std::vector<size_t> pids;

        for (auto pid : get_child_process_ids(ppid))
        {
            pids.emplace_back(pid);
            boost::assign::push_back(pids).range(get_child_process_ids_recursive(pid));
        }

        return pids;
    }

    std::string WindowsUtil::get_program(size_t pid)
    {
        auto pe = find_process([&](PROCESSENTRY32& pe) { return pe.th32ProcessID == pid; });
        return pe.has_value() ? pe->szExeFile : "";
    }

    size_t WindowsUtil::get_parent_process_id(size_t pid)
    {
        auto pe = find_process([&](PROCESSENTRY32& pe) { return pe.th32ProcessID == pid; });
        return pe.has_value() ? pe->th32ParentProcessID : 0;
    }

    size_t WindowsUtil::get_root_process_id(size_t pid)
    {
        size_t topmost = 0;

        while (pid = get_parent_process_id(pid))
        {
            topmost = pid;
        }

        return topmost;
    }

    size_t WindowsUtil::get_root_process_id_for_the_same_program(size_t pid)
    {
        auto program = get_program(pid);
        auto topmost = pid;

        while (auto res = find_process([&](PROCESSENTRY32& pe) { return pe.th32ProcessID == pid && boost::iequals(program, path(pe.szExeFile).filename().string()); }))
        {
            topmost = pid;
            pid = res->th32ParentProcessID;
        }

        return topmost;
    }

    void WindowsUtil::kill_process(size_t pid, size_t exit_code)
    {
        auto handle = ::OpenProcess(SYNCHRONIZE | PROCESS_TERMINATE, TRUE, pid);
        ::TerminateProcess(handle, 0);
    }

    void WindowsUtil::kill_process_tree(size_t ppid, size_t exit_code)
    {
        for (auto pid : get_child_process_ids_recursive(ppid))
        {
            kill_process(pid, exit_code);
        }
    }

    void WindowsUtil::activate_window(HWND hwnd)
    {
        if (hwnd)
        {
            CWnd w;
            w.Attach(hwnd);
            activate_window(w);
            w.Detach();
        }
    }

    void WindowsUtil::activate_window(CWnd& w)
    {
        w.SetForegroundWindow();
        w.SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        w.SetWindowPos(&CWnd::wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    }

    void WindowsUtil::show_window(HWND hwnd)
    {
        if (hwnd)
        {
            CWnd w;
            w.Attach(hwnd);
            show_window(w);
            w.Detach();
        }
    }

    void WindowsUtil::hide_window(HWND hwnd)
    {
        if (hwnd)
        {
            CWnd w;
            w.Attach(hwnd);
            hide_window(w);
            w.Detach();
        }
    }

    void WindowsUtil::show_window(CWnd& w)
    {
        if (w.m_hWnd)
        {
            // w.ShowWindow(SW_SHOW);
            w.ShowWindow(SW_SHOWNORMAL);
        }
    }

    void WindowsUtil::hide_window(CWnd& w)
    {
        if (w.m_hWnd)
        {
            w.ShowWindow(SW_HIDE);
        }
    }

    size_t WindowsUtil::get_pid_by_tcp_port(std::string port)
    {
        return get_pid_by_tcp_port(std::stoi(port));
    }

    size_t WindowsUtil::get_pid_by_tcp_port(unsigned int port)
    {
        char buffer[10240] = {0};
        DWORD size = 10240;

        if (auto res = ::GetExtendedTcpTable(buffer, &size, FALSE, AF_INET, TCP_TABLE_OWNER_PID_LISTENER, 0); NO_ERROR == res)
        {
            auto& t = *(MIB_TCPTABLE_OWNER_PID*)buffer;

            for (auto i = 0; i < t.dwNumEntries; ++i)
            {
                MIB_TCPROW_OWNER_PID& r = t.table[i];
                LOG_DEBUG_IF(s_debug, "get_pid_by_tcp_port(): %s", nvps(r.dwState, r.dwLocalAddr, ntohs(r.dwLocalPort), r.dwRemoteAddr, ntohs(r.dwRemotePort), r.dwOwningPid));

                if (ntohs(r.dwLocalPort) == port)
                {
                    return r.dwOwningPid;
                }
            }
        }

        return 0;
    }
}
