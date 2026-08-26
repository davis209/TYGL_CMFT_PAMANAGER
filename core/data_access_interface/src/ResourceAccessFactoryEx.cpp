/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/ResourceAccessFactoryEx.cpp $
 * @author:  Julian Chevalley
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * Implements the ResourceAccessFactory class
 *
 */

#include "ResourceAccessFactoryEx.h"
#include "Resource.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactoryEx.h"
#include "core/utilities/src/StdUtil.h"
#include "core/utilities/src/CacheDecorator.h"

namespace TA_Base_Core
{
    ResourceAccessFactory& ResourceAccessFactoryEx::instance()
    {
        return ResourceAccessFactory::getInstance();
    }

    IResourcePtr ResourceAccessFactoryEx::getResourceFromEntity(size_t key)
    {
        static CacheDecorator<IResourcePtr, size_t> s_func([](auto && ... args)
        {
            return stdutil::to_shared(instance().getResourceFromEntity(std::forward<decltype(args)>(args)...));
        });

        return s_func(key);
    }

    std::string ResourceAccessFactoryEx::toName(size_t key, const std::string& defaultName)
    {
        return stdutil::no_throw_or(defaultName, [&]
        {
            return EntityAccessFactoryEx::toName(getResourceFromEntity(key)->getKey(), defaultName);
        });
    }
}
