/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/ISubsystem.h $
 * @author:  Karen Graham
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * ISubsystem is an interface to a Subsystem object. It allows the Subsystem object implementation
 * to be chagned (e.g. if necessary due to a database schema change) without changing code
 * that uses it.
 *
 */

#pragma once
#include "core/types/src/ta_types.h"
#include <string>

#include "core/data_access_interface/src/IItem.h"

namespace TA_Base_Core
{
    class ISubsystem : public IItem
    {
    public:

        virtual ~ISubsystem() {};

        /**
         * getDisplayName
         *
         * Returns the getDisplayName of this subsystem.
         *
         * @return the getDisplayName of this subsystem.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. TYPEKEY should be an
         *             ta_uint32 ), or if the wrong ammount of data is retrieved, or if the
         *            the subsystem key is invalid (and this is not a new subsystem).
         */

        virtual std::string getDisplayName() = 0;
        /**
         * isPhysical
         *
         * This returns whether this is a physical subsystem or not.
         *
         * @return bool - True if this is a physical subsystem, false otherwise
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. ACCESSLEVEL should be an
         *             ta_uint32 ), or if the wrong ammount of data is retrieved.
         */
        virtual bool isPhysical() = 0;

        /**
         * isExclusive
         *
         * This returns whether this is an exclusive subsystem or not.
         *
         * @return bool - True if this is an exclusive subsystem, false otherwise
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. ACCESSLEVEL should be an
         *             ta_uint32 ), or if the wrong ammount of data is retrieved.
         */
        virtual bool isExclusive() = 0;

        /**
         * isLocationExclusive
         *
         * This returns whether this is an location exclusive subsystem or not.
         *
         * @return bool - True if this is an location exclusive subsystem, false otherwise
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. ACCESSLEVEL should be an
         *             ta_uint32 ), or if the wrong ammount of data is retrieved.
         */
        virtual bool isLocationExclusive() = 0;

        /**
         * getDateCreated
         *
         * Returns the date created for this subsystem.
         *
         * @return The date created for this subsystem as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. TYPEKEY should be an
         *             ta_uint32 ), or if the wrong ammount of data is retrieved, or if the
         *            the subsystem key is invalid (and this is not a new subsystem).
         */
        virtual time_t getDateCreated() = 0;

        /**
         * getDateModified
         *
         * Returns the date modified for this subsystem.
         *
         * @return The date modified for this subsystem as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. TYPEKEY should be an
         *             ta_uint32 ), or if the wrong ammount of data is retrieved, or if the
         *            the subsystem key is invalid (and this is not a new subsystem).
         */
        virtual time_t getDateModified() = 0;

        /**
         * getSystemKey
         *
         * Returns the key for this system.
         *
         * @return The key for this system as an ta_uint32 .
         */
        virtual ta_uint32 getSystemKey() = 0;
    };

    using ISubsystemPtr = boost::shared_ptr<ISubsystem>;
    using ISubsystemPtrList = std::vector<ISubsystemPtr>;
}
