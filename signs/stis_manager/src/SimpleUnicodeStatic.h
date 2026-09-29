/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/J155/transactive/app/signs/stis_manager/src/SimpleUnicodeStatic.h $
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

namespace TA_IRS_App::simpleunicodestatic::detail
{
    struct SimpleUnicodeStatic
    {
        SimpleUnicodeStatic(CDialog* dlg = nullptr, int id = 0);

        void init();
        void set_id(CDialog* dlg, int id);

        void set_window_text(std::wstring str);
        void set_window_text_ansi(std::string str);
        void set_window_text_utf8(std::string str);
        void set_text_color(std::uint32_t color);

        std::wstring get_window_text() const;
        std::string get_window_text_ansi() const;
        std::string get_window_text_utf8() const;

        struct Impl;
        std::shared_ptr<Impl> m_impl;
    };

    using SimpleUnicodeStaticPtr = std::shared_ptr<SimpleUnicodeStatic>;
}

namespace TA_IRS_App
{
    using simpleunicodestatic::detail::SimpleUnicodeStatic;
    using simpleunicodestatic::detail::SimpleUnicodeStaticPtr;
}
