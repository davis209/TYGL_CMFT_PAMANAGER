// test.cpp : Defines the entry point for the console application.
//

#include "pch.h"
#include "core/utility/tests/boost_test/test_utility/FileGuard.h"
#include "core/utility/tests/boost_test/test_utility/LogFileGuard.h"
#include "core/utility/tests/boost_test/test_utility/DebugLevelGuard.h"
#include "core/utility/tests/boost_test/test_utility/CorbaUtilGuard.h"
#include "app/signs/common_library/src/stis_protocol/STISLibraryClient.inl"
#include "core/utility/src/base_ex/DebugUtilEx.h"
#include "core/utility/src/base_ex/RunParamsEx.h"
#include "core/utility/src/core/FileEx.h"
#include "core/utility/src/core/StdEx.h"
#include "core/naming/src/Naming.h"
#include <boost/functional/hash.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

using namespace TA_IRS_App;
using namespace TA_Base_Core;
using namespace TA_IRS_App::STIS_PROTOCOL;
using namespace std::literals;
using Blob = std::vector<unsigned char>;
using ta_utility::core::FileEx;
using ta_utility::core::Serialize;

namespace
{
    auto STSMSGLIB  = R"(data\sftp_root\PMLIBRARY\001\STSMSGLIB.XML)";
    auto STSTMLIB   = R"(data\sftp_root\tmlibrary\001\STSTMLIB.XML)";

    DebugLevelGuard s_debuglevel = DebugUtil::DebugDebug;

    static const std::string s_db_filename          = "stis_library.sqlite";
    static const std::string s_server_db_filename   = "stis_library.server.sqlite";
    static const std::string s_client_db_filename   = "stis_library.client.sqlite";

    FileGuard s_file_guard1(s_db_filename);
    FileGuard s_file_guard2(s_server_db_filename);
    FileGuard s_file_guard3(s_client_db_filename);

    struct DummySTISLibraryServer : GenericServantCorbaDef, STISLibrarySQLiteEx
    {
        DummySTISLibraryServer()
        {
            m_db = std::make_shared<STISLibrarySQLite>("--open --filename=" + s_server_db_filename);
        }

        void start()
        {
            activate_servant_with_name(STIS_LIBRARY_SERVANT_NAME);
        }

        void stop()
        {
            deactivate_servant();
        }

        // GenericServantCorbaDef

        virtual GenericServantCorbaDef::Result on_generic_corba_invoke_return(std::string name, std::string args) override
        {
            if (boost::iequals(name, "get_db_file_content"))
            {
                return get_db_file_content(args);
            }
            else if (boost::iequals(name, "get_versions"))
            {
                return get_versions();
            }
            else if (boost::iequals(name, "get_library_infos"))
            {
                return m_db->get_library_infos();
            }
            // ISTISLibrarySQLite
            else if (boost::iequals(name, "get_filename"))
            {
                return m_db->get_filename();
            }
            else if (boost::iequals(name, "get_library_version"))
            {
                using Args = std::tuple<std::string, std::string>;
                auto&& [category, version] = Serialize::deserialize<Args>(std::move(args));
                return get_library_version(category, version);
            }
            else if (boost::iequals(name, "has_library"))
            {
                using Args = std::tuple<std::string, std::string>;
                auto&& [category, version] = Serialize::deserialize<Args>(std::move(args));
                return has_library(category, version);
            }
            else if (boost::iequals(name, "get_library"))
            {
                using Args = std::tuple<std::string, std::string>;
                auto&& [category, version] = Serialize::deserialize<Args>(std::move(args));
                return get_library(category, version);
            }
            // ISTISLibrarySQLite
            else if (boost::iequals(name, "set_library_version"))
            {
                using Args = std::tuple<std::string, std::string, std::string>;
                auto&& [category, version, number] = Serialize::deserialize<Args>(std::move(args));
                return m_db->set_library_version(category, version, number);
            }
            else if (boost::iequals(name, "add_library"))
            {
                using Args = std::tuple<std::string, std::string, Blob>;
                auto&& [category, version, blob] = Serialize::deserialize<Args>(std::move(args));
                return m_db->add_library(category, version, blob);
            }
            else if (boost::iequals(name, "remove_library"))
            {
                using Args = std::tuple<std::string, std::string>;
                auto&& [category, version] = Serialize::deserialize<Args>(std::move(args));
                return m_db->remove_library(category, version);
            }
            // library-download
            else if (boost::iequals(name, "library_download"))
            {
                using Args = std::tuple<std::string, std::string>;
                auto&& [category, version] = Serialize::deserialize<Args>(std::move(args));
                return library_download(category, version);
            }

            return {};
        }

        virtual void on_generic_corba_invoke(std::string name, std::string args) override
        {
            if (boost::iequals(name, "upgrade_library_version"))
            {
                using Args = std::tuple<std::string, std::string, std::string>;
                auto&& [category, version, session] = Serialize::deserialize<Args>(std::move(args));
                set_library_version(category, "current", version);
            }
        }

        Blob get_db_file_content(const std::string& hash)
        {
            FileEx file(m_db->get_filename());
            return file.size() && file.hash_string() != hash ? file.blob() : Blob{};
        }

        std::vector<std::vector<std::string>> get_versions()
        {
            return
            {
                {"message", "current", current_message_library_version()},
                {"message", "next", next_message_library_version()},
                {"template", "current", current_template_library_version()},
                {"template", "next", next_template_library_version()}
            };
        }

        Blob library_download(const std::string& category, const std::string& version)
        {
            return Blob{};
        }
    };
}

BOOST_AUTO_TEST_SUITE(SIGNS)
BOOST_AUTO_TEST_SUITE(COMMON)
BOOST_AUTO_TEST_SUITE(PROTOCOL)
BOOST_AUTO_TEST_SUITE(STISLibraryClientTest)

BOOST_AUTO_TEST_CASE(test)
{
    STISLibraryClient client("--server=local");
    client.start();

    client.set_current_message_library_version("001");
    client.set_next_message_library_version("001");
    client.set_current_template_library_version("000");
    client.set_next_template_library_version("000");

    BOOST_TEST(client.current_message_library_version() == "001");
    BOOST_TEST(client.next_message_library_version() == "001");
    BOOST_TEST((client.current_next_message_library_versions() == std::make_pair("001"s, "001"s)));
    BOOST_TEST(client.current_template_library_version() == "000");
    BOOST_TEST(client.next_template_library_version() == "000");
    BOOST_TEST((client.current_next_template_library_versions() == std::make_pair("000"s, "000"s)));

    BOOST_TEST(client.has_current_message_library() == false);
    BOOST_TEST(client.has_next_message_library() == false);
    BOOST_TEST(client.has_current_template_library() == false);
    BOOST_TEST(client.has_next_template_library() == false);

    FileEx message(STSMSGLIB);
    FileEx templat(STSTMLIB);
    client.add_message_library("current", message.blob());
    client.add_template_library("current", templat.blob());

    BOOST_TEST(client.has_current_message_library());
    BOOST_TEST(client.has_next_message_library());
    BOOST_TEST(client.has_current_template_library());
    BOOST_TEST(client.has_next_template_library());

    client.remove_message_library("current");
    client.remove_template_library("current");
    BOOST_TEST(client.has_current_message_library() == false);
    BOOST_TEST(client.has_current_template_library() == false);
}

BOOST_AUTO_TEST_CASE(test_synchronization)
{
    LogFileGuard log("SignsCommonLibrary-PROTOCOL-STISLibraryClientTest-test_synchronization.log");

    RunParamsEx::set(RPARAM_DBCONNECTIONFILE, R"(D:\LocalTest\C830G_TIP\ConfigFiles\CSV\OCCConnectionStrings.csv)");
    RunParamsEx::set("DbConnectionDefaultMaxTry", 1);
    RunParamsEx::set("DbConnectionDefaultTimeout", 1);
    RunParamsEx::set(RPARAM_ENTITYNAME_v + "NamedObjectRepository", "corbaloc::localhost:9999/" + RPARAM_ENTITYNAME_v + "Repo");
    CorbaUtilGuard corba;
    Naming::getInstance().init();

    FileEx message(STSMSGLIB);
    FileEx templat(STSTMLIB);

    DummySTISLibraryServer server;
    server.start();

    STISLibraryClient client
    (
        " --sync-all"
        " --client-call-timeout-seconds=1"
        " --sync-interval-ms=100"
        " --server=BoostTest"
        " --use-singleton-local-db=false"
        " --db-filename=" + s_client_db_filename
    );
    client.start();

    {
        server.set_current_message_library_version("001");
        server.set_next_message_library_version("001");
        server.set_current_template_library_version("001");
        server.set_next_template_library_version("001");

        BOOST_TEST(client.current_message_library_version() == "");
        BOOST_TEST(client.next_message_library_version() == "");
        BOOST_TEST(client.current_template_library_version() == "");
        BOOST_TEST(client.current_template_library_version() == "");

        stdex::sleep_for(500ms);

        BOOST_TEST(client.current_message_library_version() == "001");
        BOOST_TEST(client.next_message_library_version() == "001");
        BOOST_TEST(client.current_template_library_version() == "001");
        BOOST_TEST(client.current_template_library_version() == "001");
    }

    {
        BOOST_TEST(client.has_message_library("001") == false);
        BOOST_TEST(client.has_template_library("001") == false);

        server.add_message_library("current", message.blob());
        server.add_template_library("current", templat.blob());

        stdex::sleep_for(1s);

        BOOST_TEST(client.has_message_library("current") == true);
        BOOST_TEST(client.has_template_library("current") == true);
    }

    {
        BOOST_TEST(client.next_message_library_version() == "001");
        BOOST_TEST(client.has_message_library("002") == false);

        server.add_message_library("002", message.blob());
        server.set_next_message_library_version("002");

        stdex::sleep_for(1s);

        BOOST_TEST(client.next_message_library_version() == "002");
        BOOST_TEST(client.has_message_library("002"));
        BOOST_TEST(client.has_next_message_library());
    }

    {
        client.m_impl->m_local_db->remove_library("message", "001");
        client.m_impl->m_local_db->remove_library("message", "002");
        BOOST_TEST(client.has_message_library("001") == false);
        BOOST_TEST(client.has_message_library("002") == false);

        stdex::sleep_for(1s);

        BOOST_TEST(client.has_message_library("001") == true);
        BOOST_TEST(client.has_message_library("002") == true);
    }

    {
        client.add_message_library("003", message.blob());
        BOOST_TEST(client.has_message_library("003") == false);
        BOOST_TEST(server.has_message_library("002") == true);

        client.upgrade_message_library_version("003");
        BOOST_TEST(client.current_message_library_version() == "001");
        BOOST_TEST(server.current_message_library_version() == "003");

        stdex::sleep_for(1s);

        BOOST_TEST(client.current_message_library_version() == "003");
        BOOST_TEST(client.has_message_library("003") == true);
    }

    {
        server.add_message_library("009", message.blob());

        stdex::sleep_for(1s);

        BOOST_TEST(client.has_message_library("009") == true);
    }
}

BOOST_AUTO_TEST_CASE(test_observer)
{
    struct Observer : ISTISLibraryObserver
    {
        virtual void on_current_message_version_change(std::string old_version, std::string new_version) override
        {
            m_called_function = __func__;
            m_old_version = old_version;
            m_new_version = new_version;
        }

        virtual void on_next_message_version_change(std::string old_version, std::string new_version) override
        {
            m_called_function = __func__;
            m_old_version = old_version;
            m_new_version = new_version;
        }

        virtual void on_current_template_version_change(std::string old_version, std::string new_version) override
        {
            m_called_function = __func__;
            m_old_version = old_version;
            m_new_version = new_version;
        }

        virtual void on_next_template_version_change(std::string old_version, std::string new_version) override
        {
            m_called_function = __func__;
            m_old_version = old_version;
            m_new_version = new_version;
        }

        virtual void on_new_message_library(std::string version) override
        {
            m_called_function = __func__;
            m_version = version;
        }

        virtual void on_new_template_library(std::string version) override
        {
            m_called_function = __func__;
            m_version = version;
        }

        virtual void on_any_change() override
        {
            m_on_changed_called = true;
        }

        void clear()
        {
            m_on_changed_called = false;
            m_called_function.clear();
            m_old_version.clear();
            m_new_version.clear();
            m_version.clear();
        }

        bool m_on_changed_called = false;
        std::string m_called_function;
        std::string m_old_version;
        std::string m_new_version;
        std::string m_version;
    };

    RunParamsEx::set("BoostTestNamedObjectRepository", "corbaloc::localhost:9999/" + RPARAM_ENTITYNAME_v + "Repo");
    RunParamsEx::set(RPARAM_DBCONNECTIONFILE, R"(D:\LocalTest\C830G_TIP\ConfigFiles\CSV\OCCConnectionStrings.csv)");
    RunParamsEx::set("DbConnectionDefaultMaxTry", 1);
    RunParamsEx::set("DbConnectionDefaultTimeout", 1);
    CorbaUtilGuard corba;
    Naming::getInstance().init();

    FileEx message(STSMSGLIB);
    FileEx templat(STSTMLIB);

    DummySTISLibraryServer server;
    server.start();

    STISLibraryClient client
    (
        " --sync-all"
        " --client-call-timeout-seconds=1"
        " --sync-interval-ms=100"
        " --server=BoostTest"
        " --use-singleton-local-db=false"
        " --db-filename=" + s_client_db_filename
    );
    client.start();

    auto observer = std::make_shared<Observer>();
    client.set_observer(observer);

    {
        server.set_current_message_library_version("001");
        stdex::sleep_for(1s);
        BOOST_TEST((observer->m_on_changed_called));
        BOOST_TEST((observer->m_called_function == "on_current_message_version_change"));
        BOOST_TEST((observer->m_old_version == ""));
        BOOST_TEST((observer->m_new_version == "001"));
        observer->clear();

        server.set_current_message_library_version("002");
        stdex::sleep_for(1s);
        BOOST_TEST((observer->m_on_changed_called));
        BOOST_TEST((observer->m_called_function == "on_current_message_version_change"));
        BOOST_TEST((observer->m_old_version == "001"));
        BOOST_TEST((observer->m_new_version == "002"));
        observer->clear();
    }

    {
        server.set_next_message_library_version("001");
        stdex::sleep_for(1s);
        BOOST_TEST((observer->m_on_changed_called));
        BOOST_TEST((observer->m_called_function == "on_next_message_version_change"));
        BOOST_TEST((observer->m_old_version == ""));
        BOOST_TEST((observer->m_new_version == "001"));
        observer->clear();

        server.set_next_message_library_version("002");
        stdex::sleep_for(1s);
        BOOST_TEST((observer->m_on_changed_called));
        BOOST_TEST((observer->m_called_function == "on_next_message_version_change"));
        BOOST_TEST((observer->m_old_version == "001"));
        BOOST_TEST((observer->m_new_version == "002"));
        observer->clear();
    }

    {
        server.set_current_template_library_version("001");
        stdex::sleep_for(1s);
        BOOST_TEST((observer->m_on_changed_called));
        BOOST_TEST((observer->m_called_function == "on_current_template_version_change"));
        BOOST_TEST((observer->m_old_version == ""));
        BOOST_TEST((observer->m_new_version == "001"));
        observer->clear();

        server.set_current_template_library_version("002");
        stdex::sleep_for(1s);
        BOOST_TEST((observer->m_on_changed_called));
        BOOST_TEST((observer->m_called_function == "on_current_template_version_change"));
        BOOST_TEST((observer->m_old_version == "001"));
        BOOST_TEST((observer->m_new_version == "002"));
        observer->clear();
    }

    {
        server.set_next_template_library_version("001");
        stdex::sleep_for(1s);
        BOOST_TEST((observer->m_on_changed_called));
        BOOST_TEST((observer->m_called_function == "on_next_template_version_change"));
        BOOST_TEST((observer->m_old_version == ""));
        BOOST_TEST((observer->m_new_version == "001"));
        observer->clear();

        server.set_next_template_library_version("002");
        stdex::sleep_for(1s);
        BOOST_TEST((observer->m_on_changed_called));
        BOOST_TEST((observer->m_called_function == "on_next_template_version_change"));
        BOOST_TEST((observer->m_old_version == "001"));
        BOOST_TEST((observer->m_new_version == "002"));
        observer->clear();
    }

    {
        server.set_current_message_library_version("001");
        stdex::sleep_for(1s);
        server.add_message_library("001", message.blob());
        stdex::sleep_for(1s);
        BOOST_TEST((observer->m_on_changed_called));
        BOOST_TEST((observer->m_called_function == "on_new_message_library"));
        BOOST_TEST((observer->m_version == "001"));
        observer->clear();
    }

    {
        server.set_current_template_library_version("001");
        stdex::sleep_for(1s);
        server.add_template_library("001", templat.blob());
        stdex::sleep_for(1s);
        BOOST_TEST((observer->m_on_changed_called));
        BOOST_TEST((observer->m_called_function == "on_new_template_library"));
        BOOST_TEST((observer->m_version == "001"));
        observer->clear();
    }
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
