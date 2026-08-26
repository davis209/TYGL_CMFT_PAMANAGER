/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source: $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/SimpleDbDatabase.h $
 * @author Nick Jardine
 * @version $Revision: #3 $
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by: Nick Jardine
 *
 * SimpleDbDatabase is an implementation of IDatabase, and is responsible for executing queries
 * on the database. It does not analyse the returned data - that is the responsiblility of
 * SimpleDbData.
 * SimpleDbDatabase is specific to SimpleDb and will not work for any other database library.
 *
 */

#if !defined(SimpleDbDatabase_CB29E09F_5CF3_4639_BAF7_DD2E3AE25EC5__INCLUDED_)
#define SimpleDbDatabase_CB29E09F_5CF3_4639_BAF7_DD2E3AE25EC5__INCLUDED_

#include "IDatabase.h"
#include "DatabaseFactory.h"
#include "core/database/src/SimpleDb.h"
#include <string>

// class AbstractOCIType;

namespace TA_Base_Core
{
    //TD17205,jianghp
    const static ta_int32 INVALID_USERNAME_PASSWORD = 1017;

    class SimpleDbDatabase : public IDatabase
    {
    public:

        SimpleDbDatabase() = default;

        /**
         * SimpleDbDatabase (constructor)
         *
         * Consturcts a new instance of SimpleDbDatabase, and initialises the database pointer
         * to null.
         */
        SimpleDbDatabase(EDataTypes dataType, EDataActions dataAction)
            : m_db(nullptr),
              m_dataType(dataType),
              m_dataAction(dataAction)
        {
        }

        virtual ~SimpleDbDatabase();

        SimpleDbDatabase(const SimpleDbDatabase&) = delete;
        SimpleDbDatabase& operator=(const SimpleDbDatabase&) = delete;

        /**
         * connect
         *
         * This method creates a connection to the database using the SimpleDb library.
         * It will place the SimpleDb connection in the member variable m_db
         *
         * @exception DatabaseException This exception will be thrown under the following circumstances:
         *            1) The connection string is invalid
         *            2) The database does not successfully open
         *
         */
        virtual void connect(const std::string& connectionString);

        /**
         * disconnect
         *
         * This method closes the connection to the database.
         *
         */
        virtual void disconnect();

        virtual void cleanQuery();

        /**
         * getSqlString
         *
         * This method get the real SQL string for the connected database.
         *  or return empty string if invalid.
         *
         */
        virtual std::string getSqlString(const SQLStatement& sqlStatement);

        /**
         * executeQuery
         *
         * This method carries out the specified query and stores the retrieved data in an IData
         * object that is returned to the client.
         *
         * @param sql The sql query to execute. This query must not be null.
         * @param columnNames This is the vector of column names that will be used to store the
         * retrieved data. This vector must not be empty.
         * @param numRows This is an optional parameter. It is the number of rows to store in the
         * IData object. By default this is set to SimpleDb::MAX_ROWS. For SimpleDb, this number must be between
         * 1 and SimpleDb::MAX_ROWS
         *
         * @return The IData object pointer returned contains the data retrieved from the database.
         * NB: It is the responsibility of the *client* to delete the IData object.
         *
         * @exception DatabaseException This exception will be thrown under the followin circumstances:
         *            1) The database has inadvertantly been closed, and cannot be reopend.
         *            2) The exectuion of the sql query fails.
         *
         *
         * pre: sql.size() > 0;
         *      connect() MUST be called prior to this method
         *      0 < numRows <= SimpleDb::MAX_ROWS (the upper limit is a restriction on SimpleDb
         */
        virtual IData* executeQuery(const SQLStatement& rSqlObj, const std::vector<std::string>& columnNames, size_t numRows, const std::string& options = "");
        virtual IData* executeQuery(const SQLStatement& rSqlObj, const std::vector<std::string>& columnNames, const std::string& options = "");

        /**
         * executeModification
         *
         * This method carries out the specified query. Unlike executeQuery(), this method will not
         * store any returned data, and as such should be used for SQL INSERT, DELETE, or UPDATE
         * statements
         *
         * @param sql The sql query to execute. This query must not be null.
         *
         * @exception DatabaseException This exception will be thrown under the followin circumstances:
         *            1) The database has inadvertantly been closed, and cannot be reopend.
         *            2) The exectuion of the sql query fails.
         *
         * pre: sql.size() > 0;
         *      connect() MUST be called prior to this method
         */
        virtual void executeModification(const SQLStatement& rSqlObj);

        /**
         * executeProcedure
         *
         * This method CANNOT be used with SimpleDb. It will fail an assertion on any call.
         *
         * Note for review: Can this be made private? And if so, will it be picked up as an
         * error at compile time as the IDatabase interface declares it public.
         *
         */
        virtual IData* executeProcedure(const SQLStatement& rSqlObj, const std::vector<std::string>& columnNames,
                                        const ta_uint32 numRows = SimpleDb::MAX_ROWS);

        /**
         * executeProcedure
         *
         * This method executes the speicifed procedure using the executeModification() method.
         * This procedure CANNOT take an INOUT or OUT parameter, or the query will fail.
         *
         * @param sql The sql query to execute. This query must not be null.
         *
         * @exception DatabaseException This exception will be thrown under the followin circumstances:
         *            1) The database has inadvertantly been closed, and cannot be reopend.
         *            2) The exectuion of the sql query fails.
         *
         * pre: sql.size() > 0;
         *      connect() MUST be called prior to this method
         */
        virtual void executeProcedure(const SQLStatement& rSqlObj);

        /**
         * moreData
         *
         * This method determines if there is any more data to be retrieved for the current
         * query, and creates an IData object if necessary.
         *
         * @param returnData This is a pointer to an IData object. It MUST be a null pointer,
         * as this method will overwrite it (if not, a memory leak will occur). This is confirmed
         * by the use of a TA_ASSERT.
         *
         * @returns true iff there was data to return, and the IData pointer has been instantiated.
         * If it returns false, then the IData object passed in will not be changed.
         * If it returns true, then the IData object passed in contains the next lot of data.
         *
         * @exception DatabaseException This exception will be thrown under the followin circumstances:
         *            1) The database has inadvertantly been closed, and cannot be reopend.
         *            2) The exectuion of the sql query fails.
         *
         * pre: returnData == NULL
         *      connect() MUST be called prior to this method
         */
        virtual bool moreData(IData*& returnData);

        /**
         * readBLOB
         *
         * This method reads a single row from a single BLOB (Binary Large Object) field into
         * the vector parameter.
         *
         * @param fieldName     the name of the BLOB field to read from
         * @param tableName     the name of the table that the field is a part of
         * @param whereClause   SQL clause to specify the exact row to read from
         * @param data          the BLOB data will be read into this vector
         *
         * @exception DatabaseException This exception will be thrown under the following circumstances:
         *            1) The database has inadvertantly been closed, and cannot be reopend.
         *            2) The exectuion of the sql query fails.
         *
         */
        virtual void readBLOB(const std::string& fieldName, const std::string& tableName,
                              const std::string& whereClause, std::vector<unsigned char>& data);

        /**
         * writeBLOB
         *
         * This method updates a single BLOB (Binary Large Object) field in a single row from
         * the vector parameter.
         *
         * @param fieldName     the name of the BLOB field to write to
         * @param tableName     the name of the table that the field is a part of
         * @param whereClause   SQL clause to specify the exact row to write to
         * @param data          the BLOB data will be read from this vector
         *
         * @exception DatabaseException This exception will be thrown under the following circumstances:
         *            1) The database has inadvertantly been closed, and cannot be reopend.
         *            2) The exectuion of the sql query fails.
         *
         */
        virtual void writeBLOB(const std::string& fieldName, const std::string& tableName,
                               const std::string& whereClause, std::vector<unsigned char>& data);

        /**
         * beginTransaction
         *
         * Implemented to support IDatabase, but SimpleDb does not support transactions, so
         * this will simply assert false.
         */
        virtual void beginTransaction();

        /**
         * commitTransaction
         *
         * Implemented to support IDatabase, but SimpleDb does not support transactions, so
         * this will simply assert false.
         */
        virtual void commitTransaction();

        /**
         * rollbackTransaction
         *
         * Implemented to support IDatabase, but SimpleDb does not support transactions, so
         * this will simply assert false.
         */
        virtual void rollbackTransaction();

        /**
         * escapeInsertString
         *
         * Parses a string for insertion into the database, and returns it with any special
         * characters escaped.
         * Note: This method assumes that the string passed in has NOT had any escaping
         * already applied. If it does, the items will be escaped a second time.
         *
         * @param theString The string to parse
         *
         * @return The input string with escape characters added where and if necessary.
         */
        std::string escapeInsertString(const std::string& theString);

        /**
         * escapeQueryString
         *
         * Parses a string for use in a SELECT query, and returns it with any special
         * characters escaped.
         * Note: 1) This method assumes that the string passed in has NOT had any escaping
         * already applied. If it does, the items will be escaped a second time.
         *       2) For the Orcale implementation of SQL, there is no pre-defined escape
         * character for the "_" and "%" characters within "select...like..." statements.
         * Due to this problem, this method will only escape the standard single-quote character
         * as done for insert statements.
         *
         * @param theString The string to parse
         *
         * @return The string with escape characters added where and if necessary.
         */
        std::string escapeQueryString(const std::string& theString);

        /**
         * escapeAQSQLString
         *
         * Parses a string for use in a special AQ and returns it with any special
         * characters escaped. (use for Distributed DB)
         * @return std::string
         * @param : const std::string& theString
         *
         * @exception <exceptions> Optional
         */
        std::string escapeAQSQLString(const std::string& theString);

        /**
         * getNextPkey
         *
         * get next pkey for insert sql script
         * @return int
         * @param : const std::string& str, it is sequence name
         *
         * @exception <exceptions> Optional
         */
        std::string getNextPkey(const std::string& strSeqName);

		/**
		 * getNextPkeyDynamic
		 *
		 * get next pkey for insert msyql script
		 * @return int
		 * @param : const std::string& str, it is sequence name
		 *
		 * @exception <exceptions> Optional
		 */
		std::string getNextPkeyDynamic(const std::string& strSeqName);

        /**
         * GetDbServerType
         *
         * This method get the database type that currently in used.
         *
         */
        virtual EDatabaseServerType getDbServerType();

        /**
         * setDataTypeAction
         *
         * This method set the datatype and action for the current thread,
         * each thread may has different datatype and action for each access.
         * the purpose is to avoid the datatype and action wrong during failover.
         *
         */
        virtual void setDataTypeAction(EDataTypes dataType, EDataActions dataAction);

        /**
         * prepareSQLStatement
         *
         * @brief: to prepare the SQL statement, this method accept the
         *         SQLVarParms object which used to construct the SQL
         *         statement into a vector.
         *
         * @param varParms    the SQLVarParms object to construct the
         *                    SQL statement into a vector
         * Returns the SQL statement as a std::string.
         */
        using IDatabase::prepareSQLStatement;
        void prepareSQLStatement(SQLStatement& rSqlStatement, const SQLVarParms& varParms) override;

    private:

        //#ifdef TDS_DEVELOP
        /*parser the sql string, if the str is like select then add ';' and the end of the string and '\n'
        if the str is like "begin **** end;" then add ';' and \n and add '/'
        add by hongzhi
        */
        void ParserSQLString(std::string& str);         //for TDS CE
        void ParserCallPkgString(std::string& str);     //for TDS CE

        void _GetBothSQLFromObj(const SQLStatement& rSqlStatement, std::string& strOra, std::string& strMy);    //for TDS CE
        //#endif
        void tryToOpenDb();

        const std::string& makePrettyCallstackId();

    private:

        SimpleDb* m_db;                             // The database connection
        std::vector<std::string> m_columnNames;     // Local copy of the vector of column names
        ta_uint32 m_numRows;                        // Number of rows of data to retrieve at a time
        EDataTypes m_dataType;                      // Required for the factory in case the db goes bad
        EDataActions m_dataAction;                  // Required for the factory in case the db goes bad
    };
} // closes TA_Base_Core

#endif // !defined(SimpleDbDatabase_CB29E09F_5CF3_4639_BAF7_DD2E3AE25EC5__INCLUDED_)
