/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/ActionGroupAccessFactoryEx.h $
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

#pragma once
#include "ActionGroupAccessFactory.h"
#include <functional>

namespace TA_Base_Core
{
    class IActionGroup;
    class IConfigActionGroup;
    using IActionGroupPtr = boost::shared_ptr<IActionGroup>;
    using IActionGroupPtrList = std::vector<IActionGroupPtr>;

    struct ActionGroupAccessFactoryEx
    {
        static ActionGroupAccessFactory& instance();

        static IActionGroupPtrList& getAllActionGroups();
        static std::vector<ta_uint32>& getAllActionGroupKeys();
        static IActionGroupPtr getActionGroup(size_t key);
        static IActionGroupPtr getActionGroupIf(std::function<bool(IActionGroupPtr)> pred);
        static std::string toName(size_t key, const std::string& defaultName = "");

#if 0
        IConfigActionGroup* createActionGroup();
        void getAllActionGroupKeys(std::vector<ta_uint32>& groupKeys);
        std::string getActionGroupName(ta_uint32 key);
        void getAllActionGroupNames(std::map<ta_uint32, std::string>& groupNames);
        IConfigActionGroup* copyActionGroup(const IConfigActionGroup* actionGroupToCopy);
#endif
    };
}
