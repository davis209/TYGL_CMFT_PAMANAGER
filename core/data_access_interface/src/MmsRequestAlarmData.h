/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/MmsRequestAlarmData.h $
 * @author:  Luo HuiRong
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * It holds the data specific to a MmsRequestAlarmData
 * entry in the database.
 */

#if !defined(MmsRequestAlarmData_EAE4B0FF_EA1F_44d0_8046_F18DF7D250A1__INCLUDED_)
#define MmsRequestAlarmData_EAE4B0FF_EA1F_44d0_8046_F18DF7D250A1__INCLUDED_

#include "core/types/src/ta_types.h"
#include <string>
#include <time.h>
#include <sys/timeb.h>

namespace TA_Base_Core
{
    class MmsRequestAlarmData
    {
    public:

        /**
         * MmsRequestAlarmData (constructor)
         *
         * Construct a MmsRequestAlarmData class for the entry represented by the specified parameters.
         */
        MmsRequestAlarmData(ta_uint32 key,
                            const std::string& alarmID,
                            const timeb alarmDate,
                            ta_uint32 alarmSeverity,
                            const std::string& equipmentID,
                            const std::string& alarmFaultCode,
                            const std::string& alarmDescription,
                            const std::string& requestor,
                            const std::string& problemReport,
                            const timeb targetDate,
							bool isAutoTrigger = true,
                            ta_uint32 alarmState = 1);

        /**
         * MmsRequestAlarmData (deconstructor)
         *
         * Deconstruct a MmsRequestAlarmData class.
         *
         */
        virtual ~MmsRequestAlarmData();

        /**
         * getKey
         *
         * Returns the key for this alarm MMS request.
         */
        ta_uint32 getKey();

        /**
         * getAlarmID
         *
         * Returns the ID of alarm
         */
        std::string getAlarmID();

        /**
         * getAlarmDate
         *
         * Returns the source time of alarm
         */
        timeb getAlarmDate();

        /**
         * getAlarmServerity
         *
         * Returns the serverity of alarm
         */
        ta_uint32 getAlarmSeverity();

        /**
         * getEquipmentID
         *
         * Returns the equipment id of alarm
         */
        std::string getAlarmEquipmentID();

        /**
         * getFaultCode
         *
         * Returns the fault code id of alarm
         */
        std::string getAlarmFaultCode();

        /**
         * getDescription
         *
         * Returns the description id of alarm
         */
        std::string getAlarmDescription();

        /**
         * getRequestor
         *
         * Returns name of operator who raises the MMS request
         */
        std::string getRequestor();

        /**
         * getProblemReport
         *
         * Returns the problem Report
         */
        std::string getProblemReport();

        /**
         * getTargetDate
         *
         * Returns the target date time
         */
        timeb getTargetDate();

		ta_uint32 getAlarmState();

        /**
         * getIsAutoTrigger
         *
         * Returns the IsAutoTrigger
         */
        bool  getIsAutoTrigger();

    private:

        MmsRequestAlarmData& operator=(const MmsRequestAlarmData&);
        MmsRequestAlarmData(const MmsRequestAlarmData& theMmsRequestAlarmData);

        ta_uint32 m_key;
        std::string m_alarmID;
        timeb m_alarmDate;
        ta_uint32 m_alarmSeverity;
        std::string m_equipmentID;
        std::string m_alarmFaultCode;
        std::string m_alarmDescription;
        std::string m_requestor;
        std::string m_problemReport;
        timeb m_targetDate;
		ta_uint32 m_alarmState;
        bool m_isAutoTrigger;
    };
} // closes TA_Base_Core

#endif // !defined(MmsRequestAlarmData_EAE4B0FF_EA1F_44d0_8046_F18DF7D250A1__INCLUDED_)
