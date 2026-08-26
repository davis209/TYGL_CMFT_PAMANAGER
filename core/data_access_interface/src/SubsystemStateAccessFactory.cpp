/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/SubsystemStateAccessFactory.cpp $
 * @author:  Anita Lee
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * SubsystemStateAccessFactory is a singleton that is used to retrieve subsystem state objects either from the
 * database or newly created. All subsystem state objects returned will adhear to the ISubsystemState interface.
 * Data is primarily retrieved from the SE_SUBSYSTEM_STATE table.
 *
 */

#ifdef __WIN32__
    #pragma warning(disable:4786)
#endif

#include "core/types/src/ta_types.h"
#include "core/data_access_interface/src/SubsystemStateAccessFactory.h"
#include "core/data_access_interface/src/SubsystemState.h"

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    SubsystemStateAccessFactory::SubsystemStateAccessFactory()
    {
    }

    SubsystemStateAccessFactory& SubsystemStateAccessFactory::getInstance()
    {
        static SubsystemStateAccessFactory factory;
        return factory;
    }

    ISubsystemState* SubsystemStateAccessFactory::getSubsystemState(const ta_uint32 SubsystemStateKey)
    {
        FUNCTION_ENTRY("getSubsystemState");

        // Create the Operator object to represent this object. Initially only populate it
        // with key and name. We will populate the rest of the data on demand.
        ISubsystemState* theSubsystemState = new SubsystemState(SubsystemStateKey);

        // Return the pointer. The class that recieves this pointer is responsible for deleting it.
        FUNCTION_EXIT;
        return theSubsystemState;
    }

    std::vector<ISubsystemState*> SubsystemStateAccessFactory::getAllSubsystemStates()
    {
        FUNCTION_ENTRY("getAllSubsystemStates");

        // get a connection to the database
        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

        // create the SQL string to retrieve all the locations of the alarm
        /*std::ostringstream sql;
        sql << "select SESSTA_ID, NAME, DESCRIPTION from SE_SUBSYSTEM_STATE";*/
        //      std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SUBSYSTEM_STD_SELECT_18005);
        //      std::string strSql  = databaseConnection->prepareSQLStatement(SUBSYSTEM_SELECT_18005);
        SQLStatement strSql;
        databaseConnection->prepareSQLStatement(strSql, SUBSYSTEM_SELECT_18005);

        // Set up the columnNames vector to be passed to executeQuery()
        std::string keyColumn("PKEY");
        std::string nameColumn("NAME");
        std::string descriptionColumn("DESCRIPTION");
        std::vector<std::string> columnNames;
        columnNames.push_back(keyColumn);
        columnNames.push_back(nameColumn);
        columnNames.push_back(descriptionColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(strSql, columnNames);

        // Loop for each row returned
        std::vector<ISubsystemState*> SubsystemStates;

        do
        {
            for (ta_uint32 i = 0; i < data->getNumRows(); i++)
            {
                // Catch any exceptions so we can clean up the pointers
                try
                {
                    SubsystemStates.push_back(new SubsystemState(data->getUnsignedLongData(i, keyColumn),
                                                                 data->getStringData(i, nameColumn),
                                                                 data->getStringData(i, descriptionColumn)));
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
        return SubsystemStates;
    }

} // closes TA_Base_Core
