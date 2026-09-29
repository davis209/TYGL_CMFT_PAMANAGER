#pragma once
#include <string>
#include <memory>

namespace TA_IRS_App
{
    struct MessageViewerProcess
    {
        MessageViewerProcess();
        static MessageViewerProcess& instance();
        static void remove();

        void launch();

        struct Impl;
        std::shared_ptr<Impl> m_impl;
    };
}
