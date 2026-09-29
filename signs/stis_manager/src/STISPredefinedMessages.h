/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/stis_manager/src/STISPredefinedMessages.h $
 * @author:  Adam Radics
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 *
 * This is used to load the current STIS pre-defined messages.
 * Messages are organised into normal and emergency priorities, and
 * indexed by name and Id.
 *
 * If a list of messages is kept, the GUI keeping the messages should
 * register itself with this object to be notified when the pre-defined messages
 * are re-loaded. A WM_UPDATE_CURRENT_STIS_VERSION will be sent.
 *
 * This will also receive notification of changes in the next message library version.
 * Windows interested in this will receive a WM_UPDATE_NEXT_STIS_VERSION message.
 *
 * This will also receive notification of changes to message library synchronisation.
 * Windows interested in this will receive a WM_UPDATE_LIBRARY_SYNCHRONISED message.
 */

#pragma once
#include "core/types/src/ta_types.h"
#include "core/data_access_interface/tis_agent_4669/src/IPredefinedMessageLibrary.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utility/src/core/Vector.h"
#include "core/utility/src/core/Map.h"

#define WM_UPDATE_CURRENT_STIS_VERSION  (WM_USER + 110)
#define WM_UPDATE_NEXT_STIS_VERSION     (WM_USER + 111)
#define WM_UPDATE_LIBRARY_SYNCHRONISED  (WM_USER + 112)

namespace TA_IRS_App
{
    class STISPredefinedMessages //: public TA_Base_Bus::IEntityUpdateEventProcessor
    {
    public:

        /**
         * ~STISPredefinedMessages
         *
         * Standard destructor.
         */
        virtual ~STISPredefinedMessages();

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return      STISPredefinedMessages&
         *              A reference to an instance of a STISPredefinedMessages object.
         */
        static STISPredefinedMessages* getInstance();

        /**
         * registerCurrentVersionUser
         *
         * When the pre-defined message library is updated, window
         * will be given a WM_UPDATE_PREDEFINEDLIST message. It should re-load its lists.
         *
         * @param window       The window to be notified of change.
         *
         */
        void registerCurrentVersionUser(CWnd* window);

        /**
         * registerNextVersionUser
         *
         * When the next version of the pre-defined message library is updated, window
         * will be given a WM_UPDATE_NEXT_STIS_VERSION message.
         *
         * @param window       The window to be notified of change.
         *
         */
        void registerNextVersionUser(CWnd* window);

        /**
         * registerLibrarySynchronisedUser
         *
         * When the next pre-defined message library version is synchronised, window
         * will be given a WM_UPDATE_LIBRARY_SYNCHRONISED message.
         *
         * @param window       The window to be notified of change.
         *
         */
        void registerLibrarySynchronisedUser(CWnd* window);

        /**
         * deregisterCurrentVersionUser
         *
         * window is no longer interested in being notified of pre-defined message library changes.
         *
         * @param window       The window to be removed
         */
        void deregisterCurrentVersionUser(CWnd* window);

        /**
         * deregisterNextVersionUser
         *
         * window is no longer interested in being notified of next message library changes.
         *
         * @param window       The window to be removed
         */
        void deregisterNextVersionUser(CWnd* window);

        /**
         * deregisterLibrarySynchronisedUser
         *
         * window is no longer interested in being notified of library synchronisation changes.
         *
         * @param window       The window to be removed
         */
        void deregisterLibrarySynchronisedUser(CWnd* window);

        /**
         * getCurrentMessageLibraryVersion
         *
         * This returns the current version of the pre-defined message library
         *
         * @return current version of the pre-defined message library
         */
        inline unsigned short getCurrentMessageLibraryVersion()
        {
            return m_currentSTISLibraryVersion;
        }

        void setCurrentMessageLibraryVersion(unsigned short newVersion)
        {
            m_currentSTISLibraryVersion = newVersion;
            notifyWindowsOfCurrentVersionChange();
        }

        /**
         * getNextMessageLibraryVersion
         *
         * This returns the current version of the pre-defined message library
         *
         * @return current version of the pre-defined message library
         */
        inline unsigned short getNextMessageLibraryVersion()
        {
            return m_nextSTISLibraryVersion;
        }

        void setNextMessageLibraryVersion(unsigned short newVersion)
        {
            m_nextSTISLibraryVersion = newVersion;
            notifyWindowsOfNextVersionChange();
        }

        /**
         * getCurrentOCCMessageLibraryVersion
         *
         * This returns the current version of the pre-defined message library
         *
         * @return current version of the pre-defined message library
         */
        inline unsigned short getCurrentOCCMessageLibraryVersion()
        {
            return m_currentOCCSTISLibraryVersion;
        }

        /**
         * getNextOCCMessageLibraryVersion
         *
         * This returns the current version of the pre-defined message library
         *
         * @return current version of the pre-defined message library
         */
        inline unsigned short getNextOCCMessageLibraryVersion()
        {
            return m_nextOCCSTISLibraryVersion;
        }

        /**
         * getMessageLibrarySynchronised
         *
         * This returns whether the message libraries are synchronised
         *
         * @return whether the message libraries are synchronised
         */
        inline bool getMessageLibrarySynchronised()
        {
            return m_librariesAreSynchronised;
        }

        void setMessageLibrarySynchronised(bool isLibrarySynchronised)
        {
            m_librariesAreSynchronised = isLibrarySynchronised;
            notifyWindowsOfLibrarySynchronisationChange();
        }

        /**
         * getNormalMessages
         *
         * Get a vector of each normal priority
         * pre-defined message
         *
         *
         * @return A vector of messages (still owned by this singleton)
         */
        inline std::vector<TA_Base_Core::PredefinedMessage> getNormalMessages()
        {
            TA_THREADGUARD(m_messageLock);

            return m_normalSTISPredefinedMessages;
        }

        /**
         * getEmergencyMessages
         *
         * Get a vector of each emergency priority
         * pre-defined message
         *
         *
         * @return A vector of messages (still owned by this singleton)
         */
        inline std::vector<TA_Base_Core::PredefinedMessage> getEmergencyMessages()
        {
            TA_THREADGUARD(m_messageLock);

            return m_emergencySTISPredefinedMessages;
        }

        /**
         * getNormalMessageById
         *
         * Given the id of a normal priority message, retrieve the message
         * corresponding to the id.
         *
         * @param messageId    The message Id to search for.
         *
         * @return     A pointer to the matching message, NULL if not found.
         */
        const TA_Base_Core::PredefinedMessage* getNormalMessageById(int messageId);

        /**
         * getEmergencyMessageById
         *
         * Given the id of an emergency priority message, retrieve the message
         * corresponding to the id.
         *
         * @param messageId    The message Id to search for.
         *
         * @return     A pointer to the matching message, NULL if not found.
         */
        const TA_Base_Core::PredefinedMessage* getEmergencyMessageById(int messageId);

        /**
         * getDefaultLedDisplayAttributes
         *
         * Gets the default ad-hoc attributes set in the predefined message library.
         *
         * @return The structure containing the default adhoc attributes.
         */
        inline TA_Base_Core::STISLedAttributes getDefaultLedDisplayAttributes()
        {
            TA_THREADGUARD(m_messageLock);
            return m_defaultLedAttributes;
        }

        /**
         * getDefaultPlasmaDisplayAttributes
         *
         * Gets the default ad-hoc attributes set in the predefined message library.
         *
         * @return The structure containing the default adhoc attributes.
         */
        inline TA_Base_Core::STISPlasmaAttributes getDefaultPlasmaDisplayAttributes()
        {
            TA_THREADGUARD(m_messageLock);
            return m_defaultPlasmaAttributes;
        }

        /**
         * processEntityUpdateEvent
         *
         * implemented from IEntityUpdateEventProcessor.
         * called when a datapoint is updated.
         *
         * @param entityKey
         * @param updateType
         *
         */
        //virtual void processEntityUpdateEvent(std::uint32_t entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType);

        /**
         * loadSTISPredefinedMessages
         *
         * load all pre-defined messages from the database
         *
         */
        void loadSTISPredefinedMessages();

    private:

        /**
         * STISPredefinedMessages
         *
         * Private constructors.
         */
        STISPredefinedMessages();
        STISPredefinedMessages& operator=(const STISPredefinedMessages&) = delete;  // TD11310 ~ edited
        STISPredefinedMessages(const STISPredefinedMessages&) = delete;   // TD11310 ~ edited

        //static const std::string NEXT_LIBRARY_VERSION_DP_NAME;
        //static const std::string CURRENT_LIBRARY_VERSION_DP_NAME;
        //static const std::string LIBRARY_SYNCHRONISED_DP_NAME;

        /**
         * registerForLibraryChanges
         *
         * registers for datapoint state changes for the library version.
         *
         */
        void registerForLibraryChanges();

        /**
         * notifyWindowsOfCurrentVersionChange
         *
         * Sends an update message to all interested windows
         * telling them the current library version has changed.
         *
         */
        void notifyWindowsOfCurrentVersionChange();

        /**
         * notifyWindowsOfNextVersionChange
         *
         * Sends an update message to all interested windows
         * telling them the next library version has changed.
         *
         */
        void notifyWindowsOfNextVersionChange();

        /**
         * notifyWindowsOfCurrentOCCVersionChange
         *
         * Sends an update message to all interested windows
         * telling them the current library version has changed.
         *
         */
        void notifyWindowsOfCurrentOCCVersionChange();

        /**
         * notifyWindowsOfNextOCCVersionChange
         *
         * Sends an update message to all interested windows
         * telling them the next library version has changed.
         *
         */
        void notifyWindowsOfNextOCCVersionChange();

        /**
         * notifyWindowsOfLibrarySynchronisationChange
         *
         * Sends an update message to all interested windows
         * telling them the library synchronisation status has changed.
         *
         */
        void notifyWindowsOfLibrarySynchronisationChange();

        /**
         * postMessageToWindows
         *
         * post the given message to the given vector of windows
         *
         * @param windows  A vector of windows to post the message to
         * @param message  The message number to post
         *
         */
        void postMessageToWindows(thread_safe::vector<CWnd*>& windows, int message);

        // The current and next pre-defined message library version at this location
        unsigned short m_currentSTISLibraryVersion;
        unsigned short m_nextSTISLibraryVersion;

        // OCC library version information
        unsigned short m_currentOCCSTISLibraryVersion = 0;
        unsigned short m_nextOCCSTISLibraryVersion = 0;
        bool m_librariesAreSynchronised = false; // according to the OCC that is

        //      // the name of the current and next library datapoints at this location
        //      TA_Base_Bus::DataPointProxySmartPtr m_currentLibraryDp;
        //      TA_Base_Bus::DataPointProxySmartPtr m_nextLibraryDp;

        //// OCC library version datapoints
        //      TA_Base_Bus::DataPointProxySmartPtr m_currentOCCLibraryDp;
        //      TA_Base_Bus::DataPointProxySmartPtr m_nextOCCLibraryDp;
        //      TA_Base_Bus::DataPointProxySmartPtr m_librarySynchronisedDp;

        // the windows that must be notified of updates to the loaded library
        st::vector<CWnd*> m_currentWindowsToNotify;
        // the windows that must be notified of updates to the next library version
        st::vector<CWnd*> m_nextWindowsToNotify;
        // the windows that must be notified of updates to the library synchronisation
        st::vector<CWnd*> m_syncWindowsToNotify;

        // a vector of normal priority pre-defined messages
        st::vector<TA_Base_Core::PredefinedMessage> m_normalSTISPredefinedMessages;

        // a vector of emergency priority pre-defined messages
        st::vector<TA_Base_Core::PredefinedMessage> m_emergencySTISPredefinedMessages;

        // a map of normal priority pre-defined message id's
        // to the message index
        st::map<int, unsigned int> m_normalMessageIndexFromId;

        // a map of emergency priority pre-defined message id's
        // to the message index
        st::map<int, unsigned int> m_emergencyMessageIndexFromId;

        // the default STIS adhoc attributes for the current message library
        TA_Base_Core::STISLedAttributes m_defaultLedAttributes;
        TA_Base_Core::STISPlasmaAttributes m_defaultPlasmaAttributes;

        // threadlock for registration/deregistration
        TA_Base_Core::NonReEntrantThreadLockable m_windowListLock;

        // threadlock for message lists and maps
        TA_Base_Core::NonReEntrantThreadLockable m_messageLock;

        std::string m_locationName;
    };
}
