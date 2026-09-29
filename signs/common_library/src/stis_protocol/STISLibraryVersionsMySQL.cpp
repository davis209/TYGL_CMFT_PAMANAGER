#include "pch.h"
#include "STISLibraryVersionsMySQL.h"
#include "STISDatabaseCommon.h"
#include "app/signs/common_library/src/STISUtility.h"
#include "core/utilities/src/CallstackLogger.h"
#include "core/utility/src/base_ex/DebugUtilEx.h"
#include "core/utility/src/base_ex/RunParamsEx.h"
#include "core/utility/src/core/StdEx.h"
#include "core/utility/src/core/StaticObject.h"
#include "core/utility/src/core/MakeNameValuePairsString.h"
#include <boost/program_options.hpp>

using st::StaticObject;
using TA_IRS_App::STIS_UTILITY::Version;
using namespace TA_Base_Core;

namespace
{
    const std::string TABLE_NAME = "stis_library_versions";
}

namespace TA_IRS_App::STIS_PROTOCOL::IMPL::stislibraryversionsmysql::detail
{
    struct STISLibraryVersionsMySQL::Impl
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
                "version VARCHAR(32) NOT NULL, "
                "updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP, "
                "PRIMARY KEY (name))");
            m_table_checked = true;
        }

        VersionMap get_versions()
        {
            LOG_CALLSTACK("STISLibraryVersionsMySQL::get_versions");
            auto l = lock();
            on_pre_query();

            VersionMap versions;
            try
            {
                auto db = read_db();
                std::vector<std::string> columns;
                columns.push_back("name");
                columns.push_back("version");

                auto* data = stisdatabase::detail::execute_query(db,
                    "SELECT name, version FROM " + TABLE_NAME,
                    columns);
                stisdatabase::detail::for_each_row(db, data, [&](IData& rowset, unsigned long row)
                {
                    versions.emplace(rowset.getStringData(row, "name"), Version{rowset.getStringData(row, "version")});
                });

                m_cache = versions;
            }
            catch (std::exception& e)
            {
                LOG_ERROR("get_versions(): %s", e);
            }
            return versions;
        }

        void update_versions(const VersionMap& versions)
        {
            LOG_CALLSTACK("STISLibraryVersionsMySQL::update_versions");
            auto l = lock();
            on_pre_update();

            try
            {
                auto db = write_db();
                for (auto&& [name, version] : versions)
                {
                    const auto version_value = Version{version}.value();
                    const std::string sql =
                        "INSERT INTO " + TABLE_NAME + " (name, version) VALUES (" +
                        stisdatabase::detail::sql_string(name) + ", " +
                        stisdatabase::detail::sql_string(version_value) + ") " +
                        "ON DUPLICATE KEY UPDATE version = VALUES(version)";
                    stisdatabase::detail::execute_modification(db, sql);
                    LOG_DEBUG("update_versions(): %s=%s", name, version);
                }
            }
            catch (std::exception& e)
            {
                LOG_ERROR("update_versions(): %s", e);
            }
        }

        void remove_db_file()
        {
            LOG_CALLSTACK("STISLibraryVersionsMySQL::remove_db_file");
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
        ScopedLockPtr lock() { return std::make_shared<ScopedLock>(m_mutex); }

        void parse_options(std::string options)
        {
            if (options == m_options) return;
            m_options = std::move(options);
            m_database_type = Tis_Cd;
            m_table_checked = false;
        }

        std::string m_options = "uninitialized";
        EDataTypes m_database_type = Tis_Cd;
        bool m_table_checked = false;
        VersionMap m_cache;
        std::map<std::string, std::function<void()>> m_pre_query_callbacks;
        std::map<std::string, std::function<void()>> m_pre_update_callbacks;
        std::recursive_mutex m_mutex;
    };

    STISLibraryVersionsMySQL& STISLibraryVersionsMySQL::instance() { return StaticObject<STISLibraryVersionsMySQL>::value(); }
    STISLibraryVersionsMySQL::STISLibraryVersionsMySQL(std::string options) : m_impl(std::make_shared<Impl>(std::move(options))) {}
    void STISLibraryVersionsMySQL::pare_options(std::string options) { m_impl->parse_options(std::move(options)); }
    VersionMap STISLibraryVersionsMySQL::get_versions() { return m_impl->get_versions(); }
    void STISLibraryVersionsMySQL::update_versions(const VersionMap& versions) { m_impl->update_versions(versions); }
    void STISLibraryVersionsMySQL::remove_db_file() { m_impl->remove_db_file(); }
    void STISLibraryVersionsMySQL::register_pre_query_callback(std::string name, std::function<void()> callback) { m_impl->register_pre_query_callback(std::move(name), std::move(callback)); }
    void STISLibraryVersionsMySQL::register_pre_update_callback(std::string name, std::function<void()> callback) { m_impl->register_pre_update_callback(std::move(name), std::move(callback)); }
    void STISLibraryVersionsMySQL::remove_all_callbacks() { m_impl->remove_all_callbacks(); }
    ScopedLockPtr STISLibraryVersionsMySQL::lock() { return m_impl->lock(); }
}
