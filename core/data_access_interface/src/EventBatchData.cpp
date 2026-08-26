#include "core/data_access_interface/src/EventBatchData.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

#include "boost/format.hpp"

#include "ace/OS.h"
#include <sstream>
#include <time.h>
namespace TA_Base_Core
{
    EventBatchData::EventBatchData()
    {
    }

    EventBatchData::~EventBatchData()
    {
        m_dataList.clear();
    }

    void EventBatchData::addData(ICombinedEventDataPtr pEvCombined)
    {
        m_dataList.push_back(pEvCombined);
    }

    void EventBatchData::applyChanges()
    {
        FUNCTION_ENTRY("applyChanges");
        // Now we can get on with writing the entity to the database.
        // Get the database connection
        IDatabase* databaseConnection =
            DatabaseFactory::getInstance().getDatabase(Event_Ad, Write);

        std::string strQracle;
        std::string strMySQL;
        bool bFirst = true;

        for (int idx = 0; idx < m_dataList.size(); idx++)
        {
            ICombinedEventDataPtr pData = m_dataList[idx];
            SQLStatement tempStmt;

            if (prepareInsertStmt(pData, tempStmt))
            {
                if (bFirst == true)
                {
                    strQracle = tempStmt.strOracle;
                    strMySQL = tempStmt.strMySQL;
                    bFirst = false;
                }
                else
                {
                    strQracle += " " + tempStmt.strOracle;
                    strMySQL += "," + tempStmt.strMySQL;
                }
            }
        }

        SQLStatement formatSQL;
        databaseConnection->prepareSQLStatement(formatSQL, EV_COMBINED_INSERT_14002);


        boost::format fmtOracle = boost::format(formatSQL.strOracle) % strQracle;
        boost::format fmtMySQL = boost::format(formatSQL.strMySQL) % strMySQL;

        formatSQL.strOracle = fmtOracle.str();
        formatSQL.strMySQL = fmtMySQL.str();

        databaseConnection->executeModification(formatSQL);

        FUNCTION_EXIT;
    }

    bool EventBatchData::prepareInsertStmt(ICombinedEventDataPtr pData, SQLStatement& outStmt)
    {
        try
        {
            // Now we can get on with writing the entity to the database.
            // Convert time data into formatted strings
            struct tm createTimeStruct;
            // Note for code review: Which one of these should we use? Currently getting
            // the local time. Could get GMT instead.

            timeb createTime = pData->getCreateTime();

            // extract the time from the struct and make it into a string
            char sourceTimeString[24] = { 0 };
            LOGBEFORECOTS("ACE_OS::localtime_r");

            if (ACE_OS::localtime_r(&createTime.time, &createTimeStruct) != NULL)
            {
				if (createTime.millitm > 1000)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CreateTime to sprintf: Day:%d Mon:%d Year:%0d  Hour:%d Min:%d Sec:%d Milli:%d",
						createTimeStruct.tm_mday, createTimeStruct.tm_mon + 1,
						createTimeStruct.tm_year + 1900, createTimeStruct.tm_hour, createTimeStruct.tm_min, createTimeStruct.tm_sec,
						createTime.millitm);

					LOG3(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid Millisecond Time: %d passed with eventID: %s entitykey: %d", createTime.millitm, pData->getEventID().c_str(), pData->getEntityKey());
					createTime.millitm = createTime.millitm % 1000; // reduce it to 3 digit
				}

                sprintf(sourceTimeString, "%02d/%02d/%04d %02d:%02d:%02d.%03d", createTimeStruct.tm_mday, createTimeStruct.tm_mon + 1,
                        createTimeStruct.tm_year + 1900, createTimeStruct.tm_hour, createTimeStruct.tm_min, createTimeStruct.tm_sec,
                        createTime.millitm);
            }

            LOGAFTERCOTS("ACE_OS::localtime_r");

            // do it again with the transactiveTime
            struct tm createDateStruct;
            char createDateString[24] = { 0 };

            if (ACE_OS::localtime_r(&createTime.time, &createDateStruct) != NULL)
            {
                sprintf(createDateString, "%02d/%02d/%04d %02d:%02d:%02d", createDateStruct.tm_mday, createDateStruct.tm_mon + 1,
                        createDateStruct.tm_year + 1900, createDateStruct.tm_hour, createDateStruct.tm_min, createDateStruct.tm_sec);
            }

            // Get the database connection
            IDatabase* databaseConnection =
                DatabaseFactory::getInstance().getDatabase(Event_Ad, Write);

            databaseConnection->prepareSQLStatement(outStmt, EV_COMBINED_INSERT_14003,
                                                    pData->getSourceTable(), std::string(createDateString), std::string(sourceTimeString),
                                                    pData->getSubsystemKey(), pData->getPhysicalSubsystemKey(), pData->getLocationKey(), pData->getSeverityKey(), pData->getAlarmSeverityName(), databaseConnection->escapeInsertString(pData->getAssetName()),
                                                    databaseConnection->escapeInsertString(pData->getDescription()), pData->getEventTypeKey(), databaseConnection->escapeInsertString(pData->getEventTypeName()), pData->getAlarmId(), pData->getAlarmTypeKey(),
                                                    databaseConnection->escapeInsertString(pData->getValue()), pData->getAlarmMmsState(), pData->getAlarmDssState(), pData->getAlarmAvlState(),
                                                    pData->getOperatorKey(), databaseConnection->escapeInsertString(pData->getOperatorName()), databaseConnection->escapeInsertString(pData->getAlarmComment()), pData->getEventLevel(), pData->getAlarmAck(),
                                                    pData->getAlarmStatus(), pData->getSessionKey(), pData->getSessionLocation(), pData->getProfileId(), pData->getActionId(), pData->getOperationMode(),
                                                    pData->getEntityKey(), pData->getAvlAlarmHeadId(), pData->getSystemKey(), pData->getEventID());
        }
        catch (...)
        {
            LOG_ERROR("EventBatchData::prepareInsertStmt failed.");
            return false;
        }

        return true;
    }

    std::string EventBatchData::getLocalDatabaseName()
    {
        if (m_localDatabase.empty())
        {
            // have to find out what is the local db name
            // this should be the first in the connection list so grab it
            if (!RunParams::getInstance().isSet(RPARAM_DBCONNECTIONFILE))
            {
                throw DatabaseException("DbConnectionFile not set");
            }

            try
            {
                std::string strDbConnFile(RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
                m_localDatabase = DatabaseFactory::getInstance().getDatabaseName(strDbConnFile, Alarm_Sd, Write);
            }
            catch (...) // most likely a file not found error
            {
                throw DatabaseException("Unable to find database connection infomation");
            }
        }

        return m_localDatabase;
    }

} //TA_Base_Core