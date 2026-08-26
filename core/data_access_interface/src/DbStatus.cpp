/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/DbStatus.cpp $
 * @author:  R.Stagg
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 */

#ifdef __WIN32__
    #pragma warning(disable:4786)  // identifier was truncated to 'number' characters in the debug information
    #pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include "core/utilities/src/TAAssert.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/ThreadReadGuard.h"
#include "core/data_access_interface/src/DbStatus.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParamsEx.h"
#include "core/utilities/src/StaticObject.h"
#include "core/synchronisation/src/DoubleCheckedLocking.h"

namespace TA_Base_Core
{
    DbStatus* DbStatus::m_instance = 0;

    DbStatus::DbStatus()
    {
    }

    DbStatus::~DbStatus()
    {
    }

    DbStatus& DbStatus::getInstance()
    {
        return *double_checked_locking_new<DbStatus>(m_instance, StaticObject<ReEntrantThreadLockable, DbStatus>::value());
    }

    void DbStatus::removeInstance()
    {
        double_checked_locking_delete(m_instance, StaticObject<ReEntrantThreadLockable, DbStatus>::value());
    }

    bool DbStatus::isAvailable(const std::string& databaseId)
    {
        FUNCTION_ENTRY("isAvailable");

        // Check first if we know about this database.
        try
        {
            EDbState dbstate = getDbState(databaseId);

            // we do know about this database
            if (dbstate == Offline)
            {
                // it's known to be bad
                LOG_DEBUG("Return status of %s offline", databaseId);
                return false;
            }

            // it's not marked bad so return true.
            FUNCTION_EXIT;
            LOG_DEBUG("Return status of %s online", databaseId);
            return true;
        }
        catch (...)
        {
            // If not, do we know about this database.
            std::string pointname(RPARAM_DBPREFIX + databaseId);

            LOG_DEBUG("Not in map yet checking if system controller has set runtime parameter %s", pointname);

            if (RunParams::getInstance().isSet(pointname.c_str()))
            {
                EDbState state = RunParamsEx::iequals(pointname, RPARAM_DBONLINE) ? Online : Offline;
                addDbState(databaseId, state);
                RunParams::getInstance().registerRunParamUser(this, pointname.c_str());

                if (state == Online)
                {
                    FUNCTION_EXIT;
                    LOG_DEBUG("System controller runtime parameter %s is set to online", pointname);
                    return true;
                }
                else
                {
                    FUNCTION_EXIT;
                    LOG_DEBUG("System controller runtime parameter %s is set to offline", pointname);
                    return false;
                }
            }
            else // No not yet :(
            {
                LOG_DEBUG("System controller has not yet is set %s default setting to offline", pointname);

                // add point to runparmas and our own list of db's
                RunParams::getInstance().set(pointname.c_str(), RPARAM_DBUNKNOWN);
                //addDbState(databaseId, Online);
                addDbState(databaseId, Offline);

                RunParams::getInstance().registerRunParamUser(this, pointname.c_str());
                FUNCTION_EXIT;
                //return true;
                return false;
            }
        }

        // should never get here but if we do,
        FUNCTION_EXIT;
        return true;
    }

    void DbStatus::onRunParamChange(const std::string& name, const std::string& value)
    {
        FUNCTION_ENTRY("onRunParamChange");
        std::string dbId(name);
        dbId.erase(0, name.find_first_of("_") + 1);

        if (value.compare(RPARAM_DBONLINE) == 0)
        {
            LOG_DEBUG("Runtime param change detected, updating state for dbid %s to online", dbId);

            updateDbState(dbId, Online);
        }
        else
        {
            LOG_DEBUG("	Runtime param change detected, updating state for dbid %s to offline", dbId);

            updateDbState(dbId, Offline);
        }

        FUNCTION_EXIT;
    }

    EDbState DbStatus::getDbState(const std::string& dbId)
    {
        FUNCTION_ENTRY("getDbState");
        TA_THREADGUARD(m_lock);
        DbList::iterator db = m_dbList.find(dbId);

        if (db == m_dbList.end())
        {
            throw 1; // make real exception
        }

        FUNCTION_EXIT;
        return db->second;
    }

    void DbStatus::addDbState(const std::string& dbId, const EDbState dbState)
    {
        FUNCTION_ENTRY("addDbState");
        TA_THREADGUARD(m_lock);
        m_dbList.erase(dbId);
        m_dbList.emplace(dbId, dbState);
        FUNCTION_EXIT;
    }

    void DbStatus::updateDbState(const std::string& dbId, const EDbState dbState)
    {
        FUNCTION_ENTRY("updateDbState");
        TA_THREADGUARD(m_lock);
        DbList::iterator db = m_dbList.find(dbId);

        if (db == m_dbList.end())
        {
            addDbState(dbId, dbState);
            return;
        }

        db->second = dbState;
        FUNCTION_EXIT;
    }
}
