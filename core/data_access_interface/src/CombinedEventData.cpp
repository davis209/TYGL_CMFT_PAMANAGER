/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/CombinedEventData.cpp $
 * @author:  Bradley Cleaver
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * This is an implementation of the ICombinedEvent interface. It provides all
 * the data required by the Event Viewer from both the EVENT and LO_DATAPT_STATE_CHANGE
 * tables.
 *
 * All data is populated when the object is constructed and is read-only.
 *
 */

// Disable "identifier was trucated to '255' characters" warnings.
#ifdef __WIN32__
    #pragma warning(disable:4786)
#endif

#include "core/types/src/ta_types.h"
#include "core/data_access_interface/src/CombinedEventData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"

#include <memory>
#include <sstream>

namespace TA_Base_Core
{
    const ECombinedEventType CombinedEventData::getEventType()
    {
        return m_type;
    }

    const EventCategory CombinedEventData::getEventCategoryType()
    {
        return m_categoryType;
    }

    const ta_uint64 CombinedEventData::getKey()
    {
        return m_key;
    }

    const timeb CombinedEventData::getCreateTime()
    {
        return m_createTime;
    }

    const std::string& CombinedEventData::getAlarmSeverityName()
    {
        return m_alarmSeverityName;
    }

    const std::string& CombinedEventData::getAssetName()
    {
        return m_assetName;
    }

    const std::string& CombinedEventData::getEventTypeName()
    {
        return m_EventTypeName;
    }

    const std::string& CombinedEventData::getDescription()
    {
        return m_description;
    }

    const std::string& CombinedEventData::getValue()
    {
        return m_value;
    }

    const std::string& CombinedEventData::getAlarmMmsState()
    {
        return m_alarmMmsState;
    }

    const std::string& CombinedEventData::getOperationMode()
    {
        return m_operationMode;
    }

    const std::string& CombinedEventData::getAlarmDssState()
    {
        return m_alarmDssState;
    }

    const std::string& CombinedEventData::getAlarmAvlState()
    {
        return m_alarmAvlState;
    }

    const std::string& CombinedEventData::getOperatorName()
    {
        return m_operatorName;
    }

    const std::string& CombinedEventData::getAlarmComment()
    {
        return m_alarmComment;
    }

    const std::string& CombinedEventData::getAlarmId()
    {
        return m_alarmId;
    }

    const std::string& CombinedEventData::getAlarmAck()
    {
        return m_alarmAck;
    }

    const std::string& CombinedEventData::getAlarmStatus()
    {
        return m_alarmStatus;
    }

    //++Noel
    const std::string& CombinedEventData::getSourceTable()
    {
        return m_sourceTable;
    }

    const ta_uint32 CombinedEventData::getSubsystemKey()
    {
        return m_subsystemKey;
    }

    const ta_int32 CombinedEventData::getPhysicalSubsystemKey()
    {
        return m_physicalSubsystemKey;
    }

    const ta_uint32 CombinedEventData::getLocationKey()
    {
        return m_locationKey;
    }

    const ta_uint32 CombinedEventData::getSeverityKey()
    {
        return m_severityKey;
    }

    const ta_uint32 CombinedEventData::getEventTypeKey()
    {
        return m_eventTypeKey;
    }

    const ta_uint32 CombinedEventData::getAlarmTypeKey()
    {
        return m_alarmTypeKey;
    }

    const ta_uint32 CombinedEventData::getOperatorKey()
    {
        return m_operatorKey;
    }

    const ta_uint32 CombinedEventData::getEventLevel()
    {
        return m_eventLevel;
    }

    const std::string& CombinedEventData::getSessionKey()
    {
        return m_sessionKey;
    }

    const ta_uint32 CombinedEventData::getSessionLocation()
    {
        return m_sessionLocation;
    }

    const ta_int32 CombinedEventData::getProfileId()
    {
        return m_profileId;
    }

    const ta_uint32 CombinedEventData::getActionId()
    {
        return m_actionId;
    }

    const ta_uint32 CombinedEventData::getEntityKey()
    {
        return m_entityKey;
    }

    const std::string& CombinedEventData::getAvlAlarmHeadId()
    {
        return m_avlalarmheadid;
    }

    const ta_uint32 CombinedEventData::getSystemKey()
    {
        return m_systemKey;
    }

    const std::string& CombinedEventData::getEventID()
    {
        return m_eventID;
    }

    bool CombinedEventData::getIsSystemEvent()
    {
        return m_isSystemEvent;
    }
};
