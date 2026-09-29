#pragma once
#include <string>
#include <map>
#include <memory>
#include <functional>
#include <mutex>

namespace TA_IRS_App::STIS_PROTOCOL::IMPL::stisalllibraryversionssqlite::detail
{
    using VersionsMap = std::map<std::string, std::vector<std::string>>;
    using ScopedLock = std::scoped_lock<std::recursive_mutex>;
    using ScopedLockPtr = std::shared_ptr<ScopedLock>;

    /**
     * STISAllLibraryVersionsSQLite
     *
     * table: versions (name, version)
     */
    struct STISAllLibraryVersionsSQLite
    {
        static STISAllLibraryVersionsSQLite& instance();

        STISAllLibraryVersionsSQLite(std::string options = "");

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

    using STISLibraryVersionsSQLitePtr = std::shared_ptr<STISAllLibraryVersionsSQLite>;
}

namespace TA_IRS_App::STIS_PROTOCOL::IMPL
{
    using stisalllibraryversionssqlite::detail::STISAllLibraryVersionsSQLite;
    //using stisalllibraryversionssqlite::detail::STISLibraryVersionsSQLitePtr;
}
