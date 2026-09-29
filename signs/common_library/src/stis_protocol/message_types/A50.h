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

    // Current Displaying Message/Template
    struct MSGCUR : EnableFixedLengthUtility<MSGCUR>
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
            return const_cast<MSGCUR*>(this)->tied();
        }

        MSGCUR() = default;

        MSGCUR(const CurrentDisplayMessage& rhs)
        {
            *this = rhs;
        }

        MSGCUR& operator=(const CurrentDisplayMessage& rhs)
        {
            clear();
            tied() = rhs.tied();
            return *this;
        }

        operator CurrentDisplayMessage() const
        {
            CurrentDisplayMessage m;
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

    using MSGCURLIST = List<MSGCUR, SizeIsSize, Integer<2>>;

    // CurrentDisplayingMessageTemplateReport
    struct A50 : EnableFixedLengthUtility<A50>, EnableOperatorGeneralPacket<A50>
    {
        static inline const std::string ID = "A50";

        Byte                        STX{0x02};
        Integer<4>                  SQN;
        String<14>                  Timestamp = STIS_UTILITY::message_timestamp();
        String<3>                   MessageID{ID};
        Integer<4>                  DataLength;
        String<6>                   ReportStation;
        DigitString<3>              ReportPID;
        TLPRE                       CurrentDiaplayTemplate;
        DigitString<2>              MessageStatus;
        // Note 2:
        // The Field from Message Tag to Message Text will be repeated if more than one current active message is scheduled for the display.
        // Integer<2>               MessageListLength;
        MSGCURLIST                  CurrentDisplayMessages;
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
                ReportStation,
                ReportPID,
                CurrentDiaplayTemplate,
                MessageStatus,
                CurrentDisplayMessages,
                ETX
            );
        }

        auto tied() const noexcept
        {
            return const_cast<A50*>(this)->tied();
        }

        auto data_tied() noexcept
        {
            return std::tie
            (
                ReportStation,
                ReportPID,
                CurrentDiaplayTemplate,
                MessageStatus,
                CurrentDisplayMessages
            );
        }

        auto data_tied() const noexcept
        {
            return const_cast<A50*>(this)->data_tied();
        }

        size_t datasize() const
        {
            return std::apply(FixedLengthSizer(), data_tied());
        }

        operator A50_CurrentDisplayMessageTemplateReport() const
        {
            A50_CurrentDisplayMessageTemplateReport a;
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
            os << INDENT << boost::format("ReportStation[%d]: %s\n") % ReportStation.size() % ReportStation.str();
            os << INDENT << boost::format("ReportPID[%d]: %s\n") % ReportPID.size() % ReportPID.str();
            os << INDENT << boost::format("CurrentDiaplayTemplate[%d]:\n") % CurrentDiaplayTemplate.size();
            CurrentDiaplayTemplate.dump_details(os, indent + 4);
            os << INDENT << boost::format("MessageStatus[%d]: %s\n") % MessageStatus.size() % MessageStatus.str();
            os << INDENT << boost::format("CurrentDisplayMessages[%d]:\n") % CurrentDisplayMessages.vector_total_size();

            for (auto& msg : CurrentDisplayMessages.m_vector)
            {
                os << INDENT + std::string(4, ' ') << msg.MessageTag.str() << std::endl;
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
