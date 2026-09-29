#pragma once
#include "core/utility/src/core/StdEx.h"
#include <string>
#include <map>
#include <vector>

namespace TA_IRS_App::STIS_PROTOCOL::SIMULATOR::stissimulator::detail
{
    struct NamePortMap
    {
        template <class R = std::string>
        static R port_from_name(std::string name)
        {
            auto it = s_name_port_map.find(name);
            auto res = it != s_name_port_map.end() ? it->second : s_default_port;

            if constexpr (std::is_same_v<R, std::string>)
            {
                return res;
            }
            else
            {
                return std::stoi(res);
            }
        }

        static std::vector<std::string> names()
        {
            return st::keys(s_name_port_map);
        }

        static inline std::string s_default_port = "15285";

        static inline std::map<std::string, std::string, st::CompareNoCase> s_name_port_map =
        {
            {"OCC",  "15285"},
            {"JS01", "15287"},
            {"JS02", "15289"},
            {"JS03", "15290"},
            {"JS04", "15291"},
            {"JS05", "15292"},
            {"TGD",  "15300"},
        };
    };
}
