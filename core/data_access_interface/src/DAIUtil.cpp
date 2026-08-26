#include "DAIUtil.h"
#include "core/utilities/src/StdUtil.h"
#include "core/utilities/src/ToString.h"
#include "core/utilities/src/TAMap.h"
#include "core/data_access_interface/src/OperatorAccessFactoryEx.h"
#include "core/data_access_interface/src/ProfileAccessFactoryEx.h"
#include "core/data_access_interface/src/LocationAccessFactoryEx.h"
#include "core/data_access_interface/src/RegionAccessFactoryEx.h"
#include "core/data_access_interface/src/SubsystemAccessFactoryEx.h"
#include "core/data_access_interface/src/ActionAccessFactoryEx.h"
#include "core/data_access_interface/src/ActionGroupAccessFactoryEx.h"
#include "core/data_access_interface/src/ResourceAccessFactoryEx.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactoryEx.h"
#include <boost/regex.hpp>
#include <boost/any.hpp>
#include <mutex>

using namespace std::string_literals;

namespace TA_Base_Core
{
    namespace DAIUtil
    {
        namespace format_security_log_detail
        {
            using Formatter = std::function<void(const boost::any&, std::ostream& os)>;

            Formatter& getFormatter(const std::string& key)
            {
                static ta::map<std::string, Formatter, stdutil::CompareNoCase> s_formatters;
                static std::once_flag s_once;

                std::call_once(s_once, [&]
                {
                    s_formatters =
                    {
                        {"Session",             [](auto & any, auto & os) { auto key = stdutil::any_cast_string<std::string>(any);  os << boost::format("SessionId[%s]")    % key; }},
                        {"User",                [](auto & any, auto & os) { auto key = stdutil::any_cast_integer<size_t>(any);      os << boost::format("User[%d][%s]")     % key % OperatorAccessFactoryEx::toName(key); }},
                        {"Operator",            [](auto & any, auto & os) { auto key = stdutil::any_cast_integer<size_t>(any);      os << boost::format("Operator[%d][%s]") % key % OperatorAccessFactoryEx::toName(key); }},
                        {"Location",            [](auto & any, auto & os) { auto key = stdutil::any_cast_integer<size_t>(any);      os << boost::format("Location[%d][%s]") % key % LocationAccessFactoryEx::toName(key); }},
                        {"LocationNoPrefix",    [](auto & any, auto & os) { auto key = stdutil::any_cast_integer<size_t>(any);      os << boost::format("[%d][%s]")         % key % LocationAccessFactoryEx::toName(key); }},
                        {"Region",              [](auto & any, auto & os) { auto key = stdutil::any_cast_integer<size_t>(any);      os << boost::format("Region[%d][%s]")   % key % RegionAccessFactoryEx::toName(key); }},
                        {"Profile",             [](auto & any, auto & os) { auto key = stdutil::any_cast_integer<size_t>(any);      os << boost::format("Profile[%d][%s]")  % key % ProfileAccessFactoryEx::toName(key); }},

                        {   "ProfileType",      [](auto & any, auto & os)
                            {
                                auto key = stdutil::any_cast_integer<size_t>(any);
                                auto profile = ProfileAccessFactoryEx::getProfile(key);
                                os << (profile ? str(boost::format("ProfileType[%d][%s]") % profile->getType() % profile->getTypeAsString()) : "ProfileType[][]"s);
                            }
                        },

                        {"Subject",             [](auto & any, auto & os) { auto key = stdutil::any_cast_integer<size_t>(any);      os << boost::format("Subject[%d][%s]")     % key % ProfileAccessFactoryEx::toName(key); }},
                        {"Subsystem",           [](auto & any, auto & os) { auto key = stdutil::any_cast_integer<size_t>(any);      os << boost::format("Subsystem[%d][%s]")   % key % SubsystemAccessFactoryEx::toName(key); }},
                        {"Action",              [](auto & any, auto & os) { auto key = stdutil::any_cast_integer<size_t>(any);      os << boost::format("Action[%d][%s]")      % key % ActionAccessFactoryEx::toName(key); }},
                        {"ActionGroup",         [](auto & any, auto & os) { auto key = stdutil::any_cast_integer<size_t>(any);      os << boost::format("ActionGroup[%d][%s]") % key % ActionGroupAccessFactoryEx::toName(key); }},
                        {"Duty",                [](auto & any, auto & os) { auto key = stdutil::any_cast_integer<size_t>(any);      os << boost::format("Duty[%d][%s]")        % key % ActionGroupAccessFactoryEx::toName(key); }},
                        {"Workstation",         [](auto & any, auto & os) { auto key = stdutil::any_cast_integer<size_t>(any);      os << boost::format("Workstation[%d][%s]") % key % ConsoleAccessFactoryEx::toName(key); }},
                        {"Console",             [](auto & any, auto & os) { auto key = stdutil::any_cast_integer<size_t>(any);      os << boost::format("Console[%d][%s]")     % key % ConsoleAccessFactoryEx::toName(key); }},
                        {"Resource",            [](auto & any, auto & os) { auto key = stdutil::any_cast_integer<size_t>(any);      os << boost::format("Resource[%d][%s]")    % key % ResourceAccessFactoryEx::toName(key); }},
                        {"BoolResult",          [](auto & any, auto & os) { auto key = stdutil::any_cast_integer<bool>(any);        os << boost::format("Result[%s]")          % BoolToStr(key); }},
                        {"StrResult",           [](auto & any, auto & os) { auto key = stdutil::any_cast_string<std::string>(any);  os << boost::format("Result[%s]")          % key; }},  // same as StringResult
                        {"StringResult",        [](auto & any, auto & os) { auto key = stdutil::any_cast_string<std::string>(any);  os << boost::format("Result[%s]")          % key; }},
                        {"DisplayOnly",         [](auto & any, auto & os) { auto key = stdutil::any_cast_integer<bool>(any);        os << boost::format("DisplayOnly[%s]")     % BoolToStr(key); }}
                    };

                    auto makeListFormatter = [](auto prefix, auto transformer)
                    {
                        return [ = ](auto & any, auto & os)
                        {
                            auto keys = stdutil::any_cast_integer_list<std::vector<size_t>>(any);
                            os << prefix;
                            boost::for_each(keys, [&](auto & key) { os << boost::format("[%d][%s]") % key % transformer(key); });
                        };
                    };

                    s_formatters["UserList"]        = makeListFormatter("User",         [](auto key) { return OperatorAccessFactoryEx::toName(key); });
                    s_formatters["OperatorList"]    = makeListFormatter("Operator",     [](auto key) { return OperatorAccessFactoryEx::toName(key); });
                    s_formatters["LocationList"]    = makeListFormatter("Location",     [](auto key) { return LocationAccessFactoryEx::toName(key); });
                    s_formatters["RegionList"]      = makeListFormatter("Region",       [](auto key) { return RegionAccessFactoryEx::toName(key); });
                    s_formatters["ProfileList"]     = makeListFormatter("Profile",      [](auto key) { return ProfileAccessFactoryEx::toName(key); });
                    s_formatters["SubsystemList"]   = makeListFormatter("Subsystem",    [](auto key) { return SubsystemAccessFactoryEx::toName(key); });
                    s_formatters["ActionList"]      = makeListFormatter("Action",       [](auto key) { return ActionAccessFactoryEx::toName(key); });
                    s_formatters["ResourceList"]    = makeListFormatter("Resource",     [](auto key) { return ResourceAccessFactoryEx::toName(key); });

                    s_formatters["KeyList"] = [](auto & any, auto & os)
                    {
                        auto keys = stdutil::any_cast_integer_list<std::vector<size_t>>(any);
                        os << boost::format("[%s]") % stdutil::join(keys, ",");
                    };

                    s_formatters["StringList"] = [](auto & any, auto & os)
                    {
                        auto strs = boost::any_cast<std::vector<std::string>>(any);
                        os << boost::format("[%s]") % stdutil::join(strs, ",");
                    };

                    // compatible with printf/boost::format
                    s_formatters["s"]   = [](auto & any, auto & os) { os << stdutil::any_cast_string<std::string>(any); };
                    s_formatters["i"]   = [](auto & any, auto & os) { os << stdutil::any_cast_integer<int>(any); };
                    s_formatters["d"]   = [](auto & any, auto & os) { os << stdutil::any_cast_integer<int>(any); };
                    s_formatters["u"]   = [](auto & any, auto & os) { os << stdutil::any_cast_integer<unsigned int>(any); };
                    s_formatters["l"]   = [](auto & any, auto & os) { os << stdutil::any_cast_integer<long>(any); };
                    s_formatters["lu"]  = [](auto & any, auto & os) { os << stdutil::any_cast_integer<unsigned long>(any); };
                    s_formatters["ll"]  = [](auto & any, auto & os) { os << stdutil::any_cast_integer<long long>(any); };
                    s_formatters["llu"] = [](auto & any, auto & os) { os << stdutil::any_cast_integer<unsigned long long>(any); };
                    s_formatters["zu"]  = [](auto & any, auto & os) { os << stdutil::any_cast_integer<size_t>(any); };
                });

                if (auto value = s_formatters.get_value_optional(key))
                {
                    return *value;
                }

                LOG_ERROR("getFormatter(): can not get formatter for %s", key);

                static thread_local Formatter s_formatter;
                s_formatter = [ = ](auto & any, auto & os) { os << "${" << key << "}"; };
                return s_formatter;
            }

            std::stringstream& formatSecurityLogStreamImpl(const std::string& format, const std::vector<boost::any>& anyList)
            {
                static thread_local std::stringstream s_strm;
                s_strm.clear();
                s_strm.str("");
                return formatSecurityLogStreamImpl(s_strm, format, anyList);
            }

            std::stringstream& formatSecurityLogStreamImpl(std::stringstream& strm, const std::string& format, const std::vector<boost::any>& anyList)
            {
				// format: ${variable} or $(variable)
				// R ( )  C++11 Raw String
				// (?ix)  Pattern Modifiers, i(icase) without regard to case, x(mod_x) causes unescaped whitespace in the expression to be ignored.
				// /\$    : $
				// (\w+)   \w	equal ¡¯[A - Za - z0 - 9_]',   +  The + matches the preceding character or preceding group, 1 or more times. 
				// [\{\(]  match { or [ 
				// [\)\}]
                static thread_local boost::regex VARIABLE_REGEX(R"((?ix) \$ [\{\(] \s* (\w+) \s* [\)\}])");

                size_t i = 0;
                size_t pos = 0;
                auto& sb = *strm.rdbuf();
				//boost::sregex_iterator( begin, end =boost::sregex_iterator()  £©
				//Rang based for loop
				//m.str(1): get first group (\w+)
                for (auto& m : boost::make_iterator_range(boost::sregex_iterator(format.begin(), format.end(), VARIABLE_REGEX), boost::sregex_iterator()))
                {
                    sb.sputn(format.c_str() + pos, m.position() - pos);
                    getFormatter(m.str(1))(anyList[i++], strm);
                    pos = m.position() + m.length();
                }

                if (i != anyList.size())
                {
                    LOG_ERROR("formatSecurityLogStreamImpl(): format: %s, variable size: %d", format, anyList.size());
                }

                strm << format.c_str() + pos;
                return strm;
            }
        }
    }
}
