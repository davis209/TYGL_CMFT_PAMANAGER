#include "stdafx.h"
#include "TemplateViewerProcess.h"
#include "WindowsUtil.h"
#include "PythonServer.h"
#include "app/signs/common_library/src/STISUtility.h"
#include "app/signs/common_library/src/stis_protocol/STISClient.h"
#include "core/utility/src/base_ex/RunParamsEx.h"
#include "core/utility/src/base_ex/DebugUtilEx.h"
#include "core/utility/src/base_ex/LocationAccessFactoryEx.h"
#include "core/utility/src/base_ex/EntityAccessFactoryEx.h"
#include "core/utility/src/core/TagInvoke.h"
#include "core/utility/src/core/StdEx.h"
#include "core/utility/src/core/StaticObject.h"
#include "core/utility/src/core/algorithm/file_system.h"
#include "core/utility/src/core/algorithm/strings.h"
#include "core/utility/src/core/algorithm/utility.h"
#include <boost/process.hpp>
#include <boost/python.hpp>
#include <boost/python/call.hpp>
#include <boost/python/call_method.hpp>
#include <thread>
#include <execution>

#ifdef min
#undef min
#endif

#define RPARAM_STISTEMPLATEVIEWEREXE                 "STISTemplateViewerExe"

using namespace std::chrono;
using namespace std::literals;
using boost::filesystem::path;

using st::StaticObject;
using TA_Base_Ex::RunParamsEx;
using TA_Base_Ex::LocationEx;
using namespace TA_Base_Core;
using namespace TA_IRS_App;
using namespace TA_IRS_App::STIS_PROTOCOL;

namespace
{
    const path DEFAULT_STIS_TEMPLATE_VIEWER = R"(C:\transActive\bin\STISTemplateViewer.exe)";
}

namespace TA_IRS_App
{
    struct TemplateViewerProcess::Impl
    {
        Impl()
        {
            PythonServer::instance().start();
        }

        ~Impl()
        {
            kill();

            if (m_future.has_value())
            {
                m_future.wait();
            }
        }

        bool launch(std::string options)
        {
            if (!is_running())
            {
                try
                {
                    if (!exists(m_executable))
                    {
                        LOG_ERROR("launch(): can not find %s", m_executable);
                        return false;
                    }

                    auto& server = PythonServer::instance();
                    m_process = std::make_shared<boost::process::child>(st2::format("%s --location=%s --server-type=%s --port=%s --timeout=%s %s", m_executable.string(), ThisLocation::name(), server.get_type(), server.get_port(), server.get_timeout(), options));

                    if (!st::wait_for<10 * 1000, 100>([&]
                    {
                        if (auto hwnd = WindowsUtil::find_main_window_recursive(m_process->id()))
                        {
                            m_window.Attach(hwnd);
                            return true;
                        }

                        return false;
                    }));

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

            activate_window();
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

        bool is_running()
        {
            return (m_process && m_process->running());
        }

        void activate_window()
        {
            WindowsUtil::show_window(m_window);
            WindowsUtil::activate_window(m_window);
        }

        CWnd m_window;
        boost::future<void> m_future;
        std::shared_ptr<boost::process::child> m_process;
        path m_executable = st2::absoluted_copy(RunParamsEx::get_or(RPARAM_STISTEMPLATEVIEWEREXE, DEFAULT_STIS_TEMPLATE_VIEWER.string()));
    };

    TemplateViewerProcess::TemplateViewerProcess()
        : m_impl(std::make_shared<Impl>())
    {
    }

    TemplateViewerProcess& TemplateViewerProcess::instance()
    {
        return StaticObject<TemplateViewerProcess>::value();
    }

    void TemplateViewerProcess::remove()
    {
        return StaticObject<TemplateViewerProcess>::clear();
    }

    void TemplateViewerProcess::launch(std::string options)
    {
        m_impl->launch(std::move(options));
    }
}
