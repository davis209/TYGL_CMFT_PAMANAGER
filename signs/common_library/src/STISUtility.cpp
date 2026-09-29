#include "pch.h"
#include "STISUtility.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/utility/src/base_ex/EntityAccessFactoryEx.h"
#include "core/utility/src/base_ex/LocationAccessFactoryEx.h"
#include "core/utility/src/base_ex/RunParamsEx.h"
#include "core/utility/src/base_ex/DebugUtilEx.h"
#include "core/utility/src/core/StdEx.h"
#include "core/utility/src/core/fixed_length_data/all.h"
#include "core/utility/src/core/algorithm/strings.h"
#include "core/utility/src/core/CacheDecorator.h"
#include "core/utility/src/core/algorithm/strings.h"
#include "core/utility/src/core/Vector.h"

using namespace std::literals;
using namespace boost::adaptors;
using namespace boost::assign;
using namespace boost::hof;
using namespace TA_Base_Core;

/**
 * class PID
 */
namespace TA_IRS_App::STIS_UTILITY::detail
{
    using st::make_cached;
    using st::fixed_length_data::DigitString;
    using TA_Base_Ex::Location;
    using TA_Base_Ex::Entity;

    const PIDList& get_pids_by_tokey(const std::string& token)
    {
        static auto s_func = make_cached([](const std::string& t)
        {
            auto entities = Entity::get_entities_of_type_with_name_like_token<DataNodeEntityData>(t);
            return PID::from_entity_names(st::transform_to_vector(entities, std::mem_fn(&IEntityData::getName)));
        });

        return s_func(token);
    }

    bool PID::empty() const
    {
        return station.empty() || asset.empty() || name.empty() || id.empty() || level.empty();
    }

    PID::operator bool() const
    {
        return has_value();
    }

    bool PID::has_value() const
    {
        return !empty();
    }

    bool PID::is_occ() const
    {
        return extra.has_occ_prefix;
    }

    bool PID::is_lcd() const
    {
        return boost::iequals(type, "LCD");
    }

    bool PID::is_led() const
    {
        return boost::iequals(type, "LED");
    }

    const PID& PID::from_entity_name(const std::string& entity_name)
    {
        static const auto PID_PREFIX_LIST = std::vector<std::string>{"LCD", "LED", "PDP"};

        static auto s_func = make_cached([](const std::string& name)
        {
            auto occ = boost::istarts_with(name, "OCC_");

            if (auto parts = st::splitted(occ ? st2::iremove_left_copy(name, "OCC_") : name, "."); 4 <= parts.size())
            {
                if (boost::algorithm::any_of(PID_PREFIX_LIST, [&](auto& prefix) { return boost::istarts_with(parts[3], prefix); }))
                {
                    PID pid;
                    pid.asset = name;
                    pid.station = std::move(parts[0]);
                    pid.name = std::move(parts[3]);
                    pid.type = st2::remove_right_copy(pid.name, st2::last_digits(pid.name));
                    pid.id = DigitString<3>{st2::last_n_digits<3>(pid.name)};
                    pid.level = std::move(parts[2]);
                    pid.extra.has_occ_prefix = occ;
                    pid.extra.entity = Entity::to_key(name);
                    pid.extra.location = Location::to_key(pid.station);
                    pid.extra.location_order = Location::get_location(pid.extra.location)->getOrderId();
                    pid.extra.id = std::stoi(pid.id);
                    return pid;
                }
            }

            return PID{};
        });

        return s_func(entity_name);
    }

    const PID& PID::from_entity_key(size_t key)
    {
        return from_entity_name(Entity::to_name(key));
    }

    const PID& PID::from_station_and_id(const std::string& station, const std::string& id)
    {
        static auto s_func = make_cached([](const std::string& station, const std::string& id)
        {
            using namespace boost::hof;
            auto& pids = get_all(station);
            auto it = boost::find_if(pids, proj(&PID::id, _ == id));
            return it != pids.end() ? *it : PID{};
        });

        return s_func(station, id);
    }

    std::vector<PID> PID::from_entity_names(const std::vector<std::string>& entity_names)
    {
        std::vector<PID> pids;
        return push_back(pids, entity_names
                         | transformed(from_entity_name)
                         | filtered(std::mem_fn(&PID::has_value)));
    }

    std::vector<PID> PID::from_entity_keys(const std::vector<size_t>& entity_keys)
    {
        std::vector<PID> pids;
        return push_back(pids, entity_keys
                         | transformed(from_entity_key)
                         | filtered(std::mem_fn(&PID::has_value)));
    }

    const PIDList& PID::get_all()
    {
        if (ThisLocation::is_occ())
        {
            static PIDList s_pids = boost::hof::eval([]
            {
                auto entities = Entity::get_entities_of_type_with_name_like_token<DataNodeEntityData>("OCC_%.TIS.");
                Location::sort_by_order_id(entities);
                return PID::from_entity_names(st::transform_to_vector(entities, std::mem_fn(&ILocation::getName)));
            });

            return s_pids;
        }
        else
        {
            return get_all(ThisLocation::name());
        }
    }

    PIDList PID::get_all_copy()
    {
        return get_all();
    }

    const PIDList& PID::get_all(size_t location)
    {
        return get_all(Location::to_name(location));
    }

    const PIDList& PID::get_all(const std::string& location)
    {
        return ThisLocation::is_occ()
            ? get_all_occ(location)
            : get_all_station(location)
            ;
    }

    const PIDList& PID::get_all_occ(size_t location)
    {
        return get_all_occ(Location::to_name(location));
    }

    const PIDList& PID::get_all_occ(const std::string& location)
    {
        return get_pids_by_tokey(str(boost::format("OCC_%s.TIS.") % Location::to_name(location)));
    }

    const PIDList& PID::get_all_station(size_t location)
    {
        return get_all_station(Location::to_name(location));
    }

    const PIDList& PID::get_all_station(const std::string& location)
    {
        static auto s_func = make_cached([](const std::string& location)
        {
            auto pids = get_pids_by_tokey(str(boost::format("%s.TIS.") % Location::to_name(location)));
            return st::filter_out(pids, std::mem_fn(&PID::is_occ));
        });

        return s_func(location);
    }

    const PIDList& PID::get_all_stations()
    {
        static auto s_func = make_cached([]()
        {
            auto pids = get_pids_by_tokey(".TIS.");
            return st::filter_out(pids, std::mem_fn(&PID::is_occ));
        });

        return s_func();
    }

    const std::vector<std::string>& PID::get_areas()
    {
        if (ThisLocation::is_occ())
        {
            return get_areas(Location::get_all_location_names());
        }
        else
        {
            return get_areas(ThisLocation::name());
        }
    }

    const std::vector<std::string>& PID::get_areas(const std::string& location)
    {
        static auto s_func = make_cached([](const std::string& location)
        {
            return st::vector<std::string>{st::transform_to_vector(PID::get_all(location), std::mem_fn(&PID::level))}.sort_unique();
        });

        return s_func(location);
    }

    const std::vector<std::string>& PID::get_areas(const std::vector<std::string>& locations)
    {
        static auto s_func = make_cached([](const std::vector<std::string>& locations)
        {
            st::vector<std::string> areas;

            for (auto& location : locations)
            {
                areas.push_back_if_none_of_equal(PID::get_areas(location));
            }

            return areas.sort_unique();
        });

        return s_func(locations);
    }

    const PID& PID::npid()
    {
        static const PID s_pid;
        return s_pid;
    }
}

namespace TA_IRS_App::STIS_UTILITY::detail
{
    std::string message_timestamp()
    {
        return st::get_time_YYYYMMDDHHMMSS();
    }

    int next_message_sequence()
    {
        static std::atomic_int s_sequence = 0;

        if (9999 < ++s_sequence)
        {
            s_sequence = 1;
        }

        return s_sequence;
    }

    std::string join_4_languages(std::vector<std::string> languages, const std::string& delimiter)
    {
        if (languages.size() != 4)
        {
            languages.resize(4);
        }

        return st::join_transformed(delimiter, languages, [](auto& s)
        {
            return st2::as_string(st2::utf8_to_utf16le(s));
        }) + delimiter;
    }

    std::vector<std::string> split_to_4_languages(const std::string& str, const std::string delimiter)
    {
        std::vector<std::string> vs;
        auto ws = st2::as_wstring(str);
        auto wch = st2::as_wstring(delimiter)[0];

        auto begin = 0;

        for (auto pos = ws.find(wch); pos != std::wstring::npos; pos = ws.find(wch, begin))
        {
            auto line = ws.substr(begin, pos - begin);
            boost::trim(line);
            begin = pos + 1;
            vs.emplace_back(st2::utf16le_to_utf8(st2::as_u16string(line)));
        }

        return vs;
    }

    std::string join_4_languages_utf8(std::vector<std::string> languages, const std::string& delimiter)
    {
        if (languages.size() != 4)
        {
            languages.resize(4);
        }

        return boost::algorithm::join(languages, delimiter) + delimiter;
    }

    std::string join_4_utf16_languages_to_utf8(std::vector<std::vector<unsigned char>> languages, const std::string& delimiter)
    {
        if (languages.size() != 4)
        {
            languages.resize(4);
        }

        std::string str;

        for (auto i = 0; i < 4; ++i)
        {
            str += st2::utf16le_to_utf8(st2::as_u16string(languages[i]));
            push_back(str).range(delimiter);
        }

        return str;
    }

    std::vector<unsigned char> join_4_languages_utf16(std::vector<std::vector<unsigned char>> languages, const std::string& delimiter)
    {
        if (languages.size() != 4)
        {
            languages.resize(4);
        }

        std::vector<unsigned char> res;

        for (auto i = 0; i < 4; ++i)
        {
            push_back(res).range(languages[i]);
            push_back(res).range(delimiter);
        }

        return res;
    }

    std::vector<std::string> split_to_4_languages_utf8(const std::string& str, const std::string delimiter)
    {
        std::vector<std::string> vs;
        auto begin = 0UL;

        for (auto pos = str.find(delimiter); pos != std::string::npos; pos = str.find(delimiter, begin))
        {
            auto line = str.substr(begin, pos - begin);
            boost::trim(line);
            vs.emplace_back(std::move(line));
            begin = pos + delimiter.size();
        }

        return vs;
    }

    std::vector<std::vector<unsigned char>> split_to_4_languages_utf16(const std::vector<unsigned char>& utf16, const std::string delimiter)
    {
        std::vector<std::vector<unsigned char>> vs;
        auto str = st2::as_string(utf16);
        auto begin = 0UL;

        for (auto pos = str.find(delimiter); pos != std::string::npos; pos = str.find(delimiter, begin))
        {
            auto line = str.substr(begin, pos - begin);
            vs.emplace_back(st2::as_blob(line));  // utf16-le
            begin = pos + delimiter.size();
        }

        return vs;
    }

    std::vector<unsigned char> transform_4_languages_from_utf8_to_utf16(const std::string& str, const std::string delimiter)
    {
        std::vector<std::vector<unsigned char>> utf16s;

        for (auto utf8 : split_to_4_languages_utf8(str, "\xFF\xFF"))
        {
            utf16s.emplace_back(st2::as_blob(st2::utf8_to_utf16le(utf8)));
        }

        return join_4_languages_utf16(utf16s, delimiter);
    }

    std::string transform_4_languages_from_utf16_to_utf8(std::vector<unsigned char> languages, const std::string delimiter)
    {
        auto utf16s = split_to_4_languages_utf16(languages, "\xFF\xFF");
        return join_4_utf16_languages_to_utf8(utf16s, delimiter);
    }
}
