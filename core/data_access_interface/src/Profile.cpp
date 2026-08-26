/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/Profile.cpp $
 * @author:  Karen Graham
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * Profile is an implementation of IProfile. It holds the data specific to an profile entry
 * in the database, and allows read-only access to that data.
 *
 */

#ifdef __WIN32__
    #pragma warning(disable: 4786)
#endif

#include "core/types/src/ta_types.h"
#include "core/data_access_interface/src/Profile.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Core
{
    Profile::Profile(const ta_uint32 key)
        : m_profileHelper(new ProfileHelper(key))
    {
    }

    Profile::Profile(const ta_uint32 row, TA_Base_Core::IData& data)
        : m_profileHelper(new ProfileHelper(row, data))
    {
    }

    Profile::~Profile()
    {
        if (m_profileHelper != NULL)
        {
            delete m_profileHelper;
            m_profileHelper = NULL;
        }
    }

    ta_uint32 Profile::getKey()
    {
        TA_ASSERT(m_profileHelper != NULL, "The ProfileHelper was NULL");

        return m_profileHelper->getKey();
    }

    std::string Profile::getName()
    {
        TA_ASSERT(m_profileHelper != NULL, "The ProfileHelper was NULL");

        return m_profileHelper->getName();
    }

    std::string Profile::getDisplayName()
    {
        TA_ASSERT(m_profileHelper != NULL, "The ProfileHelper was NULL");

        return m_profileHelper->getDisplayName();
    }

    bool Profile::isSystemProfile()
    {
        TA_ASSERT(m_profileHelper != NULL, "getName() has been called after profile was deleted");

        return m_profileHelper->isSystemProfile();
    }

    std::string Profile::getDefaultDisplay(ta_uint32 locationKey, int displayNumber)
    {
        // Check to see if the Profile Type is LOCAL
        TA_ASSERT(m_profileHelper != NULL, "The ProfileHelper was NULL");

        return m_profileHelper->getDefaultDisplay(locationKey, displayNumber);
    }

    std::string Profile::getParameterValue(ta_uint32 locationKey, const std::string& parameterName)
    {
        // Check to see if the Profile Type is LOCAL
        TA_ASSERT(m_profileHelper != NULL, "The ProfileHelper was NULL");

        return m_profileHelper->getParameterValue(locationKey, parameterName);
    }

    std::string Profile::getParameterActualValue(ta_uint32 locationKey, const std::string& parameterName)
    {
        // Check to see if the Profile Type is LOCAL
        TA_ASSERT(m_profileHelper != NULL, "The ProfileHelper was NULL");

        return m_profileHelper->getParameterActualValue(locationKey, parameterName);
    }

    bool Profile::requiresLocationToBeSelected()
    {
        // Check to see if the Profile Type is LOCAL
        TA_ASSERT(m_profileHelper != NULL, "The ProfileHelper was NULL");

        return m_profileHelper->isLocalProfile();
    }

    void Profile::invalidate()
    {
        TA_ASSERT(m_profileHelper != NULL, "The ProfileHelper was NULL");

        m_profileHelper->invalidate();
    }

    std::vector<ILocation*> Profile::getAssociatedLocations()
    {
        TA_ASSERT(m_profileHelper != NULL, "The ProfileHelper was NULL");

        return m_profileHelper->getAssociatedLocations();
    }

    std::vector<ta_uint32> Profile::getAssociatedLocationKeys()
    {
        TA_ASSERT(m_profileHelper != NULL, "The ProfileHelper was NULL");

        return m_profileHelper->getAssociatedLocationKeys();
    }

    std::string Profile::getActionGroupAsStringForResource(ta_uint32 resourceKey)
    {
        TA_ASSERT(m_profileHelper != NULL, "The ProfileHelper was NULL");

        return m_profileHelper->getActionGroupAsStringForResource(resourceKey);
    }

    std::string Profile::getActionGroupAsStringForSubsystem(ta_uint32 subsystemKey,
                                                            ta_uint32 subsystemStateKey)
    {
        TA_ASSERT(m_profileHelper != NULL, "The ProfileHelper was NULL");

        return m_profileHelper->getActionGroupAsStringForSubsystem(subsystemKey, subsystemStateKey);
    }

    bool Profile::isExclusive()
    {
        TA_ASSERT(m_profileHelper != NULL, "The ProfileHelper was NULL");

        return (!(m_profileHelper->isLocalProfile()));
    }

    ta_uint32 Profile::getType()
    {
        TA_ASSERT(m_profileHelper != NULL, "The ProfileHelper was NULL");

        return m_profileHelper->getType();
    }

    std::string Profile::getTypeAsString()
    {
        TA_ASSERT(m_profileHelper != NULL, "The ProfileHelper was NULL");

        return m_profileHelper->getTypeAsString();
    }

    bool Profile::getActionGroup(ta_uint32 subsystem, ta_uint32 subsystemState,
                                 ta_uint32& actionGroup, bool& isControl)
    {
        TA_ASSERT(m_profileHelper != NULL, "The ProfileHelper was NULL");

        return (m_profileHelper->getActionGroup(subsystem, subsystemState, actionGroup, isControl));
    }

    ta_uint32 Profile::getAccessControlGroup()
    {
        TA_ASSERT(m_profileHelper != NULL, "The ProfileHelper was NULL");

        return (m_profileHelper->getAccessControlGroup());
    }

    std::string Profile::getItsiGroupName()
    {
        TA_ASSERT(m_profileHelper != NULL, "The ProfileHelper was NULL");

        return (m_profileHelper->getItsiGroupName());
    }

    std::string Profile::getAccessControlGroupAsString()
    {
        TA_ASSERT(m_profileHelper != NULL, "The ProfileHelper was NULL");

        return (m_profileHelper->getAccessControlGroupAsString());
    }

    std::vector<ta_uint32> Profile::getAccessibleSubsystemKeys()
    {
        TA_ASSERT(m_profileHelper != NULL, "The ProfileHelper was NULL");

        return (m_profileHelper->getAccessibleSubsystemKeys());
    }

    bool Profile::isLocationAssociated(ta_uint32 locationId)
    {
        TA_ASSERT(m_profileHelper != NULL, "The ProfileHelper is NULL");

        return m_profileHelper->isLocationAssociated(locationId);
    }

    void Profile::setAccessControlGroup(ta_uint32 accessControlGroupKey)
    {
        FUNCTION_ENTRY("setAccessControlGroup");

        TA_ASSERT(m_profileHelper != NULL, "applyChanges() has been called after profile was deleted");

        m_profileHelper->setAccessControlGroup(accessControlGroupKey);

        FUNCTION_EXIT;
    }

    void Profile::setItsiGroup(const std::string& itsiGroupName)
    {
        FUNCTION_ENTRY("setItsiGroup");

        TA_ASSERT(m_profileHelper != NULL, "applyChanges() has been called after profile was deleted");

        m_profileHelper->setItsiGroup(itsiGroupName);

        FUNCTION_EXIT;
    }

} // closes TA_Base_Core
