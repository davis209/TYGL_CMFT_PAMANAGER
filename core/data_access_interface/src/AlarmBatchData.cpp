
#include "core/data_access_interface/src/AlarmBatchData.h"
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
    AlarmBatchData::AlarmBatchData()
    {
    }

    AlarmBatchData::~AlarmBatchData()
    {
        m_dataList.clear();
    }

    bool AlarmBatchData::addData(IAlarmDataPtr pAlarmData)
    {
        FUNCTION_ENTRY("addData");
        T_AlarmDataMap::iterator itr = m_dataList.find(pAlarmData->getKey());

        if (itr != m_dataList.end())
        { return false; }

        m_dataList.insert(T_AlarmDataMap::value_type(pAlarmData->getKey(), pAlarmData));
        FUNCTION_EXIT;
        return true;
    }

    bool AlarmBatchData::checkDuplicate(const std::string& alarmID)
    {
        FUNCTION_ENTRY("checkDuplicate");
        T_AlarmDataMap::iterator itr = m_dataList.find(alarmID);

        if (itr != m_dataList.end())
        { return true; }

        FUNCTION_EXIT;
        return false;
    }

    void AlarmBatchData::applyChanges()
    {
        FUNCTION_ENTRY("applyChanges");

        // Now we can get on with writing the entity to the database.
        // Get the database connection
        IDatabase* databaseConnection =
            DatabaseFactory::getInstance().getDatabase(Alarm_Sd, Write);

        LOG_DEBUG("Preparing to insert Alarm Batch data with size %d", m_dataList.size());
        std::string strQracle;
        std::string strMySQL;
        bool bFirst = true;
        T_AlarmDataMap::iterator itr = m_dataList.begin();

        for (itr; itr != m_dataList.end(); itr++)
        {
            IAlarmDataPtr pData = itr->second;
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
                    strQracle += " UNION " + tempStmt.strOracle;
                    strMySQL += ", " + tempStmt.strMySQL;
                }
            }
        }

        SQLStatement formatSQL;
        databaseConnection->prepareSQLStatement(formatSQL, ALARM_INSERT_3854);

        boost::format fmtOracle = boost::format(formatSQL.strOracle) % strQracle;
        boost::format fmtMySQL = boost::format(formatSQL.strMySQL) % strMySQL;

        formatSQL.strOracle = fmtOracle.str();
        formatSQL.strMySQL = fmtMySQL.str();

        databaseConnection->executeModification(formatSQL);

        FUNCTION_EXIT;
    }

    bool AlarmBatchData::prepareInsertStmt(IAlarmDataPtr pData, SQLStatement& outStmt)
    {
        FUNCTION_ENTRY("prepareInsertStmt");
        bool bRetVal = false;

        try
        {
            // extract the time from the struct and make it into a string
            struct tm sourceTimeStruct;
            char sourceTimeString[24] = { 0 };

            LOGBEFORECOTS("ACE_OS::localtime_r");

            timeb sourcetmb = pData->getSourceTime();

            if (ACE_OS::localtime_r(&sourcetmb.time, &sourceTimeStruct) != NULL)
            {
                sprintf(sourceTimeString, "%02d/%02d/%04d %02d:%02d:%02d.%03d", sourceTimeStruct.tm_mday, sourceTimeStruct.tm_mon + 1,
                        sourceTimeStruct.tm_year + 1900, sourceTimeStruct.tm_hour, sourceTimeStruct.tm_min, sourceTimeStruct.tm_sec,
                        sourcetmb.millitm);
            }

            LOGAFTERCOTS("ACE_OS::localtime_r");
            // do it again with the transactiveTime
            struct tm transactiveTimeStruct;
            char transactiveTimeString[24] = { 0 };
            timeb transactivetmb = pData->getTransactiveTime();

            if (ACE_OS::localtime_r(&transactivetmb.time, &transactiveTimeStruct) != NULL)
            {
                sprintf(transactiveTimeString, "%02d/%02d/%04d %02d:%02d:%02d.%03d", transactiveTimeStruct.tm_mday, transactiveTimeStruct.tm_mon + 1,
                        transactiveTimeStruct.tm_year + 1900, transactiveTimeStruct.tm_hour, transactiveTimeStruct.tm_min, transactiveTimeStruct.tm_sec,
                        transactivetmb.millitm);
            }

            struct tm ackTimeStruct;

            char ackTimeString[24] = { 0 };

            time_t acktime = pData->getAckTime();

            if (ACE_OS::localtime_r(&acktime, &ackTimeStruct) != NULL)
            {
                sprintf(ackTimeString, "%02d/%02d/%04d %02d:%02d:%02d",
                        ackTimeStruct.tm_mday,
                        ackTimeStruct.tm_mon + 1,
                        ackTimeStruct.tm_year + 1900,
                        ackTimeStruct.tm_hour,
                        ackTimeStruct.tm_min,
                        ackTimeStruct.tm_sec);
            }

            struct tm closeTimeStruct;

            char closeTimeString[24] = { 0 };

            time_t closetime = pData->getCloseTime();

            if (ACE_OS::localtime_r(&closetime, &closeTimeStruct) != NULL)
            {
                sprintf(closeTimeString, "%02d/%02d/%04d %02d:%02d:%02d",
                        closeTimeStruct.tm_mday,
                        closeTimeStruct.tm_mon + 1,
                        closeTimeStruct.tm_year + 1900,
                        closeTimeStruct.tm_hour,
                        closeTimeStruct.tm_min,
                        closeTimeStruct.tm_sec);
            }

            // Get the database connection
            IDatabase* databaseConnection =
                DatabaseFactory::getInstance().getDatabase(Alarm_Sd, Write);

            databaseConnection->prepareSQLStatement(outStmt, ALARM_INSERT_3853,
                                                    pData->getKey(), std::string(sourceTimeString), std::string(transactiveTimeString),
                                                    databaseConnection->escapeInsertString(pData->getParamList()), pData->getSeverity(), pData->getTypeKey(),
                                                    databaseConnection->escapeInsertString(pData->getContext()), pData->getState(), pData->getMmsStateType(),
                                                    databaseConnection->escapeInsertString(pData->getAcknowledgedBy()), pData->getEntityKey(),
                                                    databaseConnection->escapeInsertString(pData->getComments()), databaseConnection->escapeInsertString(pData->getAssetName()),
                                                    databaseConnection->escapeInsertString(pData->getPlanStatus()), databaseConnection->escapeInsertString(pData->getAVLHeadId()),
                                                    pData->getAVLStatus(), pData->getIsUnique(), std::string(ackTimeString), std::string(closeTimeString),
                                                    pData->getTypeOfAssociatedEntity(), pData->getSubsystemKeyOfAssociatedEntity(), pData->getLocationKeyOfAssociatedEntity(), pData->getOperatorName());

            bRetVal = true;
        }
        catch (...)
        {
            LOG_ERROR("Exception occured while formatting data.");
        }

        FUNCTION_EXIT;
        return bRetVal;
    }
} //!TA_Base_Core