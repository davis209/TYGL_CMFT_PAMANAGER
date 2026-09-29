#pragma once
#include "app/signs/common_library/src/stis_protocol/message_types/MessageTypes.h"
#include <string>
#include <memory>
#include <tuple>
#include <iosfwd>

namespace TA_IRS_App::STIS_PROTOCOL::SIMULATOR::stissimulator::detail
{
    using VersionsTuple =  std::tuple<std::string, std::string, std::string, std::string>;

    struct STISSimulator
    {
        static STISSimulator& instance();

        STISSimulator(int argc, char* argv[]);
        STISSimulator(std::string options = "");

        void parse_options(std::string options);

        void start();
        void stop();

        void set_versions(const VersionsTuple& versions);
        void set_current_message_library_version(std::string version);
        void set_next_message_library_version(std::string version);
        void set_current_template_library_version(std::string version);
        void set_next_template_library_version(std::string version);
        void set_connection_link_status(int status);
        void set_occ_server_status(int status);
        void set_pid_status(std::string pid, int status);
        void set_response_nack(bool enable, int reason);
        std::pair<bool, int> set_response_nack();
        void reset_all_to_default();

        VersionsTuple get_versions();
        std::string get_name();
        bool is_occ();
        std::ostream& dump(std::ostream& os);
        std::string dump();
        bool is_online();
        void exec_system_cmd(std::string cmd);

        static std::string help();
        static std::string name_to_port(std::string name);
        static void enable_receive_response_output(bool enable = true);

        void show_details(std::string msgs);
        bool is_show_details(std::string msg);

        struct Impl;
        std::shared_ptr<Impl> m_impl;
    };

    using STISSimulatorPtr = std::shared_ptr<STISSimulator>;
}

namespace TA_IRS_App::STIS_PROTOCOL::SIMULATOR
{
    using stissimulator::detail::STISSimulator;
    using stissimulator::detail::STISSimulatorPtr;
}
