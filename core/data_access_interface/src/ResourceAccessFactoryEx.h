/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/ResourceAccessFactoryEx.h $
 * @author:  Julian Chevalley
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * ResourceAccessFactory is a singleton used to create Resource objects
 *
 */

#pragma once
#include "ResourceAccessFactory.h"
#include <boost/shared_ptr.hpp>

namespace TA_Base_Core
{
    class IResource;
    using IResourcePtr = boost::shared_ptr<IResource>;

    struct ResourceAccessFactoryEx
    {
        static ResourceAccessFactory& instance();
        static IResourcePtr getResourceFromEntity(size_t key);

        static std::string toName(size_t key, const std::string& defaultName = "");
    };
}
