#include "pch.h"
#include "STISLibraryVersionsSQLite.h"
#include "CommonDefs.h"
#include "app/signs/common_library/src/STISUtility.h"
#include "core/utilities/src/CallstackLogger.h"
#include "core/utility/src/base_ex/DebugUtilEx.h"
#include "core/utility/src/base_ex/SimpleSQLite.h"
#include "core/utility/src/base_ex/RunParamsEx.h"
#include "core/utility/src/core/FileEx.h"
#include "core/utility/src/core/StdEx.h"
#include "core/utility/src/core/StaticObject.h"
#include "core/utility/src/core/algorithm/files.h"
#include "core/utility/src/core/fixed_length_data/all.h"
#include "core/utility/src/core/CacheDecorator.h"
#include "core/utility/src/core/MakeNameValuePairsString.h"
#include "core/utility/src/core/algorithm/file_system.h"

#define RPARAM_DEBUGSTISLIBRARYVERSIONSSQLITE "DebugSTISLibraryVersionsSQLite"

using namespace std::string_literals;
using namespace boost::program_options;
using boost::filesystem::path;
using st::FileEx;
using st::make_time_cached;
using st::fixed_length_data::Integer;
using st::StaticObject;
using st2::SimpleMD5FileManager;
using namespace TA_Base_Ex;
using TA_IRS_App::STIS_UTILITY::Version;
using ScopedLock = std::scoped_lock<std::recursive_mutex>;

namespace TA_IRS_App::STIS_PROTOCOL::IMPL::stislibraryversionssqlite::detail
{
    struct STISLibraryVersionsSQLite::Impl
    {
        Impl() = default;

        Impl(std::string options)
        {
            parse_options(std::move(options));
        }

        SimpleSQLitePtr get_db()
        {
            on_pre_query();

            if (auto shared = m_weak_db.lock())
            {
                return shared;
            }

            auto debug = RunParamsEx::is_true(RPARAM_DEBUGSTISLIBRARYVERSIONSSQLITE, "--quiet");
            auto db = std::make_shared<SimpleSQLite>(m_db_filename, debug ? "" : "--quiet");
            db->execute("CREATE TABLE IF NOT EXISTS versions (name, version, UNIQUE(name));");
            m_weak_db = db;
            return db;
        }

        VersionMap get_versions()
        {
            LOG_CALLSTACK("STISLibraryVersionsSQLite::get_versions");

            auto l = lock();

            if (!exists(m_db_filename))
            {
                m_md5_mgr.remove_md5(m_filename);
                m_cache.clear();
                m_md5.clear();
                return {};
            }

            auto md5 = m_md5_mgr.get_file_md5(m_db_filename);

            if (m_md5 == md5)
            {
                return m_cache;
            }

            m_cache.clear();

            try
            {
                VersionMap versions;

                for (auto&& row : get_db()->get_table("SELECT name, version FROM versions;"))
                {
                    versions.emplace(row[0], Version{std::move(row[1])});
                }

                m_md5 = std::move(md5);
                m_cache = std::move(versions);
            }
            catch (std::exception& e)
            {
                LOG_ERROR("get_versions(): %s", e);
            }

            return m_cache;
        }

        void set_versions(const VersionMap& versions)
        {
            LOG_CALLSTACK("STISLibraryVersionsSQLite::set_versions");

            auto l = lock();

            auto db = get_db();
            auto old_versions = get_versions();

            if (old_versions == versions)
            {
                return;
            }

            insert_or_update_versions_impl(db, old_versions, versions);

            try
            {
                for (auto&& [name, _] : old_versions)
                {
                    if (versions.count(name) == 0)
                    {
                        db->execute("DELETE FROM versions WHERE name=?;", {name});
                        LOG_DEBUG("set_versions(): DELETE %s", name);
                    }
                }
            }
            catch (std::exception& e)
            {
                LOG_ERROR("set_versions(): %s", e);
            }
        }

        void update_versions(const VersionMap& versions)
        {
            LOG_CALLSTACK("STISLibraryVersionsSQLite::update_versions");

            auto l = lock();

            auto db = get_db();
            auto old_versions = get_versions();

            if (old_versions == versions)
            {
                return;
            }

            insert_or_update_versions_impl(db, old_versions, versions);
        }

        void remove_db_file()
        {
            auto l = lock();
            boost::system::error_code ec;
            remove(m_db_filename, ec);
        }

        void register_pre_query_callback(std::string name, std::function<void()> callback)
        {
            m_pre_query_callbacks.insert_or_assign(std::move(name), std::move(callback));
        }

        void register_pre_update_callback(std::string name, std::function<void()> callback)
        {
            m_pre_update_callbacks.insert_or_assign(std::move(name), std::move(callback));
        }

        void remove_all_callbacks()
        {
            m_pre_query_callbacks.clear();
            m_pre_update_callbacks.clear();
        }

        // implementation

        void insert_or_update_versions_impl(SimpleSQLitePtr db, const VersionMap& old_versions, const VersionMap& new_versions)
        {
            on_pre_update();

            try
            {
                for (auto&& [name, version] : new_versions)
                {
                    if (old_versions.count(name))
                    {
                        db->execute("UPDATE versions SET version=? WHERE name=?;", {Version{version}.value(), name});
                    }
                    else
                    {
                        db->execute("INSERT INTO versions VALUES(?, ?);", {name, Version{version}.value()});
                    }

                    LOG_DEBUG("insert_or_update_versions_impl(): %s=%s", name, version);
                }
            }
            catch (std::exception& e)
            {
                LOG_ERROR("insert_or_update_versions_impl(): %s", e);
            }
        }

        void on_pre_query()
        {
            LOG_CALLSTACK("STISLibraryVersionsSQLite::on_pre_query");
            boost::for_each(m_pre_query_callbacks, [](auto& pair) { pair.second(); });
        }

        void on_pre_update()
        {
            LOG_CALLSTACK("STISLibraryVersionsSQLite::on_pre_update");
            boost::for_each(m_pre_update_callbacks, [](auto& pair) { pair.second(); });
        }

        ScopedLockPtr lock()
        {
            return std::make_shared<ScopedLock>(m_mutex);
        }

        void parse_options(std::string options)
        {
            LOG_CALLSTACK("STISLibraryVersionsSQLite::parse_options");

            if (options == m_options)
            {
                return;
            }

            m_options = std::move(options);
            options_description desc;
            desc.add_options()
                ("root-dir", value<std::string>())
                ("filename", value<std::string>())
                ("versions-filename", value<std::string>())
                ;

            st::set_value_from_options(m_options, desc)
                .set<path, std::string>(m_root, {"root-dir"})
                .set<path, std::string>(m_filename, {"filename", "versions-filename"})
                ;

            m_db_filename = st2::absoluted(m_root) / st2::relatived(m_filename, m_root);
            m_md5_mgr.set_root_dirs(m_root, m_root / ".md5");
        }

        std::string m_options = "uninitialized";
        std::string m_md5;
        path m_root = st2::absoluted_copy(RunParamsEx::expand(RunParamsEx::get_or(RPARAM_STISROOTDIR, DEFAULT_STIS_ROOT_DIR)));
        path m_filename = RunParamsEx::expand(RunParamsEx::get_or(RPARAM_STISLIBRARYVERSIONSFILENAME, DEFAULT_STIS_LIBRARY_VERSIONS_FILENAME));
        path m_db_filename = m_root / m_filename;
        SimpleMD5FileManager m_md5_mgr;
        VersionMap m_cache;
        std::weak_ptr<SimpleSQLite> m_weak_db;
        std::map<std::string, std::function<void()>> m_pre_query_callbacks;
        std::map<std::string, std::function<void()>> m_pre_update_callbacks;
        std::recursive_mutex m_mutex;
    };

    STISLibraryVersionsSQLite& STISLibraryVersionsSQLite::instance()
    {
        return StaticObject<STISLibraryVersionsSQLite>::value();
    }

    STISLibraryVersionsSQLite::STISLibraryVersionsSQLite(std::string options)
        : m_impl(std::make_shared<Impl>(std::move(options)))
    {
    }

    void STISLibraryVersionsSQLite::pare_options(std::string options)
    {
        m_impl->parse_options(std::move(options));
    }

    VersionMap STISLibraryVersionsSQLite::get_versions()
    {
        return m_impl->get_versions();
    }

    void STISLibraryVersionsSQLite::update_versions(const VersionMap& versions)
    {
        m_impl->update_versions(versions);
    }

    void STISLibraryVersionsSQLite::remove_db_file()
    {
        m_impl->remove_db_file();
    }

    void STISLibraryVersionsSQLite::register_pre_query_callback(std::string name, std::function<void()> callback)
    {
        m_impl->register_pre_query_callback(std::move(name), std::move(callback));
    }

    void STISLibraryVersionsSQLite::register_pre_update_callback(std::string name, std::function<void()> callback)
    {
        m_impl->register_pre_update_callback(std::move(name), std::move(callback));
    }

    void STISLibraryVersionsSQLite::remove_all_callbacks()
    {
        m_impl->remove_all_callbacks();
    }

    ScopedLockPtr STISLibraryVersionsSQLite::lock()
    {
        return m_impl->lock();
    }
}
