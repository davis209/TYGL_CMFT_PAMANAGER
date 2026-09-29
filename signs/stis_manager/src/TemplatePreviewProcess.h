#pragma once
#include <string>
#include <memory>

namespace TA_IRS_App
{
    struct TemplatePreviewProcess
    {
        TemplatePreviewProcess();
        static TemplatePreviewProcess& instance();
        static void remove();

        bool preview(std::string template_id);
        void launch_and_hide_async();

        struct Impl;
        std::shared_ptr<Impl> m_impl;
    };
}
