#include "pch.h"
#include "STISAdHocMySQL.h"
#include "STISDatabaseCommon.h"
#include "core/utilities/src/CallstackLogger.h"
#include "core/utility/src/base_ex/DebugUtilEx.h"
#include "core/utility/src/base_ex/RunParamsEx.h"
#include "core/utility/src/core/StdEx.h"
#include "core/utility/src/core/StaticObject.h"
#include "core/utility/src/core/MakeNameValuePairsString.h"
#include <boost/program_options.hpp>
#include <mutex>

using st::StaticObject;
using namespace TA_Base_Core;

namespace
{
    const std::string MESSAGES_TABLE = "stis_ad_hoc_messages";
    const std::string LOCKS_TABLE = "stis_ad_hoc_locks";
}

namespace TA_IRS_App::STIS_PROTOCOL::IMPL::stisadhocmysql::detail
{
    struct STISAdHocMySQL::Impl
    {
        Impl(std::string options = "") { parse_options(std::move(options)); }

        IDatabase* read_db()  { ensure_tables(); return stisdatabase::detail::get_stis_read_database(m_database_type); }
        IDatabase* write_db() { ensure_tables(); return stisdatabase::detail::get_stis_write_database(m_database_type); }

        void ensure_tables()
        {
            std::scoped_lock<std::recursive_mutex> l(m_mutex);
            if (m_tables_checked) return;

            auto db = stisdatabase::detail::get_stis_write_database(m_database_type);

            stisdatabase::detail::execute_modification(db,
                "CREATE TABLE IF NOT EXISTS " + MESSAGES_TABLE + " ("
                "message_key INT NOT NULL, "
                "title VARCHAR(255) NOT NULL, "
                "content TEXT NOT NULL, "
                "updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP, "
                "PRIMARY KEY (message_key))");

            stisdatabase::detail::execute_modification(db,
                "CREATE TABLE IF NOT EXISTS " + LOCKS_TABLE + " ("
                "message_key INT NOT NULL, "
                "owner VARCHAR(128) NOT NULL, "
                "expires_at TIMESTAMP NOT NULL, "
                "PRIMARY KEY (message_key))");

            m_tables_checked = true;
        }

        void set(int key, const std::string& title, const std::string& content)
        {
            LOG_CALLSTACK(boost::format("STISAdHocMySQL::set[%d]") % key);
            std::scoped_lock<std::recursive_mutex> l(m_mutex);

            try
            {
                auto db = write_db();
                const std::string sql =
                    "INSERT INTO " + MESSAGES_TABLE + " (message_key, title, content) VALUES (" +
                    stisdatabase::detail::sql_int(key) + ", " +
                    stisdatabase::detail::sql_string(title) + ", " +
                    stisdatabase::detail::sql_string(content) + ") " +
                    "ON DUPLICATE KEY UPDATE title = VALUES(title), content = VALUES(content)";
                stisdatabase::detail::execute_modification(db, sql);
                LOG_DEBUG("set(): %s", nvps(key, title, content));
                m_cache.reset();
            }
            catch (std::exception& e)
            {
                LOG_ERROR("set(): %s", e);
            }
        }

        void del(int key)
        {
            LOG_CALLSTACK(boost::format("STISAdHocMySQL::del[%d]") % key);
            std::scoped_lock<std::recursive_mutex> l(m_mutex);

            try
            {
                auto db = write_db();
                stisdatabase::detail::execute_modification(db,
                    "DELETE FROM " + MESSAGES_TABLE + " WHERE message_key = " + stisdatabase::detail::sql_int(key));
                LOG_DEBUG("del(): %s", nvps(key));
                m_cache.reset();
            }
            catch (std::exception& e)
            {
                LOG_ERROR("del(): %s", e);
            }
        }

        AdHocMessageItem get(int key)
        {
            LOG_CALLSTACK(boost::format("STISAdHocMySQL::get[%d]") % key);
            std::scoped_lock<std::recursive_mutex> l(m_mutex);

            try
            {
                auto db = read_db();

                std::vector<std::string> columns;
                columns.push_back("message_key");
                columns.push_back("title");
                columns.push_back("content");

                auto* data = stisdatabase::detail::execute_query(db,
                    "SELECT message_key, title, content FROM " + MESSAGES_TABLE +
                    " WHERE message_key = " + stisdatabase::detail::sql_int(key),
                    columns);
                AdHocMessageItem item{};
                bool found = false;

                stisdatabase::detail::for_each_row(db, data, [&](IData& rowset, unsigned long row)
                {
                    item = AdHocMessageItem{
                        rowset.getIntegerData(row, "message_key"),
                        rowset.getStringData(row, "title"),
                        rowset.getStringData(row, "content")};
                    found = true;
                });

                return found ? item : AdHocMessageItem{};
            }
            catch (std::exception& e)
            {
                LOG_ERROR("get(): %s", e);
                return {};
            }
        }

        AdHocMessageMapPtr load()
        {
            LOG_CALLSTACK("STISAdHocMySQL::load");
            std::scoped_lock<std::recursive_mutex> l(m_mutex);

            try
            {
                auto db = read_db();

                std::vector<std::string> columns;
                columns.push_back("message_key");
                columns.push_back("title");
                columns.push_back("content");

                AdHocMessageMap messages;
                auto* data = stisdatabase::detail::execute_query(db,
                    "SELECT message_key, title, content FROM " + MESSAGES_TABLE + " ORDER BY message_key",
                    columns);
                stisdatabase::detail::for_each_row(db, data, [&](IData& rowset, unsigned long row)
                {
                    const auto key = rowset.getIntegerData(row, "message_key");
                    messages.emplace(key, AdHocMessageItem{key, rowset.getStringData(row, "title"), rowset.getStringData(row, "content")});
                });

                m_cache = std::make_shared<AdHocMessageMap>(std::move(messages));
            }
            catch (std::exception& e)
            {
                LOG_ERROR("load(): %s", e);
                m_cache = std::make_shared<AdHocMessageMap>();
            }

            return m_cache;
        }

        std::pair<std::string, bool> lock(int key, std::string name)
        {
            LOG_CALLSTACK(boost::format("STISAdHocMySQL::lock[%d:%s]") % key % name);
            std::scoped_lock<std::recursive_mutex> l(m_mutex);

            try
            {
                auto db = write_db();

                stisdatabase::detail::execute_modification(db,
                    "DELETE FROM " + LOCKS_TABLE + " WHERE expires_at < NOW()");

                const std::string key_sql = stisdatabase::detail::sql_int(key);
                const std::string owner_sql = stisdatabase::detail::sql_string(name);
                const std::string timeout_sql = stisdatabase::detail::sql_int(m_lock_timeout_seconds);

                // This lets the caller take over an expired lock, otherwise keeps the current owner.
                const std::string try_lock_sql =
                    "INSERT INTO " + LOCKS_TABLE + " (message_key, owner, expires_at) VALUES (" +
                    key_sql + ", " + owner_sql + ", DATE_ADD(NOW(), INTERVAL " + timeout_sql + " SECOND)) " +
                    "ON DUPLICATE KEY UPDATE "
                    "owner = IF(expires_at < NOW(), VALUES(owner), owner), "
                    "expires_at = IF(expires_at < NOW() OR owner = VALUES(owner), VALUES(expires_at), expires_at)";
                stisdatabase::detail::execute_modification(db, try_lock_sql);

                std::vector<std::string> columns;
                columns.push_back("owner");

                std::string owner;
                auto* data = stisdatabase::detail::execute_query(db,
                    "SELECT owner FROM " + LOCKS_TABLE + " WHERE message_key = " + key_sql,
                    columns);
                stisdatabase::detail::for_each_row(db, data, [&](IData& rowset, unsigned long row)
                {
                    owner = rowset.getStringData(row, "owner");
                });

                const bool acquired = (owner == name);
                LOG_DEBUG("lock(): key=%d, owner=%s, acquired=%d", key, owner, acquired);
                return {owner, acquired};
            }
            catch (std::exception& e)
            {
                LOG_ERROR("lock(): %s", e);
                return {"DATABASE FAILED", false};
            }
        }

        void unlock(int key)
        {
            LOG_CALLSTACK(boost::format("STISAdHocMySQL::unlock[%d]") % key);
            std::scoped_lock<std::recursive_mutex> l(m_mutex);

            try
            {
                auto db = write_db();
                stisdatabase::detail::execute_modification(db,
                    "DELETE FROM " + LOCKS_TABLE + " WHERE message_key = " + stisdatabase::detail::sql_int(key));
                LOG_DEBUG("unlock(): key=%d", key);
            }
            catch (std::exception& e)
            {
                LOG_ERROR("unlock(): %s", e);
            }
        }

        void parse_options(std::string options)
        {
            using namespace boost::program_options;
            if (m_options == options) return;
            m_options = std::move(options);

            options_description desc;
            desc.add_options()
                ("lock-ad-hoc-message-heartbeat-timeout-seconds", value<int>())
                ("stis-ad-hoc-lock-timeout-seconds", value<int>())
                ;

            st::set_value_from_options(m_options, desc)
                (m_lock_timeout_seconds, {"stis-ad-hoc-lock-timeout-seconds", "lock-ad-hoc-message-heartbeat-timeout-seconds"})
                ;

            m_database_type = Tis_Cd;
            m_tables_checked = false;
            m_cache.reset();
        }

        std::string m_options = "uninitialized";
        EDataTypes m_database_type = Tis_Cd;
        bool m_tables_checked = false;
        int m_lock_timeout_seconds = 30;
        AdHocMessageMapPtr m_cache;
        std::recursive_mutex m_mutex;
    };

    STISAdHocMySQL& STISAdHocMySQL::instance() { return StaticObject<STISAdHocMySQL>::value(); }
    STISAdHocMySQL::STISAdHocMySQL(std::string options) : m_impl(std::make_shared<Impl>(std::move(options))) {}
    void STISAdHocMySQL::parse_options(std::string options) { m_impl->parse_options(std::move(options)); }
    void STISAdHocMySQL::set(int key, const std::string& title, const std::string& content) { m_impl->set(key, title, content); }
    void STISAdHocMySQL::del(int key) { m_impl->del(key); }
    AdHocMessageItem STISAdHocMySQL::get(int key) { return m_impl->get(key); }
    AdHocMessageMapPtr STISAdHocMySQL::load() { return m_impl->load(); }
    std::pair<std::string, bool> STISAdHocMySQL::lock(int key, std::string name) { return m_impl->lock(key, std::move(name)); }
    void STISAdHocMySQL::unlock(int key) { m_impl->unlock(key); }
}
