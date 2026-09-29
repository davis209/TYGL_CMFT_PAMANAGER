#include "pch.h"
#include "STISAdHocSQLite.h"
#include "CommonDefs.h"
#include "core/utility/src/base_ex/DebugUtilEx.h"
#include "core/utility/src/base_ex/SimpleSQLite.h"
#include "core/utility/src/base_ex/RunParamsEx.h"
#include "core/utility/src/core/FileEx.h"
#include "core/utility/src/core/StdEx.h"
#include "core/utility/src/core/Map.h"
#include "core/utility/src/core/StaticObject.h"
#include "core/utility/src/core/algorithm/file_system.h"
#include "core/utility/src/core/algorithm/files.h"
#include "core/utility/src/core/CacheDecorator.h"
#include "core/utility/src/core/MakeNameValuePairsString.h"

#define RPARAM_DEBUGSTISADHOCSQLITE "DebugSTISAdHocSQLite"

using boost::filesystem::path;
using namespace std::string_literals;
using st::FileEx;
using st::StaticObject;
using st2::SimpleMD5FileManager;
using st::make_time_cached;
using namespace TA_Base_Ex;

namespace TA_IRS_App::STIS_PROTOCOL::IMPL::stisadhocsqlite::detail
{
    using AdHocMessageThreadSafeMap = st::map<int, AdHocMessageItem>;

    struct STISAdHocSQLite::Impl
    {
        Impl(std::string options = "")
        {
            parse_options(std::move(options));
        }

        SimpleSQLitePtr get_db()
        {
            if (auto shared = m_weak_db.lock())
            {
                return shared;
            }

            auto debug = RunParamsEx::is_true(RPARAM_DEBUGSTISADHOCSQLITE, "--quiet");
            auto db = std::make_shared<SimpleSQLite>(m_db_filename, debug ? "" : "--quiet");
            db->execute("CREATE TABLE IF NOT EXISTS messages (key, title, content, UNIQUE(key));");
            m_weak_db = db;
            return db;
        }

        void set(int key, const std::string& title, const std::string& content)
        {
            LOG_CALLSTACK(boost::format("STISAdHocSQLite::set[%d]") % key);

            try
            {
                auto db = get_db();
                auto items = load();

                if (auto v = st::get_value_optional(*items, key); v.has_value())
                {
                    if (*v != std::tie(key, title, content))
                    {
                        db->execute("UPDATE messages SET (title, content)=(?, ?) WHERE key=?;", {title, content, key});
                        LOG_DEBUG("set(): %s", nvps(key, title, content));
                    }
                }
                else
                {
                    db->execute("INSERT INTO messages VALUES(?, ?, ?);", {key, title, content});
                    LOG_DEBUG("set(): %s", nvps(key, title, content));
                }
            }
            catch (std::exception& e)
            {
                LOG_ERROR("set(): %s", e);
            }
        }

        void del(int key)
        {
            LOG_CALLSTACK(boost::format("STISAdHocSQLite::del[%d]") % key);

            if (!exists(m_db_filename))
            {
                return;
            }

            try
            {
                auto db = get_db();
                auto items = load();

                if (auto v = st::get_value_optional(*items, key); v.has_value())
                {
                    db->execute("DELETE FROM messages WHERE key=?;", {key});
                    LOG_DEBUG("del(): %s", nvps(key));

                    if (items->size() == 1 && db->get_value("SELECT COUNT(*) FROM messages;") == "0")
                    {
                        db.reset();
                        remove(m_db_filename);
                        LOG_DEBUG("del(): remove %s", m_db_filename);
                    }
                }
            }
            catch (std::exception& e)
            {
                LOG_ERROR("del(): %s", e);
            }
        }

        AdHocMessageItem get(int key)
        {
            return (*load())[key];
        }

        AdHocMessageMapPtr load()
        {
            LOG_CALLSTACK("STISAdHocSQLite::load[%d]");

            if (!exists(m_db_filename))
            {
                m_md5_mgr.remove_md5(m_db_filename);
                m_md5.clear();
                m_messages.clear();
                m_cache.reset();
                return {};
            }

            auto md5 = m_md5_mgr.get_file_md5(m_db_filename);

            if (md5 == m_md5)
            {
                return m_cache;
            }

            m_messages.clear();

            try
            {
                AdHocMessageMap messages;

                for (auto&& i : get_db()->get_table("SELECT * FROM messages;"))
                {
                    auto key = std::stoi(i[0]);
                    messages.emplace(key, AdHocMessageItem{key, std::move(i[1]), std::move(i[2])});
                }

                m_md5 = std::move(md5);
                m_messages = std::move(messages);
            }
            catch (std::exception& e)
            {
                LOG_ERROR("load(): %s", e);
            }

            return m_cache = std::make_shared<AdHocMessageMap>(m_messages.get_copy());
        }

        std::pair<std::string, bool> lock(int key, std::string name)
        {
            LOG_CALLSTACK(boost::format("STISAdHocSQLite::lock[%d:%s]") % key % name);

            if (auto v = m_locked_entries.get_value_optional(key))
            {
                LOG_DEBUG("lock(): failed to lock %s", nvps(key, name));
                return {*v, false};
            }

            m_locked_entries.emplace(key, name);
            LOG_DEBUG("lock(): %s", nvps(key, name));
            return {name, true};
        }

        void unlock(int key)
        {
            LOG_CALLSTACK(boost::format("STISAdHocSQLite::unlock[%d]") % key);

            if (auto node = m_locked_entries.extract(key))
            {
                LOG_DEBUG("unlock(): key=%d, name=%s", node.key(), node.mapped());
            }
        }

    public:

        void parse_options(std::string options)
        {
            using namespace boost::program_options;

            if (m_options == options)
            {
                return;
            }

            m_options = std::move(options);

            options_description desc;
            desc.add_options()
                ("root-dir", value<std::string>())
                ("filename", value<std::string>())
                ("ad-hoc-message-library-filename", value<std::string>())
                ;

            st::set_value_from_options(m_options, desc)
                .set<path, std::string>(m_root, "root-dir")
                (m_filename, {"filename", "ad-hoc-message-library-filename"})
                ;

            m_db_filename = st2::absoluted(m_root) / m_filename;
            m_md5_mgr.set_root_dirs(m_root, m_root / ".md5");
        }

        std::string m_md5;
        std::string m_options = "uninitialized";
        AdHocMessageMapPtr m_cache;
        AdHocMessageThreadSafeMap m_messages;
        path m_root = st2::absoluted_copy(RunParamsEx::expand(RunParamsEx::get_or(RPARAM_STISROOTDIR, DEFAULT_STIS_ROOT_DIR)));
        std::string m_filename = RunParamsEx::expand(RunParamsEx::get_or(RPARAM_STISADHOCMESSAGELIBRARYFILENAME, DEFAULT_STIS_AD_HOC_MESSAGE_LIBRARY_FILENAME));
        path m_db_filename = m_root / m_filename;
        SimpleMD5FileManager m_md5_mgr;
        std::weak_ptr<SimpleSQLite> m_weak_db;
        st::map<int, std::string> m_locked_entries;
    };

    STISAdHocSQLite& STISAdHocSQLite::instance()
    {
        return StaticObject<STISAdHocSQLite>::value();
    }

    STISAdHocSQLite::STISAdHocSQLite(std::string options)
        : m_impl(std::make_shared<Impl>(std::move(options)))
    {
    }

    void STISAdHocSQLite::parse_options(std::string options)
    {
        m_impl->parse_options(std::move(options));
    }

    void STISAdHocSQLite::set(int key, const std::string& title, const std::string& content)
    {
        m_impl->set(key, title, content);
    }

    void STISAdHocSQLite::del(int key)
    {
        m_impl->del(key);
    }

    AdHocMessageItem STISAdHocSQLite::get(int key)
    {
        return m_impl->get(key);
    }

    AdHocMessageMapPtr STISAdHocSQLite::load()
    {
        return m_impl->load();
    }

    std::pair<std::string, bool> STISAdHocSQLite::lock(int key, std::string name)
    {
        return m_impl->lock(key, name);
    }

    void STISAdHocSQLite::unlock(int key)
    {
        m_impl->unlock(key);
    }
}
