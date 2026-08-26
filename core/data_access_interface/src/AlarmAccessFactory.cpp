/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/AlarmAccessFactory.cpp $
 * @author:  Nick Jardine
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * AlarmAccessFactory is a singleton that is used to retrieve AlarmData objects from the database.
 * It provides both read-write, and read-only objects, but does not create new objects as they are not
 * able to be defined through the system.
 */

#ifdef __WIN32__
    #pragma warning(disable:4786)
#endif
#include "ace/OS.h"
#include "core/types/src/ta_types.h"
#include "core/data_access_interface/src/AlarmAccessFactory.h"
#include "core/data_access_interface/src/AlarmData.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Core
{
    AlarmAccessFactory* AlarmAccessFactory::m_instance = 0;

    AlarmAccessFactory& AlarmAccessFactory::getInstance()
    {
        if (m_instance == 0)
        {
            m_instance = new AlarmAccessFactory();
        }

        return *m_instance;
    }

    void AlarmAccessFactory::freeInstance()
    {
        if (0 != m_instance)
        {
            delete m_instance;
            m_instance = 0;
        }
    }

    IAlarmData* AlarmAccessFactory::getAlarm(const std::string& key)
    {
        FUNCTION_ENTRY("getAlarm");

        // TD17632
        //      std::ostringstream sql;
        //        sql << "select a.ALARM_ID,TO_CHAR(a.SOURCETIME,'YYYYMMDDHH24MISSFF3'), TO_CHAR(a.TRANSACTIVETIME,'YYYYMMDDHH24MISSFF3'), "
        //            << "a.DESCRIPTION,a.PARAM_LIST,a.SEVERITY,a.TYPE,a.CONTEXT,a.STATE,a.MMS_STATE_TYPE, a.ACKNOWLEDGED_BY,a.ENTITYKEY,a.COMMENTS,a.ASSET_NAME, "
        //            << "a.PLANSTATUS, a.AVL_HEAD_ID, a.AVL_STATUS, a.IS_UNIQUE, TO_CHAR(a.ACK_TIME,'YYYYMMDDHH24MISS'), TO_CHAR(a.CLOSE_TIME,'YYYYMMDDHH24MISS'), e.SUBSYSTEMKEY, e.PHYSICAL_SUBSYSTEM_KEY, "
        //            << "e.LOCATIONKEY, e.NAME, e.TYPEKEY, (select o.NAME from TA_SESSION s, OPERATOR o where o.pkey (+) = s.operatorkey AND s.PKEY (+) = a.ACKNOWLEDGED_BY) as operator_name "
        //            << "from ALARM a, ENTITY e "
        //            << "where a.ENTITYKEY = e.PKEY and a.ALARM_ID = '" << key << "'";
        //
        //      std::vector<IAlarmData*> alarms;
        //        runQueryForAlarms(sql.str(), alarms);
        //
        //        if ( 0 == alarms.size())
        //        {
        //            std::ostringstream message;
        //          message << "No data found for alarm with key " << key;
        //            TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, sql.str() ) );
        //        }
        //
        //      TA_ASSERT(1 == alarms.size(), "Alarm key unique constraint violated");

        // Return the pointer. The class that recieves this pointer is responsible for deleting it.
        FUNCTION_EXIT;
        //      return alarms[0];

        // Return the pointer. And set the m_isNew = false for updating
        return new AlarmData(key);
        // TD17632
    }

    std::vector<IAlarmData*> AlarmAccessFactory::getActiveAlarms()
    {
        FUNCTION_ENTRY("getActiveAlarms");

        /*std::ostringstream sql;
        sql << "select a.ALARM_ID,TO_CHAR(a.SOURCETIME,'YYYYMMDDHH24MISSFF3'), TO_CHAR(a.TRANSACTIVETIME,'YYYYMMDDHH24MISSFF3'), "
            << "a.PARAM_LIST,a.SEVERITY,a.TYPE,a.CONTEXT,a.STATE,a.MMS_STATE_TYPE, a.ACKNOWLEDGED_BY,a.ENTITYKEY,a.COMMENTS,a.ASSET_NAME, "
            << "a.PLANSTATUS, a.AVL_HEAD_ID, a.AVL_STATUS, a.IS_UNIQUE, TO_CHAR(a.ACK_TIME,'YYYYMMDDHH24MISS'), TO_CHAR(a.CLOSE_TIME,'YYYYMMDDHH24MISS'), e.SUBSYSTEMKEY, e.PHYSICAL_SUBSYSTEM_KEY, "
            << "e.LOCATIONKEY, e.TYPEKEY,  a.OPERATOR_NAME "
            << "from ALARM a, ENTITY e "
            << "where (a.STATE = "
            <<  AlarmData::OPEN
            << " or a.ACKNOWLEDGED_BY IS NULL) "
            << "AND e.PKEY = a.ENTITYKEY ";*/

        // get a connection to the database and retrieve the DB type
        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Alarm_Sd, Read);
        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ALARM_Oracle_SELECT_3551, AlarmData::OPEN);
        //      std::string sql  = databaseConnection->prepareSQLStatement(ALARM_SELECT_3551, AlarmData::OPEN);
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ALARM_SELECT_3551, AlarmData::OPEN);
        //      TA_ASSERT(sql.size() > 0, "getActiveAlarms: Prepare SQL Statement error");

        std::vector<IAlarmData*> alarms;
        runQueryForAlarms(databaseConnection, sql, alarms);

        FUNCTION_EXIT;
        return alarms;
    }

    std::vector<IAlarmData*> AlarmAccessFactory::getActiveAlarms(ta_uint32 maxCount)
    {
        FUNCTION_ENTRY("getActiveAlarms");

        //      std::ostringstream sql;
        //         sql << "select a.ALARM_ID,TO_CHAR(a.SOURCETIME,'YYYYMMDDHH24MISSFF3'), TO_CHAR(a.TRANSACTIVETIME,'YYYYMMDDHH24MISSFF3'), "
        //             << "a.PARAM_LIST,a.SEVERITY,a.TYPE,a.CONTEXT,a.STATE,a.MMS_STATE_TYPE, a.ACKNOWLEDGED_BY,a.ENTITYKEY,a.COMMENTS,a.ASSET_NAME, "
        //             << "a.PLANSTATUS, a.AVL_HEAD_ID, a.AVL_STATUS, a.IS_UNIQUE, TO_CHAR(a.ACK_TIME,'YYYYMMDDHH24MISS'), TO_CHAR(a.CLOSE_TIME,'YYYYMMDDHH24MISS'), e.SUBSYSTEMKEY, e.PHYSICAL_SUBSYSTEM_KEY, "
        //             << "e.LOCATIONKEY, e.TYPEKEY,  a.OPERATOR_NAME "
        //             << "from ALARM a, ENTITY e "
        //             << "where (a.STATE = "
        //             <<  AlarmData::OPEN
        //             << " or a.ACKNOWLEDGED_BY IS NULL) "
        //             << "AND e.PKEY = a.ENTITYKEY  and rownum <=" << maxCount;

        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Alarm_Sd, Read);

        //      std::string sql  = databaseConnection->prepareSQLStatement(ALARM_SELECT_3554, AlarmData::OPEN, maxCount);
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ALARM_SELECT_3554, AlarmData::OPEN, maxCount);
        //      TA_ASSERT(sql.size() > 0, "getActiveAlarms: Prepare SQL Statement error");

        std::vector<IAlarmData*> alarms;
        runQueryForAlarms(databaseConnection, sql, alarms);

        FUNCTION_EXIT;
        return alarms;
    }

    std::vector<IAlarmData*> AlarmAccessFactory::getActiveAlarmsForLocation(ta_uint32 location)
    {
        FUNCTION_ENTRY("getActiveAlarmsForLocation");

        /*std::ostringstream sql;
        sql << "select a.ALARM_ID,TO_CHAR(a.SOURCETIME,'YYYYMMDDHH24MISSFF3'), TO_CHAR(a.TRANSACTIVETIME,'YYYYMMDDHH24MISSFF3'), "
            << "a.PARAM_LIST,a.SEVERITY,a.TYPE,a.CONTEXT,a.STATE, a.MMS_STATE_TYPE, a.ACKNOWLEDGED_BY,a.ENTITYKEY,a.COMMENTS,a.ASSET_NAME, "
            << "a.PLANSTATUS, a.AVL_HEAD_ID, a.AVL_STATUS, a.IS_UNIQUE, TO_CHAR(a.ACK_TIME,'YYYYMMDDHH24MISS'), TO_CHAR(a.CLOSE_TIME,'YYYYMMDDHH24MISS'), e.SUBSYSTEMKEY, e.PHYSICAL_SUBSYSTEM_KEY, "
            << "e.LOCATIONKEY, e.TYPEKEY, a.OPERATOR_NAME "
            << "from ALARM a, ENTITY e "
            << "where ( (a.STATE = " << AlarmData::OPEN
            << " or a.ACKNOWLEDGED_BY IS NULL) "
            << "and e.LOCATIONKEY = " << location
            << ") AND (e.PKEY = a.ENTITYKEY)";*/

        // get a connection to the database and retrieve the DB type
        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Alarm_Sd, Read);
        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ALARM_Oracle_SELECT_3552,
        //      std::string sql  = databaseConnection->prepareSQLStatement(ALARM_SELECT_3552,
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ALARM_SELECT_3552,
                                                AlarmData::OPEN, location);
        //      TA_ASSERT(sql.size() > 0, "getActiveAlarmsForLocation: Prepare SQL Statement error");

        std::vector<IAlarmData*> alarms;
        runQueryForAlarms(databaseConnection, sql, alarms);

        FUNCTION_EXIT;
        return alarms;
    }

    std::vector<IAlarmData*> AlarmAccessFactory::getActiveAlarmsForLocation(ta_uint32 location, ta_uint32 maxCount)
    {
        FUNCTION_ENTRY("getActiveAlarmsForLocation");

        //      std::ostringstream sql;
        //         sql << "select a.ALARM_ID,TO_CHAR(a.SOURCETIME,'YYYYMMDDHH24MISSFF3'), TO_CHAR(a.TRANSACTIVETIME,'YYYYMMDDHH24MISSFF3'), "
        //             << "a.PARAM_LIST,a.SEVERITY,a.TYPE,a.CONTEXT,a.STATE, a.MMS_STATE_TYPE, a.ACKNOWLEDGED_BY,a.ENTITYKEY,a.COMMENTS,a.ASSET_NAME, "
        //             << "a.PLANSTATUS, a.AVL_HEAD_ID, a.AVL_STATUS, a.IS_UNIQUE, TO_CHAR(a.ACK_TIME,'YYYYMMDDHH24MISS'), TO_CHAR(a.CLOSE_TIME,'YYYYMMDDHH24MISS'), e.SUBSYSTEMKEY, e.PHYSICAL_SUBSYSTEM_KEY, "
        //             << "e.LOCATIONKEY, e.TYPEKEY, a.OPERATOR_NAME "
        //             << "from ALARM a, ENTITY e "
        //             << "where ( (a.STATE = " << AlarmData::OPEN
        //             << " or a.ACKNOWLEDGED_BY IS NULL) "
        //             << "and e.LOCATIONKEY = " << location
        //             << ") AND (e.PKEY = a.ENTITYKEY)  and rownum <=" << maxCount;

        // get a connection to the database and retrieve the DB type
        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Alarm_Sd, Read);
        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ALARM_Oracle_SELECT_3552,
        //      std::string sql  = databaseConnection->prepareSQLStatement(ALARM_SELECT_3555,
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ALARM_SELECT_3555,
                                                AlarmData::OPEN, location, maxCount);
        //      TA_ASSERT(sql.size() > 0, "getActiveAlarmsForLocation: Prepare SQL Statement error");

        std::vector<IAlarmData*> alarms;
        runQueryForAlarms(databaseConnection, sql, alarms);

        FUNCTION_EXIT;
        return alarms;
    }

    /**
     * getDBActiveAlarmCount
     *
     * Returns the quantity of active alarms in the DB
     */
    unsigned int AlarmAccessFactory::getDBActiveAlarmCount()
    {
        unsigned int count(0);

        /*char state[50];

        sprintf(state, "%d", AlarmData::OPEN);

        std::string sql = "select count(ALARM_ID) ";
                    sql+= "from ALARM ";
                    sql+= "where (STATE = ";
                    sql+= state;
                    sql+= " or ACKNOWLEDGED_BY IS NULL) ";*/

        // get a connection to the database
        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Alarm_Sd, Read);
        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ALARM_STD_SELECT_3501, AlarmData::OPEN);
        //      std::string sql  = databaseConnection->prepareSQLStatement(ALARM_SELECT_3501, AlarmData::OPEN);
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ALARM_SELECT_3501, AlarmData::OPEN);
        //      TA_ASSERT(sql.size() > 0, "getDBActiveAlarmCount: Prepare SQL Statement error");

        std::string countCol = "COUNT(a.ALARM_ID)";
        std::vector<std::string> columnNames;
        columnNames.push_back(countCol);

        IData* data = databaseConnection->executeQuery(sql, columnNames);

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        try
        {
            if (1 == data->getNumRows())
            {
                count = data->getIntegerData(0, countCol);
            }

            TA_ASSERT(1 == data->getNumRows(), "There is more than 1 row returned by the count SQL statement");
        }
        catch (TA_Base_Core::DataException&)
        {
            // Clean up the data pointer
            delete data;
            data = NULL;
            throw;
        }

        // clean up the pointer
        delete data;
        data = NULL;

        return count;
    }

    /**
     * getDBActiveAlarmCountForLocation
     *
     * Returns the quantity of active alarms in the DB
     *
     */
    unsigned int AlarmAccessFactory::getDBActiveAlarmCountForLocation(ta_uint32 locationKey)
    {
        unsigned int count(0);

        /*char loc[255];
        char state[50];

        sprintf(state, "%d", AlarmData::OPEN);
        sprintf(loc, "%u", locationKey);

        std::string sql = "select count(ALARM_ID) ";
                    sql+= "from ALARM ";
                    sql+= "where (STATE = ";
                    sql+= state;
                    sql+= " or ACKNOWLEDGED_BY IS NULL) ";
                    sql+= "and entitykey in (select pkey from entity where locationkey = ";
                    sql+= loc;
                    sql+= ")";*/
        // get a connection to the database
        IDatabase* databaseConnection =
            DatabaseFactory::getInstance().getDatabase(Alarm_Sd, Read);
        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ALARM_STD_SELECT_3502,
        //      std::string sql  = databaseConnection->prepareSQLStatement(ALARM_SELECT_3502,
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ALARM_SELECT_3502,
                                                AlarmData::OPEN, locationKey);
        //      TA_ASSERT(sql.size() > 0, "getDBActiveAlarmCountForLocation: Prepare SQL Statement error");

        std::string countCol = "COUNT(a.ALARM_ID)";
        std::vector<std::string> columnNames;
        columnNames.push_back(countCol);

        IData* data = databaseConnection->executeQuery(sql, columnNames);

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        try
        {
            if (1 == data->getNumRows())
            {
                count = data->getIntegerData(0, countCol);
            }

            TA_ASSERT(1 == data->getNumRows(), "There is more than 1 row returned by the count SQL statement");
        }
        catch (TA_Base_Core::DataException&)
        {
            // Clean up the data pointer
            delete data;
            data = NULL;
            throw;
        }

        // clean up the pointer
        delete data;
        data = NULL;

        return count;
    }

    void AlarmAccessFactory::runQueryForAlarms(IDatabase* pDatabase, const SQLStatement& query, std::vector<IAlarmData*>& alarms)
    {
        // get a connection to the database
        //IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Alarm_Sd, Read);
        TA_ASSERT(0 != pDatabase, "the Database connection is null");

        // Set up the columnNames vector to be passed to executeQuery()
        std::string idColumn = "a.ALARM_ID";
        std::string sourceTimeColumn = "a.SOURCETIME";
        std::string transactiveTimeColumn = "a.TRANSACTIVETIME";
        std::string paramColumn = "a.PARAMLIST";
        std::string severityColumn = "a.SEVERITY";
        std::string typeColumn = "a.TYPE";
        std::string contextColumn = "a.CONTEXT";
        std::string stateColumn = "a.STATE";
        std::string mmsStateTypeColumn = "a.MMS_STATE_TYPE";
        std::string acknowledgedColumn = "a.ACKNOWLEDGED_BY";
        std::string entityKeyColumn = "a.ENTITYKEY";
        std::string commentsColumn = "a.COMMENTS";
        std::string assetNameColumn = "a.ASSET_NAME";
        std::string planStatusColumn = "a.PLANSTATUS";
        std::string avlHeadIdColumn = "a.AVL_HEAD_ID";
        std::string avlStatusColumn = "a.AVL_STATUS";
        std::string isUniqueColumn = "a.IS_UNIQUE";
        std::string ackTimeColumn = "a.ACK_TIME";
        std::string closeTimeColumn = "a.CLOSE_TIME";
        std::string subsystemKeyOfAssociatedEntityColumn = "a.SUBSYSTEMKEY";
        std::string locationKeyOfAssociatedEntityColumn = "a.LOCATIONKEY";
        std::string typeOfAssociatedEntityColumn = "a.ENTITYTYPEKEY";
        std::string operatorNameForAcknowledgedBySessionKeyColumn = "a.OPERATOR_NAME";
        std::vector<std::string> columnNames;
        columnNames.push_back(idColumn);
        columnNames.push_back(sourceTimeColumn);
        columnNames.push_back(transactiveTimeColumn);
        columnNames.push_back(paramColumn);
        columnNames.push_back(severityColumn);
        columnNames.push_back(typeColumn);
        columnNames.push_back(contextColumn);
        columnNames.push_back(stateColumn);
        columnNames.push_back(mmsStateTypeColumn);
        columnNames.push_back(acknowledgedColumn);
        columnNames.push_back(entityKeyColumn);
        columnNames.push_back(commentsColumn);
        columnNames.push_back(assetNameColumn);
        columnNames.push_back(planStatusColumn);
        columnNames.push_back(avlHeadIdColumn);
        columnNames.push_back(avlStatusColumn);
        columnNames.push_back(isUniqueColumn);
        columnNames.push_back(ackTimeColumn);
        columnNames.push_back(closeTimeColumn);
        columnNames.push_back(subsystemKeyOfAssociatedEntityColumn);
        columnNames.push_back(locationKeyOfAssociatedEntityColumn);
        columnNames.push_back(typeOfAssociatedEntityColumn);
        columnNames.push_back(operatorNameForAcknowledgedBySessionKeyColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = pDatabase->executeQuery(query, columnNames);

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        if (NULL == data || 0 == data->getNumRows())   // No entry found with the specified alarm_id
        {
            // clean up the pointer
            delete data;
            data = NULL;

            return;
        }

        // Loop for each row returned
        do
        {
            for (ta_uint32 i = 0; i < data->getNumRows(); i++)
            {
                try
                {
                    alarms.push_back(new AlarmData(
                                         data->getStringData(i, idColumn),
                                         //data->getStringData(i, descriptionColumn),
                                         data->getStringData(i, paramColumn),
                                         data->getTimestampData(i, sourceTimeColumn),
                                         data->getTimestampData(i, transactiveTimeColumn),
                                         data->getDateData(i, ackTimeColumn),
                                         data->getDateData(i, closeTimeColumn),
                                         data->getUnsignedLongData(i, severityColumn),
                                         data->getUnsignedLongData(i, typeColumn),
                                         data->getStringData(i, contextColumn),
                                         static_cast<TA_Base_Core::IAlarmData::EAlarmState>(data->getIntegerData(i, stateColumn)),
                                         static_cast<TA_Base_Core::IAlarmData::EMmsState>(data->getIntegerData(i, mmsStateTypeColumn)),
                                         data->getStringData(i, acknowledgedColumn),
                                         data->getUnsignedLongData(i, entityKeyColumn),
                                         data->getStringData(i, commentsColumn),
                                         data->getStringData(i, assetNameColumn),
                                         data->getStringData(i, planStatusColumn),
                                         data->getUnsignedLongData(i, subsystemKeyOfAssociatedEntityColumn),
                                         data->getUnsignedLongData(i, locationKeyOfAssociatedEntityColumn),
                                         //data->getStringData(i, nameOfAssociatedEntityColumn),
                                         data->getUnsignedLongData(i, typeOfAssociatedEntityColumn),
                                         data->getStringData(i, operatorNameForAcknowledgedBySessionKeyColumn),
                                         data->getBooleanData(i, isUniqueColumn),
                                         data->getStringData(i, avlHeadIdColumn),
                                         static_cast<TA_Base_Core::IAlarmData::EAlarmAVLStatus>(data->getIntegerData(i, avlStatusColumn))));
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
        }
        while (pDatabase->moreData(data));
    }

    IAlarmData* AlarmAccessFactory::createAlarm()
    {
        return new AlarmData();
    }

	AlarmTopLocDataList AlarmAccessFactory::getTop10Alarms(const time_t dateFrom, const time_t dateTo, const TopAlarmFilter& filter)
	{
		FUNCTION_ENTRY("getTop10Alarms");
		AlarmTopLocDataList retVal;

		// get a connection to the database
		IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Alarm_Sd, Read);

		// Convert time data into formatted strings
		struct tm dateFromStruct;
		char dateFromString[24] = { 0 };

		LOGBEFORECOTS("ACE_OS::localtime_r");
		if (ACE_OS::localtime_r(&dateFrom, &dateFromStruct) != NULL)
		{
			sprintf(dateFromString, "%02d/%02d/%04d", dateFromStruct.tm_mday, dateFromStruct.tm_mon + 1,
				dateFromStruct.tm_year + 1900);
		}

		struct tm dateToStruct;
		char dateToString[24] = { 0 };

		LOGBEFORECOTS("ACE_OS::localtime_r");
		if (ACE_OS::localtime_r(&dateTo, &dateToStruct) != NULL)
		{
			sprintf(dateToString, "%02d/%02d/%04d", dateToStruct.tm_mday, dateToStruct.tm_mon + 1,
				dateToStruct.tm_year + 1900);
		}
		
		std::string strFilter = buildTop10Filter(filter);

		// Build the query statement
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, ALARM_SELECT_3556, dateFromString, dateToString, strFilter);
		
		std::string locationCol = "locationkey";
		std::string entitykeyCol = "entitykey";
		std::string alarmTypeCol = "type";
		std::string alarmSevCol = "severity";
		std::string paramListCol = "param_list";
		std::string countCol = "count";

		std::vector<std::string> columnNames;
		columnNames.push_back(locationCol);
		columnNames.push_back(entitykeyCol);
		columnNames.push_back(alarmTypeCol);
		columnNames.push_back(alarmSevCol);
		columnNames.push_back(paramListCol);
		columnNames.push_back(countCol);

		IData* data = databaseConnection->executeQuery(sql, columnNames);

		// Need to bring in the DataException
		using TA_Base_Core::DataException;

		try
		{
			for (int idx = 0; idx < data->getNumRows(); idx++)
			{
				AlarmTopLocDataPtr pData(new AlarmTopLocData);
				pData->locationKey = data->getUnsignedLongData(idx, locationCol);
				pData->entityKey = data->getUnsignedLongData(idx, entitykeyCol);
				pData->alarmType = data->getUnsignedLongData(idx, alarmTypeCol);
				pData->alarmSeverity = data->getUnsignedLongData(idx, alarmSevCol);
				pData->paramList = data->getStringData(idx, paramListCol);
				pData->alarmcount = data->getUnsignedLongData(idx, countCol);

				retVal.push_back(pData);
			}

		}
		catch (TA_Base_Core::DataException&)
		{
			// Clean up the data pointer
			delete data;
			data = NULL;
			throw;
		}

		// clean up the pointer
		delete data;
		data = NULL;

		return retVal;
	}

	std::string AlarmAccessFactory::buildTop10Filter(const TopAlarmFilter& filter)
	{
		std::stringstream ss;

		if (filter.locations.size() != 0)
		{ // Add location Filter
			ss << " and locationkey in (";

			bool bFirst = true;
			for (ta_uint16 idx = 0; idx < filter.locations.size(); idx++)
			{
				if (!bFirst)
					ss << ", ";
				
				ss << filter.locations[idx];
				bFirst = false;
			}

			ss << ") ";
		}

		if (filter.alarmtypes.size() != 0)
		{ // Add location Filter
			ss << " and type in (";

			bool bFirst = true;
			for (ta_uint16 idx = 0; idx < filter.alarmtypes.size(); idx++)
			{
				if (!bFirst)
					ss << ", ";

				ss << filter.alarmtypes[idx];
				bFirst = false;
			}

			ss << ") ";
		}

		if (filter.subsystems.size() != 0)
		{ // Add location Filter
			ss << " and subsystemkey in (";

			bool bFirst = true;
			for (ta_uint16 idx = 0; idx < filter.subsystems.size(); idx++)
			{
				if (!bFirst)
					ss << ", ";

				ss << filter.subsystems[idx];
				bFirst = false;
			}

			ss << ") ";
		}

		if (filter.severities.size() != 0)
		{ // Add location Filter
			ss << " and severity in (";

			bool bFirst = true;
			for (ta_uint16 idx = 0; idx < filter.severities.size(); idx++)
			{
				if (!bFirst)
					ss << ", ";

				ss << filter.severities[idx];
				bFirst = false;
			}

			ss << ") ";
		}

		return ss.str();
	}

		
	AlarMadbData AlarmAccessFactory::getMadbDataById(std::string madbID)
	{
		FUNCTION_ENTRY("getMadbDataById");

		AlarMadbData res;
		res.pkey = atol(madbID.c_str());

		IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(Madb_SCd, Read);
		if (databaseConnection == 0) 
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unable to get database connection!");
			return res;
		}

		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, ALARM_SELECT_3558, madbID);

		std::string cause = "cause";
		std::string conseqNoAction = "conseq_no_action";
		std::string correctiveAction = "corrective_action";

		std::vector<std::string> columnNames;
		columnNames.push_back(cause);
		columnNames.push_back(conseqNoAction);
		columnNames.push_back(correctiveAction);

		IData* data = databaseConnection->executeQuery(sql, columnNames);

		if (NULL == data || 0 == data->getNumRows())   // No entry found with the specified alarm_id
		{
			// clean up the pointer
			delete data;
			data = NULL;

			return res;
		}

		// Loop for each row returned
		do
		{
			for (ta_uint32 i = 0; i < data->getNumRows(); i++)
			{
				try
				{
					res.cause = data->getStringData(i, cause);
					res.conseq_no_action = data->getStringData(i, conseqNoAction);
					res.corrective_action = data->getStringData(i, correctiveAction);
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
		return res;
	}

} // closes TA_Base_Core
