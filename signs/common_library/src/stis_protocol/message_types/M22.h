#pragma once
#include "MessageTypes.h"
#include "GENERALPACKET.h"
#include "MSGDEST.h"
#include "TLPRE.h"
#include "app/signs/common_library/src/STISUtility.h"
#include "core/utility/src/core/fixed_length_data/all.h"

namespace TA_IRS_App::STIS_PROTOCOL::MESSAGE_DATA_TYPES
{
    using namespace STIS_PROTOCOL::MESSAGE_TYPES;
    using namespace st::fixed_length_data;

    // SendPredefinedDisplayTemplateRequest
    struct M22 : EnableFixedLengthUtility<M22>, EnableOperatorGeneralPacket<M22>
    {
        static inline const std::string ID = "M22";

        Byte            STX{0x02};
        Integer<4>      SQN = STIS_UTILITY::next_message_sequence();
        String<14>      Timestamp = STIS_UTILITY::message_timestamp();
        String<3>       MessageID{ID};
        Integer<4>      DataLength;
        MSGDEST         Destination;
        TLPRE           LCDDisplayTemplate;
        TLPRE           Spare;  // Reserved for future (no LED for TYGL CMFT)
        Byte            ETX{0x03};

        auto tied() noexcept
        {
            return std::tie(STX, SQN, Timestamp, MessageID, DataLength, Destination, LCDDisplayTemplate, Spare, ETX);
        }

        auto tied() const noexcept
        {
            return const_cast<M22*>(this)->tied();
        }

        auto data_tied() noexcept
        {
            return std::tie(Destination, LCDDisplayTemplate, Spare);
        }

        auto data_tied() const noexcept
        {
            return const_cast<M22*>(this)->data_tied();
        }

        size_t datasize() const
        {
            return std::apply(FixedLengthSizer(), data_tied());
        }

        M22() = default;

        M22(const MESSAGE_TYPES::Destination& dest, const PredefinedDisplayTemplate& lcd, const PredefinedDisplayTemplate& spare = {})
        {
            set(dest, lcd, spare);
        }

        M22& set(const MESSAGE_TYPES::Destination& dest, const PredefinedDisplayTemplate& lcd, const PredefinedDisplayTemplate& spare = {})
        {
            this->data_tied() = std::tie(dest, lcd, spare);
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
            os << INDENT << boost::format("LCDDisplayTemplate[%d]:\n") % LCDDisplayTemplate.size();
            LCDDisplayTemplate.dump_details(os, indent + 4);
            os << INDENT << boost::format("Spare[%d]:\n") % Spare.size();
            Spare.dump_details(os, indent + 4);
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
