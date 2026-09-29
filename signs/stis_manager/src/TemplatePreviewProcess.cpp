#include "stdafx.h"
#include "TemplatePreviewProcess.h"
#include "WindowsUtil.h"
#include "app/signs/common_library/src/stis_protocol/CommonDefs.h"
#include "core/utility/src/base_ex/RunParamsEx.h"
#include "core/utility/src/base_ex/DebugUtilEx.h"
#include "core/utility/src/core/StdEx.h"
#include "core/utility/src/core/StaticObject.h"
#include "core/utility/src/core/Vector.h"
#include "core/utility/src/core/algorithm/file_system.h"
#include "core/utility/src/core/algorithm/strings.h"
#include <boost/process.hpp>
#include <boost/process/windows.hpp>

#define RPARAM_TEMPLATEPREVIEWEXE      "TemplatePreviewExe"
#define RPARAM_TEMPLATEPREVIEWPORT     "TemplatePreviewPort"

using namespace std::chrono;
using namespace std::literals;
using namespace boost::program_options;
namespace bp = boost::process;
using boost::filesystem::path;
using boost::filesystem::exists;
using st::StaticObject;
using TA_Base_Ex::RunParamsEx;

namespace
{
    const path DEFAULT_STIS_TEMPLATE_PREVIEW_EXE = R"(C:\transActive\bin\STISTemplatePreview\app.js)";
    const path DEFAULT_SNAPSHOT_DIR = R"(C:\transActive\config\database\stis\tmlibrary\snapshot)";
    const path CHROME_PATH = R"(C:\Program Files\Google\Chrome\Application\chrome.exe)";
    const path CHROME_32_PATH = R"(C:\Program Files (x86)\Google\Chrome\Application\chrome.exe)";
    const path CHROME_SEARCH_PATH = bp::search_path("chrome.exe");
    const path MICROSOFT_EDGE_PATH = R"(C:\Program Files (x86)\Microsoft\Edge\Application\msedge.exe)";
    const path MICROSOFT_EDGE_SEARCH_PATH = bp::search_path("msedge.exe");
    const size_t MONITOR_WIDTH = 1920;
    const size_t MONITOR_HEIGHT = 1080;
}

namespace TA_IRS_App
{
    struct TemplatePreviewProcess::Impl
    {
        Impl() = default;

        ~Impl()
        {
            kill();

            if (m_node_future.has_value())
            {
                m_node_future.wait();
            }

            for (auto& browser : m_browser_processes)
            {
                browser->terminate();
            }
        }

        bool launch()
        {
            if (!is_running())
            {
                try
                {
                    path snapshot_dir = DEFAULT_SNAPSHOT_DIR;

                    if (RunParamsEx::is_set(RPARAM_STISROOTDIR))
                    {
                        snapshot_dir = path{RunParamsEx::get_expanded(RPARAM_STISROOTDIR)} / "tmlibrary" / "snapshot";
                    }

                    m_process = std::make_shared<bp::child>(bp::search_path("node.exe"),
                                                            m_executable.filename().string(),
                                                            bp::start_dir(m_executable.parent_path().string()),
                                                            "--auto-exit",
                                                            "--port", m_port,
                                                            "--snapshot-dir", snapshot_dir.string(),
                                                            bp::windows::create_no_window);

                    if (!st::wait_for<10 * 1000, 100>([&] { return is_connectable(); }))
                    {
                        kill();
                        return false;
                    }

                    m_node_future = boost::async([=]
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

            return true;
        }

        void kill()
        {
            if (m_process)
            {
                m_process->terminate();
            }
        }

        void kill_all()
        {
            kill();

            if (is_running())
            {
                WindowsUtil::kill_process(WindowsUtil::get_pid_by_tcp_port(m_port));
            }
        }

        bool preview(std::string template_id)
        {
            if (RunParamsEx::is_true("debug-template-preview-start-new-node"))
            {
                kill_all();
                launch();
            }

            if (!wait_for_connectable())
            {
                return false;
            }

            if (auto browsers = st::filter_copy({CHROME_PATH, CHROME_32_PATH, CHROME_SEARCH_PATH, MICROSOFT_EDGE_PATH, MICROSOFT_EDGE_SEARCH_PATH}, BOOST_HOF_LIFT(exists)); browsers.size())
            {
                CPoint point;
                GetCursorPos(&point);

                auto x = (point.x / 1920) * 1920;
                auto user_data_dir = boost::filesystem::temp_directory_path() / "stis" / "chrome";
                auto window_position = st2::format("%d,155", x);
                auto window_size = "1920,860";

                auto cmd = st2::format("%s --app=http://localhost:%s/%s --user-data-dir=%s --window-position=%s --window-size=%s",
                                       browsers[0].string(),
                                       m_port,
                                       boost::to_upper_copy(template_id),
                                       user_data_dir,
                                       window_position,
                                       window_size);

                auto process = std::make_shared<bp::child>(cmd);
                m_browser_processes.emplace_back(process);

                boost::async([=]
                {
                    process->wait();
                    m_browser_processes.remove(process);
                });

                return true;
            }

            return bp::system(st2::format("explorer.exe http://localhost:%s/%s", m_port, boost::to_upper_copy(template_id)), bp::windows::create_no_window);
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

        bool wait_for_connectable(int timeout_seconds = 1)
        {
            if (!is_running())
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

        void launch_and_hide_async()
        {
            boost::async([&]
            {
                launch();
            });
        }

        boost::future<void> m_node_future;
        std::shared_ptr<boost::process::child> m_process;
        st::vector<std::shared_ptr<boost::process::child>> m_browser_processes;
        std::string m_port = RunParamsEx::get_or(RPARAM_TEMPLATEPREVIEWPORT, "10099");
        path m_executable = st2::absoluted_copy(RunParamsEx::get_or(RPARAM_TEMPLATEPREVIEWEXE, DEFAULT_STIS_TEMPLATE_PREVIEW_EXE.string()));
    };

    TemplatePreviewProcess::TemplatePreviewProcess()
        : m_impl(std::make_shared<Impl>())
    {
    }

    TemplatePreviewProcess& TemplatePreviewProcess::instance()
    {
        return StaticObject<TemplatePreviewProcess>::value();
    }

    void TemplatePreviewProcess::remove()
    {
        return StaticObject<TemplatePreviewProcess>::clear();
    }

    bool TemplatePreviewProcess::preview(std::string template_id)
    {
        return m_impl->preview(std::move(template_id));
    }

    void TemplatePreviewProcess::launch_and_hide_async()
    {
        m_impl->launch_and_hide_async();
    }
}
