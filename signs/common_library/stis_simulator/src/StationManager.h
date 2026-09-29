#pragma once
#include "Station.h"
#include "STISStatus.h"
#include "core/utility/src/core/type_traits/stl.h"
#include "core/utility/src/core/Map.h"
#include "core/utility/src/core/StdEx.h"
#include "app/signs/common_library/src/stis_protocol/message_types/MessageDataTypes.h"

namespace TA_IRS_App::STIS_PROTOCOL::SIMULATOR::stissimulator::detail
{
    using namespace MESSAGE_TYPES;
    using namespace MESSAGE_DATA_TYPES;
    namespace sf = st::thread_safe;
    using st::typetraits2::is_decayed_same_any_v;
    using Versions = std::tuple<std::string, std::string, std::string, std::string>;

    struct StationManager
    {
        static StationManager& instance();

        StationManager(STISStatusPtr stis = {});
        void init(STISStatusPtr stis);

        A10 on_M10(const M10& m);
        A10 on_M11(const M11& m);
        A20 on_M20(const M20& m);
        A21 on_M21(const M21& m);
        A22 on_M22(const M22& m);
        A23 on_M23(const M23& m);
        A24 on_M24(const M24& m);
        A30 on_M30(const M30& m);
        A31 on_M31(const M31& m);
        A32 on_M32(const M32& m);
        A33 on_M33(const M33& m);
        A50 on_M50(const M50& m);
        A51 on_M51(const M51& m);
        A52 on_M52(const M52& m);
        A53 on_M53(const M53& m);
        A70 on_M70(const M70& m);

        template <class M>
        auto dispatch_message_to_station(const M& m)
        {
            if constexpr (is_decayed_same_any_v<M, M30, M50, M51, M52>)
            {
                return get_station(m.DestinationStation).on_message(m);
            }
            else if constexpr (is_decayed_same_any_v<M, M31, M33>)
            {
                return get_station(m.DestinationOCC).on_message(m);
            }
            else
            {
                return get_station(m.Destination.StationID).on_message(m);
            }
        }

        Station& this_station();
        Station& get_station(std::string id);
        Station& get_occ();
        bool has_station(std::string id);
        bool has_occ();

        void set_pid_status(std::string id, int status);

        STISStatusPtr m_stis;
        sf::map<std::string, StationPtr, st::CompareNoCase> stations;
        std::once_flag m_once;
    };

    using StationManagerPtr = std::shared_ptr<StationManager>;
}
