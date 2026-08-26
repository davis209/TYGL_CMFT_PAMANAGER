/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/data_access_interface/entity_access/src/OpcServerAgentAccessFactory.cpp $
 * @author:  Dhanshri
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2016/07/15 17:01:52 $
 * Last modified by:  $Author: dhanshri $
 *
 * Data is primarily retrieved from the entity table
 *
 */

#ifdef __WIN32__
    #pragma warning(disable:4786)
#endif

#include "core/types/src/ta_types.h"
#include "core/data_access_interface/src/OpcServerAgentAccessFactory.h"

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    void OpcServerAgentAccessFactory::getOccAndStationAgentList(std::vector<AgentData*>& list)
    {
        FUNCTION_ENTRY("getOccAndStationAgentList");
        // Get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineSecurity_Cd, Read);

        //std::string sql = "";
        //sql = "select distinct AGENTKEY, AGENTNAME from entity_v where typename  in ('DataPoint','DataNode') and deleted=0 and agentname not like '%OpcServerAgent'";

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        std::string agKeyColumn = "AGENTKEY";
        std::string agNameColumn = "AGENTNAME";

        SQLStatement strSql;
        databaseConnection->prepareSQLStatement(strSql, ENTITY_SELECT_4555);

        columnNames.push_back(agKeyColumn);
        columnNames.push_back(agNameColumn);
        IData* data = databaseConnection->executeQuery(strSql, columnNames);

        // Loop for each row returned
        do
        {
            unsigned int numRows = data->getNumRows();

            for (ta_uint32 row = 0; row < numRows; row++)
            {
                // The getUnsignedLongData() call can throw an exception. Need to catch
                // it to do pointer clean up.
                try
                {
                    //Do not insert for AgentKey <= 0
                    if (data->getUnsignedLongData(row, agKeyColumn) > 0)
                    {
                        AgentData* objAgentData = new AgentData(data->getUnsignedLongData(row, agKeyColumn), data->getStringData(row, agNameColumn));
                        list.push_back(objAgentData);
                    }
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
    }

} // closes TA_Base_Core
