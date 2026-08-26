/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/IAlarmData.h $
 * @author:  Nick Jardine
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * IAlarmData is an interface to the Alarm table. It provides both read and write access
 * for agents and user applicaitons.
 */

#if !defined(IAlarmData_532D0846_3483_438f_9831_29DF62EB8E44__INCLUDED_)
#define IAlarmData_532D0846_3483_438f_9831_29DF62EB8E44__INCLUDED_

#include "core/data_access_interface/src/IDbWritableData.h"
#include "core/types/src/ta_types.h"
#include <string>
#include <sys/timeb.h>

namespace TA_Base_Core
{
    class IAlarmData
        : public IDbWritableData
    {
    public:

        enum EMmsState
        {
            TYPE_NONE = 0,
            TYPE_SEMI,
            TYPE_AUTO,
            TYPE_JR_SENT,
            TYPE_ALM_SENT,
            TYPE_INHIBIT,
            TYPE_FAILED
        };

        enum EAlarmState
        {
            CLOSED = 0,
            OPEN,
            SUPPRESSED
        };

        enum EAlarmAVLStatus
        {
            NO_ASSOC_AVL = 0,
            HEAD_AVL,
            CHILD_AVL
        };

        /**
         *
         * Const Definitions of Plan Status
         *
         */
        static const std::string NO_ASSOC_PLAN;
        static const std::string AUTO_PLAN_NOT_RUN;
        static const std::string SEMIAUTO_PLAN_NOT_RUN;
        static const std::string PLAN_RUN;
        static const std::string PLAN_POSTPONED;

        /**
         * getKey
         *
         * Returns the key for this Alarm.
         *
         * @return The key for this alarm as an ta_uint32 .
         */
        virtual std::string getKey() = 0;

        virtual void setKey(const std::string& key) = 0;

        /**
         * getDescription
         *
         * Returns the description for this Alarm.
         *
         * @return The description for this alarm as a std::string.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be
         *            converted to the required format (e.g. ACKNOWLEDGED should be a
         *            b), or if the wrong ammount of data is retrieved.
         */
        //virtual std::string getDescription() =0;

        //virtual void setDescription(const std::string& description) =0;

        virtual timeb getSourceTime() = 0;

        virtual void setParamList(const std::string& paramString) = 0;

        virtual std::string getParamList() = 0;

        virtual void setSourceTime(const timeb sourceTime) = 0;

        virtual timeb getTransactiveTime() = 0;

        virtual void setTransactiveTime(const timeb transactiveTime) = 0;

        virtual void setAckTime(const time_t ackTime) = 0;

        virtual void setCloseTime(const time_t ackTime) = 0;

        virtual time_t getAckTime() = 0;

        virtual time_t getCloseTime() = 0;

        virtual ta_uint32 getSeverity() = 0;

        virtual void setSeverity(const ta_uint32 severity) = 0;

        virtual ta_uint32 getTypeKey() = 0;

        virtual std::string getContext() = 0;

        //virtual void setType (const ta_uint32 typeKey) =0;

        virtual void setTypeKey(const ta_uint32 typeKey) = 0;

        virtual void setContext(const std::string& context) = 0;

        virtual void setState(const EAlarmState state) = 0;

        virtual EAlarmState getState() = 0;

        virtual void setMmsStateType(const EMmsState mmsStatetType) = 0;

        virtual EMmsState getMmsStateType() = 0;

        virtual std::string getAcknowledgedBy() = 0;

        virtual void setAcknowledgedBy(const std::string& sessionID) = 0;

        virtual std::string getOperatorName() = 0;

        virtual void setOperatorName(const std::string& operatorName) = 0;

        virtual ta_uint32 getEntityKey() = 0;

        virtual void setEntityKey(const ta_uint32 key) = 0;

        virtual std::string getComments() = 0;

        virtual void addComment(const std::string& comment) = 0;

        virtual std::string getAssetName() = 0;

        virtual void setAssetName(const std::string& assetName) = 0;

        virtual std::string getPlanStatus() = 0;

        virtual void setPlanStatus(const std::string& planStatus) = 0;

        virtual EAlarmAVLStatus getAVLStatus() = 0;

        virtual void setAVLStatus(const EAlarmAVLStatus avlStatus) = 0;

        virtual std::string getAVLHeadId() = 0;

        virtual void setAVLHeadId(const std::string& avlHeadId) = 0;

        virtual bool getIsUnique() = 0;

        virtual void setIsUnique(const bool isUnique) = 0;

        virtual void setPropagate(const bool doPropagate) = 0;

        virtual bool getPropagate() = 0;

        //
        // The following get methods are not accompanied by any set methods.
        // This is because the variables originate from different tables (ie not the alarm table)
        //

        virtual void setSubsystemKeyOfAssociatedEntity(const ta_uint32 subsystemKey) = 0;
        virtual void setLocationKeyOfAssociatedEntity(const ta_uint32 locationKey) = 0;
        virtual void setTypeOfAssociatedEntity(const ta_uint32 entityType) = 0;

        virtual ta_uint32 getSubsystemKeyOfAssociatedEntity() = 0;
        virtual ta_uint32 getLocationKeyOfAssociatedEntity() = 0;
        virtual ta_uint32 getTypeOfAssociatedEntity() = 0;

        // Non get/set methods

        virtual void invalidate() = 0;

        virtual void applyChanges(const std::string& key) = 0;

        virtual void applyChanges() = 0;

        virtual ~IAlarmData() {};
    };

} //close namespace TA_Base_Core

#endif // !defined(IAlarmData_532D0846_3483_438f_9831_29DF62EB8E44__INCLUDED_)
