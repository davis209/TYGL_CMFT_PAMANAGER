
#ifndef __ALARM_TOP_LOCATION_DATA_H_INCLUDED__
#define __ALARM_TOP_LOCATION_DATA_H_INCLUDED__
#pragma once

#include "core/types/src/ta_types.h"
#include "boost/shared_ptr.hpp"
#include <string>
#include <vector>

namespace TA_Base_Core
{
	typedef std::vector<ta_uint32> NumericList;

	struct TopAlarmFilter
	{
		NumericList locations;
		
		NumericList alarmtypes;

		NumericList subsystems;

		NumericList severities;
	};

	typedef boost::shared_ptr<TopAlarmFilter> TopAlarmFilterPtr;

	struct AlarmTopLocData
	{
		ta_uint32 locationKey;

		ta_uint32 entityKey;

		ta_uint32 alarmType;

		ta_uint32 alarmSeverity;

		std::string paramList;

		ta_uint32 alarmcount;
	};

	typedef boost::shared_ptr<AlarmTopLocData> AlarmTopLocDataPtr;
	typedef std::vector<AlarmTopLocDataPtr> AlarmTopLocDataList;
}


#endif // !__ALARM_TOP_LOCATION_DATA_H_INCLUDED__



