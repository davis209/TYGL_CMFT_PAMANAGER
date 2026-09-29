#include "pch.h"
#include "STISAllLibraryVersionsMySQL.h"
#include "STISDatabaseCommon.h"
#include "core/utilities/src/CallstackLogger.h"
#include "core/utility/src/base_ex/DebugUtilEx.h"
#include "core/utility/src/core/StaticObject.h"
#include "core/utility/src/core/StdEx.h"
#include "core/utility/src/core/MakeNameValuePairsString.h"
#include "core/utility/src/core/fixed_length_data/all.h"
#include "core/utility/src/core/algorithm/strings.h"

using st::StaticObject;
using st::fixed_length_data::DigitString;
using namespace TA_Base_Core;

namespace
{
    const std::string VERSION_SEPERATOR = ",";
    const std::string TABLE_NAME = "stis_all_library_versions";
}

namespace TA_IRS_App::STIS_PROTOCOL::IMPL::stisalllibraryversionsmysql::detail
{
    struct STISAllLibraryVersionsMySQL::Impl
    {
        Impl() = default;
        Impl(std::string options) { parse_options(std::move(options)); }

        IDatabase* read_db()  { ensure_table(); return stisdatabase::detail::get_stis_read_database(m_database_type); }
        IDatabase* write_db() { ensure_table(); return stisdatabase::detail::get_stis_write_database(m_database_type); }

        void ensure_table()
        {
            auto l = lock();
            if (m_table_checked) return;

            auto db = stisdatabase::detail::get_stis_write_database(m_database_type);
            stisdatabase::detail::execute_modification(db,
                "CREATE TABLE IF NOT EXISTS " + TABLE_NAME + " ("
                "name VARCHAR(128) NOT NULL, "
                "version TEXT NOT NULL, "
                "updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP, "
                "PRIMARY KEY (name))");
            m_table_checked = true;
        }

        const VersionsMap& get_versions()
        {
            LOG_CALLSTACK("STISAllLibraryVersionsMySQL::get_versions");
            auto l = lock();
            on_pre_query();

            try
            {
                VersionsMap versions;
                auto db = read_db();

                std::vector<std::string> columns;
                columns.push_back("name");
                columns.push_back("version");

                auto* data = stisdatabase::detail::execute_query(db,
                    "SELECT name, version FROM " + TABLE_NAME,
                    columns);
                stisdatabase::detail::for_each_row(db, data, [&](IData& rowset, unsigned long row)
                {
                    versions.emplace(rowset.getStringData(row, "name"), st2::from_csv(rowset.getStringData(row, "version"), VERSION_SEPERATOR));
                });

                normalize(versions);
                m_cache = std::move(versions);
            }
            catch (std::exception& e)
            {
                LOG_ERROR("get_versions(): %s", e);
            }
            return m_cache;
        }

        void set_versions(VersionsMap versions)
        {
            LOG_CALLSTACK("STISAllLibraryVersionsMySQL::set_versions");
            auto l = lock();
            normalize(versions);

            try
            {
                auto old_versions = get_versions();
                update_versions_impl(old_versions, versions);

                for (auto&& [name, _] : old_versions)
                {
                    if (versions.count(name) == 0)
                    {
                        auto db = write_db();
                        stisdatabase::detail::execute_modification(db,
                            "DELETE FROM " + TABLE_NAME + " WHERE name = " + stisdatabase::detail::sql_string(name));
                        LOG_DEBUG("set_versions(): DELETE %s", name);
                    }
                }
            }
            catch (std::exception& e)
            {
                LOG_ERROR("set_versions(): %s", e);
            }
        }

        void update_versions(VersionsMap versions)
        {
            LOG_CALLSTACK("STISAllLibraryVersionsMySQL::update_versions");
            auto l = lock();
            update_versions_impl(get_versions(), std::move(normalize(versions)));
        }

        void update_versions_impl(VersionsMap old_versions, VersionsMap versions)
        {
            try
            {
                if (old_versions == versions)
                {
                    LOG_TRACE("update_versions_impl(): %s", nvps(versions, old_versions));
                    return;
                }

                on_pre_update();
                auto db = write_db();

                for (auto&& [name, version] : versions)
                {
                    if (!old_versions.count(name) || old_versions[name] != version)
                    {
                        const auto csv = st2::to_csv(version, VERSION_SEPERATOR);
                        const std::string sql =
                            "INSERT INTO " + TABLE_NAME + " (name, version) VALUES (" +
                            stisdatabase::detail::sql_string(name) + ", " +
                            stisdatabase::detail::sql_string(csv) + ") " +
                            "ON DUPLICATE KEY UPDATE version = VALUES(version)";
                        stisdatabase::detail::execute_modification(db, sql);
                        LOG_DEBUG("update_versions_impl(): %s=%s", name, csv);
                    }
                }
            }
            catch (std::exception& e)
            {
                LOG_ERROR("update_versions_impl(): %s", e);
            }
        }

        void update_versions(const std::string& name, const std::vector<std::string>& versions)
        {
            LOG_CALLSTACK("STISAllLibraryVersionsMySQL::update_versions");
            auto l = lock();

            if (4 == versions.size())
            {
                auto all_station_versions = get_versions();
                auto this_station_versions = all_station_versions[name];

                if (this_station_versions.empty())
                {
                    this_station_versions.resize(8);
                }

                auto old_iscs_versions = std::vector<std::string>(this_station_versions.begin(), next(this_station_versions.begin(), 4));

                if (versions != old_iscs_versions)
                {
                    std::copy(versions.begin(), versions.end(), this_station_versions.begin());
                }

                update_versions({{name, std::move(this_station_versions)}});
            }
            else
            {
                update_versions({{name, versions}});
            }
        }

        void delete_versions(const std::vector<std::string>& names)
        {
            LOG_CALLSTACK("STISAllLibraryVersionsMySQL::delete_versions");
            auto l = lock();

            try
            {
                auto old_versions = get_versions();
                auto db = write_db();

                if (auto to_delete = st::set_intersection(names, st::keys(old_versions)); to_delete.size())
                {
                    on_pre_update();
                    for (auto& name : to_delete)
                    {
                        stisdatabase::detail::execute_modification(db,
                            "DELETE FROM " + TABLE_NAME + " WHERE name = " + stisdatabase::detail::sql_string(name));
                        LOG_DEBUG("delete_versions(): DELETE %s", name);
                    }
                }
            }
            catch (std::exception& e)
            {
                LOG_ERROR("delete_versions(): %s", e);
            }
        }

        void remove_db_file()
        {
            LOG_CALLSTACK("STISAllLibraryVersionsMySQL::remove_db_file");
            auto l = lock();
            try
            {
                auto db = write_db();
                stisdatabase::detail::execute_modification(db, "DELETE FROM " + TABLE_NAME);
            }
            catch (std::exception& e)
            {
                LOG_ERROR("remove_db_file(): %s", e);
            }
        }

        void register_pre_query_callback(std::string name, std::function<void()> callback) { m_pre_query_callbacks.insert_or_assign(std::move(name), std::move(callback)); }
        void register_pre_update_callback(std::string name, std::function<void()> callback) { m_pre_update_callbacks.insert_or_assign(std::move(name), std::move(callback)); }
        void remove_all_callbacks() { m_pre_query_callbacks.clear(); m_pre_update_callbacks.clear(); }
        void on_pre_query() { boost::for_each(m_pre_query_callbacks, [](auto& pair) { pair.second(); }); }
        void on_pre_update() { boost::for_each(m_pre_update_callbacks, [](auto& pair) { pair.second(); }); }

        VersionsMap& normalize(VersionsMap& versions)
        {
            for (auto&& [name, version8] : versions)
            {
                for (auto& version : version8)
                {
                    if (version.size() < 3)
                    {
                        version = DigitString<3>{version};
                    }
                }
            }
            return versions;
        }

        ScopedLockPtr lock() { return std::make_shared<ScopedLock>(m_mutex); }

        void parse_options(std::string options)
        {
            if (options == m_options) return;
            m_options = std::move(options);
            m_database_type = Tis_Cd;
            m_table_checked = false;
            m_cache.clear();
        }

        std::string m_options = "uninitialized";
        EDataTypes m_database_type = Tis_Cd;
        bool m_table_checked = false;
        VersionsMap m_cache;
        std::map<std::string, std::function<void()>> m_pre_query_callbacks;
        std::map<std::string, std::function<void()>> m_pre_update_callbacks;
        std::recursive_mutex m_mutex;
    };

    STISAllLibraryVersionsMySQL& STISAllLibraryVersionsMySQL::instance() { return StaticObject<STISAllLibraryVersionsMySQL>::value(); }
    STISAllLibraryVersionsMySQL::STISAllLibraryVersionsMySQL(std::string options) : m_impl(std::make_shared<Impl>(std::move(options))) {}
    void STISAllLibraryVersionsMySQL::pare_options(std::string options) { m_impl->parse_options(std::move(options)); }
    const VersionsMap& STISAllLibraryVersionsMySQL::get_versions() { return m_impl->get_versions(); }
    void STISAllLibraryVersionsMySQL::set_versions(const VersionsMap& versions) { m_impl->set_versions(versions); }
    void STISAllLibraryVersionsMySQL::update_versions(const VersionsMap& versions) { m_impl->update_versions(versions); }
    void STISAllLibraryVersionsMySQL::update_versions(const std::string& name, const std::vector<std::string>& versions) { m_impl->update_versions(name, versions); }
    void STISAllLibraryVersionsMySQL::delete_versions(const std::vector<std::string>& names) { m_impl->delete_versions(names); }
    void STISAllLibraryVersionsMySQL::remove_db_file() { m_impl->remove_db_file(); }
    void STISAllLibraryVersionsMySQL::register_pre_query_callback(std::string name, std::function<void()> callback) { m_impl->register_pre_query_callback(std::move(name), std::move(callback)); }
    void STISAllLibraryVersionsMySQL::register_pre_update_callback(std::string name, std::function<void()> callback) { m_impl->register_pre_update_callback(std::move(name), std::move(callback)); }
    void STISAllLibraryVersionsMySQL::remove_all_callbacks() { m_impl->remove_all_callbacks(); }
    ScopedLockPtr STISAllLibraryVersionsMySQL::lock() { return m_impl->lock(); }
}
