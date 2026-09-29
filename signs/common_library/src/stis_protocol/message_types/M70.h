#pragma once
#include "MessageTypes.h"
#include "GENERALPACKET.h"
#include "app/signs/common_library/src/STISUtility.h"
#include "core/utility/src/core/fixed_length_data/all.h"

namespace TA_IRS_App::STIS_PROTOCOL::MESSAGE_DATA_TYPES
{
    using namespace STIS_PROTOCOL::MESSAGE_TYPES;
    using namespace st::fixed_length_data;

    // UpgradePredefinedMessageDisplayTemplateLibraryRequest
    struct M70 : EnableFixedLengthUtility<M70>, EnableOperatorGeneralPacket<M70>
    {
        static inline const std::string ID = "M70";

        Byte                STX{0x02};
        Integer<4>          SQN = STIS_UTILITY::next_message_sequence();
        String<14>          Timestamp = STIS_UTILITY::message_timestamp();
        String<3>           MessageID{ID};
        Integer<4>          DataLength;
        Enum<ELibraryType>  Type = ELibraryType::PredefinedMessage;
        DigitString<3>      Version;
        Byte                ETX{0x03};

        auto tied() noexcept
        {
            return std::tie(STX, SQN, Timestamp, MessageID, DataLength, Type, Version, ETX);
        }

        auto tied() const noexcept
        {
            return const_cast<M70*>(this)->tied();
        }

        auto data_tied() noexcept
        {
            return std::tie(Type, Version);
        }

        auto data_tied() const noexcept
        {
            return const_cast<M70*>(this)->data_tied();
        }

        size_t datasize() const
        {
            return std::apply(FixedLengthSizer(), data_tied());
        }

        M70() = default;

        M70(ELibraryType type, const std::string& version)
        {
            set(type, version);
        }

        M70& set(ELibraryType type, const std::string& version)
        {
            this->Type = type;
            this->Version = version;
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
            os << INDENT << boost::format("Type[%d]: %s\n") % Type.size() % Type.str();
            os << INDENT << boost::format("Version[%d]: %s\n") % Version.size() % Version.str();
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
