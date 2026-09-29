#pragma once
#include "MessageTypes.h"
#include "core/utility/src/core/fixed_length_data/all.h"

namespace TA_IRS_App::STIS_PROTOCOL::MESSAGE_DATA_TYPES
{
    using namespace STIS_PROTOCOL::MESSAGE_TYPES;
    using namespace st::fixed_length_data;

    // Pre-Defined DisplayTemplate Structure
    struct TLPRE : EnableFixedLengthUtility<TLPRE>
    {
        Enum<EDisplayTemplateType>  DisplayTemplateType = EDisplayTemplateType::Default;
        DigitString<3>              DisplayTemplateID;
        String<14>                  StartTime;
        String<14>                  EndTime;

        auto tied() noexcept
        {
            return std::tie(DisplayTemplateType, DisplayTemplateID, StartTime, EndTime);
        }

        auto tied() const noexcept
        {
            return const_cast<TLPRE*>(this)->tied();
        }

        TLPRE() = default;

        TLPRE(const PredefinedDisplayTemplate& rhs)
        {
            *this = rhs;
        }

        TLPRE& operator=(const PredefinedDisplayTemplate& rhs)
        {
            clear();
            tied() = rhs.tied();
            return *this;
        }

        operator PredefinedDisplayTemplate() const
        {
            PredefinedDisplayTemplate t;
            t.tied() = this->tied();
            return t;
        }

        std::ostream& dump_details(std::ostream& os, int indent = 0) const
        {
            std::string INDENT(indent, ' ');
            os << INDENT << boost::format("DisplayTemplateType[%d]: %s\n") % DisplayTemplateType.size() % DisplayTemplateType.str();
            os << INDENT << boost::format("DisplayTemplateID[%d]: %s\n") % DisplayTemplateID.size() % DisplayTemplateID.str();
            os << INDENT << boost::format("StartTime[%d]: %s\n") % StartTime.size() % StartTime.str();
            os << INDENT << boost::format("EndTime[%d]: %s\n") % EndTime.size() % EndTime.str();
            return os;
        }

        std::string dump_details(int indent = 0) const
        {
            std::stringstream ss;
            dump_details(ss, indent);
            return ss.str();
        }

        // helpers

        std::string make_unique_id() const // 4 digits: 1000
        {
            return DisplayTemplateType.str() + DisplayTemplateID.str();
        }

        bool is_empty() const
        {
            return DisplayTemplateType.empty() && DisplayTemplateID.empty() && (StartTime.empty() || StartTime.all_zero()) && (EndTime.empty() || EndTime.all_zero());
        }
    };
}
