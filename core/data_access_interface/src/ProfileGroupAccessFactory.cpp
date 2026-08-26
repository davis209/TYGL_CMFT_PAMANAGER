/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/ProfileGroupAccessFactory.cpp $
 * @author:  Anita Lee
 * @version: $Revision: #4 $
 *
 * Last modification: $DateTime: 2024/09/27 18:12:18 $
 * Last modified by:  $Author: josef $
 *
 * ProfileGroupAccessFactory is a singleton that is used to retrieve profile group objects either from the
 * database or newly created. All profile group objects returned will adhere to the IProfileGroup interface.
 * Data is primarily retrieved from the SE_PROFILE_GROUP table
 *
 */

#ifdef __WIN32__
    #pragma warning(disable:4786)
#endif

#include "core/types/src/ta_types.h"
#include "core/data_access_interface/src/ProfileGroupAccessFactory.h"
#include "core/data_access_interface/src/ProfileGroup.h"
#include "core/data_access_interface/src/ConfigProfileGroup.h"
#include "core/data_access_interface/src/IConfigProfileGroup.h"

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    ProfileGroupAccessFactory* ProfileGroupAccessFactory::m_instance = 0;

    ProfileGroupAccessFactory& ProfileGroupAccessFactory::getInstance()
    {
        if (m_instance == 0)
        {
            m_instance = new ProfileGroupAccessFactory();
        }

        return *m_instance;
    }

    void ProfileGroupAccessFactory::getProfileGroupBySql(IDatabase* pDatabase, const SQLStatement& sql, std::vector<IProfileGroup*>& profileGroups, const bool readWrite)
    {
        FUNCTION_ENTRY("getProfileGroupBySql");

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back("SEPGRO_ID");
        columnNames.push_back("NAME");
        columnNames.push_back("DATE_MODIFIED");
        columnNames.push_back("DATE_CREATED");

        // Get a connection to the database

        TA_ASSERT(NULL != pDatabase, "database connection is null");
        IData* data = pDatabase->executeQuery(sql, columnNames);

        // Loop for each row returned
        do
        {
            unsigned int numRows = data->getNumRows();

            for (ta_uint32 i = 0; i < numRows; i++)
            {
                // The getUnsignedLongData() call can throw an exception. Need to catch
                // it to do pointer clean up.
                try
                {
                    if (readWrite)
                    {
                        // create new config profile group if readWrite is enabled
                        profileGroups.push_back(new ConfigProfileGroup(i, *data));
                    }
                    else
                    {
                        // create read only profile group if readWrite is disabled
                        profileGroups.push_back(new ProfileGroup(i, *data));
                    }
                }
                //exception
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
        while (pDatabase->moreData(data));

        FUNCTION_EXIT;
    }

    IProfileGroup* ProfileGroupAccessFactory::getProfileGroup(const ta_uint32 ProfileGroupKey, const bool readWrite /*false*/)
    {
        // Get a specific profile group
        FUNCTION_ENTRY("getProfileGroup");

        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase({ OnlineSecurity_Cd, OnlineSecurity_Cached}, Read);
        //      std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_PROFILE_GROUP_Oracle_SELECT_19551, ProfileGroupKey);
        //      std::string strSql  = databaseConnection->prepareSQLStatement(SE_PROFILE_GROUP_SELECT_19551, ProfileGroupKey);
        SQLStatement strSql;
        databaseConnection->prepareSQLStatement(strSql, SE_PROFILE_GROUP_SELECT_19551, ProfileGroupKey);
        //      TA_ASSERT(strSql.size() > 0, "ProfileGroupAccessFactory::getProfileGroup(): Prepare SQL Statement error");

        // Create the query
        /* std::ostringstream sql;
        sql << " select SEPGRO_ID, NAME,"
         << " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
         << " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS')"
         << " from SE_PROFILE_GROUP where SEPGRO_ID = " << ProfileGroupKey;*/

        std::vector<IProfileGroup*> profileGroups;
        getProfileGroupBySql(databaseConnection, strSql, profileGroups, readWrite);

        if (0 == profileGroups.size())
        {
            std::ostringstream message;
            message << "No data found for ProfileGroup with key " << ProfileGroupKey;
            // TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, strSql ) );

            std::string strExSql = databaseConnection->getSqlString(strSql);
            TA_THROW(DataException(message.str().c_str(), DataException::NO_VALUE, strExSql));
        }

        TA_ASSERT(1 == profileGroups.size(), "ProfileGroup key unique constraint violated");

        // Return the pointer. The class that recieves this pointer is responsible for deleting it.
        FUNCTION_EXIT;
        return profileGroups[0];
    }

    std::vector<IProfileGroup*> ProfileGroupAccessFactory::getAllProfileGroups(const bool readWrite /*false*/)
    {
        FUNCTION_ENTRY("getAllProfileGroups");

        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase({ OnlineSecurity_Cd, OnlineSecurity_Cached}, Read);
        //      std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SE_PROFILE_GROUP_Oracle_SELECT_19552);
        //      std::string strSql  = databaseConnection->prepareSQLStatement(SE_PROFILE_GROUP_SELECT_19552);
        SQLStatement strSql;
        databaseConnection->prepareSQLStatement(strSql, SE_PROFILE_GROUP_SELECT_19552);
        //      TA_ASSERT(strSql.size() > 0, "ProfileGroupAccessFactory::getAllProfileGroups(): Prepare SQL Statement error");

        // Create the query
        /*std::ostringstream sql;
        sql << " select SEPGRO_ID, NAME,"
            << " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS'),"
            << " TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS')"
            << " from SE_PROFILE_GROUP";*/

        std::vector<IProfileGroup*> profileGroups;
        getProfileGroupBySql(databaseConnection, strSql, profileGroups, readWrite);

        // Return the vector
        FUNCTION_EXIT;
        return profileGroups;
    }

    IConfigProfileGroup* ProfileGroupAccessFactory::createProfileGroup()
    {
        return new ConfigProfileGroup();
    }

    IConfigProfileGroup* ProfileGroupAccessFactory::copyProfileGroup(const IConfigProfileGroup* profileGpToCopy)
    {
        FUNCTION_ENTRY("copyProfileGroup");

        TA_ASSERT(profileGpToCopy !=  NULL, "The profile group to copy was NULL");

        const ConfigProfileGroup* cast = dynamic_cast<const ConfigProfileGroup*>(profileGpToCopy);

        TA_ASSERT(cast != NULL, "Profile group passed could not be converted into a ConfigProfileGroup");

        FUNCTION_EXIT;

        return new ConfigProfileGroup(*cast);
    }

} // closes TA_Base_Core
