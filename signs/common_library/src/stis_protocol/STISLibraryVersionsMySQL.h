#pragma once
#include <string>
#include <map>
#include <memory>
#include <functional>
#include <mutex>

namespace TA_IRS_App::STIS_PROTOCOL::IMPL::stislibraryversionsmysql::detail
{
    struct STISLibraryVersionsMySQL;
    using STISLibraryVersionsMySQLPtr = std::shared_ptr<STISLibraryVersionsMySQL>;
    using VersionMap = std::map<std::string, std::string>;
    using ScopedLock = std::scoped_lock<std::recursive_mutex>;
    using ScopedLockPtr = std::shared_ptr<ScopedLock>;

    /**
     * STISLibraryVersionsMySQL
     *
     * DatabaseFactory/IDatabase backed variant of STISLibraryVersionsSQLite.
     * table: stis_library_versions (name, version)
     */
    struct STISLibraryVersionsMySQL
    {
        static STISLibraryVersionsMySQL& instance();

        STISLibraryVersionsMySQL(std::string options = "");

        void pare_options(std::string options);

        VersionMap get_versions();
        void update_versions(const VersionMap& versions);
        void remove_db_file();

        void register_pre_query_callback(std::string name, std::function<void()> callback);
        void register_pre_update_callback(std::string name, std::function<void()> callback);
        void remove_all_callbacks();

        ScopedLockPtr lock();

        struct Impl;
        std::shared_ptr<Impl> m_impl;
    };
}

namespace TA_IRS_App::STIS_PROTOCOL::IMPL
{
    using stislibraryversionsmysql::detail::STISLibraryVersionsMySQL;
    using stislibraryversionsmysql::detail::STISLibraryVersionsMySQLPtr;
}
