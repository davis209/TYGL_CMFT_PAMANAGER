/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/IProfileGroup.h $
 * @author:  Anita Lee
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * IProfileGroup is an interface to a ProfileGroup object. It allows the ProfileGroup
 * object implementation to be changed (e.g. if necessary due to a database schema change)
 * without changing code that uses it.
 */

#if !defined(IProfileGroup_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IProfileGroup_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include "core/types/src/ta_types.h"
#include <string>
#include <vector>

#include "core/data_access_interface/src/IItem.h"

namespace TA_Base_Core
{
    class IProfileGroup : public virtual IItem
    {
    public:

        virtual ~IProfileGroup() {};

        /**
         * getKey
         *
         * Returns the profile group key for this profile group.
         *
         * @return The profile group key for this profile group as an ta_uint32
         */
        virtual ta_uint32 getKey() = 0;

        /**
         * getName
         *
         * Returns the profile group name for this profile group.
         *
         * @return The profile group name as a string
         */
        virtual std::string getName() = 0;

        /**
         * getType
         *
         * Returns the profile group type name for this profile group.
         * Currently it is either PROFILE or ITSI
         *
         * @return The profile group type name as a string
         */
        virtual std::string getType() = 0;

        /**
         * isGroupIsUsedInProfile
         *
         * Checks whether this profile group is used in profile table from the database.
         * The calling application MUST NOT delete this object, as it will affect the
         * constraint / relationship in the profile table.
         *
         * @param key is the profile group primary key for check in profile table
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem writing the data to the database.
         *
         * pre: Either - this profile was initially loaded from the database
         *      OR     - apply() has already been called
         *      This profile group is to be deleted
         */
        virtual bool isGroupUsedInProfile(ta_uint32 key) = 0;

        /**
         * invalidate
         *
         * Mark the data contained by this subsystemState as invalid. The next call to get...()
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate() = 0;
    };
} //close namespace TA_Base_Core

#endif // !defined(IProfileGroup_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
