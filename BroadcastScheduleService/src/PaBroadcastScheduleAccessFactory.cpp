/**
  * PaBroadcastScheduleAccessFactory database implementation.
  */

#pragma warning(disable:4786 4290)

//#include "core/types/src/ta_types.h"
#include "PaBroadcastScheduleAccessFactory.h"
//#include "core/data_access_interface/src/IData.h"
//#include "core/data_access_interface/src/DatabaseFactory.h"
//#include "core/data_access_interface/src/SQLCode.h"
//#include "core/exceptions/src/DataException.h"
//#include "core/synchronisation/src/ThreadGuard.h"

#include <memory>
#include <sstream>

namespace TA_IRS_Core
{
    PaBroadcastScheduleAccessFactory* PaBroadcastScheduleAccessFactory::s_instance = NULL;
    //TA_Base_Core::NonReEntrantThreadLockable PaBroadcastScheduleAccessFactory::s_singletonLock;

    static const std::string SCHEDULE_ID_COL = "SCHEDULE_ID";
    static const std::string LOCATION_ID_COL = "LOCATION_ID";
    static const std::string SCHEDULE_NAME_COL = "SCHEDULE_NAME";
    static const std::string MSG_ID_COL = "MSG_ID";
    static const std::string MSG_VERSION_COL = "MSG_VERSION";
    static const std::string STATIONS_COL = "STATIONS";
    static const std::string ZONES_COL = "ZONES";
    static const std::string SEAT_ID_COL = "SEAT_ID";
    static const std::string LANGUAGE_COL = "LANGUAGE";
    static const std::string PLAY_COUNT_COL = "PLAY_COUNT";
    static const std::string PLAY_INTERVAL_COL = "PLAY_INTERVAL_SEC";
    static const std::string SCHEDULE_TYPE_COL = "SCHEDULE_TYPE";
    static const std::string START_AT_COL = "START_AT";
    static const std::string REPEAT_INTERVAL_COL = "REPEAT_INTERVAL";
    static const std::string WEEKDAY_MASK_COL = "WEEKDAY_MASK";
    static const std::string NEXT_RUN_AT_COL = "NEXT_RUN_AT";
    namespace
    {
        std::string getStatusName(const pa_scheduler::ExecutionStatus status)
        {
            switch (status)
            {
            case pa_scheduler::ExecutionStatus::Success:       return "SUCCESS";
            case pa_scheduler::ExecutionStatus::PartialFailed: return "PARTIAL_FAILED";
            case pa_scheduler::ExecutionStatus::Cancelled:     return "CANCELLED";
            default:                                            return "FAILED";
            }
        }

        std::vector<std::string> getScheduleColumnNames()
        {
            std::vector<std::string> columnNames;
            columnNames.push_back(SCHEDULE_ID_COL);
            columnNames.push_back(LOCATION_ID_COL);
            columnNames.push_back(SCHEDULE_NAME_COL);
            columnNames.push_back(MSG_ID_COL);
            columnNames.push_back(MSG_VERSION_COL);
            columnNames.push_back(STATIONS_COL);
            columnNames.push_back(ZONES_COL);
            columnNames.push_back(SEAT_ID_COL);
            columnNames.push_back(LANGUAGE_COL);
            columnNames.push_back(PLAY_COUNT_COL);
            columnNames.push_back(PLAY_INTERVAL_COL);
            columnNames.push_back(SCHEDULE_TYPE_COL);
            columnNames.push_back(START_AT_COL);
            columnNames.push_back(REPEAT_INTERVAL_COL);
            columnNames.push_back(WEEKDAY_MASK_COL);
            columnNames.push_back(NEXT_RUN_AT_COL);
            return columnNames;
        }

        //long getLastInsertId(TA_Base_Core::IDatabase* databaseConnection)
        //{
        //    std::vector<std::string> columnNames;
        //    columnNames.push_back("RUN_ID");
        //    TA_Base_Core::SQLStatement sql;
        //    databaseConnection->prepareSQLStatement(sql, PABROADCASTSCHEDULELOG_SELECT_90005);
        //    std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery(sql, columnNames));
        //    if (data.get() == 0 || data->getNumRows() != 1)
        //    {
        //        TA_THROW(TA_Base_Core::DataException("Unable to obtain PA broadcast schedule log key", TA_Base_Core::DataException::NO_VALUE, "RUN_ID"));
        //    }
        //    return data->getUnsignedLongData(0, "RUN_ID");
        //}
    }

    PaBroadcastScheduleAccessFactory& PaBroadcastScheduleAccessFactory::getInstance()
    {
        //TA_THREADGUARD(s_singletonLock);

        if (s_instance == NULL)
        {
            s_instance = new PaBroadcastScheduleAccessFactory();
        }
        return *s_instance;
    }

    void PaBroadcastScheduleAccessFactory::removeInstance()
    {
        //TA_THREADGUARD(s_singletonLock);

        if (s_instance != NULL)
        {
            delete s_instance;
            s_instance = NULL;
        }
    }

    std::vector<pa_scheduler::BroadcastSchedule> PaBroadcastScheduleAccessFactory::getDuePaBroadcastSchedules(
        const int locationKey,
        const int maximumCount,
        const std::function<std::string(const pa_scheduler::BroadcastSchedule&)>& calculateNextRun)
    {
		std::vector<pa_scheduler::BroadcastSchedule> schedules;

		pa_scheduler::BroadcastSchedule schedule;
		schedule.scheduleId = 1;
        schedule.locationId = 1;
        schedule.scheduleName = "s-test-1";
        schedule.messageId = 560;
        schedule.messageVersion = "200401010000";
        schedule.stations = "1,2";
        schedule.zones = 8;
        schedule.seatId = 1;
        schedule.language = 1;
        schedule.playCount = 1;
        schedule.playIntervalSeconds = 0;
        schedule.scheduleType = "DAILY";
        schedule.startAt = "2026-08-27 18:20:06.000";
        schedule.repeatInterval = 1;
        schedule.weekdayMask = 0;
        schedule.plannedAt = "2026-08-27 18:20:06.000";
        schedules.push_back(schedule);

		return schedules;
        //TA_Base_Core::IDatabase* databaseConnection =
        //    TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Pa_Cd, TA_Base_Core::Write);

        //std::vector<std::string> columnNames = getScheduleColumnNames();
        //TA_Base_Core::SQLStatement beginTransactionSql;
        //databaseConnection->prepareSQLStatement(beginTransactionSql, PABROADCASTSCHEDULE_BEGIN_TRANSACTION_90000);
        //databaseConnection->executeModification(beginTransactionSql);
        //try
        //{
        //    TA_Base_Core::SQLStatement selectSql;
        //    databaseConnection->prepareSQLStatement(selectSql, PABROADCASTSCHEDULE_SELECT_90001, locationKey, maximumCount);
        //    std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery(selectSql, columnNames));
        //    std::vector<pa_scheduler::BroadcastSchedule> schedules;
        //    for (ta_uint32 row = 0; row < data->getNumRows(); ++row)
        //    {
        //        pa_scheduler::BroadcastSchedule schedule;
        //        schedule.scheduleId = data->getUnsignedLongData(row, SCHEDULE_ID_COL);
        //        schedule.locationId = data->getUnsignedLongData(row, LOCATION_ID_COL);
        //        schedule.scheduleName = data->getStringData(row, SCHEDULE_NAME_COL);
        //        schedule.messageId = data->getUnsignedLongData(row, MSG_ID_COL);
        //        schedule.messageVersion = data->getStringData(row, MSG_VERSION_COL);
        //        schedule.stations = data->getStringData(row, STATIONS_COL);
        //        schedule.zones = data->getUnsignedLongData(row, ZONES_COL);
        //        schedule.seatId = data->getUnsignedLongData(row, SEAT_ID_COL);
        //        schedule.language = data->getUnsignedLongData(row, LANGUAGE_COL);
        //        schedule.playCount = data->getUnsignedLongData(row, PLAY_COUNT_COL);
        //        schedule.playIntervalSeconds = data->getUnsignedLongData(row, PLAY_INTERVAL_COL);
        //        schedule.scheduleType = data->getStringData(row, SCHEDULE_TYPE_COL);
        //        schedule.startAt = data->getStringData(row, START_AT_COL);
        //        schedule.repeatInterval = data->getUnsignedLongData(row, REPEAT_INTERVAL_COL);
        //        schedule.weekdayMask = data->isNull(row, WEEKDAY_MASK_COL) ? 0 : data->getUnsignedLongData(row, WEEKDAY_MASK_COL);
        //        schedule.plannedAt = data->getStringData(row, NEXT_RUN_AT_COL);
        //        schedules.push_back(schedule);
        //    }

        //    for (std::vector<pa_scheduler::BroadcastSchedule>::iterator schedule = schedules.begin(); schedule != schedules.end(); ++schedule)
        //    {
        //        const std::string nextRun = calculateNextRun(*schedule);
        //        TA_Base_Core::SQLStatement updateSql;
        //        databaseConnection->prepareSQLStatement(updateSql, PABROADCASTSCHEDULE_UPDATE_90002,
        //            databaseConnection->escapeInsertString(nextRun), schedule->scheduleId);
        //        databaseConnection->executeModification(updateSql);

        //        TA_Base_Core::SQLStatement insertSql;
        //        databaseConnection->prepareSQLStatement(insertSql, PABROADCASTSCHEDULELOG_INSERT_90003,
        //            schedule->scheduleId,
        //            databaseConnection->escapeInsertString(schedule->plannedAt),
        //            schedule->messageId,
        //            databaseConnection->escapeInsertString(schedule->messageVersion),
        //            databaseConnection->escapeInsertString(schedule->stations),
        //            schedule->zones);
        //        databaseConnection->executeModification(insertSql);
        //        schedule->runId = getLastInsertId(databaseConnection);
        //    }

        //    TA_Base_Core::SQLStatement commitSql;
        //    databaseConnection->prepareSQLStatement(commitSql, PABROADCASTSCHEDULE_COMMIT_90006);
        //    databaseConnection->executeModification(commitSql);
        //    return schedules;
        //}
        //catch (...)
        //{
        //    try
        //    {
        //        TA_Base_Core::SQLStatement rollbackSql;
        //        databaseConnection->prepareSQLStatement(rollbackSql, PABROADCASTSCHEDULE_ROLLBACK_90007);
        //        databaseConnection->executeModification(rollbackSql);
        //    }
        //    catch (...) { }
        //    throw;
        //}
    }

    void PaBroadcastScheduleAccessFactory::updatePaBroadcastScheduleLog(
        const long runId,
        const pa_scheduler::ExecutionResult& execution)
    {
        //TA_Base_Core::IDatabase* databaseConnection =
        //    TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Pa_Cd, TA_Base_Core::Write);

        //TA_Base_Core::SQLStatement sql;
        //databaseConnection->prepareSQLStatement(sql, PABROADCASTSCHEDULELOG_UPDATE_90004,
        //    databaseConnection->escapeInsertString(getStatusName(execution.status)),
        //    execution.announceId,
        //    databaseConnection->escapeInsertString(execution.errorCode),
        //    databaseConnection->escapeInsertString(execution.errorMessage),
        //    runId);
        //databaseConnection->executeModification(sql);
    }

} // closes TA_IRS_Core
