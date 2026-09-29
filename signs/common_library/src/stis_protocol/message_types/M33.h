#pragma once
#include "MessageTypes.h"
#include "GENERALPACKET.h"
#include "app/signs/common_library/src/STISUtility.h"
#include "core/utility/src/core/fixed_length_data/all.h"
#include <sstream>

namespace TA_IRS_App::STIS_PROTOCOL::MESSAGE_DATA_TYPES
{
    using namespace STIS_PROTOCOL::MESSAGE_TYPES;
    using namespace st::fixed_length_data;

    // M33: OCC STIS Status Sync Request [OCC]
    struct M33 : EnableFixedLengthUtility<M33>, EnableOperatorGeneralPacket<M33>
    {
        static inline const std::string ID = "M33";

        Byte            STX{0x02};
        Integer<4>      SQN = STIS_UTILITY::next_message_sequence();
        String<14>      Timestamp = STIS_UTILITY::message_timestamp();
        String<3>       MessageID{ID};
        Integer<4>      DataLength;
        String<6>       DestinationOCC;
        DigitString<3>  CurrentSTISLibraryVersion;
        DigitString<3>  NextSTISLibraryVersion;
        DigitString<3>  CurrentSTISDisplayVersion;
        DigitString<3>  NextSTISDisplayVersion;
        Byte            ETX{0x03};

        auto tied() noexcept
        {
            return std::tie
            (
                STX,
                SQN,
                Timestamp,
                MessageID,
                DataLength,
                DestinationOCC,
                CurrentSTISLibraryVersion,
                NextSTISLibraryVersion,
                CurrentSTISDisplayVersion,
                NextSTISDisplayVersion,
                ETX
            );
        }

        auto tied() const noexcept
        {
            return const_cast<M33*>(this)->tied();
        }

        auto versions_tied()
        {
            return std::tie(CurrentSTISLibraryVersion, NextSTISLibraryVersion, CurrentSTISDisplayVersion, NextSTISDisplayVersion);
        }

        auto data_tied() noexcept
        {
            return std::tie(DestinationOCC, CurrentSTISLibraryVersion, NextSTISLibraryVersion, CurrentSTISDisplayVersion, NextSTISDisplayVersion);
        }

        auto data_tied() const noexcept
        {
            return const_cast<M33*>(this)->data_tied();
        }

        size_t datasize() const
        {
            return std::apply(FixedLengthSizer(), data_tied());
        }

        M33() = default;

        M33(const std::string& dest, const CurNxtMsgTmpLibVers& versions)
        {
            set(dest, versions);
        }

        M33& set(const std::string& dest, const CurNxtMsgTmpLibVers& versions)
        {
            this->DestinationOCC = dest;
            this->versions_tied() = versions.tied();
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
            os << INDENT << boost::format("DestinationOCC[%d]: %s\n") % DestinationOCC.size() % DestinationOCC.str();
            os << INDENT << boost::format("CurrentSTISLibraryVersion[%d]: %s\n") % CurrentSTISLibraryVersion.size() % CurrentSTISLibraryVersion.str();
            os << INDENT << boost::format("NextSTISLibraryVersion[%d]: %s\n") % NextSTISLibraryVersion.size() % NextSTISLibraryVersion.str();
            os << INDENT << boost::format("CurrentSTISDisplayVersion[%d]: %s\n") % CurrentSTISDisplayVersion.size() % CurrentSTISDisplayVersion.str();
            os << INDENT << boost::format("NextSTISDisplayVersion[%d]: %s\n") % NextSTISDisplayVersion.size() % NextSTISDisplayVersion.str();
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
