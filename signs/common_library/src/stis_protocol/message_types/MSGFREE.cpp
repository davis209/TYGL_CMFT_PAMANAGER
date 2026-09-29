#include "pch.h"
#include "MSGFREE.h"
#include "app/signs/common_library/src/STISUtility.h"

namespace TA_IRS_App::STIS_PROTOCOL::MESSAGE_DATA_TYPES
{
    using namespace STIS_PROTOCOL::MESSAGE_TYPES;
    using namespace st::fixed_length_data;

    std::ostream& MSGFREE::dump_details(std::ostream& os, int indent) const
    {
        std::string INDENT(indent, ' ');
        os << INDENT << boost::format("MessageTag[%d]: %s\n") % MessageTag.size() % MessageTag.str();
        os << INDENT << boost::format("StartTime[%d]: %s\n") % StartTime.size() % StartTime.str();
        os << INDENT << boost::format("EndTime[%d]: %s\n") % EndTime.size() % EndTime.str();
        os << INDENT << boost::format("Priority[%d]: %s\n") % Priority.size() % Priority.str();
        os << INDENT << boost::format("LCDEmergencyDisplayTemplateType[%d]: %s\n") % LCDEmergencyDisplayTemplateType.size() % LCDEmergencyDisplayTemplateType.str();
        os << INDENT << boost::format("LCDEmergencyDisplayTemplateID[%d]: %s\n") % LCDEmergencyDisplayTemplateID.size() % LCDEmergencyDisplayTemplateID.str();
        os << INDENT << boost::format("LEDEmergencyDisplayTemplateType[%d]: %s\n") % LEDEmergencyDisplayTemplateType.size() % LEDEmergencyDisplayTemplateType.str();
        os << INDENT << boost::format("LEDEmergencyDisplayTemplateID[%d]: %s\n") % LEDEmergencyDisplayTemplateID.size() % LEDEmergencyDisplayTemplateID.str();
        os << INDENT << boost::format("MessageLength[%d]: %s\n") % MessageText.m_length.size() % MessageText.m_length.str();
        os << INDENT << boost::format("MESSAGETEXT[%d]: %s\n") % MessageText.m_text.size() % STIS_UTILITY::transform_4_languages_from_utf16_to_utf8(MessageText.m_text, "");
        return os;
    }
}
