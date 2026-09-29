#pragma once
#include <string>
#include <memory>

namespace TA_IRS_App
{
    struct TemplateViewerProcess
    {
        TemplateViewerProcess();
        static TemplateViewerProcess& instance();
        static void remove();

        void launch(std::string options = {});

        struct Impl;
        std::shared_ptr<Impl> m_impl;
    };
}
