#pragma once
#include "GENERALPACKET.h"
#include "app/signs/common_library/src/STISUtility.h"
#include "core/utility/src/core/fixed_length_data/all.h"

namespace TA_IRS_App::STIS_PROTOCOL::MESSAGE_DATA_TYPES
{
    using namespace STIS_PROTOCOL::MESSAGE_TYPES;
    using namespace st::fixed_length_data;

    // CurrentDisplayingMessageTemplateRequest
    struct M50 : EnableFixedLengthUtility<M50>, EnableOperatorGeneralPacket<M50>
    {
        static inline const std::string ID = "M50";

        Byte            STX{0x02};
        Integer<4>      SQN = STIS_UTILITY::next_message_sequence();
        String<14>      Timestamp = STIS_UTILITY::message_timestamp();
        String<3>       MessageID{ID};
        Integer<4>      DataLength;
        String<6>       DestinationStation;
        DigitString<3>  DestinationPID;
        Byte            ETX{0x03};

        auto tied() noexcept
        {
            return std::tie(STX, SQN, Timestamp, MessageID, DataLength, DestinationStation, DestinationPID, ETX);
        }

        auto tied() const noexcept
        {
            return const_cast<M50*>(this)->tied();
        }

        auto data_tied() noexcept
        {
            return std::tie(DestinationStation, DestinationPID);
        }

        auto data_tied() const noexcept
        {
            return const_cast<M50*>(this)->data_tied();
        }

        size_t datasize() const
        {
            return std::apply(FixedLengthSizer(), data_tied());
        }

        M50() = default;

        M50(const std::string& dest, const std::string& pid)
        {
            set(dest, pid);
        }

        M50& set(const std::string& dest, const std::string& pid)
        {
            this->data_tied() = std::tie(dest, pid);
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
            os << INDENT << boost::format("DestinationStation[%d]: %s\n") % DestinationStation.size() % DestinationStation.str();
            os << INDENT << boost::format("DestinationPID[%d]: %s\n") % DestinationPID.size() % DestinationPID.str();
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
