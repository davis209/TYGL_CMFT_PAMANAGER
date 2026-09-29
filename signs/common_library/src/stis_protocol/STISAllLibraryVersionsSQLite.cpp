#include "pch.h"
#include "STISAllLibraryVersionsSQLite.h"
#include "CommonDefs.h"
#include "core/utilities/src/CallstackLogger.h"
#include "core/utility/src/base_ex/DebugUtilEx.h"
#include "core/utility/src/base_ex/SimpleSQLite.h"
#include "core/utility/src/base_ex/RunParamsEx.h"
#include "core/utility/src/core/FileEx.h"
#include "core/utility/src/core/StdEx.h"
#include "core/utility/src/core/StaticObject.h"
#include "core/utility/src/core/algorithm/files.h"
#include "core/utility/src/core/algorithm/strings.h"
#include "core/utility/src/core/fixed_length_data/all.h"
#include "core/utility/src/core/CacheDecorator.h"
#include "core/utility/src/core/MakeNameValuePairsString.h"
#include "core/utility/src/core/algorithm/file_system.h"

#define RPARAM_DEBUGSTISALLLIBRARYVERSIONSSQLITE "DebugSTISAllLibraryVersionsSQLite"

using namespace std::string_literals;
using namespace boost::program_options;
using boost::filesystem::path;
using st::FileEx;
using st::make_time_cached;
using st::fixed_length_data::Integer;
using st::fixed_length_data::DigitString;
using st::StaticObject;
using st2::SimpleMD5FileManager;

#define USE_LIKE 1 // FIXME: REPLACE name like ? WITH name=?

namespace
{
    const std::string VERSION_SEPERATOR = ",";
}

namespace TA_IRS_App::STIS_PROTOCOL::IMPL::stisalllibraryversionssqlite::detail
{
    struct STISAllLibraryVersionsSQLite::Impl
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

            auto debug = RunParamsEx::is_true(RPARAM_DEBUGSTISALLLIBRARYVERSIONSSQLITE, "--quiet");
            auto db = std::make_shared<SimpleSQLite>(m_db_filename, debug ? "" : "--quiet");
            db->execute("CREATE TABLE IF NOT EXISTS versions (name, version, UNIQUE(name));");
            m_weak_db = db;
            return db;
        }

        const VersionsMap& get_versions()
        {
            LOG_CALLSTACK("STISAllLibraryVersionsSQLite::get_versions");

            auto l = lock();

            if (!exists(m_db_filename))
            {
                m_md5_mgr.remove_md5(m_db_filename);
                m_cache.clear();
                m_md5.clear();
                return m_cache;
            }

            auto md5 = m_md5_mgr.get_file_md5(m_db_filename);

            if (m_md5 == md5)
            {
                return m_cache;
            }

            m_cache.clear();

            try
            {
                VersionsMap versions;

                for (auto& row : get_db()->get_table("SELECT name, version FROM versions;"))
                {
                    versions.emplace(std::move(row[0]), st2::from_csv(row[1], VERSION_SEPERATOR));
                }

                m_md5 = std::move(md5);
                m_cache = std::move(versions);
                normalize(m_cache);
            }
            catch (std::exception& e)
            {
                LOG_ERROR("get_versions(): %s", e);
            }

            return m_cache;
        }

        void set_versions(VersionsMap versions)
        {
            LOG_CALLSTACK("STISAllLibraryVersionsSQLite::set_versions");

            auto l = lock();

            try
            {
                normalize(versions);
                auto db = get_db();
                auto old_versions = get_versions();
                update_versions_impl(db, old_versions, versions);

                if (auto to_delete = st::filter_copy(st::keys(old_versions), [&](auto& name) { return versions.count(name) == 0; }); to_delete.size())
                {
                    boost::for_each(to_delete, [&](auto& name)
                    {
                        db->execute("DELETE FROM versions WHERE name=?;", {name});
                        LOG_DEBUG("set_versions(): DELETE %s", name);
                    });
                }
            }
            catch (std::exception& e)
            {
                LOG_ERROR("set_versions(): %s", e);
            }
        }

        void update_versions(VersionsMap versions)
        {
            LOG_CALLSTACK("STISAllLibraryVersionsSQLite::update_versions");
            auto l = lock();
            update_versions_impl(get_db(), get_versions(), std::move(normalize(versions)));
        }

        void update_versions_impl(SimpleSQLitePtr db, VersionsMap old_versions, VersionsMap versions)
        {
            try
            {
                if (old_versions == versions)
                {
                    LOG_TRACE("update_versions_impl(): %s", nvps(versions, old_versions));
                    return;
                }

                on_pre_update();

                for (auto&& [name, version] : versions)
                {
                    if (old_versions.count(name))
                    {
                        if (old_versions[name] != version)
                        {
#if USE_LIKE
                            db->execute("UPDATE versions SET version=? WHERE name LIKE ?;", {st2::to_csv(version, VERSION_SEPERATOR), name});
#else
                            db->execute("UPDATE versions SET version=? WHERE name=?;", {st2::to_csv(version, VERSION_SEPERATOR), name});
#endif
                            LOG_DEBUG("update_versions_impl(): %s=%s", name, st2::to_csv(version, VERSION_SEPERATOR));
                        }
                    }
                    else
                    {
                        db->execute("INSERT INTO versions VALUES(?, ?);", {name, st2::to_csv(version, VERSION_SEPERATOR)});
                        LOG_DEBUG("update_versions_impl(): %s=%s", name, st2::to_csv(version, VERSION_SEPERATOR));
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
            LOG_CALLSTACK("STISAllLibraryVersionsSQLite::update_versions");

            auto l = lock();

            if (4 == versions.size()) // assume first 4 items (iscs versions)
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
            LOG_CALLSTACK("STISAllLibraryVersionsSQLite::delete_versions");

            auto l = lock();

            auto db = get_db();
            auto old_versions = get_versions();

            if (auto to_delete = st::set_intersection(names, st::keys(old_versions)); to_delete.size())
            {
                on_pre_update();

                for (auto& name : to_delete)
                {
#if USE_LIKE
                    db->execute("DELETE FROM versions WHERE name LIKE ?;", {name});
#else
                    db->execute("DELETE FROM versions WHERE name=?;", {name});
#endif
                    LOG_DEBUG("delete_versions(): DELETE %s", name);
                }
            }
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

        void on_pre_query()
        {
            LOG_CALLSTACK("STISAllLibraryVersionsSQLite::on_pre_query");
            boost::for_each(m_pre_query_callbacks, [](auto& pair) { pair.second(); });
        }

        void on_pre_update()
        {
            LOG_CALLSTACK("STISAllLibraryVersionsSQLite::on_pre_update");
            boost::for_each(m_pre_update_callbacks, [](auto& pair) { pair.second(); });
        }

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

        ScopedLockPtr lock()
        {
            return std::make_shared<ScopedLock>(m_mutex);
        }

        void parse_options(std::string options)
        {
            LOG_CALLSTACK("STISAllLibraryVersionsSQLite::parse_options");

            if (options == m_options)
            {
                return;
            }

            m_options = std::move(options);
            options_description desc;
            desc.add_options()
                ("root-dir", value<std::string>())
                ("stis-root-dir", value<std::string>())
                ("filename", value<std::string>())
                ("all-versions-filename", value<std::string>())
                ;

            st::set_value_from_options(m_options, desc)
                .set<path, std::string>(m_root, {"root-dir", "stis-root-dir"})
                .set<path, std::string>(m_filename, {"filename", "all-versions-filename"})
                ;

            m_db_filename = st2::absoluted(m_root) / st2::relatived(m_filename, m_root);
            m_md5_mgr.set_root_dirs(m_root, m_root / ".md5");
        }

        std::string m_options = "uninitialized";
        std::string m_md5;
        path m_root = st2::absoluted_copy(RunParamsEx::expand(RunParamsEx::get_or(RPARAM_STISROOTDIR, DEFAULT_STIS_ROOT_DIR)));
        path m_filename = RunParamsEx::expand(RunParamsEx::get_or(RPARAM_STISALLLIBRARYVERSIONSFILENAME, DEFAULT_STIS_ALL_LIBRARY_VERSIONS_FILENAME));
        path m_db_filename = m_root / m_filename;
        SimpleMD5FileManager m_md5_mgr;
        VersionsMap m_cache;
        std::weak_ptr<SimpleSQLite> m_weak_db;
        std::map<std::string, std::function<void()>> m_pre_query_callbacks;
        std::map<std::string, std::function<void()>> m_pre_update_callbacks;
        std::recursive_mutex m_mutex;
    };

    STISAllLibraryVersionsSQLite& STISAllLibraryVersionsSQLite::instance()
    {
        return StaticObject<STISAllLibraryVersionsSQLite>::value();
    }

    STISAllLibraryVersionsSQLite::STISAllLibraryVersionsSQLite(std::string options)
        : m_impl(std::make_shared<Impl>(std::move(options)))
    {
    }

    void STISAllLibraryVersionsSQLite::pare_options(std::string options)
    {
        m_impl->parse_options(std::move(options));
    }

    const VersionsMap& STISAllLibraryVersionsSQLite::get_versions()
    {
        return m_impl->get_versions();
    }

    void STISAllLibraryVersionsSQLite::set_versions(const VersionsMap& versions)
    {
        m_impl->set_versions(versions);
    }

    void STISAllLibraryVersionsSQLite::update_versions(const VersionsMap& versions)
    {
        m_impl->update_versions(versions);
    }

    void STISAllLibraryVersionsSQLite::update_versions(const std::string& name, const std::vector<std::string>& versions)
    {
        m_impl->update_versions(name, versions);
    }

    void STISAllLibraryVersionsSQLite::delete_versions(const std::vector<std::string>& names)
    {
        m_impl->delete_versions(names);
    }

    void STISAllLibraryVersionsSQLite::remove_db_file()
    {
        m_impl->remove_db_file();
    }

    void STISAllLibraryVersionsSQLite::register_pre_query_callback(std::string name, std::function<void()> callback)
    {
        m_impl->register_pre_query_callback(std::move(name), std::move(callback));
    }

    void STISAllLibraryVersionsSQLite::register_pre_update_callback(std::string name, std::function<void()> callback)
    {
        m_impl->register_pre_update_callback(std::move(name), std::move(callback));
    }

    void STISAllLibraryVersionsSQLite::remove_all_callbacks()
    {
        m_impl->remove_all_callbacks();
    }

    ScopedLockPtr STISAllLibraryVersionsSQLite::lock()
    {
        return m_impl->lock();
    }
}
