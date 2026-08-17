#pragma once

#include <string>

namespace pa_scheduler {

class ServiceLogger {
public:
    static void Initialize(const std::string& fileName);
    static void Info(const std::string& message);
    static void Error(const std::string& message);
};

} // namespace pa_scheduler
