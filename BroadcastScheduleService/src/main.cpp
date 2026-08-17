#include "BroadcastSchedulerService.h"
#include "ServiceLogger.h"

#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

namespace {
std::atomic_bool g_stopRequested{false};

void OnSignal(int) { g_stopRequested.store(true); }

void PrintUsage() {
    std::cout << "Usage: BroadcastScheduleService --location-id <id> --db-user <user> --db-name <database> "
                 "--pa-host <ip-or-hostname> --pa-port <port> [--db-host <host>] [--db-port <port>] "
                 "[--db-password <password>] [--poll-seconds <seconds>] [--pa-console-id <id>]\n";
}

const char* ValueAfter(int& index, int argc, char* argv[], const std::string& option) {
    if (++index >= argc) throw std::runtime_error("Missing value for " + option);
    return argv[index];
}
} // namespace

int main(int argc, char* argv[]) {
    try {
        pa_scheduler::ServiceLogger::Initialize("BroadcastScheduleService.log");
        pa_scheduler::SchedulerConfig config;
        pa_scheduler::PaDeviceConfig paDevice;
        for (int index = 1; index < argc; ++index) {
            const std::string option(argv[index]);
            if (option == "--help" || option == "-h") { PrintUsage(); return 0; }
            if (option == "--location-id") config.locationId = std::stoi(ValueAfter(index, argc, argv, option));
            else if (option == "--db-host") config.database.host = ValueAfter(index, argc, argv, option);
            else if (option == "--db-port") config.database.port = static_cast<unsigned int>(std::stoul(ValueAfter(index, argc, argv, option)));
            else if (option == "--db-user") config.database.user = ValueAfter(index, argc, argv, option);
            else if (option == "--db-password") config.database.password = ValueAfter(index, argc, argv, option);
            else if (option == "--db-name") config.database.database = ValueAfter(index, argc, argv, option);
            else if (option == "--poll-seconds") config.pollIntervalSeconds = static_cast<unsigned int>(std::stoul(ValueAfter(index, argc, argv, option)));
            else if (option == "--pa-host") paDevice.host = ValueAfter(index, argc, argv, option);
            else if (option == "--pa-port") paDevice.port = static_cast<unsigned short>(std::stoul(ValueAfter(index, argc, argv, option)));
            else if (option == "--pa-server-id") paDevice.serverId = static_cast<unsigned char>(std::stoul(ValueAfter(index, argc, argv, option)));
            else if (option == "--pa-console-id") paDevice.consoleId = static_cast<unsigned char>(std::stoul(ValueAfter(index, argc, argv, option)));
            else if (option == "--pa-version") paDevice.version = ValueAfter(index, argc, argv, option);
            else if (option == "--pa-line-id") paDevice.lineId = ValueAfter(index, argc, argv, option);
            else if (option == "--pa-station-id") paDevice.stationId = ValueAfter(index, argc, argv, option);
            else if (option == "--pa-connect-timeout-ms") paDevice.connectTimeoutMilliseconds = static_cast<unsigned int>(std::stoul(ValueAfter(index, argc, argv, option)));
            else if (option == "--pa-response-timeout-ms") paDevice.responseTimeoutMilliseconds = static_cast<unsigned int>(std::stoul(ValueAfter(index, argc, argv, option)));
            else throw std::runtime_error("Unknown option: " + option);
        }

        if (config.database.password.empty()) {
#ifdef _WIN32
            char* password = nullptr;
            std::size_t length = 0;
            if (_dupenv_s(&password, &length, "PA_SCHEDULER_DB_PASSWORD") == 0 && password != nullptr) {
                config.database.password = password;
                std::free(password);
            }
#else
            const char* password = std::getenv("PA_SCHEDULER_DB_PASSWORD");
            if (password != nullptr) config.database.password = password;
#endif
        }
        if (config.locationId <= 0 || config.database.user.empty() || config.database.database.empty() || paDevice.host.empty() || paDevice.port == 0) {
            PrintUsage();
            return 2;
        }

        std::signal(SIGINT, OnSignal);
        std::signal(SIGTERM, OnSignal);
        auto executor = std::make_shared<pa_scheduler::M44BroadcastExecutor>(paDevice);
        pa_scheduler::BroadcastSchedulerService service(config, executor);
        pa_scheduler::ServiceLogger::Info("Starting scheduler for location " + std::to_string(config.locationId) +
            "; database=" + config.database.host + ":" + std::to_string(config.database.port) + "/" + config.database.database +
            "; PA device=" + paDevice.host + ":" + std::to_string(paDevice.port));
        service.Start();
        while (!g_stopRequested.load()) std::this_thread::sleep_for(std::chrono::milliseconds(200));
        pa_scheduler::ServiceLogger::Info("Stop signal received; waiting for running broadcasts to finish");
        service.Stop();
        pa_scheduler::ServiceLogger::Info("BroadcastScheduleService stopped");
        return 0;
    } catch (const std::exception& error) {
        pa_scheduler::ServiceLogger::Error(error.what());
        std::cerr << "BroadcastScheduleService: " << error.what() << '\n';
        return 1;
    }
}
