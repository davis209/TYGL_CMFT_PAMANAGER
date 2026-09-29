#include "stdafx.h"
#include "PythonServer.h"
#include "WindowsUtil.h"
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

#define RPARAM_STISPYTHONSERVERPORT                     "STISPythonServerPort"
#define RPARAM_STISPYTHONSERVERTYPE                     "STISPythonServerType"
#define RPARAM_STISPYTHONSERVERUSESCRIPTFILE            "STISPythonServerUseScriptFile"
#define RPARAM_STISPYTHONSERVERSCRIPTDIR                "STISPythonServerScriptDir"
#define RPARAM_STISPYTHONSERVERTIMEOUT                  "STISPythonServerTimeout"
#define RPARAM_STISPYTHONSERVERDEBUGDELAYGETMESSAGES    "STISPythonServerDebugDelayGetMessages"
#define RPARAM_STISPYTHONSERVERDEBUGDELAYGETTEMPLATES   "STISPythonServerDebugDelayGetTemplates"

using namespace std::chrono;
using namespace std::literals;
using boost::filesystem::path;
namespace python = boost::python;
using boost::python::def;

using st::StaticObject;
using TA_Base_Ex::RunParamsEx;
using TA_Base_Ex::LocationEx;
using namespace TA_Base_Core;
using namespace TA_IRS_App;
using namespace TA_IRS_App::STIS_PROTOCOL;

namespace
{
    const path DEFAULT_STIS_SCRIPT_DIR = R"(C:\transActive\bin)";
}

namespace TA_IRS_App::STIS_PROTOCOL::MESSAGE_TYPES
{
    using st::TagInvoke::tag_invoke;
    using st::TagInvoke::to_json;
    using st::TagInvoke::from_json;
}

namespace EMBEDED_MESSAGE
{
    using namespace boost::json;
    using st::TagInvoke::tag_invoke;
    using st::TagInvoke::to_json;
    using st::TagInvoke::from_json;
    using TemplateIdList = std::vector<std::string>;
    using TA_IRS_App::STIS_PROTOCOL::MESSAGE_TYPES::DestinationList;

    struct WithGIL
    {
        WithGIL() { state_ = PyGILState_Ensure(); }
        ~WithGIL() { PyGILState_Release(state_); }

        WithGIL(const WithGIL&) = delete;
        WithGIL& operator=(const WithGIL&) = delete;

        PyGILState_STATE state_;
    };

    struct WithoutGIL
    {
        WithoutGIL() { state_ = PyEval_SaveThread(); }
        ~WithoutGIL() { PyEval_RestoreThread(state_); }

        WithoutGIL(const WithoutGIL&) = delete;
        WithoutGIL& operator=(const WithoutGIL&) = delete;

        PyThreadState* state_ = nullptr;
    };

    struct Location
    {
        size_t key = 0;
        std::string name;
        std::string description;
        size_t order_id = 0;
        std::string display_name;
        std::string type_name = "OCC";
    };

    BOOST_DESCRIBE_STRUCT(Location, (), (key, name, description, order_id, display_name, type_name));

    using LocationList = std::vector<Location>;

    struct Pid
    {
        std::string asset;
        std::string location;
        std::string level;
        std::string name;
        std::string id;
    };

    BOOST_DESCRIBE_STRUCT(Pid, (), (asset, location, level, name, id));

    using PidList = std::vector<Pid>;

    struct Message
    {
        std::string location;
        std::string level;
        std::string pid;
        std::string display_message;
        std::string tag;
        int priority = 0;
        std::string start_datetime;
        std::string end_datetime;
        std::string display_template_type;
        std::string id;
        std::string template_start_datetime;
        std::string template_end_datetime;
    };

    BOOST_DESCRIBE_STRUCT(Message, (), (location, level, pid, display_message, tag, priority, start_datetime, end_datetime, display_template_type, id, template_start_datetime, template_end_datetime));

    using MessageList = std::vector<Message>;

    struct Template
    {
        std::string location;
        std::string level;
        std::string pid;
        std::string display_template_type;
        std::string id;
        std::string template_start_datetime;
        std::string template_end_datetime;
    };

    BOOST_DESCRIBE_STRUCT(Template, (), (location, level, pid, display_template_type, id, template_start_datetime, template_end_datetime));

    using TemplateList = std::vector<Template>;

    struct RemoveTemplateArgs
    {
        std::string location;
        std::vector<std::string> pids;
        std::vector<std::string> display_template_list;
    };

    BOOST_DESCRIBE_STRUCT(RemoveTemplateArgs, (), (location, pids, display_template_list));

    using BatchRemoveTemplateArgs = std::vector<RemoveTemplateArgs>;

    int get_port()
    {
        return RunParamsEx::get_or(RPARAM_STISPYTHONSERVERPORT, 10089);
    }

    void clear_messages(std::string destinations)
    {
        using DataType = std::vector<std::vector<std::string>>;
        auto data = from_json<DataType>(destinations);
        LOG_DEBUG("clear_messages(): %s", data);

        try
        {
            std::for_each(std::execution::par_unseq, data.begin(), data.end(), [&](auto& x)
            {
                auto location = LocationEx::to_name(x[0]);
                auto& pid = x[1];
                auto& tag = x[2];
                STISClient::instance().submit_M24_ClearCurrentMessagesRequestByMessageTag(Destination::from_location_pid(location, pid), tag);
            });
        }
        catch (...)
        {
        }
    }

    std::string get_locations()
    {
        LOG_CALLSTACK("EMBEDED_MESSAGE::get_locations");
        std::vector<std::string> locations;
        locations.reserve(LocationEx::get_all_locations().size());

        for (auto l : LocationEx::get_all_locations())
        {
            Location loc{};
            loc.key = l->getKey();
            loc.name = l->getName();
            loc.description = l->getDescription();
            loc.display_name = l->getDisplayName();
            loc.order_id = l->getOrderId();
            loc.type_name = l->getTypeName();
            locations.emplace_back(to_json(loc));
        }

        auto res = to_json(locations);
        LOG_DEBUG("get_locations(): %s", res);
        return res;
    }

    std::string get_pids()
    {
        LOG_CALLSTACK("EMBEDED_MESSAGE::get_pids");
        std::vector<std::string> pids;
        auto all = STIS_UTILITY::PID::get_all_stations();
        pids.reserve(all.size());

        for (auto pid : all)
        {
            Pid p{};
            p.asset = pid.asset;
            p.id = pid.id;
            p.level = pid.level;
            p.location = pid.extra.location;
            p.name = pid.name;
            pids.emplace_back(to_json(p));
        }

        return to_json(pids);
    }

    std::string get_messages(std::string location)
    {
        LOG_CALLSTACK("EMBEDED_MESSAGE::get_messages");
        LOG_DEBUG("get_messages(): %s", location);

        boost::trim(location);
        std::vector<std::string> messages;

        if (boost::iequals(location, "ALL") || location.empty())
        {
            location = std::string(6, ' ');
            messages.reserve(1000);
        }
        else
        {
            location = LocationEx::to_name(location);
            messages.reserve(100);
        }

        try
        {
            auto report = STISClient::instance().submit_M51_StationScheduledDisplayingMessageTemplateListRequest(location);

            for (auto&& sdm_for_pid : report.scheduled_display_message_for_pids)
            {
                for (auto&& sdm : sdm_for_pid.scheduled_display_messages)
                {
                    auto& pid = STIS_UTILITY::PID::from_station_and_id(sdm_for_pid.report_station, sdm_for_pid.report_pid);

                    Message m{};
                    m.location = LocationEx::to_display_name(sdm_for_pid.report_station);
                    m.level = pid.level;
                    m.pid = pid.name;

                    auto languages = STIS_UTILITY::split_to_4_languages_utf8(STIS_UTILITY::transform_4_languages_from_utf16_to_utf8(sdm.message_text));
                    st::remove_empty(languages);
                    m.display_message = boost::join(languages, "\n");

                    m.tag = sdm.message_tag;
                    m.priority = sdm.message_priority;
                    m.start_datetime = sdm.message_start_time;
                    m.end_datetime = sdm.message_end_time;
                    m.display_template_type = st2::enum_to_string(sdm_for_pid.current_display_template.display_template_type);
                    m.id = sdm_for_pid.current_display_template.display_template_id;
                    m.template_start_datetime = sdm_for_pid.current_display_template.start_time;
                    m.template_end_datetime = sdm_for_pid.current_display_template.end_time;
                    messages.emplace_back(to_json(m));
                }
            }

            if (auto value = RunParamsEx::get_optional<int>(RPARAM_STISPYTHONSERVERDEBUGDELAYGETMESSAGES, "--quiet"))
            {
                LOG_DEBUG("get_messages(): sleep for %d seconds", *value);
                st::sleep_for_s(*value);
            }

            return to_json(messages);
        }
        catch (const std::exception& e)
        {
            return "error: "s + e.what();
        }
    }

    std::string get_templates(std::string location)
    {
        LOG_CALLSTACK("EMBEDED_MESSAGE::get_templates");
        LOG_DEBUG("get_templates(): %s", location);

        boost::trim(location);
        std::vector<std::string> templates;

        if (boost::iequals(location, "ALL") || location.empty())
        {
            location = std::string(6, ' ');
            templates.reserve(1000);
        }
        else
        {
            location = LocationEx::to_name(location);
            templates.reserve(100);
        }

        try
        {
            auto report = STISClient::instance().submit_M52_StationScheduledDisplayingTemplateListRequest(location);

            for (auto&& stl_fo_pid : report.scheduled_display_template_list_for_pids)
            {
                for (auto&& stl : stl_fo_pid.scheduled_display_template_list)
                {
                    auto& pid = STIS_UTILITY::PID::from_station_and_id(stl_fo_pid.report_station, stl_fo_pid.report_pid);

                    Template t{};
                    t.location = LocationEx::to_display_name(stl_fo_pid.report_station);
                    t.level = pid.level;
                    t.pid = pid.name;
                    t.display_template_type = st2::enum_to_string(stl.display_template_type);
                    t.id = stl.display_template_id;
                    t.template_start_datetime = stl.start_time;
                    t.template_end_datetime = stl.end_time;
                    templates.emplace_back(to_json(t));
                }
            }

            if (auto value = RunParamsEx::get_optional<int>(RPARAM_STISPYTHONSERVERDEBUGDELAYGETTEMPLATES, "--quiet"))
            {
                LOG_DEBUG("get_templates(): sleep for %d seconds", *value);
                st::sleep_for_s(*value);
            }

            return to_json(templates);
        }
        catch (const std::exception& e)
        {
            return "error: "s + e.what();
        }
    }

    void remove_templates(std::string args)
    {
        LOG_CALLSTACK("EMBEDED_MESSAGE::remove_templates");
        LOG_DEBUG("remove_templates(): %s", args);

        try
        {
            // TODO: merge pids
            auto params_json = from_json<std::vector<std::string>>(args);

            std::for_each(std::execution::par_unseq, params_json.begin(), params_json.end(), [&](auto& param_json)
            {
                auto param = from_json<RemoveTemplateArgs>(param_json);
                auto destination = Destination::from_location_pids(LocationEx::to_name(param.location), param.pids);

                PredefinedDisplayTemplateList display_template_list;

                for (auto& x : param.display_template_list)
                {
                    display_template_list.emplace_back(from_json<PredefinedDisplayTemplate>(x));
                }

                STISClient::instance().submit_M53_RemoveDisplayTemplateByTemplateIDRequest(destination, display_template_list);
            });
        }
        catch (const std::exception& e)
        {
        }
    }

    BOOST_PYTHON_MODULE(embedded_message)
    {
        def("get_port", get_port);
        def("get_messages", get_messages);
        def("clear_messages", clear_messages);
        def("get_templates", get_templates);
        def("remove_templates", remove_templates);
        def("get_locations", get_locations);
        def("get_pids", get_pids);
    }

    // NOTE: https://docs.python.org/zh-cn/3/c-api/import.html
    auto s_embedded_message = PyImport_AppendInittab("embedded_message", PyInit_embedded_message);
}

namespace
{
    std::string xmlrpc_server_python_script = R"(
from xmlrpc.server import SimpleXMLRPCServer
from socketserver import ThreadingMixIn
from threading import Thread
import embedded_message as embeded

class MySimpleXMLRPCServer(ThreadingMixIn, SimpleXMLRPCServer):

    def __init__(self, host):
        super().__init__(host, allow_none=True, logRequests=False)
        self.register_introspection_functions()
        self.register_function(self.get_messages, 'get_messages')
        self.register_function(self.clear_messages, 'clear_messages')
        self.register_function(self.get_templates, 'get_templates')
        self.register_function(self.remove_templates, 'remove_templates')
        self.register_function(self.get_locations, 'get_locations')
        self.register_function(self.get_pids, 'get_pids')

    def get_messages(self, location):
        print('Location: ' + location)
        return embeded.get_messages(location)

    def clear_messages(self, data):
        print('CLEAR:', data)
        embeded.clear_messages(data)

    def get_templates(self, location):
        print('Location: ' + location)
        return embeded.get_templates(location)

    def remove_templates(self, templates):
        print('Templates: ' + templates)
        embeded.remove_templates(templates)

    def get_locations(slef):
        return embeded.get_locations()

    def get_pids(self):
        return embeded.get_pids()

host = ('', embeded.get_port())
server = MySimpleXMLRPCServer(host)
t = Thread(target=server.serve_forever)
t.start()
)";

    std::string http_server_python_script = R"(
import json
from http.server import HTTPServer, SimpleHTTPRequestHandler
from threading import Thread
import embedded_message as embeded

class Resquest(SimpleHTTPRequestHandler):

    def do_GET(self):
        self.send_response(200)
        self.send_header('Content-type', 'application/json')
        self.end_headers()
        print('GET', self.path)

        data = ''
        parts = self.path.split('/')
        match parts[1]:
            case 'get_messages':
                data = embeded.get_messages(parts[2])
            case 'get_locations':
                data = embeded.get_locations()
            case 'get_templates':
                data = embeded.get_templates(parts[2])
            case 'get_pids':
                data = embeded.get_pids()
        self.wfile.write(data.encode())

    def do_POST(self):
        data = self.rfile.read(int(self.headers["content-length"]))
        data = json.loads(data)
        print('POST', self.path, data)

        match self.path:
            case '/clear_messages':
                embeded.clear_messages(data)
            case '/remove_templates':
                embeded.remove_templates(data)

        self.send_response(200)
        self.send_header('Content-type', 'application/json')
        self.end_headers()

    def log_message(self, format, *args):
        pass

host = ('', embeded.get_port())
server = HTTPServer(host, Resquest)
t = Thread(target=server.serve_forever)
t.start()
)";

    std::string tcp_server_python_script = R"(
import json
import socketserver
from socketserver import ThreadingTCPServer, TCPServer
from threading import Thread
import embedded_message as embeded

class TCPHandler(socketserver.StreamRequestHandler):

    def setup(self):
        super().setup()

    def handle(self):
        try:
            while True:
                data = self.rfile.readline().strip().decode()
                if not data:
                    break
                request = json.loads(data)
                match request['function']:
                    case 'get_messages':
                        data = embeded.get_messages(request['args'])
                        self.wfile.write(bytes(data, "utf-8"))
                    case 'get_templates':
                        data = embeded.get_templates(request['args'])
                        self.wfile.write(bytes(data, "utf-8"))
                    case 'get_locations':
                        data = embeded.get_locations()
                        self.wfile.write(bytes(data, "utf-8"))
                    case 'get_pids':
                        data = embeded.get_pids()
                        self.wfile.write(bytes(data, "utf-8"))
                    case 'clear_messages':
                        embeded.clear_messages(request['args'])
                    case 'remove_templates':
                        embeded.remove_templates(request['args'])
        except:
            pass

    def finish(self):
        super().finish()

host = ('', embeded.get_port())
server = ThreadingTCPServer(host, TCPHandler)
t = Thread(target=server.serve_forever)
t.start()
)";
}

namespace TA_IRS_App
{
    struct PythonServer::Impl
    {
        void start()
        {
            std::call_once(m_once, [&]
            {
                boost::async([&]
                {
                    using namespace boost::python;

                    Py_Initialize();

                    try
                    {
                        python::object main = python::import("__main__");
                        python::object global(main.attr("__dict__"));

                        std::map<std::string, std::pair<std::string, path>, st::CompareNoCase> configs =
                        {
                            {"tcp", {tcp_server_python_script, m_script_dir / "tcp_message_server.py"}},
                            {"http", {http_server_python_script, m_script_dir / "http_message_server.py"}},
                            {"xmlrpc", {xmlrpc_server_python_script, m_script_dir / "xmlrpc_message_server.py"}},
                        };

                        if (auto& [script, script_file] = configs[m_server_type]; m_use_script_file && exists(script_file))
                        {
                            python::exec_file(script_file.string().c_str(), global, global);
                        }
                        else
                        {
                            python::exec(script.c_str(), global, global);
                        }

                        global["t"].attr("join")();
                    }
                    catch (...)
                    {
                    }

                    LOG_INFO("DONE");
                });
            });
        }

        std::string get_type()
        {
            return m_server_type;
        }

        std::string get_port()
        {
            return m_port;
        }

        std::string get_timeout()
        {
            return m_timeout;
        }

        std::once_flag m_once;
        std::string m_port = RunParamsEx::get_or(RPARAM_STISPYTHONSERVERPORT, "10089");
        std::string m_server_type = RunParamsEx::get_or(RPARAM_STISPYTHONSERVERTYPE, "tcp");
        std::string m_timeout = RunParamsEx::get_or(RPARAM_STISPYTHONSERVERTIMEOUT, "60");
        bool m_use_script_file = RunParamsEx::is_true(RPARAM_STISPYTHONSERVERUSESCRIPTFILE);
        path m_script_dir = st2::absoluted_copy(RunParamsEx::get_or(RPARAM_STISPYTHONSERVERSCRIPTDIR, DEFAULT_STIS_SCRIPT_DIR.string()));
    };

    PythonServer::PythonServer()
        : m_impl(std::make_shared<Impl>())
    {
    }

    PythonServer& PythonServer::instance()
    {
        return StaticObject<PythonServer>::value();
    }

    void PythonServer::start()
    {
        m_impl->start();
    }

    std::string PythonServer::get_type()
    {
        return m_impl->get_type();
    }

    std::string PythonServer::get_port()
    {
        return m_impl->get_port();
    }

    std::string PythonServer::get_timeout()
    {
        return m_impl->get_timeout();
    }
}
