// test.cpp : Defines the entry point for the console application.
//

#include "pch.h"
#include "core/utility/tests/boost_test/test_utility/DebugLevelGuard.h"
#include "core/utility/tests/boost_test/test_utility/LogFileGuard.h"
#include "app/signs/common_library/src/stis_protocol/XMLParser.h"
#include "core/utility/src/base_ex/DebugUtilEx.h"
#include "core/data_access_interface/tis_agent_4669/src/ITemplateLibrary.h"
#include "core/data_access_interface/tis_agent_4669/src/IPredefinedMessageLibrary.h"
#include <boost/test/unit_test.hpp>

using namespace TA_Base_Core;
using namespace TA_IRS_App::STIS_PROTOCOL;
using namespace TA_IRS_App::STIS_PROTOCOL::IMPL;

namespace
{
    auto STSMSGLIB  = R"(data\sftp_root\PMLIBRARY\001\STSMSGLIB.XML)";
    auto STSTMLIB   = R"(data\sftp_root\tmlibrary\001\STSTMLIB.XML)";

    DebugLevelGuard s_debuglevel = DebugUtil::DebugDebug;
}

BOOST_AUTO_TEST_SUITE(SIGNS)
BOOST_AUTO_TEST_SUITE(COMMON)
BOOST_AUTO_TEST_SUITE(PROTOCOL)
BOOST_AUTO_TEST_SUITE(XMLParserTest)

BOOST_AUTO_TEST_CASE(test_STSMSGLIB_XML)
{
    STSMSGLIB_XML_PTR xml = XMLParser::parse_message_library(STSMSGLIB);
    BOOST_TEST(xml->PredefMsgLib.Version == "001");
}

BOOST_AUTO_TEST_CASE(test_STSTMLIB_XML)
{
    STSTMLIB_XML_PTR xml = XMLParser::parse_template_library(STSTMLIB);
    BOOST_TEST(xml->TemplateLib.Version == "001");
}

BOOST_AUTO_TEST_CASE(test_from_xml_PredefinedMessageLibrary)
{
    LogFileGuard log("SIGNS-COMMON-PROTOCOL-XMLParserTest-test_from_xml_PredefinedMessageLibrary.log");

    auto xml = XMLParser::parse_message_library(STSMSGLIB);
    auto msg_lib = XMLParser::from_xml(xml);
    auto msgs = msg_lib->getMessages();
    BOOST_TEST(msgs.size());

    boost::for_each(msgs, [](auto * m)
    {
        LOG_INFO("MSGPRE: {tag=%d, section=%d, description=%s, priority=%d, message=%s, led=%d, lcd=%d}",
                 m->messageTag, m->librarySection, m->description, m->priority, m->message, m->ledEmgTemplate, m->lcdEmgTemplate);
    });
}

BOOST_AUTO_TEST_CASE(test_from_xml_Template)
{
    LogFileGuard log("SIGNS-COMMON-PROTOCOL-XMLParserTest-test_from_xml_Template.log");

    auto xml = XMLParser::parse_template_library(STSTMLIB);
    auto tm_lib = XMLParser::from_xml(xml);
    BOOST_TEST(tm_lib != nullptr);
    auto tms = tm_lib->getTemplates();
    BOOST_TEST(tms.size());

    boost::for_each(tms, [](auto * t)
    {
        LOG_INFO("TEMPLATE: {id=%d, description=%s, type=%d}", t->templateID, t->description, t->templateType);
    });
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
