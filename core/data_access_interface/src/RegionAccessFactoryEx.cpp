/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/RegionAccessFactoryEx.cpp $
 * @author:  Karen Graham
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * RegionAccessFactory is a singleton that is used to retrieve Region objects either from the
 * database or newly created. All Region objects returned will adhear to the IRegion interface.
 *
 */

#include "RegionAccessFactoryEx.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/StdUtil.h"

namespace TA_Base_Core
{
    RegionAccessFactory& RegionAccessFactoryEx::instance()
    {
        return RegionAccessFactory::getInstance();
    }

    IRegionPtrList& RegionAccessFactoryEx::getAllRegions()
    {
        static auto s_all = stdutil::to_shared(instance().getAllRegions());
        return s_all;
    }

    IRegionPtr RegionAccessFactoryEx::getRegion(size_t key)
    {
        return getRegionIf([&](auto & region) { return region->getKey() == key; });
    }

    IRegionPtr RegionAccessFactoryEx::getRegionByKey(size_t key)
    {
        return getRegion(key);
    }

    IRegionPtr RegionAccessFactoryEx::getRegionIf(std::function<bool(const IRegionPtr&)> pred)
    {
        return stdutil::get_if(getAllRegions(), pred);
    }

    std::string RegionAccessFactoryEx::toName(size_t key, const std::string& defaultName)
    {
        return stdutil::no_throw_or(defaultName, [&]
        {
            auto region = getRegionByKey(key);
            return region ? region->getName() : defaultName;
        });
    }
}
