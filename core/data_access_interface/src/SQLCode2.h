#pragma once
#include "sql.h"
#include "core/database/src/SimpleSQLConverter.h"
#include "core/database/src/CommonType.h"
#include "core/utilities/src/StdUtil.h"
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <map>

namespace TA_Base_Core
{
    struct SQLCode2
    {
        using SQLMap = std::map<std::string, SQLStatement, stdutil::CompareNoCase>;

        template <typename... Args>
        static SQLStatement get(const std::string& sqlId, Args&& ... args)
        {
            auto stmt = stdutil::get_or(getSQLMap(), sqlId);

            if (sizeof...(args))
            {
                for (auto* sql : std::vector<std::string*> { &stmt.strCommon, &stmt.strOracle, &stmt.strMySQL, &stmt.strSQLite })
                {
                    if (sql->size())
                    {
                        boost::format fmt(*sql);
                        int unroll[] = { (fmt % boost::lexical_cast<std::string>(std::forward<Args>(args)), 0)..., 0 };
                        *sql = std::move(fmt.str());
                    }
                }

                if (stmt.strSQLite.empty() && stmt.strOracle.size())
                {
                    stmt.strSQLite = SimpleSQLConverter::oracle_to_sqlite(stmt.strOracle);
                }
            }

            return stmt;
        }

        static SQLMap& getSQLMap()
        {
            static std::once_flag s_once;
            static SQLMap s_map;

            std::call_once(s_once, [&]
            {
                for (auto& sqls : s_SQLArray)
                {
                    SQLStatement stmt;
                    stmt.nSQLID = 0; // no use
                    stmt.strSQLID = sqls.pSQLID;
                    stmt.strCommon = sqls.pCommonSQL;
                    stmt.strOracle = sqls.pOracleSQL;
                    stmt.strMySQL = sqls.pMySQLSQL;
                    stmt.strSQLite = sqls.pSQLiteSQL;
                    s_map.emplace(stmt.strSQLID, std::move(stmt));
                }
            });

            return s_map;
        }
    };
}
