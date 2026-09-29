/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/stis_manager/src/RightsManager.cpp $
 * @author:  Adam Radics
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 *
 * A singleton used to check rights.
 * It posts out windows messages to a window when it should re-check rights.
 * It is also used to handle server online and server offline - by denying
 * some actions.
 *
 * STATION_TIS_FREE_TEXT
 * STATION_TIS_ISCS_LIBRARY_UPGRADE
 * STATION_TIS_PID_GROUPS
 * STATION_TIS_PRE_DEFINED
 * STATION_TIS_RESET
 */

#include "stdafx.h"
#include "core/types/src/ta_types.h"
#include "RightsManager.h"
#include "resource.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/RightsException.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"

#include "core/data_access_interface/entity_access/src/TrainAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/src/ResourceAccessFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include <algorithm>

using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;
using TA_Base_Core::RightsException;

using TA_Base_Core::EntityAccessFactory;
using TA_Base_Core::IEntityData;
using TA_Base_Core::IEntityDataList;
using TA_Base_Core::DataException;

namespace TA_IRS_App
{
    RightsManager* RightsManager::m_theClassInstance;
    TA_Base_Core::NonReEntrantThreadLockable RightsManager::m_singletonLock;

    RightsManager& RightsManager::getInstance()
    {
        TA_THREADGUARD(m_singletonLock);

        // If guard acquired then make sure the singleton is still NULL.

        if (m_theClassInstance == NULL)
        {
            // Create the one & only object
            m_theClassInstance = new RightsManager();
        }

        return *m_theClassInstance;
    }

    RightsManager::RightsManager()
        : m_windowsToNotify(),
        m_serverIsOnline(true),
        m_rightsLibrary(NULL)
    {
        // build the rights library
        TA_Base_Bus::RightsLibraryFactory factory;
        m_rightsLibrary = factory.buildRightsLibrary(TA_Base_Bus::DEFAULT_RL, TA_Base_Bus::RightsLibraryFactory::MANAGER);

        // register for session id changes
        RunParams::getInstance().registerRunParamUser(this, RPARAM_SESSIONID);
    }

    RightsManager::~RightsManager()
    {
        RunParams::getInstance().deregisterRunParamUser(this);

        if (m_rightsLibrary != NULL)
        {
            delete m_rightsLibrary;
            m_rightsLibrary = NULL;
        }
    }

    void RightsManager::registerForRightsChanges(CWnd* window)
    {
        if (window != NULL)
        {
            TA_THREADGUARD(m_windowListLock);

            // add it to the map
            m_windowsToNotify.push_back(window);
        }
    }

    void RightsManager::deregisterForRightsChanges(CWnd* window)
    {
        TA_THREADGUARD(m_windowListLock);

        // find the map entry
        std::vector<CWnd*>::iterator findIter = std::find(m_windowsToNotify.begin(),
                                                          m_windowsToNotify.end(),
                                                          window);

        // if found, delete it
        if (findIter != m_windowsToNotify.end())
        {
            m_windowsToNotify.erase(findIter);
        }
    }

    void RightsManager::setServerStatus(bool online)
    {
        if (online != m_serverIsOnline)
        {
            m_serverIsOnline = online;

            // update what the user can do
            TA_THREADGUARD(m_windowListLock);

            postMessageToWindows(m_windowsToNotify, WM_UPDATE_RIGHTS);
        }
    }

    void RightsManager::onRunParamChange(const std::string& name, const std::string& value)
    {
        FUNCTION_ENTRY("onRunParamChange");

        // if the session id changes, rights have changed
        if (0 == name.compare(RPARAM_SESSIONID))
        {
            TA_THREADGUARD(m_windowListLock);

            postMessageToWindows(m_windowsToNotify, WM_UPDATE_RIGHTS);
        }

        FUNCTION_EXIT;
    }

    void RightsManager::postMessageToWindows(const std::vector<CWnd*>& windows, int message)
    {
        for (std::vector<CWnd*>::const_iterator iter = windows.begin();
             iter != windows.end(); iter++)
        {
            // if the window is not null and is a window
            if (*iter != NULL)
            {
                (*iter)->PostMessage(message);
            }
        }
    }

    bool RightsManager::checkPermission(std::uint32_t actionKey, std::uint32_t resourceKey)
    {
        TA_ASSERT(m_rightsLibrary != NULL, "The rights library is null");

        // if the server is not online, then nothing will work anyway
        // so disallow it
        if (!m_serverIsOnline)
        {
            return false;
        }

        bool allowed = false;

        try
        {
            std::string reasonString;
            TA_Base_Bus::EDecisionModule decisionModule;

            std::string sessionId = RunParams::getInstance().get(RPARAM_SESSIONID);

            allowed = m_rightsLibrary->isActionPermittedOnResource(
                sessionId, resourceKey, actionKey, reasonString, decisionModule);
        }
        catch (RightsException& re)
        {
            LOG_EXCEPTION("RightsException", re.what());
        }

        return allowed;
    }

    std::uint32_t RightsManager::getTisAgentResourceKey()
    {
        // if its already been loaded
        if (m_stismanagerResourceKey.hasBeenSet())
        {
            return m_stismanagerResourceKey.getValue();
        }

        //TD 16288
        IEntityDataList stismanagerEntity;
        //TD 16288
        TA_Base_Core::IResource* resource = NULL;

        try
        {
            stismanagerEntity = EntityAccessFactory::getInstance().getEntitiesOfType("STISManager", false, true, true);

            // no TIS agent
            if (stismanagerEntity.size() < 1)
            {
                TA_THROW(DataException("No STISManager", DataException::NO_VALUE, ""));
            }
            // multiple TIS agents
            else if (stismanagerEntity.size() > 1)
            {
                TA_THROW(DataException("Multiple STISManager", DataException::NOT_UNIQUE, ""));
            }

            resource = TA_Base_Core::ResourceAccessFactory::getInstance().getResourceFromEntity(stismanagerEntity[0]->getKey());
            m_stismanagerResourceKey.setValue(resource->getKey());

            // clean up
            for (IEntityDataList::iterator iter = stismanagerEntity.begin();
                 iter != stismanagerEntity.end(); iter++)
            {
                delete (*iter);
            }

            stismanagerEntity.clear();

            if (resource != NULL)
            {
                delete resource;
                resource = NULL;
            }

            return m_stismanagerResourceKey.getValue();
        }
        catch (...)
        {
            // clean up
            for (IEntityDataList::iterator iter = stismanagerEntity.begin();
                 iter != stismanagerEntity.end(); iter++)
            {
                delete (*iter);
            }

            stismanagerEntity.clear();

            if (resource != NULL)
            {
                delete resource;
                resource = NULL;
            }

            throw;
        }

        //TD 16288
    }

    bool RightsManager::canDisplayFreeText()
    {
        FUNCTION_ENTRY("canDisplayFreeText");

        std::uint32_t resourceKey = 0;

        try
        {
            resourceKey = getTisAgentResourceKey();
        }
        catch (...)
        {
            // cant find an agent to use
            // so the function wont be available.
            LOG_EXCEPTION("...", "Error getting resource key. Functionality will be disallowed.");

            return false;
        }

        FUNCTION_EXIT;
        return checkPermission(TA_Base_Bus::aca_STATION_TIS_FREE_TEXT, resourceKey);
    }

    bool RightsManager::canDisplayPredefined()
    {
        FUNCTION_ENTRY("canDisplayPredefined");

        std::uint32_t resourceKey = 0;

        try
        {
            resourceKey = getTisAgentResourceKey();
        }
        catch (...)
        {
            // cant find an agent to use
            // so the function wont be available.
            LOG_EXCEPTION("...", "Error getting resource key. Functionality will be disallowed.");

            return false;
        }

        FUNCTION_EXIT;
        return checkPermission(TA_Base_Bus::aca_STATION_TIS_PRE_DEFINED, resourceKey);
    }

    bool RightsManager::canDisplayTemplate()
    {
        // TODO
        return true;
    }

    bool RightsManager::canClear()
    {
        FUNCTION_ENTRY("canClear");

        std::uint32_t resourceKey = 0;

        try
        {
            resourceKey = getTisAgentResourceKey();
        }
        catch (...)
        {
            // cant find an agent to use
            // so the function wont be available.
            LOG_EXCEPTION("...", "Error getting resource key. Functionality will be disallowed.");

            return false;
        }

        FUNCTION_EXIT;
        return checkPermission(TA_Base_Bus::aca_STATION_TIS_RESET, resourceKey);
    }

    bool RightsManager::canUpgradeISCS()
    {
        FUNCTION_ENTRY("canUpgradeISCS");

        std::uint32_t resourceKey = 0;

        try
        {
            resourceKey = getTisAgentResourceKey();
        }
        catch (...)
        {
            // cant find an agent to use
            // so the function wont be available.
            LOG_EXCEPTION("...", "Error getting resource key. Functionality will be disallowed.");

            return false;
        }

        FUNCTION_EXIT;
        return checkPermission(TA_Base_Bus::aca_STATION_TIS_ISCS_LIBRARY_UPGRADE, resourceKey);
    }

    bool RightsManager::canModifyPIDGroups()
    {
        FUNCTION_ENTRY("canModifyPIDGroups");

        std::uint32_t resourceKey = 0;

        try
        {
            resourceKey = getTisAgentResourceKey();
        }
        catch (...)
        {
            // cant find an agent to use
            // so the function wont be available.
            LOG_EXCEPTION("...", "Error getting resource key. Functionality will be disallowed.");

            return false;
        }

        FUNCTION_EXIT;
        return checkPermission(TA_Base_Bus::aca_STATION_TIS_PID_GROUPS, resourceKey);
    }

    //TD17901 AVteam++
    bool RightsManager::canPIDControl()
    {
        FUNCTION_ENTRY("canPIDControl");

        std::uint32_t resourceKey = 0;

        try
        {
            resourceKey = getTisAgentResourceKey();
        }
        catch (...)
        {
            //cnat find an agent to use
            //so the function wont be available.
            LOG_EXCEPTION("...", "Error getting resource key. Functionality will be disallowed.");
            return false;
        }

        FUNCTION_EXIT;
        return checkPermission(TA_Base_Bus::aca_STATION_TIS_PID_CONTROL, resourceKey);
    }
    //TD17901 ++AVteam
} // end namespace TA_IRS_App
