#pragma once
#include <string>
#include <memory>

namespace TA_IRS_App
{
    struct PythonServer
    {
        PythonServer();
        static PythonServer& instance();

        void start();
        std::string get_type();
        std::string get_port();
        std::string get_timeout();

        struct Impl;
        std::shared_ptr<Impl> m_impl;
    };
}
