/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/J155/transactive/app/signs/stis_manager/src/SimpleUnicodeStatic.cpp $
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
#include "SimpleUnicodeStatic.h"
#include "helperfun.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/CodeConverter.h"

namespace TA_IRS_App::simpleunicodestatic::detail
{
    using namespace TA_Base_Core;

    struct SimpleUnicodeStatic::Impl : SimpleUnicodeControl
    {
        using SimpleUnicodeControl::SimpleUnicodeControl;

        void init() override
        {
            this->create_window(L"STATIC");
        }
    };

    SimpleUnicodeStatic::SimpleUnicodeStatic(CDialog* dlg, int id)
        : m_impl(std::make_shared<Impl>(dlg, id))
    {
    }

    void SimpleUnicodeStatic::init()
    {
        m_impl->init();
    }

    void SimpleUnicodeStatic::set_id(CDialog* dlg, int id)
    {
        m_impl->set_id(dlg, id);
    }

    void SimpleUnicodeStatic::set_window_text(std::wstring str)
    {
        m_impl->set_window_text(std::move(str));
    }

    void SimpleUnicodeStatic::set_window_text_ansi(std::string str)
    {
        m_impl->set_window_text_ansi(std::move(str));
    }

    void SimpleUnicodeStatic::set_window_text_utf8(std::string str)
    {
        m_impl->set_window_text_utf8(std::move(str));
    }

    std::wstring SimpleUnicodeStatic::get_window_text() const
    {
        return m_impl->get_window_text();
    }

    std::string SimpleUnicodeStatic::get_window_text_ansi() const
    {
        return m_impl->get_window_text_ansi();
    }

    std::string SimpleUnicodeStatic::get_window_text_utf8() const
    {
        return m_impl->get_window_text_utf8();
    }

    void SimpleUnicodeStatic::set_text_color(std::uint32_t color)
    {
        m_impl->set_text_color((COLORREF)color);
    }
}
