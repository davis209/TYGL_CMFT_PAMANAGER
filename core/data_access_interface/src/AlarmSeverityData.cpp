/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/AlarmSeverityData.cpp $
 * @author:  Andy Siow
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * AlarmSeverityData is an implementation of IAlarmSeverityData. It holds the data specific to a Alarm
 * severity entry in the database.
 */

#ifdef __WIN32__
    #pragma warning(disable:4786)
    #pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include "core/types/src/ta_types.h"
#include "core/data_access_interface/src/AlarmSeverityData.h"
#include "core/data_access_interface/src/AlarmSeverityHelper.h"
#include "core/data_access_interface/src/IData.h"
#include "core/exceptions/src/DataException.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
    AlarmSeverityData::AlarmSeverityData(const ta_uint32 key)
        :
        m_helper(new AlarmSeverityHelper(key))
    {
    }

    AlarmSeverityData::AlarmSeverityData(const ta_uint32 row, TA_Base_Core::IData& data)
        :
        m_helper(new AlarmSeverityHelper(row, data))
    {
    }

    AlarmSeverityData::~AlarmSeverityData()
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

    ta_uint32 AlarmSeverityData::getKey()
    {
        return m_helper->getKey();
    }

    std::string AlarmSeverityData::getName()
    {
        return m_helper->getName();
    }

    ta_uint32 AlarmSeverityData::getSeverityColourKey(EColourType type)
    {
        TA_ASSERT(m_helper != NULL, "The SeverityData helper is NULL");

        return m_helper->getSeverityColourKey(type);
    }

    time_t AlarmSeverityData::getDateCreated()
    {
        TA_ASSERT(m_helper != NULL, "The SeverityData helper is NULL");

        return m_helper->getDateCreated();
    }

    time_t AlarmSeverityData::getDateModified()
    {
        TA_ASSERT(m_helper != NULL, "The SeverityData helper is NULL");

        return m_helper->getDateModified();
    }

    void AlarmSeverityData::invalidate()
    {
        FUNCTION_ENTRY("AlarmSeverityData::invalidate");
        m_helper->invalidate();
    }

} // closes TA_Base_Core
