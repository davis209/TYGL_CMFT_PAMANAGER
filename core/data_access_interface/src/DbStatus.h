/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/DbStatus.h $
 * @author:  R.Stagg
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * This class maintains a list of 'known' databases.
 * It tracks the state of this databases to provide a quick reference of which
 * db's are avalable
 */

#pragma once

#include "core/utilities/src/RunParams.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include <string>
#include <map>

namespace TA_Base_Core
{
    enum EDbState
    {
        Pending,
        Online,
        Offline
    };

    class DbStatus : public RunParamUser
    {
    public:

        DbStatus();
        virtual ~DbStatus();

    public:

        /**
         * Retuns true if the requested database is not marked offline.
         * @param databaseId    an identifier used to query the cache to see if the db is
         * 'up'
         *
         */
        bool isAvailable(const std::string& databaseId);
        /**
         * Returns the only copy of this class.
         */
        static DbStatus& getInstance();
        static void removeInstance();

        /**
         * Callback for when a runparam changes
         */
        void onRunParamChange(const std::string& name, const std::string& value);

    private:

        /**
         * Adds a new database to the list of monitored db's.
         * @param dbId
         * @param dbState    The current state
         *
         */
        void addDbState(const std::string& dbId, const EDbState dbState);

        /**
         * Gets the current state of a known database.
         * If the database is not known an exception is thrown.
         * @param dbId    an identifier used to query the cache
         *
         */
        EDbState getDbState(const std::string& dbId);

        /**
         * Updates the state of a known database.
         * @param dbId    An identifier used to query the cache
         * @param dbState
         *
         */
        void updateDbState(const std::string& dbName, const EDbState dbState);

        /**
         * Private constructor/copy/assignment
         */
        DbStatus(const DbStatus& theDbStatus) = delete;

    private:

        typedef std::map<std::string, EDbState> DbList;
        DbList m_dbList;
        ReEntrantThreadLockable m_lock;
        static DbStatus* m_instance;
    };
}
