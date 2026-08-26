//////////////////////////////////////////////////////////////////////
///     @file       SQLFileHelper.h
///     @author     zhilin,ouyang
///     @date       2010-12-9 14:44:41
///
///     @brief      to load the sql.dat file and store the SQL statements
///                 into a hash-table for later retrieve.
//////////////////////////////////////////////////////////////////////

#pragma once
#include "SQLCommonDef.h"
#include <string>
#include <vector>
#include "boost/unordered_map.hpp"
#include "core/database/src/CommonType.h"

namespace TA_Base_Core
{
    class SQLFileHelper
    {
    public:

        SQLFileHelper(const char* pszSQLFile = "");

        /**
         * GetSQLString
         *
         * @brief: to retrieve the SQL statement from the hash-table which
         *         store the SQL statements loaded from the sql.dat file.
         *         It's match the SQL statement based on the input SQL Key
         *
         * @param  pszSQLID  the SQL statement key which map into
         *                   the sql.dat file
         * Returns the SQL statement.
         */
        void getSQLString(const std::string& strSQLKey, SQLStatement& rSqlStatements);

    private:

        void   init();

    private:

        boost::unordered_map<std::string, size_t> m_hSQLHashIndex;
    };

    using SQLFileHelperPtr = std::shared_ptr<SQLFileHelper>;
}
