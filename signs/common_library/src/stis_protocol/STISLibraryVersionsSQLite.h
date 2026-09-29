#pragma once
#include <string>
#include <map>
#include <memory>
#include <functional>
#include <mutex>

namespace TA_IRS_App::STIS_PROTOCOL::IMPL::stislibraryversionssqlite::detail
{
    struct STISLibraryVersionsSQLite;
    using STISLibraryVersionsSQLitePtr = std::shared_ptr<STISLibraryVersionsSQLite>;
    using VersionMap = std::map<std::string, std::string>;
    using ScopedLock = std::scoped_lock<std::recursive_mutex>;
    using ScopedLockPtr = std::shared_ptr<ScopedLock>;

    /**
     * STISLibraryVersionsSQLite
     *
     * table: versions (name, version)
     */
    struct STISLibraryVersionsSQLite
    {
        static STISLibraryVersionsSQLite& instance();

        STISLibraryVersionsSQLite(std::string options = "");

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
    using stislibraryversionssqlite::detail::STISLibraryVersionsSQLite;
    using stislibraryversionssqlite::detail::STISLibraryVersionsSQLitePtr;
}
