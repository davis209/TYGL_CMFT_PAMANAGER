/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/Location.cpp $
 * @author:  Karen Graham
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * Location is an implementation of ILocation. It holds the data specific to an Location entry
 * in the database, and allows read-only access to that data.
 *
 */

#ifdef __WIN32__
    #pragma warning(disable:4786)
    #pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include "core/types/src/ta_types.h"
#include "core/data_access_interface/src/Location.h"
#include "core/data_access_interface/src/LocationHelper.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/StdPubUtility.h"

namespace TA_Base_Core
{
    Location::Location(const ta_uint32 key)
        : m_locationHelper(new LocationHelper(key))
    {
    }

    Location::Location(const ta_uint32 key, const std::string& name, const std::string& description, const ta_uint32 orderId,
                       const std::string& displayname, const time_t dateCreated, const time_t dateModified, bool displayOnly)
        : m_locationHelper(new LocationHelper(key, name, description, orderId, displayname, dateCreated, dateModified, displayOnly))
    {
    }

    Location::Location(const ta_uint32 row, TA_Base_Core::IData& data)
        : m_locationHelper(new LocationHelper(row, data))
    {
    }

    Location::~Location()
    {
        try
        {
            if (m_locationHelper != NULL)
            {
                delete m_locationHelper;
                m_locationHelper = NULL;
            }
        }
        catch (...)
        {
            LOG_EXCEPTION("Unknown", "Caught in destructor");
        }
    }

    ta_uint32 Location::getKey()
    {
        TA_ASSERT(m_locationHelper != NULL, "The location helper is NULL");

        return m_locationHelper->getKey();
    }

    std::string Location::getName()
    {
        TA_ASSERT(m_locationHelper != NULL, "The location helper is NULL");

        return m_locationHelper->getName();
    }
    std::string Location::getDisplayName()
    {
        TA_ASSERT(m_locationHelper != NULL, "The location helper is NULL");
        return m_locationHelper->getDisplayName();
    }

    std::string Location::getDescription()
    {
        TA_ASSERT(m_locationHelper != NULL, "The location helper is NULL");

        return m_locationHelper->getDescription();
    }

    ta_uint32 Location::getOrderId()
    {
        TA_ASSERT(m_locationHelper != NULL, "The location helper is NULL");

        return m_locationHelper->getOrderId();
    }

    void Location::invalidate()
    {
        TA_ASSERT(m_locationHelper != NULL, "The location helper is NULL");

        m_locationHelper->invalidate();
    }

    std::string Location::getDefaultDisplay(ta_uint32 profileKey, int displayNumber)
    {
        TA_ASSERT(m_locationHelper != NULL, "The location helper is NULL");

        return m_locationHelper->getDefaultDisplay(profileKey, displayNumber);
    }

    bool Location::isProfileAssociated(ta_uint32 profileId)
    {
        TA_ASSERT(m_locationHelper != NULL, "The location helper is NULL");

        return m_locationHelper->isProfileAssociated(profileId);
    }
    bool Location::isDisplayOnly()
    {
        TA_ASSERT(m_locationHelper != NULL, "The location helper is NULL");

        return m_locationHelper->isDisplayOnly();
    }

#ifdef PROJECT_4669

    ILocation::ELocationType Location::getLocationType()
    {
        TA_ASSERT(m_locationHelper != NULL, "The location helper is NULL");

        try
        {
            return convertType(m_locationHelper->getTypeName());
        }
        catch (...)
        {
            LOG_ERROR("unknown location type name [key=%d][name=%s][type_name=%s]", getKey(), getName(), getTypeName());
        }

        std::string name = getName();

        return LocationAccessFactory::getInstance().getLocationType(name);
    }

#endif // #ifdef PROJECT_4669

    std::string Location::getTypeName()
    {
        TA_ASSERT(m_locationHelper != NULL, "The location helper is NULL");
        return m_locationHelper->getTypeName();
    }

    ILocation::ELocationType Location::convertType(const std::string& typeName)
    {
        if (0 == TA_Base_Core::CStdPubUtility::stricmp(typeName.c_str(), "OCC"))
        {
            return ILocation::OCC;
        }

        if (0 == TA_Base_Core::CStdPubUtility::stricmp(typeName.c_str(), "BOCC"))
        {
            return ILocation::BOCC;
        }
        else if (0 == TA_Base_Core::CStdPubUtility::stricmp(typeName.c_str(), "DEPOT"))
        {
            return ILocation::DPT;
        }
        else if (0 == TA_Base_Core::CStdPubUtility::stricmp(typeName.c_str(), "STATION"))
        {
            return ILocation::STATION;
        }
        else
        {
            throw "Bad type name";
        }
    }

    const std::string& Location::convertType(const ILocation::ELocationType locationType)
    {
        const static std::string OCC("OCC");
        const static std::string BOCC("BOCC");
        const static std::string DEPOT("DEPOT");
        const static std::string STATION("STATION");
        const static std::string UNKNOWN("");

        switch (locationType)
        {
            case ILocation::OCC:
                return OCC;

            case ILocation::DPT:
                return DEPOT;

            case  ILocation::STATION:
                return STATION;

            case ILocation::BOCC:
                return BOCC;

            default:
                TA_ASSERT(false, "Bad location type");
                return UNKNOWN;
        }
    }

} // closes TA_Base_Core
