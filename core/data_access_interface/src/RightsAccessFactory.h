/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/RightsAccessFactory.h $
 * @author:  Karen Graham
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * RightsAccessFactory is a singleton that is used to retrieve rights objects either from the
 * database or newly created. All rights objects returned will adhear to the IRights interface.
 *
 * These come from the SE_PROFILE_ACCESS database table.
 *
 */

#if !defined(RIGHTSACCESSFACTORY_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define RIGHTSACCESSFACTORY_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include "core/data_access_interface/src/IDatabase.h"
#include "core/types/src/ta_types.h"
#include <string>
#include <vector>
#include <map>
#include <tuple>

namespace TA_Base_Core
{
    class IConfigRights;
    class IRights;
    class IDatabase;

    // yanrong ++
    struct GroupQueryParam
    {
        size_t profileKey;
        size_t subsystemStatekey;

        friend bool operator<(const GroupQueryParam& lhs, const GroupQueryParam& rhs)
        {
            return std::tie(lhs.profileKey, lhs.subsystemStatekey) < std::tie(rhs.profileKey, rhs.subsystemStatekey);
        }
    };

    typedef std::map<GroupQueryParam, std::map<size_t, size_t> > ActionQueryParamMap;
    // ++ yanrong

    class RightsAccessFactory
    {
    public:

        virtual ~RightsAccessFactory()
        {
        }

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of an RightsAccessFactory object.
         */
        static RightsAccessFactory& getInstance();

        /**
         * getAllRights
         *
         * Retrieves all specified rights and returns the associated data as a vector of
         * objects conforming to the IRights interface.
         *
         * @return A vector of pointers to objects conforming to the IRights interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IRights
         * objects
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException
         */
        std::vector<IRights*> getAllRights(const bool readWrite = false);

        /**
         * getRights
         *
         * Retrieves all specified rights and returns the associated data as a vector of
         * objects conforming to the IRights interface.
         *
         * @return A vector of pointers to objects conforming to the IRights interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IRights
         * objects
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException
         */
        std::vector<IRights*> getRights(ta_uint32 subsystemStateKey, const bool readWrite = false);

        /**
         * createRights
         *
         * Creates a new configuration rights object.
         *
         * @param ta_uint32 - The key of the profile this rights configuration is for
         * @param ta_uint32 - The key of the subsystem this rights configuration is for
         *
         * @return The configuration rights object as a pointer to an IConfigRights object.
         *
         * @exception DatabaseException Thrown if there is a database problem.
         * @exception DataException Thrown if there is bad data.
         */
        IConfigRights* createRights(const ta_int32 profileKey, const ta_int32 subsystemKey,
                                    const ta_int32 subsystemStateKey);

        //TD15533++
        ta_uint32 getActionGroup(ta_uint32 profileKey,
                                 ta_uint32 subsystemKey,
                                 ta_uint32 subsystemStateKey);
        //++TD15533

        // yanrong ++
        // Query data from DB as many as possible at one time
        // to improve the launching speed of Duty Manager
        ta_uint32 getActionGroupEx(ta_uint32 profileKey,
                                   ta_uint32 subsystemKey,
                                   ta_uint32 subsystemStateKey);
        // ++ yanrong

    private:

        //
        // These are private as this method is a singleton
        //
        RightsAccessFactory() = default;

        RightsAccessFactory(const RightsAccessFactory& theRightsAccessFactory) = delete;
        RightsAccessFactory& operator=(const RightsAccessFactory&) = delete;

        void getRightBySql(IDatabase* pDatabase, const SQLStatement& sql, std::vector<IRights*>& rights, const bool readWrite);

        static RightsAccessFactory* m_instance;

        ActionQueryParamMap m_queryStore; // Yanrong ++
    };

} // closes TA_Base_Core

#endif // !defined(RightsAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
