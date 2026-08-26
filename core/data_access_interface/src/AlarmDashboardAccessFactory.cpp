

#ifdef __WIN32__
    #pragma warning(disable:4786)
#endif

#include "core/types/src/ta_types.h"
#include "ace/OS.h"
#include <sstream>

#include "core/data_access_interface/src/AlarmDashboardAccessFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"

#include "boost/algorithm/string/split.hpp"          // Include for boost::split
#include "boost/algorithm/string/classification.hpp" // Include boost::for is_any_of

namespace TA_Base_Core
{
	AlarmDashboardAccessFactory* AlarmDashboardAccessFactory::m_instance = 0;

	AlarmDashboardAccessFactory& AlarmDashboardAccessFactory::getInstance()
    {
        if (m_instance == 0)
        {
            m_instance = new AlarmDashboardAccessFactory();
        }

        return *m_instance;
    }
	
    void AlarmDashboardAccessFactory::freeInstance()
    {
        if (0 != m_instance)
        {
            delete m_instance;
            m_instance = 0;
        }
    }

	void AlarmDashboardAccessFactory::getSubsystemGroups(SubsysGroupMap& subsysGrpMap)
	{
		FUNCTION_ENTRY("getTodayPumpEntities");
		IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Alarms_SCd, Read);
		TA_ASSERT(0 != databaseConnection, "getLocationBySql: the database connection is null");

		/*std::ostringstream sql;
	   sql << " select grp.pkey GROUPID, grp.group_name GROUPNAME, subsys.pkey SUBSYS_ID, subsys.name SUBSYS_NAME 
	   from subsystem_group_map grpmap left join subsystem subsys on grpmap.subsystem_id=subsys.pkey 
	   left join subsystem_group grp on grpmap.group_id = grp.pkey order by grp.pkey*/

		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, ALARM_DASHBOARD_110003);

		// Set up the columnNames vector to be passed to executeQuery()
		std::vector<std::string> columnNames;
		columnNames.push_back("GROUPID");
		columnNames.push_back("GROUPNAME");
		columnNames.push_back("SUBSYS_ID");
		columnNames.push_back("SUBSYS_NAME");

		// Execute the query. The method can throw a DatabaseException.
		// This is documented in the comment of this method.
		// We are responsible for deleting the returned IData object when we're done with it
		IData* data = databaseConnection->executeQuery(sql, columnNames);

		std::string strEntities = "";

		// Loop for each row returned
		do
		{
			for (size_t i = 0; i < data->getNumRows(); i++)
			{
				// The getUnsignedLongData() call can throw an exception. Need to catch
				// it to do pointer clean up.
				try
				{
					SubsystemInfo val;
					val.groupId = data->getUnsignedLongData(i, "GROUPID");
					val.groupName = data->getStringData(i, "GROUPNAME");
					val.subsysId = data->getUnsignedLongData(i, "SUBSYS_ID");
					val.subsysName = data->getStringData(i, "SUBSYS_NAME");

					SubsysGroupMapIter iter = subsysGrpMap.find(val.groupId);

					if (iter != subsysGrpMap.end())
					{
						iter->second.push_back(val);
					}
					else
					{						
						SubsystemInfoList list;
						list.push_back(val);
						subsysGrpMap[val.groupId] = list;
					}
				}
				catch (TA_Base_Core::DataException&)
				{
					// Clean up the data pointer
					delete data;
					data = NULL;
					throw;
				}
			}

			delete data;
			data = NULL;
		} while (databaseConnection->moreData(data));

		FUNCTION_EXIT;
	}

	void AlarmDashboardAccessFactory::getTodayPumpEntities(PumpEntityMap& pumpMap)
    {
        FUNCTION_ENTRY("getTodayPumpEntities");
        // get a connection to the database
        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Alarms_Cd, Read);
		TA_ASSERT(0 != databaseConnection, "getLocationBySql: the database connection is null");

		// Convert time data into formatted strings
		struct tm currentDateStruct;

		timeb createDate;
		ftime(&createDate);


		// extract the time from the struct and make it into a string
		char  currentDateString[24] = { 0 };

		if (ACE_OS::localtime_r(&createDate.time, &currentDateStruct) != NULL)
		{
			sprintf(currentDateString, "%02d/%02d/%04d", currentDateStruct.tm_mday, currentDateStruct.tm_mon + 1,
				currentDateStruct.tm_year + 1900);
		}

        /*std::ostringstream sql;
        sql << " select ENTITYKEY, VALUE FROM PUMP_STATE_CHANGE WHERE UPDATETIME == STR_TO_DATE(''%s'',''%%d/%%m/%%Y'')" */
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ALARM_DASHBOARD_110001, std::string(currentDateString));
	
		// Set up the columnNames vector to be passed to executeQuery()
		std::vector<std::string> columnNames;
		columnNames.push_back("ENTITYKEY");
		columnNames.push_back("VALUE");
		columnNames.push_back("UPDATETIME");

		// Execute the query. The method can throw a DatabaseException.
		// This is documented in the comment of this method.
		// We are responsible for deleting the returned IData object when we're done with it
		IData* data = databaseConnection->executeQuery(sql, columnNames);

		std::string strEntities = "";

		// Loop for each row returned
		do
		{
			for (size_t i = 0; i < data->getNumRows(); i++)
			{
				// The getUnsignedLongData() call can throw an exception. Need to catch
				// it to do pointer clean up.
				try
				{
					ta_uint32 entitykey = data->getUnsignedLongData(i, "ENTITYKEY");
					PumpEntityMapIter iter = pumpMap.find(entitykey);

					if (iter != pumpMap.end())
					{
						PumpEntity val;
						val.entitykey = entitykey;
						std::string value = data->getStringData(i, "VALUE");
						if (value == "1" || value == "true" || value == "True" || value == "TRUE")
						{
							val.isRunning = true;
						}
						else
						{
							val.isRunning = false;
						}					
						val.updateTime = data->getDateData(i, "UPDATETIME");
						iter->second.push_back(val);
					}
					else
					{
						PumpEntity val;
						val.entitykey = entitykey;
						std::string value = data->getStringData(i, "VALUE");
						if (value == "1" || value == "true" || value == "True" || value == "TRUE")
						{
							val.isRunning = true;
						}
						else
						{
							val.isRunning = false;
						}
						val.updateTime = data->getDateData(i, "UPDATETIME");
						PumpEntityList list;
						list.push_back(val);
						pumpMap[entitykey] = list;
					}
					strEntities = strEntities + std::to_string(entitykey) + ", ";
				}
				catch (TA_Base_Core::DataException&)
				{
					// Clean up the data pointer
					delete data;
					data = NULL;
					throw;
				}
			}

			delete data;
			data = NULL;
		} while (databaseConnection->moreData(data));

		strEntities.erase(strEntities.end() - 2, strEntities.end());

		// get a connection to the database
		IDatabase* databaseConnection2 = DatabaseFactory::getInstance().getDatabase(Alarms_SCd, Read);
		TA_ASSERT(0 != databaseConnection2, "getLocationBySql: the database connection is null");

		/*std::ostringstream sql;
		sql << " SELECT et.pkey, et.name, et.locationkey, loc.name FROM ENTITY et LEFT JOIN LOCATION loc ON et.locationkey = loc.pkey WHERE et.pkey in ('%s') */
		SQLStatement etSql;
		databaseConnection2->prepareSQLStatement(etSql, ALARM_DASHBOARD_110002, strEntities);

		// Set up the columnNames vector to be passed to executeQuery()
		std::vector<std::string> etcolumnNames;
		etcolumnNames.push_back("ENTITYKEY");
		etcolumnNames.push_back("ENTITYNAME");
		etcolumnNames.push_back("LOCATIONKEY");
		etcolumnNames.push_back("LOCATIONNAME");
		etcolumnNames.push_back("FLOW_RATE");


		// Execute the query. The method can throw a DatabaseException.
		// This is documented in the comment of this method.
		// We are responsible for deleting the returned IData object when we're done with it
		IData* etdata = databaseConnection2->executeQuery(etSql, etcolumnNames);


		// Loop for each row returned
		do
		{
			for (size_t i = 0; i < etdata->getNumRows(); i++)
			{
				// The getUnsignedLongData() call can throw an exception. Need to catch
				// it to do pointer clean up.
				try
				{
					ta_uint32 entitykey = etdata->getUnsignedLongData(i, "ENTITYKEY");

					PumpEntityMapIter mapIter = pumpMap.find(entitykey);
					if (mapIter != pumpMap.end())
					{
						PumpEntityListIter iter = mapIter->second.begin();
						for (; iter != mapIter->second.end(); iter++)
						{
							std::string etName = etdata->getStringData(i, "ENTITYNAME");
							std::vector<std::string> etNamesVec;
							boost::split(etNamesVec, etName, boost::is_any_of("."), boost::token_compress_on);
							iter->entityname = etNamesVec[3];
							iter->locationkey = etdata->getUnsignedLongData(i, "LOCATIONKEY");
							iter->locationName = etdata->getStringData(i, "LOCATIONNAME");
							iter->floatRate = etdata->getRealData(i, "FLOW_RATE");
						}
					}
				}
				catch (TA_Base_Core::DataException&)
				{
					// Clean up the data pointer
					delete etdata;
					etdata = NULL;
					throw;
				}
			}

			delete etdata;
			etdata = NULL;
		} while (databaseConnection2->moreData(etdata));

        FUNCTION_EXIT;
    }

} //closes TA_Base_Core
