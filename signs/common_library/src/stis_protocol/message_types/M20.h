#pragma once
#include "MessageTypes.h"
#include "GENERALPACKET.h"
#include "MSGDEST.h"
#include "app/signs/common_library/src/STISUtility.h"
#include "core/utility/src/core/fixed_length_data/all.h"

namespace TA_IRS_App::STIS_PROTOCOL::MESSAGE_DATA_TYPES
{
    using namespace STIS_PROTOCOL::MESSAGE_TYPES;
    using namespace st::fixed_length_data;

    // ClearCurrentMessagesRequest
    struct M20 : EnableFixedLengthUtility<M20>, EnableOperatorGeneralPacket<M20>
    {
        static inline const std::string ID = "M20";

        Byte            STX{0x02};
        Integer<4>      SQN = STIS_UTILITY::next_message_sequence();
        String<14>      Timestamp = STIS_UTILITY::message_timestamp();
        String<3>       MessageID{ID};
        Integer<4>      DataLength;
        MSGDEST         Destination;
        Integer<1>      Priority1;
        Integer<1>      Priority2;
        Integer<1>      Priority3;
        Integer<1>      Priority4;
        Integer<1>      Priority5;
        Integer<1>      Priority6;
        Integer<1>      Priority7;
        Integer<1>      Priority8;
        Byte            ETX{0x03};

        auto tied() noexcept
        {
            return std::tie
            (
                STX, SQN, Timestamp, MessageID, DataLength, Destination,
                Priority1, Priority2, Priority3, Priority4, Priority5, Priority6, Priority7, Priority8, ETX
            );
        }

        auto tied() const noexcept
        {
            return const_cast<M20*>(this)->tied();
        }

        auto data_tied() noexcept
        {
            return std::tie(Destination, Priority1, Priority2, Priority3, Priority4, Priority5, Priority6, Priority7, Priority8);
        }

        auto data_tied() const noexcept
        {
            return const_cast<M20*>(this)->data_tied();
        }

        size_t datasize() const
        {
            return std::apply(FixedLengthSizer(), data_tied());
        }

        M20() = default;

        M20(const MESSAGE_TYPES::Destination& dest, const std::vector<int>& priority)
        {
            set(dest, priority);
        }

        M20& set(const MESSAGE_TYPES::Destination& dest, const std::vector<int>& priority)
        {
            this->data_tied() = std::tie(dest, priority[0], priority[1], priority[2], priority[3], priority[4], priority[5], priority[6], priority[7]);
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
            os << INDENT << boost::format("Priority1[%d]: %s\n") % Priority1.size() % Priority1.str();
            os << INDENT << boost::format("Priority2[%d]: %s\n") % Priority2.size() % Priority2.str();
            os << INDENT << boost::format("Priority3[%d]: %s\n") % Priority3.size() % Priority3.str();
            os << INDENT << boost::format("Priority4[%d]: %s\n") % Priority4.size() % Priority4.str();
            os << INDENT << boost::format("Priority5[%d]: %s\n") % Priority5.size() % Priority5.str();
            os << INDENT << boost::format("Priority6[%d]: %s\n") % Priority6.size() % Priority6.str();
            os << INDENT << boost::format("Priority7[%d]: %s\n") % Priority7.size() % Priority7.str();
            os << INDENT << boost::format("Priority8[%d]: %s\n") % Priority8.size() % Priority8.str();
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
