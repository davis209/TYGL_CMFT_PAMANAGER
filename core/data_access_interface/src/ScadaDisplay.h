/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/ScadaDisplay.h $
 * @author:  Karen Graham
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * ScadaDisplay is an implementation of IScadaDisplay. It holds the data specific to a scada display
 * entry in the database and allows read-only access to that data.
 */

#if !defined(ScadaDisplay_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define ScadaDisplay_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

#include "core/types/src/ta_types.h"
#include <string>

#include "core/data_access_interface/src/IScadaDisplay.h"

namespace TA_Base_Core
{
    class IData;
    class ScadaDisplayHelper;

    class ScadaDisplay : public IScadaDisplay
    {
    public:

        /**
         * ScadaDisplay (constructor)
         *
         * Construct an ScadaDisplay class based around a key.
         *
         * @param key The key of this ScadaDisplay in the database
         */
        ScadaDisplay(const ta_uint32 key);

        /**
         * Constructor
         *
         * Construct a ScadaDisplay class based around dataset
         */
        ScadaDisplay(const ta_uint32 row, TA_Base_Core::IData& data);

        virtual ~ScadaDisplay();

        /**
         * getKey
         *
         * Returns the key for this ScadaDisplay.
         *
         * @return The key for this display as an ta_uint32 .
         */
        ta_uint32 getKey();

        /**
         * getName
         *
         * Returns the name of this display. If this is the first time data for this display
         * has been requested, all the data is loaded from the database.
         *
         * @return The name for this display as a std::string
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
        virtual std::string getPath();

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
        virtual ta_uint32 getLocation();

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
        virtual std::string getLocationName();

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
        virtual time_t getDateCreated();

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
        virtual time_t getDateModified();

        /**
         * invalidate
         *
         * Make the data contained by this display as invalid. The next call to get...()
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();

    private:

        // Assignment operator not used so it is made private
        ScadaDisplay& operator=(const ScadaDisplay&);

        // Copy constructor should not be used and has therefore been made private. If a copy
        // constructor is required care should be taken. The copy constructor fo the ScadaDisplayHelper
        // has only been written for ConfigScadaDisplay objects and will not copy the display helper
        // for a read-only object (ie this one).
        ScadaDisplay(const ScadaDisplay& theDisplay);

        TA_Base_Core::ScadaDisplayHelper* m_displayHelper;
    };
} // closes TA_Base_Core

#endif // !defined(ScadaDisplay_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
