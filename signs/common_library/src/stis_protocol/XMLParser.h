#pragma once
#include "STSTMLIB_XML.h"
#include "STSMSGLIB_XML.h"
#include <boost/filesystem.hpp>

namespace TA_Base_Core
{
    class ITemplateLibrary;
    using ITemplateLibraryPtr = std::shared_ptr<ITemplateLibrary>;

    class IPredefinedMessageLibrary;
    using IPredefinedMessageLibraryPtr = std::shared_ptr<IPredefinedMessageLibrary>;
}

namespace TA_IRS_App::STIS_PROTOCOL::IMPL::xmlparser::detail
{
    using namespace TA_Base_Core;
    using Blob = std::vector<unsigned char>;
    using boost::filesystem::path;

    struct XMLParser
    {
        static STSMSGLIB_XML_PTR parse_message_library(const Blob& blob);
        static STSMSGLIB_XML_PTR parse_message_library(const path& xml);
        static STSMSGLIB_XML_PTR parse_message_library_by_version(const path& root_dir, const std::string& version);
        static IPredefinedMessageLibraryPtr from_xml(STSMSGLIB_XML_PTR xml);

        static STSTMLIB_XML_PTR parse_template_library(const Blob& blob);
        static STSTMLIB_XML_PTR parse_template_library(const path& xml);
        static STSTMLIB_XML_PTR parse_template_library_by_version(const path& root_dir, const std::string& version);
        static ITemplateLibraryPtr from_xml(STSTMLIB_XML_PTR xml);

        static std::string make_message_library_xml_path(const path& root_dir, const std::string& version);
        static std::string make_template_library_xml_path(const path& root_dir, const std::string& version);
    };

    struct CacheedXMLParser
    {
        static STSMSGLIB_XML_PTR parse_message_library(const path& xml);
        static STSMSGLIB_XML_PTR parse_message_library_by_version(const path& root_dir, std::string version);

        static STSTMLIB_XML_PTR parse_template_library(const path& xml);
        static STSTMLIB_XML_PTR parse_template_library_by_version(const path& root_dir, std::string version);
    };
}

namespace TA_IRS_App::STIS_PROTOCOL::IMPL
{
    using xmlparser::detail::XMLParser;
    using xmlparser::detail::CacheedXMLParser;
}
