/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/AlarmData.h $
 * @author:  Nick Jardine
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * This is an implementation of the IAlarmData interface. It provides access to the data
 * stored in a single entry in the Alarm table.
 */

#if !defined(AlarmData_F27DA2D5_8C73_4478_8ECB_F00E559B2665__INCLUDED_)
#define AlarmData_F27DA2D5_8C73_4478_8ECB_F00E559B2665__INCLUDED_

#include "core/types/src/ta_types.h"
#include "core/data_access_interface/src/IAlarmData.h"

#include <string>

namespace TA_Base_Core
{
    class AlarmData : public IAlarmData
    {
    public:

        AlarmData(const std::string& key): m_isValidData(false), m_key(key),
            m_isNew(false), m_propagate(false)
        {
            reload();
        };

        /**
         * constructor
         *
         * Construct a new, valid, Alarm object
         */
        AlarmData(const std::string& key,
                  //const std::string& description,
                  const std::string& paramString,
                  const timeb sourceTime,
                  const timeb transactiveTime,
                  const time_t ackTime,
                  const time_t closeTime,
                  ta_uint32 severityKey,
                  ta_uint32 typeKey,
                  const std::string& context,
                  EAlarmState state,
                  EMmsState mmsStateType,
                  const std::string& acknowledgedBy,
                  ta_uint32 entityKey,
                  const std::string& comment,
                  const std::string& assetName,
                  const std::string& planStatus,
                  ta_uint32 subsystemKey,
                  ta_uint32 locationKey,
                  ta_uint32 entityType,
                  const std::string& ackByOperatorName,
                  const bool isUnique,
                  const std::string& avlHeadId,
                  EAlarmAVLStatus avlStatus = NO_ASSOC_AVL,
                  const bool doPropagate = false);

        AlarmData(): m_isValidData(true), m_key(""),
            m_paramString(""), m_ackTime(0), m_severityKey(UINT_MAX),
            m_typeKey(0), m_context(""), m_state(CLOSED), m_mmsStateType(TYPE_NONE), m_acknowledgedBy(""),
            m_entityKey(UINT_MAX), m_comment(""), m_planStatus(NO_ASSOC_PLAN), m_ackByOperatorName(""),
            m_avlHeadId(""), m_avlStatus(NO_ASSOC_AVL), m_isUnique(true), m_isNew(true), m_propagate(false)
        {
            m_sourceTime.time = 0;
            m_sourceTime.millitm = 0;
            m_sourceTime.timezone = -1;
            m_sourceTime.dstflag = -1;
            m_transactiveTime.time = 0;
            m_transactiveTime.millitm = 0;
            m_transactiveTime.timezone = -1;
            m_transactiveTime.dstflag = -1;
        };

        /**
         * getKey  Returns the key for this Alarm.
         * @return The key for this alarm as a std::string.
         */
        std::string getKey();

        void setKey(const std::string& key);
        /**
         * getDescription  Returns the description for this Alarm.
         * @return The description for this alarm as a std::string.
         */
        //std::string getDescription();
        //void setDescription(const std::string& description);
        timeb getSourceTime();
        void setParamList(const std::string& paramString);
        std::string getParamList();
        void setSourceTime(const timeb sourceTime);
        timeb getTransactiveTime();
        void setTransactiveTime(const timeb transactiveTime);
        void setAckTime(const time_t ackTime);
        void setCloseTime(const time_t closeTime);
        time_t getAckTime();
        time_t getCloseTime();
        ta_uint32 getSeverity();
        void setSeverity(const ta_uint32 severity);
        ta_uint32 getTypeKey();
        std::string getContext();
        void setTypeKey(const ta_uint32 typeKey);
        void setContext(const std::string& context);
        void setState(const EAlarmState);
        EAlarmState getState();
        /**
         * setState
         *
         * pre: 0 < state < 3
         */
        void setMmsStateType(const EMmsState);
        EMmsState getMmsStateType();
        std::string getAcknowledgedBy();
        void setAcknowledgedBy(const std::string& sessionID);
        std::string getOperatorName();

        void setOperatorName(const std::string& operatorName);

        ta_uint32 getEntityKey();
        void setEntityKey(const ta_uint32 key);
        std::string getComments();
        void addComment(const std::string& comment);
        std::string getAssetName();
        void setAssetName(const std::string& assetName);
        std::string getPlanStatus();
        void setPlanStatus(const std::string& planStatus);
        EAlarmAVLStatus getAVLStatus();
        void setAVLStatus(const EAlarmAVLStatus avlStatus);
        std::string getAVLHeadId();
        void setAVLHeadId(const std::string& avlHeadId);
        bool getIsUnique();
        void setIsUnique(const bool isUnique);

        //
        // The following get methods are not accompanied by any set methods.
        // This is because the variables originate from different tables (ie not the alarm table)
        //
        void setSubsystemKeyOfAssociatedEntity(const ta_uint32 subsystemKey);
        void setLocationKeyOfAssociatedEntity(const ta_uint32 locationKey);
        void setTypeOfAssociatedEntity(const ta_uint32 entityType);

        ta_uint32 getSubsystemKeyOfAssociatedEntity();
        ta_uint32 getLocationKeyOfAssociatedEntity();
        ta_uint32 getTypeOfAssociatedEntity();

        void setPropagate(const bool doPropagate);
        bool getPropagate();

        void invalidate();
        void applyChanges(const std::string& key);
        void applyChanges();
        virtual ~AlarmData() {};

    private:

        AlarmData(const AlarmData& theAlarmData);
        AlarmData& operator=(const AlarmData&);

        /**
         * getLocalDatabaseName
         *
         * returns the name of the local database
         */
        std::string getLocalDatabaseName();

        void reload();
        void createNewAlarm();
        void updateExistingAlarm();

        // The name of the local database to send updates to
        std::string m_localDatabase;
        bool m_isValidData;
        std::string m_key;
        //std::string m_description;
        std::string m_paramString;
        timeb m_sourceTime;
        timeb m_transactiveTime;
        time_t m_ackTime;
        time_t m_closeTime;
        ta_uint32 m_severityKey;
        ta_uint32 m_typeKey;
        std::string m_context;
        EAlarmState m_state;
        EMmsState m_mmsStateType;
        std::string m_acknowledgedBy;
        ta_uint32 m_entityKey;
        std::string m_comment;
        std::string m_assetName;
        std::string m_planStatus;
        std::string m_avlHeadId;
        EAlarmAVLStatus m_avlStatus;
        ta_uint32 m_subsystemKey;
        ta_uint32 m_locationKey;
        ta_uint32 m_entityType;
        std::string m_ackByOperatorName;
        bool m_propagate;
        bool m_isUnique;

        bool m_isSourceTimeModified;
        bool m_isTransactiveTimeModified;
        bool m_isAckTimeModified;
        bool m_isCloseTimeModified;
        //bool m_isDescriptionModified;
        bool m_isParamListModified;
        bool m_isSeverityKeyModified;
        bool m_isTypeModified;
        bool m_isContextModified;
        bool m_isStateModified;
        bool m_isMmsStateTypeModified;
        bool m_isAcknowledgedByModified;
        bool m_isEntityKeyModified;
        bool m_isCommentModified;
        bool m_isAssetNameModified;
        bool m_isPlanStatusModified;
        bool m_isAVLStatusModified;
        bool m_isAVLHeadIdModified;
        bool m_isIsUniqueModified;

        bool m_isSubsystemKeyModified;
        bool m_isLocationKeyModified;
        bool m_isEntityTypeModified;

        bool m_isNew;
    };
} // closes TA_Base_Core

#endif // !defined(AlarmData_F27DA2D5_8C73_4478_8ECB_F00E559B2665__INCLUDED_)
