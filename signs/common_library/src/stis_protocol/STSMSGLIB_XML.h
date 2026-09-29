#pragma once
#include <string>
#include <vector>
#include <memory>
#include <algorithm>

namespace TA_IRS_App::STIS_PROTOCOL::XML
{
    struct STSMSGLIB_XML
    {
        struct PREDEFMSGLIB
        {
            struct MESSAGE
            {
                struct PREDEFMSG
                {
                    std::string MsgTag;
                    std::string Description;
                    std::string Priority;
                    std::string MsgAttribute;
                    std::string LEDEmgTemplate;
                    std::string LCDEmgTemplate;
                    std::string EnglishMesg;
                    std::string ChineseMesg;
                    std::string MalayMesg;
                    std::string TamilMesg;
                };

                using PREDEFMSGLIST = std::vector<PREDEFMSG>;

                std::string MessageType;
                std::string NumberOfPredefinedMessage;
                PREDEFMSGLIST PredefMsgList;
            };

            std::string DatabaseLength;
            std::string Version;
            std::string CreatedBy;
            std::string CreatedDate;
            std::string ModifyDate;
            MESSAGE EmergencyMessage;
            MESSAGE NormalMessage;
            MESSAGE PAMessage;
            MESSAGE PIDDefaultMessage;
        };

        PREDEFMSGLIB PredefMsgLib;

        auto get_message(const PREDEFMSGLIB::MESSAGE::PREDEFMSGLIST& messages, std::string type, std::string tag) const
        {
            tag = tag.size() == 3 ? type + tag : tag;
            auto it = std::find_if(messages.begin(), messages.end(), [&](auto & m) { return m.MsgTag == tag; });
            return it != messages.end() ? *it : PREDEFMSGLIB::MESSAGE::PREDEFMSG{};
        }

        auto get_normal_message(std::string tag) const
        {
            return get_message(PredefMsgLib.NormalMessage.PredefMsgList, "1", tag);
        }

        auto get_emergency_message(std::string tag) const
        {
            return get_message(PredefMsgLib.EmergencyMessage.PredefMsgList, "0", tag);
        }
    };

    using STSMSGLIB_XML_PTR = std::shared_ptr<STSMSGLIB_XML>;
}

namespace TA_IRS_App::STIS_PROTOCOL
{
    using namespace XML;
}
