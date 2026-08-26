/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/MmsRequestAlarmData.cpp $
 * @author:  Luo HuiRong
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * It holds the data specific to a MmsRequestAlarmData
 * entry in the database.
 */

#ifdef _MSC_VER
    #pragma warning(disable:4786)
#endif // _MSC_VER

#include "core/types/src/ta_types.h"
#include "MmsRequestAlarmData.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
    MmsRequestAlarmData::MmsRequestAlarmData(ta_uint32 key,
                                             const std::string& alarmID,
                                             const timeb alarmDate,
                                             ta_uint32 alarmSeverity,
                                             const std::string& equipmentID,
                                             const std::string& alarmFaultCode,
                                             const std::string& alarmDescription,
                                             const std::string& requestor,
                                             const std::string& problemReport,
                                             const timeb targetDate,
											 bool isAutoTrigger,
											 ta_uint32 alarmState):
        m_key(key), m_alarmID(alarmID.c_str()), m_alarmDate(alarmDate), m_alarmSeverity(alarmSeverity), m_equipmentID(equipmentID.c_str()),
        m_alarmFaultCode(alarmFaultCode.c_str()), m_alarmDescription(alarmDescription.c_str()), m_requestor(requestor.c_str()),
        m_problemReport(problemReport.c_str()), m_targetDate(targetDate), m_alarmState(alarmState), m_isAutoTrigger(isAutoTrigger)
    {
        m_alarmDate.dstflag = -1;
        m_alarmDate.timezone = -1;

        m_targetDate.dstflag = -1;
        m_targetDate.timezone = -1;
    }

    MmsRequestAlarmData::~MmsRequestAlarmData()
    {
    }

    ta_uint32 MmsRequestAlarmData::getKey()
    {
        return m_key;
    }

    std::string MmsRequestAlarmData::getAlarmID()
    {
        return m_alarmID;
    }

    timeb MmsRequestAlarmData::getAlarmDate()
    {
        return m_alarmDate;
    }

    ta_uint32 MmsRequestAlarmData::getAlarmSeverity()
    {
        return m_alarmSeverity;
    }

    std::string MmsRequestAlarmData::getAlarmEquipmentID()
    {
        return m_equipmentID;
    }

    std::string MmsRequestAlarmData::getAlarmFaultCode()
    {
        return m_alarmFaultCode;
    }

    std::string MmsRequestAlarmData::getAlarmDescription()
    {
        return m_alarmDescription;
    }

    std::string MmsRequestAlarmData::getRequestor()
    {
        return m_requestor;
    }

    std::string MmsRequestAlarmData::getProblemReport()
    {
        return m_problemReport;
    }

    timeb MmsRequestAlarmData::getTargetDate()
    {
        return m_targetDate;
    }

	ta_uint32 MmsRequestAlarmData::getAlarmState()
	{
		return m_alarmState;
	}

    bool  MmsRequestAlarmData::getIsAutoTrigger()
    {
        return m_isAutoTrigger;
    }
} // closes TA_Base_Core
