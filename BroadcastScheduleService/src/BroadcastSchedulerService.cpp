#include "BroadcastSchedulerService.h"
#include "ServiceLogger.h"

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <stdexcept>

#ifdef _WIN32
#include <winsock2.h>
#endif
#include <mysql.h>

namespace pa_scheduler {
namespace {

class MysqlConnection {
public:
    explicit MysqlConnection(const DatabaseConfig& config) {
        connection_ = mysql_init(nullptr);
        if (connection_ == nullptr) throw std::runtime_error("mysql_init failed");
        unsigned int timeout = 5;
        mysql_options(connection_, MYSQL_OPT_CONNECT_TIMEOUT, &timeout);
        if (mysql_real_connect(connection_, config.host.c_str(), config.user.c_str(), config.password.c_str(),
                               config.database.c_str(), config.port, nullptr, 0) == nullptr) {
            const std::string error = mysql_error(connection_);
            mysql_close(connection_);
            connection_ = nullptr;
            throw std::runtime_error("MySQL connection failed: " + error);
        }
    }

    ~MysqlConnection() { if (connection_ != nullptr) mysql_close(connection_); }
    MYSQL* get() const { return connection_; }

private:
    MYSQL* connection_ = nullptr;
};

void Execute(MYSQL* connection, const std::string& sql) {
    if (mysql_query(connection, sql.c_str()) != 0)
        throw std::runtime_error("MySQL query failed: " + std::string(mysql_error(connection)));
}

std::string Escape(MYSQL* connection, const std::string& value) {
    std::string escaped(value.size() * 2 + 1, '\0');
    const auto length = mysql_real_escape_string(connection, &escaped[0], value.c_str(), static_cast<unsigned long>(value.size()));
    escaped.resize(length);
    return escaped;
}

std::string Quote(MYSQL* connection, const std::string& value) { return "'" + Escape(connection, value) + "'"; }

unsigned int ToUInt(const char* value) { return value == nullptr ? 0U : static_cast<unsigned int>(std::strtoul(value, nullptr, 10)); }
std::uint64_t ToUInt64(const char* value) { return value == nullptr ? 0ULL : static_cast<std::uint64_t>(std::strtoull(value, nullptr, 10)); }
std::string Text(const char* value) { return value == nullptr ? std::string() : std::string(value); }

std::tm ParseDateTime(const std::string& value) {
    std::tm parsed = {};
    std::istringstream stream(value.substr(0, 19));
    stream >> std::get_time(&parsed, "%Y-%m-%d %H:%M:%S");
    if (stream.fail()) throw std::runtime_error("Invalid database datetime: " + value);
    parsed.tm_isdst = -1;
    return parsed;
}

std::tm LocalTime(std::time_t value) {
    std::tm result = {};
#ifdef _WIN32
    localtime_s(&result, &value);
#else
    localtime_r(&value, &result);
#endif
    return result;
}

std::string FormatDateTime(std::time_t value) {
    const std::tm local = LocalTime(value);
    std::ostringstream stream;
    stream << std::put_time(&local, "%Y-%m-%d %H:%M:%S");
    return stream.str();
}

std::time_t AddDays(std::time_t from, int days) {
    std::tm local = LocalTime(from);
    local.tm_mday += days;
    local.tm_isdst = -1;
    return std::mktime(&local);
}

int MondayBasedWeekday(std::time_t value) {
    const std::tm local = LocalTime(value);
    return (local.tm_wday + 6) % 7; // Monday=0 ... Sunday=6
}

std::time_t MondayOfWeek(std::time_t value) { return AddDays(value, -MondayBasedWeekday(value)); }

std::string ComputeNextRun(const BroadcastSchedule& schedule, std::time_t now) {
    if (schedule.scheduleType == "ONCE") return std::string();

    std::tm plannedTime = ParseDateTime(schedule.plannedAt);
    std::time_t planned = std::mktime(&plannedTime);
    const unsigned int interval = schedule.repeatInterval == 0 ? 1U : schedule.repeatInterval;
    if (schedule.scheduleType == "DAILY") {
        do { planned = AddDays(planned, static_cast<int>(interval)); } while (planned <= now);
        return FormatDateTime(planned);
    }

    if (schedule.scheduleType == "WEEKLY") {
        std::tm anchorTime = ParseDateTime(schedule.startAt);
        const std::time_t anchor = std::mktime(&anchorTime);
        std::tm candidate = LocalTime(planned);
        const std::tm start = LocalTime(anchor);
        candidate.tm_hour = start.tm_hour;
        candidate.tm_min = start.tm_min;
        candidate.tm_sec = start.tm_sec;
        candidate.tm_isdst = -1;
        std::time_t when = std::mktime(&candidate);
        do {
            when = AddDays(when, 1);
            const auto weeksSinceAnchor = static_cast<unsigned long>((MondayOfWeek(when) - MondayOfWeek(anchor)) / (7 * 24 * 60 * 60));
            const bool selectedDay = (schedule.weekdayMask & (1U << MondayBasedWeekday(when))) != 0;
            if (when > now && selectedDay && weeksSinceAnchor % interval == 0) return FormatDateTime(when);
        } while (when < AddDays(now, 8000));
        throw std::runtime_error("Unable to calculate next weekly run");
    }

    throw std::runtime_error("Unsupported schedule type: " + schedule.scheduleType);
}

std::string StatusSql(ExecutionStatus status) {
    switch (status) {
    case ExecutionStatus::Success: return "SUCCESS";
    case ExecutionStatus::PartialFailed: return "PARTIAL_FAILED";
    case ExecutionStatus::Cancelled: return "CANCELLED";
    default: return "FAILED";
    }
}

std::string ReplaceAll(std::string value, const std::string& from, const std::string& to) {
    std::size_t position = 0;
    while ((position = value.find(from, position)) != std::string::npos) {
        value.replace(position, from.size(), to);
        position += to.size();
    }
    return value;
}

std::string ShellQuote(const std::string& value) {
#ifdef _WIN32
    return "\"" + ReplaceAll(value, "\"", "\\\"") + "\"";
#else
    return "'" + ReplaceAll(value, "'", "'\\''") + "'";
#endif
}

std::string BuildCommand(const std::string& commandTemplate, const BroadcastSchedule& schedule) {
    std::string command = commandTemplate;
    const std::vector<std::pair<std::string, std::string>> values = {
        {"{schedule_id}", std::to_string(schedule.scheduleId)}, {"{msg_id}", std::to_string(schedule.messageId)},
        {"{msg_version}", schedule.messageVersion}, {"{stations}", schedule.stations},
        {"{zones}", std::to_string(schedule.zones)}, {"{seat_id}", std::to_string(schedule.seatId)},
        {"{language}", std::to_string(schedule.language)}, {"{play_count}", std::to_string(schedule.playCount)},
        {"{play_interval_sec}", std::to_string(schedule.playIntervalSeconds)}};
    for (const auto& value : values) command = ReplaceAll(command, value.first, ShellQuote(value.second));
    return command;
}

} // namespace

CommandBroadcastExecutor::CommandBroadcastExecutor(std::string commandTemplate) : commandTemplate_(std::move(commandTemplate)) {}

ExecutionResult CommandBroadcastExecutor::Execute(const BroadcastSchedule& schedule, const std::atomic_bool& stopRequested) {
    if (stopRequested.load()) return {ExecutionStatus::Cancelled, 0, "STOP_REQUESTED", "Service stop requested"};
    if (commandTemplate_.empty()) return {ExecutionStatus::Failed, 0, "NO_COMMAND", "No broadcast command was configured"};
    const int result = std::system(BuildCommand(commandTemplate_, schedule).c_str());
    if (stopRequested.load()) return {ExecutionStatus::Cancelled, 0, "STOP_REQUESTED", "Service stop requested"};
    if (result != 0) return {ExecutionStatus::Failed, 0, "COMMAND_FAILED", "Broadcast command exited with code " + std::to_string(result)};
    return {ExecutionStatus::Success, 0, std::string(), std::string()};
}

BroadcastSchedulerService::BroadcastSchedulerService(SchedulerConfig config, std::shared_ptr<IBroadcastExecutor> executor)
    : config_(std::move(config)), executor_(std::move(executor)) {
    if (config_.locationId <= 0) throw std::invalid_argument("locationId must be greater than zero");
    if (!executor_) throw std::invalid_argument("executor is required");
}

BroadcastSchedulerService::~BroadcastSchedulerService() { Stop(); }

void BroadcastSchedulerService::Start() {
    bool expected = false;
    if (!running_.compare_exchange_strong(expected, true)) return;
    stopRequested_.store(false);
    ServiceLogger::Info("Scheduler worker started");
    worker_ = std::thread(&BroadcastSchedulerService::WorkerLoop, this);
}

void BroadcastSchedulerService::Stop() {
    if (!running_.exchange(false) && !worker_.joinable()) return;
    stopRequested_.store(true);
    ServiceLogger::Info("Scheduler worker stopping");
    if (worker_.joinable()) worker_.join();
    std::vector<std::future<void>> jobs;
    { std::lock_guard<std::mutex> lock(jobsMutex_); jobs.swap(jobs_); }
    for (auto& job : jobs) { try { job.get(); } catch (...) {} }
}

bool BroadcastSchedulerService::IsRunning() const { return running_.load(); }

void BroadcastSchedulerService::WorkerLoop() {
    while (!stopRequested_.load()) {
        try {
            DispatchDueSchedules();
        } catch (const std::exception& error) {
            ServiceLogger::Error("Schedule polling failed: " + std::string(error.what()));
        } catch (...) {
            ServiceLogger::Error("Schedule polling failed: unknown error");
        }
        const auto end = std::chrono::steady_clock::now() + std::chrono::seconds(config_.pollIntervalSeconds);
        while (!stopRequested_.load() && std::chrono::steady_clock::now() < end)
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void BroadcastSchedulerService::DispatchDueSchedules() {
    MysqlConnection database(config_.database);
    MYSQL* connection = database.get();
    Execute(connection, "START TRANSACTION");
    try {
        std::ostringstream query;
        query << "SELECT SCHEDULE_ID,LOCATION_ID,SCHEDULE_NAME,MSG_ID,MSG_VERSION,STATIONS,ZONES,SEAT_ID,LANGUAGE,PLAY_COUNT,PLAY_INTERVAL_SEC,"
              << "SCHEDULE_TYPE,START_AT,REPEAT_INTERVAL,WEEKDAY_MASK,NEXT_RUN_AT FROM pa_broadcast_schedule WHERE LOCATION_ID=" << config_.locationId
              << " AND ENABLED=1 AND NEXT_RUN_AT IS NOT NULL AND NEXT_RUN_AT<=NOW(3) ORDER BY NEXT_RUN_AT LIMIT " << config_.maxDueSchedulesPerPoll << " FOR UPDATE";
        Execute(connection, query.str());
        MYSQL_RES* result = mysql_store_result(connection);
        if (result == nullptr && mysql_field_count(connection) != 0) throw std::runtime_error(mysql_error(connection));

        std::vector<BroadcastSchedule> due;
        MYSQL_ROW row;
        while (result != nullptr && (row = mysql_fetch_row(result)) != nullptr) {
            BroadcastSchedule schedule;
            schedule.scheduleId = ToUInt64(row[0]); schedule.locationId = static_cast<int>(ToUInt(row[1])); schedule.scheduleName = Text(row[2]);
            schedule.messageId = static_cast<int>(ToUInt(row[3])); schedule.messageVersion = Text(row[4]); schedule.stations = Text(row[5]);
            schedule.zones = ToUInt(row[6]); schedule.seatId = static_cast<int>(ToUInt(row[7])); schedule.language = ToUInt(row[8]);
            schedule.playCount = ToUInt(row[9]); schedule.playIntervalSeconds = ToUInt(row[10]); schedule.scheduleType = Text(row[11]);
            schedule.startAt = Text(row[12]); schedule.repeatInterval = ToUInt(row[13]); schedule.weekdayMask = ToUInt(row[14]); schedule.plannedAt = Text(row[15]);
            due.push_back(std::move(schedule));
        }
        if (result != nullptr) mysql_free_result(result);

        if (!due.empty())
            ServiceLogger::Info("Found " + std::to_string(due.size()) + " due schedule(s) for location " + std::to_string(config_.locationId));

        const std::time_t now = std::time(nullptr);
        for (auto& schedule : due) {
            const std::string nextRun = ComputeNextRun(schedule, now);
            std::ostringstream update;
            update << "UPDATE pa_broadcast_schedule SET LAST_RUN_AT=NOW(3),NEXT_RUN_AT="
                   << (nextRun.empty() ? "NULL" : Quote(connection, nextRun)) << ",UPDATED_AT=NOW(3) WHERE SCHEDULE_ID=" << schedule.scheduleId;
            Execute(connection, update.str());

            std::ostringstream log;
            log << "INSERT INTO pa_broadcast_schedule_log (SCHEDULE_ID,PLANNED_AT,STARTED_AT,STATUS,MSG_ID,MSG_VERSION,STATIONS,ZONES,TRIGGERED_BY) VALUES ("
                << schedule.scheduleId << ',' << Quote(connection, schedule.plannedAt) << ",NOW(3),'IN_PROGRESS'," << schedule.messageId << ','
                << Quote(connection, schedule.messageVersion) << ',' << Quote(connection, schedule.stations) << ',' << schedule.zones << ",'SCHEDULER')";
            Execute(connection, log.str());
            schedule.runId = mysql_insert_id(connection);
        }
        Execute(connection, "COMMIT");

        std::lock_guard<std::mutex> lock(jobsMutex_);
        jobs_.erase(std::remove_if(jobs_.begin(), jobs_.end(), [](std::future<void>& job) {
            if (job.wait_for(std::chrono::seconds(0)) != std::future_status::ready) return false;
            job.get(); return true;
        }), jobs_.end());
        for (const auto& schedule : due)
            jobs_.push_back(std::async(std::launch::async, &BroadcastSchedulerService::RunSchedule, this, schedule));
    } catch (...) {
        try { Execute(connection, "ROLLBACK"); } catch (...) {}
        throw;
    }
}

void BroadcastSchedulerService::RunSchedule(BroadcastSchedule schedule) {
    ServiceLogger::Info("Executing schedule " + std::to_string(schedule.scheduleId) + " (" + schedule.scheduleName + "), run " + std::to_string(schedule.runId));
    ExecutionResult execution;
    try { execution = executor_->Execute(schedule, stopRequested_); }
    catch (const std::exception& error) { execution = {ExecutionStatus::Failed, 0, "EXECUTOR_EXCEPTION", error.what()}; }
    catch (...) { execution = {ExecutionStatus::Failed, 0, "EXECUTOR_EXCEPTION", "Unknown executor error"}; }

    const std::string completion = "Schedule " + std::to_string(schedule.scheduleId) + " completed with " + StatusSql(execution.status);
    if (execution.status == ExecutionStatus::Success) ServiceLogger::Info(completion);
    else ServiceLogger::Error(completion + (execution.errorMessage.empty() ? std::string() : ": " + execution.errorMessage));

    try {
        MysqlConnection database(config_.database);
        MYSQL* connection = database.get();
        std::ostringstream update;
        update << "UPDATE pa_broadcast_schedule_log SET FINISHED_AT=NOW(3),STATUS='" << StatusSql(execution.status) << "',ANNOUNCE_ID="
               << (execution.announceId == 0 ? "NULL" : std::to_string(execution.announceId)) << ",ERROR_CODE="
               << (execution.errorCode.empty() ? "NULL" : Quote(connection, execution.errorCode)) << ",ERROR_MESSAGE="
               << (execution.errorMessage.empty() ? "NULL" : Quote(connection, execution.errorMessage)) << " WHERE RUN_ID=" << schedule.runId;
        Execute(connection, update.str());
    } catch (const std::exception& error) {
        ServiceLogger::Error("Unable to update execution log for schedule " + std::to_string(schedule.scheduleId) + ": " + error.what());
    } catch (...) {
        ServiceLogger::Error("Unable to update execution log for schedule " + std::to_string(schedule.scheduleId) + ": unknown error");
    }
}

} // namespace pa_scheduler
