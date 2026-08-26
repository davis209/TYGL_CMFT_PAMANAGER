/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/NullLocation.h $
 * @author:  Karen Graham
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * ILocation is an interface to a Location object. It allows the Location object implementation
 * to be chagned (e.g. if necessary due to a database schema change) without changing code
 * that uses it.
 *
 * NOTE: This is temporary and must be changed later.
 */

#pragma once
#include "core/data_access_interface/src/ILocation.h"

namespace TA_Base_Core
{
    struct NullLocation : ILocation
    {
        NullLocation(ta_uint32 key = 0, const std::string& name = "")
            : m_key(key),
              m_name(name),
              m_orderId(0),
              m_locationType(ILocation::STATION),
              m_isProfileAssociated(false),
              m_isDidplayOnly(false)
        {
        }

        virtual ta_uint32 getKey() { return m_key; }
        virtual std::string getName() { return m_name; }
        virtual void invalidate() {}

        virtual std::string getDescription() { return m_description; }
        virtual ta_uint32 getOrderId() { return m_orderId; }
        virtual std::string getDisplayName() { return m_displayName; }
        virtual std::string getDefaultDisplay(ta_uint32 profileKey, int displayNumber) { return m_defaultDisplay; }
        virtual ELocationType getLocationType() { return m_locationType; }
        virtual bool isProfileAssociated(ta_uint32 profileId) { return m_isProfileAssociated; }
        virtual bool isDisplayOnly() { return m_isDidplayOnly; }
        virtual std::string getTypeName() { return m_typeName; }

        ta_uint32 m_key;
        std::string m_name;
        std::string m_description;
        ta_uint32 m_orderId;
        std::string m_displayName;
        std::string m_defaultDisplay;
        ELocationType m_locationType;
        std::string m_typeName;
        bool m_isProfileAssociated;
        bool m_isDidplayOnly;
    };

    using NullLocationPtr = boost::shared_ptr<NullLocation>;
}
