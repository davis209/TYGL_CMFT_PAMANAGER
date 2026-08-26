/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/ECSStationColourData.cpp $
 * @author:  Andy Siow
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * ECSStationColourData is an implementation of IECSStationColourData. It holds the data specific to a Alarm
 * Station entry in the database.
 */

#ifdef __WIN32__
    #pragma warning(disable:4786)
    #pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include "core/types/src/ta_types.h"
#include "core/data_access_interface/src/ECSStationColourData.h"
#include "core/data_access_interface/src/ECSStationColourHelper.h"
#include "core/data_access_interface/src/IData.h"
#include "core/exceptions/src/DataException.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
    ECSStationColourData::ECSStationColourData(const ta_uint32 key)
        :
        m_helper(new ECSStationColourHelper(key))
    {
    }

    ECSStationColourData::ECSStationColourData(const ta_uint32 row, TA_Base_Core::IData& data)
        :
        m_helper(new ECSStationColourHelper(row, data))
    {
    }

    ECSStationColourData::~ECSStationColourData()
    {
        try
        {
            delete m_helper;
            m_helper = NULL;
        }
        catch (...)
        {
            LOG_ERROR("Unknown exception caught in destructor");
        }
    }

    ta_uint32 ECSStationColourData::getKey()
    {
        return m_helper->getKey();
    }

    std::string ECSStationColourData::getName()
    {
        return m_helper->getName();
    }

    ta_uint32 ECSStationColourData::getStationColourKey(EColourType type)
    {
        TA_ASSERT(m_helper != NULL, "The StationData helper is NULL");

        return m_helper->getStationColourKey(type);
    }

    time_t ECSStationColourData::getDateCreated()
    {
        TA_ASSERT(m_helper != NULL, "The StationData helper is NULL");

        return m_helper->getDateCreated();
    }

    time_t ECSStationColourData::getDateModified()
    {
        TA_ASSERT(m_helper != NULL, "The StationData helper is NULL");

        return m_helper->getDateModified();
    }

    void ECSStationColourData::invalidate()
    {
        FUNCTION_ENTRY("ECSStationColourData::invalidate");
        m_helper->invalidate();
    }

} // closes TA_Base_Core
