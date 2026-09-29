#pragma once
#include "MessageTypes.h"
#include "GENERALPACKET.h"
#include "TLPRE.h"
#include "app/signs/common_library/src/STISUtility.h"
#include "core/utility/src/core/fixed_length_data/all.h"

namespace TA_IRS_App::STIS_PROTOCOL::MESSAGE_DATA_TYPES
{
    using namespace STIS_PROTOCOL::MESSAGE_TYPES;
    using namespace st::fixed_length_data;

    // Scheduled Displaying Message
    struct MSGSCH : EnableFixedLengthUtility<MSGSCH>
    {
        using MESSAGETEXT = UnicodeText;

        String<12>      MessageTag;
        String<14>      MessageStartTime;
        String<14>      MessageEndTime;
        Integer<1>      MessagePriority;
        // Integer<4>   MessageLength;
        MESSAGETEXT     MessageText;

        auto tied() noexcept
        {
            return std::tie(MessageTag, MessageStartTime, MessageEndTime, MessagePriority, /*MessageLength,*/ MessageText);
        }

        auto tied() const noexcept
        {
            return const_cast<MSGSCH*>(this)->tied();
        }

        MSGSCH() = default;

        MSGSCH(const ScheduledDisplayMessage& rhs)
        {
            *this = rhs;
        }

        MSGSCH& operator=(const ScheduledDisplayMessage& rhs)
        {
            clear();
            tied() = rhs.tied();
            return *this;
        }

        operator ScheduledDisplayMessage() const
        {
            ScheduledDisplayMessage m;
            m.message_tag = this->MessageTag;
            m.message_start_time = this->MessageStartTime;
            m.message_end_time = this->MessageEndTime;
            m.message_priority = this->MessagePriority;
            m.message_text = this->MessageText;
            return m;
        }

        std::ostream& dump_details(std::ostream& os, int indent = 0) const;

        std::string dump_details(int indent = 0) const
        {
            std::stringstream ss;
            dump_details(ss, indent);
            return ss.str();
        }
    };

    using MSGSCHLIST = List<MSGSCH, SizeIsSize, Integer<2>>;

    // Scheduled Displaying Message/Template List for PID
    struct MSGSCHSTNPID : EnableFixedLengthUtility<MSGSCHSTNPID>
    {
        String<6>           ReportStation;
        String<3>           ReportPid;
        TLPRE               CurrentDiaplayTemplate;
        DigitString<2>      MessageStatus;
        // Integer<2>       MessageListLength;
        MSGSCHLIST          ScheduledDisplayMessages;

        auto tied() noexcept
        {
            return std::tie(ReportStation, ReportPid, CurrentDiaplayTemplate, MessageStatus, ScheduledDisplayMessages);
        }

        auto tied() const noexcept
        {
            return const_cast<MSGSCHSTNPID*>(this)->tied();
        }

        MSGSCHSTNPID() = default;

        MSGSCHSTNPID(const ScheduledDisplayMessageTemplateForPid& rhs)
        {
            *this = rhs;
        }

        MSGSCHSTNPID& operator=(const ScheduledDisplayMessageTemplateForPid& rhs)
        {
            clear();
            tied() = rhs.tied();
            return *this;
        }

        operator ScheduledDisplayMessageTemplateForPid() const
        {
            ScheduledDisplayMessageTemplateForPid m;
            m.report_station = this->ReportStation;
            m.report_pid = this->ReportPid;
            m.current_display_template = this->CurrentDiaplayTemplate;
            m.message_status = this->MessageStatus;
            m.scheduled_display_messages = this->ScheduledDisplayMessages;
            return m;
        }

        std::ostream& dump_details(std::ostream& os, int indent = 0) const;

        std::string dump_details(int indent = 0) const
        {
            std::stringstream ss;
            dump_details(ss, indent);
            return ss.str();
        }
    };

    using MSGSCHSTNPIDLIST = List<MSGSCHSTNPID, SizeIsSize>;

    // Station Scheduled Displaying Message/Template List Report [Station] & [OCC]
    struct A51 : EnableFixedLengthUtility<A51>, EnableOperatorGeneralPacket6<A51>
    {
        static inline const std::string ID = "A51";

        Byte                        STX{0x02};
        Integer<4>                  SQN;
        String<14>                  Timestamp = STIS_UTILITY::message_timestamp();
        String<3>                   MessageID{ID};
        Integer<6>                  DataLength;
        // Integer<4>               MessageListLength;
        MSGSCHSTNPIDLIST            ScheduledDisplayingMessageTemplateOfStationPids;
        Byte                        ETX{0x03};

        auto tied() noexcept
        {
            return std::tie
            (
                STX,
                SQN,
                Timestamp,
                MessageID,
                DataLength,
                ScheduledDisplayingMessageTemplateOfStationPids,
                ETX
            );
        }

        auto tied() const noexcept
        {
            return const_cast<A51*>(this)->tied();
        }

        auto data_tied() noexcept
        {
            return std::tie
            (
                ScheduledDisplayingMessageTemplateOfStationPids
            );
        }

        auto data_tied() const noexcept
        {
            return const_cast<A51*>(this)->data_tied();
        }

        size_t datasize() const
        {
            return std::apply(FixedLengthSizer(), data_tied());
        }

        operator A51_StationScheduledDisplayingMessageTemplateListReport() const
        {
            A51_StationScheduledDisplayingMessageTemplateListReport a;
            a.tied() = this->data_tied();
            return a;
        }

        std::ostream& dump_details(std::ostream& os, int indent = 0) const
        {
            std::string INDENT(indent, ' ');
            os << INDENT << boost::format("STX[%d]: %s\n") % STX.size() % STX.str();
            os << INDENT << boost::format("SQN[%d]: %s\n") % SQN.size() % SQN.str();
            os << INDENT << boost::format("Timestamp[%d]: %s\n") % Timestamp.size() % Timestamp.str();
            os << INDENT << boost::format("MessageID[%d]: %s\n") % MessageID.size() % MessageID.str();
            os << INDENT << boost::format("DataLength[%d]: %s\n") % DataLength.size() % DataLength.str();
            os << INDENT << boost::format("ScheduledDisplayingMessageTemplateOfStationPidsCount[%d]: %d\n") % ScheduledDisplayingMessageTemplateOfStationPids.m_size_length.size() % ScheduledDisplayingMessageTemplateOfStationPids.m_vector.size();
            os << INDENT << boost::format("ScheduledDisplayingMessageTemplateOfStationPids[%d]:\n") % ScheduledDisplayingMessageTemplateOfStationPids.vector_total_size();

            for (auto& msg : ScheduledDisplayingMessageTemplateOfStationPids.m_vector)
            {
                os << INDENT + std::string(4, ' ') << msg.ReportStation.str() << msg.ReportPid.str() << std::endl;
                msg.dump_details(os, indent + 4 + 4);
            }

            os << INDENT << boost::format("ETX[%d]: %s\n") % ETX.size() % ETX.str();
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
