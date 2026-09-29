#pragma once
#include "MessageTypes.h"
#include "GENERALPACKET.h"
#include "STATIONSTATUSDETAIL.h"
#include "app/signs/common_library/src/STISUtility.h"
#include "core/utility/src/core/fixed_length_data/all.h"

namespace TA_IRS_App::STIS_PROTOCOL::MESSAGE_DATA_TYPES
{
    using namespace STIS_PROTOCOL::MESSAGE_TYPES;
    using namespace st::fixed_length_data;

    // AllStationSTISStatusReport
    struct A32 : EnableFixedLengthUtility<A32>, EnableOperatorGeneralPacket<A32>
    {
        static inline const std::string ID = "A32";

        Byte                        STX{0x02};
        Integer<4>                  SQN;
        String<14>                  Timestamp = STIS_UTILITY::message_timestamp();
        String<3>                   MessageID{ID};
        // Integer<4>               DataLength;
        ALLSTATIONSTATUSDETAILS     AllStationStatusDetails;
        Byte                        ETX{0x03};

        auto tied() noexcept
        {
            return std::tie
            (
                STX,
                SQN,
                Timestamp,
                MessageID,
                // DataLength,
                AllStationStatusDetails,
                ETX
            );
        }

        auto tied() const noexcept
        {
            return const_cast<A32*>(this)->tied();
        }

        size_t datasize() const
        {
            return AllStationStatusDetails.size();
        }

        operator A32_AllStationStatusDetails() const
        {
            A32_AllStationStatusDetails a;

            for (auto& v : this->AllStationStatusDetails.m_vector)
            {
                a.emplace_back(v);
            }

            return a;
        }

        std::ostream& dump_details(std::ostream& os, int indent = 0) const
        {
            std::string INDENT(indent, ' ');
            os << INDENT << boost::format("STX[%d]: %s\n") % STX.size() % STX.str();
            os << INDENT << boost::format("SQN[%d]: %s\n") % SQN.size() % SQN.str();
            os << INDENT << boost::format("Timestamp[%d]: %s\n") % Timestamp.size() % Timestamp.str();
            os << INDENT << boost::format("MessageID[%d]: %s\n") % MessageID.size() % MessageID.str();
            os << INDENT << boost::format("DataLength[%d]: %s\n") % AllStationStatusDetails.m_size_length.size() % AllStationStatusDetails.m_size_length.str();
            os << INDENT << boost::format("AllStationStatusDetails[%d]:\n") % AllStationStatusDetails.vector_total_size();

            for (auto& station : AllStationStatusDetails.m_vector)
            {
                os << INDENT + std::string(4, ' ') << station.ReportStation.str() << std::endl;
                station.dump_details(os, indent + 4 + 4);
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
