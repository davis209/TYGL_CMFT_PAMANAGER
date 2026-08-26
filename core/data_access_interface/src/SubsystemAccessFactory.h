/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/SubsystemAccessFactory.h $
 * @author:  Karen Graham
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * SubsystemAccessFactory is a singleton that is used to retrieve Subsystem objects either from the
 * database or newly created. All Subsystem objects returned will adhear to the ISubsystem interface.
 */

#if !defined(SUBSYSTEMACCESSFACTORY_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define SUBSYSTEMACCESSFACTORY_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include "core/types/src/ta_types.h"
#include <vector>

#include "core/data_access_interface/src/ConfigSubsystem.h"
#include "core/data_access_interface/src/IConfigSubsystem.h"
#include "core/data_access_interface/src/IDatabase.h"

namespace TA_Base_Core
{
    class ISubsystem;
    class IDatabase;

    class SubsystemAccessFactory
    {
    public:

        virtual ~SubsystemAccessFactory()
        {
        };

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of an SubsystemAccessFactory object.
         */
        static SubsystemAccessFactory& getInstance();

        /**
         * getAllSubsystems
         *
         * Retrieves all specified Subsystems and returns the associated data as a vector of
         * objects conforming to the ISubsystem interface.
         *
         * @return A vector of pointers to objects conforming to the ISubsystem interface.
         * N.B.: It is the responsibility of the *client* to delete the returned ISubsystem
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
        std::vector<ISubsystem*> getAllSubsystems(const bool readWrite = false);

        /**
         * getAllPhysicalSubsystems
         *
         * Retrieves all physical Subsystems and returns the associated data as a vector of
         * objects conforming to the ISubsystem interface.
         *
         * @return A vector of pointers to objects conforming to the ISubsystem interface.
         * N.B.: It is the responsibility of the *client* to delete the returned ISubsystem
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
        std::vector<ISubsystem*> getAllPhysicalSubsystems(const bool readWrite = false);

        /**
         * getSubsystemByKey
         *
         * Get a subsystem based on the pkey
         *
         * @return ISubsystem* the subsystem
         *
         * @param ta_uint32 key
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException
         */
        ISubsystem* getSubsystemByKey(const ta_uint32 key, const bool readWrite = false);

        std::vector<ISubsystem*> getSubsystemBySystemKeys(std::vector<ta_uint32> keys, const bool readWrite = false);

        /**
         * getSubsystemsAtLocation
         *
         * Retrieves all Subsystems associated with a given region. Returns the data
         * as a vector of objects conforming to the ISubsystem interface.
         *
         * @return A vector of pointers to objects conforming to the ISubsystem interface.
         * N.B.: It is the responsibility of the *client* to delete the returned ISubsystem
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
        std::vector<ISubsystem*> getSubsystemsByRegion(ta_uint32 regionKey,
                                                       const bool readWrite = false);

        /**
         * createSubsystem
         *
         * Creates a new configuration subsystem object.
         *
         * @return The configuration subsystem object as a pointer to an
         *         IConfigSubsystem object.
         *
         * @exception DatabaseException Thrown if there is a database problem.
         * @exception DataException Thrown if there is bad data.
         */
        IConfigSubsystem* createSubsystem();

        /**
         * copySubsystem
         *
         * Creates a new configuration subsystem object using
         * the subsystem passed in
         *
         * @param IConfigSubsystem* The subsystem to copy
         *
         * @return The configuration subsystem object as a pointer
         *         to an IConfigSubsystem object.
         *
         * @exception DatabaseException Thrown if there is a database problem.
         */
        IConfigSubsystem* copySubsystem(const IConfigSubsystem* subsystemToCopy);

    private:

        //
        // These are private as this method is a singleton
        //
        SubsystemAccessFactory()
        {
        };
        SubsystemAccessFactory(const SubsystemAccessFactory& theSubsystemAccessFactory);
        SubsystemAccessFactory& operator=(const SubsystemAccessFactory&);

        void getSubsystemBySql(IDatabase* pDatabase, const SQLStatement& sql, std::vector<ISubsystem*>& subsystems, const bool readWrite);

        static SubsystemAccessFactory* m_instance;
    };

} // closes TA_Base_Core

#endif // !defined(SubsystemAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
