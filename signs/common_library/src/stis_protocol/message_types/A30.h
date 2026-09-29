#pragma once
#include "MessageTypes.h"
#include "GENERALPACKET.h"
#include "app/signs/common_library/src/STISUtility.h"
#include "core/utility/src/core/fixed_length_data/all.h"

namespace TA_IRS_App::STIS_PROTOCOL::MESSAGE_DATA_TYPES
{
    using namespace STIS_PROTOCOL::MESSAGE_TYPES;
    using namespace st::fixed_length_data;

    // StationSTISStatusReport
    struct A30 : EnableFixedLengthUtility<A30>, EnableOperatorGeneralPacket<A30>
    {
        static inline const std::string ID = "A30";

        using NUMBEROFPIDS = Integer<2>;
        using PIDID = DigitString<3>;
        using PIDSTATUS = Enum<EPIDStatus>;
        using PIDIDSTATUS = Pair<PIDID, PIDSTATUS>;
        using PIDSTATUSLIST = List<PIDIDSTATUS, SizeIsSize, NUMBEROFPIDS>;

        Byte                            STX{0x02};
        Integer<4>                      SQN;
        String<14>                      Timestamp = STIS_UTILITY::message_timestamp();
        String<3>                       MessageID{ID};
        Integer<4>                      DataLength;
        String<6>                       ReportStation;
        Enum<ELANConnectionLinkStatus>  LANConnectionLinkStatus = ELANConnectionLinkStatus::Normal;
        Enum<EAlarmSummary>             AlarmSummary = EAlarmSummary::Normal;
        Integer<1>                      Spare{0};
        DigitString<3>                  CurrentSTISLibraryVersion;
        DigitString<3>                  NextSTISLibraryVersion;
        DigitString<3>                  CurrentSTISDisplayTemplateVersion;
        DigitString<3>                  NextSTISDisplayTemplateVersion;
        // NUMBEROFPIDS                 NumberOfPIDs;
        PIDSTATUSLIST                   PIDStatusList;
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
                ReportStation,
                LANConnectionLinkStatus,
                AlarmSummary,
                Spare,
                CurrentSTISLibraryVersion,
                NextSTISLibraryVersion,
                CurrentSTISDisplayTemplateVersion,
                NextSTISDisplayTemplateVersion,
                // NumberOfPIDs,
                PIDStatusList,
                ETX
            );
        }

        auto tied() const noexcept
        {
            return const_cast<A30*>(this)->tied();
        }

        auto versions_tied()
        {
            return std::tie(CurrentSTISLibraryVersion, NextSTISLibraryVersion, CurrentSTISDisplayTemplateVersion, NextSTISDisplayTemplateVersion);
        }

        auto versions_tuple() const
        {
            return std::make_tuple(CurrentSTISLibraryVersion.str(),
                                   NextSTISLibraryVersion.str(),
                                   CurrentSTISDisplayTemplateVersion.str(),
                                   NextSTISDisplayTemplateVersion.str());
        }

        auto data_tied() noexcept
        {
            return std::tie
            (
                ReportStation,
                LANConnectionLinkStatus,
                AlarmSummary,
                Spare,
                CurrentSTISLibraryVersion,
                NextSTISLibraryVersion,
                CurrentSTISDisplayTemplateVersion,
                NextSTISDisplayTemplateVersion,
                // NumberOfPIDs,
                PIDStatusList
            );
        }

        auto data_tied() const noexcept
        {
            return const_cast<A30*>(this)->data_tied();
        }

        size_t datasize() const
        {
            return std::apply(FixedLengthSizer(), data_tied());
        }

        operator A30_StationSTISStatusReport() const
        {
            A30_StationSTISStatusReport a;
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
            os << INDENT << boost::format("LANConnectionLinkStatus[%d]: %s\n") % LANConnectionLinkStatus.size() % LANConnectionLinkStatus.str();
            os << INDENT << boost::format("AlarmSummary[%d]: %s\n") % AlarmSummary.size() % AlarmSummary.str();
            os << INDENT << boost::format("Spare[%d]: %s\n") % Spare.size() % Spare.str();
            os << INDENT << boost::format("CurrentSTISLibraryVersion[%d]: %s\n") % CurrentSTISLibraryVersion.size() % CurrentSTISLibraryVersion.str();
            os << INDENT << boost::format("NextSTISLibraryVersion[%d]: %s\n") % NextSTISLibraryVersion.size() % NextSTISLibraryVersion.str();
            os << INDENT << boost::format("CurrentSTISDisplayTemplateVersion[%d]: %s\n") % CurrentSTISDisplayTemplateVersion.size() % CurrentSTISDisplayTemplateVersion.str();
            os << INDENT << boost::format("NextSTISDisplayTemplateVersion[%d]: %s\n") % NextSTISDisplayTemplateVersion.size() % NextSTISDisplayTemplateVersion.str();
            os << INDENT << boost::format("NumberOfPIDs[%d]: %s\n") % PIDStatusList.m_size_length.size() % PIDStatusList.m_size_length.str();

            if (PIDStatusList.m_vector.size())
            {
                os << INDENT << boost::format("PIDStatusList[%d]:\n") % PIDStatusList.vector_total_size();

                for (auto&& id_status : PIDStatusList.m_vector)
                {
                    os << INDENT + std::string(4, ' ') << boost::format("PIDID[%d]: %s\n") % id_status.first.size() % id_status.first.str();
                    os << INDENT + std::string(4, ' ') << boost::format("PIDStatus[%d]: %s\n") % id_status.second.size() % id_status.second.str();
                }
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
