/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/LocationAccessFactoryEx.h $
 * @author:  Karen Graham
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * LocationAccessFactory is a singleton that is used to retrieve Location objects either from the
 * database or newly created. All Location objects returned will adhear to the ILocation interface.
 *
 */

#pragma once
#include "LocationAccessFactory.h"
#include "ILocation.h"

namespace TA_Base_Core
{
    struct LocationAccessFactoryEx
    {
        static LocationAccessFactory& instance();
        static ILocationPtrList& getAllLocations();
        static std::string toName(size_t key);
        static std::string toDisplayName(size_t key);
        static size_t toKey(const std::string& name);
        static ILocationPtr getLocation(size_t key);
        static ILocationPtr getLocation(const std::string& name);
        static ILocationPtr getLocationIf(std::function<bool(const ILocationPtr&)> pred);
        static ILocationPtr getOCC();
        static ILocationPtr getBOCC();
        static ILocationPtrList getStations();

        static ILocationPtr& getThisLocation();

        template <class Container>
        static void sortByOrderId(Container& keys)
        {
            std::sort(keys.begin(), keys.end(), [&](auto & lhs, auto & rhs) { return getLocation(lhs)->getOrderId() < getLocation(rhs)->getOrderId(); });
        }

#if 0
        std::vector<ILocation*> getAllLocations(const bool readWrite = false);
        ILocationPtrList& getAllLocationPtrs(const bool readWrite = false);
        std::vector<ILocation*> getAllAgentLocations(const bool readWrite = false);
        std::vector<ILocation*> getLocationsByProfile(ta_uint32 profileKey, const bool readWrite = false);
        ILocation::ELocationType getLocationType(const std::string& locationName) const;
        ILocation::ELocationType getLocationType(const ta_uint32 locationKey) const;
        ta_uint32 getOccLocationKey();
        ta_uint32 getOccComLocationKey();
        ta_uint32 getOccIntLocationKey();
        ta_uint32 getOccScadaLocationKey();
        ta_uint32 getBoccLocationKey();
        std::string getOccComLocationName();
        ta_uint32 getDepotLocationKey();
        ta_uint32 getDepotNDLocationKey();
        ta_uint32 getDepotSDLocationKey();
        ta_uint32 getDepotMDLocationKey();
        ta_uint32 LocationAccessFactory::getLocationKeyFromName(const std::string& name);
        std::map<std::string, unsigned char> getTIMSOriginValues();
        IConfigLocation* createLocation();
        IConfigLocation* copyLocation(const IConfigLocation* locationToCopy);
        ta_uint32 getDepotGBDLocationKey();
        ta_uint32 getDepotGDCCLocationKey();
        std::map<ta_uint32, std::pair<ta_uint32, std::string> > getOrderIdsFromRegionKeys(const std::vector<ta_uint32>& theRegionKeys);
        std::vector<ILocation*> getLocationsByType(ILocation::ELocationType locationType, const bool readWrite = false);
#endif
    };

    struct ThisLocation
    {
        static ILocationPtr& get();
        static size_t key();
        static const std::string& name();
        static ILocation::ELocationType type();
        static const std::string& typeName();
    };

    struct LocationOCC
    {
        static ILocationPtr& get();
        static size_t key();
        static const std::string& name();
        static ILocation::ELocationType type();
        static const std::string& typeName();
    };

    struct LocationBOCC
    {
        static ILocationPtr& get();
        static size_t key();
        static const std::string& name();
        static ILocation::ELocationType type();
        static const std::string& typeName();
    };
}
