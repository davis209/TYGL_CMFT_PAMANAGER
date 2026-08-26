/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/SystemAccessFactory.cpp $
 * @author:  Bradley Cleaver
 * @version: $Revision: #4 $
 *
 * Last modification: $DateTime: 2024/09/30 12:50:25 $
 * Last modified by:  $Author: josef $
 *
 * CombinedEventAccessFactory is a singleton that is used by the Event Viewer to
 * retrieve all data required to be presented to the user. It allows filtering and
 * sorting to done on the database side, and returns a vector of partially loaded
 * data to improve the efficiency.
 *
 */

// Disable "identifier was trucated to '255' characters" warnings.
#ifdef __WIN32__
    #pragma warning(disable:4786)
#endif

#include "ColumnNames.h"
#include "core/types/src/ta_types.h"
#include <time.h>

#include "SystemAccessFactory.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IDatabaseCallbackUser.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/synchronisation/src/ThreadGuard.h"

#include <sstream>

// Define all constants in an anonymous namespace.
namespace
{
    //columns for System
};

namespace TA_Base_Core
{
    SystemAccessFactory* SystemAccessFactory::m_instance = NULL;
    NonReEntrantThreadLockable SystemAccessFactory::m_singletonLock;

    SystemAccessFactory& SystemAccessFactory::getInstance()
    {
        TA_THREADGUARD(m_singletonLock);

        // Create a new instance if it doesn't exist.
        if (0 == m_instance)
        {
            m_instance = new SystemAccessFactory();
        }

        return *m_instance;
    }

    std::vector<SystemData*> SystemAccessFactory::getAllSystems()
    {
        FUNCTION_ENTRY("getAllSystems");

        // Get a connection to the database
        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(/*Event_SCd*/Event_Ad, Read);

        //      std::ostringstream sql;
        //      sql << "SELECT Pkey,name, is_physical,exclusive_control ";
        //      sql << " FROM System ";

        // Set up required columns.
        std::vector<std::string> columnNames;
        columnNames.push_back(COLUMN_PKEY);
        columnNames.push_back(COLUMN_NAME);
        columnNames.push_back(COLUMN_IS_PHYSICAL);
        columnNames.push_back(COLUMN_EXCLUSIVE_CONTROL);
        columnNames.push_back(COLUMN_LOCEXCLUSIVE_CONTROL);

        //      std::string sql  = databaseConnection->prepareSQLStatement(SYSTEM_SELECT_72001);
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, SYSTEM_SELECT_72001);
        //      TA_ASSERT(sql.size() > 0, "getAlarmStateData: GetSQLStatement error");

        //LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "sql for loading system data: %s", sql.c_str());

        std::string strExSql = databaseConnection->getSqlString(sql);
        LOG_DEBUG("sql for loading system data: %s", strExSql);

        IData* data = databaseConnection->executeQuery(sql, columnNames);

        std::vector<SystemData*> systems;

        do
        {
            // Loop through and create all the objects
            for (unsigned int i = 0; i < data->getNumRows(); i++)
            {
                try
                {
                    systems.push_back(createSystemData(i, data));
                }
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
        return systems;
    }

    SystemData* SystemAccessFactory::createSystemData(ta_uint32 row, IData* data)
    {
        // Get the pkey for the event.
        ta_uint32 pkey = data->getUnsignedLongData(row, COLUMN_PKEY);
        std::string name = data->getStringData(row, COLUMN_NAME);

        int is_physical = data->getIntegerData(row, COLUMN_IS_PHYSICAL);
        int exclusive_control = data->getIntegerData(row, COLUMN_EXCLUSIVE_CONTROL);
        int loc_exclusive_control = data->getIntegerData(row, COLUMN_LOCEXCLUSIVE_CONTROL);

        return new SystemData(pkey, name, (is_physical == 1) ? true : false, (exclusive_control == 1) ? true : false, (loc_exclusive_control == 1) ? true : false);
    }
};
