/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 *
 * Source:    $File: //depot/TYGL_CMFT_TIP/J155/transactive/app/signs/tis_agent/src/TISAgent.cpp $
 * @author:   Robin Ashcroft
 * @version:  $Revision: #6 $
 *
 * Last modification: $DateTime: 2026/05/21 16:17:09 $
 * Last modified by:  $Author: anusuya $
 *
 */

#include "pch.h"
#include "TISAgent.h"
#include "LegacySTISManager.h"
#include "StisServerServant.h"
#include "AlarmAuditMessageHandler.h"
#include "TisLibraryCache.h"
#include "app/signs/tis_agent/src/stis_protocol/STISLibraryAgentServer.h"
#include "app/signs/tis_agent/src/stis_protocol/STISStatusServer.h"
#include "app/signs/tis_agent/src/stis_protocol/STISMessageServer.h"
#include "app/signs/common_library/src/stis_protocol/CommonDefs.h"
#include "core/data_access_interface/entity_access/src/TISAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/STISEntityData.h"
#include "core/utility/src/base_ex/LocationAccessFactoryEx.h"
#include "core/utility/src/base_ex/EntityAccessFactoryEx.h"
#include "core/utility/src/base_ex/DummyGenericAgentUser.h"
#include "core/utility/src/base_ex/RunParamsEx.h"
#include "core/utility/src/base_ex/DebugUtilEx.h"
#include "core/utility/src/base_ex/DataPointUtil.h"
#include "core/utility/src/core/algorithm/file_system.h"
#include "core/utility/src/core/algorithm/strings.h"
#include <boost/thread/future.hpp>

namespace TA_IRS_App::tisagent::detail
{
    using boost::filesystem::path;
    using namespace TA_Base_Ex;
    using namespace TA_Base_Core;
    using namespace TA_Base_Bus;
    using namespace STIS_PROTOCOL::INTERFACES;
    using TA_Base_Ex::RunParamsEx;
    using TA_Base_Ex::DataPointUtil;
    using TA_Base_Ex::IsEntity;

    struct TISAgent::Impl : DummyGenericAgentUser
    {
        Impl(int argc, char* argv[])
            : DummyGenericAgentUser(argc, argv)
        {
        }

        IEntity* createEntity(IEntityDataPtr entity) override
        {
            LOG_CALLSTACK("TISAgent::createEntity");
            FUNCTION_ENTRY("createEntity");

            if (!m_library_state_update)
            {
                m_library_state_update = std::make_shared<STISLibraryAgentServer>(st2::format("--root-dir=%s --server=local-tis-agent --sync-all", m_root.string()));
            }

            try
            {
#if 0

                if (entity->getType() == TISAgentEntityData::getStaticType())
                {
                    LOG_INFO("createEntity(): Type is TISAgentEntityData");
                    auto agent = boost::dynamic_pointer_cast<TISAgentEntityData>(entity);
                    STISStatusServer::instance().parse_options(str(boost::format("--status-sync-interval-seconds=%d")
                                                                   % RunParamsEx::get_or("status-sync-interval-seconds", agent->getStatusPollInterval())));
                }

#else

                if (IsEntity<STISEntityData>(entity))
                {
                    LOG_INFO("createEntity(): Type is STISEntityData");
                    auto stis = boost::dynamic_pointer_cast<STISEntityData>(entity);

                    // TODO: add entity parameter value timeout
                    STISLibraryAgentServer::instance().parse_options(str(boost::format("--root-dir=%s --library-upgrade-timeout-seconds=%d")
                                                                         % m_root.string()
                                                                         % RunParamsEx::get_or("library-upgrade-timeout-seconds", 10)));

                    STISStatusServer::instance().parse_options(str(boost::format("--root-dir=%s --status-sync-interval-seconds=%d")
                                                                   % m_root.string()
                                                                   % RunParamsEx::get_or("status-sync-interval-seconds", 3)));
                }

#endif
            }
            catch (...)
            {
                LOG_ERROR("createEntity(): Exception caught while creating entity data objects ");
                throw;
            }

            FUNCTION_EXIT;
            return nullptr;
        }

        void agentSetControl() override
        {
            LOG_CALLSTACK("TISAgent::agentSetControl");
            DataPointUtil::instance().set_control();

            if (!m_library_state_update)
            {
                m_library_state_update = std::make_shared<STISLibraryAgentServer>(
                    st2::format("--root-dir=%s --server=local-tis-agent --sync-all", m_root.string()));
            }

            m_library_state_update->stop();
            STISLibraryAgentServer::instance().start();
            STISStatusServer::instance().start();
            STISMessageServer::instance().start();
            LegacySTISManager::instance().start();
            StisServerServant::instance().start();
            AlarmAuditMessageHandler::instance().start();

            // Pre-load CLD/TPA versions from the local filesystem so that the
            // CORBA getters (e.g. getCurrentCDBTTISMessageLibraryVersion)
            // have data to return as soon as the managers connect.
            TisLibraryCache::instance().refresh();
        }

        void agentSetMonitor() override
        {
            LOG_CALLSTACK("TISAgent::agentSetMonitor");
            STISLibraryAgentServer::instance().stop();
            STISStatusServer::instance().stop();
            STISMessageServer::instance().stop();
            LegacySTISManager::instance().stop();
            StisServerServant::instance().stop();
            AlarmAuditMessageHandler::instance().stop();

            if (!m_library_state_update)
            {
                m_library_state_update = std::make_shared<STISLibraryAgentServer>(
                    st2::format("--root-dir=%s --server=local-tis-agent --sync-all", m_root.string()));
            }

            m_library_state_update->start();
            DataPointUtil::instance().set_monitor();

            // Keep the cache populated in monitor mode too, so a fail-over
            // does not present a window where versions read as 0.
            TisLibraryCache::instance().refresh();
        }

        virtual void agentTerminate() override
        {
            LOG_CALLSTACK("TISAgent::agentTerminate");
            ::_exit(0);

            boost::async([&]
            {
                LOG_CALLSTACK("TISAgent::agentTerminate");
                STISLibraryAgentServer::instance().stop();
                STISStatusServer::instance().stop();
                STISMessageServer::instance().stop();
                LegacySTISManager::instance().stop();
                StisServerServant::instance().stop();
                AlarmAuditMessageHandler::instance().stop();

                if (!m_library_state_update)
                {
                    m_library_state_update = std::make_shared<STISLibraryAgentServer>(
                        st2::format("--root-dir=%s --server=local-tis-agent --sync-all", m_root.string()));
                }

                m_library_state_update->stop();
                DataPointUtil::instance().set_monitor().stop();
                m_generic_agent.reset();
            });
        }

        path m_root = stdex2::absoluted_copy(RunParamsEx::expand(RunParamsEx::get_or(RPARAM_STISROOTDIR, DEFAULT_STIS_ROOT_DIR)));
        STISLibraryAgentServerPtr m_library_state_update;
    };

    TISAgent::TISAgent(int argc, char* argv[])
        : m_impl(std::make_shared<Impl>(argc, argv))
    {
    }

    void TISAgent::run()
    {
        LOG_CALLSTACK("TISAgent::run");
        m_impl->run();
    }
}
