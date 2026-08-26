

#ifndef ALARM_DASHBOARD_ACCESS_FACTORY_H
#define ALARM_DASHBOARD_ACCESS_FACTORY_H

#include "core/types/src/ta_types.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

namespace TA_Base_Core
{
	struct PumpEntity
	{
		ta_uint32	entitykey;
		std::string	entityname;
		ta_uint32	locationkey;
		std::string	locationName;
		bool		isRunning;
		time_t		updateTime;
		ta_float64	floatRate;
	};

	typedef std::vector<PumpEntity> PumpEntityList;
	typedef std::vector<PumpEntity>::iterator PumpEntityListIter;
	typedef std::vector<PumpEntity>::const_iterator PumpEntityListCIter;

	//key - entitykey
	typedef std::map<ta_uint32, PumpEntityList> PumpEntityMap;
	typedef std::map<ta_uint32, PumpEntityList>::iterator PumpEntityMapIter;

	struct SubsystemInfo
	{
		ta_uint32	groupId;
		std::string	groupName;
		ta_uint32	subsysId;	
		std::string subsysName;
	};

	typedef std::vector<SubsystemInfo> SubsystemInfoList;
	typedef std::vector<SubsystemInfo>::iterator SubsysInfoListIter;
	typedef std::vector<SubsystemInfo>::const_iterator SubsysInfoListCIter;

	typedef std::map<ta_uint32, SubsystemInfoList> SubsysGroupMap;
	typedef std::map<ta_uint32, SubsystemInfoList>::iterator SubsysGroupMapIter;

    class AlarmDashboardAccessFactory
    {
    public:

        virtual ~AlarmDashboardAccessFactory() {};

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of an AlarmDashboardAccessFactory object.
         */
        static AlarmDashboardAccessFactory& getInstance();

        /**
         * freeInstance
         *
         * deletes the object instance of this class.
         *
         * @return none.
         */
        static void freeInstance();

		void getTodayPumpEntities(PumpEntityMap& pumpMap);

		void getSubsystemGroups(SubsysGroupMap& subsysGrpMap);

        //std::vector<IAlarmSeverityData*> getAllAlarmSeverities(const bool readWrite = false);

   /* private:

        void getAlarmSeverityBySql(IDatabase* pDatabase, const SQLStatement& sql, std::vector<IAlarmSeverityData*>& alarmSeverities, const bool readWrite);*/

    private:

		AlarmDashboardAccessFactory() {};
		AlarmDashboardAccessFactory(const AlarmDashboardAccessFactory& theAlarmSeverityAccessFactory);
		AlarmDashboardAccessFactory& operator=(const AlarmDashboardAccessFactory&);

        static AlarmDashboardAccessFactory* m_instance;
    };
} // closes TA_Base_Core

#endif // ALARM_DASHBOARD_ACCESS_FACTORY_H
