/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/ActionAccessFactoryEx.h $
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

#pragma once
#include "ActionAccessFactory.h"
#include <functional>

namespace TA_Base_Core
{
    class IAction;
    using IActionPtr = boost::shared_ptr<IAction>;
    using IActionPtrList = std::vector<IActionPtr>;

    struct ActionAccessFactoryEx
    {
        static ActionAccessFactory& instance();

        static IActionPtrList& getAllActions();
        static IActionPtr getAction(size_t key);
        static IActionPtr getActionIf(std::function<bool(const IActionPtr&)> pred);
        static std::string toName(size_t key, const std::string& defaultName = "");

#if 0
        IAction* getAction(const ta_uint32 actionKey);
        std::vector<IAction*> getAllActions();
        void getActionGroupMap(const std::set<ta_uint32>& groupIdList, ActionGroupMap& actionGroupMap);
        void getNonPhysicalSubsystemActionGroup(const std::vector<ta_uint32>& profileList, std::multimap<ta_uint32, ta_uint32>& groupIdList);
#endif
    };
}
