#pragma once
#include "MessageTypes.h"
#include "GENERALPACKET.h"
#include "MSGDEST.h"
#include "MSGPRE.h"
#include "app/signs/common_library/src/STISUtility.h"
#include "core/utility/src/core/fixed_length_data/all.h"

namespace TA_IRS_App::STIS_PROTOCOL::MESSAGE_DATA_TYPES
{
    using namespace STIS_PROTOCOL::MESSAGE_TYPES;
    using namespace st::fixed_length_data;

    // DisplayOfPredefinedMessageRequest
    struct M10 : EnableFixedLengthUtility<M10>, EnableOperatorGeneralPacket<M10>
    {
        static inline const std::string ID = "M10";

        Byte            STX{0x02};
        Integer<4>      SQN = STIS_UTILITY::next_message_sequence();
        String<14>      Timestamp = STIS_UTILITY::message_timestamp();
        String<3>       MessageID{ID};
        Integer<4>      DataLength;
        MSGDEST         Destination;
        MSGPRE          Message;
        Byte            ETX{0x03};

        auto tied() noexcept
        {
            return std::tie(STX, SQN, Timestamp, MessageID, DataLength, Destination, Message, ETX);
        }

        auto tied() const noexcept
        {
            return const_cast<M10*>(this)->tied();
        }

        auto data_tied() noexcept
        {
            return std::tie(Destination, Message);
        }

        auto data_tied() const noexcept
        {
            return const_cast<M10*>(this)->data_tied();
        }

        size_t datasize() const
        {
            return std::apply(FixedLengthSizer(), data_tied());
        }

        M10() = default;

        M10(const MESSAGE_TYPES::Destination& dest, const PredefinedMessage& msg)
        {
            set(dest, msg);
        }

        M10& set(const MESSAGE_TYPES::Destination& dest, const PredefinedMessage& msg)
        {
            this->data_tied() = std::tie(dest, msg);
            this->DataLength = datasize();
            return *this;
        }

        std::ostream& dump_details(std::ostream& os, int indent = 0) const
        {
            std::string INDENT(indent, ' ');
            os << INDENT << boost::format("STX[%d]: %s\n") % STX.size() % STX.str();
            os << INDENT << boost::format("SQN[%d]: %s\n") % SQN.size() % SQN.str();
            os << INDENT << boost::format("Timestamp[%d]: %s\n") % Timestamp.size() % Timestamp.str();
            os << INDENT << boost::format("MessageID[%d]: %s\n") % MessageID.size() % MessageID.str();
            os << INDENT << boost::format("DataLength[%d]: %s\n") % DataLength.size() % DataLength.str();
            os << INDENT << boost::format("Destination[%d]:\n") % Destination.size();
            Destination.dump_details(os, indent + 4);
            os << INDENT << boost::format("Message[%d]:\n") % Message.size();
            Message.dump_details(os, indent + 4);
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
