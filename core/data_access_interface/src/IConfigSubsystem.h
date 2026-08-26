/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/IConfigSubsystem.h $
 * @author:  Dirk McCormick
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * Provides a writeable interface to the Subsystem database information.
 *
 */

#ifndef ICONFIG_SUBSYSTEM_H
#define ICONFIG_SUBSYSTEM_H

#include "core/types/src/ta_types.h"
#include <string>
#include "core/data_access_interface/src/ISubsystem.h"
#include "core/data_access_interface/src/IConfigItem.h"

#include "core/data_access_interface/src/ConfigChangesTypes.h"

namespace TA_Base_Core
{
    class IConfigSubsystem : public IConfigItem, public ISubsystem
    {
    public:

        virtual ~IConfigSubsystem() {};

        /**
         * deleteThisSubsystem
         *
         * Removes this subsystem from the database.
         * The calling application MUST then delete this subsystem object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem writing the data to the database.
         *
         * pre: Either - this subsystem was initially loaded from the database
         *      OR     - applyChanges() has already been called
         *      This subsystem has not been deleted
         */
        virtual void deleteThisSubsystem() = 0;

        /**
         * canDelete
         *
         * This determines if this item can be deleted or not. Some items are reserved and can
         * therefore not be deleted
         *
         * @return bool - true if this item can be deleted, false otherwise
         */
        virtual bool canDelete() = 0;

        /**
         * setExclusiveControlStatus
         *
         * Sets whether the subsystem has exclusive control.
         *
         * @param isExclusive  true if the subsystem has exclusive control,
         *                     false otherwise.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. TYPEKEY should be an
         *             ta_uint32 ), or if the wrong ammount of data is retrieved, or if the
         *            the subsystem key is invalid (and this is not a new subsystem).
         */
        virtual void setExclusiveControlStatus(bool isExclusive) = 0;

        /**
         * setLocationExclusiveControlStatus
         *
         * Sets whether the subsystem has location exclusive control.
         *
         * @param isExclusive  true if the subsystem has exclusive control,
         *                     false otherwise.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. TYPEKEY should be an
         *             ta_uint32 ), or if the wrong ammount of data is retrieved, or if the
         *            the subsystem key is invalid (and this is not a new subsystem).
         */
        virtual void setLocationExclusiveControlStatus(bool isExclusive) = 0;

		/**
		 * setPhysicalSubsystemStatus
		 *
		 * Sets whether the subsystem is physical or not.
		 *
		 * @param isExclusive  true if the subsystem is physical,
		 *                     false otherwise.
		 *
		 * @exception DatabaseException A DatabaseException will be thrown if there is a
		 *            problem establishing a connection with the database, or if an
		 *            error is encountered while retrieving data.
		 * @exception DataException A DataException will be thrown if the data cannot be
		 *            converted to the required format (e.g. TYPEKEY should be an
		 *             ta_uint32 ), or if the wrong ammount of data is retrieved, or if the
		 *            the subsystem key is invalid (and this is not a new subsystem).
		 */
		virtual void setPhysicalSubsystemStatus(bool isPhysical) = 0;
    };
} //close namespace TA_Base_Core

#endif // ICONFIG_SUBSYSTEM_H
