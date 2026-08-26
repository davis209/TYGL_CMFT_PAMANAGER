/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/ActionGroupAccessFactory.h $
 * @author:  Karen Graham
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * ActionGroupAccessFactory is a singleton that is used to retrieve ActionGroup objects either from the
 * database or newly created. All ActionGroup objects returned will adhear to the IActionGroup interface.
 * Data is primarily retrieved from the SE_ACTION_GROUP table
 *
 */

#if !defined(ACTIONGROUPACCESSFACTORY_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define ACTIONGROUPACCESSFACTORY_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include "core/types/src/ta_types.h"
#include <string>
#include <vector>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

namespace TA_Base_Core
{
    class IActionGroup;
    class IConfigActionGroup;

    class ActionGroupAccessFactory
    {
    public:

        virtual ~ActionGroupAccessFactory()
        {
        };

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of an ActionGroupAccessFactory object.
         */
        static ActionGroupAccessFactory& getInstance();

        /**
         * getActionGroup
         *
         * Retrieves the ActionGroup of the key provided.
         *
         * @return A pointer to an object conforming to the IActionGroup interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IActionGroup
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
        IActionGroup* getActionGroup(const ta_uint32 actionGroupKey, const bool readWrite = false);

        /**
         * getAllActionGroups
         *
         * Retrieves all specified ActionGroups and returns the associated data as a vector of
         * objects conforming to the IActionGroup interface.
         *
         * @return A vector of pointers to objects conforming to the IActionGroup interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IActionGroup
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
        std::vector<IActionGroup*> getAllActionGroups(const bool readWrite = false);

        /**
         * createActionGroup
         *
         * Creates a new configuration ActionGroup object.
         *
         * @return The configuration ActionGroup object as a pointer to an IConfigActionGroup object.
         *
         * @exception DatabaseException Thrown if there is a database problem.
         * @exception DataException Thrown if there is bad data.
         */
        IConfigActionGroup* createActionGroup();

        void getAllActionGroupKeys(std::vector<ta_uint32>& groupKeys);
        std::string getActionGroupName(ta_uint32 key);
        void getAllActionGroupNames(std::map<ta_uint32, std::string>& groupNames);

        /**
         * copyActionGroup
         *
         * Creates a new configuration ActionGroup object using the ActionGroup passed in
         *
         * @param IConfigActionGroup* The ActionGroup to copy
         *
         * @return The configuration ActionGroup object as a pointer to an IConfigActionGroup object.
         *
         * @exception DatabaseException Thrown if there is a database problem.
         */
        IConfigActionGroup* copyActionGroup(const IConfigActionGroup* actionGroupToCopy);

    private:

        //
        // These are private as this method is a singleton
        //
        ActionGroupAccessFactory();
        //avoid copy constructor
        ActionGroupAccessFactory(const ActionGroupAccessFactory& theActionGroupAccessFactory);
        //avoid copy constructor
        ActionGroupAccessFactory& operator=(const ActionGroupAccessFactory&);
        //initialize action group
        void initializeActionGroup();
        //get action by action group
        void getActionByActionGroup(ta_uint32 actionGroupId, std::vector<ta_uint32>& actions);

        /**
         * getActionGroup
         *
         * Retrieves the ActionGroup of the sql statement provided.
         *
         */
        void getActionGroupBySql(IDatabase* pDatabase, const SQLStatement& sql, std::vector<IActionGroup*>& actionGroups, const bool readWrite = false);

        typedef std::map< ta_uint32, std::vector<ta_uint32> > ActionGroupToActionMap;
        ActionGroupToActionMap m_actionGroupToActionMap;

        static ActionGroupAccessFactory* m_instance;
        bool m_initialized;
    };

} // closes TA_Base_Core

#endif // !defined(ActionGroupAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
