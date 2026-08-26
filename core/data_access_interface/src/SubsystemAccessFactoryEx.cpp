/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/SubsystemAccessFactoryEx.cpp $
 * @author:  Karen Graham
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * SubsystemAccessFactory is a singleton that is used to retrieve Subsystem objects either from the
 * database or newly created. All Subsystem objects returned will adhear to the ISubsystem interface.
 */

#pragma once
#include "SubsystemAccessFactoryEx.h"
#include "ISubsystem.h"
#include "core/utilities/src/StdUtil.h"

namespace TA_Base_Core
{
    SubsystemAccessFactory& SubsystemAccessFactoryEx::instance()
    {
        return SubsystemAccessFactory::getInstance();
    }

    ISubsystemPtrList& SubsystemAccessFactoryEx::getAllSubsystems()
    {
        static auto s_all = stdutil::to_shared(instance().getAllSubsystems());
        return s_all;
    }

    ISubsystemPtrList& SubsystemAccessFactoryEx::getAllPhysicalSubsystems()
    {
        static auto s_all = getSubsystemListIf([](auto & subsystem) { return subsystem->isPhysical(); });
        return s_all;
    }

    ISubsystemPtrList SubsystemAccessFactoryEx::getSubsystemListIf(std::function<bool(const ISubsystemPtr&)> pred)
    {
        return stdutil::filter_copy(getAllSubsystems(), pred);
    }

    ISubsystemPtr SubsystemAccessFactoryEx::getSubsystemByKey(size_t key)
    {
        return getSubsystem(key);
    }

    ISubsystemPtr SubsystemAccessFactoryEx::getSubsystem(size_t key)
    {
        return getSubsystemIf([&](auto & subsystem) { return subsystem->getKey() == key; });
    }

    ISubsystemPtr SubsystemAccessFactoryEx::getSubsystemIf(std::function<bool(const ISubsystemPtr&)> pred)
    {
        return stdutil::get_if(getAllSubsystems(), pred);
    }

    std::string SubsystemAccessFactoryEx::toName(size_t key, const std::string& defaultName)
    {
        return stdutil::no_throw_or(defaultName, [&]
        {
            auto subsystem = getSubsystemByKey(key);
            return subsystem ? subsystem->getName() : defaultName;
        });
    }
}
