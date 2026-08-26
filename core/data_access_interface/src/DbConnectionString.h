#pragma once
#include "DbConnectionStrings.h"
#include "core/database/src/CommonType.h"
#include "core/data_access_interface/src/DbStatus.h"
#include "core/utilities/src/StdUtil.h"
#include <boost/range.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/range/adaptors.hpp>
#include <string>

namespace dbconnectionstring_detail
{
    using namespace TA_Base_Core;
    using namespace boost::adaptors;
    using boost::copy_range;

    struct DbConnectionString
    {
        DbConnectionString() {}

        DbConnectionString(const DataConnection& connection)
            : DbConnectionString(connection.second)
        {
        }

        DbConnectionString(const std::string& str)
            : string(str)
        {
            parse(str);
        }

        void parse(const std::string& str)
        {
            auto vs = StdUtil::splitted(str, ":,;", "--trim");

            if (vs.size() == CON_SIZE && vs[0].size())
            {
                db_type = vs[CON_STR_DBType];
                schema = vs[CON_STR_SchemaName];
                user = vs[CON_STR_UserName];
                password = vs[CON_STR_Passwd];
                hostname = vs[CON_STR_Hostname];
                runparam = RPARAM_DBPREFIX + schema + (hostname.empty() ? "" : "@" + hostname);
            }
        }

        bool empty() const { return schema.empty(); }
        operator std::string() const { return string; }

        std::string string;
        std::string db_type;
        std::string schema;
        std::string user;
        std::string password;
        std::string hostname;
        std::string runparam;
    };

    struct DbConnectionStringList
    {
        DbConnectionStringList(const DataConnections& connections)
        {
            boost::for_each(connections, [&](const DataConnection & c) { parse(c.second); });
        }

        DbConnectionStringList(const std::vector<std::string>& strings)
        {
            boost::for_each(strings, [&](const std::string & s) { parse(s); });
        }

        void parse(const std::string& str)
        {
            DbConnectionString con(str);

            if (! con.empty())
            {
                m_connection_strings.push_back(con);
                m_database_names.push_back(con.schema);
                m_runparams.push_back(con.runparam);
            }
        }

        DbConnectionString& operator[](int i)
        {
            return m_connection_strings[i];
        }

        size_t size() const { return m_connection_strings.size();  }

        std::vector<std::string> m_runparams;
        std::vector<std::string> m_database_names;
        std::vector<DbConnectionString> m_connection_strings;
    };
}

namespace TA_Base_Core
{
    using dbconnectionstring_detail::DbConnectionString;
    using dbconnectionstring_detail::DbConnectionStringList;
}
