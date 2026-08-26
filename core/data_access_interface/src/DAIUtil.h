#pragma once
#include <boost/any.hpp>
#include <string>
#include <vector>
#include <iosfwd>
#include <functional>

namespace TA_Base_Core
{
    namespace DAIUtil
    {
        namespace format_security_log_detail
        {
            std::stringstream& formatSecurityLogStreamImpl(const std::string& format, const std::vector<boost::any>& anyList);
            std::stringstream& formatSecurityLogStreamImpl(std::stringstream& strm, const std::string& format, const std::vector<boost::any>& anyList);

            template <class... Args>
            std::stringstream& formatSecurityLogStream(const std::string& format, Args&& ... args)
            {
                return formatSecurityLogStreamImpl(format, std::vector<boost::any> {std::forward<Args>(args)...});
            }

            template <class... Args>
            std::stringstream& formatSecurityLogStream(std::stringstream& strm, const std::string& format, Args&& ... args)
            {
                return formatSecurityLogStreamImpl(strm, format, std::vector<boost::any> {std::forward<Args>(args)...});
            }

            template <class... Args>
            std::string formatSecurityLog(Args&& ... args)
            {
                return formatSecurityLogStream(std::forward<Args>(args)...).str();
            }
        }

        using format_security_log_detail::formatSecurityLog;
        using format_security_log_detail::formatSecurityLogStream;
    };

    namespace daiutil = DAIUtil;
}
