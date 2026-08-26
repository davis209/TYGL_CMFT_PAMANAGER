/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/CombinedEventData.h $
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
 */

#if !defined(_COMBINED_EVENT_DATA_H_)
#define _COMBINED_EVENT_DATA_H_

#include "core/types/src/ta_types.h"
#include "core/data_access_interface/src/ICombinedEventData.h"

namespace TA_Base_Core
{
    class IDatabase;
}

namespace TA_Base_Core
{
    class CombinedEventData : public ICombinedEventData
    {
    public:

        /**
         * Constructor
         *
         * This constructs a CombinedEventData object that has an initial state.
         * If data is requested it will NOT be loaded from the database.
         *
         * @param key The key that identifies the event.
         * @param logTime The time at which the event was logged.
         * @param alarmSeverityName The name of the severity for the alarm associated
         *                          with the event.
         * @param assetName The asset name for the equipment that generated the event.
         * @param description The description of the event.
         * @param value The value of the event.
         * @param alarmMmsState The MMS state for the alarm associated with the event.
         * @param alarmDssState The DSS state for the alarm associated with the event.
         * @param alarmAvlState The AVL state for the alarm associated with the event.
         * @param operatorName The name of the operator responsible for generating
         *                     the event.
         * @param alarmComment The comment for the alarm associated with the event.
         * @param alarmId The ID for the alarm associated with the event.
         */
        CombinedEventData(const ECombinedEventType type,
                          const ta_uint64 key,
                          const timeb& createTime,
                          const std::string& alarmSeverityName,
                          const std::string& assetName,
                          const std::string& description,
                          const std::string& value,
                          const std::string& alarmMmsState,
                          const std::string& alarmDssState,
                          const std::string& alarmAvlState,
                          const std::string& operatorName,
                          const std::string& alarmComment,
                          const std::string& alarmId,
                          const std::string& alarmAck,
                          const std::string& alarmStatus,
                          const std::string& eventTypeName = "",
                          const std::string& operationMode = "",
                          const EventCategory categoryType = ISCSCategory,
                          const std::string& atsEventID = "",
                          bool isSystemEvent = false) :
            m_type(type), m_key(key), m_createTime(createTime), m_alarmSeverityName(alarmSeverityName),
            m_assetName(assetName), m_description(description), m_value(value),
            m_alarmMmsState(alarmMmsState), m_alarmDssState(alarmDssState),
            m_alarmAvlState(alarmAvlState), m_operatorName(operatorName),
            m_alarmComment(alarmComment), m_alarmId(alarmId), m_alarmAck(alarmAck),
            m_alarmStatus(alarmStatus), m_EventTypeName(eventTypeName),
            m_operationMode(operationMode), m_categoryType(categoryType), m_eventID(atsEventID),
            m_isSystemEvent(isSystemEvent)
        {
        }

        //++Noel
        CombinedEventData(
            const std::string& eventID,
            const ECombinedEventType type,
            const ta_uint64 key,
            const timeb& createTime,
            const std::string& alarmSeverityName,
            const std::string& assetName,
            const std::string& description,
            const std::string& value,
            const std::string& alarmMmsState,
            const std::string& alarmDssState,
            const std::string& alarmAvlState,
            const std::string& operatorName,
            const std::string& alarmComment,
            const std::string& alarmId,
            const std::string& alarmAck,
            const std::string& alarmStatus,
            //++Noel
            const std::string& sourceTable,
            const ta_uint32    subsystemKey,
            const ta_int32 physicalSubsystemKey,
            const ta_uint32    locationKey,
            const ta_uint32    severityKey,
            const ta_uint32    eventTypeKey,
            const ta_uint32    alarmTypeKey,
            const ta_uint32    operatorKey,
            const ta_uint32    eventLevel,
            const std::string&  sessionKey,
            const ta_uint32    sessionLocation,
            const ta_int32 profileId,
            const ta_uint32    actionId,
            const ta_uint32    entityKey,
            const std::string&  avlalarmheadid,
            const ta_uint32    systemKey,
            //Noel++
            const std::string& eventTypeName = "",
            const std::string& operationMode = "",
            bool isSystemEvent = false,
            const EventCategory categoryType = ISCSCategory,
            const std::string& atsEventID = "") :
            m_eventID(eventID), m_type(type), m_key(key), m_createTime(createTime), m_alarmSeverityName(alarmSeverityName),
            m_assetName(assetName), m_description(description), m_value(value),
            m_alarmMmsState(alarmMmsState), m_alarmDssState(alarmDssState),
            m_alarmAvlState(alarmAvlState), m_operatorName(operatorName),
            m_alarmComment(alarmComment), m_alarmId(alarmId), m_alarmAck(alarmAck),
            m_alarmStatus(alarmStatus), m_EventTypeName(eventTypeName),
            m_operationMode(operationMode), m_categoryType(categoryType),
            m_sourceTable(sourceTable), m_subsystemKey(subsystemKey), m_physicalSubsystemKey(physicalSubsystemKey),
            m_locationKey(locationKey), m_severityKey(severityKey), m_eventTypeKey(eventTypeKey),
            m_alarmTypeKey(alarmTypeKey), m_operatorKey(operatorKey), m_eventLevel(eventLevel),
            m_sessionKey(sessionKey), m_sessionLocation(sessionLocation), m_profileId(profileId),
            m_actionId(actionId), m_entityKey(entityKey), m_avlalarmheadid(avlalarmheadid), m_systemKey(systemKey),
            m_isSystemEvent(isSystemEvent)
        {
        }

        void setEventID(std::string eventID)
        {
            m_eventID = eventID;
        }

        //------------- Methods inherited from ICombinedEventData -------------//

        /**
         * getEventType
         *
         * Returns the type of the event, that is, whether it is an audit event or a datapoint state change
         * event.
         *
         * @return ECombinedEventType The type of the event.
         */
        virtual const ECombinedEventType getEventType();

        /**
         * getEventCategoryType
         *
         * Returns the Event category type of the event (ATS/ISCS),
         *
         * @return EventCategoryType The category of the event.
         */
        virtual const EventCategory getEventCategoryType();

        /**
         * getKey
         *
         * Returns the key for this event.
         *
         * @return ta_uint32 The key for the event.
         */
        virtual const ta_uint64 getKey();

        /**
         * getCreateTime
         *
         * Returns the creation time for this event.
         *
         * @return timeb The time at which the event was created.
         *
         * @exception DatabaseException Will be thrown if there is a problem retrieving
         *            the data from the database.
         * @exception DataException Will be thrown if data could not be converted to
         *            the requested format.
         */
        virtual const timeb getCreateTime();

        /**
         * getAlarmSeverityName
         *
         * Returns the severity name of the alarm associated with this event.
         *
         * @return std::string& The name of the severity of the alarm associated with
         *                      this event.
         *
         * @exception DatabaseException Will be thrown if there is a problem retrieving
         *            the data from the database.
         * @exception DataException Will be thrown if data could not be converted to
         *            the requested format.
         */
        virtual const std::string& getAlarmSeverityName();

        /**
         * getAssetName
         *
         * Returns the asset name for this event.
         *
         * @return std::string& The name of the asset that generated this event.
         *
         * @exception DatabaseException Will be thrown if there is a problem retrieving
         *            the data from the database.
         * @exception DataException Will be thrown if data could not be converted to
         *            the requested format.
         */
        virtual const std::string& getAssetName();

        /**
         * getAlarmStatusEventType
         *
         * Returns the event type name if it's like
         * (AlarmAcknowledged,AlarmClosed ->AlarmNormalized,AlarmSubmitted,AlarmSupressed) for this event.
         * Otherwise, return ""
         *
         * @return std::string& The name of the asset that generated this event.
         */
        virtual const std::string& getEventTypeName();

        /**
         * getDescription
         *
         * Returns the description for this event.
         *
         * @return std::string& The description for this event.
         *
         * @exception DatabaseException Will be thrown if there is a problem retrieving
         *            the data from the database.
         * @exception DataException Will be thrown if data could not be converted to
         *            the requested format.
         */
        virtual const std::string& getDescription();

        /**
         * getValue
         *
         * Returns the value for this event.
         *
         * @return std::string& The value for this event.
         *
         * @exception DatabaseException Will be thrown if there is a problem retrieving
         *            the data from the database.
         * @exception DataException Will be thrown if data could not be converted to
         *            the requested format.
         */
        virtual const std::string& getValue();

        /**
         * getOperationMode
         *
         * Returns the Operation_mode(B,M,O,'') for the alarm associated with this event.
         *
         * @return std::string& The Operation mode of the alarm associated with this event.
         */
        virtual const std::string& getOperationMode();

        /**
         * getAlarmMmsState
         *
         * Returns the MMS state for the alarm associated with this event.
         *
         * @return std::string& The MMS state of the alarm associated with this event.
         */
        virtual const std::string& getAlarmMmsState();

        /**
         * getAlarmDssState
         *
         * Returns the DSS state for the alarm associated with this event.
         *
         * @return std::string& The DSS state of the alarm associated with this event.
         *
         * @exception DatabaseException Will be thrown if there is a problem retrieving
         *            the data from the database.
         * @exception DataException Will be thrown if data could not be converted to
         *            the requested format.
         */
        virtual const std::string& getAlarmDssState();

        /**
         * getAlarmAvlState
         *
         * Returns the AVL state for the alarm associated with this event.
         *
         * @return std::string& The AVL state of the alarm associated with this event.
         *
         * @exception DatabaseException Will be thrown if there is a problem retrieving
         *            the data from the database.
         * @exception DataException Will be thrown if data could not be converted to
         *            the requested format.
         */
        virtual const std::string& getAlarmAvlState();

        /**
         * getOperatorName
         *
         * Returns the name of the operator directly responsible for generating
         * this event.
         *
         * @return std::string& The name of the operator.
         *
         * @exception DatabaseException Will be thrown if there is a problem retrieving
         *            the data from the database.
         * @exception DataException Will be thrown if data could not be converted to
         *            the requested format.
         */
        virtual const std::string& getOperatorName();

        /**
         * getAlarmComment
         *
         * Returns the comment of the alarm associated with this event.
         *
         * @return std::string& The comment.
         *
         * @exception DatabaseException Will be thrown if there is a problem retrieving
         *            the data from the database.
         * @exception DataException Will be thrown if data could not be converted to
         *            the requested format.
         */
        virtual const std::string& getAlarmComment();

        /**
         * getAlarmId
         *
         * Returns the ID of the alarm associated with this event.
         *
         * @return std::string& The alarm ID.
         */
        virtual const std::string& getAlarmId();

        /**
         * getAlarmAck
         *
         * Returns the Ack status of the alarm associated with this event.
         *
         * @return std::string& The alarm ack status.
         */
        virtual const std::string& getAlarmAck();

        /**
         * getAlarmStatus
         *
         * Returns the status ie. close, open or pending of the alarm associated with this event.
         *
         * @return string& The alarm status.
         */
        virtual const std::string& getAlarmStatus();

        /************************************************************************/
        /*                     Added for new Event Agent                        */
        /************************************************************************/
        /**
         * getSourceTable
         *
         * Returns the Indicator of the source: for EVENT table, anything else for LO_DATAPT_STATE_CHANGE table..
         *
         * @return std::string& The name of the source table.
         */
        virtual const std::string& getSourceTable();

        /**
         * getSubsystemKey
         *
         * Returns the Subsystem Derived from the source table.
         *
         * @return ta_uint32 the subsystem key which this event belongs to.
         */
        virtual const ta_uint32 getSubsystemKey();

        /**
         * getPhysicalSubsystemKey
         *
         * Returns the status ie. close, open or pending of the alarm associated with this event.
         *
         * @return ta_uint32 for physical Sub system key.
         */
        virtual const ta_int32 getPhysicalSubsystemKey();

        /**
         * getLocationKey
         *
         * Returns the location key for this event.
         *
         * @return ta_uint32 the location key.
         */
        virtual const ta_uint32 getLocationKey();

        /**
         * getSeverityKey
         *
         * Returns the severity of associated with this event.
         *
         * @return ta_uint32 the severity key.
         */
        virtual const ta_uint32 getSeverityKey();

        /**
         * getEventTypeKey
         *
         * Returns the event type key of this event.
         *
         * @return ta_uint32 the event type key.
         */
        virtual const ta_uint32 getEventTypeKey();

        /**
         * getAlarmTypeKey
         *
         * Returns the alarm type key of the alarm associated with this event.
         *
         * @return ta_uint32 The alarm type key.
         */
        virtual const ta_uint32 getAlarmTypeKey();

        /**
         * getOperatorKey
         *
         * Returns the operator key associated with this event.
         *
         * @return ta_uint32 operator key.
         */
        virtual const ta_uint32 getOperatorKey();

        /**
         * getEventLevel
         *
         * Returns the event level of this event.
         *
         * @return ta_uint32 the event level.
         */
        virtual const ta_uint32 getEventLevel();

        /**
         * getSessionKey
         *
         * Returns the session key associated with this event.
         *
         * @return std::string& The session key.
         */
        virtual const std::string& getSessionKey();

        /**
         * getSessionLocation
         *
         * Returns the location key of the source location related to the session ID associated with this event.
         *
         * @return ta_uint32 The location key.
         */
        virtual const ta_uint32 getSessionLocation();

        /**
         * getProfileId
         *
         * Returns the profile ID related to session associated with this event.
         *
         * @return ta_uint32 The profile ID.
         */
        virtual const ta_int32 getProfileId();

        /**
         * getActionId
         *
         * Returns the Action ID associated with this event.
         *
         * @return ta_uint32 the Action ID.
         */
        virtual const ta_uint32 getActionId();

        /**
         * getEntityKey
         *
         * Returns the entity key associated with this event.
         *
         * @return ta_uint32 The entity key.
         */
        virtual const ta_uint32 getEntityKey();

        /**
         * getAvlAlarmHeadId
         *
         * Returns the avalanche Head ID of alarm associated with this event.
         *
         * @return std::string& The avalanche head ID.
         */
        virtual const std::string& getAvlAlarmHeadId();

        /**
         * getSystemKey
         *
         * Returns the system key associated with this event.
         *
         * @return ta_uint32 the system key.
         */
        virtual const ta_uint32 getSystemKey();

        /**
         * getEventID
         *
         * Returns the event ID associated with this event.
         *
         * @return std::string the event ID.
         */
        virtual const std::string& getEventID();

        /**
         * getIsSystemEvent
         *
         * Returns true if event is system event otherwise false
         *
         * @return boolean.
         */
        virtual bool getIsSystemEvent();

    private:

        // Disable copy constructor and assignment operator.
        CombinedEventData(const CombinedEventData&);
        CombinedEventData& operator= (const CombinedEventData&);

        // Variables used to store event data.
        ECombinedEventType m_type;
		ta_uint64       m_key;
        timeb              m_createTime;
        std::string        m_alarmSeverityName;
        std::string        m_assetName;
        std::string        m_description;
        std::string        m_value;
        std::string        m_alarmMmsState;
        std::string        m_alarmDssState;
        std::string        m_alarmAvlState;
        std::string        m_operatorName;
        std::string        m_alarmComment;
        std::string        m_alarmId;
        std::string        m_alarmAck;
        std::string        m_alarmStatus;
        std::string        m_operationMode;

        // this is actually Event type name, but only for these ones:
        // "AlarmAcknowledged, AlarmClosed ->AlarmNormalized, AlarmSubmitted, AlarmSupressed"
        std::string        m_EventTypeName;
        EventCategory m_categoryType;

        //++Noel =========================
        std::string     m_sourceTable;
        ta_uint32 m_subsystemKey;
        ta_int32       m_physicalSubsystemKey;
        ta_uint32 m_locationKey;
        ta_uint32 m_severityKey;
        ta_uint32 m_eventTypeKey;
        ta_uint32 m_alarmTypeKey;
        ta_uint32 m_operatorKey;
        ta_uint32 m_eventLevel;
        std::string     m_sessionKey;
        ta_uint32 m_sessionLocation;
        ta_int32       m_profileId;
        ta_uint32 m_actionId;
        ta_uint32 m_entityKey;
        std::string     m_avlalarmheadid;
        ta_uint32 m_systemKey;
        std::string     m_eventID;
        bool m_isSystemEvent;
    };
};

#endif // !defined(_COMBINED_EVENT_DATA_H_)
