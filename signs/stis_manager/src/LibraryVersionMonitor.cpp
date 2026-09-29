/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/J155/transactive/app/signs/stis_manager/src/LibraryVersionMonitor.cpp $
 * @author:  Rob Ashcroft
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 *
 *
 */

#include "stdafx.h"
#include "core/types/src/ta_types.h"
#include "UserMessages.h"
#include "LibraryVersionMonitor.h"
#include "TisAgentInterface.h"
#include "helperfun.h"
#include "STISPredefinedMessages.h"
#include "STISTemplates.h"
#include "bus/signs_4669/tis_agent_access/src/TISAgentAccessFactory.h"
#include "core/utility/src/base_ex/RunParamsEx.h"
#include "core/utility/src/base_ex/LocationAccessFactoryEx.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/CallstackLogger.h"
#include "core/utility/src/core/StaticObject.h"
#include "core/threads/src/Thread.h"
#include "core/exceptions/src/ObjectResolutionException.h"
#include "core/naming/src/NamingMacros.h"
#include "core/utility/src/core/MakeNameValuePairsString.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include <algorithm>
#include <chrono>

#define RPARAM_LIBRARYVERSIONUPDATEINTERVALMILLISECONDS "LibraryVersionUpdateIntervalMS"

using namespace std::chrono;
using namespace TA_Base_Core;
using TA_Base_Ex::RunParamsEx;
using TA_Base_Ex::ThisLocation;
using st::StaticObject;
using TA_Base_Bus::TISAgentAccessFactory;
//using TA_IRS_App::STIS_PROTOCOL::MESSAGE_TYPES::HELPERS::AllCurNxtMsgTmpLibVersInfoList;
//using TA_IRS_App::STIS_PROTOCOL::MESSAGE_TYPES::HELPERS::AllCurNxtMsgTmpLibVersInfo;
//using TA_IRS_App::STIS_PROTOCOL::MESSAGE_TYPES::HELPERS::AllCurNxtMsgTmpLibVers;
//using TA_IRS_App::STIS_PROTOCOL::MESSAGE_TYPES::HELPERS::CurNxtMsgTmpLibVers;

namespace TA_IRS_App
{
    using namespace STIS_PROTOCOL;

    using SimpleTimer = st::SimpleTimer;
    using SimpleTimerPtr = st::SimpleTimerPtr;
    using Timer = StaticObject<SimpleTimer, LibraryVersionMonitor>;

    const std::uint32_t LibraryVersionMonitor::ALLLOCATON_LOCATION_KEY = 0;
    const std::uint32_t LibraryVersionMonitor::OCC_LOCATION_KEY = 1;
    const std::uint32_t LibraryVersionMonitor::TDS_LOCATION_KEY = 2;
    const std::uint32_t LibraryVersionMonitor::THREAD_SLEEP_TIME = 1000;

    LibraryVersionMonitor& LibraryVersionMonitor::instance()
    {
        static auto s_instance = new LibraryVersionMonitor;
        return *s_instance;
    }

    LibraryVersionMonitor::LibraryVersionMonitor()
        : Thread("LibraryVersionMonitor"),
        m_windowToNotify(NULL),
        m_stisPredefinedMessages(STISPredefinedMessages::getInstance()),
        m_stisTemplates(STISTemplates::getInstance())
    {
        FUNCTION_ENTRY("LibraryVersionMonitor");
        //setupVersionInfo();
        FUNCTION_EXIT;
    }

    LibraryVersionMonitor::~LibraryVersionMonitor()
    {
        FUNCTION_ENTRY("~LibraryVersionMonitor");
        // stop scada factory

        //m_libraryVersionDatapoints.clear();

        FUNCTION_EXIT
    }

    void LibraryVersionMonitor::run()
    {
        FUNCTION_ENTRY("run");

        initalize();

        while (m_running)
        {
            if (m_isEnabled && m_isInitialized)
            {
                process();
            }

            m_running.wait_for(milliseconds(m_interval), [&] { return !m_running.value; });
        }

        FUNCTION_EXIT;
    }

    void LibraryVersionMonitor::change_sync_interval_for_a_while(size_t new_interval_ms, size_t duration_ms)
    {
        LOG_DEBUG("change_sync_interval_for_a_while(): %s", nvps(new_interval_ms, duration_ms));
        Timer::instance().submit_once(duration_ms, [=, backup = std::exchange(m_interval, new_interval_ms)]{m_interval = backup;});
        m_running.notify_all();
    }

    void LibraryVersionMonitor::terminate()
    {
        FUNCTION_ENTRY("terminate");
        m_isInitialized = false;
        m_isEnabled = false;
        m_running = false;
        FUNCTION_EXIT;
    }

    void LibraryVersionMonitor::initalize()
    {
        FUNCTION_ENTRY("initalize");

        if (!loadConfig())
        {
            LOG_WARN("=> failed to load configuration, use default config instead");
            useDefaultConfig();
        }

        m_isEnabled = true;
        m_isInitialized = true;
        m_running = true;
        FUNCTION_EXIT;
    }

    bool LibraryVersionMonitor::loadConfig()
    {
        FUNCTION_ENTRY("loadConfig");
        FUNCTION_EXIT;
        return false;
    }

    void LibraryVersionMonitor::useDefaultConfig()
    {
        FUNCTION_ENTRY("setDefaultConfig");
        m_interval = RunParamsEx::get_or(RPARAM_LIBRARYVERSIONUPDATEINTERVALMILLISECONDS, THREAD_SLEEP_TIME);
        FUNCTION_EXIT;
    }

    inline void LibraryVersionMonitor::process()
    {
        FUNCTION_ENTRY("process");
        updateAllLibraryVersions();
        FUNCTION_EXIT;
    }

    void LibraryVersionMonitor::registerForChanges(CWnd* window)
    {
        if (window)
        {
            TA_ASSERT(m_windowToNotify == nullptr, "A second window is attempting to register with the LibraryVersionMonitor");
            m_windowToNotify = window;
        }
    }

    void LibraryVersionMonitor::deregisterForChanges(CWnd* window)
    {
        m_windowToNotify = nullptr;
    }

    void LibraryVersionMonitor::notifyWindowsOfChange(std::uint32_t entityKey, unsigned short newValue)
    {
        if (m_windowToNotify != NULL)
        {
            if (::IsWindow(m_windowToNotify->m_hWnd) == false)
            {
                //It is not a valid window
                return;
            }

            if (newValue == 0)
            {
                m_windowToNotify->PostMessage(WM_UPDATE_MESSAGE_LIBRARY_VERSION);
            }
            else if (newValue == 1)
            {
                m_windowToNotify->PostMessage(WM_UPDATE_TEMPLATE_LIBRARY_VERSION);
            }
        }
        else
        {
            LOG_INFO("No windows are registered with the LibraryVersionMonitor to receive library version updates");
        }
    }

    // Get message Library metadata lists from TIS Agent:
    LocationLibraryVersionInfoListMap LibraryVersionMonitor::getAllMessageLibraryVersions()
    {
        TA_THREADGUARD(m_messageLock);
        return m_msgVersionInfoMap;
    }

    // Get message Library metadata lists from TIS Agent:
    LocationLibraryVersionInfoListMap LibraryVersionMonitor::getAllTemplateLibraryVersions()
    {
        TA_THREADGUARD(m_messageLock);
        return m_tmpVersionInfoMap;
    }

    bool LibraryVersionMonitor::updateAllLibraryVersions()
    {
        LOG_CALLSTACK("LibraryVersionMonitor::updateAllLibraryVersions");

        static AllCurNxtMsgTmpLibVersInfoList s_itemList;
        AllCurNxtMsgTmpLibVersInfoList itemList;
        TA_THREADGUARD(m_messageLock);

        // get message library metata list:
        try
        {
            itemList = STISClient::instance().get_all_station_iscs_stis_library_versions();
        }
        catch (TA_Base_Core::ObjectResolutionException& ex)
        {
            LOG_EXCEPTION("ObjectResolutionException", ex.what());
            return false;
        }
        catch (...)
        {
            LOG_ERROR("unable to retrieve message library metadata list from OCC TISAgent");
            return false;
        }

        if (itemList == s_itemList)
        {
            return false;
        }

        // Routine for change detection
        auto itemCount = itemList.size();
        LOG_DEBUG("itemCount: %d", itemCount);

        if (itemCount == 0)
        {
            return false;
        }

        bool isMsgItemChanged = false;
        bool isTmpItemChanged = false;

        for (int i = 0; i < itemCount; i++)
        {
            LocationLibraryVersionInfo newMsgVersion;
            newMsgVersion.locationKey = itemList[i].location_key;
            newMsgVersion.name = itemList[i].location_name;
            newMsgVersion.currentISCSLibraryVersionKey = stoi_ex(itemList[i].versions.iscs.current_message_library_version);
            newMsgVersion.currentSTISLibraryVersionKey = stoi_ex(itemList[i].versions.stis.current_message_library_version);
            newMsgVersion.nextISCSLibraryVersionKey = stoi_ex(itemList[i].versions.iscs.next_message_library_version);
            newMsgVersion.nextSTISLibraryVersionKey = stoi_ex(itemList[i].versions.stis.next_message_library_version);

            LocationLibraryVersionInfo newTmpVersion;
            newTmpVersion.locationKey = itemList[i].location_key;
            newTmpVersion.name = itemList[i].location_name;
            newTmpVersion.currentISCSLibraryVersionKey = stoi_ex(itemList[i].versions.iscs.current_template_library_version);
            newTmpVersion.currentSTISLibraryVersionKey = stoi_ex(itemList[i].versions.stis.current_template_library_version);
            newTmpVersion.nextISCSLibraryVersionKey = stoi_ex(itemList[i].versions.iscs.next_template_library_version);
            newTmpVersion.nextSTISLibraryVersionKey = stoi_ex(itemList[i].versions.stis.next_template_library_version);

            // only store OCC & all other stations:
            if (newMsgVersion.locationKey == ALLLOCATON_LOCATION_KEY || newMsgVersion.locationKey == TDS_LOCATION_KEY)
            {
                continue;
            }

            LOG_DEBUG("Message: location: %s(%d), version: %s", LocationEx::to_name(newMsgVersion.locationKey), newMsgVersion.locationKey, valuess(newMsgVersion.currentISCSLibraryVersionKey, newMsgVersion.currentSTISLibraryVersionKey, newMsgVersion.nextISCSLibraryVersionKey, newMsgVersion.nextSTISLibraryVersionKey));
            LOG_DEBUG("Template: location: %s(%d), version: %s", LocationEx::to_name(newTmpVersion.locationKey), newTmpVersion.locationKey, valuess(newTmpVersion.currentISCSLibraryVersionKey, newTmpVersion.currentSTISLibraryVersionKey, newTmpVersion.nextISCSLibraryVersionKey, newTmpVersion.nextSTISLibraryVersionKey));

            auto oldMsgVersion = m_msgVersionInfoMap[newMsgVersion.locationKey];

            if (m_msgVersionInfoMap.count(newMsgVersion.locationKey) > 0)           // if metadata exists?
            {
                if (m_msgVersionInfoMap[newMsgVersion.locationKey] != newMsgVersion)
                {
                    // change detection for OCC station only
                    if (!isMsgItemChanged) { isMsgItemChanged = true; }

                    if (newMsgVersion.locationKey == ThisLocation::key())
                    {
                        if (!m_isCurrMsgLibrVerChanged && oldMsgVersion.currentISCSLibraryVersionKey != newMsgVersion.currentISCSLibraryVersionKey)
                        {
                            m_isCurrMsgLibrVerChanged = true;
                        }

                        if (!m_isNextMsgLibrVerChanged && oldMsgVersion.nextISCSLibraryVersionKey != newMsgVersion.nextISCSLibraryVersionKey)
                        {
                            m_isNextMsgLibrVerChanged = true;
                        }
                    }
                }
            }

            m_msgVersionInfoMap[newMsgVersion.locationKey] = newMsgVersion;

            auto oldTmpVersion = m_tmpVersionInfoMap[newTmpVersion.locationKey];

            if (m_tmpVersionInfoMap.count(newTmpVersion.locationKey) > 0)           // if metadata exists?
            {
                if (m_tmpVersionInfoMap[newTmpVersion.locationKey] != newTmpVersion)
                {
                    // change detection for OCC station only
                    if (!isTmpItemChanged) { isTmpItemChanged = true; }

                    if (newTmpVersion.locationKey == ThisLocation::key())
                    {
                        if (!m_isCurrTmpLibrVerChanged && oldTmpVersion.currentISCSLibraryVersionKey != newTmpVersion.currentISCSLibraryVersionKey)
                        {
                            m_isCurrTmpLibrVerChanged = true;
                        }

                        if (!m_isNextTmpLibrVerChanged && oldTmpVersion.nextISCSLibraryVersionKey != newTmpVersion.nextISCSLibraryVersionKey)
                        {
                            m_isNextTmpLibrVerChanged = true;
                        }
                    }
                }
            }

            m_tmpVersionInfoMap[newTmpVersion.locationKey] = newTmpVersion;
        }

        LOG_DEBUG("isMsgItemChanged: %d", isMsgItemChanged);

        // check if is new library is synchronized over all stations:
        // condition #1: all currStis & nextStis are the same.
        // condition #2: all currStis & currIscs are NOT the same.
        if (isMsgItemChanged)
        {
            if (ThisLocation::is_occ())
            {
                LocationLibraryVersionInfo OccInfo = m_msgVersionInfoMap[OCC_LOCATION_KEY];

                unsigned short currStis = OccInfo.currentSTISLibraryVersionKey;
                unsigned short nextStis = OccInfo.nextSTISLibraryVersionKey;
                unsigned short currIscs = OccInfo.currentISCSLibraryVersionKey;

                // check if there is new Library synchronized?
                m_isMsgLibSynchronized = (currStis == nextStis && currStis != currIscs);
                LOG_DEBUG("m_isMsgLibSynchronized: %d", m_isMsgLibSynchronized);

                if (m_isMsgLibSynchronized)
                {
                    for (auto item : m_msgVersionInfoMap)
                    {
                        // Only process station & OCC:
                        if (item.second.locationKey == ALLLOCATON_LOCATION_KEY ||
                            item.second.locationKey == TDS_LOCATION_KEY ||
                            item.second.locationKey == OCC_LOCATION_KEY)
                        {
                            continue;
                        }

                        // considered out-of-sync if conditions are NOT fulfilled.
                        if (currStis != item.second.currentSTISLibraryVersionKey ||
                            nextStis != item.second.nextSTISLibraryVersionKey ||
                            currIscs != item.second.currentISCSLibraryVersionKey)
                        {
                            m_isMsgLibSynchronized = false;
                            break;
                        }
                    }
                }
            }

            // Update View:
            notifyMsgItemChanged();
        }

        if (isTmpItemChanged)
        {
            if (ThisLocation::is_occ())
            {
                LocationLibraryVersionInfo OccInfo = m_tmpVersionInfoMap[OCC_LOCATION_KEY];

                unsigned short currStis = OccInfo.currentSTISLibraryVersionKey;
                unsigned short nextStis = OccInfo.nextSTISLibraryVersionKey;
                unsigned short currIscs = OccInfo.currentISCSLibraryVersionKey;

                // check if there is new Library synchronized?
                m_isTmpLibSynchronized = (currStis == nextStis && currStis != currIscs);

                if (m_isTmpLibSynchronized)
                {
                    for (auto item : m_tmpVersionInfoMap)
                    {
                        // Only process station & OCC:
                        if (item.second.locationKey == ALLLOCATON_LOCATION_KEY ||
                            item.second.locationKey == TDS_LOCATION_KEY ||
                            item.second.locationKey == OCC_LOCATION_KEY)
                        {
                            continue;
                        }

                        // considered out-of-sync if conditions are NOT fulfilled.
                        if (currStis != item.second.currentSTISLibraryVersionKey ||
                            nextStis != item.second.nextSTISLibraryVersionKey ||
                            currIscs != item.second.currentISCSLibraryVersionKey)
                        {
                            m_isTmpLibSynchronized = false;
                            break;
                        }
                    }
                }
            }

            // Update View:
            notifyTmpItemChanged();
        }

        s_itemList = itemList;
        return isMsgItemChanged || isTmpItemChanged;
    }

    void LibraryVersionMonitor::notifyMsgItemChanged()
    {
        LOG_CALLSTACK("LibraryVersionMonitor::notifyMsgItemChanged");

        // onChange Event
        LOG_DEBUG("notifyWindowsOfChange");
        notifyWindowsOfChange(0, 0);

        // Assuming that the OCC Locationinfo is every-ready for this stage.
        // update current version and indirectly update view within each setter function(s)
        LocationLibraryVersionInfo currentInfo = m_msgVersionInfoMap[ThisLocation::key()];

        // fire onCurrentMessageLibraryVersionChange
        if (m_isCurrMsgLibrVerChanged)
        {
            m_isCurrMsgLibrVerChanged = false;
            m_stisPredefinedMessages->setCurrentMessageLibraryVersion(currentInfo.currentISCSLibraryVersionKey);
            // reload the libraries
            m_stisPredefinedMessages->loadSTISPredefinedMessages();
        }

        // fire onNextMessageLibraryVersionChange event
        if (m_isNextMsgLibrVerChanged)
        {
            m_isNextMsgLibrVerChanged = false;
            m_stisPredefinedMessages->setNextMessageLibraryVersion(currentInfo.nextISCSLibraryVersionKey);
        }

        // fire onnewLibrarySynchronizedChange event
        if (m_isMsgLibSynchronized)
        {
            m_isMsgLibSynchronized = false;
            m_stisPredefinedMessages->setMessageLibrarySynchronised(true);
        }
    }

    void LibraryVersionMonitor::notifyTmpItemChanged()
    {
        LOG_CALLSTACK("LibraryVersionMonitor::notifyTmpItemChanged");

        // onChange Event
        notifyWindowsOfChange(0, 1);

        // Assuming that the OCC Locationinfo is every-ready for this stage.
        // update current version and indirectly update view within each setter function(s)
        LocationLibraryVersionInfo currentInfo = m_tmpVersionInfoMap[ThisLocation::key()];

        // fire onCurrentTemplateLibraryVersionChange
        if (m_isCurrTmpLibrVerChanged)
        {
            m_isCurrTmpLibrVerChanged = false;
            m_stisTemplates->setCurrentTemplateLibraryVersion(currentInfo.currentISCSLibraryVersionKey);
            // reload the libraries
            m_stisTemplates->loadSTISTemplates();
        }

        // fire onNextTemplateLibraryVersionChange event
        if (m_isNextTmpLibrVerChanged)
        {
            m_isNextTmpLibrVerChanged = false;
            m_stisTemplates->setNextTemplateLibraryVersion(currentInfo.nextISCSLibraryVersionKey);
        }

        // fire onnewLibrarySynchronizedChange event
        if (m_isTmpLibSynchronized)
        {
            m_isTmpLibSynchronized = false;
            m_stisTemplates->setTemplateLibrarySynchronised(true);
        }
    }
}
