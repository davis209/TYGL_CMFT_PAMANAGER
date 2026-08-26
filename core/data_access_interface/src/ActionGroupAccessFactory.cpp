/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/ActionGroupAccessFactory.cpp $
 * @author:  Karen Graham
 * @version: $Revision: #4 $
 *
 * Last modification: $DateTime: 2024/09/27 18:12:18 $
 * Last modified by:  $Author: josef $
 *
 * ActionGroupAccessFactory is a singleton that is used to retrieve ActionGroup objects either from the
 * database or newly created. All ActionGroup objects returned will adhear to the IActionGroup interface.
 * Data is primarily retrieved from the SE_ACTION_GROUP table
 *
 */

#ifdef __WIN32__
    #pragma warning(disable:4786)
#endif

#include "core/types/src/ta_types.h"
#include "core/data_access_interface/src/ActionGroupAccessFactory.h"
#include "core/data_access_interface/src/ActionGroup.h"
#include "core/data_access_interface/src/ConfigActionGroup.h"

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    ActionGroupAccessFactory::ActionGroupAccessFactory(): m_initialized(false)
    {
        //initializeActionGroup();
    }

    ActionGroupAccessFactory* ActionGroupAccessFactory::m_instance = 0;

    ActionGroupAccessFactory& ActionGroupAccessFactory::getInstance()
    {
        if (m_instance == 0)
        {
            m_instance = new ActionGroupAccessFactory();
        }

        return *m_instance;
    }

    void ActionGroupAccessFactory::initializeActionGroup()
    {
        FUNCTION_ENTRY("ActionGroupAccessFactory::initializeActionGroup()");

        // get a connection to the database
        IDatabase* database = DatabaseFactory::getInstance().getDatabase({ OfflineSecurity_Cd, OfflineSecurity_Cached}, Read);

        /* std::ostringstream sql;
         sql << "select seagro_id, seacti_id from se_action_group_map";*/

        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), se_action_group_STD_SELECT_11503);
        //      std::string sql  = databaseConnection->prepareSQLStatement(SE_ACTION_GROUP_SELECT_11503);
        auto sql = database->preparedSQLStatement(SE_ACTION_GROUP_SELECT_11503);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        auto data = database->executeQueryAll(sql);

        //TD17830 marc_bugfix
        //Make sure map is clear before adding entries
        m_actionGroupToActionMap.clear();

        for (size_t i = 0; i < data->getNumRows(); i++)
        {
            auto actionGroupId = data->getUnsignedLongData(i, "SEAGRO_ID");
            auto actionId = data->getUnsignedLongData(i, "SEACTI_ID");
            m_actionGroupToActionMap[actionGroupId].push_back(actionId);
        }
    }

    void ActionGroupAccessFactory::getActionByActionGroup(ta_uint32 actionGroupId, std::vector<ta_uint32>& actions)
    {
        FUNCTION_ENTRY("ActionGroupAccessFactory::getActionByActionGroup");

        ActionGroupToActionMap::iterator it = m_actionGroupToActionMap.find(actionGroupId);

        if (it != m_actionGroupToActionMap.end())
        {
            actions = it->second;
        }

        FUNCTION_EXIT;
    }

    void ActionGroupAccessFactory::getActionGroupBySql(IDatabase* pDatabase, const SQLStatement& sql, std::vector<IActionGroup*>& actionGroups, const bool readWrite)
    {
        FUNCTION_ENTRY("getActionGroupBySql");

        // get a connection to the database

        TA_ASSERT(NULL != pDatabase, "Database connection is null");

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back("SEAGRO_ID");
        columnNames.push_back("NAME");
        columnNames.push_back("IS_CONTROLTYPE");
        columnNames.push_back("DATE_MODIFIED");
        columnNames.push_back("DATE_CREATED");
        columnNames.push_back("DISPLAY_NAME");

        //TD17830 marc_bugfix
        //Initialize Action Group to action map first
        if (!m_initialized)
        {
            initializeActionGroup();
            m_initialized = true;
        }

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        auto data = pDatabase->executeQueryAll(sql, columnNames);

        for (size_t i = 0; i < data->getNumRows(); ++i)
        {
            ta_uint32 actionGroupId = data->getUnsignedLongData(i, columnNames[0]);
            std::vector<ta_uint32> actions;
            getActionByActionGroup(actionGroupId, actions);

            if (readWrite)
            {
                actionGroups.push_back(new ConfigActionGroup(i, *data, actions));
            }
            else
            {
                actionGroups.push_back(new ActionGroup(i, *data, actions));
            }
        }

        FUNCTION_EXIT;
    }

    IActionGroup* ActionGroupAccessFactory::getActionGroup(const ta_uint32 actionGroupKey, const bool readWrite)
    {
        FUNCTION_ENTRY("getActionGroup");

        IDatabase* pDatabase = DatabaseFactory::getInstance().getDatabase({ OnlineSecurity_Cd, OnlineSecurity_Cached}, Write);
        //      std::string sql = defPrepareSQLStatement(pDatabase->GetDbServerType(), se_action_group_Oracle_SELECT_11551, actionGroupKey);
        //      std::string sql  = pDatabase->prepareSQLStatement(SE_ACTION_GROUP_SELECT_11551, actionGroupKey);
        SQLStatement sql;
        pDatabase->prepareSQLStatement(sql, SE_ACTION_GROUP_SELECT_11551, actionGroupKey);
        //      TA_ASSERT(sql.size() > 0, "ActionGroupAccessFactory::getActionGroup(): Prepare SQL Statement error");

        /*std::ostringstream sql;
        sql << " select SEAGRO_ID, NAME, IS_CONTROLTYPE,"
            << " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
            << " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') FROM SE_ACTION_GROUP WHERE SEAGRO_ID = " << actionGroupKey;*/

        // Return the pointer. The class that recieves this pointer is responsible for deleting it.
        std::vector<IActionGroup*> actionGroups;
        getActionGroupBySql(pDatabase, sql, actionGroups, readWrite);

        if (0 == actionGroups.size())
        {
            std::ostringstream message;
            message << "No data found for actionGroup with key " << actionGroupKey;
            //TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, sql ) );

            std::string strExSql = pDatabase->getSqlString(sql);

            TA_THROW(DataException(message.str().c_str(), DataException::NO_VALUE, strExSql));
        }

        TA_ASSERT(1 == actionGroups.size(), "ActionGroup key unique constraint violated");

        FUNCTION_EXIT;
        return actionGroups[0];
    }

    std::vector<IActionGroup*> ActionGroupAccessFactory::getAllActionGroups(const bool readWrite)
    {
        FUNCTION_ENTRY("getAllActionGroups");

        IDatabase* pDatabase = DatabaseFactory::getInstance().getDatabase({ OnlineSecurity_Cd, OnlineSecurity_Cached}, Write);
        //      std::string sql = defPrepareSQLStatement(pDatabase->GetDbServerType(), se_action_group_Oracle_SELECT_11552);
        //      std::string sql  = pDatabase->prepareSQLStatement(SE_ACTION_GROUP_SELECT_11552);
        SQLStatement sql;
        pDatabase->prepareSQLStatement(sql, SE_ACTION_GROUP_SELECT_11552);
        //      TA_ASSERT(sql.size() > 0, "ActionGroupAccessFactory::getAllActionGroups(): Prepare SQL Statement error");

        // create the SQL string to retrieve all the locations of the alarm
        /* std::ostringstream sql;
         sql << " select SEAGRO_ID, NAME, IS_CONTROLTYPE,"
             << " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
             << " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') FROM SE_ACTION_GROUP";*/

        std::vector<IActionGroup*> actionGroups;
        getActionGroupBySql(pDatabase, sql, actionGroups, readWrite);

        FUNCTION_EXIT;
        return actionGroups;
    }

    IConfigActionGroup* ActionGroupAccessFactory::createActionGroup()
    {
        return new ConfigActionGroup();
    }

    IConfigActionGroup* ActionGroupAccessFactory::copyActionGroup(const IConfigActionGroup* actionGroupToCopy)
    {
        FUNCTION_ENTRY("copyActionGroup");

        TA_ASSERT(actionGroupToCopy !=  NULL, "The ActionGroup to copy was NULL");

        const ConfigActionGroup* cast = dynamic_cast<const ConfigActionGroup*>(actionGroupToCopy);

        TA_ASSERT(cast != NULL, "ActionGroup passed could not be converted into a ConfigActionGroup");

        FUNCTION_EXIT;
        return new ConfigActionGroup(*cast);
    }

    void ActionGroupAccessFactory::getAllActionGroupKeys(std::vector<ta_uint32>& groupKeys)
    {
        FUNCTION_ENTRY("getAllActionGroupKeys");

        // get a connection to the database
        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase({ OnlineSecurity_Cd, OnlineSecurity_Cached}, Write);

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back("SEAGRO_ID");

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        /*std::ostringstream sql;
        sql << "select seagro_id from se_action_group";*/
        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), se_action_group_STD_SELECT_11504);
        //      std::string sql  = databaseConnection->prepareSQLStatement(SE_ACTION_GROUP_SELECT_11504);
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, SE_ACTION_GROUP_SELECT_11504);
        //      TA_ASSERT(sql.size() > 0, "ActionGroupAccessFactory::getAllActionGroupKeys(): Prepare SQL Statement error");
        IData* data = databaseConnection->executeQuery(sql, columnNames);

        do
        {
            for (ta_uint32 i = 0; i < data->getNumRows(); i++)
            {
                try
                {
                    ta_uint32 actionGroupId = data->getUnsignedLongData(i, columnNames[0]);
                    groupKeys.push_back(actionGroupId);
                }
                //data exception
                catch (TA_Base_Core::DataException&)
                {
                    // Clean up the data pointer
                    delete data;
                    data = NULL;
                    throw;
                }
            }

            delete data;
            data = NULL;
        }
        while (databaseConnection->moreData(data));

        FUNCTION_EXIT;
    }

    std::string ActionGroupAccessFactory::getActionGroupName(const ta_uint32 key)
    {
        FUNCTION_ENTRY("getAllActionGroupKeys");

        // get a connection to the database
        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase({ OnlineSecurity_Cd, OnlineSecurity_Cached}, Write);

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back("NAME");

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        /*std::ostringstream sql;
        sql << "select name from se_action_group where seagro_id = " << key;*/
        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), se_action_group_STD_SELECT_11505, key);
        //      std::string sql  = databaseConnection->prepareSQLStatement(SE_ACTION_GROUP_SELECT_11505, key);
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, SE_ACTION_GROUP_SELECT_11505, key);
        //      TA_ASSERT(sql.size() > 0, "ActionGroupAccessFactory::getActionGroupName(): Prepare SQL Statement error");
        IData* data = databaseConnection->executeQuery(sql, columnNames);
        std::vector<ta_uint32> rt;

        std::string groupName;
        TA_ASSERT(1 == data->getNumRows(), "More than one name for one action group founded.");

        try
        {
            groupName = data->getStringData(0, columnNames[0]);
        }
        //data exception
        catch (TA_Base_Core::DataException&)
        {
            // Clean up the data pointer
            delete data;
            data = NULL;
            throw;
        }

        delete data;
        data = NULL;

        FUNCTION_EXIT;
        return groupName;
    }

    void ActionGroupAccessFactory::getAllActionGroupNames(std::map<ta_uint32, std::string>& groupNames)
    {
        FUNCTION_ENTRY("getAllActionGroupKeys");

        // get a connection to the database
        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase({ OnlineSecurity_Cd, OnlineSecurity_Cached}, Write);

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back("SEAGRO_ID");
        columnNames.push_back("NAME");

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        /*std::ostringstream sql;
        sql << "select seagro_id, name from se_action_group";*/
        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), se_action_group_STD_SELECT_11506);
        //      std::string sql  = databaseConnection->prepareSQLStatement(SE_ACTION_GROUP_SELECT_11506);
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, SE_ACTION_GROUP_SELECT_11506);
        //      TA_ASSERT(sql.size() > 0, "ActionGroupAccessFactory::getAllActionGroupNames(): Prepare SQL Statement error");
        IData* data = databaseConnection->executeQuery(sql, columnNames);

        do
        {
            for (ta_uint32 i = 0; i < data->getNumRows(); i++)
            {
                try
                {
                    ta_uint32 actionGroupId = data->getUnsignedLongData(i, columnNames[0]);
                    std::string actionGroupName = data->getStringData(i, columnNames[1]);
                    groupNames.insert(std::make_pair(actionGroupId, actionGroupName));
                }
                //data exception
                catch (TA_Base_Core::DataException&)
                {
                    // Clean up the data pointer
                    delete data;
                    data = NULL;
                    throw;
                }
            }

            delete data;
            data = NULL;
        }

        while (databaseConnection->moreData(data));

        FUNCTION_EXIT;
    }

} // closes TA_Base_Core
