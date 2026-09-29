// test.cpp : Defines the entry point for the console application.
//

#include "pch.h"
#include "core/utility/tests/boost_test/test_utility/FileGuard.h"
#include "core/utility/tests/boost_test/test_utility/DebugLevelGuard.h"
#include "app/signs/common_library/src/stis_protocol/STISLibrarySQLite.h"
#include "core/utility/src/base_ex/DebugUtilEx.h"
#include "core/utility/src/core/FileEx.h"
#include <boost/test/unit_test.hpp>

using namespace TA_IRS_App;
using namespace TA_Base_Core;
using namespace TA_IRS_App::STIS_PROTOCOL;
using namespace ta_utility::core;
using namespace std::string_literals;
using Blob = std::vector<unsigned char>;

namespace
{
    auto STSMSGLIB  = R"(data\sftp_root\PMLIBRARY\001\STSMSGLIB.XML)";
    auto STSTMLIB   = R"(data\sftp_root\tmlibrary\000\STSTMLIB.XML)";

    DebugLevelGuard s_debuglevel = DebugUtil::DebugDebug;
    auto s_db_filename = "stis_library.sqlite";
    FileGuard s_guard(s_db_filename);
}

BOOST_AUTO_TEST_SUITE(SIGNS)
BOOST_AUTO_TEST_SUITE(COMMON)
BOOST_AUTO_TEST_SUITE(PROTOCOL)
BOOST_AUTO_TEST_SUITE(STISLibrarySQLiteTest)

BOOST_AUTO_TEST_CASE(test)
{
    STISLibrarySQLite db(s_db_filename);
    db.open();

    db.set_library_version("message", "current", "1");
    db.set_library_version("message", "next", "01");
    db.set_library_version("template", "current", "0");
    db.set_library_version("template", "next", "00");

    BOOST_TEST(db.get_library_version("message", "current") == "001");
    BOOST_TEST(db.get_library_version("message", "next") == "001");
    BOOST_TEST(db.get_library_version("template", "current") == "000");
    BOOST_TEST(db.get_library_version("template", "next") == "000");

    FileEx message(STSMSGLIB);
    FileEx templat(STSTMLIB);

    BOOST_TEST(db.has_library("message", "current") == false);
    BOOST_TEST(db.has_library("message", "next") == false);
    BOOST_TEST(db.has_library("template", "current") == false);
    BOOST_TEST(db.has_library("template", "next") == false);

    db.add_library("message", "current", message.blob());
    db.add_library("template", "current", templat.blob());

    BOOST_TEST(db.has_library("message", "current"));
    BOOST_TEST(db.has_library("message", "next"));
    BOOST_TEST(db.has_library("message", "001"));
    BOOST_TEST(db.has_library("template", "current"));
    BOOST_TEST(db.has_library("template", "next"));
    BOOST_TEST(db.has_library("template", "000"));

    BOOST_TEST(db.get_library("message", "current") == message.blob());
    BOOST_TEST(db.get_library("message", "next") == message.blob());
    BOOST_TEST(db.get_library("template", "current") == templat.blob());
    BOOST_TEST(db.get_library("template", "next") == templat.blob());
}

BOOST_AUTO_TEST_CASE(test_set_version_number)
{
    STISLibrarySQLite db(s_db_filename);
    db.open();

    BOOST_TEST((db.set_library_version("message", "current", "001") == std::make_pair(""s, true)));
    auto number = db.get_library_version("message", "current");
    BOOST_TEST(number == "001");

    BOOST_TEST((db.set_library_version("message", "current", "002") == std::make_pair("001"s, true)));
    number = db.get_library_version("message", "current");
    BOOST_TEST(number == "002");

    BOOST_TEST((db.set_library_version("message", "current", "002") == std::make_pair("002"s, false)));
    number = db.get_library_version("message", "current");
    BOOST_TEST(number == "002");
}

BOOST_AUTO_TEST_CASE(test_get_library_infos)
{
    STISLibrarySQLite db(s_db_filename);
    db.open();
    BOOST_TEST((db.get_library_infos().empty()));
    db.add_library("template", "001", Blob{});
    BOOST_TEST((db.get_library_infos().size() == 1));

    db.add_library("message", "001", Blob{});
    db.add_library("message", "002", Blob{});
    auto infos = db.get_library_infos();
    BOOST_TEST(infos.size() == 3);
}

BOOST_AUTO_TEST_CASE(test_add_library)
{
    STISLibrarySQLite db(s_db_filename);
    db.open();

    std::string str = "message library 001";
    Blob blob{str.begin(), str.end()};
    BOOST_TEST(db.add_library("message", "001", Blob {str.begin(), str.end()}));
    auto blob2 = db.get_library("message", "001");
    BOOST_TEST(blob == blob2);

    BOOST_TEST(db.add_library("message", "001", Blob {str.begin(), str.end()}) == false);
}

BOOST_AUTO_TEST_CASE(test_remove_library)
{
    STISLibrarySQLite db(s_db_filename);
    db.open();

    std::string str = "message library 001";
    Blob blob{str.begin(), str.end()};
    db.add_library("message", "001", Blob {str.begin(), str.end()});
    BOOST_TEST(db.has_library("message", "001"));
    BOOST_TEST(db.remove_library("message", "001"));
    BOOST_TEST(db.has_library("message", "001") == false);

    BOOST_TEST(db.remove_library("message", "001") == false);
}

BOOST_AUTO_TEST_CASE(test_observer)
{
    struct Observer : ISTISLibrarySQLiteObserver
    {
        virtual void on_set_library_version(std::string category, std::string version, std::string old_number, std::string new_number) override
        {
            m_called_function = __func__;
            m_category = category;
            m_version = version;
            m_old_number = old_number;
            m_new_number = new_number;
        }

        virtual void on_new_library(std::string category, std::string version, const Blob& blob) override
        {
            m_called_function = __func__;
            m_category = category;
            m_version = version;
        }

        virtual void on_remove_library(std::string category, std::string version) override
        {
            m_called_function = __func__;
            m_category = category;
            m_version = version;
        }

        void clear()
        {
            m_called_function.clear();
            m_category.clear();
            m_version.clear();
            m_old_number.clear();
            m_new_number.clear();
        }

        std::string m_called_function;
        std::string m_category;
        std::string m_version;
        std::string m_old_number;
        std::string m_new_number;
    };

    auto observer = std::make_shared<Observer>();

    STISLibrarySQLite db(s_db_filename);
    db.open();
    db.add_observer(observer);

    {
        db.set_library_version("message", "current", "001");
        BOOST_TEST((observer->m_called_function == "on_set_library_version"));
        BOOST_TEST((observer->m_category == "message"));
        BOOST_TEST((observer->m_version == "current"));
        BOOST_TEST((observer->m_old_number == ""));
        BOOST_TEST((observer->m_new_number == "001"));

        db.set_library_version("message", "current", "002");
        BOOST_TEST((observer->m_old_number == "001"));
        BOOST_TEST((observer->m_new_number == "002"));

        observer->clear();
    }

    {
        db.add_library("message", "001", Blob{});
        BOOST_TEST((observer->m_called_function == "on_new_library"));
        BOOST_TEST((observer->m_category == "message"));
        BOOST_TEST((observer->m_version == "001"));
        observer->clear();

        db.add_library("message", "002", Blob{});
        BOOST_TEST((observer->m_called_function == "on_new_library"));
        BOOST_TEST((observer->m_category == "message"));
        BOOST_TEST((observer->m_version == "002"));
        observer->clear();

        db.add_library("message", "002", Blob{});
        BOOST_TEST((observer->m_called_function == ""));
        BOOST_TEST((observer->m_category == ""));
        BOOST_TEST((observer->m_version == ""));
        observer->clear();

        db.remove_library("message", "003");
        BOOST_TEST((observer->m_called_function == ""));
        BOOST_TEST((observer->m_category == ""));
        BOOST_TEST((observer->m_version == ""));
        observer->clear();

        db.remove_library("message", "002");
        BOOST_TEST((observer->m_called_function == "on_remove_library"));
        BOOST_TEST((observer->m_category == "message"));
        BOOST_TEST((observer->m_version == "002"));
        observer->clear();

        db.remove_library("message", "001");
        BOOST_TEST((observer->m_called_function == "on_remove_library"));
        BOOST_TEST((observer->m_category == "message"));
        BOOST_TEST((observer->m_version == "001"));
        observer->clear();
    }

    {
        observer.reset();
        db.set_library_version("message", "current", "999");
    }
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
