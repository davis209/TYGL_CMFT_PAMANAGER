/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/stis_manager/src/RightsManager.h $
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

#if !defined(RIGHTSMANAGER_H)
#define RIGHTSMANAGER_H

#if _MSC_VER > 1000
    #pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"

#define WM_UPDATE_RIGHTS  (WM_USER + 115)

#include "core/types/src/ta_types.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/RunParams.h"

#include "bus/security/rights_library/src/RightsLibrary.h"

#include "core/data_access_interface/src/PrimitiveWrapper.h"

namespace TA_IRS_App
{
    class RightsManager : public virtual TA_Base_Core::RunParamUser
    {
    public:

        /**
         * ~RightsManager
         *
         * Standard destructor.
         */

        virtual ~RightsManager();

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return      RightsManager&
         *              A reference to an instance of a RightsManager object.
         */
        static RightsManager& getInstance();

        /**
         * registerForRightsChanges
         *
         * When rights change, window will be given a
         * WM_UPDATE_RIGHTS message. It should re-check its rights.
         *
         * @param window       The window to be notified of change.
         */
        void registerForRightsChanges(CWnd* window);

        /**
         * deregisterForRightsChanges
         *
         * window is no longer interested in being notified of rights changes.
         *
         * @param window       The window to be removed
         */
        void deregisterForRightsChanges(CWnd* window);

        /**
         * canDisplayFreeText
         *
         * If true, the operator can display a free text message.
         * STATION_TIS_FREE_TEXT
         */
        bool canDisplayFreeText();

        /**
         * canDisplayPredefined
         *
         * If true, the operator can display a predefined message.
         * STATION_TIS_PRE_DEFINED
         */
        bool canDisplayPredefined();

        /**
         * canDisplayPredefined
         *
         * If true, the operator can display a predefined message.
         * STATION_TIS_PRE_DEFINED
         */
        bool canDisplayTemplate();

        /**
         * canClear
         *
         * If true, the operator can send a clear request.
         * STATION_TIS_RESET
         */
        bool canClear();

        /**
         * canUpgradeISCS
         *
         * If true, the operator can upgrade the predefined message library.
         * STATION_TIS_ISCS_LIBRARY_UPGRADE
         */
        bool canUpgradeISCS();

        /**
         * canModifyTrainGroups
         *
         * If true the operator can create/edit/delete
         * PID groups.
         * STATION_TIS_PID_GROUPS
         */
        bool canModifyPIDGroups();

        //TD17901 AVteam++
        /************************************************************************/
        /* If true, the operator can do PID control
        /* STATION_TIS_PID_CONTROL
        /************************************************************************/
        bool canPIDControl();
        //TD17901 ++AVteam

        /**
         * onRunParamChange
         *
         * This tells the application when a RunParam has changed. This should check the
         * name to see that the parameter that has changed is the SESSIONID.
         *
         * @param string& name -  The name of the changed parameter
         * @param string& value - The new value for the parameter
         */
        void onRunParamChange(const std::string& name, const std::string& value);

        /**
         * setServerStatus
         *
         * This is used to handle server online
         * and server offline messages.
         *
         * @param online whether the server is online or not.
         */
        void setServerStatus(bool online);

    private:

        /**
         * RightsManager
         *
         * Private constructors.
         */
        RightsManager();
        RightsManager& operator=(const RightsManager&) {};
        RightsManager(const RightsManager&) {};

        /**
         * checkPermission
         *
         * Checks the permission for the given action on the given resource
         *
         * @param actionKey
         * @param resourceKey
         *
         * @return whether the action is permitted or not
         */
        bool checkPermission(std::uint32_t actionKey, std::uint32_t resourceKey);

        /**
         * postMessageToWindows
         *
         * post the given message to the given vector of windows
         *
         * @param windows  A vector of windows to post the message to
         * @param message  The message number to post
         *
         */
        void postMessageToWindows(const std::vector<CWnd*>& windows, int message);

        /**
         * getTisAgentResourceKey
         *
         * @return the resource key for the local tis agent.
         */
        std::uint32_t getTisAgentResourceKey();

        // class instance
        static RightsManager* m_theClassInstance;

        // Thread lock to protect singleton creation.
        static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;

        // threadlock for registration/deregistration
        TA_Base_Core::NonReEntrantThreadLockable m_windowListLock;

        // the vector of windows
        std::vector<CWnd*> m_windowsToNotify;

        // whether this GUI will actually be able to contact any agents or not
        bool m_serverIsOnline;

        // the rights library to use
        TA_Base_Bus::RightsLibrary* m_rightsLibrary;

        // the agent resource key
        TA_Base_Core::PrimitiveWrapper< std::uint32_t > m_stismanagerResourceKey;
    };

} // end namespace TA_IRS_App

#endif
