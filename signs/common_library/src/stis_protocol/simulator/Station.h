#pragma once
#include "STISStatus.h"
#include "PlatformInformationDisplay.h"
#include "core/utility/src/core/Map.h"
#include "core/utility/src/core/type_traits/stl.h"
#include "app/signs/common_library/src/stis_protocol/message_types/MessageDataTypes.h"

namespace TA_IRS_App::STIS_PROTOCOL::SIMULATOR::stissimulator::detail
{
    using namespace MESSAGE_TYPES;
    using namespace MESSAGE_DATA_TYPES;
    namespace sf = stdex::thread_safe;
    using stdex::typetraits2::is_decayed_same_any_v;
    using Versions = std::tuple<std::string, std::string, std::string, std::string>;

    struct Station
    {
        Station(STISStatusPtr stis, std::string station = "");
        virtual ~Station() = default;

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
        A50 on_M50(const M50& m);
        A70 on_M70(const M70& m);

        A10 on_message(const M10& m) { return on_M10(m); }
        A10 on_message(const M11& m) { return on_M11(m); }
        A20 on_message(const M20& m) { return on_M20(m); }
        A21 on_message(const M21& m) { return on_M21(m); }
        A22 on_message(const M22& m) { return on_M22(m); }
        A23 on_message(const M23& m) { return on_M23(m); }
        A24 on_message(const M24& m) { return on_M24(m); }
        A30 on_message(const M30& m) { return on_M30(m); }
        A31 on_message(const M31& m) { return on_M31(m); }
        A32 on_message(const M32& m) { return on_M32(m); }
        A50 on_message(const M50& m) { return on_M50(m); }
        A70 on_message(const M70& m) { return on_M70(m); }

        template <class M>
        void dispatch_message_to_pids(const M& m)
        {
            if constexpr (stdex::typetraits2::is_decayed_same_any_v<M, M10, M11, M20, M21, M22, M23, M24>)
            {
                if (m.Destination.PIDList.m_vector.empty())
                {
                    all_pid.on_message(m);

                    for (auto& pair : pids)
                    {
                        pair.second.on_message(m);
                    }
                }
                else
                {
                    for (auto& id : m.Destination.PIDList.m_vector)
                    {
                        get_pid(id).on_message(m);
                    }
                }
            }
        }

        auto collect_pid_status()
        {
            A30::PIDSTATUSLIST list;
            pids.for_each_value([&](auto& pid) { list.emplace_back(pid.id, pid.status); });
            return list;
        }

        template <class A, class M>
        A make_response_message(const M& m)
        {
            A a;
            a.SQN = m.SQN;
            a.ReportStation = this->id;
            a.DataLength = a.datasize();
            return a;
        }

        PlatformInformationDisplay& get_pid(std::string id);

        void set_pid_status(std::string id, int status);

        bool is_occ();

        std::string id;
        STISStatusPtr m_stis;
        PlatformInformationDisplay all_pid;
        sf::map<std::string, PlatformInformationDisplay> pids;
    };

    using StationPtr = std::shared_ptr<Station>;
}
