/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/DbConnection.cpp $
 * @author:  R.Stagg
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 */

#ifdef __WIN32__
    #pragma warning(disable:4786)  // identifier was truncated to 'number' characters in the debug information
    #pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include "core/data_access_interface/src/DbConnection.h"
#include "core/data_access_interface/src/DbConnectionStrings.h"
#include "core/data_access_interface/src/DataTypesLookup.h"
#include "core/exceptions/src/DbConnectionFailed.h"
#include "core/exceptions/src/DbConnectionNoFile.h"
#include "core/exceptions/src/ThrowException.h"
#include "core/utilities/src/RunParamsEx.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/StdUtil.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/DebugUtilMacrosHelper.h"
#include "core/threads/src/Thread.h"
#include <boost/algorithm/cxx11/one_of.hpp>
#include <boost/format.hpp>
#include <boost/scope_exit.hpp>
#include <iostream>


#include "core/database/src/CommonType.h"
#include "core/database/src/DatabaseUtil.h"

namespace TA_Base_Core
{
    DbConnection::DbConnection()
    {
        // register to be notified when database when DB Connection File changes
        m_dbConnectionFileName = RPARAM_DBCONNECTIONFILE_value;
        RunParamsEx::registerRunParamUser(this, RPARAM_DBCONNECTIONFILE);

    }

    void DbConnection::getConnectionString(EDataTypes dataType, EDataActions action, std::string& connectionString, const std::string& options)
    {
        LOG_CALLSTACK("DbConnection::getConnectionString");
        LOG_DEBUG("getConnectionString(): begin dataType=%s, action=%s", getDataTypesStr(dataType), getDataActionsStr(action));
        BOOST_SCOPE_EXIT_ALL(&) { LOG_DEBUG("getConnectionString(): end dataType=%s, action=%s, connectionString=%s", getDataTypesStr(dataType), getDataActionsStr(action), connectionString); };


        // --- this code added by Limin for personal testing ...
        {
            // try to get connection-string from run param, can use wildcard * ?

            static auto s_params = RunParamsEx::getAll("DbConnectionStr[*]");

            if (s_params.size())
            {
                auto dataTypeStr = getDataTypesStr(dataType);
                auto dataActionStr = getDataActionsStr(action);
                auto target = dataTypeStr + ":" + dataActionStr;

                auto runParam = str(boost::format("DbConnectionStr[%s]") % target);
                auto runParamX = "DbConnectionStr[*]";

                if (auto value = RunParamsEx::getOptional(runParam))
                {
                    connectionString = *value;
                    return;
                }

                auto params = s_params;
                boost::remove_erase_if(params, [&](auto & nv) { return stdutil::icontains_any(nv.name, runParam, runParamX); });

                static const thread_local boost::regex s_DB_CONNECTION_STR_REGEX("DbConnectionStr\\[(.+?)\\]");
                boost::smatch m;

                for (auto& param : params)
                {
                    if (boost::regex_match(param.name, m, s_DB_CONNECTION_STR_REGEX))
                    {
                        auto pattern = m.str(1);

                        boost::replace_all(pattern, "?", ".");
                        boost::replace_all(pattern, "*", ".*?");

                        if (boost::regex_match(target, boost::regex(str(boost::format("(?ix)^ %s $") % pattern))))
                        {
                            connectionString = param.value;
                            return;
                        }
                    }
                }

                if (RunParams::getInstance().isSet(runParamX))
                {
                    connectionString = RunParams::getInstance().get(runParamX);
                    return;
                }
            }
        }
        // --- end of code added by Limin for personal testing.

        TA_THREADGUARD(m_lock);

        DataKey key = std::make_pair(dataType, action);

        if (!getDbMap().count(key))
        {
            // should create a lookup function to change the enums back into strings.
            throw DbConnectionFailed(str(boost::format("No connection string defined for %d/%s") % dataType % action));
        }

        LOG_DATABASE("Trying to find working database for data type %s, action %s ...", getDataTypesStr(dataType), getDataActionsStr(action));

        static size_t s_default_max_try = RunParamsEx::getOr("DbConnectionDefaultMaxTry", 3);
        static size_t s_default_timeout = RunParamsEx::getOr("DbConnectionDefaultTimeout", 5);

        size_t MAX_RETRY = s_default_max_try;
        size_t TIMED_WAIT = s_default_timeout;
        bool noRetry = boost::icontains(options, "--no-retry");

        auto opt = stdutil::parsed_options(options);

        if (opt.count("max-retry"))
        {
            MAX_RETRY = stdutil::lexical_cast<size_t>(opt["max-retry"], MAX_RETRY);
        }

        if (opt.count("timed-wait-seconds"))
        {
            TIMED_WAIT = stdutil::lexical_cast<size_t>(opt["timed-wait-seconds"], TIMED_WAIT);
        }

        for (size_t i = 0; i < MAX_RETRY; ++i)
        {
            LOG_DATABASE("Start Iteration %d of %d (every %d seconds)", i + 1, MAX_RETRY, TIMED_WAIT);

            for (auto& connection : getDbMap()[key])
            {
                if (DbStatus::getInstance().isAvailable(connection.first))
                {
                    connectionString = connection.second;
                    LOG_DATABASE("Found connection string: %s", connectionString);
                    return;
                }
            }

            if (noRetry)
            {
                break;
            }

            m_condition.timedWait(TIMED_WAIT * 1000);
            LOG_DATABASE("End Iteration %d of %d (every %d seconds)", i + 1, MAX_RETRY, TIMED_WAIT);
        }

        // If we make it here, we failed to find any working db, so
        TA_THROW_INFO(DbConnectionFailed("No working database found"));
    }

    std::string DbConnection::getConnectionString(EDataTypes dataType, EDataActions action)
    {
        std::string connectionString;
        getConnectionString(dataType, action, connectionString);
        return connectionString;
    }

    void DbConnection::onRunParamChange(const std::string& name, const std::string& value)
    {
        // when database status change to good.
        if (RPARAM_DBONLINE == value && stdutil::any_of_iequal(m_dbNameRunParams, name))
        {
            m_condition.signal();
        }

        // if db connection file name changed. Clear the map and set the new connection file name.
        //  This will trigger getDbMap() to re-create the m_dbMap with the new db_connection file
        if ( (RPARAM_DBCONNECTIONFILE == name) && m_dbConnectionFileName != value)
        {
            TA_THREADGUARD(m_lock);
            m_dbMap.clear();

            // [aqiu] currently RunParam does not have capability to unregister user only for specific parameter (now only unregister all)
            //RunParamsEx::deregisterRunParamUser(this, m_dbNameRunParams);
            //m_dbNameRunParams.clear();

            m_dbConnectionFileName = RPARAM_DBCONNECTIONFILE_value;

        }
    }

    DbConnection::DatabaseMap& DbConnection::getDbMap()
    {
        TA_THREADGUARD(m_lock);

        if (m_dbMap.empty())
        {
            try
            {
                DbConnectionStrings dbConnectStrings(RPARAM_DBCONNECTIONFILE_value);
                m_dbMap = dbConnectStrings.getConnectionMap();
                stdutil::transform(dbConnectStrings.getDatabaseNameSet(), m_dbNameRunParams, [](const std::string & name) { return RPARAM_DBPREFIX + name; });
                RunParamsEx::registerRunParamUser(this, m_dbNameRunParams);
            }
            catch (DbConnectionNoFile& e)
            {
                TA_THROW_INFO(DbConnectionFailed(e.what()));
            }
            catch (std::exception& e)
            {
                TA_RETHROW_INFO(e);
            }
            catch (...)
            {
                TA_RETHROW_INFO();
            }
        }

        return m_dbMap;
    }

    std::string DbConnection::getDatabaseName(/*const std::string& strFileName,*/ const EDataTypes dataType, const EDataActions action)
    {
        FUNCTION_ENTRY("getDatabaseName()");
        std::string strDbName;
        std::string connection_string;

        getConnectionString(dataType, action, connection_string);

        DatabaseUtil::getDatabaseName(connection_string, strDbName);

        FUNCTION_EXIT;
        return strDbName;
    }


}
