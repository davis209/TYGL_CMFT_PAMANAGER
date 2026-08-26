/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/ActionGroup.cpp $
 * @author:  Karen Graham
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * ActionGroup is an implementation of IActionGroup. It holds the data specific to an ActionGroup entry
 * in the database, and allows read-only access to that data.
 *
 */

#ifdef __WIN32__
    #pragma warning(disable:4786)
    #pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
    #pragma warning(disable:4284)
#endif

#include "core/types/src/ta_types.h"
#include "core/data_access_interface/src/ActionGroup.h"
#include "core/data_access_interface/src/ActionGroupHelper.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
    ActionGroup::ActionGroup(const ta_uint32 key, const std::string& name)
        : m_actionGroupHelper(new ActionGroupHelper(key, name))
    {
    }

    ActionGroup::ActionGroup(const ta_uint32 row, TA_Base_Core::IData& data, std::vector<ta_uint32>& actions)
        : m_actionGroupHelper(new ActionGroupHelper(row, data, actions))
    {
    }

    ActionGroup::~ActionGroup()
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

    ta_uint32 ActionGroup::getKey()
    {
        TA_ASSERT(m_actionGroupHelper != NULL, "The ActionGroup helper is NULL");

        return m_actionGroupHelper->getKey();
    }

    std::string ActionGroup::getName()
    {
        TA_ASSERT(m_actionGroupHelper != NULL, "The ActionGroup helper is NULL");

        return m_actionGroupHelper->getName();
    }

    std::string ActionGroup::getDisplayName()
    {
        TA_ASSERT(m_actionGroupHelper != NULL, "The ActionGroup helper is NULL");

        return m_actionGroupHelper->getDisplayName();
    }

    bool ActionGroup::isControlType()
    {
        TA_ASSERT(m_actionGroupHelper != NULL, "The ActionGroup helper is NULL");

        return m_actionGroupHelper->isControlType();
    }

    void ActionGroup::invalidate()
    {
        TA_ASSERT(m_actionGroupHelper != NULL, "The ActionGroup helper is NULL");

        m_actionGroupHelper->invalidate();
    }

} // closes TA_Base_Core
