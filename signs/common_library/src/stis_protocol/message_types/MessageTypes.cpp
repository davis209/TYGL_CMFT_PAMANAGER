#include "pch.h"
#include "MessageTypes.h"
#include "core/utility/src/core/algorithm/strings.h"

namespace TA_IRS_App::STIS_PROTOCOL::MESSAGE_TYPES
{
    std::string CurrentDisplayMessage::get_message_text_utf8() const
    {
        return st2::utf16le_to_utf8(st2::as_u16string(message_text));
    }

    std::string AdHodMessage::get_message_text_utf8() const
    {
        return st2::utf16le_to_utf8(st2::as_u16string(message_text));
    }
}
