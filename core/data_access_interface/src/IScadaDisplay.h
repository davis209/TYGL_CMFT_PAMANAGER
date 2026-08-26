/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/IScadaDisplay.h $
 * @author:  Karen Graham
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * This is an interface for a Scada Display from the database.
 */

#if !defined(IScadaDisplay_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IScadaDisplay_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include "core/types/src/ta_types.h"
#include <string>

#include "core/data_access_interface/src/IItem.h"

namespace TA_Base_Core
{
    class IScadaDisplay : public virtual IItem
    {
    public:

        /**
         * getPath
         *
         * Returns the path and file name of the display
         *
         * @return The path for this display as a std::string.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. ACCESSLEVEL should be an
         *             ta_uint32 ), or if the wrong amount of data is retrieved.
         */
        virtual std::string getPath() = 0;

        /**
         * getLocation
         *
         * Returns the location for this disply.
         *
         * @return The location for this display as an ta_uint32 .
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. TYPEKEY should be an
         *             ta_uint32 ), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual ta_uint32 getLocation() = 0;

        /**
         * getLocationName
         *
         * Returns the location name for this display.
         *
         * @return The location name for this display as a string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. TYPEKEY should be an
         *             ta_uint32 ), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual std::string getLocationName() = 0;

        /**
         * getDateCreated
         *
         * Returns the date created for this scada display
         *
         * @return The date created for this scada display as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. TYPEKEY should be an
         *             ta_uint32 ), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual time_t getDateCreated() = 0;

        /**
         * getDateModified
         *
         * Returns the date modified for this scada display.
         *
         * @return The date modified for this scada display as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. TYPEKEY should be an
         *             ta_uint32 ), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual time_t getDateModified() = 0;

        virtual ~IScadaDisplay() {};
    };
} //close namespace TA_Base_Core

#endif // !defined(IScadaDisplay_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
