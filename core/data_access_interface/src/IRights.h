/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/IRights.h $
 * @author:  Karen Graham
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * IRights is an interface to a Rights object. It allows the Rights object implementation
 * to be changed (e.g. if necessary due to a database schema change) without changing code
 * that uses it.
 */

#if !defined(IRights_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IRights_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include "core/types/src/ta_types.h"
#include <string>
#include <boost/shared_ptr.hpp>
#include <vector>

namespace TA_Base_Core
{
    class IRights
    {
    public:

        virtual ~IRights() {};

        /**
         * getProfileKey
         *
         * Returns the profilekey for this rights configuration.
         *
         * @return The profile key for these rights as an ta_uint32
         */
        virtual ta_uint32 getProfileKey() = 0;

        /**
         * getSubsystemKey
         *
         * Returns the subsystem key for this rights configuration.
         *
         * @return The subsystem key for these rights as an ta_uint32
         */
        virtual ta_uint32 getSubsystemKey() = 0;

        virtual ta_uint32 getSubsystemStateKey() = 0;

        /**
         * getActionGroupKey
         *
         * Returns the ActionGroup key for this rights configuration.
         *
         * @return The ActionGroup key for these rights as an ta_uint32
         */
        virtual ta_int32 getActionGroupKey() = 0;

        /**
         * isPhysicalSubsystem
         *
         * This determines whether this particular right is for a physical or non-physical
         * subsystem
         *
         * @return bool - True if this right is for a physical subsystem. False otherwise
         */
        virtual bool isPhysicalSubsystem() = 0;
    };

    using IRightsPtr = boost::shared_ptr<IRights>;
    using IRightsPtrList = std::vector<IRightsPtr>;
} //close namespace TA_Base_Core

#endif // !defined(IRights_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
