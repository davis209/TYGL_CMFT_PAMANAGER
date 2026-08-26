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

#pragma once
#include "core/data_access_interface/src/DataTypeEnums.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/StdUtil.h"

namespace TA_Base_Core
{
    namespace
    {
        static std::map<std::string, EDataTypes, stdutil::CompareNoCase> s_dataTypeMap =
        {
            { "Event_Ad", Event_Ad },
            { "Event_SCd", Event_SCd },
            { "Sessions_Ad", Sessions_Ad },
            { "Sessions_SCd", Sessions_SCd },
            { "Sessions_Cached", Sessions_Cached },
            { "Statistics_Ad", Statistics_Ad },
            { "Trend_Ad", Trend_Ad },
            { "Incident_Ad", Incident_Ad },
            { "Incident_SCd", Incident_SCd },
            { "Common_Ad", Common_Ad },
            { "Common_SCd", Common_SCd },
            { "Alarm_Sd", Alarm_Sd },
            { "CallStack_Sd", CallStack_Sd },
            { "TisRatis_Sd", TisRatis_Sd },
            { "Version_Sd", Version_Sd },
            { "Alarms_Cd", Alarms_Cd },
            { "Alarms_SCd", Alarms_SCd },
            { "AlarmsLocal_Cd", AlarmsLocal_Cd },
            { "Application_Cd", Application_Cd },
            { "Application_SCd", Application_SCd },
            { "Application_Cached", Application_Cached },
            { "Archiving_Cd", Archiving_Cd },
            { "Ats_Cd", Ats_Cd },
            { "Ats_SCd", Ats_SCd },
            { "Dr_Cd", Dr_Cd },
            { "Dr_SCd", Dr_SCd },
            { "Ecs_Cd", Ecs_Cd },
            { "Ecs_SCd", Ecs_SCd },
            { "GlobalStatus_Ad", GlobalStatus_Ad },
            { "Incident_Cd", Incident_Cd },
            { "MmsScheduling_Cd", MmsScheduling_Cd },
            { "OfflineSecurity_Cd", OfflineSecurity_Cd },
            { "OfflineSecurity_SCd", OfflineSecurity_SCd },
            { "OfflineSecurity_Cached", OfflineSecurity_Cached },
            { "OnlineSecurity_Cd", OnlineSecurity_Cd },
            { "OnlineSecurity_SCd", OnlineSecurity_SCd },
            { "OnlineSecurity_Cached", OnlineSecurity_Cached },
            { "OnlineUpdatable_Cd", OnlineUpdatable_Cd },
            { "OnlineUpdatable_SCd", OnlineUpdatable_SCd },
            { "OnlineUpdatable_Cached", OnlineUpdatable_Cached },
            { "Pa_Cd", Pa_Cd },
            { "Pa_SCd", Pa_SCd },
            { "Pa_DCd", Pa_DCd },
            { "Plans_Cd", Plans_Cd },
            { "Plans_SCd", Plans_SCd },
            { "Radio_Ad", Radio_Ad },
            { "Radio_Cd", Radio_Cd },
            { "Radio_Pd", Radio_Pd },
            { "Radio_SCd", Radio_SCd },
            { "Scada_Cd", Scada_Cd },
            { "Scada_SCd", Scada_SCd },
            { "Scheduling_Cd", Scheduling_Cd },
            { "Sound_Cd", Sound_Cd },
            { "Sound_SCd", Sound_SCd },
            { "SysController_Cd", SysController_Cd },
            { "SysController_SCd", SysController_SCd },
            { "Telephone_Cd", Telephone_Cd },
            { "Tis_Cd", Tis_Cd },
            { "Tis_SCd", Tis_SCd },
            { "TisDisplayRequestLogs", TisDisplayRequestLogs },
            { "Video_Cd", Video_Cd },
            { "Video_SCd", Video_SCd },
            { "Pids_Cd", Pids_Cd },
            { "Signalling_Cd", Signalling_Cd },
            { "AlarmRules_Cd", AlarmRules_Cd },
            { "AlarmRules_SCd", AlarmRules_SCd },
            { "GlobalParameter_Cd", GlobalParameter_Cd },
            { "GlobalParameter_SCd", GlobalParameter_SCd },
            { "Tis_OCC_Cd", Tis_OCC_Cd },
            { "Archive_d", Archive_d },
            { "Archive_SCd", Archive_SCd },
            { "Naming_Cd", Naming_Cd },
            { "Naming_SCd", Naming_SCd },
            { "MMSRequest", MMSRequest },
            { "LocalStorage_SCd", LocalStorage_SCd },
            { "Cache_d", Cache_d },
			{ "Madb_SCd", Madb_SCd},
            { "NotExists_d", NotExists_d },
            { "MaxDataTypes", MaxDataTypes },			
			{ "ArchiveEvent_Ad", ArchiveEvent_Ad }
        };

        static std::map<std::string, EDataActions, stdutil::CompareNoCase> s_dataActionMap =
        {
            {"Read", Read },
            {"Write", Write }
        };
    }

    inline EDataTypes getDataTypesEnum(const std::string& str)
    {
        return stdutil::get_or(s_dataTypeMap, str, NotExists_d);
    }

    inline EDataActions getDataActionsEnum(const std::string& str)
    {
        return stdutil::get_or(s_dataActionMap, str, MaxDataActions);
    }

    inline std::string getDataTypesStr(EDataTypes datatype)
    {
        return stdutil::get_key_or(s_dataTypeMap, datatype, "NotExists_d");
    }

    inline std::string getDataActionsStr(EDataActions dataaction)
    {
        return stdutil::get_key_or(s_dataActionMap, dataaction, "MaxDataActions");
    }
}
