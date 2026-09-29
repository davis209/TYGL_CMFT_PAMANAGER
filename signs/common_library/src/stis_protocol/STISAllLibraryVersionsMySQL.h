#pragma once
#include <string>
#include <map>
#include <memory>
#include <functional>
#include <mutex>
#include <vector>

namespace TA_IRS_App::STIS_PROTOCOL::IMPL::stisalllibraryversionsmysql::detail
{
    using VersionsMap = std::map<std::string, std::vector<std::string>>;
    using ScopedLock = std::scoped_lock<std::recursive_mutex>;
    using ScopedLockPtr = std::shared_ptr<ScopedLock>;

    /**
     * STISAllLibraryVersionsMySQL
     *
     * DatabaseFactory/IDatabase backed variant of STISAllLibraryVersionsSQLite.
     * table: stis_all_library_versions (name, version)
     */
    struct STISAllLibraryVersionsMySQL
    {
        static STISAllLibraryVersionsMySQL& instance();

        STISAllLibraryVersionsMySQL(std::string options = "");

        void pare_options(std::string options);

        const VersionsMap& get_versions();
        void set_versions(const VersionsMap& versions);
        void update_versions(const VersionsMap& versions);
        void update_versions(const std::string& name, const std::vector<std::string>& versions);
        void delete_versions(const std::vector<std::string>& names);
        void remove_db_file();

        void register_pre_query_callback(std::string name, std::function<void()> callback);
        void register_pre_update_callback(std::string name, std::function<void()> callback);
        void remove_all_callbacks();

        ScopedLockPtr lock();

        struct Impl;
        std::shared_ptr<Impl> m_impl;
    };

    using STISAllLibraryVersionsMySQLPtr = std::shared_ptr<STISAllLibraryVersionsMySQL>;
}

namespace TA_IRS_App::STIS_PROTOCOL::IMPL
{
    using stisalllibraryversionsmysql::detail::STISAllLibraryVersionsMySQL;
    using stisalllibraryversionsmysql::detail::STISAllLibraryVersionsMySQLPtr;
}
