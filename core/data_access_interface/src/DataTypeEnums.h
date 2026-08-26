/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $Source:  $
 * @author:  Robert Stagg
 * @version: $Revision: #3 $
 *
 * Last modification: $Date: 2023/10/16 $
 * Last modified by:  $Author: CM $
 *
 * This file is generated automagicly from Database_Directory.xls
 * Version 2.01
 * Do NOT edit it
 *
 */

#if !defined(DataTypeEnums_H_INLCUDED)
#define DataTypeEnums_H_INLCUDED

namespace TA_Base_Core
{
    enum EDataTypes
    {
        Event_Ad,
        Event_SCd,
        Sessions_Ad,
        Sessions_SCd,
        Sessions_Cached,
        Statistics_Ad,
        Trend_Ad,
        Incident_Ad,
        Incident_SCd,
        Common_Ad,
        Common_SCd,
        Alarm_Sd,
        CallStack_Sd,
        TisRatis_Sd,
        Version_Sd,
        Alarms_Cd,
        Alarms_SCd,
        AlarmsLocal_Cd,
        Application_Cd,
        Application_SCd,
        Application_Cached,
        Archiving_Cd,
        Ats_Cd,
        Ats_SCd,
        Dr_Cd,
        Dr_SCd,
        Ecs_Cd,
        Ecs_SCd,
        Incident_Cd,
        MmsScheduling_Cd,
        OfflineSecurity_Cd,
        OfflineSecurity_SCd,
        OfflineSecurity_Cached,
        OnlineSecurity_Cd,
        OnlineSecurity_SCd,
        OnlineSecurity_Cached,
        OnlineUpdatable_Cd,
        OnlineUpdatable_SCd,
        OnlineUpdatable_Cached,
        Pa_Cd,
        Pa_SCd,
        Plans_Cd,
        Plans_SCd,
        Radio_Ad,
        Radio_Cd,
        Radio_SCd,
        Scada_Cd,
        Scada_SCd,
        Scheduling_Cd,
        Sound_Cd,
        Sound_SCd,
        SysController_Cd,
        SysController_SCd,
        Telephone_Cd,
        Tis_Cd,
        Tis_SCd,
        TisDisplayRequestLogs,
        Video_Cd,
        Video_SCd,
        Pids_Cd,
        Signalling_Cd,
        AlarmRules_Cd,
        AlarmRules_SCd,
        GlobalParameter_Cd,
        GlobalParameter_SCd,
        Tis_OCC_Cd,
        Archive_d,
        Archive_SCd,
        Naming_Cd,
        Naming_SCd,
        MMSRequest,
        GlobalStatus_Ad,
        LocalStorage_SCd,
        Radio_Pd,
        Pa_DCd,
        Cache_d,
		Madb_SCd,
		ArchiveEvent_Ad,
        // for new added item should put above this item
        NotExists_d,  // for handling data type in csv file can not found in here
        MaxDataTypes
    };

    enum EDataActions
    {
        Read,
        Write,
        MaxDataActions
    };
}

#endif
