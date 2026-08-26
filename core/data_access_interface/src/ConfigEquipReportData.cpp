/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/ConfigEquipReportData.cpp $
 * @author:  Ripple
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * <description>
 *
 */
#ifdef __WIN32__
    #pragma warning(disable:4786)
    #pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include "core/types/src/ta_types.h"
#include "core/data_access_interface/src/ConfigEquipReportData.h"
#include "core/data_access_interface/src/EquipReportDataHelper.h"

#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
    ta_int32 ConfigEquipReportData::s_nextAvailableIdentifier = 0;

    const std::string   ConfigEquipReportData::REPORTKEY        = "0";

    ConfigEquipReportData::ConfigEquipReportData()
        : m_EquipReportDataHelper(new EquipReportDataHelper()),
          m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }

    ConfigEquipReportData::ConfigEquipReportData(std::string& key)
        : m_EquipReportDataHelper(new EquipReportDataHelper(key)),
          m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }

    ConfigEquipReportData::ConfigEquipReportData(const ConfigEquipReportData& theEquipReportData)
        : m_EquipReportDataHelper(new EquipReportDataHelper(*(theEquipReportData.m_EquipReportDataHelper))),
          m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }

    ConfigEquipReportData::ConfigEquipReportData(std::string& key,
                                                 std::string& reportKey)
        : m_EquipReportDataHelper(new EquipReportDataHelper(key, reportKey)),
          m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }

    ConfigEquipReportData::ConfigEquipReportData(const ta_uint32 row, TA_Base_Core::IData& data)
        : m_EquipReportDataHelper(new EquipReportDataHelper(row, data)),
          m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }

    ConfigEquipReportData::~ConfigEquipReportData()
    {
        if (m_EquipReportDataHelper != NULL)
        {
            delete m_EquipReportDataHelper;
            m_EquipReportDataHelper = NULL;
        }
    }

    bool ConfigEquipReportData::isNew()
    {
        TA_ASSERT(m_EquipReportDataHelper != NULL, "The equipment report helper is NULL");

        return m_EquipReportDataHelper->isNew();
    }

    ta_uint32 ConfigEquipReportData::getKey()
    {
        TA_ASSERT(m_EquipReportDataHelper != NULL, "The equipment report helper is NULL");

        return m_EquipReportDataHelper->getKey();
    }

    std::string  ConfigEquipReportData::getId()
    {
        TA_ASSERT(m_EquipReportDataHelper != NULL, "The equipment report helper is NULL");

        return m_EquipReportDataHelper->getId();
    }

    ta_uint32 ConfigEquipReportData::getDataNodeKey()
    {
        TA_ASSERT(m_EquipReportDataHelper != NULL, "The equipment report helper is NULL");

        return m_EquipReportDataHelper->getDataNodeKey();
    }

    ta_uint32 ConfigEquipReportData::getDataPointKey()
    {
        TA_ASSERT(m_EquipReportDataHelper != NULL, "The equipment report helper is NULL");

        return m_EquipReportDataHelper->getDataPointKey();
    }

    std::string ConfigEquipReportData::getReportKey()
    {
        TA_ASSERT(m_EquipReportDataHelper != NULL, "The equipment report helper is NULL");

        return m_EquipReportDataHelper->getReportKey();
    }

    void ConfigEquipReportData::invalidate()
    {
        TA_ASSERT(m_EquipReportDataHelper != NULL, "The equipment report helper is NULL");

        m_EquipReportDataHelper->invalidate();
        m_EquipReportDataChanges.clear();
    }

    void ConfigEquipReportData::applyChanges()
    {
        TA_ASSERT(m_EquipReportDataHelper != NULL, "The equipment report helper is NULL");

        m_EquipReportDataHelper->applyChanges();
        m_EquipReportDataChanges.clear();
    }

    void ConfigEquipReportData::deleteThisEquipReportData()
    {
        TA_ASSERT(m_EquipReportDataHelper != NULL, "The equipment report helper is NULL");

        m_EquipReportDataHelper->deleteThisEquipReportData();
    }

    void ConfigEquipReportData::setReportKey(std::string& reportKey)
    {
        TA_ASSERT(m_EquipReportDataHelper != NULL, "The equipment report helper is NULL");

        //updateEquipReportDataChanges(NAME, m_EquipReportHelper->getName(), name);

        m_EquipReportDataHelper->setReportKey(reportKey);
    }

    void ConfigEquipReportData::setDataNodeKey(const ta_uint32 dataNodeKey)
    {
        TA_ASSERT(m_EquipReportDataHelper != NULL, "The equipment report helper is NULL");

        //updateEquipReportDataChanges(NAME, m_EquipReportHelper->getName(), name);

        m_EquipReportDataHelper->setDataNodeKey(dataNodeKey);
    }

    void ConfigEquipReportData::setDataPointKey(const ta_uint32 dataPointKey)
    {
        TA_ASSERT(m_EquipReportDataHelper != NULL, "The equipment report helper is NULL");

        //updateEquipReportDataChanges(NAME, m_EquipReportHelper->getName(), name);

        m_EquipReportDataHelper->setDataPointKey(dataPointKey);
    }

    void ConfigEquipReportData::updateEquipReportDataChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
    {
        FUNCTION_ENTRY("updateLocationChanges");

        // Search for this name in the map
        ItemChanges::iterator matching = m_EquipReportDataChanges.find(name);

        if (matching != m_EquipReportDataChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_EquipReportDataChanges.erase(matching);
            }
            else
            {
                //Update the stored new value.
                matching->second.newValue = newValue;
            }

            FUNCTION_EXIT;
            return;
        }

        // The first time we come in we should get to here and we set the old value to be the original value of the attribute
        if (oldValue != newValue)
        {
            // If the old and new values are different then we can add this item to the map
            Values values;
            values.newValue = newValue;
            values.oldValue = oldValue;
            m_EquipReportDataChanges.insert(ItemChanges::value_type(name, values));
        }

        FUNCTION_EXIT;
    }

} //TA_Base_Core
