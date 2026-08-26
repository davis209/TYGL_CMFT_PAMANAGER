//////////////////////////////////////////////////////////////////////
///     @file       SQLFileHelper.cpp
///     @author     zhilin,ouyang
///     @date       2010-12-9 14:44:41
///
///     @brief      to load the sql.dat file and store the SQL statements
///                 into a hash-table for later retrieve.
//////////////////////////////////////////////////////////////////////

#include "SQLFileHelper.h"
#include "core/exceptions/src/SQLException.h"
#include "core/utilities/src/DebugUtil.h"
#include "sql.h"

#define  defSQLFileName        "sql.dat"
#define  defSQLTempFilePre     "sql_"
#define  defSQLTempFileEnd     ".txt"
#define  defSQLCommonSection   "[[common]]"
#define  defSQLOracleSection   "[[oracle]]"
#define  defSQLMySQLSection    "[[mysql]]"
#define  defSectionWordLen      20

namespace TA_Base_Core
{
    SQLFileHelper::SQLFileHelper(const char*)
    {
        init();
    }

    void  SQLFileHelper::getSQLString(const std::string& strSQLKey, SQLStatement& stmt)
    {
        auto it = m_hSQLHashIndex.find(strSQLKey);

        if (it == m_hSQLHashIndex.end())
        {
            LOG_ERROR("SQLFileHelper::GetSQLString() the SQLKey cannot match in Hash table. strSQLKey: %s", strSQLKey);
            TA_THROW(BadParamCount("the SQLKey cannot match in Hash table"));
        }

        auto& data = s_SQLArray[it->second];

        stmt.strSQLID = strSQLKey;
        stmt.strCommon = data.pCommonSQL;
        stmt.strOracle = data.pOracleSQL;
        stmt.strMySQL = data.pMySQLSQL;
        stmt.strSQLite = data.pSQLiteSQL;
    }

    void SQLFileHelper::init()
    {
        auto size = sizeof(s_SQLArray) / sizeof(*s_SQLArray);

        for (size_t i = 0; i < size; ++i)
        {
            m_hSQLHashIndex.emplace(s_SQLArray[i].pSQLID, i);
        }
    }
}
