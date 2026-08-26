/**
 * The source code in this file is the property of
 * ST Engineering Electronics and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/GlobalStatusAccessFactory.cpp $
 * @author:  Anton QIU
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * GlobalStatusAccessFactory is a singleton that is used to retrieve
 * Global Status objects.
 */

#ifdef __WIN32__
    #pragma warning (disable:4786)
#endif

#include "core/types/src/ta_types.h"
#include "core/data_access_interface/src/GlobalStatusAccessFactory.h"


#include "core/database/src/SimpleDatabaseV2.h"
#include "core/database/src/TA_Database.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/global_status/IDL/src/GlobalStatusCorbaDef.h"
#include "core/utilities/src/TA_Time.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/exceptions/src/DataException.h"
#include "core/data_access_interface/entity_access/src/GlobalStatusAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"

namespace TA_Base_Core
{
    GlobalStatusAccessFactory::~GlobalStatusAccessFactory()
    {
    }

    std::vector<GlobalStatusData*> GlobalStatusAccessFactory::getAllStatus()
    {
        FUNCTION_ENTRY("GlobalStatusAccessFactory::getAllStatus()");

        //GLOBAL_STATUS_SELECT_109051 = select NAME, VALUE, strftime('%Y%m%d%H%M%S', UPDATED_TIME), UPDATED_BY FROM global_status order by PKEY

        // Set up the columnNames vector to be passed to executeQuery()
        std::string nameColumn("NAME");
        std::string valueColumn("VALUE");
        std::string updateTimeColumn("UPDATED_TIME");
        std::string updateByColumn("UPDATED_BY");

        std::vector<std::string> columnNames;
        columnNames.push_back(nameColumn);
        columnNames.push_back(valueColumn);
        columnNames.push_back(updateTimeColumn);
        columnNames.push_back(updateByColumn);

        // get a connection to the database
        //IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(GlobalStatus_Ad, Read);
        TA_DATABASE_PTR databaseConnection (new SimpleDatabaseV2(GlobalStatus_Ad, Read));

        SQLStatement strSql;
        databaseConnection->prepareSQLStatement(strSql, GLOBAL_STATUS_SELECT_109051);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        //IData* data = databaseConnection->executeQuery(strSql, columnNames);
        TA_DB_DATA data = databaseConnection->executeQuery(strSql, columnNames);

        std::vector<GlobalStatusData*> statusList;

        // Loop for each row returned
        do
        {
            for (ta_uint32 i = 0; i < data->getNumRows(); ++i)
            {
                try
                {
                    TA_Base_Core::GlobalStatusData* st = new TA_Base_Core::GlobalStatusData;
                    std::string id = data->getStringData(i, nameColumn);
                    std::string value = data->getStringData(i, valueColumn);
                    std::string timestring = data->getStringData(i, updateTimeColumn);
                    std::string update_by = data->getStringData(i, updateByColumn);

                    LOG_DEBUG("Reading Global Status from table: id: %s, value=%s, updateTime=%s, updateBy=%s", id, value, timestring, update_by);

                    st->id = id;
                    st->value = value;
                    timeb tb;

                    if (gLocalDateTimeStringToTime(timestring, tb))
                    {
                        st->updatedTime = ((ta_uint64)tb.time * 1000) + tb.millitm; // convert to miliseconds...
                        //st->updatedTime = tb.time;    // in seconds...
                    }
                    else
                    {
                        st->updatedTime = 0;
                        LOG_ERROR("Fail to convert the Timestamp from Global Status Table: id: %s", id);
                    }

                    st->ownerID = update_by;

                    statusList.push_back(st);
                }
                catch (const TA_Base_Core::DataException&)
                {
                    // Clean up the data pointer
                    //delete data;
                    //data = NULL;
                    TA_DB_CLEAR_DATA(data);
                    throw;
                }
            }

            //delete data;
            //data = NULL;
            TA_DB_CLEAR_DATA(data);

        } while (databaseConnection->moreData(data));

        FUNCTION_EXIT;
        return statusList;
    }

    GlobalStatusData* GlobalStatusAccessFactory::getStatusByName(std::string statusName)
    {
        FUNCTION_ENTRY("GlobalStatusAccessFactory::getStatusByName(statusName)");

        //GLOBAL_STATUS_SELECT_109052 = select NAME, VALUE, strftime('%Y%m%d%H%M%f', UPDATED_TIME), UPDATED_BY FROM global_status WHERE NAME = '%s'

        // Set up the columnNames vector to be passed to executeQuery()
        std::string nameColumn("NAME");
        std::string valueColumn("VALUE");
        std::string updateTimeColumn("UPDATED_TIME");
        std::string updateByColumn("UPDATED_BY");

        std::vector<std::string> columnNames;
        columnNames.push_back(nameColumn);
        columnNames.push_back(valueColumn);
        columnNames.push_back(updateTimeColumn);
        columnNames.push_back(updateByColumn);

        // get a connection to the database
        //IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(GlobalStatus_Ad, Read);
        TA_DATABASE_PTR databaseConnection (new SimpleDatabaseV2(GlobalStatus_Ad, Read));

        SQLStatement strSql;
        databaseConnection->prepareSQLStatement(strSql, GLOBAL_STATUS_SELECT_109052, databaseConnection->escapeQueryString(statusName));

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        //IData* data = databaseConnection->executeQuery(strSql, columnNames);
        TA_DB_DATA data = databaseConnection->executeQuery(strSql, columnNames);

        std::vector<TA_Base_Core::GlobalStatusData*> statusList;

        if (data->getNumRows() == 0)
        {
            std::string message = "No data found or non unique value for " + statusName;
            std::string strExSql = databaseConnection->getSqlString(strSql);
            TA_THROW(DataException(message.c_str(), DataException::NO_VALUE, strExSql));
        }

        // Loop for each row returned
        do
        {
            for (ta_uint32 i = 0; i < data->getNumRows(); ++i)
            {
                try
                {
                    GlobalStatusData* st = new GlobalStatusData;
                    std::string id = data->getStringData(i, nameColumn);
                    std::string value = data->getStringData(i, valueColumn);
                    std::string timestring = data->getStringData(i, updateTimeColumn);
                    std::string update_by = data->getStringData(i, updateByColumn);

                    LOG_DEBUG("Reading Global Status from table: id: %s, value=%s, updateTime=%s, updateBy=%s", id, value, timestring, update_by);

                    st->id = id;
                    st->value = value;
                    timeb tb;

                    if (gLocalDateTimeStringToTime(timestring, tb))
                    {
                        st->updatedTime = (((ta_uint64)tb.time) * 1000) + tb.millitm;   // convert to miliseconds...
                        //st->updatedTime = tb.time;    // in seconds...
                    }
                    else
                    {
                        st->updatedTime = 0;
                        LOG_ERROR("Fail to convert the Timestamp from Global Status Table: id: %s", id);
                    }

                    st->ownerID = update_by;

                    statusList.push_back(st);
                }
                catch (const TA_Base_Core::DataException&)
                {
                    // Clean up the data pointer
                    //delete data;
                    //data = NULL;
                    TA_DB_CLEAR_DATA(data);
                    throw;
                }
            }

            //delete data;
            //data = NULL;
            TA_DB_CLEAR_DATA(data);
        } while (databaseConnection->moreData(data));

        FUNCTION_EXIT;
        return statusList[0];
    }

    void GlobalStatusAccessFactory::insertStatus(const GlobalStatusData& status)
    {
        FUNCTION_ENTRY("GlobalStatusAccessFactory::setStatusByName(status)");

        //IDatabase* databaseConnection =
        //    DatabaseFactory::getInstance().getDatabase(GlobalStatus_Ad, Write);
        TA_DATABASE_PTR databaseConnection (new SimpleDatabaseV2(GlobalStatus_Ad, Write));

        ta_uint64 statusUpdatedTime = status.updatedTime;
        timeb tb;
        tb.time = statusUpdatedTime / 1000;
        tb.millitm = statusUpdatedTime % 1000;

        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, GLOBAL_STATUS_INSERT_109351,
                                                status.id, status.value, TA_Base_Core::gTimeToLocalDateTimeString(tb), status.ownerID);
        //      TA_ASSERT(sql.size() > 0, "createNewAlarmRule: Get SQL Statement error");

        databaseConnection->executeModification(sql);

        FUNCTION_EXIT;
        return;
    }

    void GlobalStatusAccessFactory::updateStatus(const GlobalStatusData& status)
    {
        FUNCTION_ENTRY("GlobalStatusAccessFactory::updateStatus(status)");

        //IDatabase* databaseConnection =
        //    DatabaseFactory::getInstance().getDatabase(GlobalStatus_Ad, Write);
        TA_DATABASE_PTR databaseConnection (new SimpleDatabaseV2(GlobalStatus_Ad, Write));

        ta_uint64 statusUpdatedTime = status.updatedTime;
        timeb tb;
        tb.time = statusUpdatedTime / 1000;
        tb.millitm = statusUpdatedTime % 1000;

        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, GLOBAL_STATUS_UPDATE_109251,
                                                status.value, TA_Base_Core::gTimeToLocalDateTimeString(tb), status.ownerID, status.id);
        //      TA_ASSERT(sql.size() > 0, "createNewAlarmRule: Get SQL Statement error");

        databaseConnection->executeModification(sql);

        FUNCTION_EXIT;
        return;
    }

    TA_Base_Core::CorbaName GlobalStatusAccessFactory::getGSACorbaNameAtLocation(ta_uint32 locationkey)
    {
        TA_Base_Core::CorbaNameList agents = TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNamesOfTypeAtLocation(
                                                 TA_Base_Core::GlobalStatusAgentEntityData::getStaticType(), locationkey, false);

        if (1 != agents.size())
        {
            //LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Entity for Global Status Agent not configured properly!");
            TA_THROW(TA_Base_Core::TransactiveException("Entity for Global Status Agent not configured properly!"));
        }

        return agents[0];
    }

#if 0
    TA_Base_Core::GlobalStatusNamedObjectPtr GlobalStatusAccessFactory::getGSAAtLocation(ta_uint32 locationkey)
    {
        TA_Base_Core::GlobalStatusNamedObjectPtr p_globalStatusAgent = nullptr;

        try
        {
            //TA_Base_Core::CorbaNameList agents = TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNamesOfTypeAtLocation(
            //    TA_Base_Core::GlobalStatusAgentEntityData::getStaticType(), locationkey, false);

            //if(1 != agents.size())
            //{
            //    //LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Entity for Global Status Agent not configured properly!");
            //    TA_THROW(TA_Base_Core::TransactiveException("Entity for Global Status Agent not configured properly!"));
            //}
            //p_globalStatusAgent = TA_Base_Core::GlobalStatusNamedObjectPtr(new TA_Base_Core::GlobalStatusNamedObject(agents[0]));
            TA_Base_Core::CorbaName agent = getGSACorbaNameAtLocation(locationkey);
            p_globalStatusAgent = TA_Base_Core::GlobalStatusNamedObjectPtr(new TA_Base_Core::GlobalStatusNamedObject(agent));
        }
        catch (...)
        {
            TA_THROW(TA_Base_Core::TransactiveException("Failed to resolve Global Status Agent!"));
        }

        return p_globalStatusAgent;
    }
#endif
}
