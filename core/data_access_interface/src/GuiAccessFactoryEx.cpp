/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/GuiAccessFactoryEx.cpp $
 * @author:  Karen Graham
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * GuiAccessFactory is a singleton that is used to retrieve Gui objects either from the
 * database or newly created. All Gui objects returned will adhear to the IGui interface.
 */

#include "GuiAccessFactoryEx.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/StdUtil.h"

namespace TA_Base_Core
{
    GuiAccessFactory& GuiAccessFactoryEx::instance()
    {
        return GuiAccessFactory::getInstance();
    }

    IGuiPtrList& GuiAccessFactoryEx::getAllGuis()
    {
        static auto s_all = stdutil::to_shared(instance().getAllGuis());
        return s_all;
    }

    IGuiPtr GuiAccessFactoryEx::getGui(size_t key)
    {
        return getGuiIf([&](auto gui) { return gui->getKey() == key; });
    }

    IGuiPtr GuiAccessFactoryEx::getGuiIf(std::function<bool(IGuiPtr)> pred)
    {
        return stdutil::get_if(getAllGuis(), pred);
    }
}
