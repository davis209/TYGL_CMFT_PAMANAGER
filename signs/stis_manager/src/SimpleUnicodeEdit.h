/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/J155/transactive/app/signs/stis_manager/src/SimpleUnicodeEdit.h $
 * @author:  Robin Ashcroft
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * The free text message selection page
 *
 */

#pragma once
#include <string>
#include <memory>
#include <cstdint>

class CDialog;

namespace TA_IRS_App::simpleunicodeedit::detail
{
    struct SimpleUnicodeEdit
    {
        SimpleUnicodeEdit(CDialog* dlg = nullptr, int id = 0);

        void init();
        void set_id(CDialog* dlg, int id);

        void set_window_text(std::wstring str);
        void set_window_text_ansi(std::string str);
        void set_window_text_utf8(std::string str);
        void set_text_color(std::uint32_t color);

        std::wstring get_window_text() const;
        std::string get_window_text_ansi() const;
        std::string get_window_text_utf8() const;

        void set_limit_text(int n);
        void set_readonly(bool b);
        void use_default_font(bool b = true);

        void invalidate();
        void on_size(unsigned int nType, int cx, int cy);

        struct Impl;
        std::shared_ptr<Impl> m_impl;
    };

    using SimpleUnicodeEditPtr = std::shared_ptr<SimpleUnicodeEdit>;
}

namespace TA_IRS_App
{
    using simpleunicodeedit::detail::SimpleUnicodeEdit;
    using simpleunicodeedit::detail::SimpleUnicodeEditPtr;
}
