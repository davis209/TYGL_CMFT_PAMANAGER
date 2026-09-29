#pragma once
#include <tuple>
#include <string>
#include <memory>

namespace TA_IRS_App::STIS_PROTOCOL::SIMULATOR::stissimulator::detail
{
    using Versions = std::tuple<std::string, std::string, std::string, std::string>;

    struct STISStatus
    {
        STISStatus(std::string name = "OCC");

        void set_name(std::string name);
        void set_station_id(std::string id);
        void set_occ_server_status(int status);
        void set_connection_link_status(int status);
        void set_lan_connection_link_status(int status);
        void set_alarm_summary(int alarm);
        void set_current_message_library_version(std::string version);
        void set_next_message_library_version(std::string version);
        void set_current_template_library_version(std::string version);
        void set_next_template_library_version(std::string version);
        void set_versions(const Versions& versions);
        void set_response_nack(bool enable, int reason = 1);
        void reset();

        bool is_occ() const;
        std::string get_name() const;
        std::string get_station_id() const;
        int get_occ_server_status() const;
        int get_connection_link_status() const;
        int get_lan_connection_link_status() const;
        int get_alarm_summary() const;
        Versions get_versions();
        std::string get_current_message_library_version() const;
        std::string get_next_message_library_version() const;
        std::string get_current_template_library_version() const;
        std::string get_next_template_library_version() const;
        bool is_response_nack() const;
        int get_nack_reason() const;

        std::ostream& dump(std::ostream& os) const;
        void show_details(std::string msgs);
        bool is_show_details(std::string msg);

        struct Impl;
        std::shared_ptr<Impl> m_impl;
    };

    using STISStatusPtr = std::shared_ptr<STISStatus>;
}
