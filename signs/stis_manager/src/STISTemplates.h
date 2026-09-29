/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/stis_manager/src/STISTemplates.h $
 * @author:  Adam Radics
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 *
 * This is used to load the current STIS templates.
 * Messages are organised into normal and emergency priorities, and
 * indexed by name and Id.
 *
 * If a list of messages is kept, the GUI keeping the messages should
 * register itself with this object to be notified when the templates
 * are re-loaded. A WM_UPDATE_CURRENT_STIS_VERSION will be sent.
 *
 * This will also receive notification of changes in the next message library version.
 * Windows interested in this will receive a WM_UPDATE_NEXT_STIS_VERSION message.
 *
 * This will also receive notification of changes to message library synchronisation.
 * Windows interested in this will receive a WM_UPDATE_LIBRARY_SYNCHRONISED message.
 */

#if !defined(STISTemplates_H)
#define STISTemplates_H

#if _MSC_VER > 1000
    #pragma once
#endif // _MSC_VER > 1000

#define WM_UPDATE_CURRENT_STIS_TEMPLATE_VERSION  (WM_USER + 116)
#define WM_UPDATE_NEXT_STIS_TEMPLATE_VERSION     (WM_USER + 117)
#define WM_UPDATE_TEMPLATE_LIBRARY_SYNCHRONISED  (WM_USER + 118)

#include "core/types/src/ta_types.h"
#include "core/data_access_interface/tis_agent_4669/src/ITemplateLibrary.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"

namespace TA_IRS_App
{
    class STISTemplates
    {
    public:

        /**
         * ~STISTemplates
         *
         * Standard destructor.
         */

        virtual ~STISTemplates();

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return      STISTemplates&
         *              A reference to an instance of a STISTemplates object.
         */
        static STISTemplates* getInstance();

        /**
         * registerCurrentVersionUser
         *
         * When the template library is updated, window
         * will be given a WM_UPDATE_PREDEFINEDLIST message. It should re-load its lists.
         *
         * @param window       The window to be notified of change.
         *
         */
        void registerCurrentVersionUser(CWnd* window);

        /**
         * registerNextVersionUser
         *
         * When the next version of the template library is updated, window
         * will be given a WM_UPDATE_NEXT_STIS_VERSION message.
         *
         * @param window       The window to be notified of change.
         *
         */
        void registerNextVersionUser(CWnd* window);

        /**
         * registerLibrarySynchronisedUser
         *
         * When the next template library version is synchronised, window
         * will be given a WM_UPDATE_LIBRARY_SYNCHRONISED message.
         *
         * @param window       The window to be notified of change.
         *
         */
        void registerLibrarySynchronisedUser(CWnd* window);

        /**
         * deregisterCurrentVersionUser
         *
         * window is no longer interested in being notified of template library changes.
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
         * This returns the current version of the template library
         *
         * @return current version of the template library
         */
        inline unsigned short getCurrentTemplateLibraryVersion()
        {
            return m_currentSTISTemplateVersion;
        }

        void setCurrentTemplateLibraryVersion(unsigned short newVersion)
        {
            m_currentSTISTemplateVersion = newVersion;
            notifyWindowsOfCurrentVersionChange();
        }

        /**
         * getNextMessageLibraryVersion
         *
         * This returns the current version of the template library
         *
         * @return current version of the template library
         */
        inline unsigned short getNextTemplateLibraryVersion()
        {
            return m_nextSTISTemplateVersion;
        }

        void setNextTemplateLibraryVersion(unsigned short newVersion)
        {
            m_nextSTISTemplateVersion = newVersion;
            notifyWindowsOfNextVersionChange();
        }

        /**
         * getMessageLibrarySynchronised
         *
         * This returns whether the message libraries are synchronised
         *
         * @return whether the message libraries are synchronised
         */
        inline bool getTemplateLibrarySynchronised()
        {
            return m_librariesAreSynchronised;
        }

        void setTemplateLibrarySynchronised(bool isLibrarySynchronised)
        {
            m_librariesAreSynchronised = isLibrarySynchronised;
            notifyWindowsOfLibrarySynchronisationChange();
        }

        /**
         * getNormalMessages
         *
         * Get a vector of each normal priority
         * template
         *
         *
         * @return A vector of messages (still owned by this singleton)
         */
        inline std::vector<TA_Base_Core::Template> getNormalLcdTemplates()
        {
            TA_THREADGUARD(m_templateLock);
            return m_normalLcdTemplates;
        }

        /**
         * getEmergencyMessages
         *
         * Get a vector of each emergency priority
         * template
         *
         *
         * @return A vector of messages (still owned by this singleton)
         */
        inline std::vector<TA_Base_Core::Template> getEmergencyLcdTemplates()
        {
            TA_THREADGUARD(m_templateLock);
            return m_emergencyLcdTemplates;
        }

        /**
         * getNormalMessages
         *
         * Get a vector of each normal priority
         * template
         *
         *
         * @return A vector of messages (still owned by this singleton)
         */
        inline std::vector<TA_Base_Core::Template> getNormalLedTemplates()
        {
            TA_THREADGUARD(m_templateLock);
            return m_normalLedTemplates;
        }

        /**
         * getEmergencyMessages
         *
         * Get a vector of each emergency priority
         * template
         *
         *
         * @return A vector of messages (still owned by this singleton)
         */
        inline std::vector<TA_Base_Core::Template> getEmergencyLedTemplates()
        {
            TA_THREADGUARD(m_templateLock);
            return m_emergencyLedTemplates;
        }

        const TA_Base_Core::Template* getNormalLcdTemplateById(int templateId);
        const TA_Base_Core::Template* getEmergencyLcdTemplateById(int templateId);
        const TA_Base_Core::Template* getNormalLedTemplateById(int templateId);
        const TA_Base_Core::Template* getEmergencyLedTemplateById(int templateId);

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
         * loadSTISTemplates
         *
         * load all templates from the database
         *
         */
        void loadSTISTemplates();

    private:

        /**
         * STISTemplates
         *
         * Private constructors.
         */
        STISTemplates();
        STISTemplates& operator=(const STISTemplates&);  // TD11310 ~ edited
        STISTemplates(const STISTemplates&);   // TD11310 ~ edited

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
        void postMessageToWindows(const std::vector<CWnd*>& windows, int message);

        // The current and next template library version at this location
        unsigned short m_currentSTISTemplateVersion;
        unsigned short m_nextSTISTemplateVersion;

        // OCC library version information
        //unsigned short m_currentOCCSTISLibraryVersion;
        //unsigned short m_nextOCCSTISLibraryVersion;
        bool m_librariesAreSynchronised; // according to the OCC that is

        //      // the name of the current and next library datapoints at this location
        //      TA_Base_Bus::DataPointProxySmartPtr m_currentLibraryDp;
        //      TA_Base_Bus::DataPointProxySmartPtr m_nextLibraryDp;

        //// OCC library version datapoints
        //      TA_Base_Bus::DataPointProxySmartPtr m_currentOCCLibraryDp;
        //      TA_Base_Bus::DataPointProxySmartPtr m_nextOCCLibraryDp;
        //      TA_Base_Bus::DataPointProxySmartPtr m_librarySynchronisedDp;

        // the windows that must be notified of updates to the loaded library
        std::vector<CWnd*> m_currentWindowsToNotify;
        // the windows that must be notified of updates to the next library version
        std::vector<CWnd*> m_nextWindowsToNotify;
        // the windows that must be notified of updates to the library synchronisation
        std::vector<CWnd*> m_syncWindowsToNotify;

        // a vector of normal priority templates
        std::vector<TA_Base_Core::Template> m_normalLcdTemplates;

        // a vector of emergency priority templates
        std::vector<TA_Base_Core::Template> m_emergencyLcdTemplates;

        // a vector of normal priority templates
        std::vector<TA_Base_Core::Template> m_normalLedTemplates;

        // a vector of emergency priority templates
        std::vector<TA_Base_Core::Template> m_emergencyLedTemplates;

        // a map of normal priority template id's
        // to the message index
        std::map<int, unsigned int> m_normalLcdIndexFromId;

        // a map of emergency priority template id's
        // to the message index
        std::map<int, unsigned int> m_emergencyLcdIndexFromId;

        // a map of normal priority template id's
        // to the message index
        std::map<int, unsigned int> m_normalLedIndexFromId;

        // a map of emergency priority template id's
        // to the message index
        std::map<int, unsigned int> m_emergencyLedIndexFromId;

        // threadlock for registration/deregistration
        TA_Base_Core::NonReEntrantThreadLockable m_windowListLock;

        // threadlock for message lists and maps
        TA_Base_Core::NonReEntrantThreadLockable m_templateLock;

        std::string m_locationName;
    };

} // end namespace TA_IRS_App

#endif
