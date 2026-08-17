#pragma once

#include <atomic>
#include <cstdint>
#include <future>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace pa_scheduler {

struct DatabaseConfig {
    std::string host = "127.0.0.1";
    unsigned int port = 3306;
    std::string user;
    std::string password;
    std::string database;
};

struct BroadcastSchedule {
    std::uint64_t scheduleId = 0;
    int locationId = 0;
    std::string scheduleName;
    int messageId = 0;
    std::string messageVersion;
    std::string stations;
    unsigned int zones = 0;
    int seatId = 0;
    unsigned int language = 1;
    unsigned int playCount = 1;
    unsigned int playIntervalSeconds = 0;
    std::string scheduleType;
    std::string startAt;
    unsigned int repeatInterval = 1;
    unsigned int weekdayMask = 0;
    std::string plannedAt;
    std::uint64_t runId = 0;
};

enum class ExecutionStatus { Success, PartialFailed, Failed, Cancelled };

struct ExecutionResult {
    ExecutionStatus status = ExecutionStatus::Failed;
    int announceId = 0;
    std::string errorCode;
    std::string errorMessage;
};

// Implement this interface to call a PA protocol client directly. Execute may run concurrently.
class IBroadcastExecutor {
public:
    virtual ~IBroadcastExecutor() = default;
    virtual ExecutionResult Execute(const BroadcastSchedule& schedule,
                                    const std::atomic_bool& stopRequested) = 0;
};

// Invokes an existing broadcast client. Supported placeholders include {schedule_id}, {msg_id},
// {msg_version}, {stations}, {zones}, {seat_id}, {language}, {play_count}, and {play_interval_sec}.
class CommandBroadcastExecutor final : public IBroadcastExecutor {
public:
    explicit CommandBroadcastExecutor(std::string commandTemplate);
    ExecutionResult Execute(const BroadcastSchedule& schedule,
                            const std::atomic_bool& stopRequested) override;

private:
    std::string commandTemplate_;
};

struct PaDeviceConfig {
    std::string host;
    unsigned short port = 0;
    unsigned char serverId = 1;
    unsigned char consoleId = 54;
    std::string version = "00001";
    std::string lineId = "000";
    std::string stationId = "      ";
    unsigned int connectTimeoutMilliseconds = 5000;
    unsigned int responseTimeoutMilliseconds = 4000;
};

// Native implementation of PAManager's M44 prerecorded-message protocol.
class M44BroadcastExecutor final : public IBroadcastExecutor {
public:
    explicit M44BroadcastExecutor(PaDeviceConfig config);
    ExecutionResult Execute(const BroadcastSchedule& schedule,
                            const std::atomic_bool& stopRequested) override;

private:
    PaDeviceConfig config_;
};

struct SchedulerConfig {
    DatabaseConfig database;
    int locationId = 0;
    unsigned int pollIntervalSeconds = 2;
    unsigned int maxDueSchedulesPerPoll = 100;
};

// Thread-safe lifecycle service. Start and Stop are idempotent; Stop waits for in-flight jobs.
class BroadcastSchedulerService {
public:
    BroadcastSchedulerService(SchedulerConfig config, std::shared_ptr<IBroadcastExecutor> executor);
    ~BroadcastSchedulerService();

    void Start();
    void Stop();
    bool IsRunning() const;

private:
    void WorkerLoop();
    void DispatchDueSchedules();
    void RunSchedule(BroadcastSchedule schedule);

    SchedulerConfig config_;
    std::shared_ptr<IBroadcastExecutor> executor_;
    std::atomic_bool running_{false};
    std::atomic_bool stopRequested_{false};
    std::thread worker_;
    mutable std::mutex jobsMutex_;
    std::vector<std::future<void>> jobs_;
};

} // namespace pa_scheduler
