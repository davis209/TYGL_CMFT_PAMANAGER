//////////////////////////////////////////////////////////////////////
///     @file       SQLCode.h
///     @author     zhilin,ouyang
///     @date       2010-11-29 14:44:41
///
///     @brief      to hold all of the SQL statements used in our
///                 system in one location. The SQLCode class is based
///                 on the Singleton design pattern.
//////////////////////////////////////////////////////////////////////

#pragma once
#include "SQLCommonDef.h"
#include "SQLVarParms.h"
#include "sqlmacrodef.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/database/src/CommonType.h"

namespace TA_Base_Core
{
    class SQLFileHelper;
    using SQLFileHelperPtr = std::shared_ptr<SQLFileHelper>;

    class SQLCode
    {
    public:

        SQLCode();

        // get the only instance of the class
        static SQLCode& getInstance();
        static void removeInstance();

        /**
         * buildSQLStatement
         *
         * @brief: to build the SQL statement.
         * @param: varParms contains the SQL ID, variables in sequence.
         *
         * Returns the SQL statement if successfully otherwise empty.
         */
        void  buildSQLStatement(const SQLVarParms& varParms, SQLStatement& rSqlStatement);

    private:

        enum enumSQLType
        {
            enumCommon_SQL = 0,
            enumOracle_SQL,
            enumMySQL_SQL,
            enumSQLite_SQL,
            enumEnd_SQL
        };

    private:

        void    getDbTypeAndSQLKey(const SQLVarParms& varParms, std::string& strSQLKey);
        void    getSQLFormat(const std::string& strSQLKey, SQLStatement& strSQLFormats);
        void    getSQLID(const std::string& strSQLKey, SQLStatement& rSqlStatement);
        void    getSQLID(const SQLStatement& rSrcSQL, SQLStatement& rSqlStatement);
        bool    checkAQSQL(SQLStatement& strSQLFormats);
        void    printSQL(const std::string& strSQLKey, SQLStatement& strSQLFormats);
        void    buildNormalSQL(const SQLVarParms& varParms, SQLStatement& rSQLFormats, SQLStatement& rSqlStatement);
        size_t  getSQLSize(const SQLVarParms& varParms, const std::string& strFormat, int nSQLType = enumCommon_SQL);
        void    buildSQL(const SQLVarParms& varParms, const std::string& strFormat, std::string& strSQL, int nSQLType = enumCommon_SQL);
        void    buildAQSQL(const SQLVarParms& varParms, SQLStatement& rSQLFormats, SQLStatement& rSqlStatement);
        void    buildMultiSQL(const std::string& strOracle, std::string& strSqlite, std::string& strMySQL);

        //#ifdef TDS_DEVELOP
        void    buildTDSAQSQL(SQLStatement& rSqlStatement,
                              const std::string& strOracle,
                              const std::string& strMySQL,
                              const std::string& strSqlite);
        //#endif

        static SQLCode* m_pInstance;               // the only one instance of the class
        SQLFileHelperPtr  m_pSqlFileHelper;          // the SQL file helper

        ReEntrantThreadLockable  m_threadLock;
    };
}
