#pragma once
#include "CommonDefs.h"
#include "STSMSGLIB_XML.h"
#include "STSTMLIB_XML.h"
#include "app/signs/common_library/src/stis_protocol/message_types/MessageTypes.h"
#include <boost/filesystem.hpp>
#include <functional>

namespace TA_Base_Core
{
    class ITemplateLibrary;
    using ITemplateLibraryPtr = std::shared_ptr<ITemplateLibrary>;

    class IPredefinedMessageLibrary;
    using IPredefinedMessageLibraryPtr = std::shared_ptr<IPredefinedMessageLibrary>;
}

namespace TA_IRS_App::STIS_PROTOCOL::INTERFACES::stislibraryclient::detail
{
    using boost::filesystem::path;
    using Blob = std::vector<unsigned char>;
    using AdHocMessageItem = std::tuple<int, std::string, std::string>;
    using AdHocMessageMap = std::map<int, AdHocMessageItem>;
    using AdHocMessageMapPtr = std::shared_ptr<AdHocMessageMap>;
    using namespace TA_Base_Core;

    struct STISLibraryClient
    {
        static STISLibraryClient& instance();

        STISLibraryClient(std::string options = "");

        void parse_options(std::string options);
        bool is_server_online();
        std::string get_server_name();

        // synchronize

        void start();
        void stop();
        void async_now();
        bool sync_versions();
        bool sync_all_versions();
        bool sync_ad_hoc_message_library();
        size_t sync_predefined_message_library(std::string version);
        size_t sync_display_template_library(std::string version);
        size_t sync_display_templates(std::string version);
        size_t sync_templates();
        size_t sync_current_message_template_library(); // current libraries only
        size_t sync_current_next_message_template_library();
        void change_sync_interval_for_a_while(size_t new_interval_ms, size_t duration_ms);
        void change_sync_interval_until(size_t new_interval_ms, size_t check_pred_interval_ms, size_t timeout_ms, std::function<bool()> pred);

        // predefined message library

        std::string current_message_library_version();
        std::string next_message_library_version();
        std::pair<std::string, std::string> current_next_message_library_versions();

        void set_current_message_library_version(std::string version);
        void set_next_message_library_version(std::string version);
        void upgrade_message_library_version(std::string version);

        bool has_current_message_library();
        bool has_next_message_library();
        bool has_message_library(std::string version);

        STSMSGLIB_XML_PTR load_current_message_library_xml();
        STSMSGLIB_XML_PTR load_next_message_library_xml();
        STSMSGLIB_XML_PTR load_message_library_xml(std::string version);
        IPredefinedMessageLibraryPtr load_message_library();

        void add_message_library(std::string version, const Blob& blob);
        void remove_message_library(std::string version);

        Blob download_predefined_message_library(std::string version);

        // ad hoc message library

        AdHocMessageMapPtr load_ad_hoc_message_library();
        void set_ad_hoc_message(int key, const std::string& title, const std::string& content);
        void delete_ad_hoc_message(int key);
        AdHocMessageItem get_ad_hoc_message(int key);
        std::pair<std::string, bool> lock_ad_hoc_message(int key);
        void unlock_ad_hoc_message(int key);

        // display template library

        std::string current_template_library_version();
        std::string next_template_library_version();
        std::pair<std::string, std::string> current_next_template_library_versions();

        void set_current_template_library_version(std::string version);
        void set_next_template_library_version(std::string version);
        void upgrade_template_library_version(std::string version);

        bool has_current_template_library();
        bool has_next_template_library();
        bool has_template_library(std::string version);
        bool has_display_templates(std::string version);
        bool has_display_templates();

        STSTMLIB_XML_PTR load_current_template_library_xml();
        STSTMLIB_XML_PTR load_next_template_library_xml();
        STSTMLIB_XML_PTR load_template_library_xml(std::string version);
        ITemplateLibraryPtr load_template_library();

        void add_template_library(std::string version, const Blob& blob);
        void remove_template_library(std::string version);

        const std::string& get_lcd_template(const std::string& id);
        const std::string& get_lcd_emergency_template(const std::string& id);
        const std::string& get_led_template(const std::string& id);
        const std::string& get_led_emergency_template(const std::string& id);

        Blob download_display_template_library(std::string version);
        std::map<path, Blob> download_templates(const std::vector<path>& excludes = {});

        // both
        void set_versions(const CurNxtMsgTmpLibVers& versions);
        CurNxtMsgTmpLibVers versions();
        bool has_invalid_version();
        bool has_empty_library();
        std::string get_library_version(const std::string& category, const std::string& stage);
        void set_library_version(const std::string& category, const std::string& stage, std::string version);
        void upgrade_library_version(const std::string& category, std::string version);
        bool has_library(const std::string& category, std::string version);
        void add_library(const std::string& category, std::string version, const Blob& blob);
        Blob get_library(const std::string& category, std::string version);
        void remove_library(const std::string& category, std::string version);

        auto versions_tuple()
        {
            return std::tuple_cat(current_next_message_library_versions(), current_next_template_library_versions());
        }

        auto category_versions_tuple()
        {
            return std::make_tuple(std::make_tuple("message"s, "current"s, current_message_library_version()),
                                   std::make_tuple("message"s, "next"s, next_message_library_version()),
                                   std::make_tuple("template"s, "current"s, current_template_library_version()),
                                   std::make_tuple("template"s, "next"s, next_template_library_version()));
        }

        // library download

        Blob download_library(const std::string& category, std::string version);

        // status

        AllCurNxtMsgTmpLibVers get_this_station_iscs_stis_library_versions();
        AllCurNxtMsgTmpLibVersInfoList get_all_station_iscs_stis_library_versions();

        struct Impl;
        std::shared_ptr<Impl> m_impl;
    };

    using STISLibraryClientPtr = std::shared_ptr<STISLibraryClient>;
}

namespace TA_IRS_App::STIS_PROTOCOL::INTERFACES
{
    using stislibraryclient::detail::STISLibraryClient;
    using stislibraryclient::detail::STISLibraryClientPtr;
}

namespace TA_IRS_App::STIS_PROTOCOL
{
    using namespace INTERFACES;
}
