#pragma once

#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <functional>

#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App::STIS_PROTOCOL::IMPL::stisdatabase::detail
{
    using Rows = std::vector<std::vector<std::string>>;

    struct IDataDeleter
    {
        void operator()(TA_Base_Core::IData* data) const
        {
            delete data;
        }
    };

    using IDataPtr = std::unique_ptr<TA_Base_Core::IData, IDataDeleter>;

    inline TA_Base_Core::IDatabase* get_stis_database(TA_Base_Core::EDataTypes type, TA_Base_Core::EDataActions action)
    {
        return TA_Base_Core::DatabaseFactory::getInstance().getDatabase(type, action);
    }

    inline TA_Base_Core::IDatabase* get_stis_read_database(TA_Base_Core::EDataTypes type = TA_Base_Core::Tis_Cd)
    {
        return get_stis_database(type, TA_Base_Core::Read);
    }

    inline TA_Base_Core::IDatabase* get_stis_write_database(TA_Base_Core::EDataTypes type = TA_Base_Core::Tis_Cd)
    {
        return get_stis_database(type, TA_Base_Core::Write);
    }

    inline std::string sql_quote(const std::string& value)
    {
        std::string escaped;
        escaped.reserve(value.size() + 2);
        escaped.push_back('\'');
        for (char c : value)
        {
            if (c == '\'')
            {
                escaped += "''";
            }
            else
            {
                escaped.push_back(c);
            }
        }
        escaped.push_back('\'');
        return escaped;
    }

    inline std::string sql_string(const std::string& value)
    {
        return sql_quote(value);
    }

    inline std::string sql_int(int value)
    {
        std::ostringstream ss;
        ss << value;
        return ss.str();
    }

    inline TA_Base_Core::SQLStatement raw_sql(const std::string& sql)
    {
        TA_Base_Core::SQLStatement statement;

        // Most TransActive branches expose SQLStatement::strCommon for database-neutral SQL.
        // If your branch represents SQLStatement differently, change only this helper.
        statement.strCommon = sql;

        return statement;
    }

    inline void for_each_row(TA_Base_Core::IDatabase* database,
                             TA_Base_Core::IData* data,
                             const std::function<void(TA_Base_Core::IData&, unsigned long)>& row_func)
    {
        IDataPtr holder(data);
        bool more_data = true;

        while (more_data && holder.get() != NULL)
        {
            for (unsigned long i = 0; i < holder->getNumRows(); ++i)
            {
                row_func(*holder, i);
            }

            TA_Base_Core::IData* next = NULL;
            more_data = database->moreData(next);
            holder.reset(next);
        }
    }

    inline void execute_modification(TA_Base_Core::IDatabase* database, const std::string& sql)
    {
        TA_Base_Core::SQLStatement statement = raw_sql(sql);
        database->executeModification(statement);
    }

    inline TA_Base_Core::IData* execute_query(TA_Base_Core::IDatabase* database,
                                              const std::string& sql,
                                              const std::vector<std::string>& columns)
    {
        TA_Base_Core::SQLStatement statement = raw_sql(sql);
        return database->executeQuery(statement, columns);
    }
}
