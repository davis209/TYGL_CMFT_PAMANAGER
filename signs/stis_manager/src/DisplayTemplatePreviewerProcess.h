#pragma once
#include <string>
#include <memory>

namespace TA_IRS_App
{
    struct DisplayTemplatePreviewerProcess
    {
        DisplayTemplatePreviewerProcess();
        static DisplayTemplatePreviewerProcess& instance();
        static void remove();

        bool preview(std::string xml);
        void launch_and_hide_async();

        struct Impl;
        std::shared_ptr<Impl> m_impl;
    };
}
