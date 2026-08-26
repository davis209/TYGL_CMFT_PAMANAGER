
#include "core/data_access_interface/src/EventData.h"

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

#include <sstream>
#include "ace/OS.h"
#include <time.h>
namespace TA_Base_Core
{
    EventData::EventData(ICombinedEventDataPtr pEvCombined, bool bPropagate /*= false*/)
        : m_pEvCombined(pEvCombined)
        , m_bPropagate(bPropagate)
    {
        if (!m_bPropagate)
        { m_bPropagate = pEvCombined->getIsSystemEvent(); }
    }

    EventData::~EventData()
    {
    }

    void EventData::applyChanges()
    {
        FUNCTION_ENTRY("applyChanges");
        // Now we can get on with writing the entity to the database.

        // Convert time data into formatted strings
        struct tm createTimeStruct;
        // Note for code review: Which one of these should we use? Currently getting
        // the local time. Could get GMT instead.

        timeb createTime = m_pEvCombined->getCreateTime();

        // extract the time from the struct and make it into a string
        char sourceTimeString[24] = { 0 };
        LOGBEFORECOTS("ACE_OS::localtime_r");

        if (ACE_OS::localtime_r(&createTime.time, &createTimeStruct) != NULL)
        {
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

        SQLStatement formatSQL;

        if (m_bPropagate)
        {
            databaseConnection->prepareSQLStatement(formatSQL, EV_COMBINED_INSERT_14004, getLocalDatabaseName(),
                                                    m_pEvCombined->getSourceTable(), databaseConnection->escapeAQSQLString(createDateString), databaseConnection->escapeAQSQLString(sourceTimeString),
                                                    m_pEvCombined->getSubsystemKey(), m_pEvCombined->getPhysicalSubsystemKey(), m_pEvCombined->getLocationKey(), m_pEvCombined->getSeverityKey(), databaseConnection->escapeAQSQLString(m_pEvCombined->getAlarmSeverityName()), databaseConnection->escapeAQSQLString(m_pEvCombined->getAssetName()),
                                                    databaseConnection->escapeAQSQLString(m_pEvCombined->getDescription()), m_pEvCombined->getEventTypeKey(), databaseConnection->escapeAQSQLString(m_pEvCombined->getEventTypeName()), databaseConnection->escapeAQSQLString(m_pEvCombined->getAlarmId()), m_pEvCombined->getAlarmTypeKey(),
                                                    databaseConnection->escapeAQSQLString(m_pEvCombined->getValue()), databaseConnection->escapeAQSQLString(m_pEvCombined->getAlarmMmsState()), databaseConnection->escapeAQSQLString(m_pEvCombined->getAlarmDssState()), databaseConnection->escapeAQSQLString(m_pEvCombined->getAlarmAvlState()),
                                                    m_pEvCombined->getOperatorKey(), databaseConnection->escapeAQSQLString(m_pEvCombined->getOperatorName()), databaseConnection->escapeAQSQLString(m_pEvCombined->getAlarmComment()), m_pEvCombined->getEventLevel(), databaseConnection->escapeAQSQLString(m_pEvCombined->getAlarmAck()),
                                                    databaseConnection->escapeAQSQLString(m_pEvCombined->getAlarmStatus()), databaseConnection->escapeAQSQLString(m_pEvCombined->getSessionKey()), m_pEvCombined->getSessionLocation(), m_pEvCombined->getProfileId(), m_pEvCombined->getActionId(), databaseConnection->escapeAQSQLString(m_pEvCombined->getOperationMode()),
                                                    m_pEvCombined->getEntityKey(), databaseConnection->escapeAQSQLString(m_pEvCombined->getAvlAlarmHeadId()), m_pEvCombined->getSystemKey(), databaseConnection->escapeAQSQLString(m_pEvCombined->getEventID()));
        }
        else
        {
            databaseConnection->prepareSQLStatement(formatSQL, EV_COMBINED_INSERT_14001,
                                                    m_pEvCombined->getSourceTable(), std::string(createDateString), std::string(sourceTimeString),
                                                    m_pEvCombined->getSubsystemKey(), m_pEvCombined->getPhysicalSubsystemKey(), m_pEvCombined->getLocationKey(), m_pEvCombined->getSeverityKey(), m_pEvCombined->getAlarmSeverityName(), databaseConnection->escapeInsertString(m_pEvCombined->getAssetName()),
                                                    databaseConnection->escapeInsertString(m_pEvCombined->getDescription()), m_pEvCombined->getEventTypeKey(), databaseConnection->escapeInsertString(m_pEvCombined->getEventTypeName()), m_pEvCombined->getAlarmId(), m_pEvCombined->getAlarmTypeKey(),
                                                    databaseConnection->escapeInsertString(m_pEvCombined->getValue()), m_pEvCombined->getAlarmMmsState(), m_pEvCombined->getAlarmDssState(), m_pEvCombined->getAlarmAvlState(),
                                                    m_pEvCombined->getOperatorKey(), databaseConnection->escapeInsertString(m_pEvCombined->getOperatorName()), databaseConnection->escapeInsertString(m_pEvCombined->getAlarmComment()), m_pEvCombined->getEventLevel(), m_pEvCombined->getAlarmAck(),
                                                    m_pEvCombined->getAlarmStatus(), m_pEvCombined->getSessionKey(), m_pEvCombined->getSessionLocation(), m_pEvCombined->getProfileId(), m_pEvCombined->getActionId(), m_pEvCombined->getOperationMode(),
                                                    m_pEvCombined->getEntityKey(), m_pEvCombined->getAvlAlarmHeadId(), m_pEvCombined->getSystemKey(), m_pEvCombined->getEventID());
        }

        databaseConnection->executeModification(formatSQL);

        FUNCTION_EXIT;
    }

    std::string EventData::getLocalDatabaseName()
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

} //!TA_Base_Core