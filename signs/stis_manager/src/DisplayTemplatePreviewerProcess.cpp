#include "stdafx.h"
#include "DisplayTemplatePreviewerProcess.h"
#include "WindowsUtil.h"
#include "core/utility/src/base_ex/SimpleHttps.h"
#include "core/utility/src/base_ex/RunParamsEx.h"
#include "core/utility/src/base_ex/DebugUtilEx.h"
#include "core/utility/src/core/FileEx.h"
#include "core/utility/src/core/StdEx.h"
#include "core/utility/src/core/SimpleConditionVariable.h"
#include "core/utility/src/core/StaticObject.h"
#include "core/utility/src/core/algorithm/file_system.h"
#include <boost/process.hpp>
#include <thread>

#ifdef min
#undef min
#endif

#define RPARAM_DISPLAYTEMPLATEPREVIEWEREXE      "DisplayTemplatePreviewerExe"
#define RPARAM_DISPLAYTEMPLATEPREVIEWERPORT     "DisplayTemplatePreviewerPort"

using namespace std::chrono;
using namespace boost::program_options;
using boost::filesystem::path;
using namespace std::literals;

using st::FileEx;
using st::FileExPtr;
using st::SimpleConditionVariable;
using st::StaticObject;
using TA_Base_Ex::SimpleHttps;
using TA_Base_Ex::SimpleHttpsPtr;
using TA_Base_Ex::RunParamsEx;

namespace
{
    const path DEFAULT_STIS_TEMPLATE_VIEWER = R"(C:\transActive\bin\STISTemplateViewer\STISTemplateViewer.exe)";
}

namespace TA_IRS_App
{
    struct DisplayTemplatePreviewerProcess::Impl
    {
        Impl()
        {
        }

        ~Impl()
        {
            kill();

            if (m_future.has_value())
            {
                m_future.wait();
            }
        }

        bool launch()
        {
            if (!is_running())
            {
                try
                {
                    m_process = std::make_shared<boost::process::child>(m_executable.string());

                    if (!st::wait_for<10 * 1000, 100>([&] { return is_connectable(); }))
                    {
                        kill();
                        return false;
                    }

                    st::wait_for<3 * 1000, 100>([&]
                    {
                        if (auto hwnd = WindowsUtil::find_main_window_recursive(m_process->id()))
                        {
                            m_window.Attach(hwnd);
                            return true;
                        }

                        return false;
                    });

                    m_future = boost::async([=]
                    {
                        m_process->wait();
                        m_process.reset();
                    });
                }
                catch (std::exception& e)
                {
                    LOG_ERROR("launch(): %s", e);
                    return false;
                }
                catch (...)
                {
                    LOG_ERROR("launch(): unknown exception");
                    return false;
                }
            }

            if (is_running_external())
            {
                auto pid = WindowsUtil::get_pid_by_tcp_port(m_port);
                m_external_pid = WindowsUtil::get_root_process_id_for_the_same_program(pid);
                LOG_DEBUG("launch(): found external process with pid %d", m_external_pid);

                if (auto hwnd = WindowsUtil::find_main_window_recursive(m_external_pid))
                {
                    m_window.Attach(hwnd);
                }
            }

            return true;
        }

        void kill()
        {
            if (m_process)
            {
                WindowsUtil::kill_process_tree(m_process->id());
                m_process->terminate();
            }
        }

        bool preview(std::string xml)
        {
            m_requested = true;

            if (!wait_for_connectable())
            {
                return false;
            }

            activate_window();
            m_http.post("/xmldata", m_headers, std::move(xml));
        }

        bool is_running_anywhere()
        {
            return is_running_internal() || is_running_external();
        }

        bool is_running_internal()
        {
            return (m_process && m_process->running());
        }

        bool is_running_external()
        {
            return !m_process && is_connectable();
        }

        bool is_running()
        {
            return (m_process && m_process->running() || is_connectable());
        }

        bool is_connectable()
        {
            return st::is_port_used(m_port);
        }

        bool is_connectable(size_t timeout_seconds)
        {
            return st::wait_for(seconds(timeout_seconds), 100ms, [&]
            {
                return is_connectable();
            });
        }

        bool wait_for_connectable(int timeout_seconds = 10)
        {
            if (!is_running_anywhere())
            {
                if (!launch())
                {
                    return false;
                }
            }

            if (!is_connectable(timeout_seconds))
            {
                kill();
                return false;
            }

            return true;
        }

        bool is_owner()
        {
            return static_cast<bool>(m_process);
        }

        void launch_and_hide_async()
        {
            boost::async([&]
            {
                launch();

                if (!m_requested && is_running_internal())
                {
                    WindowsUtil::hide_window(m_window);
                }
            });
        }

        void activate_window()
        {
            WindowsUtil::show_window(m_window);
            WindowsUtil::activate_window(m_window);
        }

        CWnd m_window;
        size_t m_external_pid = 0;
        boost::future<void> m_future;
        std::atomic_bool m_requested = false;
        std::shared_ptr<boost::process::child> m_process;
        std::vector<std::string> m_headers{"Content-Type: text/plain"};
        std::string m_port = RunParamsEx::get_or(RPARAM_DISPLAYTEMPLATEPREVIEWERPORT, "10086");
        SimpleHttps m_http{"--protocol=http --port=" + m_port};
        path m_executable = st2::absoluted_copy(RunParamsEx::get_or(RPARAM_DISPLAYTEMPLATEPREVIEWEREXE, DEFAULT_STIS_TEMPLATE_VIEWER.string()));
    };

    DisplayTemplatePreviewerProcess::DisplayTemplatePreviewerProcess()
        : m_impl(std::make_shared<Impl>())
    {
    }

    DisplayTemplatePreviewerProcess& DisplayTemplatePreviewerProcess::instance()
    {
        return StaticObject<DisplayTemplatePreviewerProcess>::value();
    }

    void DisplayTemplatePreviewerProcess::remove()
    {
        return StaticObject<DisplayTemplatePreviewerProcess>::clear();
    }

    bool DisplayTemplatePreviewerProcess::preview(std::string xml)
    {
        return m_impl->preview(std::move(xml));
    }

    void DisplayTemplatePreviewerProcess::launch_and_hide_async()
    {
        m_impl->launch_and_hide_async();
    }
}
