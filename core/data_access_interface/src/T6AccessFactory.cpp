/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/T6AccessFactory.cpp $
 * @author:  Nick Jardine
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime:
 * Last modified by:  $Author:
 *
 * T6AccessFactory is a singleton that is used to retrieve Rights(for EventViewer) objects from the table t6.
 *
 */
#include "core/types/src/ta_types.h"
#include <vector>
#include "core/data_access_interface/src/T6AccessFactory.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/utilities/src/DebugUtil.h"

namespace
{
    //t6 table:
    const std::string COLUMN_T6_SEPROF_ID = "seprof_id";
    const std::string COLUMN_T6_SUBSYSTEM_KEY = "subsystemKey";
    const std::string COLUMN_T6_PHYSICAL_SUBSYSTEM_KEY = "physical_subsystem_key";
    const std::string COLUMN_T6_SESSTA_ID = "sessta_id";
    const std::string COLUMN_T6_SEAGRO_ID = "seagro_id";
    const std::string COLUMN_T6_SEACTI_ID = "seacti_id";
}

namespace TA_Base_Core
{
    T6AccessFactory* T6AccessFactory::m_instance = NULL;
    TA_Base_Core::NonReEntrantThreadLockable T6AccessFactory::m_singletonLock;

    T6AccessFactory& T6AccessFactory::getInstance()
    {
        TA_THREADGUARD(m_singletonLock);

        // Create a new instance if it doesn't exist.
        if (0 == m_instance)
        {
            m_instance = new T6AccessFactory();
        }

        return *m_instance;
    }

    T6AccessFactory::T6AccessFactory()
    {
        m_currentProfileID = 0;
        m_rightsCacheRetrieved = false;
    }

    bool T6AccessFactory::getRightsByProfileId(ta_uint32 profileId, T6AccessFactory::Rights& rights)
    {
        if (m_currentProfileID == profileId && m_rightsCacheRetrieved == true)
        {
            rights = m_rights;
            return true;
        }
       
        // We must reload the rights from database
		LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Profile ID does not match old ID = %d, new ID = %d, Reloading rights from T6", m_currentProfileID, profileId);
		retrieveRightsByProfileId(profileId);
		if (m_rightsCacheRetrieved) {
			rights = m_rights;
			return true;
		}
			
        return false;
    }

    void T6AccessFactory::retrieveRightsByProfileId(ta_uint32 profileId)
    {
        //This profileId's rights cache has already been loaded from DB
        if (profileId == m_currentProfileID && m_rightsCacheRetrieved == true)
        {
            return;
        }

        m_rights.clear();

        T6AccessFactory::Rights::iterator it = m_rights.end();

        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Event_Ad, Read);  //For test
        SQLStatement strSql;
        databaseConnection->prepareSQLStatement(strSql, T6_SELECT_90001, profileId);

        std::vector<std::string> columnNames;
        //columnNames.push_back(COLUMN_T6_SEPROF_ID);
        columnNames.push_back(COLUMN_T6_SUBSYSTEM_KEY);
        columnNames.push_back(COLUMN_T6_PHYSICAL_SUBSYSTEM_KEY);
        //columnNames.push_back(COLUMN_T6_SESSTA_ID);
        //columnNames.push_back(COLUMN_T6_SEAGRO_ID);
        columnNames.push_back(COLUMN_T6_SEACTI_ID);

        IData* data = databaseConnection->executeQuery(strSql, columnNames);

        do
        {
            for (unsigned int i = 0; i < data->getNumRows(); i++)
            {
                ta_uint32 subsystem_key = data->getUnsignedLongData(i, COLUMN_T6_SUBSYSTEM_KEY);
                ta_int32 physicalSubsystemKey = data->getIntegerData(i, COLUMN_T6_PHYSICAL_SUBSYSTEM_KEY);
                ta_uint32 seactiId = data->getUnsignedLongData(i, COLUMN_T6_SEACTI_ID);

                SubSystemKeys keys;
                keys.physicalSubsystemKey = physicalSubsystemKey;
                keys.subsystemKey = subsystem_key;

                it = m_rights.find(keys);

                if (it != m_rights.end())
                {
                    it->second.insert(seactiId);
                } else
                {
                    std::set<ta_uint32> actionSet;
                    actionSet.insert(seactiId);
                    m_rights.insert(std::pair<SubSystemKeys, std::set<ta_uint32> >(keys, actionSet));
                }
            }

            delete data;
            data = NULL;
        }
        while (databaseConnection->moreData(data));

        m_currentProfileID = profileId;
        m_rightsCacheRetrieved = true;
    }
};
