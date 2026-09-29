#pragma once
#include "MessageTypes.h"
#include "core/utility/src/core/fixed_length_data/all.h"

namespace TA_IRS_App::STIS_PROTOCOL::MESSAGE_DATA_TYPES
{
    using namespace STIS_PROTOCOL::MESSAGE_TYPES;
    using namespace st::fixed_length_data;

    struct STATIONSTATUSDETAIL : EnableFixedLengthUtility<STATIONSTATUSDETAIL>
    {
        using NUMBEROFPIDS = Integer<2>;
        using PIDID = DigitString<3>;
        using PIDSTATUS = Enum<EPIDStatus>;
        using PIDIDSTATUS = Pair<PIDID, PIDSTATUS>;
        using PIDSTATUSLIST = List<PIDIDSTATUS, SizeIsSize, NUMBEROFPIDS>;

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

        auto tied() noexcept
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

        auto tied() const noexcept
        {
            return const_cast<STATIONSTATUSDETAIL*>(this)->tied();
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

        STATIONSTATUSDETAIL() = default;

        STATIONSTATUSDETAIL(const A30_StationSTISStatusReport& rhs)
        {
            *this = rhs;
        }

        STATIONSTATUSDETAIL& operator=(const A30_StationSTISStatusReport& rhs)
        {
            clear();
            tied() = rhs.tied();
            return *this;
        }

        operator A30_StationSTISStatusReport() const
        {
            A30_StationSTISStatusReport a;
            a.report_station = this->ReportStation;
            a.lan_connection_link_status = this->LANConnectionLinkStatus;
            a.alarm_summary = this->AlarmSummary;
            a.versions.tied() = this->versions_tuple();
            // lhs.number_of_pids = rhs.NumberOfPIDs.as<size_t>();
            a.pid_status_list = this->PIDStatusList;
            return a;
        }

        std::ostream& dump_details(std::ostream& os, int indent = 0) const
        {
            std::string INDENT(indent, ' ');
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

            return os;
        }

        std::string dump_details(int indent = 0) const
        {
            std::stringstream ss;
            dump_details(ss, indent);
            return ss.str();
        }
    };

    using ALLSTATIONSTATUSDETAILS = List<STATIONSTATUSDETAIL, SizeIsLength>;
}
