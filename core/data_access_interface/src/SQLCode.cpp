//////////////////////////////////////////////////////////////////////
///     @file       SQLCode.cpp
///     @author     zhilin,ouyang
///     @date       2010-11-29 14:44:41
///
///     @brief      to hold all of the SQL statements used in our
///                 system in one location. The SQLCode class is based
///                 on the Singleton design pattern.
//////////////////////////////////////////////////////////////////////
#include "SQLCode.h"
#include "SQLFileHelper.h"
#include "sqlmacrodef.h"
#include "core/database/src/DatabaseUtil.h"
#include "core/database/src/SimpleSQLConverter.h"
#include "core/exceptions/src/SQLException.h"
#include "core/exceptions/src/ThrowException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/StaticObject.h"
#include "core/synchronisation/src/DoubleCheckedLocking.h"
#include <stdio.h>

namespace TA_Base_Core
{
    static const char*  s_pcAQOraclePrefix = "BEGIN audit_data_queue_pkg.enqueue_audit_data";
    static const char*  s_pcAQOracleSubfix = "; END;";
    static const char*  s_pcAQMySQLPrefix = "CALL prc_enqueue_audit_data";
    static const char*  s_pcAQMySQLSubfix = "";
    static const char*  s_pcAQTag = "AQ:";
    static const char*  s_pcAQOracleDelim = "||";
    static const char*  s_pcAQMySQLDelim = ",";
    static const char*  s_pcAQMySQLConStr = "CONCAT(";
    static const char*  s_pcAQStrTag = "'";
    static const size_t s_nAQTagLen = 3;
    static const size_t s_nStrBeginPos = 0;

    SQLCode* SQLCode::m_pInstance = 0;
    using SQLCodeInstanceLock = StaticObject<NonReEntrantThreadLockable, SQLCode>;

    SQLCode& SQLCode::getInstance()
    {
        return *double_checked_locking_new(m_pInstance, SQLCodeInstanceLock::value());
    }

    void SQLCode::removeInstance()
    {
        double_checked_locking_delete(m_pInstance, SQLCodeInstanceLock::value());
    }

    SQLCode::SQLCode()
    {
        LOG_SQL("Begin build the Hash table...");

        m_pSqlFileHelper = std::make_shared<SQLFileHelper>();

        LOG_SQL("End build the Hash table...");
    }

    void SQLCode::buildSQLStatement(const SQLVarParms& varParms, SQLStatement& rSqlStatement)
    {
        FUNCTION_ENTRY("SQLCode::buildSQLStatement()");
        std::string strSQLKey;
        SQLStatement SQLFormats;
        int nDbType = 0;
        size_t uiTotalSQLSize = 0;

        try
        {
            size_t uVarCount = varParms.size();

            if (uVarCount < defMINPARAMSIZE)
            {
                TA_THROW(BadParamCount("the PrepareStatement parameter count error"));
            }

            getDbTypeAndSQLKey(varParms, strSQLKey);
            getSQLFormat(strSQLKey, SQLFormats);
            getSQLID(SQLFormats, rSqlStatement);

            if (checkAQSQL(SQLFormats))  // antonqiu: even if database SQLite, if the SQL for MySQL and Oracle contain "AQ:" it will still be processed as AQSQL...
            {
                buildAQSQL(varParms, SQLFormats, rSqlStatement);
            }
            else
            {
                buildNormalSQL(varParms, SQLFormats, rSqlStatement);
            }

            printSQL(strSQLKey, rSqlStatement);
        }
        catch (BadParamCount* e)
        {
            TA_THROW(SQLCodeException(e->what()));
        }
        catch (BadIndex* e)
        {
            TA_THROW(SQLCodeException(e->what()));
        }
        catch (DbTypeNotSupported* e)
        {
            TA_THROW(SQLCodeException(e->what()));
        }
        catch (std::exception& e)
        {
            TA_THROW(SQLCodeException(e.what()));
        }
        catch (...)
        {
            TA_THROW(SQLCodeException("Unknown SQLCode exception"));
        }

        FUNCTION_EXIT;
    }

    void  SQLCode::getDbTypeAndSQLKey(const SQLVarParms& varParms, std::string& strSQLKey)
    {
        strSQLKey = varParms[0].data(enumBeginDbType);
    }

    size_t  SQLCode::getSQLSize(const SQLVarParms& varParms, const std::string& strFormat, int nSQLType)
    {
        size_t uiTotalSize = strFormat.size();
        size_t uVarCount = varParms.size();

        if (defMINPARAMSIZE == uVarCount)
        {
            return uiTotalSize;
        }

        size_t i = defMINPARAMSIZE;

        for (; i < uVarCount; i++)
        {
            uiTotalSize += varParms[i].length(nSQLType);
        }

        return uiTotalSize;
    }

    void  SQLCode::buildSQL(const SQLVarParms& varParms, const std::string& strSQLFormat, std::string& strSQL, int nSQLType)
    {
        if (strSQLFormat.empty())
        {
            return;
        }

        size_t uVarCount = varParms.size();

        if (0 == uVarCount)
        {
            strSQL.clear();
        }
        else if (defMINPARAMSIZE == uVarCount)
        {
            strSQL = strSQLFormat;
        }
        else
        {
            try
            {
                boost::format format(strSQLFormat);

                for (size_t i = 1; i < varParms.size(); ++i)
                {
                    format % varParms[i].data(nSQLType);
                }

                strSQL = format.str();
            }
            catch (std::exception& e)
            {
                std::vector<std::string> args;
                boost::find_all(args, strSQLFormat, "'%s'");
                LOG_ERROR("buildSQL(): failed to build sql for %s database, format is(%d args required, %d args provided): %s", DatabaseUtil::toDatabaseServerTypeString(nSQLType), args.size(), varParms.size() - 1, strSQLFormat);
                TA_RETHROW_INFO(e);
            }
        }
    }

    void  SQLCode::getSQLFormat(const std::string& strSQLKey, SQLStatement& strSQLFormats)
    {
        TA_THREADGUARD(m_threadLock); // avoid threads racing
        TA_ASSERT(NULL != m_pSqlFileHelper, "sql file helper handler is null.");
        m_pSqlFileHelper->getSQLString(strSQLKey, strSQLFormats);

        if (strSQLFormats.strCommon.empty() && strSQLFormats.strMySQL.empty() && strSQLFormats.strOracle.empty() && strSQLFormats.strSQLite.empty())
        {
            TA_THROW(BadParamCount("Cannot find the SQL statement in the hash-table"));
        }
    }

    void  SQLCode::getSQLID(const std::string& strSQLKey, SQLStatement& rSqlStatement)
    {
        rSqlStatement.nSQLID = atoi(strSQLKey.c_str());
    }

    void  SQLCode::getSQLID(const SQLStatement& rSrcSQL, SQLStatement& rSqlStatement)
    {
        rSqlStatement.strSQLID = rSrcSQL.strSQLID;
    }

    bool  SQLCode::checkAQSQL(SQLStatement& strSQLFormats)
    {
        bool bRetCode = false;

        if (strSQLFormats.strMySQL.empty() && strSQLFormats.strOracle.empty())
        {
            return bRetCode;
        }

        if (boost::iequals(strSQLFormats.strMySQL.substr(0, s_nAQTagLen), s_pcAQTag))
        {
            bRetCode = true;
        }

        return bRetCode;
    }

    void  SQLCode::printSQL(const std::string& strSQLKey, SQLStatement& rSqlStatement)
    {
        LOGLARGESTRING_SQL_IF(rSqlStatement.strCommon.size(), "SQLID: %s, SQL: %s", strSQLKey, rSqlStatement.strCommon);
        LOGLARGESTRING_SQL_IF(rSqlStatement.strMySQL.size(), "SQLID: %s, MySQL SQL: %s", strSQLKey, rSqlStatement.strMySQL);
        LOGLARGESTRING_SQL_IF(rSqlStatement.strOracle.size(), "SQLID: %s, Oracle SQL: %s", strSQLKey, rSqlStatement.strOracle);
        LOGLARGESTRING_SQL_IF(rSqlStatement.strSQLite.size(), "SQLID: %s, SQLite SQL: %s", strSQLKey, rSqlStatement.strSQLite);
    }

    void  SQLCode::buildAQSQL(const SQLVarParms& varParms, SQLStatement& rSQLFormats, SQLStatement& rSqlStatement)
    {
        std::string strTemSQL, strOracle, strMySQL, strSQLite;
        int  nDbT = enumBeginDbType;
        int  nInsertPos = 0;

        while (++nDbT != enumEndDbType)
        {
            switch (nDbT)
            {
                case enumOracleDb:
                    buildSQL(varParms, rSQLFormats.strOracle, strTemSQL, enumOracle_SQL);
                    strOracle = strTemSQL.substr(s_nAQTagLen);
                    break;

                case enumMysqlDb:
                    buildSQL(varParms, rSQLFormats.strMySQL, strTemSQL, enumMySQL_SQL);
                    strMySQL = strTemSQL.substr(s_nAQTagLen);
                    break;

                case enumSqliteDb:
                    if (rSQLFormats.strOracle.size() && rSQLFormats.strSQLite.empty())
                    {
                        rSQLFormats.strSQLite = SimpleSQLConverter::oracle_to_sqlite(rSQLFormats.strOracle);
                        boost::replace_all(rSQLFormats.strSQLite, "''", "'");
                    }

                    if (rSQLFormats.strSQLite.empty())
                    {
                        strSQLite = "";
                    }
                    else
                    {
                        buildSQL(varParms, rSQLFormats.strSQLite, strTemSQL, enumSQLite_SQL);
                        strSQLite = strTemSQL.substr(s_nAQTagLen);
                    }

                    break;
            }
        }

        //#ifdef TDS_DEVELOP
        if ((TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME, "--quiet").compare("ConfigurationEditor") == 0) &&
                (TA_Base_Core::RunParams::getInstance().isSet(RPARAM_TDSCE, "--quiet")))
        {
            buildTDSAQSQL(rSqlStatement, strOracle, strMySQL, strSQLite);
        }

        //#endif

        buildMultiSQL(strOracle, strSQLite, strMySQL);

        rSqlStatement.strOracle = s_pcAQOraclePrefix;
        rSqlStatement.strOracle += strMySQL;   // why are we using MySQL string for Oracle?
        rSqlStatement.strOracle += s_pcAQOracleSubfix;

        rSqlStatement.strMySQL = s_pcAQMySQLPrefix;
        rSqlStatement.strMySQL += strMySQL;

        // SQLite currently is not supported in prc_enqueue_audit_data
        //rSqlStatement.strSQLite = s_pcAQMySQLPrefix;
        //rSqlStatement.strSQLite += strSQLite;
        rSqlStatement.strSQLite = strSQLite;    // only normal statement without the AQ: format
    }

    void  SQLCode::buildNormalSQL(const SQLVarParms& varParms, SQLStatement& rSQLFormats, SQLStatement& rSqlStatement)
    {
        buildSQL(varParms, rSQLFormats.strCommon, rSqlStatement.strCommon);

        // build MySQL SQL statement
        buildSQL(varParms, rSQLFormats.strMySQL, rSqlStatement.strMySQL, enumMySQL_SQL);

        // build Oracle SQL Statement
        buildSQL(varParms, rSQLFormats.strOracle, rSqlStatement.strOracle, enumOracle_SQL);

        if (rSQLFormats.strOracle.size() && rSQLFormats.strSQLite.empty())
        {
            rSQLFormats.strSQLite = SimpleSQLConverter::oracle_to_sqlite(rSQLFormats.strOracle);
        }

        // build SQLite SQL Statement
        buildSQL(varParms, rSQLFormats.strSQLite, rSqlStatement.strSQLite, enumSQLite_SQL);

        //#ifdef TDS_DEVELOP
        if ((RunParams::getInstance().get(RPARAM_ENTITYNAME, "--quiet").compare("ConfigurationEditor") == 0) &&
                (RunParams::getInstance().isSet(RPARAM_TDSCE, "--quiet")))
        {
            rSqlStatement.bIsTdsAQ = false;
        }

        //#endif
    }

    // strSqlite is not constant as it can be modified to remove the AQ format.
    void  SQLCode::buildMultiSQL(const std::string& strOracle, std::string& strSqlite, std::string& strMultiSQL)
    {
        std::string::size_type  nTempPos = 0, nTempEnd = 0, nInsertPos = 0;
        std::string strTempSQL;

        nTempPos = strOracle.find_first_of(defAQSQL_Demit);
        nTempPos = strOracle.find_first_of(defAQSQL_Demit, nTempPos + 1);
        nTempEnd = strOracle.find_last_of(defAQSQL_EndTag);
        strTempSQL = strOracle.substr(nTempPos + 1, (nTempEnd - nTempPos - 1));
        strTempSQL += defAQSQL_Demit;

        // Currently, SQLite is not supported in prc_enqueue_audit_data
        nTempPos = strSqlite.find_first_of(defAQSQL_Demit);                     // after database name
        nTempPos = strSqlite.find_first_of(defAQSQL_Demit, nTempPos + 1);       // after 'PUBLIC', start of the SQL statement
        nTempPos = strSqlite.find_first_of(defAQSQL_SingleQuote, nTempPos + 1); // remove starting quote
        nTempEnd = strSqlite.find_last_of(defAQSQL_EndTag);                     // end of SQL Statement
        nTempEnd = strSqlite.find_last_of(defAQSQL_SingleQuote, nTempEnd - 1);  // remove end quote

        strSqlite = strSqlite.substr(nTempPos + 1, (nTempEnd - nTempPos - 1));  // Remove the AQ: formatting for SQLite
        //  strTempSQL += strSqlite.substr(nTempPos + 1, (nTempEnd - nTempPos - 1));    // not included in the call to prc_enqueue_audit_data
        //  strTempSQL += defAQSQL_Demit;

        nInsertPos = strMultiSQL.find_first_of(defAQSQL_Demit);
        nInsertPos = strMultiSQL.find_first_of(defAQSQL_Demit, nInsertPos + 1);
        nInsertPos++;

        strMultiSQL.insert(nInsertPos, strTempSQL);
    }

    //#ifdef TDS_DEVELOP
    void  SQLCode::buildTDSAQSQL(SQLStatement& rSqlStatement,
                                 const std::string& strOracle,
                                 const std::string& strMySQL,
                                 const std::string& strSqlite)
    {
        std::string::size_type  nTempPos = 0, nTempEnd = 0;

        rSqlStatement.bIsTdsAQ = true;
        nTempPos = strOracle.find_first_of(defAQSQL_Demit);
        nTempPos = strOracle.find_first_of(defAQSQL_Demit, nTempPos + 1);
        nTempPos = strOracle.find_first_of(s_pcAQStrTag, nTempPos);
        nTempEnd = strOracle.find_last_of(s_pcAQStrTag);
        rSqlStatement.strTDSOracle = strOracle.substr(nTempPos + 1, (nTempEnd - nTempPos - 1));

        nTempPos = strMySQL.find_first_of(defAQSQL_Demit);
        nTempPos = strMySQL.find_first_of(defAQSQL_Demit, nTempPos + 1);
        nTempPos = strMySQL.find_first_of(s_pcAQStrTag, nTempPos);
        nTempEnd = strMySQL.find_last_of(s_pcAQStrTag);
        rSqlStatement.strTDSMySQL = strMySQL.substr(nTempPos + 1, (nTempEnd - nTempPos - 1));

        nTempPos = strSqlite.find_first_of(defAQSQL_Demit);
        nTempPos = strSqlite.find_first_of(defAQSQL_Demit, nTempPos + 1);
        nTempPos = strSqlite.find_first_of(s_pcAQStrTag, nTempPos);
        nTempEnd = strSqlite.find_last_of(s_pcAQStrTag);
        rSqlStatement.strTDSSQlite = strSqlite.substr(nTempPos + 1, (nTempEnd - nTempPos - 1));
    }
    //#endif
}
