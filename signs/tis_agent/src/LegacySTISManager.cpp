/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 *
 * Source:    $File: //depot/J155_TIP/J155/transactive/app/signs/tis_agent/src/LegacySTISManager.cpp $
 * @author:   Robin Ashcroft
 * @version:  $Revision: #8 $
 *
 * Last modification: $DateTime: 2022/02/16 23:41:12 $
 * Last modified by:  $Author: limin.zhu $
 *
 */

#include "pch.h"
#include "LegacySTISManager.h"
#include "TisLibraryCache.h"
#include "stis_protocol/STISLibraryAgentServer.h"
#include "stis_protocol/STISMessageServer.h"
#include "app/signs/common_library/src/stis_protocol/STISMessageClient.h"
#include "bus/signs_4669/TisManagerIDL/src/ISTISManagerCorbaDef.h"
#include "core/utility/src/base_ex/DebugUtilEx.h"
#include "core/utility/src/base_ex/DAI.h"
#include "core/utility/src/core/CorbaDefUtil.h"
#include "core/corba/src/ServantBase.h"

namespace TA_IRS_App::legacytisagent::detail
{
    using namespace TA_Base_Ex;
    using namespace TA_Base_Core;
    using namespace TA_Base_Bus;
    using namespace STIS_PROTOCOL::MESSAGE_TYPES;
    using namespace STIS_PROTOCOL::INTERFACES;

    struct NullSTISManagerCorbaDefImpl : virtual POA_TA_Base_Bus::ISTISManagerCorbaDef
    {
        virtual RATISMessageList* getAllIncomingRATISMessages() override { return new RATISMessageList{}; }
        virtual RATISMessageDetails* getIncomingRATISMessage(CORBA::Long messageID) override { static RATISMessageDetails s_obj; return new RATISMessageDetails{s_obj}; }
        virtual void submitPredefinedDisplayRequest(const STISDestinationList& destinationList, ELibrarySection librarySection, CORBA::UShort libraryVersion, CORBA::UShort messageTag, const char* startTime, const char* endTime, CORBA::UShort priority, const char* sessionID) = 0;
        virtual void submitAdHocDisplayRequest(const STISDestinationList& destinationList, const char* messageContent, const char* startTime, const char* endTime, CORBA::UShort priority, const DisplayAttributes& displayAttr, const PlasmaAttributes& plasmaAttr, const LEDAttributes& LEDAttr, const char* sessionID) override {}
        virtual void submitClearRequest(const STISDestinationList& destinationList, CORBA::UShort upperPriority, CORBA::UShort lowerPriority, const char* sessionID) override {}
        virtual void upgradePredefinedStationMessageLibrary(CORBA::UShort newLibraryVersion, const char* sessionID) override {}
        virtual void upgradePredefinedTrainMessageLibrary(CORBA::UShort newLibraryVersion, const char* sessionID) override {}
        virtual CurrentDisplayingMessage* getCurrentDisplayingMessage(const STISDestination& destination) override { static CurrentDisplayingMessage s_obj; return new CurrentDisplayingMessage{s_obj}; }
        virtual void submitRATISVettingResponse(CORBA::Long key, CORBA::Boolean approved, CORBA::UShort priority, const char* content, const char* sessionID) override {}
        virtual void reApproveVettedRATISMessage(const RATISMessageDetails& ratisMessage, const char* sessionID) override {}
        virtual void submitRATISDisplayRequest(const char* messageContent, CORBA::UShort priority, const char* RATISTag, const char* RATISDestination, const char* startTime, const char* endTime, ERATISMessageType type, CORBA::Boolean overridable, CORBA::Boolean vetting, const char* sessionID) override {}
        virtual CORBA::Boolean reportStationLibraryVersionReceived(CORBA::UShort newVersion) override { return false; }
        virtual void submitPIDControlRequest(const char* destination, EPIDControl controlCommand, const char* sessionID) override {}
        virtual locationInfoDetails* getLocationInfo() override { static locationInfoDetails s_obj; return new locationInfoDetails{s_obj}; }
        virtual StorageItemList* getInformation(const StorageItemKeyList& requestList) override { return new StorageItemList{}; }
        virtual MessageLibContent* getLibrary(EMessageLibraryType type, const char* filename, CORBA::UShort newVersion) override { return new MessageLibContent{}; }
        virtual void syncLibrary(EMessageLibraryType type, const MessageLibContent& content, const char* filename, CORBA::UShort newVersion) override {}
        virtual MessageLibraryMetadataList* getMessageLibraryMetadataList(EMessageLibraryType type) override { return new MessageLibraryMetadataList{}; }
        virtual CORBA::Boolean reportTrainLibraryVersionReceived(CORBA::UShort newVersion) override { return false; }
        virtual CORBA::Boolean isStationLibrarySynchronisationComplete() override { return false; }
        virtual CORBA::Boolean isTrainLibrarySynchronisationComplete() override { return false; }
        virtual CORBA::UShort getCurrentSTISMessageLibraryVersion() = 0;
        virtual CORBA::UShort getCurrentTTISMessageLibraryVersion() override { return 0; }
        virtual CORBA::UShort getNextSTISMessageLibraryVersion() override { return 0; }
        virtual CORBA::UShort getNextTTISMessageLibraryVersion() override { return 0; }
        virtual CORBA::UShort getCurrentCDBSTISMessageLibraryVersion() override { return 0; }
        virtual CORBA::UShort getCurrentCDBTTISMessageLibraryVersion() override { return 0; }
        virtual CORBA::UShort getNextCDBSTISMessageLibraryVersion() override { return 0; }
        virtual CORBA::UShort getNextCDBTTISMessageLibraryVersion() override { return 0; }
        virtual CORBA::UShort getCurrentCDBTPALibraryVersion() override { return 0; }
        virtual CORBA::UShort getNextCDBTPALibraryVersion() override { return 0; }
        virtual TimeScheduleVersion getCurrentTrainTimeScheduleVersion() override { return {0, 0}; }
        virtual void setCurrentTrainTimeScheduleVersion(const TimeScheduleVersion& scheduleVersion, const char* sessionID) override {}
        virtual void setLockStatus(const char* name, CORBA::Boolean lock, const char* sessionID) override {}
        virtual CORBA::Boolean isRATISVettingOn() override { return false; }
        virtual void setRATISVetting(CORBA::Boolean on, const char* sessionID) override {}
    };

    struct LegacySTISManager::Impl : virtual ServantBase, virtual NullSTISManagerCorbaDefImpl
    {
        Impl()
        {
            m_corba_name = DAI::get_corba_name_from_string("local-tis-agent");
        }

        void start()
        {
            activateServantWithName(m_corba_name.getObjectName());
        }

        void stop()
        {
            deactivateServant();
        }

        virtual void submitPredefinedDisplayRequest(const STISDestinationList& destinationList,
                                                    ELibrarySection librarySection,
                                                    CORBA::UShort libraryVersion,
                                                    CORBA::UShort messageTag,
                                                    const char* startTime,
                                                    const char* endTime,
                                                    CORBA::UShort priority,
                                                    const char* sessionID) override
        {
            FUNCTION_ENTRY("submitPredefinedDisplayRequest()");

            DestinationList destinations;
            corbautil::for_each(destinationList, [&](auto& dst)
            {
                Destination d;
                d.station_id = dst.station.in();
                corbautil::to_cpp(d.pid_list, dst.pids);
                destinations.emplace_back(std::move(d));
            });

            PredefinedMessage msg;
            msg.message_tag = std::to_string(messageTag);
            msg.start_time = startTime;
            msg.end_time = endTime;
            msg.priority = priority;
            m_message_client.submit_M10_DisplayPredefinedMessageRequestList(destinations, msg, sessionID);

            FUNCTION_EXIT;
        }

        virtual CORBA::UShort getCurrentSTISMessageLibraryVersion() override
        {
            FUNCTION_ENTRY("getCurrentSTISMessageLibraryVersion()");
            FUNCTION_EXIT;
            return std::stoi(STISLibraryClient::instance().current_message_library_version());
        }

        // The "CDB" library version getters return the version actually
        // present in the local CCT-files directory on the agent host (i.e.
        // the version distributed from the Central DataBase). They are
        // served from TisLibraryCache so that the manager (Windows side)
        // no longer needs filesystem access or a train-library-gateway
        // dependency.
        virtual CORBA::UShort getCurrentCDBTTISMessageLibraryVersion() override
        {
            FUNCTION_ENTRY("getCurrentCDBTTISMessageLibraryVersion()");
            auto version = TisLibraryCache::instance().current_cld_version();
            FUNCTION_EXIT;
            return version;
        }

        virtual CORBA::UShort getNextCDBTTISMessageLibraryVersion() override
        {
            FUNCTION_ENTRY("getNextCDBTTISMessageLibraryVersion()");
            auto version = TisLibraryCache::instance().next_cld_version();
            FUNCTION_EXIT;
            return version;
        }

        virtual CORBA::UShort getCurrentCDBTPALibraryVersion() override
        {
            FUNCTION_ENTRY("getCurrentCDBTPALibraryVersion()");
            auto version = TisLibraryCache::instance().current_tpa_version();
            FUNCTION_EXIT;
            return version;
        }

        virtual CORBA::UShort getNextCDBTPALibraryVersion() override
        {
            FUNCTION_ENTRY("getNextCDBTPALibraryVersion()");
            auto version = TisLibraryCache::instance().next_tpa_version();
            FUNCTION_EXIT;
            return version;
        }

        virtual CORBA::Boolean isTrainLibrarySynchronisationComplete() override
        {
            FUNCTION_ENTRY("isTrainLibrarySynchronisationComplete()");
            auto complete = TisLibraryCache::instance().train_library_synchronisation_complete();
            FUNCTION_EXIT;
            return complete;
        }

        // Serve the raw CLD library XML blob cached by TisLibraryCache.
        // Managers (e.g. TrainBorne_PIDS::TTISPredefinedMessages) use this
        // instead of going through the legacy train-library-gateway. The
        // station-library branch is left as the inherited null stub.
        virtual MessageLibContent* getLibrary(EMessageLibraryType type,
                                              const char* /*filename*/,
                                              CORBA::UShort /*newVersion*/) override
        {
            FUNCTION_ENTRY("getLibrary()");
            auto* result = new MessageLibContent{};
            if (type == TRAIN_LIBRARY)
            {
                auto blob = TisLibraryCache::instance().current_cld_blob();
                result->length(static_cast<CORBA::ULong>(blob.size()));
                for (CORBA::ULong i = 0; i < result->length(); ++i)
                {
                    (*result)[i] = static_cast<CORBA::Octet>(blob[i]);
                }
                LOG_INFO("LegacySTISManager::getLibrary TRAIN_LIBRARY size=%u",
                         static_cast<unsigned>(result->length()));
            }
            FUNCTION_EXIT;
            return result;
        }

        CorbaName m_corba_name;
        STISMessageClient m_message_client{"--server=local-tis-gateway"};
    };

    LegacySTISManager& LegacySTISManager::instance()
    {
        static auto s_instance = new LegacySTISManager{};
        return *s_instance;
    }

    LegacySTISManager::LegacySTISManager()
        : m_impl(std::make_shared<Impl>())
    {
    }

    void LegacySTISManager::start()
    {
        m_impl->start();
    }

    void LegacySTISManager::stop()
    {
        m_impl->stop();
    }
}
