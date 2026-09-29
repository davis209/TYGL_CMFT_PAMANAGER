/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 *
 * Source:    $File: //depot/TYGL_CMFT_TIP/J155/transactive/app/signs/tis_gateway/src/TISGateway.cpp $
 * @author:   Robin Ashcroft
 * @version:  $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 */

#include "pch.h"
#include "TISGateway.h"
#include "app/signs/tis_gateway/src/stis_protocol/STISServer.h"
#include "app/signs/tis_gateway/src/stis_protocol/STISLibraryGatewayServer.h"
#include "app/signs/common_library/src/stis_protocol/CommonDefs.h"
#include "core/data_access_interface/entity_access/src/TISGatewayEntityData.h"
#include "core/data_access_interface/entity_access/src/STISGWEntityData.h"
#include "core/data_access_interface/entity_access/src/STISEntityData.h"
#include "core/utility/src/base_ex/GenericAgentUser.h"
#include "core/utility/src/base_ex/RunParamsEx.h"
#include "core/utility/src/base_ex/DebugUtilEx.h"
#include "core/utility/src/base_ex/EntityAccessFactoryEx.h"
#include "core/utility/src/base_ex/LocationAccessFactoryEx.h"
#include "core/utility/src/base_ex/DataPointUtil.h"
#include <boost/thread/future.hpp>

namespace TA_IRS_App::tisgateway::detail
{
    using TA_Base_Ex::RunParamsEx;
    using namespace TA_Base_Ex;
    using namespace TA_Base_Core;
    using namespace TA_Base_Bus;
    using namespace STIS_PROTOCOL::INTERFACES;

    struct TISGateway::Impl : GenericAgentUser
    {
        Impl(int argc, char* argv[])
            : GenericAgentUser(argc, argv)
        {
        }

        virtual bool createAllEntities(IEntityDataPtr, const SharedIEntityDataList&, IEntityList&) override
        {
            LOG_CALLSTACK("TISGateway::createAllEntities");

            try
            {
                if (auto entities = EntityEx::get_entities_of_type_at_location<STISGWEntityData>(ThisLocation::key()); entities.size())
                {
                    auto gateway = boost::dynamic_pointer_cast<STISGWEntityData>(entities[0]);

                    LOG_INFO("createAllEntities(): entity-key=%d, entity-type=%s", gateway->getKey(), gateway->getStaticType());

                    STISServer::instance().parse_options(str(boost::format("--ip=%s --port=%d --response-timeout-ms=%d --max-retries=%d")
                                                             % RunParamsEx::get_or("stis-ip", gateway->getServerIPAddress())
                                                             % RunParamsEx::get_or("stis-port", gateway->getServerPort())
                                                             % RunParamsEx::get_or("stis-message-timeout-ms", gateway->getMessageTimeout())
                                                             % RunParamsEx::get_or("stis-message-retries", gateway->getMessageRetries())));

                    STISLibraryGatewayServer::instance().parse_options(str(boost::format("--ip=%s --port=%d --username=%s --password=%s --root-dir=%s --max-threads=%d --debug-sftp=%s --debug-curl=%s")
                                                                           % RunParamsEx::get_or("stis-sftp-ip", gateway->getStisSftpIPAddress())
                                                                           % RunParamsEx::get_or("stis-sftp-port", gateway->getStisSftpPort())
                                                                           % RunParamsEx::get_or("stis-sftp-username", gateway->getStisSftpUserName())
                                                                           % RunParamsEx::get_or("stis-sftp-password", gateway->getStisSftpPassword())
                                                                           % RunParamsEx::get_or("stis-sftp-root-dir", ".")
                                                                           % RunParamsEx::get_or("stis-sftp-max-threads", 32)
                                                                           % RunParamsEx::get_or("debug-sftp", "true")
                                                                           % RunParamsEx::get_or("debug-curl", "false")));
                }
            }
            catch (...)
            {
                LOG_ERROR("createAllEntities(): Exception caught while creating entity data objects");
                throw;
            }

            return true;
        }

        void agentSetControl() override
        {
            LOG_CALLSTACK("TISGateway::agentSetControl");
            DataPointUtil::instance().set_control();
            STISServer::instance().start();
            STISLibraryGatewayServer::instance().start();
        }

        void agentSetMonitor() override
        {
            LOG_CALLSTACK("TISGateway::agentSetMonitor");
            STISServer::instance().stop();
            STISLibraryGatewayServer::instance().stop();
            DataPointUtil::instance().set_monitor();
        }

        virtual void agentTerminate() override
        {
            LOG_CALLSTACK("TISGateway::agentTerminate");
            ::_exit(0);

            boost::async([&]
            {
                LOG_CALLSTACK("TISGateway::agentTerminate");
                STISServer::instance().stop();
                STISLibraryGatewayServer::instance().stop();
                DataPointUtil::instance().set_monitor().stop();
                m_generic_agent.reset();
            });
        }
    };

    TISGateway::TISGateway(int argc, char* argv[])
        : m_impl(std::make_shared<Impl>(argc, argv))
    {
    }

    void TISGateway::run()
    {
        LOG_CALLSTACK("TISGateway::run");
        m_impl->run();
    }
}
