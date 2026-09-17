/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Ripple
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */

#include "pch.h"
#include "PAAgent.h"
#include "FireAbortMonitor.h"
#include "bus/generic_agent/src/GenericAgent.h"
#include "core/utility/src/base_ex/DataPointUtil.h"
#include "core/utility/src/base_ex/LocationAccessFactoryEx.h"
#include "core/utilities/src/CallstackLogger.h"

using TA_Base_Bus::IEntity;
using namespace TA_Base_Core;
using TA_Base_Ex::DataPointUtil;
using TA_Base_Ex::Location;
using TA_Base_Ex::ThisLocation;

namespace TA_IRS_App
{
    //
    // Constructor
    //
    PAAgent::PAAgent(int argc, char* argv[])
        : m_genericAgent(NULL),
        m_operationMode(TA_Base_Core::NotApplicable)
    {
        FUNCTION_ENTRY("Constructor");
        LOG_CALLSTACK("PAAgent");
        m_genericAgent = new TA_Base_Bus::GenericAgent(argc, argv, this);

		TA_IRS_App::SchedulerConfig config;
		TA_IRS_App::PaDeviceConfig paDevice;
		paDevice.host = TA_Base_Core::RunParams::getInstance().get("--pa-host");
		std::string paPort = TA_Base_Core::RunParams::getInstance().get("--pa-port");
		if (!paPort.empty())
		{
			paDevice.port = static_cast<unsigned short>(std::stoul(paPort));
		}
		std::string pollIntervalSeconds = TA_Base_Core::RunParams::getInstance().get("--poll-seconds");
		if (!pollIntervalSeconds.empty())
		{
			config.pollIntervalSeconds = static_cast<unsigned int>(std::stoul(pollIntervalSeconds));
		}
		auto executor = std::make_shared<TA_IRS_App::M44BroadcastExecutor>(paDevice);
		m_schedulerService = new TA_IRS_App::BroadcastSchedulerService(config, executor);

        FUNCTION_EXIT;
    }

    //
    // Destructor
    //
    PAAgent::~PAAgent()
    {
        FUNCTION_ENTRY("Destructor");
        FUNCTION_EXIT;
    }

    bool PAAgent::createAllEntities(TA_Base_Core::IEntityDataPtr agentEntityData,
                                    const TA_Base_Core::SharedIEntityDataList& entityDataList,
                                    TA_Base_Bus::IEntityList& createdEntities)
    {
        // return true to indicate not to call createEntity
        return true;
    }

    void PAAgent::entityCreationComplete()
    {
    }

    //
    // createEntity
    //
    IEntity* PAAgent::createEntity(TA_Base_Core::IEntityDataPtr EntityData)
    {
        FUNCTION_ENTRY("createEntity");

        IEntity* theEntity = 0;
        FUNCTION_EXIT;
        return theEntity;
    }

    //
    // agentTerminate
    //
    void PAAgent::agentTerminate()
    {
        FUNCTION_ENTRY("agentTerminate");
        LOG_CALLSTACK("PAAgent::agentTerminate");
		m_schedulerService->Stop();
        DataPointUtil::instance().set_monitor();
        FUNCTION_EXIT;
    }

    //
    // agentSetMonitor
    //
    void PAAgent::agentSetMonitor()
    {
        FUNCTION_ENTRY("agentSetMonitor");
        LOG_CALLSTACK("PAAgent::agentSetMonitor");

        DataPointUtil::instance().set_monitor();
        //FireAbortMonitor::instance().stop();
		m_schedulerService->Stop();
        FUNCTION_EXIT;
    }

    //
    // agentSetControl
    //
    void PAAgent::agentSetControl()
    {
        FUNCTION_ENTRY("agentSetControl");
        LOG_CALLSTACK("PAAgent::agentSetControl");

        if (m_operationMode == TA_Base_Core::Control)
        {
            FUNCTION_EXIT;
            return;
        }

        m_operationMode = TA_Base_Core::Control;
        DataPointUtil::instance().set_control();

        //FireAbortMonitor::instance().start();
		m_schedulerService->Start();
        FUNCTION_EXIT;
    }

    //
    // notifyGroupOffline
    //
    void PAAgent::notifyGroupOffline(const std::string& group)
    {
        FUNCTION_ENTRY("notifyGroupOffline");
        FUNCTION_EXIT;
    }

    //
    // notifyGroupOnline
    //
    void PAAgent::notifyGroupOnline(const std::string& group)
    {
        FUNCTION_ENTRY("notifyGroupOnline");
        FUNCTION_EXIT;
    }

    //
    // registerForStateUpdates
    //
    void PAAgent::registerForStateUpdates()
    {
        FUNCTION_ENTRY("registerForStateUpdates");
        FUNCTION_EXIT;
    }

    //
    // processOnlineUpdate
    //
    void PAAgent::processOnlineUpdate(const ConfigUpdateDetails& updateEvent)
    {
        FUNCTION_ENTRY("processOnlineUpdate");
        FUNCTION_EXIT;
    }

    //
    // run
    //
    void PAAgent::run()
    {
        FUNCTION_ENTRY("run");
        LOG_CALLSTACK("PAAgent::run");
        TA_ASSERT(m_genericAgent, "m_genericAgent is NULL");

        // check the runtime parameter OperationMode
        auto operationMode = RunParams::getInstance().get("OperationMode");

        // if defined as "Control"
        if (RPARAM_CONTROL == operationMode)
        {
            agentSetControl();
        }
        else if (RPARAM_MONITOR == operationMode)
        {
            agentSetMonitor();
        }

        m_genericAgent->run();
        FUNCTION_EXIT;
    }

    void PAAgent::receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message)
    {
        FUNCTION_ENTRY("receiveSpecialisedMessage");
        FUNCTION_EXIT;
    }
}
