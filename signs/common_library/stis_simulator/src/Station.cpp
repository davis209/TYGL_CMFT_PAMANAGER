#include "pch.h"
#include "Station.h"
#include "app/signs/common_library/src/stis_protocol/XMLParser.h"
#include "app/signs/common_library/src/stis_protocol/STSMSGLIB_XML.h"
#include "core/utilities/src/CallstackLogger.h"
#include "core/utility/src/base_ex/DebugUtilEx.h"
#include "core/utility/src/base_ex/RunParamsEx.h"
#include "core/utility/src/core/StdEx.h"
#include "core/utility/src/core/fixed_length_data/all.h"
#include "core/utility/src/core/MakeNameValuePairsString.h"

namespace TA_IRS_App::STIS_PROTOCOL::SIMULATOR::stissimulator::detail
{
    using boost::filesystem::path;
    using namespace TA_Base_Ex;
    using namespace std::literals;
    using namespace STIS_PROTOCOL::IMPL;
    using namespace STIS_PROTOCOL::MESSAGE_TYPES;
    using st::fixed_length_data::DigitString;

    Station::Station(STISStatusPtr stis, std::string id)
    {
        this->m_stis = stis;
        this->id = boost::trim_copy(id);
        all_pid.m_stis = stis;
    }

    // M10: Display of Pre-defined Message Request [Station] & [OCC]
    A10 Station::on_M10(const M10& m)
    {
        LOG_CALLSTACK(boost::format("Station[%s]::on_M10") % this->id);
        dispatch_message_to_pids(m);
        return make_response_message<A10>(m);
    }

    // M11: Display of Ad hoc Message Request [Station] & [OCC]
    A10 Station::on_M11(const M11& m)
    {
        LOG_CALLSTACK(boost::format("Station[%s]::on_M11") % this->id);
        dispatch_message_to_pids(m);
        return make_response_message<A10>(m);
    }

    // M20: Clear Current Messages Request [Station] & [OCC]
    A20 Station::on_M20(const M20& m)
    {
        LOG_CALLSTACK(boost::format("Station[%s]::on_M20") % this->id);
        dispatch_message_to_pids(m);
        return make_response_message<A20>(m);
    }

    // M21: PID On/Off Control Request [Station] & [OCC]
    A21 Station::on_M21(const M21& m)
    {
        LOG_CALLSTACK(boost::format("Station[%s]::on_M21") % this->id);
        dispatch_message_to_pids(m);
        return make_response_message<A21>(m);
    }

    // M22: Send Pre-defined Display Template Request [Station] & [OCC]
    A22 Station::on_M22(const M22& m)
    {
        LOG_CALLSTACK(boost::format("Station[%s]::on_M22") % this->id);
        dispatch_message_to_pids(m);
        return make_response_message<A22>(m);
    }

    // M23: Remove Display Template Request
    A23 Station::on_M23(const M23& m)
    {
        LOG_CALLSTACK(boost::format("Station[%s]::on_M23") % this->id);
        dispatch_message_to_pids(m);
        return make_response_message<A23>(m);
    }

    // M24: Clear Current Messages Request by Message Tag(ID)
    A24 Station::on_M24(const M24& m)
    {
        LOG_CALLSTACK(boost::format("Station[%s]::on_M24") % this->id);
        dispatch_message_to_pids(m);
        return make_response_message<A24>(m);
    }

    A30 Station::on_M30(const M30& m)
    {
        LOG_CALLSTACK(boost::format("Station[%s]::on_M30") % this->id);
        auto a = make_response_message<A30>(m);
        a.LANConnectionLinkStatus = m_stis->get_connection_link_status();
        a.AlarmSummary = m_stis->get_alarm_summary();
        a.versions_tied() = m_stis->get_versions();
        a.PIDStatusList = collect_pid_status();
        a.DataLength = a.datasize();
        return a;
    }

    A31 Station::on_M31(const M31& m)
    {
        LOG_CALLSTACK(boost::format("Station[%s]::on_M31") % this->id);
        A31 a;
        a.SQN = m.SQN;
        a.ConnectionLinkStatus = m_stis->get_connection_link_status();
        a.STISOCCServerStatus = m_stis->get_occ_server_status();
        a.versions_tied() = m_stis->get_versions();
        a.DataLength = a.datasize();
        return a;
    }

    A32 Station::on_M32(const M32& m)
    {
        LOG_CALLSTACK(boost::format("Station[%s]::on_M32") % this->id);
        assert(false);
        return {};
    }

    A33 Station::on_M33(const M33& m)
    {
        LOG_CALLSTACK(boost::format("Station[%s]::on_M33") % this->id);
        A33 a;
        a.SQN = m.SQN;
        a.LANConnectionLinkStatus = m_stis->get_lan_connection_link_status();
        a.AlarmSummary = m_stis->get_alarm_summary();
        a.versions_tied() = m_stis->get_versions();
        a.DataLength = a.datasize();
        return a;
    }

    A50 Station::on_M50(const M50& m)
    {
        LOG_CALLSTACK(boost::format("Station[%s]::on_M50") % this->id);
        auto a = make_response_message<A50>(m);
        a.ReportPID = m.DestinationPID;
        auto& pid = get_pid(m.DestinationPID);
        a.CurrentDiaplayTemplate = pid.m_display_template;
        a.MessageStatus = pid.get_message_status();
        size_t total_length = 0;

        for (auto&& msg : pid.messages.get_copy())
        {
            MSGCUR cm;
            cm.MessageTag = std::move(msg.tag);
            cm.MessageStartTime = std::move(msg.start_time);
            cm.MessageEndTime = std::move(msg.end_time);
            cm.MessagePriority = std::move(msg.priority);
#if 0
            cm.MessageText = std::move(msg.text);
#else
            cm.MessageText = STIS_UTILITY::transform_4_languages_from_utf8_to_utf16(msg.text);
#endif

            if ((9999 - 43) < (total_length += cm.size()))
            {
                LOG_DEBUG("on_M50(): data length is exceeded the limit 9999, truncated");
                break;
            }

            a.CurrentDisplayMessages.emplace_back(std::move(cm));
        }

        a.DataLength = a.datasize();
        return a;
    }

    A51 Station::on_M51(const M51& m)
    {
        LOG_CALLSTACK(boost::format("Station[%s]::on_M51") % this->id);

        A51 a;
        a.SQN = m.SQN;

        for (auto&& [x, pid] : pids)
        {
            MSGSCHSTNPID sm;
            sm.ReportStation = this->id;
            sm.ReportPid = pid.m_id;
            sm.CurrentDiaplayTemplate = pid.m_display_template;
            sm.MessageStatus = pid.get_message_status();

            for (auto&& msg : pid.messages.get_copy())
            {
                MSGSCH x;
                x.MessageTag = std::move(msg.tag);
                x.MessageStartTime = std::move(msg.start_time);
                x.MessageEndTime = std::move(msg.end_time);
                x.MessagePriority = std::move(msg.priority);
                x.MessageText = STIS_UTILITY::transform_4_languages_from_utf8_to_utf16(msg.text);
                sm.ScheduledDisplayMessages.emplace_back(std::move(x));
            }

            a.ScheduledDisplayingMessageTemplateOfStationPids.emplace_back(std::move(sm));
        }

        a.DataLength = a.datasize();
        return a;
    }

    A52 Station::on_M52(const M52& m)
    {
        LOG_CALLSTACK(boost::format("Station[%s]::on_M52") % this->id);

        A52 a;
        a.SQN = m.SQN;

        for (auto&& [x, pid] : pids)
        {
            TLSCHSTNPID sm;
            sm.ReportStation = this->id;
            sm.ReportPid = pid.m_id;

            for (auto& display_template : pid.m_display_templates)
            {
                sm.ScheduledDiaplayTemplateList.emplace_back(display_template);
            }

            a.ScheduledDisplayingTemplateListOfStationPids.emplace_back(std::move(sm));
        }

        a.DataLength = a.datasize();
        return a;
    }

    // M53: Remove Display Template by Template ID Request
    A53 Station::on_M53(const M53& m)
    {
        LOG_CALLSTACK(boost::format("Station[%s]::on_M53") % this->id);
        dispatch_message_to_pids(m);
        return make_response_message<A53>(m);
    }

    A70 Station::on_M70(const M70& m)
    {
        LOG_CALLSTACK(boost::format("Station[%s]::on_M70") % this->id);

        if (m.Type == ELibraryType::PredefinedMessage)
        {
            m_stis->set_current_message_library_version(m.Version.str());
        }
        else
        {
            m_stis->set_current_template_library_version(m.Version.str());
        }

        A70 a;
        a.SQN = m.SQN;
        a.data_tied() = m.data_tied();
        a.DataLength = a.datasize();
        return a;
    }

    PlatformInformationDisplay& Station::get_pid(std::string id)
    {
        LOG_CALLSTACK(boost::format("Station[%s]::get_pid") % this->id);

        id = DigitString<3>(id);

        if (id.empty() || id == "000")
        {
            return all_pid;
        }

        if (pids.count(id) == 0)
        {
            PlatformInformationDisplay pid;
            pid.m_id = id;
            pid.m_stis = m_stis;
            pid.status = all_pid.status;
            pid.m_station = this;
            pids.emplace(id, std::move(pid));
            LOG_DEBUG("get_pid(): add pid: %s", nvps(this->id, pid.m_id, pid.status));
        }

        return pids[id];
    }

    void Station::set_pid_status(std::string id, int status)
    {
        LOG_CALLSTACK(boost::format("Station[%s]::set_pid_status") % this->id);
        get_pid(id).set_status(status);
    }

    bool Station::is_occ()
    {
        return boost::iequals(id, "OCC");
    }
}
