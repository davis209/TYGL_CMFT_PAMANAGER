/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/RightsAccessFactory.cpp $
 * @author:  Karen Graham
 * @version: $Revision: #4 $
 *
 * Last modification: $DateTime: 2024/09/27 18:12:18 $
 * Last modified by:  $Author: josef $
 *
 * RightsAccessFactory is a singleton that is used to retrieve rights objects either from the
 * database or newly created. All rights objects returned will adhear to the IRights interface.
 *
 * These come from the SE_PROFILE_ACCESS database table.
 *
 */

#ifdef __WIN32__
    #pragma warning(disable:4786)
#endif

#include "core/types/src/ta_types.h"
#include "core/data_access_interface/src/RightsAccessFactory.h"
#include "core/data_access_interface/src/ConfigRights.h"

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    RightsAccessFactory* RightsAccessFactory::m_instance = 0;

    RightsAccessFactory& RightsAccessFactory::getInstance()
    {
        if (m_instance == 0)
        {
            m_instance = new RightsAccessFactory();
        }

        return *m_instance;
    }

    void RightsAccessFactory::getRightBySql(IDatabase* pDatabase, const SQLStatement& sql, std::vector<IRights*>& rights, const bool readWrite)
    {
        FUNCTION_ENTRY("getRightBySql");

        // get a connection to the database

        TA_ASSERT(NULL != pDatabase, "database connection is null");

        // Set up the columnNames vector to be passed to executeQuery()
        std::string profileKeyColumn("PROFILE KEY");
        std::string subsystemKeyColumn("SUBSYSTEM KEY");
        std::string subsystemStateKeyColumn("SUBSYSTEM STATE");
        std::string actionGroupColumn("ACTIONGROUP KEY");
        std::string isPhysicalColumn("ISPHYSICAL");
        std::vector<std::string> columnNames;
        columnNames.push_back(profileKeyColumn);
        columnNames.push_back(subsystemKeyColumn);
        columnNames.push_back(subsystemStateKeyColumn);
        columnNames.push_back(actionGroupColumn);
        columnNames.push_back(isPhysicalColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        auto data = pDatabase->executeQueryAll(sql, columnNames);

        for (size_t i = 0; i < data->getNumRows(); i++)
        {
            // The getUnsignedLongData() call can throw an exception. Need to catch
            // it to do pointer clean up.

            // Create a shiny new location that has all the information pre-loaded!
            IRights* thisRights = NULL;

            if (readWrite)
            {
                thisRights = new ConfigRights(
                    data->getUnsignedLongData(i, profileKeyColumn),
                    data->getUnsignedLongData(i, subsystemKeyColumn),
                    data->getUnsignedLongData(i, subsystemStateKeyColumn),
                    data->getUnsignedLongData(i, actionGroupColumn),
                    data->getBooleanData(i, isPhysicalColumn));
            }
            else
            {
                TA_ASSERT(false, "Not implemented yet");
            }

            // No need to load the data as it is already loaded
            rights.push_back(thisRights);
        }
    }

    std::vector<IRights*> RightsAccessFactory::getAllRights(const bool readWrite)
    {
        FUNCTION_ENTRY("getAllRights");

        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase({ OnlineSecurity_Cd, OnlineSecurity_Cached}, Read);
        //      std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), se_profile_access_STD_SELECT_12502);
        //      std::string strSql  = databaseConnection->prepareSQLStatement(SE_PROFILE_ACCESS_SELECT_12502);
        auto strSql = databaseConnection->preparedSQLStatement(SE_PROFILE_ACCESS_SELECT_12502);
        //      TA_ASSERT(strSql.size() > 0, "RightsAccessFactory::getAllRights(): Prepare SQL Statement error");

        // create the SQL string to retrieve all the locations of the alarm
        /*std::ostringstream sql;
        sql << "select SPA.SEPROF_ID, SPA.SUBSYSTEMKEY, SPA.SESSTA_ID, SPA.SEAGRO_ID, S.IS_PHYSICAL "
            << "from SE_PROFILE_ACCESS SPA, SUBSYSTEM S where S.PKEY = SPA.SUBSYSTEMKEY";*/

        std::vector<IRights*> rights;
        getRightBySql(databaseConnection, strSql, rights, readWrite);

        // Return the pointer. The class that recieves this pointer is responsible for deleting it.
        FUNCTION_EXIT;
        return rights;
    }

    std::vector<IRights*> RightsAccessFactory::getRights(ta_uint32 subsystemStateKey, const bool readWrite)
    {
        FUNCTION_ENTRY("getRights");

        // get a connection to the database
        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase({ OnlineSecurity_Cd, OnlineSecurity_Cached}, Read);
        //      std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), se_profile_access_STD_SELECT_12503, subsystemStateKey);
        //      std::string strSql  = databaseConnection->prepareSQLStatement(SE_PROFILE_ACCESS_SELECT_12503, subsystemStateKey);
        SQLStatement strSql;
        databaseConnection->prepareSQLStatement(strSql, SE_PROFILE_ACCESS_SELECT_12503, subsystemStateKey);
        //      TA_ASSERT(strSql.size() > 0, "RightsAccessFactory::getRights(): Prepare SQL Statement error");

        // create the SQL string to retrieve all the locations of the alarm
        /*std::ostringstream sql;
        sql << "select SPA.SEPROF_ID, SPA.SUBSYSTEMKEY, SPA.SESSTA_ID, SPA.SEAGRO_ID, S.IS_PHYSICAL "
            << "from SE_PROFILE_ACCESS SPA, SUBSYSTEM S where S.PKEY = SPA.SUBSYSTEMKEY and SPA.SESSTA_ID = "
            << subsystemStateKey;*/

        std::vector<IRights*> rights;
        getRightBySql(databaseConnection, strSql, rights, readWrite);

        // Return the pointer. The class that recieves this pointer is responsible for deleting it.
        FUNCTION_EXIT;
        return rights;
    }

    IConfigRights* RightsAccessFactory::createRights(
        const ta_int32 profileKey,
        const ta_int32 subsystemKey,
        const ta_int32 subsystemStateKey)
    {
        FUNCTION_ENTRY("createRights");

        // get a connection to the database
        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Read);
        //      std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SUBSYSTEM_STD_SELECT_18002, subsystemKey);
        //      std::string strSql  = databaseConnection->prepareSQLStatement(SUBSYSTEM_SELECT_18002, subsystemKey);
        SQLStatement strSql;
        databaseConnection->prepareSQLStatement(strSql, SUBSYSTEM_SELECT_18002, subsystemKey);
        //      TA_ASSERT(strSql.size() > 0, "RightsAccessFactory::createRights(): Prepare SQL Statement error");

        // create the SQL string to retrieve all the locations of the alarm
        /*std::ostringstream sql;
        sql << "select IS_PHYSICAL from SUBSYSTEM where PKEY = " << subsystemKey;*/

        // Set up the columnNames vector to be passed to executeQuery()
        std::string isPhysicalColumn("ISPHYSICAL");
        std::vector<std::string> columnNames;
        columnNames.push_back(isPhysicalColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(strSql, columnNames);

        // Bring the DataException into the namespace
        using TA_Base_Core::DataException;

        if (NULL == data || data->getNumRows() < 1)
        {
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
            reasonMessage << "Could not determine whether the subsystem " << subsystemKey << " is a physical subsystem or not";
            TA_THROW(DataException(reasonMessage.str().c_str(), DataException::NO_VALUE, ""));
        }
        else if (data->getNumRows() > 1)
        {
            delete data;
            data = NULL;
            std::ostringstream reasonMessage;
            reasonMessage << "Could not determine whether the subsystem " << subsystemKey << " is a physical subsystem or not";
            TA_THROW(DataException(reasonMessage.str().c_str(), DataException::NOT_UNIQUE, ""));
        }

        bool isPhysical = data->getBooleanData(0, isPhysicalColumn);

        delete data;
        data = NULL;

        return new ConfigRights(profileKey, subsystemKey, subsystemStateKey, isPhysical);
    }

    //TD15533++
    ta_uint32 RightsAccessFactory::getActionGroup(ta_uint32 profileKey,
                                                  ta_uint32 subsystemKey,
                                                  ta_uint32 subsystemStateKey)
    {
        FUNCTION_ENTRY("ActionGroup");
        ta_uint32 actionGroup = 0xFFFFFFFF;

        // get a connection to the database
        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase({ OnlineSecurity_Cd, OnlineSecurity_Cached}, Read);
        //      std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), se_profile_access_STD_SELECT_12504,
        //      std::string strSql  = databaseConnection->prepareSQLStatement(SE_PROFILE_ACCESS_SELECT_12504,
        SQLStatement strSql;
        databaseConnection->prepareSQLStatement(strSql, SE_PROFILE_ACCESS_SELECT_12504,
                                                profileKey, subsystemKey, subsystemStateKey);
        //      TA_ASSERT(strSql.size() > 0, "RightsAccessFactory::getActionGroup(): Prepare SQL Statement error");

        /*std::ostringstream sql;
        sql << "select SEAGRO_ID from SE_PROFILE_ACCESS where SEPROF_ID = " << profileKey
            << " and SUBSYSTEMKEY = " << subsystemKey
            << " and SESSTA_ID = " << subsystemStateKey;*/

        // Set up the vector to pass to exectueQuery
        std::vector<std::string> columnNames;
        std::string actionGroupColumn("SEAGRO_ID");
        columnNames.push_back(actionGroupColumn);

        // execute the query
        IData* data = databaseConnection->executeQuery(strSql, columnNames);

        if (data == NULL || 0 == data->getNumRows())
        {
            LOG_ERROR("Could not retrieve actionGroup from SE_PROFILE table");
            //TA_THROW( DataException( "ActionGroup not found",DataException::NO_VALUE, strSql.c_str() ) );

            std::string strExSql = databaseConnection->getSqlString(strSql);
            TA_THROW(DataException("ActionGroup not found", DataException::NO_VALUE, strExSql.c_str()));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the key
        {
            // clean up the pointer
            delete data;
            data = NULL;

            char reasonMessage[256] = {0};
            sprintf(reasonMessage, "More than one actionGroup found");
            TA_THROW(DataException(reasonMessage, DataException::NOT_UNIQUE, ""));
        }

        actionGroup = data->getUnsignedLongData(0, actionGroupColumn);

        // clean up the pointer
        delete data;
        data = NULL;

        FUNCTION_EXIT;
        return actionGroup;
    }
    //++TD15533

    // yanrong ++
    ta_uint32 RightsAccessFactory::getActionGroupEx(ta_uint32 profileKey,
                                                    ta_uint32 subsystemKey,
                                                    ta_uint32 subsystemStateKey)
    {
        FUNCTION_ENTRY("ActionGroupEx");
        ta_uint32 actionGroup = 0xFFFFFFFF;

        //find the action group from the store map
        GroupQueryParam param;
        param.profileKey = profileKey;
        param.subsystemStatekey = subsystemStateKey;

        auto iter = m_queryStore.find(param);

        if (iter != m_queryStore.end())
        {
            auto& actionGrps = iter->second;
            actionGroup = actionGrps[subsystemKey];
        }
        else
        {
            // get a connection to the database
            IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase({ OnlineSecurity_Cd, OnlineSecurity_Cached}, Read);
            //          std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), se_profile_access_STD_SELECT_12505,
            //          std::string strSql  = databaseConnection->prepareSQLStatement(SE_PROFILE_ACCESS_SELECT_12505,
            SQLStatement strSql;
            databaseConnection->prepareSQLStatement(strSql, SE_PROFILE_ACCESS_SELECT_12505,
                                                    profileKey, subsystemStateKey);
            //          TA_ASSERT(strSql.size() > 0, "RightsAccessFactory::getActionGroupEx(): Prepare SQL Statement error");

            /*std::ostringstream sql;
            sql << "select SEAGRO_ID, SUBSYSTEMKEY from SE_PROFILE_ACCESS where SEPROF_ID = " << profileKey
                << " and SESSTA_ID = " << subsystemStateKey;*/ //<< " and SUBSYSTEMKEY = " << subsystemKey

            // Set up the vector to pass to exectueQuery
            std::vector<std::string> columnNames;
            std::string actionGroupColumn("SEAGRO_ID");
            std::string subsystemKeyColumn("SUBSYSTEMKEY");
            columnNames.push_back(actionGroupColumn);
            columnNames.push_back(subsystemKeyColumn);

            // execute the query
            IData* data = databaseConnection->executeQuery(strSql, columnNames);

            do
            {
                if (data != NULL && data->getNumRows() > 0)
                {
                    std::map<size_t, size_t> actionGroupMap;
                    int rows = (int)data->getNumRows();

                    for (int i = 0; i < rows; ++i)
                    {
                        ta_uint32 ssKey = data->getUnsignedLongData(i, subsystemKeyColumn);
                        ta_uint32 actGroup = data->getUnsignedLongData(i, actionGroupColumn);
                        actionGroupMap[ssKey] = actGroup;

                        if (subsystemKey == ssKey)
                        {
                            actionGroup = actionGroupMap[subsystemKey];
                        }
                    }

                    m_queryStore[param] = actionGroupMap;
                }
            }
            while (databaseConnection->moreData(data));

            if (actionGroup == 0xFFFFFFFF)
            {
                delete data;
                data = NULL;
                LOG_ERROR("Could not retrieve actionGroup from SE_PROFILE table");
                //TA_THROW( DataException( "ActionGroup not found",DataException::NO_VALUE, strSql.c_str() ) );

                std::string strExSql = databaseConnection->getSqlString(strSql);
                TA_THROW(DataException(str(boost::format("ActionGroup not found for Profile[%d], Subsystem[%d], SubsystemState[%d]") % profileKey % subsystemKey % subsystemStateKey), DataException::NO_VALUE, strExSql.c_str()));
            }

            // clean up the pointer
            delete data;
            data = NULL;
        }

        FUNCTION_EXIT;
        return actionGroup;
    }
    // ++ yanrong
} // closes TA_Base_Core
