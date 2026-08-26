/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source: $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/RegionHelper.h $
 * @author Gregg Kirkpatrick
 * @version $Revision: #3 $
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by: $Author: CM $
 *
 * RegionHelper is an object that is held by all type-specific region objects, as well as by
 * the ConfigRegion. It contains all data common across regions, and manipulation
 * methods for the data. It helps avoid re-writing code for each region object.
 *
 */

#if !defined(RegionHelper_B14A02F4_8549_4115_916A_CC81775C1C32__INCLUDED_)
#define RegionHelper_B14A02F4_8549_4115_916A_CC81775C1C32__INCLUDED_

#include "core/types/src/ta_types.h"
#include <string>
#include <map>
#include <ctime>

#include "core/data_access_interface/src/IData.h"

namespace TA_Base_Core
{
    class IData;

    class RegionHelper
    {
    public:

        /**
         * Constructor
         *
         * Construct an empty ConfigRegion class ready to be populated.
         */
        RegionHelper();

        /**
         * Constructor
         *
         * Construct a Region class based around a key, this will read
         * the data from the database and throw the any DatabaseException
         * if not succesful
         *
         * @param key The key of this Region in the database
         */
        RegionHelper(const ta_uint32 key);

        /**
         * Constructor
         *
         * Construct a complete region, this will set isValidData to true
         *
         * @param key The key of this Region in the database
         * @param name The name of this Region in the database
         * @param description The description of this Region in the database
         * @param dateCreated The date and time the region was created
         * @param dateModified The date and time the region was last modified
         */
        RegionHelper(const ta_uint32 key, const std::string& name, const std::string& description,
                     const time_t dateCreated, const time_t dateModified);

        /**
         * RegionHelper
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
        RegionHelper(const ta_uint32 row, TA_Base_Core::IData& data);

        /**
         * Constructor
         *
         * Construct a RegionHelper class based around an existing region. This will
         * copy all the paramters of the existing region
         *
         * @param theRegion The region to copy
         */
        RegionHelper(const RegionHelper& theRegion);

        /**
         * Destructor
         */
        virtual ~RegionHelper();

        /**
         * isNew
         *
         * This returns whether this is a new region or not
         *
         * @return bool - True if this is a new entity
         */
        virtual bool isNew() const
        {
            return m_isNew;
        };

        /**
         * getKey
         *
         * Returns the key for this Region.
         *
         * @return The key for this Region as an ta_uint32 .
         */
        virtual ta_uint32 getKey();

        /**
         * getName
         *
         * Returns the name of this Region. If this is the first time data for this Region
         * has been requested, all the data is loaded from the database.
         *
         * @return The name for this operator as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. ACCESSLEVEL should be an
         *             ta_uint32 ), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getName();

        /**
         * getDescription
         *
         * Returns the description of this Region. If this is the first time data for this Region
         * has been requested, all the data is loaded from the database.
         *
         * @return The description for this region as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. ACCESSLEVEL should be an
         *             ta_uint32 ), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getDescription();

        /**
         * getDisplayName
         *
         * Returns the display name of this Region. If this is the first time data for this Region
         * has been requested, all the data is loaded from the database.
         *
         * @return The display name for this operator as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. ACCESSLEVEL should be an
         *             ta_uint32 ), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getDisplayName();

        /**
         * getDateCreated
         *
         * Returns the date created for this region.
         *
         * @return The date created for this region as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. TYPEKEY should be an
         *             ta_uint32 ), or if the wrong ammount of data is retrieved, or if the
         *            the region key is invalid (and this is not a new region).
         */
        time_t getDateCreated();

        /**
         * getDateModified
         *
         * Returns the date modified for this region.
         *
         * @return The date modified for this region as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. TYPEKEY should be an
         *             ta_uint32 ), or if the wrong ammount of data is retrieved, or if the
         *            the region key is invalid (and this is not a new region).
         */
        time_t getDateModified();

        /**
         * invalidate
         *
         * Make the data contained by this Region as invalid. The next call to get...()
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();

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

        // Operator = is not used so this method is hidden.
        RegionHelper& operator=(const RegionHelper&);

        ta_uint32 m_key;
        std::string m_name;
        std::string m_description;
        std::string m_displayName;
        time_t m_dateCreated;
        time_t m_dateModified;

        bool m_isNew;
        bool m_isValidData;
    };
} // closes TA_Base_Core

#endif // !defined(RegionHelper_B14A02F4_8549_4115_916A_CC81775C1C32__INCLUDED_)
