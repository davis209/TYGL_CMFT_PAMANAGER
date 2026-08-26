/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/DatabaseFactory.cpp $
 * @author:  Nick Jardine
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * DatabaseFactory is a singleton that returns a pointer to an IDatabase object
 * The database object that it returns will always be connected.
 */

#ifdef __WIN32__
    #pragma warning(disable:4786)
    #pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include "DataTypesLookup.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/DbConnection.h"
#include "core/exceptions/src/DbConnectionFailed.h"
#include "core/exceptions/src/ThrowException.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/threads/src/Thread.h"
#include "core/data_access_interface/src/DbConnectionStrings.h"
#include "core/synchronisation/src/DoubleCheckedLocking.h"
#include <boost/algorithm/string/predicate.hpp>

using namespace std::string_literals;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::Thread;

namespace TA_Base_Core
{
    DatabaseFactory* DatabaseFactory::m_instance = 0;
    NonReEntrantThreadLockable DatabaseFactory::m_instanceLock;

    DatabaseFactory::~DatabaseFactory()
    {
        cleanup();
    }

    DatabaseFactory& DatabaseFactory::getInstance()
    {
        return *double_checked_locking_new<DatabaseFactory>(m_instance, m_instanceLock);
    }

    void DatabaseFactory::removeInstance()
    {
        double_checked_locking_delete(m_instance, m_instanceLock);
    }

    IDatabase* DatabaseFactory::getDatabase(EDataTypes dataType, EDataActions dataAction, const std::string& options)
    {
        FUNCTION_ENTRY("getDatabase");
        LOG_CALLSTACK(boost::format("DatabaseFactory::getDatabase[%s:%s%s]") % getDataTypesStr(dataType) % getDataActionsStr(dataAction) % (options.size() ? ":" + options : ""s));
        // Set up a local variable to point to the database that is either created or found
        IDatabase* theDatabase = NULL;

        TA_THREADGUARD(m_getDatabaseLock);

        // get the connection string for this datatype/action
        std::string dbConnectionString;

        // forever loop will exit by exception or when a good db is found
        while (1)
        {
            // If this call fails it will throw something.
            try
            {
                DbConnection::getInstance().getConnectionString(dataType, dataAction, dbConnectionString, options);
            }
            catch (DbConnectionFailed&)
            {
                // we exit the forever loop with this throw
                TA_THROW(DatabaseException("Unable to find a working database"));
            }
            catch (std::exception& ex)
            {
                TA_RETHROW_INFO(ex);
            }
            catch (...)
            {
                TA_RETHROW_INFO();
            }

            ThreadMap::iterator threadIter;
            ConnectionMap::iterator connIter;

            try
            {
                // Get the ID of this thread - we want one database connection per thread
                int threadId = Thread::getCurrentThreadId();

                // Look to see if there is already a database connection for this thread
                //<ThreadID: Map<ConnectionString, SimpleDBDatabase>>
                threadIter = m_databaseMap.find(threadId);

                if (threadIter == m_databaseMap.end())
                {
                    // add new ConnectionMap. Set db* to null untill it's defined
                    ConnectionMap cmap;
                    cmap.insert(ConnectionMap::value_type(dbConnectionString, 0));
                    threadIter = m_databaseMap.insert(ThreadMap::value_type(threadId, cmap)).first;
                }

                connIter = threadIter->second.find(dbConnectionString);

                if (connIter == threadIter->second.end())
                {
                    connIter = threadIter->second.insert(ConnectionMap::value_type(dbConnectionString, 0)).first;
                }

                if (connIter->second != 0)
                {
                    // Then the connection already exists.
                    theDatabase = connIter->second; //TODO: need to set dataType and DataAction
                    theDatabase->setDataTypeAction(dataType, dataAction);
                }
                else // Database doesn't exist for this thread, so need to create it.
                {
                    LOG_INFO("New SimpleDbDatabase object created for thread %lu", threadId);

                    theDatabase = new TA_Base_Core::SimpleDbDatabase(dataType, dataAction);

                    // add to map
                    connIter->second = theDatabase;
                }
            }
            catch (std::exception& ex)
            {
                TA_RETHROW_INFO(ex);
            }
            catch (...)
            {
                TA_RETHROW_INFO();
            }

            // Now that we have a database object, we need to connect it - do this everytime
            // 'cause if the database is alerady connected, nothing will happen, but if it has
            // been inadvertandtly disconnected, then it will be reconnected
            try // The connect line can generate a DatabaseException
            {
                theDatabase->connect(dbConnectionString);

                // we can also exit the forever loop this (preferred) way

                FUNCTION_EXIT;
                return theDatabase;
            }
            catch (DatabaseException& ex)
            {
                // A DatabaseException was generated. Need to delete the database object for now.
                delete theDatabase;
                threadIter->second.erase(connIter);
                TA_Base_Core::Thread::sleep(100);
                // do not rethrow, just sleep for a while
                // there may be another database we can talk to.
                LOG_EXCEPTION("DatabaseException", ex.what());
            }
            catch (std::exception& ex)
            {
                LOG_EXCEPTION("std::exception", ex.what());
            }
            catch (...)
            {
                LOG_EXCEPTION("UNKNOWN", "");
            }
        } // end forever loop

        FUNCTION_EXIT;
    }

    IDatabase* DatabaseFactory::getDatabase(EDataTypes dataType, EDataActions dataAction, IDatabase* theDatabase)
    {
        FUNCTION_ENTRY("getDatabase");
        TA_THREADGUARD(m_getDatabaseLock);

        // get the connection string for this datatype/action
        std::string dbConnection;

        // forever loop will exit by exception or when a good db is found
        while (1)
        {
            // If this call fails it will throw something.
            try
            {
                DbConnection::getInstance().getConnectionString(dataType, dataAction, dbConnection);
            }
            catch (DbConnectionFailed&)
            {
                // we exit the forever loop with this throw
                throw (DatabaseException("Unable to find a working database"));
            }

            // The only reason to call this function is when a db connection has gone bad
            // We therfore try to connect every time
            try // The connect line can generate a DatabaseException
            {
                theDatabase->connect(dbConnection);

                // we can also exit the forever loop this (preferred) way
                FUNCTION_EXIT;
                return theDatabase;
            }
            catch (DatabaseException&)
            {
                // do not rethrow.
                // there may be another database we can talk to.
            }
        } // end forever loop
    }

    IDatabase* DatabaseFactory::getDatabase(const std::vector<EDataTypes>& dataTypes, EDataActions dataAction)
    {
        if (1 == dataTypes.size())
        {
            return getDatabase(dataTypes[0], dataAction);
        }
        else if (1 < dataTypes.size())
        {
            for (size_t i = 0; i < dataTypes.size() - 1; ++i)
            {
                try
                {
                    return getDatabase(dataTypes[i], dataAction, "--no-retry");
                }
                catch (...)
                {
                }
            }

            return getDatabase(dataTypes.back(), dataAction);
        }

        return nullptr;
    }

    void DatabaseFactory::cleanup()
    {
        FUNCTION_ENTRY("cleanup");
        TA_THREADGUARD(m_getDatabaseLock);

        // set up iterators to step through the database map.
        ThreadMap::iterator threadIter(m_databaseMap.begin());
        ThreadMap::iterator threadIterEnd(m_databaseMap.end());

        // step through member map
        while (threadIter != threadIterEnd)
        {
            ConnectionMap::iterator iter(threadIter->second.begin());
            ConnectionMap::iterator iterEnd(threadIter->second.end());

            while (iter != iterEnd)
            {
                if (iter->second != NULL)
                {
                    delete iter->second;
                    iter->second = NULL;
                }

                iter++;
            }

            threadIter++;
        }

        FUNCTION_EXIT;
    }

    void DatabaseFactory::delDatabase(EDataTypes dataType, EDataActions dataAction)
    {
        FUNCTION_ENTRY("delDatabase(EDataTypes,EDataActions)");

        IDatabase* pDatabase = NULL;

        {
            TA_THREADGUARD(m_getDatabaseLock);

            // get the connection string for this datatype/action
            std::string dbConnection;

            // If this call fails it will throw something.
            try
            {
                DbConnection::getInstance().getConnectionString(dataType, dataAction, dbConnection);
            }
            catch (DbConnectionFailed&)
            {
                // we exit the forever loop with this throw
                TA_THROW(DatabaseException("Unable to find a working database"));
            }

            // Get the ID of this thread - we want one database connection per thread
            int threadId = Thread::getCurrentThreadId();

            // Look to see if there is already a database connection for this thread
            ThreadMap::iterator threadIter = m_databaseMap.find(threadId);

            if (threadIter == m_databaseMap.end())
            {
                LOG_ERROR("No SimpleDbDatabase object was found for thread %lu", threadId);
                TA_THROW(DatabaseException("Unable to find the IDatabase"));
            }

            ConnectionMap::iterator connIter = threadIter->second.find(dbConnection);

            if (connIter == threadIter->second.end())
            {
                LOG_ERROR("No SimpleDbDatabase object was found for DBconnection string %s", dbConnection);
                TA_THROW(DatabaseException("Unable to find the IDatabase"));
            }

            pDatabase = connIter->second;
            threadIter->second.erase(connIter);

            if (threadIter->second.empty())
            {
                m_databaseMap.erase(threadIter);
            }

            if (NULL == pDatabase)
            {
                LOG_ERROR("Unexpected  to be deleted NULL pointer of IDatabase");
            }
        }

        if (NULL != pDatabase)
        {
            delete pDatabase;
            pDatabase = 0;
        }

        FUNCTION_EXIT;
    }

    void DatabaseFactory::delDatabase(IDatabase* pDatabase)
    {
        FUNCTION_ENTRY("delDatabase(IDatabase*)");

        if (NULL == pDatabase)
        {
            LOG_WARN("Invalid input parameter that IDatabase is NULL pointer");
            return;
        }

        bool bExist = false;

        {
            TA_THREADGUARD(m_getDatabaseLock);

            // set up iterators to step through the database map.
            ThreadMap::iterator threadIter(m_databaseMap.begin());
            ThreadMap::iterator threadIterEnd(m_databaseMap.end());

            // step through member map
            while (threadIter != threadIterEnd)
            {
                ConnectionMap::iterator iter(threadIter->second.begin());
                ConnectionMap::iterator iterEnd(threadIter->second.end());

                while (iter != iterEnd)
                {
                    if (iter->second == pDatabase)
                    {
                        threadIter->second.erase(iter);

                        if (threadIter->second.empty())
                        {
                            m_databaseMap.erase(threadIter);
                        }

                        bExist = true;
                        break;
                    }

                    iter++;
                }

                if (bExist)
                {
                    break;
                }

                threadIter++;
            }
        }

        if (bExist)
        {
            delete pDatabase;
            pDatabase = NULL;
        }
        else
        {
            TA_THROW(DatabaseException("Unable to find the IDatabase"));
        }

        FUNCTION_EXIT;
    }

    std::string DatabaseFactory::getDatabaseName(const std::string& strFileName, const EDataTypes dataType, const EDataActions action)
    {
        FUNCTION_ENTRY("getDatabaseName()");
        std::string strDbName;
        DbNameStrMap::const_iterator cIter;

        TA_THREADGUARD(m_getDbNameLock);
        cIter = m_DbStrMap.find(strFileName);

        if (cIter != m_DbStrMap.end())
        {
            // already in the map
            DbConnectionStrings* pDbConStr = cIter->second;
            DataConnections connections = pDbConStr->getConnectionList(dataType, action);

            if (connections.size() > 0)
            { strDbName = connections[0].first; }
            else
            {
                LOG_WARN("Cannot find the DB name in file: %s", strFileName);
            }
        }
        else // the db connection file changed.
        {
            LOG_INFO("Database file changed, fileName: %s", strFileName);
            DbConnectionStrings* pDbConnStr = new DbConnectionStrings(strFileName);
            DataConnections connections = pDbConnStr->getConnectionList(dataType, action);

            if (connections.size() > 0)
            { strDbName = connections[0].first; }
            else
            {
                LOG_WARN("Cannot find the DB name in file: %s", strFileName);
            }

            m_DbStrMap.insert(DbNameStrMap::value_type(strFileName, pDbConnStr));
        }

        FUNCTION_EXIT;
        return strDbName;
    }
} // Closes TA_Base_Core
