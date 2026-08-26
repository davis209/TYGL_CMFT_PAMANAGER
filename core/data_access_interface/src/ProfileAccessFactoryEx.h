/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/ProfileAccessFactoryEx.h $
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

#pragma once
#include "IProfile.h"
#include "ProfileAccessFactory.h"

namespace TA_Base_Core
{
    struct ProfileAccessFactoryEx
    {
        static ProfileAccessFactory& instance();
        static IProfilePtrList& getAllProfiles();
        static IProfilePtrList& getAllSystemProfiles();
        static IProfilePtrList& getAllNonSystemProfiles();
        static IProfilePtrList getAllProfilesIf(std::function<bool(const IProfilePtr&)> pred);
        static IProfilePtr getProfile(size_t key);
        static IProfilePtr getProfile(const std::string& name);
        static IProfilePtr getProfileIf(std::function<bool(const IProfilePtr&)> pred);
        static std::string toName(size_t key, const std::string& defaultName = "");

#if 0
        ta_uint32 getDeletedRawProfileType(ta_uint32 key);
        std::string getDeletedProfileName(ta_uint32 key);
        ta_uint32 getDeletedProfileGroup(ta_uint32 key);
        std::string getDeletedProfileGroupAsString(ta_uint32 key);
        bool getDeletedProfileIsSystemProfile(ta_uint32 key);

        std::vector<IProfile*> getProfilesByOperator(ta_uint32 key, const bool readWrite = false);
        std::vector<IProfile*> getProfilesByOperator(const std::string& name, const bool readWrite = false);
        std::vector<IProfile*> getProfilesByLocation(ta_uint32 locationId, const bool readWrite = false);
        std::vector<IProfile*> getNonSystemProfilesByOperator(ta_uint32 key, const bool readWrite = false);
        std::vector<IProfile*> getNonSystemProfilesByOperator(const std::string& name, const bool readWrite = false);
        std::vector<IProfile*> getAllProfiles(const bool readWrite = false);
        std::vector<IProfile*> getAllSystemProfiles(const bool readWrite = false);
        std::vector<IProfile*> getAllNonSystemProfiles(const bool readWrite = false);
        std::vector<std::string> getAllDistinctParameterNames();
        IProfile* getTransActiveSuperProfile();
#endif
    };
}

