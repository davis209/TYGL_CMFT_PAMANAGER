/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/ActionGroupAccessFactoryEx.cpp $
 * @author:  Karen Graham
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * ActionGroupAccessFactory is a singleton that is used to retrieve ActionGroup objects either from the
 * database or newly created. All ActionGroup objects returned will adhear to the IActionGroup interface.
 * Data is primarily retrieved from the SE_ACTION_GROUP table
 *
 */

#include "ActionGroupAccessFactoryEx.h"
#include "ActionGroup.h"
#include "core/utilities/src/StdUtil.h"

namespace TA_Base_Core
{
    ActionGroupAccessFactory& ActionGroupAccessFactoryEx::instance()
    {
        return ActionGroupAccessFactory::getInstance();
    }

    IActionGroupPtrList& ActionGroupAccessFactoryEx::getAllActionGroups()
    {
        static auto s_all = stdutil::to_shared(instance().getAllActionGroups());
        return s_all;
    }

    std::vector<ta_uint32>& ActionGroupAccessFactoryEx::getAllActionGroupKeys()
    {
        static auto s_all = stdutil::transformed(getAllActionGroups(), [](auto & group) { return group->getKey(); }).to_vector();
        return s_all;
    }

    IActionGroupPtr ActionGroupAccessFactoryEx::getActionGroup(size_t key)
    {
        return getActionGroupIf([&](auto group) { return group->getKey() == key; });
    }

    IActionGroupPtr ActionGroupAccessFactoryEx::getActionGroupIf(std::function<bool(IActionGroupPtr)> pred)
    {
        return stdutil::get_if(getAllActionGroups(), pred);
    }

    std::string ActionGroupAccessFactoryEx::toName(size_t key, const std::string& defaultName)
    {
        return stdutil::no_throw_or(defaultName, [&]
        {
            auto group = getActionGroup(key);
            return group ? group->getName() : defaultName;
        });
    }
}
