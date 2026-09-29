#include "pch.h"
#include "A51.h"
#include "app/signs/common_library/src/STISUtility.h"

namespace TA_IRS_App::STIS_PROTOCOL::MESSAGE_DATA_TYPES
{
    using namespace STIS_PROTOCOL::MESSAGE_TYPES;
    using namespace st::fixed_length_data;

    std::ostream& MSGSCH::dump_details(std::ostream& os, int indent) const
    {
        std::string INDENT(indent, ' ');
        os << INDENT << boost::format("MessageTag[%d]: %s\n") % MessageTag.size() % MessageTag.str();
        os << INDENT << boost::format("MessageStartTime[%d]: %s\n") % MessageStartTime.size() % MessageStartTime.str();
        os << INDENT << boost::format("MessageEndTime[%d]: %s\n") % MessageEndTime.size() % MessageEndTime.str();
        os << INDENT << boost::format("MessagePriority[%d]: %s\n") % MessagePriority.size() % MessagePriority.str();
        os << INDENT << boost::format("MessageLength[%d]: %s\n") % MessageText.m_length.size() % MessageText.m_length.str();
        os << INDENT << boost::format("MessageText[%d]: %s\n") % MessageText.m_text.size() % STIS_UTILITY::transform_4_languages_from_utf16_to_utf8(MessageText.m_text, "");
        return os;
    }

    std::ostream& MSGSCHSTNPID::dump_details(std::ostream& os, int indent) const
    {
        std::string INDENT(indent, ' ');
        os << INDENT << boost::format("ReportStation[%d]: %s\n") % ReportStation.size() % ReportStation.str();
        os << INDENT << boost::format("ReportPid[%d]: %s\n") % ReportPid.size() % ReportPid.str();
        os << INDENT << boost::format("CurrentDiaplayTemplate[%d]:\n") % CurrentDiaplayTemplate.size();
        CurrentDiaplayTemplate.dump_details(os, indent + 4);
        os << INDENT << boost::format("MessageStatus[%d]: %s\n") % MessageStatus.size() % MessageStatus.str();
        os << INDENT << boost::format("MessageListCount[%d]: %d\n") % ScheduledDisplayMessages.m_size_length.size() % ScheduledDisplayMessages.m_vector.size();
        os << INDENT << boost::format("MessageList[%d]:\n") % ScheduledDisplayMessages.vector_total_size();

        for (auto& msg : ScheduledDisplayMessages.m_vector)
        {
            os << INDENT + std::string(4, ' ') << msg.MessageTag.str() << std::endl;
            msg.dump_details(os, indent + 4 + 4);
        }

        return os;
    }
}
