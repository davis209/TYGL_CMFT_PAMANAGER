#pragma once
#include <memory>

namespace TA_IRS_App::legacytisagent::detail
{
    struct StisServerServant
    {
        StisServerServant();

        static StisServerServant& instance();

        void start();
        void stop();

        struct Impl;
        std::shared_ptr<Impl> m_impl;
    };
}

namespace TA_IRS_App
{
    using legacytisagent::detail::StisServerServant;
}
