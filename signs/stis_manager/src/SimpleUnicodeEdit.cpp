/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/J155/transactive/app/signs/stis_manager/src/SimpleUnicodeEdit.cpp $
 * @author:  Robin Ashcroft
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * The free text message selection page
 *
 */

#include "stdafx.h"
#include "SimpleUnicodeControl.h"
#include "SimpleUnicodeEdit.h"
#include "helperfun.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/CodeConverter.h"

namespace TA_IRS_App::simpleunicodeedit::detail
{
    using namespace TA_Base_Core;

    struct SimpleUnicodeEdit::Impl : SimpleUnicodeControl
    {
        using SimpleUnicodeControl::SimpleUnicodeControl;

        void init() override
        {
            this->create_window(L"EDIT");
        }

        void set_readonly(bool b)
        {
            ::SendMessage(m_hwnd, EM_SETREADONLY, (WPARAM)b, 0);
        }

        void set_limit_text(int n)
        {
            ::PostMessage(m_hwnd, EM_LIMITTEXT, n, 0);
        }
    };

    SimpleUnicodeEdit::SimpleUnicodeEdit(CDialog* dlg, int id)
        : m_impl(std::make_shared<Impl>(dlg, id))
    {
    }

    void SimpleUnicodeEdit::init()
    {
        m_impl->init();
    }

    void SimpleUnicodeEdit::set_id(CDialog* dlg, int id)
    {
        m_impl->set_id(dlg, id);
    }

    void SimpleUnicodeEdit::set_window_text(std::wstring str)
    {
        m_impl->set_window_text(std::move(str));
    }

    void SimpleUnicodeEdit::set_window_text_ansi(std::string str)
    {
        m_impl->set_window_text_ansi(std::move(str));
    }

    void SimpleUnicodeEdit::set_window_text_utf8(std::string str)
    {
        m_impl->set_window_text_utf8(std::move(str));
    }

    std::wstring SimpleUnicodeEdit::get_window_text() const
    {
        return m_impl->get_window_text();
    }

    std::string SimpleUnicodeEdit::get_window_text_ansi() const
    {
        return m_impl->get_window_text_ansi();
    }

    std::string SimpleUnicodeEdit::get_window_text_utf8() const
    {
        return m_impl->get_window_text_utf8();
    }

    void SimpleUnicodeEdit::set_limit_text(int n)
    {
        m_impl->set_limit_text(n);
    }

    void SimpleUnicodeEdit::set_readonly(bool b)
    {
        return m_impl->set_readonly(b);
    }

    void SimpleUnicodeEdit::use_default_font(bool b)
    {
        return m_impl->use_default_font(b);
    }

    void SimpleUnicodeEdit::invalidate()
    {
        return m_impl->invalidate();
    }

    void SimpleUnicodeEdit::on_size(unsigned int nType, int cx, int cy)
    {
        return m_impl->on_size(nType, cx, cy);
    }

    void SimpleUnicodeEdit::set_text_color(std::uint32_t color)
    {
        m_impl->set_text_color((COLORREF)color);
    }
}
