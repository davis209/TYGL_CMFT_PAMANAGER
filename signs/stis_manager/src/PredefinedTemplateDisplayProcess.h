#pragma once
#include <string>
#include <memory>

namespace TA_IRS_App
{
    struct PredefinedTemplateDisplayProcess
    {
        PredefinedTemplateDisplayProcess();
        static PredefinedTemplateDisplayProcess& instance();
        static void remove();

        bool preview(std::string xml);

        struct Impl;
        std::shared_ptr<Impl> m_impl;
    };
}
