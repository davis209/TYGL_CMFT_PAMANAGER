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

    // Scheduled Displaying Template List for PID
    struct TLSCHSTNPID : EnableFixedLengthUtility<TLSCHSTNPID>
    {
        using TLPRELIST = List<TLPRE, SizeIsSize, Integer<2>>;

        String<6>           ReportStation;
        String<3>           ReportPid;
        // Integer<2>       DiaplayTemplateListCount;
        TLPRELIST           ScheduledDiaplayTemplateList;

        auto tied() noexcept
        {
            return std::tie(ReportStation, ReportPid, ScheduledDiaplayTemplateList);
        }

        auto tied() const noexcept
        {
            return const_cast<TLSCHSTNPID*>(this)->tied();
        }

        TLSCHSTNPID() = default;

        TLSCHSTNPID(const ScheduledDisplayMessageTemplateForPid& rhs)
        {
            *this = rhs;
        }

        TLSCHSTNPID& operator=(const ScheduledDisplayTemplateListForPid& rhs)
        {
            clear();
            tied() = rhs.tied();
            return *this;
        }

        operator ScheduledDisplayTemplateListForPid() const
        {
            ScheduledDisplayTemplateListForPid m;
            m.report_station = this->ReportStation;
            m.report_pid = this->ReportPid;
            m.scheduled_display_template_list = this->ScheduledDiaplayTemplateList;
            return m;
        }

        std::ostream& dump_details(std::ostream& os, int indent = 0) const
        {
            std::string INDENT(indent, ' ');
            os << INDENT << boost::format("ReportStation[%d]: %s\n") % ReportStation.size() % ReportStation.str();
            os << INDENT << boost::format("ReportPid[%d]: %s\n") % ReportPid.size() % ReportPid.str();
            os << INDENT << boost::format("ScheduledDiaplayTemplateListCount[%d]: %d\n") % ScheduledDiaplayTemplateList.m_size_length.size() % ScheduledDiaplayTemplateList.m_vector.size();
            os << INDENT << boost::format("ScheduledDiaplayTemplateList[%d]:\n") % ScheduledDiaplayTemplateList.vector_total_size();

            for (auto& tpl : ScheduledDiaplayTemplateList.m_vector)
            {
                os << INDENT + std::string(4, ' ') << tpl.DisplayTemplateID.str() << std::endl;
                tpl.dump_details(os, indent + 4 + 4);
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

    using TLSCHSTNPIDLIST = List<TLSCHSTNPID, SizeIsSize>;

    // Station Scheduled Displaying Message/Template List Report [Station] & [OCC]
    struct A52 : EnableFixedLengthUtility<A52>, EnableOperatorGeneralPacket6<A52>
    {
        static inline const std::string ID = "A52";

        Byte                        STX{0x02};
        Integer<4>                  SQN;
        String<14>                  Timestamp = STIS_UTILITY::message_timestamp();
        String<3>                   MessageID{ID};
        Integer<6>                  DataLength;
        // Integer<4>               MessageListLength;
        TLSCHSTNPIDLIST             ScheduledDisplayingTemplateListOfStationPids;
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
                ScheduledDisplayingTemplateListOfStationPids,
                ETX
            );
        }

        auto tied() const noexcept
        {
            return const_cast<A52*>(this)->tied();
        }

        auto data_tied() noexcept
        {
            return std::tie
            (
                ScheduledDisplayingTemplateListOfStationPids
            );
        }

        auto data_tied() const noexcept
        {
            return const_cast<A52*>(this)->data_tied();
        }

        size_t datasize() const
        {
            return std::apply(FixedLengthSizer(), data_tied());
        }

        operator A52_StationScheduledDisplayingTemplateListReport() const
        {
            A52_StationScheduledDisplayingTemplateListReport a;
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
            os << INDENT << boost::format("ScheduledDisplayingTemplateListOfStationPidsCount[%d]: %d\n") % ScheduledDisplayingTemplateListOfStationPids.m_size_length.size() % ScheduledDisplayingTemplateListOfStationPids.m_vector.size();
            os << INDENT << boost::format("ScheduledDisplayingTemplateListOfStationPids[%d]:\n") % ScheduledDisplayingTemplateListOfStationPids.vector_total_size();

            for (auto& msg : ScheduledDisplayingTemplateListOfStationPids.m_vector)
            {
                os << INDENT + std::string(4, ' ') << msg.ReportStation.str() << msg.ReportPid.str() << std::endl;
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
