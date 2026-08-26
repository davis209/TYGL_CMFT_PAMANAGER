/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/ConfigActionGroup.cpp $
 * @author:  Karen Graham
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * ConfigActionGroup is an implementation of IConfigActionGroup. It holds the data specific to a ActionGroup entry
 * in the database, and allows read-write access to that data.
 *
 */

#ifdef __WIN32__
    #pragma warning(disable:4786)
    #pragma warning(disable:4284)
#endif

#include "core/types/src/ta_types.h"
#include "core/data_access_interface/src/ConfigActionGroup.h"
#include "core/data_access_interface/src/ActionGroupHelper.h"

#include "core/data_access_interface/src/ActionAccessFactory.h"
#include "core/data_access_interface/src/IAction.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Core
{
    ta_int32 ConfigActionGroup::s_nextAvailableIdentifier = 0;

    const std::string ConfigActionGroup::NAME = "Name";
    const std::string ConfigActionGroup::IS_CONTROL_TYPE = "IsControl";

    const std::string ConfigActionGroup::YES = "Yes";
    const std::string ConfigActionGroup::NO = "No";

    const std::string ConfigActionGroup::ACTION_SELECTED = "Included";
    const std::string ConfigActionGroup::ACTION_UNSELECTED = "Not Included";

    ConfigActionGroup::ConfigActionGroup()
        : m_actionGroupHelper(new ActionGroupHelper()),
          m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }

    ConfigActionGroup::ConfigActionGroup(const ta_uint32 key, const std::string& name)
        : m_actionGroupHelper(new ActionGroupHelper(key, name)),
          m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }

    ConfigActionGroup::ConfigActionGroup(const ta_uint32 row, TA_Base_Core::IData& data, std::vector<ta_uint32>& actions)
        : m_actionGroupHelper(new ActionGroupHelper(row, data, actions)),
          m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }

    ConfigActionGroup::ConfigActionGroup(const ConfigActionGroup& theActionGroup)
        : m_actionGroupHelper(new ActionGroupHelper(*(theActionGroup.m_actionGroupHelper))),
          m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }

    ConfigActionGroup::~ConfigActionGroup()
    {
        try
        {
            if (m_actionGroupHelper != NULL)
            {
                delete m_actionGroupHelper;
                m_actionGroupHelper = NULL;
            }
        }
        catch (...)
        {
            LOG_EXCEPTION("Unknown", "Caught in destructor");
        }
    }

    bool ConfigActionGroup::isNew()
    {
        TA_ASSERT(m_actionGroupHelper != NULL, "The ActionGroup helper is NULL");

        return m_actionGroupHelper->isNew();
    }

    ItemChanges ConfigActionGroup::getAllItemChanges()
    {
        FUNCTION_ENTRY("getAllItemChanges");

        ItemChanges changes;

        for (ItemChanges::iterator iter = m_actionGroupChanges.begin(); iter != m_actionGroupChanges.end(); ++iter)
        {
            if ((0 == iter->first.compare(NAME)) ||
                    (0 == iter->first.compare(IS_CONTROL_TYPE)))
            {
                // These entries can just be transferred into the new map
                changes.insert(ItemChanges::value_type(iter->first, iter->second));
                continue;
            }

            // Otherwise we must convert the first value into an action name

            // First retrieve the action key from the string
            std::istringstream actionString;
            actionString.str(iter->first);

            ta_uint32 actionKey;
            actionString >> actionKey;

            // Now retrieve the name for this action
            std::string name;

            IAction* action = NULL;

            try
            {
                action = ActionAccessFactory::getInstance().getAction(actionKey);
                name = action->getName();
            }
            catch (...)
            {
                LOG_EXCEPTION("Unknown", "Could not retrieve the action name. Using unknown instead");
                name = "Unknown";
            }

            if (action != NULL)
            {
                delete action;
                action = NULL;
            }

            changes.insert(ItemChanges::value_type(name, iter->second));
        }

        FUNCTION_EXIT;
        return changes;
    }

    ta_uint32 ConfigActionGroup::getKey()
    {
        TA_ASSERT(m_actionGroupHelper != NULL, "The ActionGroup helper is NULL");

        return m_actionGroupHelper->getKey();
    }

    std::string ConfigActionGroup::getName()
    {
        TA_ASSERT(m_actionGroupHelper != NULL, "The ActionGroup helper is NULL");

        return m_actionGroupHelper->getName();
    }

    std::string ConfigActionGroup::getDisplayName()
    {
        TA_ASSERT(m_actionGroupHelper != NULL, "The ActionGroup helper is NULL");

        return m_actionGroupHelper->getDisplayName();
    }

    bool ConfigActionGroup::isControlType()
    {
        TA_ASSERT(m_actionGroupHelper != NULL, "The ActionGroup helper is NULL");

        return m_actionGroupHelper->isControlType();
    }

    bool ConfigActionGroup::isActionIncluded(ta_uint32 actionKey)
    {
        TA_ASSERT(m_actionGroupHelper != NULL, "The ActionGroup helper is NULL");

        return m_actionGroupHelper->isActionIncluded(actionKey);
    }

    time_t ConfigActionGroup::getDateCreated()
    {
        TA_ASSERT(m_actionGroupHelper != NULL, "The ActionGroup helper is NULL");

        return m_actionGroupHelper->getDateCreated();
    }

    time_t ConfigActionGroup::getDateModified()
    {
        TA_ASSERT(m_actionGroupHelper != NULL, "The ActionGroup helper is NULL");

        return m_actionGroupHelper->getDateModified();
    }

    void ConfigActionGroup::setName(const std::string& name)
    {
        FUNCTION_ENTRY("setName");

        TA_ASSERT(m_actionGroupHelper != NULL, "The ActionGroup helper is NULL");

        updateChanges(NAME, m_actionGroupHelper->getName(), name);

        m_actionGroupHelper->setName(name);

        FUNCTION_EXIT;
    }

    void ConfigActionGroup::setDisplayName(const std::string& displayName)
    {
        FUNCTION_ENTRY("displayName");

        TA_ASSERT(m_actionGroupHelper != NULL, "The ActionGroup helper is NULL");

        updateChanges(NAME, m_actionGroupHelper->getDisplayName(), displayName);

        m_actionGroupHelper->setDisplayName(displayName);

        FUNCTION_EXIT;
    }
    void ConfigActionGroup::setIsControlType(bool isControlType)
    {
        FUNCTION_ENTRY("setIsControlType");

        TA_ASSERT(m_actionGroupHelper != NULL, "The ActionGroup helper is NULL");

        std::string newValue = NO;
        std::string oldValue = NO;

        if (isControlType)
        {
            newValue = YES;
        }

        if (m_actionGroupHelper->isControlType())
        {
            oldValue = YES;
        }

        updateChanges(IS_CONTROL_TYPE, oldValue, newValue);

        m_actionGroupHelper->setIsControlType(isControlType);

        FUNCTION_EXIT;
    }

    void ConfigActionGroup::addAction(ta_uint32 actionKey)
    {
        FUNCTION_ENTRY("addAction");

        TA_ASSERT(m_actionGroupHelper != NULL, "The ActionGroup helper is NULL");

        std::string oldValue = ACTION_UNSELECTED;

        if (m_actionGroupHelper->isActionIncluded(actionKey))
        {
            oldValue = ACTION_SELECTED;
        }

        std::ostringstream action;
        action << actionKey;

        updateChanges(action.str(), oldValue, ACTION_SELECTED);

        m_actionGroupHelper->addAction(actionKey);

        FUNCTION_EXIT;
    }

    void ConfigActionGroup::removeAction(ta_uint32 actionKey)
    {
        FUNCTION_ENTRY("removeAction");

        TA_ASSERT(m_actionGroupHelper != NULL, "The ActionGroup helper is NULL");

        std::string oldValue = ACTION_UNSELECTED;

        if (m_actionGroupHelper->isActionIncluded(actionKey))
        {
            oldValue = ACTION_SELECTED;
        }

        std::ostringstream action;
        action << actionKey;

        updateChanges(action.str(), oldValue, ACTION_UNSELECTED);

        m_actionGroupHelper->removeAction(actionKey);

        FUNCTION_EXIT;
    }

    void ConfigActionGroup::invalidate()
    {
        FUNCTION_ENTRY("invalidate");

        TA_ASSERT(m_actionGroupHelper != NULL, "The ActionGroup helper is NULL");

        m_actionGroupHelper->invalidate();
        m_actionGroupChanges.clear();

        FUNCTION_EXIT;
    }

    void ConfigActionGroup::applyChanges()
    {
        FUNCTION_ENTRY("applyChanges");

        TA_ASSERT(m_actionGroupHelper != NULL, "The ActionGroup helper is NULL");
        m_actionGroupHelper->applyChanges();
        m_actionGroupChanges.clear();

        FUNCTION_EXIT;
    }

    void ConfigActionGroup::deleteThisActionGroup()
    {
        FUNCTION_ENTRY("deleteThisActionGroup");

        TA_ASSERT(m_actionGroupHelper != NULL, "The ActionGroup helper is NULL");

        m_actionGroupHelper->deleteThisActionGroup();

        FUNCTION_EXIT;
    }

    void ConfigActionGroup::updateChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
    {
        FUNCTION_ENTRY("updateChanges");

        // Search for this name in the map
        ItemChanges::iterator matching = m_actionGroupChanges.find(name);

        if (matching != m_actionGroupChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_actionGroupChanges.erase(matching);
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
            m_actionGroupChanges.insert(ItemChanges::value_type(name, values));
        }

        FUNCTION_EXIT;
    }

} // closes TA_Base_Core
