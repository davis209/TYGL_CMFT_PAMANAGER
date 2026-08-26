/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/ConfigSubsystem.cpp $
 * @author:  Dirk McCormick
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * This class implements the IConfigSubsystem interface.
 *
 */

#ifdef __WIN32__
    #pragma warning(disable: 4786)
#endif

#include "core/types/src/ta_types.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/src/ConfigSubsystem.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    ta_int32 ConfigSubsystem::s_nextAvailableIdentifier = 0;

    const std::string ConfigSubsystem::NAME      = "Name";
    const std::string ConfigSubsystem::DISPLAY_NAME      = "Display_Name";
    const std::string ConfigSubsystem::EXCLUSIVE = "Exclusive";
    const std::string ConfigSubsystem::LOCATION_EXCLUSIVE = "Location_Exclusive";
	const std::string ConfigSubsystem::PHYSICAL_SUBSYSTEM = "Physical_Subsystem";

    ConfigSubsystem::~ConfigSubsystem()
    {
        try
        {
            if (m_helper != NULL)
            {
                delete m_helper;
                m_helper = NULL;
            }
        }
        catch (...)
        {
            LOG_EXCEPTION("Unknown", "Caught in destructor");
        }
    }

    ConfigSubsystem::ConfigSubsystem()
        :
        m_helper(new SubsystemHelper()),
        m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }

    ConfigSubsystem::ConfigSubsystem(const ta_uint32 key)
        :
        m_helper(new SubsystemHelper(key)),
        m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }

    ConfigSubsystem::ConfigSubsystem(const ta_uint32 row, TA_Base_Core::IData& data)
        :
        m_helper(new SubsystemHelper(row, data)),
        m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }

    ConfigSubsystem::ConfigSubsystem(const ConfigSubsystem& theSubsystem)
        :
        m_helper(new SubsystemHelper(*(theSubsystem.m_helper))),
        m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }

    bool ConfigSubsystem::canDelete()
    {
        return m_helper->canDelete();
    }

    ta_uint32 ConfigSubsystem::getKey()
    {
        TA_ASSERT(m_helper != NULL, "The subsystem helper is NULL");

        return m_helper->getKey();
    }

    ta_uint32 ConfigSubsystem::getSystemKey()
    {
        TA_ASSERT(m_helper != NULL, "The subsystem helper is NULL");

        return m_helper->getSystemKey();
    }

    std::string ConfigSubsystem::getName()
    {
        TA_ASSERT(m_helper != NULL, "The subsystem helper is NULL");

        return m_helper->getName();
    }

    std::string ConfigSubsystem::getDisplayName()
    {
        TA_ASSERT(m_helper != NULL, "The subsystem helper is NULL");

        return m_helper->getDisplayName();
    }

    bool ConfigSubsystem::isPhysical()
    {
        TA_ASSERT(m_helper != NULL, "The subsystem helper is NULL");

        return m_helper->isPhysical();
    }

    bool ConfigSubsystem::isExclusive()
    {
        TA_ASSERT(m_helper != NULL, "The subsystem helper is NULL");

        return m_helper->isExclusive();
    }

    bool ConfigSubsystem::isLocationExclusive()
    {
        TA_ASSERT(m_helper != NULL, "The subsystem helper is NULL");

        return m_helper->isLocationExclusive();
    }

    time_t ConfigSubsystem::getDateCreated()
    {
        TA_ASSERT(m_helper != NULL, "The subsystem helper is NULL");

        return m_helper->getDateCreated();
    }

    time_t ConfigSubsystem::getDateModified()
    {
        TA_ASSERT(m_helper != NULL, "The subsystem helper is NULL");

        return m_helper->getDateModified();
    }

    ta_uint32 ConfigSubsystem::getUniqueIdentifier()
    {
        return m_uniqueIdentifier;
    }

    ItemChanges ConfigSubsystem::getAllItemChanges()
    {
        return m_subsystemChanges;
    }

    bool ConfigSubsystem::hasChanged()
    {
        // If the subsystem changes is empty then nothing has changed
        return !m_subsystemChanges.empty();
    }

    bool ConfigSubsystem::isNew()
    {
        TA_ASSERT(m_helper != NULL, "The subsystem helper is NULL");

        return m_helper->isNew();
    }

    void ConfigSubsystem::setName(const std::string& name)
    {
        TA_ASSERT(m_helper != NULL, "The subsystem helper is NULL");

        updateSubsystemChanges(NAME, m_helper->getName(), name);

        m_helper->setName(name);
    }

    void ConfigSubsystem::setDisplayName(const std::string& name)
    {
        TA_ASSERT(m_helper != NULL, "The subsystem helper is NULL");

        updateSubsystemChanges(DISPLAY_NAME, m_helper->getDisplayName(), name);

        m_helper->setDisplayName(name);
    }

    void ConfigSubsystem::setExclusiveControlStatus(bool isExclusive)
    {
        TA_ASSERT(m_helper != NULL, "The subsystem helper is NULL");

        std::string oldExclusive = getBoolAsString(m_helper->isExclusive());
        std::string newExclusive = getBoolAsString(isExclusive);

        updateSubsystemChanges(EXCLUSIVE, oldExclusive, newExclusive);

        m_helper->setExclusiveControlStatus(isExclusive);
    }

    void ConfigSubsystem::setLocationExclusiveControlStatus(bool isExclusive)
    {
        TA_ASSERT(m_helper != NULL, "The subsystem helper is NULL");

        std::string oldExclusive = getBoolAsString(m_helper->isLocationExclusive());
        std::string newExclusive = getBoolAsString(isExclusive);

        updateSubsystemChanges(LOCATION_EXCLUSIVE, oldExclusive, newExclusive);

        m_helper->setLocationExclusiveControlStatus(isExclusive);
    }

	void ConfigSubsystem::setPhysicalSubsystemStatus(bool isPhysical)
	{
		TA_ASSERT(m_helper != NULL, "The subsystem helper is NULL");

		std::string oldPhysical = getBoolAsString(m_helper->isPhysical());
		std::string newPhysical = getBoolAsString(isPhysical);

		updateSubsystemChanges(PHYSICAL_SUBSYSTEM, oldPhysical, newPhysical);

		m_helper->setPhysicalSubsystemStatus(isPhysical);
	}

    void ConfigSubsystem::applyChanges()
    {
        TA_ASSERT(m_helper != NULL, "The subsystem helper is NULL");

        m_helper->applyChanges();

        m_subsystemChanges.clear();
    }

    void ConfigSubsystem::deleteThisSubsystem()
    {
        TA_ASSERT(m_helper != NULL, "The subsystem helper is NULL");

        m_helper->deleteThisSubsystem();
    }

    void ConfigSubsystem::invalidate()
    {
        TA_ASSERT(m_helper != NULL, "The subsystem helper is NULL");

        m_helper->invalidate();
        m_subsystemChanges.clear();
    }

    void ConfigSubsystem::updateSubsystemChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
    {
        FUNCTION_ENTRY("updateSubsystemChanges");

        // Search for this name in the map
        ItemChanges::iterator matching = m_subsystemChanges.find(name);

        if (matching != m_subsystemChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_subsystemChanges.erase(matching);
            }
            else
            {
                //Update the stored new value.
                matching->second.newValue = newValue;
            }

            FUNCTION_EXIT;
            return;
        }

        // The first time we come in we should get to here and we set the old value to be the original value of the attribute
        if (oldValue != newValue)
        {
            // If the old and new values are different then we can add this item to the map
            Values values;
            values.newValue = newValue;
            values.oldValue = oldValue;
            m_subsystemChanges.insert(ItemChanges::value_type(name, values));
        }

        FUNCTION_EXIT;
    }

    std::string ConfigSubsystem::getBoolAsString(bool theBool)
    {
        //                 true  false
        return theBool ? "1" : "0";
    }

    bool ConfigSubsystem::getBoolFromString(const std::string& theString)
    {
        // "1" is true, anmything else is false
        return ((theString == "1") ? true : false);
    }

} //close namespace TA_Base_Core
