/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/J155/transactive/app/signs/stis_manager/src/SimpleUnicodeControl.h $
 * @author:  Robin Ashcroft
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * The free text message selection page
 *
 */

#include "WindowsUtil.h"
#include "core/utilities/src/CodeConverter.h"
#include "core/utility/src/core/Map.h"
#include <boost/scope_exit.hpp>
#include <string>
#include <memory>
#include <afx.h>

namespace TA_IRS_App::simpleunicodecontrol::detail
{
    using namespace TA_Base_Core;

    struct SimpleUnicodeControl
    {
        SimpleUnicodeControl(CDialog* dlg = nullptr, int id = 0) :
            m_dialog(dlg),
            m_id(id)
        {
        }

        virtual ~SimpleUnicodeControl() = default;

        virtual void init() = 0;

        HWND create_window(std::wstring class_name)
        {
            if (auto w = m_dialog->GetDlgItem(m_id))
            {
                CRect r;
                w->GetClientRect(r);
                m_hwnd = ::CreateWindowExW(w->GetExStyle(), class_name.c_str(), NULL, w->GetStyle(), r.left, r.top, r.Width(), r.Height(), w->m_hWnd, NULL, NULL, NULL);

                if (m_font == nullptr)
                {
                    set_font(m_use_default_font ? WindowsUtil::get_default_font(w) : WindowsUtil::get_transactive_font(w));
                }

                if (m_text_color)
                {
                    set_text_color(m_text_color);
                }
            }

            return m_hwnd;
        }

        void set_id(CDialog* dlg, int id)
        {
            m_dialog = dlg;
            m_id = id;

            if (m_hwnd == nullptr)
            {
                return;
            }

            ::SendMessage(m_hwnd, WM_CLOSE, 0, 0);
            this->init();
        }

        void set_font(HFONT font)
        {
            m_font = font;

            if (m_hwnd)
            {
                ::PostMessage(m_hwnd, WM_SETFONT, (WPARAM)m_font, 0);
            }
        }

        void set_window_text(std::wstring str)
        {
            ::SetWindowTextW(m_hwnd, str.c_str());
        }

        void set_window_text_utf8(std::string str)
        {
            set_window_text(CodeConverter::UTF8ToUnicode(str));
        }

        void set_window_text_ansi(std::string str)
        {
            set_window_text(CodeConverter::ANSIToUnicode(str));
        }

        std::string get_window_text_ansi() const
        {
            return CodeConverter::UnicodeToANSI(get_window_text());
        }

        std::string get_window_text_utf8() const
        {
            return CodeConverter::UnicodeToUTF8(get_window_text());
        }

        std::wstring get_window_text() const
        {
            wchar_t buffer[10240];
            ::GetWindowTextW(m_hwnd, buffer, 10240);
            return buffer;
        }

        void set_readonly(bool b)
        {
            ::SendMessage(m_hwnd, EM_SETREADONLY, (WPARAM)b, 0);
        }

        void set_limit_text(int n)
        {
            ::PostMessage(m_hwnd, EM_LIMITTEXT, n, 0);
        }

        void use_default_font(bool b = true)
        {
            m_use_default_font = b;
        }

        void invalidate()
        {
            m_dialog->Invalidate();

            if (auto w = m_dialog->GetDlgItem(m_id))
            {
                w->Invalidate();
            }
        }

        void on_size(unsigned int nType, int cx, int cy)
        {
            if (auto w = m_dialog->GetDlgItem(m_id))
            {
                CRect r;
                w->GetClientRect(r);
                ::MoveWindow(m_hwnd, r.left, r.top, r.Width(), r.Height(), TRUE);
            }
        }

        void set_text_color(COLORREF color)
        {
            static st::map<HWND, WNDPROC> s_wndprocs;
            static st::map<HWND, COLORREF> s_text_colors;

            struct OnCtlColor
            {
                static LRESULT CALLBACK callback(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
                {
                    if (WM_CTLCOLORSTATIC == message || WM_CTLCOLOREDIT == message)
                    {
                        auto dc = (HDC)wparam;
                        ::SetBkMode(dc, TRANSPARENT);
                        ::SetTextColor(dc, s_text_colors[hwnd]);
                        static auto s_brush = ::CreateSolidBrush(::GetSysColor(COLOR_BTNFACE));
                        return (LRESULT)s_brush;
                    }

                    return ::CallWindowProc(s_wndprocs[hwnd], hwnd, message, wparam, lparam);
                }
            };

            auto w = m_dialog->GetDlgItem(m_id);

            if (w == nullptr)
            {
                m_text_color = color;
                return;
            }

            auto hwnd = w->GetSafeHwnd();

            if (s_wndprocs.count(hwnd) == 0)
            {
                s_wndprocs[hwnd] = (WNDPROC)::SetWindowLong(hwnd, GWL_WNDPROC, (LONG)&OnCtlColor::callback);
            }

            if (s_text_colors[hwnd] != color)
            {
                m_text_color = color;
                s_text_colors[hwnd] = color;
                w->Invalidate();
            }
        }

        int m_id = 0;
        CWnd* m_dialog = nullptr;
        HWND m_hwnd = nullptr;
        HFONT m_font = nullptr;
        COLORREF m_text_color = 0;
        bool m_use_default_font = false;
    };

    using SimpleUnicodeControlPtr = std::shared_ptr<SimpleUnicodeControl>;
}

namespace TA_IRS_App
{
    using simpleunicodecontrol::detail::SimpleUnicodeControl;
    using simpleunicodecontrol::detail::SimpleUnicodeControlPtr;
}
