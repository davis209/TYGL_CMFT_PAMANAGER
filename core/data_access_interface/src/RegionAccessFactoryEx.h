/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/RegionAccessFactoryEx.h $
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

#pragma once
#include "RegionAccessFactory.h"

namespace TA_Base_Core
{
    struct RegionAccessFactoryEx
    {
        static RegionAccessFactory& instance();
        static IRegionPtrList& getAllRegions();
        static IRegionPtr getRegionByKey(size_t key);
        static IRegionPtr getRegion(size_t key);
        static IRegionPtr getRegionIf(std::function<bool(const IRegionPtr&)> pred);
        static std::string toName(size_t key, const std::string& defaultName = "");
#if 0
        static std::vector<IRegion*> getRegionsByOperator(const ta_uint32 operatorKey);
        static std::vector<IRegion*> getRegionsByLocation(const ta_uint32 locationKey);
#endif
    };
}
