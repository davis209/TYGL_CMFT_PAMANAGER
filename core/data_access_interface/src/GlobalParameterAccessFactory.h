/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/GlobalParameterAccessFactory.h $
 * @author:  Karen Graham
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * GlobalParameterAccessFactory is a singleton that is used to retrieve GlobalParameter objects either from the
 * database or newly created. All GlobalParameter objects returned will adhear to the IGlobalParameter interface.
 *
 */

#if !defined(GlobalParameterAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define GlobalParameterAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include "core/types/src/ta_types.h"
#include <string>
#include <vector>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IGlobalParameter.h"

namespace TA_Base_Core
{
    class IConfigGlobalParameter;

    class GlobalParameterAccessFactory
    {
    public:

        virtual ~GlobalParameterAccessFactory() {};

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of an GlobalParameterAccessFactory object.
         */
        static GlobalParameterAccessFactory& getInstance();

        /**
         * getAllGlobalParameters
         *
         * Retrieves all specified GlobalParameters and returns the associated data as a vector of
         * objects conforming to the IGlobalParameter interface.
         *
         * @return A vector of pointers to objects conforming to the IGlobalParameter interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IGlobalParameter
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
        std::vector<IGlobalParameter*> getAllGlobalParameters(const bool readWrite = false);

        /**
         * getAllGlobalParametersForSubsystem
         *
         * Retrieves all specified GlobalParameters and returns the associated data as a vector of
         * objects conforming to the IGlobalParameter interface.
         *
         * @return A vector of pointers to objects conforming to the IGlobalParameter interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IGlobalParameter
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
        std::vector<IGlobalParameter*> getAllGlobalParametersForSubsystem(ta_uint32 subsystemKey,
                                                                          const bool readWrite = false);

        /**
         * getGlobalParameterByKey
         *
         * Get a GlobalParameter based on the GlobalParameterKey
         *
         * @return IGlobalParameter* the
         *
         * @param ta_uint32 GlobalParameterKey
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException
         */
        IGlobalParameter* getGlobalParameterByKey(const ta_uint32 GlobalParameterKey, const bool readWrite = false);

        /**
         * getGlobalParameterByName
         *
         * Get a GlobalParameter based on the GlobalParameter name
         *
         * @return IGlobalParameter* the object representing the GlobalParameter with the specified name
         *
         * @param std::string GlobalParameterName The name of the GlobalParameter to retrieve
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException
         */
        IGlobalParameter* getGlobalParameterByNameAndSubsystem(const std::string& name,
                                                               ta_uint32 subsystemKey,
                                                               const bool readWrite = false);

        /**
         * getGlobalParameterKeyFromName
         *
         * Gets the GlobalParameter key of a station, given its name.
         *
         * @param GlobalParameterName  the name of the GlobalParameter whose key is returned.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. ACCESSLEVEL should be an
         *             ta_uint32 ), or if the wrong ammount of data is retrieved.
         */
        ta_uint32 getGlobalParameterKeyFromNameAndSubsystem(const std::string& name,
                                                            ta_uint32 subsystemKey);

        /**
         * getGlobalParameterValueFromNameAndSubsystem
         *
         * Gets the GlobalParameter value of a subsystem, given its name.
         *
         * @param GlobalParameterName  the name of the GlobalParameter whose key is returned.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. ACCESSLEVEL should be an
         *             ta_uint32 ), or if the wrong ammount of data is retrieved.
         */
        std::string getGlobalParameterValueFromNameAndSubsystem(const std::string& name,
                                                                ta_uint32 subsystemKey);

        /**
         * createGlobalParameter
         *
         * Creates a new configuration GlobalParameter object.
         *
         * @return The configuration GlobalParameter object as a pointer to an IConfigGlobalParameter object.
         *
         * @exception DatabaseException Thrown if there is a database problem.
         * @exception DataException Thrown if there is bad data.
         */
        IConfigGlobalParameter* createGlobalParameter();

        /**
         * copyGlobalParameter
         *
         * Creates a new configuration GlobalParameter object using the GlobalParameter passed in
         *
         * @param IConfigGlobalParameter* The GlobalParameter to copy
         *
         * @return The configuration GlobalParameter object as a pointer to an IConfigGlobalParameter object.
         *
         * @exception DatabaseException Thrown if there is a database problem.
         */
        IConfigGlobalParameter* copyGlobalParameter(const IConfigGlobalParameter* GlobalParameterToCopy);

    private:

        //
        // These are private as this method is a singleton
        //
        GlobalParameterAccessFactory() {};
        GlobalParameterAccessFactory(const GlobalParameterAccessFactory& theGlobalParameterAccessFactory);
        GlobalParameterAccessFactory& operator=(const GlobalParameterAccessFactory&);

        void getGlobalParameterBySql(IDatabase* pDatabase, const SQLStatement& sql, std::vector<IGlobalParameter*>& globalParameters, const bool readWrite);

        static GlobalParameterAccessFactory* m_instance;
    };

} // closes TA_Base_Core

#endif // !defined(GlobalParameterAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
