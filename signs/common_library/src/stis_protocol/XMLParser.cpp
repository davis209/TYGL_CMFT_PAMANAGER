#include "pch.h"
#include "XMLParser.h"
#include "CommonDefs.h"
#include "core/utilities/src/CallstackLogger.h"
#include "core/utility/src/base_ex/DebugUtilEx.h"
#include "core/utility/src/base_ex/SimpleSQLite.h"
#include "core/utility/src/base_ex/RunParamsEx.h"
#include "core/utility/src/core/FileEx.h"
#include "core/utility/src/core/CacheDecorator.h"
#include "core/utility/src/core/SimpleBOMParser.h"
#include "core/utility/src/core/MakeNameValuePairsString.h"
#include "core/utility/src/core/algorithm/strings.h"
#include "core/data_access_interface/tis_agent_4669/src/ITemplateLibrary.h"
#include "core/data_access_interface/tis_agent_4669/src/IPredefinedMessageLibrary.h"
#include <rapidxml.hpp>
#include <boost/assign.hpp>
#include <string>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>

#define RPARAM_DEBUGSTISXMLPARSER "DebugSTISXMLParser"

using boost::filesystem::path;
using namespace rapidxml;
using XmlDocument = rapidxml::xml_document<>;
using XmlNode = rapidxml::xml_node<>;
using namespace TA_Base_Ex;
using st::FileEx;
using st::make_cached;
using st::CacheDecorator;
using TA_Base_Ex::RunParamsEx;

namespace
{
    auto s_is_debug = [] { return RunParamsEx::is_true(RPARAM_DEBUGSTISXMLPARSER, "--quiet"); };
}

namespace TA_IRS_App::STIS_PROTOCOL::IMPL::xmlparser::detail
{
    STSMSGLIB_XML_PTR parse_message_library_by_string(const std::string& str)
    {
        auto debug = s_is_debug();

        auto parse_PREDEFMSG = [&](XmlNode* node)
        {
            STSMSGLIB_XML::PREDEFMSGLIB::MESSAGE::PREDEFMSG m;
            m.MsgTag = node->first_node("MsgTag")->value();
            m.Description = node->first_node("Description")->value();
            m.Priority = node->first_node("Priority")->value();
            m.MsgAttribute = node->first_node("MsgAttribute")->value();
            m.LEDEmgTemplate = node->first_node("LEDEmgTemplate")->value();
            m.LCDEmgTemplate = node->first_node("LCDEmgTemplate")->value();
            m.EnglishMesg = node->first_node("EnglishMesg")->value();
            m.ChineseMesg = node->first_node("ChineseMesg")->value();
            m.MalayMesg = node->first_node("MalayMesg")->value();
            m.TamilMesg = node->first_node("TamilMesg")->value();
            LOG_DEBUG_IF(debug, "parse_PREDEFMSG(): %s", nvps(m.MsgTag, m.Description, m.Priority, m.MsgAttribute, m.LEDEmgTemplate, m.LCDEmgTemplate, m.EnglishMesg, m.ChineseMesg, m.MalayMesg, m.TamilMesg));
            return m;
        };

        auto parse_PREDEFMSGLIST = [&](XmlNode* node)
        {
            STSMSGLIB_XML::PREDEFMSGLIB::MESSAGE::PREDEFMSGLIST list;

            for (auto msg = node->first_node(); msg; msg = msg->next_sibling())
            {
                list.emplace_back(parse_PREDEFMSG(msg));
            }

            return list;
        };

        auto parse_MESSAGE = [&](XmlNode* node)
        {
            STSMSGLIB_XML::PREDEFMSGLIB::MESSAGE m;
            m.MessageType = node->first_node("MessageType")->value();
            m.NumberOfPredefinedMessage = node->first_node("NumberOfPredefinedMessage")->value();
            m.PredefMsgList = parse_PREDEFMSGLIST(node->first_node("PredefMsgList"));
            LOG_DEBUG_IF(debug, "parse_MESSAGE(): %s", nvps(m.MessageType, m.NumberOfPredefinedMessage));
            return m;
        };

        auto parse_PREDEFINEDMESSAGELIBRARY = [&](XmlNode* node)
        {
            STSMSGLIB_XML::PREDEFMSGLIB l;
            l.DatabaseLength = node->first_node("DatabaseLength")->value();
            l.Version = node->first_node("Version")->value();
            l.CreatedBy = node->first_node("CreatedBy")->value();
            l.CreatedDate = node->first_node("CreatedDate")->value();
            l.ModifyDate = node->first_node("ModifyDate")->value();
            l.EmergencyMessage = parse_MESSAGE(node->first_node("EmergencyMessage"));
            l.NormalMessage = parse_MESSAGE(node->first_node("NormalMessage"));
            l.PAMessage = parse_MESSAGE(node->first_node("PAMessage"));
            l.PIDDefaultMessage = parse_MESSAGE(node->first_node("PIDDefaultMessage"));
            LOG_DEBUG_IF(debug, "parse_PREDEFINEDMESSAGELIBRARY(): %s", nvps(l.DatabaseLength, l.Version, l.CreatedBy, l.CreatedDate, l.ModifyDate));
            return l;
        };

        if (str.size())
        {
            try
            {
                XmlDocument doc;
                doc.parse<0>((char*)str.data());
                STSMSGLIB_XML xml;
                xml.PredefMsgLib = parse_PREDEFINEDMESSAGELIBRARY(doc.first_node("PredefMsgLib"));
                return std::make_shared<STSMSGLIB_XML>(std::move(xml));
            }
            catch (std::exception& e)
            {
                LOG_ERROR("parse_message_library_by_string(): %s", e);
            }
        }

        return {};
    }

    IPredefinedMessageLibraryPtr parse_from_xml(STSMSGLIB_XML_PTR xml)
    {
        struct PredefinedMessageLibrary : IPredefinedMessageLibrary
        {
            PredefinedMessageLibrary(STSMSGLIB_XML_PTR xml)
            {
                this->xml = std::move(xml);
            }

            virtual ta_uint32 getKey() override { return 0; }
            virtual ta_uint32 getVersion() override { return std::stol(xml->PredefMsgLib.Version); }
            virtual std::string getLibraryType() override { return "message"; }
            virtual void getLibraryFileContent(std::vector<unsigned char>& fileContent) override {}
            virtual PredefinedMessage* getMessage(int librarySection, ta_uint16 messageTag) override { return nullptr; }
            virtual STISLedAttributes getDefaultSTISLedAttributes() override { return {}; }
            virtual STISPlasmaAttributes getDefaultSTISPlasmaAttributes() override { return {}; }
            virtual TTISLedAttributes getDefaultTTISLedAttributes() override { return {}; }
            virtual void deleteThisLibrary() override {}
            virtual void invalidate() override {}

            virtual PredefinedMessageList getMessages() override
            {
                PredefinedMessageList list; // TODO: cache

                if (xml)
                {
                    using namespace boost::assign;
                    push_back(list).range(convert(xml->PredefMsgLib.EmergencyMessage));
                    push_back(list).range(convert(xml->PredefMsgLib.NormalMessage));
                    push_back(list).range(convert(xml->PredefMsgLib.PAMessage));
                    push_back(list).range(convert(xml->PredefMsgLib.PIDDefaultMessage));
                }

                return list;
            }

            PredefinedMessageList convert(STSMSGLIB_XML::PREDEFMSGLIB::MESSAGE& message)
            {
                PredefinedMessageList list;
                auto message_type = std::stol(message.MessageType);

                for (auto& msg : message.PredefMsgList)
                {
                    auto& m = *new PredefinedMessage;
                    m.messageTag = std::stol(msg.MsgTag);
                    m.librarySection = message_type;
                    m.description = msg.Description;
                    m.priority = std::stol(msg.Priority);
                    m.message = msg.EnglishMesg + msg.ChineseMesg + msg.MalayMesg + msg.TamilMesg;
                    m.englishOnly = false;
                    m.ledEmgTemplate = std::stol(st2::last_n_digits<3>(msg.LEDEmgTemplate));
                    m.lcdEmgTemplate = std::stol(st2::last_n_digits<3>(msg.LCDEmgTemplate));
                    list.emplace_back(&m);
                }

                return list;
            };

            STSMSGLIB_XML_PTR xml;
        };

        if (xml)
        {
            return std::make_shared<PredefinedMessageLibrary>(std::move(xml));
        }

        return {};
    }

    STSTMLIB_XML_PTR parse_template_library_by_string(const std::string& str)
    {
        auto debug = s_is_debug();

        auto parse_TEMPLATE = [&](XmlNode* node)
        {
            STSTMLIB_XML::TEMPLATELIB::LXDTEMPLATE::TEMPLATE t;
            t.TemplateID = node->first_node("TemplateID")->value();
            t.Description = node->first_node("Description")->value();
            LOG_DEBUG_IF(debug, "parse_TEMPLATE(): %s", nvps(t.TemplateID, t.Description));
            return t;
        };

        auto parse_TEMPLATELIST = [&](XmlNode* node)
        {
            STSTMLIB_XML::TEMPLATELIB::LXDTEMPLATE::TEMPLATELIST list;

            for (auto t = node->first_node(); t; t = t->next_sibling())
            {
                list.emplace_back(parse_TEMPLATE(t));
            }

            return list;
        };

        auto parse_LCDLEDTEMPLATE = [&](XmlNode* node)
        {
            STSTMLIB_XML::TEMPLATELIB::LXDTEMPLATE t;
            t.TemplateType = node->first_node("TemplateType")->value();
            t.NumberOfTemplate = node->first_node("NumberOfTemplate")->value();
            t.TemplateList = parse_TEMPLATELIST(node->first_node("TemplateList"));
            LOG_DEBUG_IF(debug, "parse_LCDLEDTEMPLATE(): %s", nvps(t.TemplateType, t.NumberOfTemplate));
            return t;
        };

        auto parse_TEMPLATELIB = [&](XmlNode* node)
        {
            STSTMLIB_XML::TEMPLATELIB l;
            l.DatabaseLength = node->first_node("DatabaseLength")->value();
            l.Version = node->first_node("Version")->value();
            l.UploadedBy = node->first_node("UploadedBy")->value();
            l.UploadDate = node->first_node("UploadDate")->value();
            l.lcdemgtemplate = parse_LCDLEDTEMPLATE(node->first_node("lcdemgtemplate"));
            l.ledemgtemplate = parse_LCDLEDTEMPLATE(node->first_node("ledemgtemplate"));
            l.lcdtemplate = parse_LCDLEDTEMPLATE(node->first_node("lcdtemplate"));
            l.ledtemplate = parse_LCDLEDTEMPLATE(node->first_node("ledtemplate"));
            LOG_DEBUG_IF(debug, "parse_TEMPLATELIB(): %s", nvps(l.DatabaseLength, l.Version, l.UploadedBy, l.UploadDate));
            return l;
        };

        if (str.size())
        {
            try
            {
                XmlDocument doc;
                doc.parse<0>((char*)str.data());
                STSTMLIB_XML xml;
                xml.TemplateLib = parse_TEMPLATELIB(doc.first_node("TemplateLib"));
                return std::make_shared<STSTMLIB_XML>(std::move(xml));
            }
            catch (std::exception& e)
            {
                LOG_ERROR("parse_template_library_by_string(): %s", e);
            }
        }

        return {};
    }

    ITemplateLibraryPtr parse_from_xml(STSTMLIB_XML_PTR xml)
    {
        struct TemplateLibrary : ITemplateLibrary
        {
            TemplateLibrary(STSTMLIB_XML_PTR xml)
            {
                this->xml = std::move(xml);
            }

            virtual ta_uint32 getKey() override { return 0; }
            virtual ta_uint32 getVersion() override { return std::stol(xml->TemplateLib.Version); }
            virtual std::string getLibraryType() override { return "template"; }
            virtual void getLibraryFileContent(std::vector<unsigned char>& fileContent) override {}
            virtual Template* getTemplate(int librarySection, ta_uint16 templateTag) override { return nullptr; }
            virtual Template getDefaultLCDTemplate() override { return xml ? convert_first(xml->TemplateLib.lcdtemplate) : Template(); }
            virtual Template getDefaultLEDTemplate() override { return xml ? convert_first(xml->TemplateLib.ledtemplate) : Template(); }
            virtual void deleteThisLibrary() override {}
            virtual void invalidate() override {}

            virtual TemplateList getTemplates()
            {
                TemplateList list; // TODO: cache

                if (xml)
                {
                    using namespace boost::assign;
                    push_back(list).range(convert(xml->TemplateLib.lcdemgtemplate));
                    push_back(list).range(convert(xml->TemplateLib.ledemgtemplate));
                    push_back(list).range(convert(xml->TemplateLib.lcdtemplate));
                    push_back(list).range(convert(xml->TemplateLib.ledtemplate));
                }

                return list;
            }

            TemplateList convert(const STSTMLIB_XML::TEMPLATELIB::LXDTEMPLATE& the_template)
            {
                /**
                 * TemplateID
                 *     EMERGENCY  NORMAL
                 * LCD EMG1000    TMP3000
                 * LED EMG2001    TMP4000
                 */

                TemplateList list;
                auto type = std::stol(the_template.TemplateType);

                for (auto& tm : the_template.TemplateList)
                {
                    auto& t = *new Template;
                    t.templateType = type;
                    t.templateID = std::stol(st2::last_n_digits<3>(tm.TemplateID));
                    t.description = tm.Description;
                    list.emplace_back(&t);
                }

                return list;
            };

            Template convert_first(const STSTMLIB_XML::TEMPLATELIB::LXDTEMPLATE& the_template)
            {
                Template list;
                auto type = std::stol(the_template.TemplateType);

                for (auto& tm : the_template.TemplateList)
                {
                    Template t;
                    t.templateType = type;
                    t.templateID = std::stol(st2::last_n_digits<3>(tm.TemplateID));
                    t.description = tm.Description;
                    return t;
                }

                return {};
            };

            STSTMLIB_XML_PTR xml;
        };

        if (xml)
        {
            return std::make_shared<TemplateLibrary>(std::move(xml));
        }

        return {};
    }

    STSMSGLIB_XML_PTR XMLParser::parse_message_library(const Blob& blob)
    {
        return parse_message_library_by_string(FileEx(blob).to_unix_newline().string());
    }

    STSMSGLIB_XML_PTR XMLParser::parse_message_library(const path& xml)
    {
        LOG_CALLSTACK(boost::format("XMLParser::parse_message_library[%s]") % xml.string());

        if (!exists(xml))
        {
            LOG_DEBUG("parse_message_library(): can not find %s", xml);
            return {};
        }

        return parse_message_library_by_string(FileEx(xml).to_unix_newline().string());
    }

    STSMSGLIB_XML_PTR XMLParser::parse_message_library_by_version(const path& root_dir, const std::string& version)
    {
        return parse_message_library(make_message_library_xml_path(root_dir, version));
    }

    IPredefinedMessageLibraryPtr XMLParser::from_xml(STSMSGLIB_XML_PTR xml)
    {
        return parse_from_xml(xml);
    }

    STSTMLIB_XML_PTR XMLParser::parse_template_library(const Blob& blob)
    {
        return parse_template_library_by_string(FileEx(blob).to_unix_newline().string());
    }

    STSTMLIB_XML_PTR XMLParser::parse_template_library(const path& xml)
    {
        LOG_CALLSTACK(boost::format("XMLParser::parse_template_library[%s]") % xml.string());

        if (!exists(xml))
        {
            LOG_DEBUG("parse_template_library(): can not find %s", xml);
            return {};
        }

        return parse_template_library_by_string(FileEx(xml).to_unix_newline().string());
    }

    STSTMLIB_XML_PTR XMLParser::parse_template_library_by_version(const path& root_dir, const std::string& version)
    {
        return parse_template_library(make_template_library_xml_path(root_dir, version));
    }

    ITemplateLibraryPtr XMLParser::from_xml(STSTMLIB_XML_PTR xml)
    {
        return parse_from_xml(xml);
    }

    std::string XMLParser::make_message_library_xml_path(const path& root_dir, const std::string& version)
    {
        return (path(root_dir) / PMLIBRARY / version / "STSMSGLIB.XML").string();
    }

    std::string XMLParser::make_template_library_xml_path(const path& root_dir, const std::string& version)
    {
        return (path(root_dir) / TMLIBRARY / version / "STSTMLIB.XML").string();
    }
}

namespace TA_IRS_App::STIS_PROTOCOL::IMPL::xmlparser::detail
{
    STSMSGLIB_XML_PTR CacheedXMLParser::parse_message_library(const path& xml)
    {
        static auto s_func = make_cached([](const path& file)
        {
            return XMLParser::parse_message_library(file);
        });

        return s_func(boost::filesystem::absolute(xml).string());
    }

    STSMSGLIB_XML_PTR CacheedXMLParser::parse_message_library_by_version(const path& root_dir, std::string version)
    {
        return parse_message_library(XMLParser::make_message_library_xml_path(root_dir, version));
    }

    STSTMLIB_XML_PTR CacheedXMLParser::parse_template_library(const path& xml)
    {
        static auto s_func = make_cached([](const path& file)
        {
            return XMLParser::parse_template_library(file);
        });

        return s_func(boost::filesystem::absolute(xml).string());
    }

    STSTMLIB_XML_PTR CacheedXMLParser::parse_template_library_by_version(const path& root_dir, std::string version)
    {
        return parse_template_library(XMLParser::make_template_library_xml_path(root_dir, version));
    }
}
