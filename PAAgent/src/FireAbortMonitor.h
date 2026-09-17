#pragma once
#include <string>
#include <vector>
#include <memory>

namespace TA_IRS_App
{
    struct FireAbortMonitor
    {
        static FireAbortMonitor& instance();

        FireAbortMonitor(std::string options = "");

        void parse_options(std::string options);

        void start();
        void stop();

        struct Impl;
        std::shared_ptr<Impl> m_impl;
    };

    using FireAbortMonitorPtr = std::shared_ptr<FireAbortMonitor>;
}
