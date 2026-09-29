#pragma once
#include "STISStatus.h"
#include "core/utility/src/core/Vector.h"
#include "app/signs/common_library/src/stis_protocol/message_types/MessageDataTypes.h"

namespace TA_IRS_App::STIS_PROTOCOL::SIMULATOR::stissimulator::detail
{
    using namespace MESSAGE_TYPES;
    using namespace MESSAGE_DATA_TYPES;

    struct PlatformInformationDisplay
    {
        void on_message(const M10& m);
        void on_message(const M11& m);
        void on_message(const M20& m);
        void on_message(const M21& m);
        void on_message(const M22& m);
        void on_message(const M23& m);
        void on_message(const M24& m);

        void set_status(int status);
        void set_status(EPIDStatus status);

        std::string get_message_status() const;
        void remove_outdated_messages();

        struct Message
        {
            std::string tag;
            std::string start_time;
            std::string end_time;
            int priority = 0;
            std::string text;
        };

        using MessageList = thread_safe::vector<Message>;
        MessageList messages;

        std::string id;
        EPIDStatus status = EPIDStatus::Off;
        TLPRE display_template;
        STISStatusPtr m_stis;
    };
}
