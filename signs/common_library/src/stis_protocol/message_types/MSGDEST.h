#pragma once
#include "core/utility/src/core/fixed_length_data/all.h"

namespace TA_IRS_App::STIS_PROTOCOL::MESSAGE_DATA_TYPES
{
    using namespace st::fixed_length_data;

    // Destination Data
    struct MSGDEST : EnableFixedLengthUtility<MSGDEST>
    {
        using PID = DigitString<3>;
        using PIDLIST = List<PID, SizeIsSize, Integer<3>>;

        String<3>           SystemID;
        String<6>           StationID;
        // Integer<4>       NumberOfPIDs;
        PIDLIST             PIDList;

        auto tied() noexcept
        {
            return std::tie(SystemID, StationID, PIDList);
        }

        auto tied() const noexcept
        {
            return const_cast<MSGDEST*>(this)->tied();
        }

        MSGDEST() = default;

        MSGDEST(const Destination& rhs)
        {
            *this = rhs;
        }

        MSGDEST& operator=(const Destination& rhs)
        {
            clear();
            tied() = rhs.tied();
            return *this;
        }

        std::ostream& dump_details(std::ostream& os, int indent = 0) const
        {
            std::string INDENT(indent, ' ');
            os << INDENT << boost::format("SystemID[%d]: %s\n") % SystemID.size() % SystemID.str();
            os << INDENT << boost::format("StationID[%d]: %s\n") % StationID.size() % StationID.str();
            os << INDENT << boost::format("NumberOfPIDs[%d]: %s\n") % PIDList.m_size_length.size() % PIDList.m_size_length.str();

            if (PIDList.m_vector.size())
            {
                os << INDENT << boost::format("PIDList[%d]:\n") % PIDList.vector_total_size();

                std::for_each(PIDList.m_vector.begin(), PIDList.m_vector.end(), [&](auto& pid)
                {
                    os << INDENT << std::string(4, ' ') << pid.str() << std::endl;
                });
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
}
