/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/ActionAccessFactory.cpp $
 * @author:  Karen Graham
 * @version: $Revision: #4 $
 *
 * Last modification: $DateTime: 2024/09/27 18:12:18 $
 * Last modified by:  $Author: josef $
 *
 * ActionAccessFactory is a singleton that is used to retrieve action objects either from the
 * database or newly created. All action objects returned will adhear to the IAction interface.
 * Data is primarily retrieved from the SE_ACTION table
 *
 */

#ifdef __WIN32__
    #pragma warning(disable:4786)
#endif

#include "core/types/src/ta_types.h"
#include "core/data_access_interface/src/ActionAccessFactory.h"
#include "core/data_access_interface/src/Action.h"

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/data_access_interface/src/SQLCode.h"

namespace TA_Base_Core
{
    ActionAccessFactory::ActionAccessFactory()
    {
        initializeAction();
    }

    ActionAccessFactory* ActionAccessFactory::m_instance = 0;

    ActionAccessFactory& ActionAccessFactory::getInstance()
    {
        if (m_instance == 0)
        {
            m_instance = new ActionAccessFactory();
        }

        return *m_instance;
    }

    void ActionAccessFactory::initializeAction()
    {
        FUNCTION_ENTRY("ActionAccessFactory::initializeAction()");

        // get a connection to the database
        IDatabase* database = DatabaseFactory::getInstance().getDatabase({OfflineSecurity_Cd, OfflineSecurity_Cached}, Read);

        /*std::ostringstream sql;
        sql << "select m.seacti_id, g.name from se_action_group g, se_action_group_map m where m.seagro_id = g.seagro_id";*/

        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), se_action_group_STD_SELECT_11501);
        //      std::string sql  = databaseConnection->prepareSQLStatement(SE_ACTION_GROUP_SELECT_11501);
        auto sql = database->preparedSQLStatement(SE_ACTION_GROUP_SELECT_11501);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        auto data = database->executeQueryAll(sql);

        for (auto row : *data)
        {
            auto actionId  = row.getUnsignedLongData("SEACTI_ID");
            auto actionGroup = row.getStringData("NAME");
            m_actionToActionGroupMap[actionId].emplace_back(std::move(actionGroup));
        }
    }

    void ActionAccessFactory::getActionGroupByAction(ta_uint32 actionId, std::vector<std::string>& actionGroups)
    {
        FUNCTION_ENTRY("ActionAccessFactory::getActionGroupByAction");

        ActionToActionGroupMap::iterator it = m_actionToActionGroupMap.find(actionId);

        if (it != m_actionToActionGroupMap.end())
        {
            actionGroups = it->second;
        }

        FUNCTION_EXIT;
    }

    void ActionAccessFactory::getActionBySql(IDatabase* pDatabase, const SQLStatement& sql, std::vector<IAction*>& actions)
    {
        FUNCTION_ENTRY("getActionBySql");

        // get a connection to the database
        //IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase({OfflineSecurity_Cd, OfflineSecurity_Cached}, Read);
        TA_ASSERT(pDatabase != NULL, "Database connection is null");

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back("PKEY");
        columnNames.push_back("NAME");
        columnNames.push_back("DESCRIPTION");
        columnNames.push_back("IS_DISPLAY_ONLY");

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        auto data = pDatabase->executeQueryAll(sql, columnNames);

        for (size_t i = 0; i < data->getNumRows(); i++)
        {
            auto actionId = data->getUnsignedLongData(i, columnNames[0]);
            std::vector<std::string> actionGroups;
            getActionGroupByAction(actionId, actionGroups);
            actions.push_back(new Action(i, *data, actionGroups));
        }

        FUNCTION_EXIT;
    }

    IAction* ActionAccessFactory::getAction(const ta_uint32 actionKey)
    {
        FUNCTION_ENTRY("getAction");

        // get a connection to the database
        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase({OfflineSecurity_Cd, OfflineSecurity_Cached}, Read);

        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_ACTION_STD_SELECT_12001, actionKey);
        //      std::string sql  = databaseConnection->prepareSQLStatement(SE_ACTION_SELECT_12001, actionKey);
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, SE_ACTION_SELECT_12001, actionKey);
        //      TA_ASSERT(sql.size() > 0, "ActionAccessFactory::getAction(): Prepare SQL Statement error");

        /*std::ostringstream sql;
        sql << "select SEACTI_ID, NAME, DESCRIPTION from SE_ACTION where SEACTI_ID = " << actionKey;*/

        std::vector<IAction*> actions;
        getActionBySql(databaseConnection, sql, actions);

        // Bring the DataException into the namespace
        using TA_Base_Core::DataException;

        if (0 == actions.size())
        {
            std::ostringstream message;
            message << "No data found for action with key " << actionKey;
            //  TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, sql ) );

            std::string strExSql = databaseConnection->getSqlString(sql);
            TA_THROW(DataException(message.str().c_str(), DataException::NO_VALUE, strExSql));
        }

        TA_ASSERT(1 == actions.size(), "Action key unique constraint violated");

        // Return the pointer. The class that recieves this pointer is responsible for deleting it.
        FUNCTION_EXIT;
        return actions[0];
    }

    std::vector<IAction*> ActionAccessFactory::getAllActions()
    {
        FUNCTION_ENTRY("getAllActions");

        // get a connection to the database
        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase({ OfflineSecurity_Cd, OfflineSecurity_Cached}, Read);

        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_ACTION_STD_SELECT_12002);
        //      std::string sql  = databaseConnection->prepareSQLStatement(SE_ACTION_SELECT_12002);
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, SE_ACTION_SELECT_12002);
        //      TA_ASSERT(sql.size() > 0, "ActionAccessFactory::getAllActions(): Prepare SQL Statement error");

        /*std::ostringstream sql;
        sql << " select SEACTI_ID, NAME, DESCRIPTION from SE_ACTION";*/

        std::vector<IAction*> actions;
        getActionBySql(databaseConnection, sql, actions);

        FUNCTION_EXIT;
        return actions;
    }

    //limin++ for centralize duty/rights
    void ActionAccessFactory::getActionGroupMap(const std::set<ta_uint32>& groupIdList, ActionGroupMap& actionGroupMap)
    {
        TA_ASSERT(0 < groupIdList.size() && groupIdList.size() < 256, "group id list is too small or large");
        actionGroupMap.clear();

        // get a connection to the database
        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase({ OfflineSecurity_Cd, OfflineSecurity_Cached}, Read);

        std::stringstream sql;
        //sql << "SELECT seagro_id, seacti_id FROM se_action_group_map WHERE seagro_id IN(";

        for (std::set<ta_uint32>::const_iterator iter = groupIdList.begin(); iter != groupIdList.end(); ++iter)
        {
            if (iter != groupIdList.begin())
            {
                sql << ", ";
            }

            sql << *iter;
        }

        //sql << ") ORDER BY seagro_id";

        //      std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), se_action_group_STD_SELECT_11502, sql.str());
        //      std::string strSql  = databaseConnection->prepareSQLStatement(SE_ACTION_GROUP_SELECT_11502, sql.str());
        SQLStatement strSql;
        databaseConnection->prepareSQLStatement(strSql, SE_ACTION_GROUP_SELECT_11502, sql.str());
        //      TA_ASSERT(strSql.size() > 0, "getActionGroupMap(): Prepare SQL Statement error");

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        auto data = databaseConnection->executeQueryAll(strSql);

        for (auto row : *data)
        {
            auto groupId =  row.getUnsignedLongData("SEAGRO_ID");
            auto actionId = row.getUnsignedLongData("SEACTI_ID");
            actionGroupMap[ groupId ].push_back(actionId);
        }
    }

    void ActionAccessFactory::getNonPhysicalSubsystemActionGroup(const std::vector<ta_uint32>& profileList, std::multimap<ta_uint32, ta_uint32>& groupIdList)
    {
        TA_ASSERT(1 == profileList.size() || 2 == profileList.size(), "profile id list is too small or large");

        groupIdList.clear();
        // get a connection to the database
        IDatabase* database = DatabaseFactory::getInstance().getDatabase({ OfflineSecurity_Cd, OfflineSecurity_Cached}, Read);

        std::stringstream sql;
        //sql << "select spa.subsystemkey, spa.seagro_id from se_profile_access spa, subsystem sub where sub.is_physical=0 and sub.PKEY = spa.SUBSYSTEMKEY and ";

        if (1 == profileList.size())
        {
            sql << "spa.SEPROF_ID = " << profileList[0];
        }

        else
        {
            sql << "(spa.SEPROF_ID = " << profileList[0] << " or spa.SEPROF_ID = " << profileList[1] << ")";
        }

        //      std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), se_profile_access_STD_SELECT_12501, sql.str());
        //      std::string strSql  = databaseConnection->prepareSQLStatement(SE_PROFILE_ACCESS_SELECT_12501, sql.str());
        SQLStatement strSql;
        database->prepareSQLStatement(strSql, SE_PROFILE_ACCESS_SELECT_12501, sql.str());
        //      TA_ASSERT(strSql.size() > 0, "getNonPhysicalSubsystemActionGroup(): Prepare SQL Statement error");

        //LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "SQL sentence: %s", strSql.c_str());

        std::string strExSql = database->getSqlString(strSql);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        auto data = database->executeQueryAll(strSql);

        for (auto row : *data)
        {
            auto subsystemId =  row.getUnsignedLongData("SUBSYSTEMKEY");
            auto groupId =  row.getUnsignedLongData("SEAGRO_ID");
            groupIdList.emplace(subsystemId, groupId);
        }
    }
}
