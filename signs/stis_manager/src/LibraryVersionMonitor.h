/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/stis_manager/src/LibraryVersionMonitor.h $
 * @author:  Rob Ashcroft
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/01/08 14:06:20 $
 * Last modified by:  $Author: builder $
 *
 * This is used to keep track of the STIS predefined message library
 * version at each location
 *
 */

#pragma once
#include "core/types/src/ta_types.h"
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utility/src/core/SimpleConditionVariable.h"
#include "core/utility/src/core/preprocessor/tuple.h"
#include "core/utility/src/core/SimpleTimer.h"
#include "core/utility/src/base_ex/LocationAccessFactoryEx.h"

#define WM_UPDATE_MESSAGE_LIBRARY_VERSION  (WM_USER + 113)
#define WM_UPDATE_TEMPLATE_LIBRARY_VERSION  (WM_USER + 114)

namespace TA_IRS_App
{
    using TA_Base_Ex::LocationOrderIdComparator;

    class STISPredefinedMessages;
    class STISTemplates;

    struct LocationLibraryVersionInfo
    {
        std::string name;
        std::uint32_t locationKey;
        std::uint32_t currentISCSLibraryVersionKey;
        std::uint32_t nextISCSLibraryVersionKey;
        std::uint32_t currentSTISLibraryVersionKey;
        std::uint32_t nextSTISLibraryVersionKey;

        TA_PP_TUPLE_ENABLE_MEMBER_UTILITIES(LocationLibraryVersionInfo, (name, locationKey, currentISCSLibraryVersionKey, nextISCSLibraryVersionKey, currentSTISLibraryVersionKey, nextSTISLibraryVersionKey));
    };

    using LocationLibraryVersionInfoList = std::vector<LocationLibraryVersionInfo>;
    using LocationLibraryVersionInfoListMap = std::map<size_t, LocationLibraryVersionInfo, LocationOrderIdComparator>;

    class LibraryVersionMonitor : public TA_Base_Core::Thread // : public TA_Base_Bus::IEntityUpdateEventProcessor
    {
    public:

        using SimpleTimer = st::SimpleTimer;
        using SimpleTimerPtr = st::SimpleTimerPtr;

        /**
         * ~LibraryVersionMonitor
         *
         * Standard destructor.
         */
        virtual ~LibraryVersionMonitor();

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return      LibraryVersionMonitor&
         *              A reference to an instance of a LibraryVersionMonitor object.
         */
        static LibraryVersionMonitor& instance();

        /**
         * registerForChanges
         *
         * When the next version of the pre-defined message library is updated, window
         * will be given a WM_UPDATE_NEXT_STIS_VERSION message.
         *
         * @param window       The window to be notified of change.
         *
         */
        void registerForChanges(CWnd* window);

        /**
         * deregisterForChanges
         *
         * window is no longer interested in being notified of pre-defined message library changes.
         *
         * @param window       The window to be removed
         */
        void deregisterForChanges(CWnd* window);

        /**
         * getAllLibraryVersions
         *
         * This returns the current value of all message library versionss
         *
         * @return current value of all message libraries
         */
        LocationLibraryVersionInfoListMap getAllMessageLibraryVersions();

        /**
         * getAllLibraryVersions
         *
         * This returns the current value of all template library versionss
         *
         * @return current value of all template libraries
         */
        LocationLibraryVersionInfoListMap getAllTemplateLibraryVersions();

        bool updateAllLibraryVersions();

        void notifyMsgItemChanged();
        void notifyTmpItemChanged();

        // Thread Interface(s):
        virtual void run();
        virtual void terminate();
        void useDefaultConfig();
        bool loadConfig();
        void initalize();
        inline void process();

        void change_sync_interval_for_a_while(size_t new_interval_ms, size_t duration_ms);

    private:

        static const std::uint32_t THREAD_SLEEP_TIME;
        static const std::uint32_t ALLLOCATON_LOCATION_KEY;
        static const std::uint32_t OCC_LOCATION_KEY;
        static const std::uint32_t TDS_LOCATION_KEY;

        /**
         * LibraryVersionMonitor
         *
         * Private constructors.
         */
        LibraryVersionMonitor();
        LibraryVersionMonitor& operator=(const LibraryVersionMonitor&) = delete;
        LibraryVersionMonitor(const LibraryVersionMonitor&) = delete;

        /**
         * notifyWindowsOfChange
         *
         * Sends an update message to all interested windows
         * telling them the current library version has changed.
         *
         */
        void notifyWindowsOfChange(std::uint32_t entityKey, unsigned short newValue);

        // the window that must be notified of updates to the loaded library
        CWnd* m_windowToNotify = nullptr;

        // threadlock for message lists and maps
        TA_Base_Core::NonReEntrantThreadLockable m_messageLock;

        bool m_isInitialized = false;
        bool m_isEnabled = false;

        std::uint32_t m_interval;

        // Flag to control OnChangeEvent:
        bool m_isCurrMsgLibrVerChanged;
        bool m_isNextMsgLibrVerChanged;
        bool m_isMsgLibSynchronized;
        LocationLibraryVersionInfoListMap m_msgVersionInfoMap;
        STISPredefinedMessages* m_stisPredefinedMessages;

        bool m_isCurrTmpLibrVerChanged;
        bool m_isNextTmpLibrVerChanged;
        bool m_isTmpLibSynchronized;
        LocationLibraryVersionInfoListMap m_tmpVersionInfoMap;
        STISTemplates* m_stisTemplates;
        st::SimpleConditionVariable m_running;
    };
}
