/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/ProfileAccessFactoryEx.cpp $
 * @author:  Karen Graham
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * ProfileAccessFactory is a singleton that is used to retrieve Profile objects either from the
 * database or newly created. All Profile objects returned will adhear to the IProfile interface.
 *
 */

#include "ProfileAccessFactoryEx.h"
#include "core/utilities/src/StdUtil.h"

namespace TA_Base_Core
{
    ProfileAccessFactory& ProfileAccessFactoryEx::instance()
    {
        return ProfileAccessFactory::getInstance();
    }

    IProfilePtrList& ProfileAccessFactoryEx::getAllProfiles()
    {
        static auto s_all = stdutil::to_shared(instance().getAllProfiles());
        return s_all;
    }

    IProfilePtrList& ProfileAccessFactoryEx::getAllSystemProfiles()
    {
        static IProfilePtrList s_all = getAllProfilesIf([](auto & profile) { return profile->isSystemProfile(); });
        return s_all;
    }

    IProfilePtrList& ProfileAccessFactoryEx::getAllNonSystemProfiles()
    {
        static IProfilePtrList s_all = getAllProfilesIf([](auto & profile) { return !profile->isSystemProfile(); });
        return s_all;
    }

    IProfilePtrList ProfileAccessFactoryEx::getAllProfilesIf(std::function<bool(const IProfilePtr&)> pred)
    {
        return stdutil::filter_copy(getAllProfiles(), pred);
    }

    IProfilePtr ProfileAccessFactoryEx::getProfile(size_t key)
    {
        return getProfileIf([&](auto & profile) { return profile->getKey() == key; });
    }

    IProfilePtr ProfileAccessFactoryEx::getProfile(const std::string& name)
    {
        return getProfileIf([&](auto & profile) { return profile->getName() == name; });
    }

    IProfilePtr ProfileAccessFactoryEx::getProfileIf(std::function<bool(const IProfilePtr&)> pred)
    {
        return stdutil::get_if(getAllProfiles(), pred);
    }

    std::string ProfileAccessFactoryEx::toName(size_t key, const std::string& defaultName)
    {
        return stdutil::no_throw_or(defaultName, [&]
        {
            auto profile = getProfile(key);
            return profile ? profile->getName() : defaultName;
        });
    }
}
