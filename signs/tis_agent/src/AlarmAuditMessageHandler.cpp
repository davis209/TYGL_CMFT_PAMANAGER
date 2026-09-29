/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 *
 * Source:    $File: //depot/TYGL_CMFT_TIP/J155/transactive/app/signs/tis_agent/src/AlarmAuditMessageHandler.cpp $
 * @author:   Robin Ashcroft
 * @version:  $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * Manages all Station Traveller Information System-related functionality
 *
 * Implements the ISTISManagerCorbaDef CORBA interface
 *
 */

#include "pch.h"
#include "AlarmAuditMessageHandler.h"
#include "app/signs/tis_agent/src/stis_protocol/STISEvents.h"
#include "app/signs/common_library/src/stis_protocol/message_types/MessageTypes.h"
#include "core/utility/src/base_ex/DescriptionParametersEx.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/src/CommsMessageSender.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/TISAlarms_MessageTypes.h"
#include "core/message/types/TISAudit_MessageTypes.h"
#include "core/message/types/TISComms_MessageTypes.h"
#include "core/alarm/src/AlarmHelper.h"
#include "core/alarm/src/AlarmConstants.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/alarm/src/NonUniqueAlarmHelper.h"

namespace TA_IRS_App::alarmauditmessagehandler::detail
{
    using namespace STIS_PROTOCOL::MESSAGE_TYPES;
    using st::Serialize;

    struct AlarmAuditMessageHandler::Impl
    {
        Impl(std::string options = "")
        {
            parse_options(std::move(options));
        }

        void parse_options(std::string options)
        {
            m_options = std::move(options);
        }

        void start()
        {
            m_audit.reset(MessagePublicationManager::getInstance().getAuditMessageSender(TISAudit::Context));
            m_alarm = &AlarmHelperManager::getInstance().getAlarmHelper();

            s_stis_events->connect("library-upgrade-started", [&](auto args)
            {
                using Args = std::tuple<std::string, std::string, std::string>;
                auto&& [category, version, session] = Serialize::deserialize<Args>(std::move(args));
                // TODO: send audit message
            });

            s_stis_events->connect("library-upgrade-completed", [&](auto args)
            {
                using Args = std::tuple<std::string, std::string, std::string>;
                auto&& [category, version, session] = Serialize::deserialize<Args>(std::move(args));
                // TODO: send audit message
            });

            s_stis_events->connect("library-upgrade-timedout", [&](auto args)
            {
                using Args = std::tuple<std::string, std::string, std::string, std::vector<std::string>>;
                auto&& [category, version, session, failed] = Serialize::deserialize<Args>(std::move(args));
                // TODO: send audit message
            });

            s_stis_events->connect("library-downloaded", [&](auto args)
            {
                using Args = std::tuple<std::string, std::string>;
                auto&& [category, version] = Serialize::deserialize<Args>(std::move(args));
                // TODO: send audit message
            });
        }

        void stop()
        {
        }

        void submitAuditMessage(const MessageType& type, const DescriptionParameters& desc, const std::string& session)
        {
            m_audit->sendAuditMessage(type,
                                      m_entityKey,
                                      desc,
                                      "", // Further description text
                                      session,
                                      "", // alarm ID - not required
                                      "", // incident key - not required
                                      ""); // event key - not required
        }

        void submitAlarm(const MessageType& type, const DescriptionParameters& dp)
        {
            if (type.getTypeKey() == TISAlarms::STISServerCommFailure.getTypeKey())
            {
                m_alarm->submitAlarmWithAsset(type,
                                              m_entityKey,
                                              m_entityTypeKey,
                                              dp,
                                              m_entityName,
                                              m_locationKey,
                                              m_subsystemKey,
                                              m_agentAssetName,
                                              AlarmConstants::defaultTime);
            }
            else
            {
                closeAlarm(type);
                m_alarm->submitAlarmWithAsset(type,
                                              m_entityKey,
                                              m_entityTypeKey,
                                              dp,
                                              m_entityName,
                                              m_locationKey,
                                              m_subsystemKey,
                                              m_agentAssetName,
                                              AlarmConstants::defaultTime);
            }
        }

        void closeAlarm(const MessageType& type)
        {
            m_alarm->closeAlarmAtLocation(type, m_entityKey, m_locationKey);
        }

        std::string m_entityName;
        std::string m_agentAssetName;
        std::uint32_t m_entityKey;
        std::uint32_t m_locationKey;
        std::uint32_t m_entityTypeKey;
        std::uint32_t m_subsystemKey;
        AlarmHelper* m_alarm = nullptr;
        AuditMessageSenderPtr m_audit;
        std::string m_options;

#if 0
        m_entityKey = m_stisEntityData->getKey();
        m_entityTypeKey = m_stisEntityData->getTypeKey();
        m_locationKey = m_stisEntityData->getLocation();
        m_locationName = m_stisEntityData->getLocationName();
        m_subsystemKey = m_stisEntityData->getSubsystem();
        m_serverIPAddress = m_stisEntityData->getServerIPAddress();
        m_serverPort = m_stisEntityData->getServerPort();
        m_messageTimeout = m_stisEntityData->getMessageTimeout();
        m_messageRetries = m_stisEntityData->getMessageRetries();
        m_entityName = m_stisEntityData->getName();
        m_stationLibrarySynchronisationTimeout = m_stisEntityData->getStationLibrarySynchronisationTimeout();
        m_localDBString = m_stisEntityData->getLocalDBConnectionString();
        m_needVetting = m_stisEntityData->getVetting();
#endif
    };

    AlarmAuditMessageHandler& AlarmAuditMessageHandler::instance()
    {
        static auto s_instance = new AlarmAuditMessageHandler;
        return *s_instance;
    }

    AlarmAuditMessageHandler::AlarmAuditMessageHandler(std::string options)
        : m_impl(std::make_shared<Impl>(std::move(options)))
    {
    }

    void AlarmAuditMessageHandler::start()
    {
        m_impl->start();
    }

    void AlarmAuditMessageHandler::stop()
    {
        m_impl->stop();
    }

    void AlarmAuditMessageHandler::submitAuditMessage(const MessageType& type, const DescriptionParameters& desc, const std::string& session)
    {
        m_impl->submitAuditMessage(type, desc, session);
    }

    void AlarmAuditMessageHandler::submitAlarm(const TA_Base_Core::MessageType& type, const DescriptionParameters& dp)
    {
        m_impl->submitAlarm(type, dp);
    }

    void AlarmAuditMessageHandler::closeAlarm(const TA_Base_Core::MessageType& type)
    {
        m_impl->closeAlarm(type);
    }
}
