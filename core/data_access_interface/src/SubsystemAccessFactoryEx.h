/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/SubsystemAccessFactoryEx.h $
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
#include "SubsystemAccessFactory.h"
#include <functional>

namespace TA_Base_Core
{
    class ISubsystem;
    using ISubsystemPtr = boost::shared_ptr<ISubsystem>;
    using ISubsystemPtrList = std::vector<ISubsystemPtr>;

    struct SubsystemAccessFactoryEx
    {
        static SubsystemAccessFactory& instance();
        static ISubsystemPtrList& getAllSubsystems();
        static ISubsystemPtrList& getAllPhysicalSubsystems();
        static ISubsystemPtr getSubsystemByKey(size_t key);
        static ISubsystemPtr getSubsystem(size_t key);
        static ISubsystemPtr getSubsystemIf(std::function<bool(const ISubsystemPtr&)> pred);
        static ISubsystemPtrList getSubsystemListIf(std::function<bool(const ISubsystemPtr&)> pred);

        static std::string toName(size_t key, const std::string& defaultName = "");

#if 0
        std::vector<ISubsystem*> getAllPhysicalSubsystems(const bool readWrite = false);
        ISubsystem* getSubsystemByKey(const ta_uint32 key, const bool readWrite = false);
        std::vector<ISubsystem*> getSubsystemBySystemKeys(std::vector<ta_uint32> keys, const bool readWrite = false);
        std::vector<ISubsystem*> getSubsystemsByRegion(ta_uint32 regionKey, const bool readWrite = false);
        IConfigSubsystem* createSubsystem();
        IConfigSubsystem* copySubsystem(const IConfigSubsystem* subsystemToCopy);
#endif
    };
}
