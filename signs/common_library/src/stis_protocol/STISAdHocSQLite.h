#pragma once
#include <string>
#include <map>
#include <memory>

namespace TA_IRS_App::STIS_PROTOCOL::IMPL::stisadhocsqlite::detail
{
    using AdHocMessageItem = std::tuple<int, std::string, std::string>;
    using AdHocMessageMap = std::map<int, AdHocMessageItem>;
    using AdHocMessageMapPtr = std::shared_ptr<AdHocMessageMap>;

    /**
     * STISAdHocSQLite
     *
     * table: messages (key, title, content)
     */
    struct STISAdHocSQLite
    {
        static STISAdHocSQLite& instance();

        STISAdHocSQLite(std::string options = "");

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

    using STISAdHocSQLitePtr = std::shared_ptr<STISAdHocSQLite>;
}

namespace TA_IRS_App::STIS_PROTOCOL::IMPL
{
    using stisadhocsqlite::detail::STISAdHocSQLite;
    using stisadhocsqlite::detail::STISAdHocSQLitePtr;
}
