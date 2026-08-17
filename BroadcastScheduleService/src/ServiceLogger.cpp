#include "ServiceLogger.h"

#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>

namespace pa_scheduler {
namespace {
std::mutex g_logMutex;
std::ofstream g_logFile;

std::string Timestamp() {
    const auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm local = {};
#ifdef _WIN32
    localtime_s(&local, &now);
#else
    localtime_r(&now, &local);
#endif
    std::ostringstream stream;
    stream << std::put_time(&local, "%Y-%m-%d %H:%M:%S");
    return stream.str();
}

void Write(const char* level, const std::string& message) {
    std::lock_guard<std::mutex> lock(g_logMutex);
    const std::string entry = "[" + Timestamp() + "] [" + level + "] " + message;
    std::cout << entry << std::endl;
    if (g_logFile.is_open()) {
        g_logFile << entry << std::endl;
        g_logFile.flush();
    }
}
} // namespace

void ServiceLogger::Initialize(const std::string& fileName) {
    std::lock_guard<std::mutex> lock(g_logMutex);
    g_logFile.open(fileName, std::ios::out | std::ios::app);
    const std::string entry = "[" + Timestamp() + "] [INFO] ===== BroadcastScheduleService started =====";
    std::cout << entry << std::endl;
    if (g_logFile.is_open()) {
        g_logFile << entry << std::endl;
        g_logFile.flush();
    }
}

void ServiceLogger::Info(const std::string& message) { Write("INFO", message); }
void ServiceLogger::Error(const std::string& message) { Write("ERROR", message); }

} // namespace pa_scheduler
