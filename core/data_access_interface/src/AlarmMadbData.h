
#ifndef __ALARM_MADB_DATA_H_INCLUDED__
#define __ALARM_MADB_DATA_H_INCLUDED__
#pragma once

#include "core/types/src/ta_types.h"
#include <string>

namespace TA_Base_Core
{

	struct AlarMadbData
	{
		ta_uint32 pkey;
		std::string alarmName;
		std::string alarmSource;
		std::string description;
		std::string alarmMsg;
		std::string cause;
		std::string conseq_no_action;
		std::string corrective_action;
		std::string alarmDoc;
	};

}


#endif // !__ALARM_TOP_LOCATION_DATA_H_INCLUDED__



