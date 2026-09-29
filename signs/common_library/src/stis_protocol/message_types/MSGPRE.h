#pragma once
#include "MessageTypes.h"
#include "core/utility/src/core/fixed_length_data/all.h"

namespace TA_IRS_App::STIS_PROTOCOL::MESSAGE_DATA_TYPES
{
    using namespace st::fixed_length_data;
    using namespace STIS_PROTOCOL::MESSAGE_TYPES;

    // Pre-Defined Message Structure
    struct MSGPRE : EnableFixedLengthUtility<MSGPRE>
    {
        String<12>                              MessageTag;
        String<14>                              StartTime;
        String<14>                              EndTime;
        Integer<1>                              Priority;
        Integer<1>                              Spare{0};
        Enum<ETemplateType>                     LCDEmergencyDisplayTemplateType = ETemplateType::Default;
        DigitString<3>                          LCDEmergencyDisplayTemplateID;
        Enum<ETemplateType>                     LEDEmergencyDisplayTemplateType = ETemplateType::Default;
        DigitString<3>                          LEDEmergencyDisplayTemplateID;

        auto tied() noexcept
        {
            return std::tie
            (
                MessageTag,
                StartTime,
                EndTime,
                Priority,
                Spare,
                LCDEmergencyDisplayTemplateType,
                LCDEmergencyDisplayTemplateID,
                LEDEmergencyDisplayTemplateType,
                LEDEmergencyDisplayTemplateID
            );
        }

        auto tied() const noexcept
        {
            return const_cast<MSGPRE*>(this)->tied();
        }

        MSGPRE() = default;

        MSGPRE(const PredefinedMessage& rhs)
        {
            *this = rhs;
        }

        MSGPRE& operator=(const PredefinedMessage& rhs)
        {
            clear();
            tied() = rhs.tied();
            return *this;
        }

        std::ostream& dump_details(std::ostream& os, int indent = 0) const
        {
            std::string INDENT(indent, ' ');
            os << INDENT << boost::format("MessageTag[%d]: %s\n") % MessageTag.size() % MessageTag.str();
            os << INDENT << boost::format("StartTime[%d]: %s\n") % StartTime.size() % StartTime.str();
            os << INDENT << boost::format("EndTime[%d]: %s\n") % EndTime.size() % EndTime.str();
            os << INDENT << boost::format("Priority[%d]: %s\n") % Priority.size() % Priority.str();
            os << INDENT << boost::format("Spare[%d]: %s\n") % Spare.size() % Spare.str();
            os << INDENT << boost::format("LCDEmergencyDisplayTemplateType[%d]: %s\n") % LCDEmergencyDisplayTemplateType.size() % LCDEmergencyDisplayTemplateType.str();
            os << INDENT << boost::format("LCDEmergencyDisplayTemplateID[%d]: %s\n") % LCDEmergencyDisplayTemplateID.size() % LCDEmergencyDisplayTemplateID.str();
            os << INDENT << boost::format("LEDEmergencyDisplayTemplateType[%d]: %s\n") % LEDEmergencyDisplayTemplateType.size() % LEDEmergencyDisplayTemplateType.str();
            os << INDENT << boost::format("LEDEmergencyDisplayTemplateID[%d]: %s\n") % LEDEmergencyDisplayTemplateID.size() % LEDEmergencyDisplayTemplateID.str();
            return os;
        }

        std::string dump_details(int indent = 0) const
        {
            std::stringstream ss;
            dump_details(ss, indent);
            return ss.str();
        }
    };
}
