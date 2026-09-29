#include "stdafx.h"
#include "PredefinedTemplateDisplayProcess.h"
#include "WindowsUtil.h"
#include "core/utility/src/base_ex/SimpleHttps.h"
#include "core/utility/src/base_ex/RunParamsEx.h"
#include "core/utility/src/core/FileEx.h"
#include "core/utility/src/core/StdEx.h"
#include "core/utility/src/core/SimpleConditionVariable.h"
#include "core/utility/src/core/StaticObject.h"
#include <boost/process.hpp>
#include <thread>

#ifdef min
    #undef min
#endif

#define RPARAM_DISPLAYTEMPLATEPREVIEWER         "DisplayTemplatePreviewer"
#define RPARAM_DISPLAYTEMPLATEPREVIEWERPORT     "DisplayTemplatePreviewerPort"

using namespace std::chrono;
using namespace boost::program_options;
using boost::filesystem::path;
using namespace std::literals;

using ta_utility::core::FileEx;
using ta_utility::core::FileExPtr;
using ta_utility::core::SimpleConditionVariable;
using ta_utility::core::StaticObject;
using TA_Base_Ex::SimpleHttps;
using TA_Base_Ex::SimpleHttpsPtr;
using TA_Base_Ex::RunParamsEx;

namespace
{
    std::string& hack_preview(std::string& xml)
    {
        boost::replace_all(xml, "LEDTemplate", "Template");
        boost::replace_all(xml, "EmergencyTemplate", "Template");
        boost::replace_all(xml, "LEDScreen", "LCDScreen");
        return xml;
    }
}

namespace TA_IRS_App
{
    struct PredefinedTemplateDisplayProcess::Impl
    {
        Impl()
        {
        }

        ~Impl()
        {
            kill();
        }

        bool launch()
        {
            if (!is_running())
            {
                try
                {
                    m_process = std::make_shared<boost::process::child>(m_executable);
                    m_start_time = steady_clock::now();
                    m_handle = ::OpenProcess(PROCESS_ALL_ACCESS, false, (DWORD)(m_process->id()));

                    boost::async([&]
                    {
                        m_process->wait();
                        m_process.reset();
                        m_handle = nullptr;
                        m_window = nullptr;
                    });
                }
                catch (...)
                {
                }
            }

            return true;
        }

        void kill()
        {
            if (m_process)
            {
                m_process->terminate();
            }

            m_handle = nullptr;
            m_window = nullptr;
            m_start_time = steady_clock::time_point::min();
        }

        bool preview(std::string xml)
        {
            if (!wait_for_connectable())
            {
                return false;
            }

            hack_preview(xml);

            m_http.post("/xmldata", m_headers, std::move(xml));
        }

        bool is_running()
        {
            return (m_process && m_process->running() || is_connectable());
        }

        bool is_connectable()
        {
            return stdex::is_port_used(m_port);
        }

        bool wait_for_connectable(int timeout_seconds = 10)
        {
            if (!is_running())
            {
                if (!launch())
                {
                    return false;
                }
            }

            auto timeout = seconds(timeout_seconds);
            bool connectable = is_connectable();


            while (!connectable && (steady_clock::now() - m_start_time < timeout))
            {
                if (connectable = is_connectable())
                {
                    break;
                }

                stdex::sleep_for(100ms);
            }

            if (!connectable)
            {
                kill();
                return false;
            }

            if (!m_window && m_process)
            {
                m_window = WindowsUtil::find_main_window(m_process->id());
            }

            if (m_window)
            {
                bring_window_to_topmost();
            }

            return true;
        }

        void bring_window_to_topmost()
        {
            // TODO
        }

        steady_clock::time_point m_start_time = steady_clock::time_point::min();
        std::shared_ptr<boost::process::child> m_process;
        std::string m_port = RunParamsEx::get_or(RPARAM_DISPLAYTEMPLATEPREVIEWERPORT, "10086");
        SimpleHttps m_http{"--protocol=http --port=" + m_port};
        std::string m_executable = RunParamsEx::get_or(RPARAM_DISPLAYTEMPLATEPREVIEWER, "Predefined_Display.exe");
        std::vector<std::string> m_headers{"Content-Type: text/plain"};
        HANDLE m_handle = nullptr;
        HWND m_window = nullptr;
    };

    PredefinedTemplateDisplayProcess::PredefinedTemplateDisplayProcess()
        : m_impl(std::make_shared<Impl>())
    {
    }

    PredefinedTemplateDisplayProcess& PredefinedTemplateDisplayProcess::instance()
    {
        return StaticObject<PredefinedTemplateDisplayProcess>::value();
    }

    void PredefinedTemplateDisplayProcess::remove()
    {
        return StaticObject<PredefinedTemplateDisplayProcess>::clear();
    }

    bool PredefinedTemplateDisplayProcess::preview(std::string xml)
    {
        return m_impl->preview(std::move(xml));
    }
}
