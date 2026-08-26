/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/MmsRequestScheduledData.cpp $
 * @author:  Luo HuiRong
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * It holds the data specific to a MmsRequestScheduledData
 * entry in the database.
 */

#ifdef _MSC_VER
    #pragma warning(disable:4786)
#endif // _MSC_VER

#include "core/types/src/ta_types.h"
#include "MmsRequestScheduledData.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
    MmsRequestScheduledData::MmsRequestScheduledData(ta_uint32 key,
                                                     ScheduledType scheduledType,
                                                     ta_uint32 entityKey,
                                                     const std::string& equipmentID,
                                                     const std::string& meterCode,
                                                     const std::string& equipmentDescription,
                                                     double dpValue,
                                                     timeb planDate,
                                                     timeb createDate):
        m_key(key), m_scheduledType(scheduledType), m_entityKey(entityKey), m_equipmentID(equipmentID.c_str()),
        m_meterCode(meterCode.c_str()), m_equipmentDescription(equipmentDescription.c_str()),
        m_dpValue(dpValue), m_planDate(planDate), m_createDate(createDate)
    {
        m_planDate.dstflag = -1;
        m_planDate.timezone = -1;

        m_createDate.dstflag = -1;
        m_createDate.timezone = -1;
    }

    MmsRequestScheduledData::~MmsRequestScheduledData()
    {
    }

    ta_uint32 MmsRequestScheduledData::getKey()
    {
        return m_key;
    }

    MmsRequestScheduledData::ScheduledType MmsRequestScheduledData::getScheduledType()
    {
        return m_scheduledType;
    }

    double MmsRequestScheduledData::getDpValue()
    {
        return m_dpValue;
    }

    ta_uint32 MmsRequestScheduledData::getEntityKey()
    {
        return m_entityKey;
    }

    std::string MmsRequestScheduledData::getEquipmentID()
    {
        return m_equipmentID;
    }

    std::string MmsRequestScheduledData::getMeterCode()
    {
        return m_meterCode;
    }

    std::string MmsRequestScheduledData::getEquipmentDescription()
    {
        return m_equipmentDescription;
    }

    timeb MmsRequestScheduledData::getPlanDate()
    {
        return m_planDate;
    }

    timeb MmsRequestScheduledData::getCreateDate()
    {
        return m_createDate;
    }

} // closes TA_Base_Core
