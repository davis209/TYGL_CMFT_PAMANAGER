/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source: $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/ECSStationColourHelper.h $
 * @author Andy Siow
 * @version $Revision: #3 $
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by: $Author: CM $
 *
 * Provides access to the ECS Station Colour data in the database.
 */

#ifndef ECS_STATION_COLOUR_HELPER_H
#define ECS_STATION_COLOUR_HELPER_H

#include "core/types/src/ta_types.h"
#include <string>
#include <vector>

#include "core/data_access_interface/src/IConfigECSStationColourData.h"

namespace TA_Base_Core
{
    // forward declarations
    class IData;

    class ECSStationColourHelper
    {
    public:

        /**
         * applyChanges
         *
         * This will apply all changes made to the database.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if the a parameter name cannot be found, or if
         *            there is more than one value for a parmeter.
         *
         * pre: This item has not been deleted
         */
        void applyChanges();

        /**
         * invalidate
         *
         * Make the data contained by this item as invalid. The next call to get...()
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        void invalidate();

        /**
         * setName
         *
         * Sets the name of the AlarmStation.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. colour should be an
         *             ta_uint32 ), or if the wrong amount of data is retrieved.
         */
        void setName(const std::string name);

        /**
         * getKey
         *
         * Returns the key for this item.
         *
         * @return The key for this item as an ta_uint32 .
         */
        ta_uint32 getKey();

        /**
         * getName
         *
         * Returns the name of this item. If this is the first time data for this item
         * has been requested, all the data is loaded from the database.
         *
         * @return The name for this item as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. ACCESSLEVEL should be an
         *             ta_uint32 ), or if the wrong ammount of data is retrieved.
         */
        std::string getName();

        /**
         * setStationColourKey
         *
         * Sets the Station RGB level of this Alarm type.
         *
         * @param ta_uint32 - The RGB colour
         * @param EColourType - Which colour type is being updated.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. colour should be an
         *             ta_uint32 ), or if the wrong amount of data is retrieved.
         */
        void setStationColourKey(ta_uint32 colour, IECSStationColourData::EColourType type);

        /**
         * getStationColourKey
         *
         * Returns the Station RGB level of this Alarm type.
         *
         * @param EColourType - Which colour to retrieve. This defaults to OPEN_ACKED which was the
         *                      behaviour when only one alarm colour was available.
         *
         * @return The Station RGB level of the Alarm type as an unsigned int.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. persist_message should be
         *            either 0 or 1), or if the wrong amount of data is retrieved.
         */
        virtual ta_uint32 getStationColourKey(IECSStationColourData::EColourType type);

        /**
         * getDateCreated
         *
         * Returns the date created for this alarm Station.
         *
         * @return The date created for this alarm Station as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. TYPEKEY should be an
         *             ta_uint32 ), or if the wrong ammount of data is retrieved, or if the
         *            the location key is invalid (and this is not a new location).
         */
        time_t getDateCreated();

        /**
         * getDateModified
         *
         * Returns the date modified for this alarm Station.
         *
         * @return The date modified for this alarm Station as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. TYPEKEY should be an
         *             ta_uint32 ), or if the wrong ammount of data is retrieved, or if the
         *            the location key is invalid (and this is not a new location).
         */
        time_t getDateModified();

        virtual ~ECSStationColourHelper();

        ECSStationColourHelper(const ta_uint32 pkey);

        /**
         * ActionGroupHelper
         *
         * This constructor creates a new object using the input data
         * which is representative of a row returned from SQL statement
         *
         * @param : const ta_uint32 row
         *          - the row of data in the data object that we should collect data from
         *          in order to construct ourselves
         * @param : TA_Base_Core::IData& data
         *          - the IData interface that should have been obtained using the
         *          getBasicQueryData function
         *
         * @exception  DatabaseException
         *             - if there is a problem establishing a connection with the database.
         *             - if an error is encountered while retrieving data.
         * @exception  DataException
         *             - if the data cannot be converted to the required format
         *             - NO_VALUE if the record cannot be found for the helper
         *             constructed with a key.
         *             - NOT_UNIQUE if the primary key returns multiple records
         */
        ECSStationColourHelper(const ta_uint32 row, TA_Base_Core::IData& data);

    private:

        // get column name list
        void getColumnNames(std::vector<std::string>& columnNames);

        /**
         * reload()
         *
         * This method reloads the data from the database. It is called when a get...
         * or set... method is called and the data state is not valid.
         *
         * pre: Either - this entity was initially loaded from the database
         *      OR     - writeEntityData() has already been called
         */
        void reload();

        /**
         * reloadUsing
         *
         * Fills out the local members with data contained in the input data object
         *
         * @param : const ta_uint32 row - the row to query in the data object
         * @param : TA_Base_Core::IData& data - the IData interface that should
         *          have been obtained using the getBasicQueryData function
         *
         * @exception  DatabaseException
         *             - if there is a problem establishing a connection with the database.
         *             - if an error is encountered while retrieving data.
         * @exception  DataException
         *             - if the data cannot be converted to the required format
         *             - NO_VALUE if the record cannot be found for the helper
         *             constructed with a key.
         *             - NOT_UNIQUE if the primary key returns multiple records
         */
        void reloadUsing(const ta_uint32 row, TA_Base_Core::IData& data);

        std::string m_name;
        ta_uint32 m_key;
        std::map<IECSStationColourData::EColourType, ta_uint32> m_ecsStationColourKey;
        time_t m_dateCreated;
        time_t m_dateModified;
        bool m_isValidData;
    };

} //close namespace TA_Base_Core

#endif // ECS_STATION_COLOUR_HELPER_H
