/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/LocationAccessFactory.h $
 * @author:  Karen Graham
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * LocationAccessFactory is a singleton that is used to retrieve Location objects either from the
 * database or newly created. All Location objects returned will adhear to the ILocation interface.
 *
 */

#pragma once
#include "core/types/src/ta_types.h"
#include <string>
#include <vector>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/ILocation.h"

namespace TA_Base_Core
{
    class IConfigLocation;

    class LocationAccessFactory
    {
    public:

        virtual ~LocationAccessFactory()
        {
        }

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of an LocationAccessFactory object.
         */
        static LocationAccessFactory& getInstance();

        /**
         * getAllLocations
         *
         * Retrieves all specified Locations and returns the associated data as a vector of
         * objects conforming to the ILocation interface.
         *
         * @return A vector of pointers to objects conforming to the ILocation interface.
         * N.B.: It is the responsibility of the *client* to delete the returned ILocation
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
        std::vector<ILocation*> getAllLocations(bool readWrite = false);
        ILocationPtrList& getAllLocationPtrs(bool readWrite = false);

        //xinsong++, exclude location=0
        std::vector<ILocation*> getAllAgentLocations(bool readWrite = false);

        /**
         * getLocationByKey
         *
         * Get a location based on the locationKey
         *
         * @return ILocation* the
         *
         * @param size_t locationKey
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException
         */
        ILocation* getLocationByKey(const ta_uint32 locationKey, const bool readWrite = false) { return getLocationByKeyImpl(locationKey, readWrite); }
        ILocation* getLocationByKeyImpl(size_t locationKey, bool readWrite = false);

        /**
         * getLocationByName
         *
         * Get a location based on the location name
         *
         * @return ILocation* the object representing the location with the specified name
         *
         * @param std::string locationName The name of the location to retrieve
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException
         */
        ILocation* getLocationByName(const std::string& locationKey, bool readWrite = false);

        /**
         * getLocationsByProfile
         *
         * Get all locations associated with the specified profile
         *
         * @return A vector of pointers to ILocation objects that are associated with the specified profile
         *
         * @param profileKey The key to the profile for which to retreive associated locations
         *
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         */
        std::vector<ILocation*> getLocationsByProfile(size_t profileKey, bool readWrite = false);

        /**
         * getLocationType
         *
         * Get the type of a location given its name.
         *
         * @param locationName  the name of the location whose type is returned.
         *
         * Returns the location type for this location. This is used in applications that differentiate between
         * a central location (or locations), and general locations. For C830 this will determine the OCC, Depot,
         * and station locaitons.
         *
         * @return The location type for this location.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. ACCESSLEVEL should be an
         *            size_t ), or if the wrong ammount of data is retrieved.
         */
        ILocation::ELocationType getLocationType(const std::string& locationName) const;
        ILocation::ELocationType getLocationType(size_t locationKey) const;

        /**
         * getOccLocationKey
         *
         * Get the location key of the OCC.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. ACCESSLEVEL should be an
         *            size_t ), or if the wrong ammount of data is retrieved.
         */
        size_t getOccLocationKey();

        size_t getOccComLocationKey();
        size_t getOccIntLocationKey();
        size_t getOccScadaLocationKey();

        size_t getBoccLocationKey();

        std::string getOccComLocationName();

        /**
         * getDepotLocationKey
         *
         * Get the location key of the Depot.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. ACCESSLEVEL should be an
         *             size_t ), or if the wrong ammount of data is retrieved.
         */
        size_t getDepotLocationKey();

        //hongran++ TD 15473

        /**
         * getDepotLocationKey
         *
         * Get the location key of the Depot of ND.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. ACCESSLEVEL should be an
         *             size_t ), or if the wrong ammount of data is retrieved.
         */
        size_t getDepotNDLocationKey();
        /**
         * getDepotLocationKey
         *
         * Get the location key of the Depot of SD.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. ACCESSLEVEL should be an
         *             size_t ), or if the wrong ammount of data is retrieved.
         */
        size_t getDepotSDLocationKey();
        /**
         * getDepotLocationKey
         *
         * Get the location key of the Depot of MD.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. ACCESSLEVEL should be an
         *             size_t ), or if the wrong ammount of data is retrieved.
         */
        size_t getDepotMDLocationKey();
        //++hongran TD 15473

        /**
         * getLocationKeyFromName
         *
         * Gets the location key of a station, given its name.
         *
         * @param locationName  the name of the location whose key is returned.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. ACCESSLEVEL should be an
         *             size_t ), or if the wrong ammount of data is retrieved.
         */
        size_t LocationAccessFactory::getLocationKeyFromName(const std::string& name);

        /**
         * Get collection of TIMS origin values.
         */
        std::map<std::string, unsigned char> getTIMSOriginValues();

        /**
         * createLocation
         *
         * Creates a new configuration location object.
         *
         * @return The configuration location object as a pointer to an IConfigLocation object.
         *
         * @exception DatabaseException Thrown if there is a database problem.
         * @exception DataException Thrown if there is bad data.
         */
        IConfigLocation* createLocation();

        /**
         * copyLocation
         *
         * Creates a new configuration location object using the location passed in
         *
         * @param IConfigLocation* The location to copy
         *
         * @return The configuration location object as a pointer to an IConfigLocation object.
         *
         * @exception DatabaseException Thrown if there is a database problem.
         */
        IConfigLocation* copyLocation(const IConfigLocation* locationToCopy);

        size_t getDepotGBDLocationKey();

        size_t getDepotGDCCLocationKey();

        std::map<ta_uint32, std::pair<ta_uint32, std::string> > getOrderIdsFromRegionKeys(const std::vector<ta_uint32>& theRegionKeys);

        std::vector<ILocation*> getLocationsByType(ILocation::ELocationType locationType, bool readWrite = false);

    protected:

        LocationAccessFactory() = default;

    private:

        //
        // These are private as this method is a singleton
        //
        LocationAccessFactory(const LocationAccessFactory& theLocationAccessFactory) = delete;
        LocationAccessFactory& operator=(const LocationAccessFactory&) = delete;

        void getLocationBySql(IDatabase* pDatabase, const SQLStatement& sql, std::vector<ILocation*>& locations, bool readWrite);

        ILocation::ELocationType convertLocationName2Type(const std::string& locationName) const; // old logic

    public:

        // The names of the OCC and Depot in the database.
        static const std::string OCC_LOCATION_NAME;
        static const std::string OCC_COM_LOCATION_NAME;
        static const std::string OCC_INT_LOCATION_NAME;
        static const std::string OCC_SCADA_LOCATION_NAME;

        static const std::string BOCC_LOCATION_NAME;

        static const std::string DEPOT_LOCATION_NAME;
        //hongran++ TD 15473
        static const std::string DEPOT_ND_LOCATION_NAME;
        static const std::string DEPOT_SD_LOCATION_NAME;
        static const std::string DEPOT_MD_LOCATION_NAME;
        //++hongran TD 15473

        //955 depots
        static const std::string DEPOT_GBD_LOCATION_NAME;
        static const std::string DEPOT_GDCC_LOCATION_NAME;

        static const size_t ALL_LOCATION;
#if 0
        // TODO
        static const size_t UNKNOWN_LOCATION;
        static const std::string UNKNOWN_LOCATION_NAME;
#endif
    };
}
