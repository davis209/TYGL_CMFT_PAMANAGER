/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/ConfigRights.cpp $
 * @author:  Karen Graham
 * @version: $Revision: #4 $
 *
 * Last modification: $DateTime: 2024/09/27 18:12:18 $
 * Last modified by:  $Author: josef $
 *
 * ConfigRights is an implementation of IConfigRights. It holds the data specific to a Rights entry
 * in the database, and allows read-write access to that data.
 *
 * This class does not have an invalidate() method like other similar classes as the data does not change
 * and is loaded in bulk. Because the Config Editor uses rights differently to other objects there is no need
 * to conform to the standard.
 */

#ifdef __WIN32__
    #pragma warning(disable:4786)
    #pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include "core/types/src/ta_types.h"
#include "core/data_access_interface/src/ConfigRights.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/data_access_interface/src/ActionGroupAccessFactory.h"
#include "core/data_access_interface/src/IActionGroup.h"
#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/data_access_interface/src/IProfile.h"
#include "core/data_access_interface/src/SubsystemAccessFactory.h"
#include "core/data_access_interface/src/ISubsystem.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
    ta_uint32 ConfigRights::s_nextAvailableIdentifier = 1;

    const std::string ConfigRights::ACTIONGROUP = "Action Group";
    const ta_int32 ConfigRights::INVALID_ACTIONGROUP = -1;

    ConfigRights::ConfigRights(const ta_uint32 profileKey, ta_uint32 subsystemKey,
                               ta_uint32 subsystemStateKey, bool isPhysical)
        : m_uniqueIdentifier(s_nextAvailableIdentifier++),
          m_profileKey(profileKey),
          m_subsystemKey(subsystemKey),
          m_subsystemStateKey(subsystemStateKey),
          m_isPhysicalSubsystem(isPhysical),
          m_actionGroupKey(INVALID_ACTIONGROUP),
          m_originalActionGroupKey(INVALID_ACTIONGROUP),
          m_isNew(true)

    {
    }

    ConfigRights::ConfigRights(const ta_uint32 profileKey, ta_uint32 subsystemKey, ta_uint32 subsystemStateKey,
                               ta_uint32 actionGroupKey, bool isPhysical)
        : m_uniqueIdentifier(s_nextAvailableIdentifier++),
          m_profileKey(profileKey),
          m_subsystemKey(subsystemKey),
          m_subsystemStateKey(subsystemStateKey),
          m_isPhysicalSubsystem(isPhysical),
          m_actionGroupKey(actionGroupKey),
          m_originalActionGroupKey(actionGroupKey),
          m_isNew(false)
    {
    }

    ConfigRights::ConfigRights(const ta_uint32 row, TA_Base_Core::IData& data)
        : m_uniqueIdentifier(s_nextAvailableIdentifier++),
          m_isNew(false)
    {
    }

    ConfigRights::~ConfigRights()
    {
        try
        {
        }

        // exception
        catch (...)
        {
            LOG_EXCEPTION("Unknown", "Caught in destructor");
        }
    }

    ItemChanges ConfigRights::getAllItemChanges()
    {
        ItemChanges changes;

        if (m_originalActionGroupKey == m_actionGroupKey)
        {
            // No changes have been made
            return changes;
        }

        // Otherwise changes have been made and we need to determine the ActionGroup names
        Values values;
        values.newValue = "Unknown";
        values.oldValue = "Unknown";

        IActionGroup* actionGroup = NULL;

        try
        {
            // Retrieve the names of the ActionGroup items
            if (m_actionGroupKey == INVALID_ACTIONGROUP)
            {
                values.newValue = "None";
            }

            else
            {
                actionGroup = ActionGroupAccessFactory::getInstance().getActionGroup(m_actionGroupKey);
                values.newValue = actionGroup->getName();

                delete actionGroup;
                actionGroup = NULL;
            }

            if (m_originalActionGroupKey == INVALID_ACTIONGROUP)
            {
                values.oldValue = "None";
            }

            else
            {
                actionGroup = ActionGroupAccessFactory::getInstance().getActionGroup(m_originalActionGroupKey);
                values.oldValue = actionGroup->getName();

                delete actionGroup;
                actionGroup = NULL;
            }
        }
        //database exception
        catch (const DatabaseException&)
        {
            LOG_EXCEPTION("DatabaseException", "Could not retrieve the ActionGroup names");

            if (actionGroup != NULL)
            {
                delete actionGroup;
                actionGroup = NULL;
            }
        }
        //data exception
        catch (const DataException&)
        {
            LOG_EXCEPTION("DataException", "Could not retrieve the ActionGroup names");

            if (actionGroup != NULL)
            {
                delete actionGroup;
                actionGroup = NULL;
            }
        }

        // Now add this change to the vector and return it.
        changes.insert(ItemChanges::value_type(ACTIONGROUP, values));

        return changes;
    }

    std::string ConfigRights::getFormattedName()
    {
        FUNCTION_ENTRY("getFormattedName");

        std::ostringstream formattedName;

        IProfile* profile = NULL;
        ISubsystem* subsystem = NULL;

        try
        {
            profile = ProfileAccessFactory::getInstance().getProfile(m_profileKey);
            subsystem = SubsystemAccessFactory::getInstance().getSubsystemByKey(m_subsystemKey);

            formattedName << "Profile: ";
            formattedName << profile->getName();
            formattedName << ", Subsystem: ";
            formattedName << subsystem->getName();
            formattedName << "";
        }
        //database exception
        catch (const DatabaseException&)
        {
            LOG_EXCEPTION("DatabaseException", "Could not determine profile or subsystem name for a rights change");

            if (profile != NULL)
            {
                delete profile;
                profile = NULL;
            }

            if (subsystem != NULL)
            {
                delete subsystem;
                subsystem = NULL;
            }

            FUNCTION_EXIT;
            return "Unknown";
        }
        //data exception
        catch (const DataException&)
        {
            LOG_EXCEPTION("DataException", "Could not determine profile or subsystem name for a rights change");

            if (profile != NULL)
            {
                delete profile;
                profile = NULL;
            }

            if (subsystem != NULL)
            {
                delete subsystem;
                subsystem = NULL;
            }

            FUNCTION_EXIT;
            return "Unknown";
        }

        FUNCTION_EXIT;
        return formattedName.str();
    }

    void ConfigRights::applyChanges()
    {
        FUNCTION_ENTRY("applyChanges");

        if (m_isNew)
        {
            if (m_actionGroupKey == INVALID_ACTIONGROUP)
            {
                // No need to add this entry as it is a blank one. We'll just leave it as a new entry
                // and not change anything.
                FUNCTION_EXIT;
                return;
            }
            else
            {
                // we need to add this as a new setting
                addNewRights();
            }
        }
        else // we have an existing entry
        {
            if (m_actionGroupKey == INVALID_ACTIONGROUP)
            {
                // This has been blanked so we need to delete it
                deleteThisRight();

                // This right no longer exists in the database so we must set it to be a new one
                m_originalActionGroupKey = INVALID_ACTIONGROUP;
                m_isNew = true;

                FUNCTION_EXIT;
                return;
            }
            else if (m_actionGroupKey != m_originalActionGroupKey)
            {
                modifyExistingRights();
            }

            // else nothing has changed so we let it be
        }

        m_isNew = false;
        m_originalActionGroupKey = m_actionGroupKey;

        FUNCTION_EXIT;
    }

    void ConfigRights::deleteThisRight()
    {
        FUNCTION_ENTRY("deleteThisRight");

        // get a connection to the database
        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase({ OnlineSecurity_Cd, OnlineSecurity_Cached}, Write);

        // Now we can get on with writing the rights to the database.
        // generate the SQL string to write the rights
        /* std::ostringstream sql;;
         sql << "delete SE_PROFILE_ACCESS where SEPROF_ID = " << m_profileKey;
         sql << " and SUBSYSTEMKEY = " << m_subsystemKey;
        sql << " and SESSTA_ID = " << m_subsystemStateKey;
         sql << " and SEAGRO_ID = " << m_originalActionGroupKey;*/
        //      std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), se_profile_access_STD_DELETE_12901, m_profileKey,
        //      std::string strSql  = databaseConnection->prepareSQLStatement(SE_PROFILE_ACCESS_DELETE_12901, m_profileKey,
        SQLStatement strSql;
        databaseConnection->prepareSQLStatement(strSql, SE_PROFILE_ACCESS_DELETE_12901, m_profileKey,
                                                m_subsystemKey, m_subsystemStateKey, m_originalActionGroupKey);

        databaseConnection->executeModification(strSql);

        FUNCTION_EXIT;
    }

    void ConfigRights::addNewRights()
    {
        FUNCTION_ENTRY("addNewRights");

        // get a connection to the database
        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase({ OnlineSecurity_Cd, OnlineSecurity_Cached}, Write);

        // Now we can get on with writing the rights to the database.
        // generate the SQL string to write the rights
        /*std::ostringstream sql;
        sql << "insert into SE_PROFILE_ACCESS (SEPROF_ID,SUBSYSTEMKEY,SESSTA_ID,SEAGRO_ID) values ";
        sql << "(" << m_profileKey << "," << m_subsystemKey << "," << m_subsystemStateKey << "," << m_actionGroupKey << ")";*/
        //      std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), se_profile_access_STD_INSERT_12801, m_profileKey,
        //      std::string strSql  = databaseConnection->prepareSQLStatement(SE_PROFILE_ACCESS_INSERT_12801, m_profileKey,
        SQLStatement strSql;
        databaseConnection->prepareSQLStatement(strSql, SE_PROFILE_ACCESS_INSERT_12801, m_profileKey,
                                                m_subsystemKey, m_subsystemStateKey, m_actionGroupKey);

        databaseConnection->executeModification(strSql);

        FUNCTION_EXIT;
    }

    void ConfigRights::modifyExistingRights()
    {
        FUNCTION_ENTRY("modifyExistingRights");

        // get a connection to the database
        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase({ OnlineSecurity_Cd, OnlineSecurity_Cached}, Write);

        // Now we can get on with writing the rights to the database.
        // generate the SQL string to write the rights
        /*std::ostringstream sql;;
        sql << "update SE_PROFILE_ACCESS set SEAGRO_ID = " << m_actionGroupKey;
        sql << " where SEPROF_ID = " << m_profileKey;
        sql << " and SUBSYSTEMKEY = " << m_subsystemKey;
        sql << " and SESSTA_ID = " << m_subsystemStateKey;*/
        //      std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), se_profile_access_STD_UPDATE_12651, m_actionGroupKey,
        //      std::string strSql  = databaseConnection->prepareSQLStatement(SE_PROFILE_ACCESS_UPDATE_12651, m_actionGroupKey,
        SQLStatement strSql;
        databaseConnection->prepareSQLStatement(strSql, SE_PROFILE_ACCESS_UPDATE_12651, m_actionGroupKey,
                                                m_profileKey, m_subsystemKey, m_subsystemStateKey);

        databaseConnection->executeModification(strSql);

        FUNCTION_EXIT;
    }

    void ConfigRights::setSubsystemState(ta_uint32 subsystemStateKey)
    {
        m_subsystemStateKey = subsystemStateKey;
    }

} // closes TA_Base_Core
