#pragma once
#include "MessageTypes.h"
#include "GENERALPACKET.h"
#include "app/signs/common_library/src/STISUtility.h"
#include "core/utility/src/core/fixed_length_data/all.h"

namespace TA_IRS_App::STIS_PROTOCOL::MESSAGE_DATA_TYPES
{
    using namespace STIS_PROTOCOL::MESSAGE_TYPES;
    using namespace st::fixed_length_data;

    // A33: OCC STIS Status Sync Report [OCC]
    struct A33 : EnableFixedLengthUtility<A33>, EnableOperatorGeneralPacket<A33>
    {
        static inline const std::string ID = "A33";

        Byte                            STX{0x02};
        Integer<4>                      SQN;
        String<14>                      Timestamp = STIS_UTILITY::message_timestamp();
        String<3>                       MessageID{ID};
        Integer<4>                      DataLength;
        Enum<ELANConnectionLinkStatus>  LANConnectionLinkStatus = ELANConnectionLinkStatus::Normal;
        Enum<EAlarmSummary>             AlarmSummary = EAlarmSummary::Normal;
        Integer<1>                      Spare{0};
        DigitString<3>                  CurrentSTISMessageLibraryVersion;
        DigitString<3>                  NextSTISMessageLibraryVersion;
        DigitString<3>                  CurrentSTISDisplayTemplateVersion;
        DigitString<3>                  NextSTISDisplayTemplateVersion;
        Byte                            ETX{0x03};

        auto tied() noexcept
        {
            return std::tie
            (
                STX,
                SQN,
                Timestamp,
                MessageID,
                DataLength,
                LANConnectionLinkStatus,
                AlarmSummary,
                Spare,
                CurrentSTISMessageLibraryVersion,
                NextSTISMessageLibraryVersion,
                CurrentSTISDisplayTemplateVersion,
                NextSTISDisplayTemplateVersion,
                ETX
            );
        }

        auto tied() const noexcept
        {
            return const_cast<A33*>(this)->tied();
        }

        auto versions_tied()
        {
            return std::tie
            (
                CurrentSTISMessageLibraryVersion,
                NextSTISMessageLibraryVersion,
                CurrentSTISDisplayTemplateVersion,
                NextSTISDisplayTemplateVersion
            );
        }

        auto versions_tuple() const
        {
            return std::make_tuple
            (
                CurrentSTISMessageLibraryVersion.str(),
                NextSTISMessageLibraryVersion.str(),
                CurrentSTISDisplayTemplateVersion.str(),
                NextSTISDisplayTemplateVersion.str()
            );
        }

        auto data_tied() noexcept
        {
            return std::tie
            (
                LANConnectionLinkStatus,
                AlarmSummary,
                Spare,
                CurrentSTISMessageLibraryVersion,
                NextSTISMessageLibraryVersion,
                CurrentSTISDisplayTemplateVersion,
                NextSTISDisplayTemplateVersion
            );
        }

        auto data_tied() const noexcept
        {
            return const_cast<A33*>(this)->data_tied();
        }

        size_t datasize() const
        {
            return std::apply(FixedLengthSizer(), data_tied());
        }

        operator A33_OCCSTISStatusSyncReport() const
        {
            A33_OCCSTISStatusSyncReport a;
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
            os << INDENT << boost::format("LANConnectionLinkStatus[%d]: %s\n") % LANConnectionLinkStatus.size() % LANConnectionLinkStatus.str();
            os << INDENT << boost::format("AlarmSummary[%d]: %s\n") % AlarmSummary.size() % AlarmSummary.str();
            os << INDENT << boost::format("Spare[%d]: %s\n") % Spare.size() % Spare.str();
            os << INDENT << boost::format("CurrentSTISMessageLibraryVersion[%d]: %s\n") % CurrentSTISMessageLibraryVersion.size() % CurrentSTISMessageLibraryVersion.str();
            os << INDENT << boost::format("NextSTISMessageLibraryVersion[%d]: %s\n") % NextSTISMessageLibraryVersion.size() % NextSTISMessageLibraryVersion.str();
            os << INDENT << boost::format("CurrentSTISDisplayTemplateVersion[%d]: %s\n") % CurrentSTISDisplayTemplateVersion.size() % CurrentSTISDisplayTemplateVersion.str();
            os << INDENT << boost::format("NextSTISDisplayTemplateVersion[%d]: %s\n") % NextSTISDisplayTemplateVersion.size() % NextSTISDisplayTemplateVersion.str();
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
