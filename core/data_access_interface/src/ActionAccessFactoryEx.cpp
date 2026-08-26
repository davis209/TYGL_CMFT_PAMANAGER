/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/ActionAccessFactoryEx.cpp $
 * @author:  Karen Graham
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * ActionAccessFactory is a singleton that is used to retrieve action objects either from the
 * database or newly created. All action objects returned will adhear to the IAction interface.
 * Data is primarily retrieved from the SE_ACTION table
 *
 */

#include "ActionAccessFactoryEx.h"
#include "Action.h"
#include "core/utilities/src/StdUtil.h"

namespace TA_Base_Core
{
    ActionAccessFactory& ActionAccessFactoryEx::instance()
    {
        return ActionAccessFactory::getInstance();
    }

    IActionPtrList& ActionAccessFactoryEx::getAllActions()
    {
        static auto s_all = stdutil::to_shared(instance().getAllActions());
        return s_all;
    }

    IActionPtr ActionAccessFactoryEx::getAction(size_t key)
    {
        return getActionIf([&](auto & action) { return action->getKey() == key; });
    }

    IActionPtr ActionAccessFactoryEx::getActionIf(std::function<bool(const IActionPtr&)> pred)
    {
        return stdutil::get_if(getAllActions(), pred);
    }

    std::string ActionAccessFactoryEx::toName(size_t key, const std::string& defaultName)
    {
        return stdutil::no_throw_or(defaultName, [&]
        {
            auto action = getAction(key);
            return action ? action->getName() : defaultName;
        });
    }
}
