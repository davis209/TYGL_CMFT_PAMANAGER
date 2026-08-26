/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/LocationAccessFactoryEx.cpp $
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

#include "LocationAccessFactoryEx.h"
#include "core/utilities/src/StdUtil.h"
#include "core/utilities/src/RunParamsEx.h"

using namespace std::string_literals;

namespace TA_Base_Core
{
    LocationAccessFactory& LocationAccessFactoryEx::instance()
    {
        return LocationAccessFactory::getInstance();
    }

    ILocationPtrList& LocationAccessFactoryEx::getAllLocations()
    {
        return instance().getAllLocationPtrs();
    }

    std::string LocationAccessFactoryEx::toName(size_t key)
    {
        return stdutil::no_throw_or("", [&]
        {
            auto location = getLocation(key);
            return location ? location->getName() : ""s;
        });
    }

    std::string LocationAccessFactoryEx::toDisplayName(size_t key)
    {
        auto location = getLocation(key);
        return location ? location->getDisplayName() : ""s;
    }

    size_t LocationAccessFactoryEx::toKey(const std::string& name)
    {
        auto location = getLocation(name);
        return location ? location->getKey() : 0;
    }

    ILocationPtr LocationAccessFactoryEx::getLocation(size_t key)
    {
        return getLocationIf([&](auto & location) { return location->getKey() == key; });
    }

    ILocationPtr LocationAccessFactoryEx::getLocation(const std::string& name)
    {
        return getLocationIf([&](auto & location) { return location->getName() == name; });
    }

    ILocationPtr LocationAccessFactoryEx::getLocationIf(std::function<bool(const ILocationPtr&)> pred)
    {
        return stdutil::get_if(getAllLocations(), pred);
    }

    ILocationPtr LocationAccessFactoryEx::getOCC()
    {
        return getLocationIf([&](auto & location) { return location->getLocationType() == ILocation::OCC; });
    }

    ILocationPtr LocationAccessFactoryEx::getBOCC()
    {
        return getLocationIf([&](auto & location) { return location->getLocationType() == ILocation::BOCC; });
    }

    ILocationPtrList LocationAccessFactoryEx::getStations()
    {
        return stdutil::filter_copy(getAllLocations(), [](auto & location) { return location->getLocationType() == ILocation::STATION; });
    }

    ILocationPtr& LocationAccessFactoryEx::getThisLocation()
    {
        static auto s_this = getLocation(RunParamsEx::get<size_t>(RPARAM_LOCATIONKEY));
        return s_this;
    }

    ILocationPtr& ThisLocation::get()
    {
        return LocationAccessFactoryEx::getThisLocation();
    }

    size_t ThisLocation::key()
    {
        static auto s_key = get()->getKey();
        return s_key;
    }

    const std::string& ThisLocation::name()
    {
        static auto s_name = get()->getName();
        return s_name;
    }

    ILocation::ELocationType ThisLocation::type()
    {
        static auto s_type = get()->getLocationType();
        return s_type;
    }

    const std::string& ThisLocation::typeName()
    {
        static auto s_name = get()->getTypeName();
        return s_name;
    }

    ILocationPtr& LocationOCC::get()
    {
        static auto s_occ = LocationAccessFactoryEx::getOCC();
        return s_occ;
    }

    size_t LocationOCC::key()
    {
        static auto s_key = get()->getKey();
        return s_key;
    }

    const std::string& LocationOCC::name()
    {
        static auto s_name = get()->getName();
        return s_name;
    }

    ILocation::ELocationType LocationOCC::type()
    {
        return ILocation::OCC;
    }

    const std::string& LocationOCC::typeName()
    {
        static auto s_name = get()->getTypeName();
        return s_name;
    }

    ILocationPtr& LocationBOCC::get()
    {
        static auto s_bocc = LocationAccessFactoryEx::getBOCC();
        return s_bocc;
    }

    size_t LocationBOCC::key()
    {
        if (auto& l = get())
        {
            return l->getKey();
        }

        return -1;
    }

    const std::string& LocationBOCC::name()
    {
        if (auto& l = get())
        {
            static auto s_name = l->getName();
            return s_name;
        }

        return LocationAccessFactory::BOCC_LOCATION_NAME;
    }

    ILocation::ELocationType LocationBOCC::type()
    {
        return ILocation::BOCC;
    }

    const std::string& LocationBOCC::typeName()
    {
        if (auto& l = get())
        {
            static auto s_typename = l->getTypeName();
            return s_typename;
        }

        return LocationAccessFactory::BOCC_LOCATION_NAME;
    }
}
