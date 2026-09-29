#pragma once
#include "MessageTypes.h"
#include "core/utility/src/core/fixed_length_data/all.h"

namespace TA_IRS_App::STIS_PROTOCOL::MESSAGE_DATA_TYPES
{
    using namespace STIS_PROTOCOL::MESSAGE_TYPES;
    using namespace st::fixed_length_data;

    // Ad Hoc Message Structure
    struct MSGFREE : EnableFixedLengthUtility<MSGFREE>
    {
#if 0
        using MESSAGETEXT = Text<Integer<4>>;
#else
        using MESSAGETEXT = UnicodeText;
#endif

        String<12>                              MessageTag;
        String<14>                              StartTime;
        String<14>                              EndTime;
        Integer<1>                              Priority;
        Enum<ETemplateType>                     LCDEmergencyDisplayTemplateType = ETemplateType::Default;
        DigitString<3>                          LCDEmergencyDisplayTemplateID;
        Enum<ETemplateType>                     LEDEmergencyDisplayTemplateType = ETemplateType::Default;
        DigitString<3>                          LEDEmergencyDisplayTemplateID;
        // Integer<4>                           MessageLength;
        MESSAGETEXT                             MessageText;

        auto tied() noexcept
        {
            return std::tie
            (
                MessageTag,
                StartTime,
                EndTime,
                Priority,
                LCDEmergencyDisplayTemplateType,
                LCDEmergencyDisplayTemplateID,
                LEDEmergencyDisplayTemplateType,
                LEDEmergencyDisplayTemplateID,
                // MessageLength,
                MessageText
            );
        }

        auto tied() const noexcept
        {
            return const_cast<MSGFREE*>(this)->tied();
        }

        MSGFREE() = default;

        MSGFREE(const AdHodMessage& rhs)
        {
            *this = rhs;
        }

        MSGFREE& operator=(const AdHodMessage& rhs)
        {
            clear();
            tied() = rhs.tied();
            return *this;
        }

        std::ostream& dump_details(std::ostream& os, int indent = 0) const;

        std::string dump_details(int indent = 0) const
        {
            std::stringstream ss;
            dump_details(ss, indent);
            return ss.str();
        }
    };
}
