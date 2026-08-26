/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/ActionAccessFactory.h $
 * @author:  Karen Graham
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * ActionAccessFactory is a singleton that is used to retrieve action objects either from the
 * database or newly created. All action objects returned will adhear to the IAction interface.
 * Data is primarily retrieved from the SE_ACTION table
 *
 */

#pragma once
#include "core/types/src/ta_types.h"
#include <string>
#include <vector>
#include <set>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

namespace TA_Base_Core
{
    class IAction;

    class ActionAccessFactory
    {
    public:

        virtual ~ActionAccessFactory()
        {
        }

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of an ActionAccessFactory object.
         */
        static ActionAccessFactory& getInstance();

        /**
         * getAction
         *
         * Retrieves the action of the key provided.
         *
         * @return A pointer to an object conforming to the IAction interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IAction
         * object
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possible
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException
         */
        IAction* getAction(const ta_uint32 actionKey);

        /**
         * getAllActions
         *
         * Retrieves all specified actions and returns the associated data as a vector of
         * objects conforming to the IAction interface.
         *
         * @return A vector of pointers to objects conforming to the IAction interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IAction
         * objects
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possible
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException
         */
        std::vector<IAction*> getAllActions();

        //limin++ for centralize duty/rights                action_group   action_keys
        typedef std::map< ta_uint32, std::vector<ta_uint32> > ActionGroupMap;
        void getActionGroupMap(const std::set<ta_uint32>& groupIdList, ActionGroupMap& actionGroupMap);
        void getNonPhysicalSubsystemActionGroup(const std::vector<ta_uint32>& profileList, std::multimap<ta_uint32, ta_uint32>& groupIdList);

    private:

        //
        // These are private as this method is a singleton
        //
        ActionAccessFactory();

        void initializeAction();
        void getActionGroupByAction(ta_uint32 actionId, std::vector<std::string>& actionGroups);
        void getActionBySql(IDatabase* pDatabase, const SQLStatement& sql, std::vector<IAction*>& actions);

        ActionAccessFactory(const ActionAccessFactory& theActionAccessFactory);
        ActionAccessFactory& operator=(const ActionAccessFactory&);

        typedef std::vector <std::string> ActionGroupVector;

        typedef std::map< ta_uint32, std::vector<std::string> > ActionToActionGroupMap;
        ActionToActionGroupMap m_actionToActionGroupMap;

        static ActionAccessFactory* m_instance;
    };
}
