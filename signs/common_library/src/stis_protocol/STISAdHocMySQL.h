#pragma once
#include <string>
#include <map>
#include <memory>

namespace TA_IRS_App::STIS_PROTOCOL::IMPL::stisadhocmysql::detail
{
    using AdHocMessageItem = std::tuple<int, std::string, std::string>;
    using AdHocMessageMap = std::map<int, AdHocMessageItem>;
    using AdHocMessageMapPtr = std::shared_ptr<AdHocMessageMap>;

    /**
     * STISAdHocMySQL
     *
     * DatabaseFactory/IDatabase backed variant of STISAdHocSQLite.
     * table: stis_ad_hoc_messages (message_key, title, content)
     */
    struct STISAdHocMySQL
    {
        static STISAdHocMySQL& instance();

        STISAdHocMySQL(std::string options = "");

        void parse_options(std::string options);

        void set(int key, const std::string& title, const std::string& content);
        void del(int key);
        AdHocMessageItem get(int key);
        AdHocMessageMapPtr load();

        std::pair<std::string, bool> lock(int key, std::string name);
        void unlock(int key);

        struct Impl;
        std::shared_ptr<Impl> m_impl;
    };

    using STISAdHocMySQLPtr = std::shared_ptr<STISAdHocMySQL>;
}

namespace TA_IRS_App::STIS_PROTOCOL::IMPL
{
    using stisadhocmysql::detail::STISAdHocMySQL;
    using stisadhocmysql::detail::STISAdHocMySQLPtr;
}
