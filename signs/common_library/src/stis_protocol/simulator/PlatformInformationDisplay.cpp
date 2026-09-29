#include "pch.h"
#include "PlatformInformationDisplay.h"
#include "OptionalOutput.h"
#include "app/signs/common_library/src/stis_protocol/XMLParser.h"
#include "app/signs/common_library/src/stis_protocol/STSMSGLIB_XML.h"
#include "app/signs/common_library/src/STISUtility.h"
#include "core/utilities/src/CallstackLogger.h"
#include "core/utility/src/base_ex/DebugUtilEx.h"
#include "core/utility/src/base_ex/RunParamsEx.h"
#include "core/utility/src/core/StdEx.h"

namespace TA_IRS_App::STIS_PROTOCOL::SIMULATOR::stissimulator::detail
{
    using namespace std::literals;
    using namespace boost::mp11;
    using boost::filesystem::path;
    using namespace TA_Base_Ex;
    using namespace STIS_PROTOCOL::IMPL;
    using namespace STIS_PROTOCOL::MESSAGE_TYPES;

    // M10: Display of Pre-defined Message Request [Station] & [OCC]
    void PlatformInformationDisplay::on_message(const M10& m)
    {
        LOG_CALLSTACK(boost::format("PlatformInformationDisplay[%s]::on_message") % this->id);

        Message msg;
        msg.tag = m.Message.MessageTag;
        msg.start_time = m.Message.StartTime;
        msg.end_time = m.Message.EndTime;
        msg.priority = m.Message.Priority;

        static auto s_root_dir = RunParamsEx::get("stis-library-root-dir");

        if (auto xml = CacheedXMLParser::parse_message_library_by_version(s_root_dir, m_stis->get_current_message_library_version()))
        {
            auto tag = boost::trim_copy(m.Message.MessageTag.str());
            auto pm = m.Message.Priority.value() < 4 ? xml->get_emergency_message(tag) : xml->get_normal_message(tag);
#if 0
            msg.text = str(boost::format("%s%s%s%s") % pm.EnglishMesg % pm.ChineseMesg % pm.MalayMesg % pm.TamilMesg);
#else
            msg.text = STIS_UTILITY::join_4_languages_utf8({pm.EnglishMesg, pm.ChineseMesg, pm.MalayMesg, pm.TamilMesg});
#endif
        }
        else
        {
            static std::once_flag s_once;
            std::call_once(s_once, [&]
            {
                s_cout << boost::format("ERROR: can not open  %s") % XMLParser::make_message_library_xml_path(s_root_dir, m_stis->get_current_message_library_version()) << std::endl;
            });
        }

        messages.emplace_back(std::move(msg));
    }

    // M11: Display of Ad hoc Message Request [Station] & [OCC]
    void PlatformInformationDisplay::on_message(const M11& m)
    {
        LOG_CALLSTACK(boost::format("PlatformInformationDisplay[%s]::on_message") % this->id);

        Message msg;
        msg.tag = m.Message.MessageTag;
        msg.start_time = m.Message.StartTime;
        msg.end_time = m.Message.EndTime;
        msg.priority = m.Message.Priority;
#if 0
        msg.text = m.Message.MessageText;
#else
        msg.text = STIS_UTILITY::join_4_languages_utf8({m.Message.MessageText.value()});
#endif
        messages.emplace_back(std::move(msg));
    }

    // M20: Clear Current Messages Request [Station] & [OCC]
    void PlatformInformationDisplay::on_message(const M20& m)
    {
        LOG_CALLSTACK(boost::format("PlatformInformationDisplay[%s]::on_message") % this->id);

        auto priorities = std::tie(m.Priority1, m.Priority2, m.Priority3, m.Priority4, m.Priority5, m.Priority6, m.Priority7, m.Priority8);

        mp_for_each<mp_iota_c<std::tuple_size_v<decltype(priorities)>>>([&](auto N)
        {
            if (auto& priority = std::get<N>(priorities); priority.value())
            {
                messages.remove_if([&](auto& msg) { return msg.priority == N + 1; });
            }
        });
    }

    // M21: PID On/Off Control Request [Station] & [OCC]
    void PlatformInformationDisplay::on_message(const M21& m)
    {
        LOG_CALLSTACK(boost::format("PlatformInformationDisplay[%s]::on_message") % this->id);

        switch (m.ControlOn)
        {
        case EPIDControlOn::ControlOn:
            set_status(EPIDStatus::On);
            break;
        case EPIDControlOn::NoAction:
            if (m.ControlOff == EPIDControlOff::ControlOff)
            {
                set_status(EPIDStatus::Off);
            }
            break;
        }
    }

    // M22: Send Pre-defined Display Template Request [Station] & [OCC]
    void PlatformInformationDisplay::on_message(const M22& m)
    {
        LOG_CALLSTACK(boost::format("PlatformInformationDisplay[%s]::on_message") % this->id);

        if (m.LCDDisplayTemplate.DisplayTemplateID.str().size())
        {
            display_template = m.LCDDisplayTemplate;
        }
    }

    // M23: Remove Display Template Request
    void PlatformInformationDisplay::on_message(const M23& m)
    {
        LOG_CALLSTACK(boost::format("PlatformInformationDisplay[%s]::on_message") % this->id);
        display_template = {};
    }

    // M24: Clear Current Messages Request by Message Tag(ID)
    void PlatformInformationDisplay::on_message(const M24& m)
    {
        LOG_CALLSTACK(boost::format("PlatformInformationDisplay[%s]::on_message") % this->id);

        messages.remove_if([&](auto& msg)
        {
            return  boost::iequals(m.MessageTag.str(), msg.tag);
        });
    }

    void PlatformInformationDisplay::set_status(EPIDStatus status)
    {
        set_status(static_cast<int>(status));
    }

    void PlatformInformationDisplay::set_status(int status)
    {
        LOG_CALLSTACK(boost::format("PlatformInformationDisplay[%s]::set_status") % this->id);

        if (status < 0 || 2 < status)
        {
            throw std::runtime_error(str(boost::format("bad pid status %d") % status));
        }

        this->status = static_cast<EPIDStatus>(status);
        LOG_DEBUG("set_status(): set pid %s status to %d", this->id, status);
    }

    std::string PlatformInformationDisplay::get_message_status() const
    {
        return status == EPIDStatus::On && messages.size() ? "01" : "00";
    }

    void PlatformInformationDisplay::remove_outdated_messages()
    {
        auto now = std::time(nullptr);
        messages.remove_if([&](auto& msg)
        {
            return 4 <= msg.priority && stdex::from_time_YYYYMMDDHHMMSS(msg.end_time) < now;
        });
    }
}
