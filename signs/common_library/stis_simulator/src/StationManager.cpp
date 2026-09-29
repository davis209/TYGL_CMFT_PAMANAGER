#include "pch.h"
#include "StationManager.h"
#include "OptionalOutput.h"
#include "app/signs/common_library/src/stis_protocol/XMLParser.h"
#include "app/signs/common_library/src/stis_protocol/STSMSGLIB_XML.h"
#include "app/signs/common_library/src/STISUtility.h"
#include "core/utilities/src/CallstackLogger.h"
#include "core/utility/src/base_ex/DebugUtilEx.h"
#include "core/utility/src/base_ex/RunParamsEx.h"
#include "core/utility/src/core/StdEx.h"
#include "core/utility/src/core/StaticObject.h"
#include "core/utility/src/core/SimpleTimer.h"
#include "core/utility/src/core/algorithm/strings.h"
#include "core/utility/src/core/fixed_length_data/all.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/thread/executor.hpp>

namespace TA_IRS_App::STIS_PROTOCOL::SIMULATOR::stissimulator::detail
{
    using namespace std::literals;
    using boost::filesystem::path;
    using boost::executors::thread_executor;
    namespace pt = boost::property_tree;
    using st::SimpleTimer;
    using namespace TA_Base_Ex;
    using namespace STIS_PROTOCOL::IMPL;
    using namespace STIS_PROTOCOL::MESSAGE_TYPES;
    using st::StaticObject;
    using st::fixed_length_data::DigitString;

    StationManager& StationManager::instance()
    {
        return StaticObject<StationManager>::value();
    }

    StationManager::StationManager(STISStatusPtr stis)
        : m_stis(stis)
    {
        if (stis)
        {
            init(stis);
        }
    }

    void StationManager::init(STISStatusPtr stis)
    {
        LOG_CALLSTACK("StationManager::init");

        if (m_stis != stis)
        {
            m_stis = stis;
            stations.emplace(m_stis->get_name(), std::make_shared<Station>(m_stis, m_stis->get_name()));
        }

        std::call_once(m_once, [&]
        {
            auto cfg = RunParamsEx::get_or("pids", "pids_" + stis->get_name() + ".ini");

            if (!exists(path(cfg)))
            {
                return;
            }

            pt::ptree tree;
            pt::read_ini(cfg, tree);

            for (auto node : tree)
            {
                try
                {
                    set_pid_status(node.first, node.second.get_value<int>());
                }
                catch (const std::exception& e)
                {
                    LOG_ERROR("init(): error: %s", e.what());
                }
            }

            StaticObject<SimpleTimer, StationManager>::instance().submit("remove-outdated-display-messages", 1s, [&]
            {
                stations.for_each_value([&](auto& station)
                {
                    station->pids.for_each_value_mem_fn(&PlatformInformationDisplay::remove_outdated_messages);
                });
            });
        });
    }

   // M10: Display of Pre-defined Message Request [Station] & [OCC]
    A10 StationManager::on_M10(const M10& m)
    {
        LOG_CALLSTACK("StationManager::on_M10");
        return dispatch_message_to_station(m);
    }

    // M11: Display of Ad hoc Message Request [Station] & [OCC]
    A10 StationManager::on_M11(const M11& m)
    {
        LOG_CALLSTACK("StationManager::on_M11");
        return dispatch_message_to_station(m);
    }

    // M20: Clear Current Messages Request [Station] & [OCC]
    A20 StationManager::on_M20(const M20& m)
    {
        LOG_CALLSTACK("StationManager::on_M20");
        return dispatch_message_to_station(m);
    }

    // M21: PID On/Off Control Request [Station] & [OCC]
    A21 StationManager::on_M21(const M21& m)
    {
        LOG_CALLSTACK("StationManager::on_M21");
        return dispatch_message_to_station(m);
    }

    // M22: Send Pre-defined Display Template Request [Station] & [OCC]
    A22 StationManager::on_M22(const M22& m)
    {
        LOG_CALLSTACK("StationManager::on_M22");
        return dispatch_message_to_station(m);
    }

    // M23: Remove Display Template Request
    A23 StationManager::on_M23(const M23& m)
    {
        LOG_CALLSTACK("StationManager::on_M23");
        return dispatch_message_to_station(m);
    }

    // M24: Clear Current Messages Request by Message Tag(ID)
    A24 StationManager::on_M24(const M24& m)
    {
        LOG_CALLSTACK("StationManager::on_M24");
        return dispatch_message_to_station(m);
    }

    // M30: Station STIS Status Request [Station]
    A30 StationManager::on_M30(const M30& m)
    {
        LOG_CALLSTACK("StationManager::on_M30");
        return dispatch_message_to_station(m);
    }

    A31 StationManager::on_M31(const M31& m)
    {
        LOG_CALLSTACK("StationManager::on_M31");
        return dispatch_message_to_station(m);
    }

    A32 StationManager::on_M32(const M32& m)
    {
        LOG_CALLSTACK("StationManager::on_M32");
        auto make_station_status_detail = [&](auto& s)
        {
            STATIONSTATUSDETAIL detail;
            detail.ReportStation = s.id;
            detail.LANConnectionLinkStatus = m_stis->get_connection_link_status();
            detail.AlarmSummary = m_stis->get_alarm_summary();
            detail.versions_tied() = m_stis->get_versions();
            detail.PIDStatusList = s.collect_pid_status();
            return detail;
        };

        A32 a;
        a.SQN = m.SQN;

        if (has_occ())
        {
            a.AllStationStatusDetails.emplace_back(make_station_status_detail(get_occ()));
        }

        stations.for_each_value([&](auto sp)
        {
            if (!sp->is_occ())
            {
                a.AllStationStatusDetails.emplace_back(make_station_status_detail(*sp));
            }
        });

        return a;
    }

    // M33: OCC STIS Status Sync Request [OCC]
    A33 StationManager::on_M33(const M33& m)
    {
        LOG_CALLSTACK("StationManager::on_M33");
        return dispatch_message_to_station(m);
    }

    A50 StationManager::on_M50(const M50& m)
    {
        LOG_CALLSTACK("StationManager::on_M50");
        return dispatch_message_to_station(m);
    }

    A51 StationManager::on_M51(const M51& m)
    {
        LOG_CALLSTACK("StationManager::on_M51");

        // all stations
        if (m.DestinationStation.empty())
        {
            A51 a;
            a.SQN = m.SQN;

            for (auto [x, station] : stations)
            {
                auto as = station->on_M51(m);
                LOG_DEBUG("on_M51(): size=%d", as.ScheduledDisplayingMessageTemplateOfStationPids.size());

                for (auto i : as.ScheduledDisplayingMessageTemplateOfStationPids.m_vector)
                {
                    a.ScheduledDisplayingMessageTemplateOfStationPids.emplace_back(i);
                }
            }

            a.DataLength = a.datasize();
            return a;
        }

        return dispatch_message_to_station(m);
    }

    A52 StationManager::on_M52(const M52& m)
    {
        LOG_CALLSTACK("StationManager::on_M52");

        // all stations
        if (m.DestinationStation.empty())
        {
            A52 a;
            a.SQN = m.SQN;

            for (auto [x, station] : stations)
            {
                auto as = station->on_M52(m);
                LOG_DEBUG("on_M52(): size=%d", as.ScheduledDisplayingTemplateListOfStationPids.size());

                for (auto i : as.ScheduledDisplayingTemplateListOfStationPids.m_vector)
                {
                    a.ScheduledDisplayingTemplateListOfStationPids.emplace_back(i);
                }
            }

            a.DataLength = a.datasize();
            return a;
        }

        return dispatch_message_to_station(m);
    }

    // M53: Remove Display Template by Template ID Request
    A53 StationManager::on_M53(const M53& m)
    {
        LOG_CALLSTACK("StationManager::on_M53");
        return dispatch_message_to_station(m);
    }

    A70 StationManager::on_M70(const M70& m)
    {
        LOG_CALLSTACK("StationManager::on_M70");
        return get_occ().on_M70(m);
    }

    Station& StationManager::get_station(std::string id)
    {
        LOG_CALLSTACK("StationManager::get_station");
        boost::trim(id);

        if (stations.count(id) == 0)
        {
            stations.emplace(id, std::make_shared<Station>(m_stis, id));
        }

        return *stations.get_value(id);
    }

    Station& StationManager::get_occ()
    {
        LOG_CALLSTACK("StationManager::get_occ");
        return get_station("OCC");
    }

    Station& StationManager::this_station()
    {
        init(m_stis);
        return get_station(m_stis->get_name());
    }

    bool StationManager::has_station(std::string id)
    {
        return stations.count(boost::trim_copy(id));
    }

    bool StationManager::has_occ()
    {
        return has_station("OCC");
    }

    void StationManager::set_pid_status(std::string id, int status)
    {
        LOG_CALLSTACK("StationManager::set_pid_status");

        if (stdex2::isdigits(id))
        {
            this_station().get_pid(id).status = static_cast<EPIDStatus>(status);
        }
        else if (auto pid = STIS_UTILITY::PID::from_entity_name(id))
        {
            auto station = pid.station;
            auto id2 = pid.id;
            get_station(station).set_pid_status(id2, status);
        }
        else
        {
            throw std::runtime_error(str(boost::format("bad pid format: %s") % id));
        }
    }
}
