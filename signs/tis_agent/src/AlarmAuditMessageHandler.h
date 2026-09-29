/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 *
 * Source:    $File: //depot/TYGL_CMFT_TIP/J155/transactive/app/signs/tis_agent/src/AlarmAuditMessageHandler.h $
 * @author:   Robin Ashcroft
 * @version:  $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 */

#pragma once
#include "core/message/types/MessageTypes.h"
#include "core/message/src/NameValuePair.h"
#include <memory>

namespace TA_IRS_App::alarmauditmessagehandler::detail
{
    using namespace TA_Base_Core;

    struct AlarmAuditMessageHandler
    {
        static AlarmAuditMessageHandler& instance();

        AlarmAuditMessageHandler(std::string options = "");

        void parse_options(std::string options);

        void start();
        void stop();

        void submitAuditMessage(const MessageType& type, const DescriptionParameters& desc, const std::string& session);
        void submitAlarm(const TA_Base_Core::MessageType& type, const DescriptionParameters& dp);
        void closeAlarm(const TA_Base_Core::MessageType& type);

        struct Impl;
        std::shared_ptr<Impl> m_impl;
    };
}

namespace TA_IRS_App
{
    using alarmauditmessagehandler::detail::AlarmAuditMessageHandler;
}
