#include "pch.h"
#include "STISAuditMessage.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/TISAudit_MessageTypes.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/src/CommsMessageSender.h"
#include "core/message/types/MessageTypes.h"
#include "core/message/src/NameValuePair.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/utility/src/core/StaticObject.h"
#include "core/utility/src/base_ex/EntityAccessFactoryEx.h"
#include "core/utility/src/base_ex/LocationAccessFactoryEx.h"
#include "core/utility/src/base_ex/RunParamsEx.h"
#include "core/utility/src/base_ex/DebugUtilEx.h"
#include "core/utility/src/base_ex/DescriptionParametersEx.h"

using namespace std::literals;
using st::StaticObject;
using TA_Base_Ex::RunParamsEx;
using TA_Base_Ex::DescriptionParametersEx;
using TA_Base_Ex::ThisEntity;
using TA_Base_Ex::LocationEx;
using namespace TA_Base_Core;
using TA_IRS_App::STIS_PROTOCOL::MESSAGE_TYPES::Destination;
using TA_IRS_App::STIS_PROTOCOL::MESSAGE_TYPES::DestinationList;

namespace
{
    void send_audit_message_impl(const MessageType& type, DescriptionParametersEx desc, const std::string& session)
    {
        static auto s_entity = ThisEntity::key();
        static auto s_sender = MessagePublicationManager::getInstance().getAuditMessageSender(STISAudit::Context);
        LOG_MSGPUB("AuditMessageSender::sendAuditMessage(): context=%s, type=%s, desc=%s", STISAudit::Context.getContextName(), type.getTypeName(), desc.dump());

        s_sender->sendAuditMessage(type,
                                   s_entity,
                                   desc,
                                   "",  // Further description text
                                   session,
                                   "",  // alarm ID - not required
                                   "",  // incident key - not required
                                   ""); // event key - not required
    }

    void send_audit_message(const MessageType& type, DescriptionParametersEx desc)
    {
        static auto s_session = RunParamsEx::get_or(RPARAM_SESSIONID, SessionAccessFactory::getInstance().getSuperSessionId());
        send_audit_message_impl(type, std::move(desc), s_session);
    }

    std::string to_string_location(const std::string& station_id)
    {
        return station_id.empty() ? "All Station"s : LocationEx::to_name(station_id);
    }

    std::string to_string_pid(const std::vector<std::string>& pids)
    {
        return pids.empty() ? "All PID"s : boost::algorithm::join(pids, ",");
    }
}

void STISAuditMessage::sendPredefinedNormalMessage(std::string location, std::string pid_list, std::string priority, std::string start_time, std::string end_time)
{
    send_audit_message(STISAudit::SendPredefinedNormalMessage,
                       {
                           {"Priority"s, std::move(priority)},
                           {"Location"s, std::move(location)},
                           {"PID list"s, std::move(pid_list)},
                           {"Start time"s, std::move(start_time)},
                           {"End time"s, std::move(end_time)},
                       });
}

void STISAuditMessage::sendPredefinedEmergencyMessage(std::string location, std::string pid_list, std::string priority)
{
    send_audit_message(STISAudit::SendPredefinedEmergencyMessage,
                       {
                           {"Priority"s, std::move(priority)},
                           {"Location"s, std::move(location)},
                           {"PID list"s, std::move(pid_list)},
                       });
}

void STISAuditMessage::sendAdhocNormalMessage(std::string location, std::string pid_list, std::string priority, std::string start_time, std::string end_time)
{
    send_audit_message(STISAudit::SendAdhocNormalMessage,
                       {
                           {"Priority"s, std::move(priority)},
                           {"Location"s, std::move(location)},
                           {"PID list"s, std::move(pid_list)},
                           {"Start time"s, std::move(start_time)},
                           {"End time"s, std::move(end_time)},
                       });
}

void STISAuditMessage::sendAdhocEmergencyMessage(std::string location, std::string pid_list, std::string priority)
{
    send_audit_message(STISAudit::SendAdhocEmergencyMessage,
                       {
                           {"Priority"s, std::move(priority)},
                           {"Location"s, std::move(location)},
                           {"PID list"s, std::move(pid_list)},
                       });
}

void STISAuditMessage::sendPredefinedNormalTemplate(std::string location, std::string pid_list, std::string start_time, std::string end_time)
{
    send_audit_message(STISAudit::SendPredefinedNormalTemplate,
                       {
                           {"Location"s, std::move(location)},
                           {"PID list"s, std::move(pid_list)},
                           {"Start time"s, std::move(start_time)},
                           {"End time"s, std::move(end_time)},
                       });
}

void STISAuditMessage::sendPredefinedEmergencyTemplate(std::string location, std::string pid_list)
{
    send_audit_message(STISAudit::SendPredefinedEmergencyTemplate,
                       {
                           {"Location"s, std::move(location)},
                           {"PID list"s, std::move(pid_list)},
                       });
}

void STISAuditMessage::clearNormalMessage(std::string location, std::string pid_list)
{
    send_audit_message(STISAudit::ClearNormalMessage,
                       {
                           {"Location"s, std::move(location)},
                           {"PID list"s, std::move(pid_list)},
                       });
}

void STISAuditMessage::clearNormalTemplate(std::string location, std::string pid_list)
{
    send_audit_message(STISAudit::ClearNormalTemplate,
                       {
                           {"Location"s, std::move(location)},
                           {"PID list"s, std::move(pid_list)},
                       });
}

void STISAuditMessage::clearEmergencyMessageAndTemplate(std::string location, std::string pid_list)
{
    send_audit_message(STISAudit::ClearEmergencyMessageAndTemplate,
                       {
                           {"Location"s, std::move(location)},
                           {"PID list"s, std::move(pid_list)},
                       });
}

void STISAuditMessage::upgradeMessageLibrary(std::string version)
{
    send_audit_message(STISAudit::UpgradeMessageLibrary,
                       {
                           {"Version"s, std::move(version)},
                       });
}

void STISAuditMessage::upgradeTemplateLibrary(std::string version)
{
    send_audit_message(STISAudit::UpgradeTemplateLibrary,
                       {
                           {"Version"s, std::move(version)},
                       });
}

void STISAuditMessage::createPIDGroup(std::string name, std::string pid_list)
{
    send_audit_message(STISAudit::CreatePIDGroup,
                       {
                            {"name"s, std::move(name)},
                            {"PID list"s, std::move(pid_list)},
                       });
}

void STISAuditMessage::deletePIDGroup(std::string name)
{
    send_audit_message(STISAudit::DeletePIDGroup,
                       {
                            {"name"s, std::move(name)},
                       });
}

void STISAuditMessage::modifyPIDGroup(std::string name, std::string pid_list)
{
    send_audit_message(STISAudit::ModifyPIDGroup,
                       {
                            {"name"s, std::move(name)},
                            {"PID list"s, std::move(pid_list)},
                       });
}

void STISAuditMessage::changePIDStatus(std::string location, std::string pid, std::string on_off)
{
    send_audit_message(STISAudit::ChangePIDStatus,
                       {
                            {"Location"s, std::move(location)},
                            {"PID"s, std::move(pid)},
                            {"on/off"s, std::move(on_off)},
                       });
}

void STISAuditMessage::saveAdhocMessage(std::string message_name, std::string message_content)
{
    send_audit_message(STISAudit::SaveAdhocMessage,
                       {
                            {"MessageName"s, std::move(message_name)},
                            {"MessageContent"s, std::move(message_content)},
                       });
}

void STISAuditMessage::send(const MessageType& type, std::initializer_list<std::pair<std::string, std::string>> nvps, const std::string& session)
{
    send_audit_message_impl(type, nvps, session);
}

//
// helper functions
//

// DestinationList

void STISAuditMessage::sendPredefinedNormalMessage(DestinationList dests, std::string priority, std::string start_time, std::string end_time)
{
    for (auto& dest : dests)
    {
        sendPredefinedNormalMessage(to_string_location(dest.station_id), to_string_pid(dest.pid_list), priority, start_time, end_time);
    }
}

void STISAuditMessage::sendPredefinedEmergencyMessage(DestinationList dests, std::string priority)
{
    for (auto& dest : dests)
    {
        sendPredefinedEmergencyMessage(to_string_location(dest.station_id), to_string_pid(dest.pid_list), priority);
    }
}

void STISAuditMessage::sendAdhocNormalMessage(DestinationList dests, std::string priority, std::string start_time, std::string end_time)
{
    for (auto& dest : dests)
    {
        sendAdhocNormalMessage(to_string_location(dest.station_id), to_string_pid(dest.pid_list), priority, start_time, end_time);
    }
}

void STISAuditMessage::sendAdhocEmergencyMessage(DestinationList dests, std::string priority)
{
    for (auto& dest : dests)
    {
        sendAdhocEmergencyMessage(to_string_location(dest.station_id), to_string_pid(dest.pid_list), priority);
    }
}

void STISAuditMessage::sendPredefinedNormalTemplate(DestinationList dests, std::string start_time, std::string end_time)
{
    for (auto& dest : dests)
    {
        sendPredefinedNormalTemplate(to_string_location(dest.station_id), to_string_pid(dest.pid_list), start_time, end_time);
    }
}

void STISAuditMessage::sendPredefinedEmergencyTemplate(DestinationList dests)
{
    for (auto& dest : dests)
    {
        sendPredefinedEmergencyTemplate(to_string_location(dest.station_id), to_string_pid(dest.pid_list));
    }
}

void STISAuditMessage::clearNormalMessage(DestinationList dests)
{
    for (auto& dest : dests)
    {
        clearNormalMessage(to_string_location(dest.station_id), to_string_pid(dest.pid_list));
    }
}

void STISAuditMessage::clearNormalTemplate(DestinationList dests)
{
    for (auto& dest : dests)
    {
        clearNormalTemplate(to_string_location(dest.station_id), to_string_pid(dest.pid_list));
    }
}

void STISAuditMessage::clearEmergencyMessageAndTemplate(DestinationList dests)
{
    for (auto& dest : dests)
    {
        clearEmergencyMessageAndTemplate(to_string_location(dest.station_id), to_string_pid(dest.pid_list));
    }
}

// Normal/Emergency

void STISAuditMessage::sendPredefinedMessage(DestinationList dests, std::string priority, std::string start_time, std::string end_time)
{
    4 <= std::stoi(priority)
        ? sendPredefinedNormalMessage(dests, priority, start_time, end_time)
        : sendPredefinedEmergencyMessage(dests, priority)
        ;
}

void STISAuditMessage::sendAdhocMessage(DestinationList dests, std::string priority, std::string start_time, std::string end_time)
{
    4 <= std::stoi(priority)
        ? sendAdhocNormalMessage(dests, priority, start_time, end_time)
        : sendAdhocEmergencyMessage(dests, priority)
        ;
}

void STISAuditMessage::sendPredefinedTemplate(DestinationList dests, bool is_emergency, std::string start_time, std::string end_time)
{
    is_emergency
        ? sendPredefinedEmergencyTemplate(std::move(dests))
        : sendPredefinedNormalTemplate(std::move(dests), std::move(start_time), std::move(end_time))
        ;
}

// Conditionals

void STISAuditMessage::sendPredefinedNormalMessageIf(bool test, DestinationList dests, std::string priority, std::string start_time, std::string end_time)
{
    if (test)
    {
        sendPredefinedNormalMessage(std::move(dests), std::move(priority), std::move(start_time), std::move(end_time));
    }
}

void STISAuditMessage::sendPredefinedEmergencyMessageIf(bool test, DestinationList dests, std::string priority)
{
    if (test)
    {
        sendPredefinedEmergencyMessage(std::move(dests), std::move(priority));
    }
}

void STISAuditMessage::sendAdhocNormalMessageIf(bool test, DestinationList dests, std::string priority, std::string start_time, std::string end_time)
{
    if (test)
    {
        sendAdhocNormalMessage(std::move(dests), std::move(priority), std::move(start_time), std::move(end_time));
    }
}

void STISAuditMessage::sendAdhocEmergencyMessageIf(bool test, DestinationList dests, std::string priority)
{
    if (test)
    {
        sendAdhocEmergencyMessage(std::move(dests), std::move(priority));
    }
}

void STISAuditMessage::sendPredefinedNormalTemplateIf(bool test, DestinationList dests, std::string start_time, std::string end_time)
{
    if (test)
    {
        sendPredefinedNormalTemplate(std::move(dests), std::move(start_time), std::move(end_time));
    }
}

void STISAuditMessage::sendPredefinedEmergencyTemplateIf(bool test, DestinationList dests)
{
    if (test)
    {
        sendPredefinedEmergencyTemplate(std::move(dests));
    }
}

void STISAuditMessage::clearNormalMessageIf(bool test, DestinationList dests)
{
    if (test)
    {
        clearNormalMessage(std::move(dests));
    }
}

void STISAuditMessage::clearNormalTemplateIf(bool test, DestinationList dests)
{
    if (test)
    {
        clearNormalTemplate(std::move(dests));
    }
}

void STISAuditMessage::clearEmergencyMessageAndTemplateIf(bool test, DestinationList dests)
{
    if (test)
    {
        clearEmergencyMessageAndTemplate(std::move(dests));
    }
}

void STISAuditMessage::sendPredefinedMessageIf(bool test, DestinationList dests, std::string priority, std::string start_time, std::string end_time)
{
    if (test)
    {
        sendPredefinedMessage(std::move(dests), std::move(priority), std::move(start_time), std::move(end_time));
    }
}

void STISAuditMessage::sendAdhocMessageIf(bool test, DestinationList dests, std::string priority, std::string start_time, std::string end_time)
{
    if (test)
    {
        sendAdhocMessage(std::move(dests), std::move(priority), std::move(start_time), std::move(end_time));
    }
}
