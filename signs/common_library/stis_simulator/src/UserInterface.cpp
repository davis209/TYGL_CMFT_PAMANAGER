/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/J155/transactive/app/signs/common_library/stis_simulator/src/UserInterface.cpp $
 * @author:  Ripple
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 */

#include "pch.h"
#include "UserInterface.h"
#include "STISSimulator.h"
#include "core/utility/src/base_ex/RunParamsEx.h"
#include "core/utility/src/base_ex/DebugUtilEx.h"
#include "core/utility/src/core/StdEx.h"
#include "core/utility/src/core/algorithm/strings.h"
#include "core/utility/src/core/SimpleTimer.h"
#include "core/utility/src/core/Vector.h"
#include "core/process_management/src/UtilityInitialiser.h"
#include "core/utilities/src/DebugUtilInit.h"
#include "core/versioning/src/Version.h"
#include <iostream>

using namespace std::literals;
using namespace TA_Base_Ex;
using namespace TA_Base_Core;
using namespace TA_IRS_App::STIS_PROTOCOL::SIMULATOR;
using namespace st2::string_literals;
using st::SimpleTimer;
namespace sf = st::thread_safe;

namespace
{
    struct Command
    {
        std::set<std::string, st::CompareNoCase> ids;
        std::function<void(std::vector<std::string>)> func;
        std::string desc;
        std::string help;
        int min_args = 1;

        operator bool() const
        {
            return func ? true : false;
        }
    };

    using CommandList = sf::vector<Command>;
}

struct UserInterface::Impl
{
    Impl(STISSimulatorPtr simulator)
        : m_simulator(std::move(simulator))
    {
        add_basic_commands();
        add_command("--separator--", "", [&](auto& args) {});
        add_additional_commands();
    }

    void add_basic_commands()
    {
        add_command("1", "1) set current message library version", [&](auto& args) { m_simulator->set_current_message_library_version(args[1]); }, 2);
        add_command("2", "2) set next message library version", [&](auto& args) { m_simulator->set_next_message_library_version(args[1]); }, 2);
        add_command("3", "3) set current template library version", [&](auto& args) { m_simulator->set_current_template_library_version(args[1]); }, 2);
        add_command("4", "4) set next template library version", [&](auto& args) { m_simulator->set_next_template_library_version(args[1]); }, 2);
        add_command("5", "5) set lan connection link status (0|1|2|3)", [&](auto& args) { m_simulator->set_lan_connection_link_status(std::stol(args[1])); }, 2);
        add_command("6", "6) set alarm summary (0|1|2|3)", [&](auto& args) { m_simulator->set_alarm_summary(std::stol(args[1])); }, 2);
        add_command("9", "9) set response A99 <enable: 1|0> [reason: 1|2]", [&](auto& args) { args.emplace_back("1"); m_simulator->set_response_nack(std::stoi(args[1]), std::stoi(args[2])); });
        add_command("0", "0) reset all to default", [&](auto& args) { m_simulator->reset_all_to_default(); });
    }

    void add_additional_commands()
    {
        add_command("show-details, show-detail", "show-details message-list: show message details", [&](auto& args)
        {
            m_simulator->show_details(stdex::join(args, ","));
        }, 2);

        add_command("set-pid", "set-pid id value: edit pid value [0:off, 1:on, 2: alarm]", [&](auto& args)
        {
            m_simulator->set_pid_status(args[1], std::stoi(args[2]));
        }, 3);
    }

    void show_usage()
    {
        std::system("cls");

        auto title = str(boost::format("TITLE %s STIS SIMULATOR") % boost::to_upper_copy(m_simulator->get_name()));
        std::system(title.c_str());

        m_simulator->dump(std::cout);

        std::cout << std::endl;

        std::stringstream ss;

        for (auto&& cmd : m_commands)
        {
            ss << cmd.desc << std::endl;
        }

        ss << "========================================" << std::endl;
        std::cout << ss.str();

        std::cout << boost::to_upper_copy(m_simulator->get_name()) << "> " << std::flush;
    };

    void add_command(std::string cmds, std::string desc, std::function<void(std::vector<std::string>)> func, int min_args = 1, std::string help = {})
    {
        auto ids = st::splitted(cmds, ",;:", "--minimize");
        Command cmd;
        cmd.ids = {ids.begin(), ids.end()};
        cmd.func = std::move(func);
        cmd.min_args = min_args;
        cmd.desc = desc;
        cmd.help = help.size() ? help : desc;
        m_commands.emplace_back(std::move(cmd));
    }

    Command& get_command(std::string id)
    {
        return m_commands.get_if([&](auto& c) { return c.ids.count(id); });
    }

    void run()
    {
        show_usage();

        for (std::string line; std::getline(std::cin, line); show_usage())
        {
            if (boost::trim(line); line.empty())
            {
                if (m_block_print_seconds)
                {
                    static auto timer = SimpleTimer{};
                    m_simulator->enable_receive_response_output(false);
                    timer.submit_once("enable output", m_block_print_seconds * 1000, [=] { m_simulator->enable_receive_response_output(true); });
                }

                continue;
            }

            if (stdex::any_of_iequal("help ?"_split, line))
            {
                std::cout << STISSimulator::help() << std::endl;
                std::system("pause");
                continue;
            }

            if (stdex::any_of_iequal("quit exit q"_split, line))
            {
                DebugUtil::getInstance().setLevel("NONE");
                ::_exit(0);
            }

            auto args = st::splitted(line, " ", "--minimize");
            auto it = boost::find_if(m_commands, [&](auto& c) { return c.ids.count(args[0]); });

            if (it == m_commands.end())
            {
                std::cout << "error: bad command" << std::endl;
                std::system("pause");
                continue;
            }

            auto& cmd = *it;

            if (args.size() < cmd.min_args)
            {
                std::cout << "error: bad args" << std::endl;
                std::cout << "usage:" << std::endl;
                std::cout << "    " << cmd.help << std::endl;
                std::system("pause");
                continue;
            }

            try
            {
                cmd.func(args);
            }
            catch (std::exception& e)
            {
                std::cout << "error: " << e.what() << std::endl;
                std::system("pause");
            }
            catch (...)
            {
                std::cout << "error: execution failed" << std::endl;
                std::system("pause");
            }

            m_simulator->enable_receive_response_output(true);
        }
    }

    int m_block_print_seconds = RunParamsEx::get_or("block-print-seconds", 1);
    CommandList m_commands;
    STISSimulatorPtr m_simulator;
};

UserInterface::UserInterface(STISSimulatorPtr simulator)
    : m_impl(std::make_shared<Impl>(std::move(simulator)))
{
}

void UserInterface::run()
{
    m_impl->run();
}

void UserInterface::add_command(std::string cmd, std::string desc, std::function<void(std::vector<std::string>)> f)
{
    m_impl->add_command(std::move(cmd), std::move(desc), std::move(f));
}
