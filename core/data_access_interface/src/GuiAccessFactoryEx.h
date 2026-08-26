/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/GuiAccessFactoryEx.h $
 * @author:  Karen Graham
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * GuiAccessFactory is a singleton that is used to retrieve Gui objects either from the
 * database or newly created. All Gui objects returned will adhear to the IGui interface.
 */

#pragma once
#include "IGui.h"
#include "GuiAccessFactory.h"
#include <functional>

namespace TA_Base_Core
{
    struct GuiAccessFactoryEx
    {
        static GuiAccessFactory& instance();
        static IGuiPtrList& getAllGuis();
        static IGuiPtr getGui(size_t key);
        static IGuiPtr getGuiIf(std::function<bool(IGuiPtr)> pred);

#if 0
        IGui* getGui(const ta_uint32 key, const bool readWrite = false);
        IConfigGui* createGui(const std::string& type);
        IConfigGui* copyGui(const IConfigGui* guiToCopy);
#endif
    };
}
