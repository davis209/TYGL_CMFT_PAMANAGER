#pragma once
#include "app/signs/common_library/src/stis_protocol/message_types/MessageTypes.h"

namespace TA_Base_Core
{
    class MessageType;
}

struct STISAuditMessage
{
    using Destination = TA_IRS_App::STIS_PROTOCOL::MESSAGE_TYPES::Destination;
    using DestinationList = TA_IRS_App::STIS_PROTOCOL::MESSAGE_TYPES::DestinationList;

    // implementation

    static void sendPredefinedNormalMessage(std::string location, std::string pid_list, std::string priority, std::string start_time, std::string end_time);
    static void sendPredefinedEmergencyMessage(std::string location, std::string pid_list, std::string priority);
    static void sendAdhocNormalMessage(std::string location, std::string pid_list, std::string priority, std::string start_time, std::string end_time);
    static void sendAdhocEmergencyMessage(std::string location, std::string pid_list, std::string priority);
    static void sendPredefinedNormalTemplate(std::string location, std::string pid_list, std::string start_time, std::string end_time);
    static void sendPredefinedEmergencyTemplate(std::string location, std::string pid_list);
    static void clearNormalMessage(std::string location, std::string pid_list);
    static void clearNormalTemplate(std::string location, std::string pid_list);
    static void clearEmergencyMessageAndTemplate(std::string location, std::string pid_list);
    static void upgradeMessageLibrary(std::string version);
    static void upgradeTemplateLibrary(std::string version);
    static void createPIDGroup(std::string name, std::string pid_list);
    static void deletePIDGroup(std::string name);
    static void modifyPIDGroup(std::string name, std::string pid_list);
    static void changePIDStatus(std::string location, std::string pid, std::string on_off);
    static void saveAdhocMessage(std::string message_name, std::string message_content);

    static void send(const TA_Base_Core::MessageType& type, std::initializer_list<std::pair<std::string, std::string>> nvps, const std::string& session);

    //
    // helper functions
    //

    // DestinationList

    static void sendPredefinedNormalMessage(DestinationList dests, std::string priority, std::string start_time, std::string end_time);
    static void sendPredefinedEmergencyMessage(DestinationList dests, std::string priority);
    static void sendAdhocNormalMessage(DestinationList dests, std::string priority, std::string start_time, std::string end_time);
    static void sendAdhocEmergencyMessage(DestinationList dests, std::string priority);
    static void sendPredefinedNormalTemplate(DestinationList dests, std::string start_time, std::string end_time);
    static void sendPredefinedEmergencyTemplate(DestinationList dests);
    static void clearNormalMessage(DestinationList dests);
    static void clearNormalTemplate(DestinationList dests);
    static void clearEmergencyMessageAndTemplate(DestinationList dests);

    // Normal/Emergency

    static void sendPredefinedMessage(DestinationList dests, std::string priority, std::string start_time, std::string end_time);
    static void sendAdhocMessage(DestinationList dests, std::string priority, std::string start_time, std::string end_time);
    static void sendPredefinedTemplate(DestinationList dests, bool is_emergency, std::string start_time, std::string end_time);

    // Conditionals

    static void sendPredefinedNormalMessageIf(bool test, DestinationList dests, std::string priority, std::string start_time, std::string end_time);
    static void sendPredefinedEmergencyMessageIf(bool test, DestinationList dests, std::string priority);
    static void sendAdhocNormalMessageIf(bool test, DestinationList dests, std::string priority, std::string start_time, std::string end_time);
    static void sendAdhocEmergencyMessageIf(bool test, DestinationList dests, std::string priority);
    static void sendPredefinedNormalTemplateIf(bool test, DestinationList dests, std::string start_time, std::string end_time);
    static void sendPredefinedEmergencyTemplateIf(bool test, DestinationList dests);
    static void clearNormalMessageIf(bool test, DestinationList dests);
    static void clearNormalTemplateIf(bool test, DestinationList dests);
    static void clearEmergencyMessageAndTemplateIf(bool test, DestinationList dests);
    static void sendPredefinedMessageIf(bool test, DestinationList dests, std::string priority, std::string start_time, std::string end_time);
    static void sendAdhocMessageIf(bool test, DestinationList dests, std::string priority, std::string start_time, std::string end_time);
};
