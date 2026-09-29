#include "pch.h"
#include "STISStatus.h"
#include "STISSimulator.h"
#include "CommonDefs.h"
#include "app/signs/common_library/src/stis_protocol/message_types/MessageTypes.h"
#include "core/utility/src/core/StdEx.h"
#include "core/utility/src/core/fixed_length_data/all.h"
#include "core/utility/src/base_ex/SimpleSQLite.h"

namespace TA_IRS_App::STIS_PROTOCOL::SIMULATOR::stissimulator::detail
{
    using namespace TA_Base_Ex;
    using namespace STIS_PROTOCOL::MESSAGE_TYPES;
    using st::fixed_length_data::Integer;
    using st::fixed_length_data::DigitString;
    using Version = DigitString<3>;

    struct STISStatus::Impl
    {
        Impl(std::string name)
        {
            set_name(std::move(name));
        }

        void set_name(std::string n)
        {
            name = (n.empty() ? "OCC"s : n);
            m_is_occ = boost::iequals(name, "OCC");
            set_station_id(name);
            init_versions_from_sqlite();
        }

        void set_station_id(std::string id)
        {
            m_station_id = std::move(id);
        }

        void set_occ_server_status(int status)
        {
            occ_server_status = status;
        }

        void set_connection_link_status(int status)
        {
            connection_link_status = status;
        }

        void set_lan_connection_link_status(int status)
        {
            if (status < 0 || 3 < status)
            {
                throw std::out_of_range{"LAN Connetion Link Status: 0,1,2,3"};
            }

            lan_connection_link_status = status;
        }

        void set_alarm_summary(int alarm)
        {
            if (alarm < 0 || 3 < alarm)
            {
                throw std::out_of_range{"Alarm Summary: 0,1,2,3"};
            }

            alarm_summary = alarm;
        }

        void set_current_message_library_version(std::string version)
        {
            Version ver{version};
            update_version_to_sqlite("message", "current", versions.current_message_library_version, ver.str());
            versions.current_message_library_version = ver.str();
        }

        void set_next_message_library_version(std::string version)
        {
            Version ver{version};
            update_version_to_sqlite("message", "next", versions.next_message_library_version, ver.str());
            versions.next_message_library_version = ver.str();
        }

        void set_current_template_library_version(std::string version)
        {
            Version ver{version};
            update_version_to_sqlite("template", "current", versions.current_template_library_version, ver.str());
            versions.current_template_library_version = ver.str();
        }

        void set_next_template_library_version(std::string version)
        {
            Version ver{version};
            update_version_to_sqlite("template", "next", versions.next_template_library_version, ver.str());
            versions.next_template_library_version = ver.str();
        }

        void set_versions(const Versions& versions)
        {
            set_current_message_library_version(std::get<0>(versions));
            set_next_message_library_version(std::get<1>(versions));
            set_current_template_library_version(std::get<2>(versions));
            set_next_template_library_version(std::get<3>(versions));
        }

        void update_version_to_sqlite(const std::string& category, const std::string& version, const std::string& old_number, const std::string& new_number)
        {
            if (is_occ() && old_number != new_number)
            {
                m_db->execute("UPDATE version SET number=? WHERE category=? AND version=?;", {new_number, category, version});
            }
        }

        std::ostream& dump(std::ostream& os) const
        {
            os << "========== " << boost::to_upper_copy(name) << " STIS SIMULATOR ==========" << std::endl;
            os << "current message library version:     " << versions.current_message_library_version << std::endl;
            os << "next message library version:        " << versions.next_message_library_version << std::endl;
            os << "current template library version:    " << versions.current_template_library_version << std::endl;
            os << "next template library version:       " << versions.next_template_library_version << std::endl;
            os << "lan connection link status:          " << lan_connection_link_status << std::endl;
            os << "alarm summary:                       " << alarm_summary << std::endl;

            if (response_nack)
            {
                os << "response A99:                        " << nack_reason << std::endl;
            }

            if (m_show_details.size())
            {
                os << "SHOW DETAILS: " << st::join(m_show_details, ",") << std::endl;
            }

            return os;
        }

        void reset()
        {
            nack_reason = 0;
            response_nack = false;
            occ_server_status = 0;
            connection_link_status = 0;
            lan_connection_link_status = 0;
            alarm_summary = 0;
            set_versions({"001", "001", "001", "001"});
        }

        std::string get_name() const
        {
            return name;
        }

        std::string get_station_id() const
        {
            return m_station_id;
        }

        int get_occ_server_status() const
        {
            return occ_server_status;
        }

        int get_connection_link_status() const
        {
            return connection_link_status;
        }

        int get_lan_connection_link_status() const
        {
            return lan_connection_link_status;
        }

        int get_alarm_summary() const
        {
            return alarm_summary;
        }

        Versions get_versions()
        {
            return versions.tied();
        }

        std::string get_current_message_library_version() const
        {
            return versions.current_message_library_version;
        }

        std::string get_next_message_library_version() const
        {
            return versions.next_message_library_version;
        }

        std::string get_current_template_library_version() const
        {
            return versions.current_template_library_version;
        }

        std::string get_next_template_library_version() const
        {
            return versions.next_template_library_version;
        }

        void set_response_nack(bool enable, int reason)
        {
            response_nack = enable;
            nack_reason = reason;
        }

        bool is_response_nack() const
        {
            return response_nack;
        }

        int get_nack_reason() const
        {
            return nack_reason;
        }

        void init_versions_from_sqlite()
        {
            if (!is_occ())
            {
                return;
            }

            m_db->execute("CREATE TABLE IF NOT EXISTS version (category, version, number, UNIQUE (category, version));");

            auto init = [&](auto category, auto version, auto get_version, auto set_version)
            {
                if (auto n = m_db->get_value("SELECT number FROM version WHERE category=? and version=?;", {category, version}); n.empty())
                {
                    m_db->execute("INSERT INTO version VALUES(?, ?, ?)", {category, version, std::invoke(get_version, this)});
                }
                else
                {
                    std::invoke(set_version, this, n);
                }
            };

            init("message", "current", &Impl::get_current_message_library_version, &Impl::set_current_message_library_version);
            init("message", "next", &Impl::get_next_message_library_version, &Impl::set_next_message_library_version);
            init("template", "current", &Impl::get_current_template_library_version, &Impl::set_current_template_library_version);
            init("template", "next", &Impl::get_next_template_library_version, &Impl::set_next_template_library_version);
        }

        bool is_occ() const
        {
            return m_is_occ;
        }

        std::vector<std::string> parse_details(std::string msgs)
        {
            boost::to_upper(msgs);

            if (stdex::any_of_iequal({"*", "ALL"}, msgs))
            {
                return ALL_MESSAGE_TYPES;
            }
            else if (stdex::any_of_iequal({"M*"}, msgs))
            {
                return ALL_M_MESSAGES_TYPES;
            }
            else if (stdex::any_of_iequal({"A*"}, msgs))
            {
                return ALL_A_MESSAGE_TYPES;
            }
            else
            {
                auto list = st::splitted(msgs, ",;:/");
                boost::remove_erase_if(list, [&](auto m) { return boost::count(ALL_MESSAGE_TYPES, m) == 0; });
                return boost::sort(list, [&](auto lhs, auto rhs)
                {
                    auto to_int = [](auto s) { return std::stoi(s.substr(1)) + (s[0] == 'A' ? 100 : 0); };
                    return to_int(lhs) < to_int(rhs);
                });
            }
        }

        void show_details(std::string msgs)
        {
            m_show_details = parse_details(msgs);
        }

        bool is_show_details(std::string msg)
        {
            return boost::count(m_show_details, boost::to_upper_copy(msg));
        }

        std::string name = "OCC";
        bool m_is_occ = true;
        int nack_reason = 0;
        bool response_nack = false;
        int occ_server_status = 0;
        int connection_link_status = 0;
        int lan_connection_link_status = 0;
        int alarm_summary = 0;
        std::string m_station_id = "OCC";
        CurNxtMsgTmpLibVers versions = std::tie("001", "001", "001", "001");
        std::string m_config = "stis_simulator.sqlite";
        SimpleSQLitePtr m_db = std::make_shared<SimpleSQLite>(m_config);
        std::vector<std::string> m_show_details;
    };

    STISStatus::STISStatus(std::string name)
        : m_impl(std::make_shared<Impl>(std::move(name)))
    {
    }

    std::ostream& STISStatus::dump(std::ostream& os) const
    {
        return m_impl->dump(os);
    }

    void STISStatus::reset()
    {
        m_impl->reset();
    }

    void STISStatus::set_name(std::string name)
    {
        m_impl->set_name(std::move(name));
    }

    void STISStatus::set_station_id(std::string id)
    {
        m_impl->set_station_id(std::move(id));
    }

    void STISStatus::set_occ_server_status(int status)
    {
        m_impl->set_occ_server_status(status);
    }

    void STISStatus::set_connection_link_status(int status)
    {
        m_impl->set_connection_link_status(status);
    }

    void STISStatus::set_lan_connection_link_status(int status)
    {
        m_impl->set_lan_connection_link_status(status);
    }

    void STISStatus::set_alarm_summary(int alarm)
    {
        m_impl->set_alarm_summary(alarm);
    }

    void STISStatus::set_versions(const Versions& versions)
    {
        m_impl->set_versions(versions);
    }

    void STISStatus::set_current_message_library_version(std::string version)
    {
        m_impl->set_current_message_library_version(std::move(version));
    }

    void STISStatus::set_next_message_library_version(std::string version)
    {
        m_impl->set_next_message_library_version(std::move(version));
    }

    void STISStatus::set_current_template_library_version(std::string version)
    {
        m_impl->set_current_template_library_version(std::move(version));
    }

    void STISStatus::set_next_template_library_version(std::string version)
    {
        m_impl->set_next_template_library_version(std::move(version));
    }

    std::string STISStatus::get_name() const
    {
        return m_impl->get_name();
    }

    bool STISStatus::is_occ() const
    {
        return m_impl->is_occ();
    }

    std::string STISStatus::get_station_id() const
    {
        return m_impl->get_station_id();
    }

    int STISStatus::get_occ_server_status() const
    {
        return m_impl->get_occ_server_status();
    }

    int STISStatus::get_connection_link_status() const
    {
        return m_impl->get_connection_link_status();
    }

    int STISStatus::get_lan_connection_link_status() const
    {
        return m_impl->get_lan_connection_link_status();
    }

    int STISStatus::get_alarm_summary() const
    {
        return m_impl->get_alarm_summary();
    }

    Versions STISStatus::get_versions()
    {
        return m_impl->get_versions();
    }

    std::string STISStatus::get_current_message_library_version() const
    {
        return m_impl->get_current_message_library_version();
    }

    std::string STISStatus::get_next_message_library_version() const
    {
        return m_impl->get_next_message_library_version();
    }

    std::string STISStatus::get_current_template_library_version() const
    {
        return m_impl->get_current_template_library_version();
    }

    std::string STISStatus::get_next_template_library_version() const
    {
        return m_impl->get_next_template_library_version();
    }

    void STISStatus::set_response_nack(bool enable, int reason)
    {
        m_impl->set_response_nack(enable, reason);
    }

    bool STISStatus::is_response_nack() const
    {
        return m_impl->is_response_nack();
    }

    int STISStatus::get_nack_reason() const
    {
        return m_impl->get_nack_reason();
    }

    void STISStatus::show_details(std::string msgs)
    {
        m_impl->show_details(msgs);
    }

    bool STISStatus::is_show_details(std::string msg_id)
    {
        return m_impl->is_show_details(msg_id);
    }
}
