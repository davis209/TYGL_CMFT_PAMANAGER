/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/J155/transactive/app/signs/stis_manager/src/PidSelectionManager.h $
 * @author:  Adam Radics
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * Manages the PID list and PID groups
 */

#pragma once
#include "core/types/src/ta_types.h"
#include "IPidSelectionListener.h"
#include "PidListCtrl.h"
#include "PidTreeCtrl.h"
#include "SimpleTreeFilterCtrl.h"
#include "PidGroupCombo.h"
#include "CallbackButton.h"
#include "app/signs/common_library/src/stis_protocol/message_types/MessageTypes.h"
#include "app/signs/common_library/src/STISUtility.h"
#include "bus/signs_4669/tis_agent_access/src/TISAgentAccessFactory.h"
#include "bus/mfc_extensions/src/coloured_controls/ColourCombo.h"
#include "bus/mfc_extensions/src/list_ctrl_selection_without_focus/ListCtrlSelNoFocus.h"
#include "core/utilities/src/RunParams.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/types/TISAudit_MessageTypes.h"
#include "core/configuration_updates/src/IOnlineUpdatable.h"
#include "core/utility/src/core/Map.h"
#include <ace/Synch.h>
#include <ace/Guard_T.h>
#include <vector>
#include <list>
#include <map>
#include <string>

using TA_Base_Bus::ColourCombo;
using TA_Base_Bus::ListCtrlSelNoFocus;
using TA_Base_Bus::TISAgentAccessFactory;

using TA_IRS_App::STIS_PROTOCOL::MESSAGE_TYPES::DestinationList;

namespace TA_Core
{
    class IPidGroup;
}

// TD9158 ~ added forward declaration
namespace TA_Base_Core
{
    class ConfigUpdateMessageSender;
}

namespace TA_IRS_App
{
    class DisplayPage;

    class PidSelectionManager : public TA_Base_Core::RunParamUser, public IButtonListener, public TA_Base_Core::IOnlineUpdatable   // TD9158
    {
        friend class DisplayPage;

    public:

        using PID = STIS_UTILITY::PID;

        // List of PIDs selected on the PID selection schematics
        // list is used so iterators arent invalidated on item removal
        using PidList = std::vector<PID>;
        using PidGroupMap = st::map<std::string, PidList>;
        using PidGroupMapPair = std::pair<std::string, PidList>;

        /**
         * PidSelectionManager
         *
         * Constructor which takes gui elements
         *
         * @param pidListCtrl
         * @param pidGroupCombo
         * @param saveButton
         * @param deleteButton
         *
         */
        PidSelectionManager(PidListCtrl& pidListCtrl,
                            PidTreeCtrl& pidTreeCtrl,
                            SimpleTreeFilterCtrl& m_location_filter,
                            SimpleTreeFilterCtrl& m_area_filter,
                            PidGroupCombo& pidGroupCombo,
                            CallbackButton& saveButton,
                            CallbackButton& deleteButton,
                            IPidSelectionListener& pidSelectionListener);

        ~PidSelectionManager();

        void init();

        /**
         * setPermissions
         *
         * Set whether the user can modify PID groups.
         *
         * @param canModifyPidGroups
         */
        void setPermissions(bool canModifyPidGroups);

        /**
         * populateDisplayDestination
         *
         * Build a destination list based on the current selection.
         *
         * @param destinationList the corba sequence to populate
         */
        void populateDisplayDestination(DestinationList& destinationList);
        DestinationList populateDisplayDestination();
        std::vector<PID> get_selected_pids();
        std::pair<bool, bool> is_lcd_led_selected();

        /**
         * populateClearDestination
         *
         * Build a destination structure based on the currently selected list item.
         *
         * TES 880 - no longer a different format for clear and display messages
         *
         * @param destination  the corba structure to populate
         */
        //void populateClearDestination( TA_IRS_Core::STISDestination& destination );

        /**
         * onRunParamChange
         *
         * Called for a runparam change. selection and deselection.
         *
         */
        void onRunParamChange(const std::string& name, const std::string& value);

        /* These are passed on by the custom controls - this isnt a window, commands are just
           delegated to it so the functionality can be segregated from all the other display page
           functionality */

        /**
         * buttonPressed
         *
         * Gets called when a button is pressed.
         *
         * @param button   the button that was pressed
         */
        void buttonPressed(CallbackButton* button) override;

        /**
         * pidGroupComboChanged
         *
         * Called when another entry is selected from the pid
         * group combo
         *
         */
        void pidGroupComboChanged();

        /**
         * pidSelectionChanged
         *
         * Called when a PID is selected or deselected.
         * Or when the number of pids in the list changes.
         */
        void pidSelectionChanged();

        // Extracts the station from the header included in a run param value
        // e.g passing --select=DBG,1281,ALL will return DBG
        std::string getLocationFromRunParam(const std::string& runParamValue);

        // Extracts the x position from the header included in a run param value
        // e.g passing --select=DBG,1281,ALL will return 1281
        std::string getPositionFromRunParam(const std::string& runParamValue);

        void processShowCommand(const std::string& showCommand);

        static volatile int DONTGOFRONT_TAG;
        static ACE_Thread_Mutex Tag_mutex;

    private:

        /**
         * savePidGroup
         *
         * Called when the save group button is pressed
         *
         */
        void savePidGroup();

        /**
         * deletePidGroup
         *
         * Called when the delete group button is pressed
         *
         */
        void deletePidGroup();

        /**
         * addAllPIDs
         *
         * Add all Pids from all stations
         */
        void addAllPIDs();

        /**
         * deselectAllPIDs
         *
         * Deselects all PIDs
         */
        void deselectAllPIDs();

        /**
         * addAllStationPIDs
         *
         * Add all Pids from the given station
         *
         * @param theStation   the 3 character station name eg DBG
         */
        void addAllStationPIDs(const std::string& theStation);

        /**
         * deselectAllStationPIDs
         *
         * remove all Pids from the given station
         *
         * @param the station name eg DBG
         */
        void deselectAllStationPIDs(const std::string& theStation);

        /**
         * addLevelAtAllStations
         *
         * Add a level from all stations
         *
         * @param levelName the name of the level eg Platform1
         */
        void addLevelAtAllStations(const std::string& levelName);

        /**
         * deselectLevelAtAllStations
         *
         * Deselect a level across all stations
         *
         * @param levelName the name of the level eg Platform1
         */
        void deselectLevelAtAllStations(const std::string& levelName);

        /**
         * addLevelAtStation
         *
         * Add a level at a station
         *
         * @param station  The station name eg DBG
         * @param levelName The level eg Platform2
         */
        void addLevelAtStation(const std::string& station, const std::string& levelName);

        /**
         * deselectLevelAtStation
         *
         * Remove a level at a station
         *
         * @param station  The station name eg DBG
         * @param levelName The level eg Platform2
         */
        void deselectLevelAtStation(const std::string& station, const std::string& levelName);

        /**
         * addPID
         *
         * Add a single PID
         *
         * @param pidAssetName the PID name eg DBG.TIS.Basement.LED01
         */
        void addPID(const std::string& pidAssetName);

        /**
         * deselectPID
         *
         * Remove a single PID from the selection list
         *
         * @param pidAssetName The datanode name for the PID
         */
        void deselectPID(const std::string& pidAssetName);

        /**
         * getAllPIDs
         *
         * Gets all pids matching the search string.
         *
         * @param searchToken  an sql type search string eg DBG.TIS.%
         *
         * @return The PIDs (not levels) matching that token
         */
        std::vector<PID> getAllPIDs(std::string searchToken);

        /**
         * addPIDsToSelection
         *
         * Adds the pids to the selected pid list. ensures there are
         * no duplicates.
         *
         * @param pidList  A vector of PIDs
         * @param pidGroup The PID group to add the selection to
         */
        void addPIDsToSelection(std::vector<PID> pidList, std::string pidGroup);

        /**
         * refreshPIDList
         *
         * This will apply the active PID selection group to
         * the PID list.
         */
        void refreshPIDList();

        /**
         * activateSchematic
         *
         * Activates the current schematic
         *
         */
        void activateSchematic(unsigned int schematicID);

        // TD9158 ~ added method declaration
        /**
         * registerForConfigChanges
         *
         * This registers the application for config updates.
         *
         * @return void
         */
        void registerForConfigChanges();

        // TD9158 ~ added method declaration
        /**
         * unregisterFromConfigChanges
         *
         * This unregisters the application from registered config updates.
         *
         * @return void
         */
        void unregisterFromConfigChanges();

        // TD9158 ~ added method declaration
        /**
         * submitConfigUpdate
         *
         * Called whenever there is a change being made by this class to the database
         *  - Note: Call AFTER the changes has been made
         *
         * This will post out ConfigUpdate messages to everyone listening out for
         *  configEditor updates (with relevant change information)
         *
         * @return void
         * @param : TA_Base_Core::EModificationType modificationType
         */
        void submitConfigUpdate(TA_Base_Core::EModificationType modificationType);

        //
        // From IOnlineUpdatable
        //

        // TD9158 ~ added section for IOnlineUpdatable and method processUpdate
        /**
         * processUpdate
         *
         * When there is a configuration update of the type and key matching
         * one registered by this class, this method will be invoked
         * to process the update accordingly.
         *
         * @return  void
         * @param   updateEvent - This event contains all the
         *                        information about the update
         */
        void processUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent);

        // read the groups ( for your location ) from the db
        // then remove all groups from the m_pidGroupMap data member, less the group
        //that is associated with the schematic, then and add the new groups (from the db )
        // to m_pidGroupMap and update the combo box.
        //
        //
        //This method does not throw on read failure, instead reportPidReadError()
        // is called
        //
        void readAndAddGroups();

        //tell the user that we could not read the saved PIDs
        //and update the controls accordingly
        void handlePidReadWriteError(const std::string& error);

        //display the db error to the user
        void displayDbError(const std::string& error);

        //read the groups that you are entitled to see given the location
        //currently associated with your session id
        //throws TA_Base_Core::DatabaseException
        PidGroupMap readGroupsFromDataBase();

        //return the location associated with the current session
        std::uint32_t getSessionLocation();

        //set the Button States in the gui based on selections and the m_canUsePidGroups
        // value
        void setButtonStates();

        void on_selection_change(const std::vector<PID>& pids);
        void on_double_click(const PID& pid);

        PidGroupMap m_pidGroupMap;

        //used to store the success of the last db interaction
        bool m_dbOk = false;

        // This is the lock that protects the PID group (corba and MFC threads use it)
        TA_Base_Core::ReEntrantThreadLockable m_pidGroupMapLock;

        // this is the group that is active in the PID group combo - the group in use
        std::string m_activeGroupName;

        // the GUI controls
        PidListCtrl& m_pidListCtrl;
        PidTreeCtrl& m_pidTreeCtrl;
        SimpleTreeFilterCtrl& m_location_filter;
        SimpleTreeFilterCtrl& m_area_filter;
        PidGroupCombo& m_pidGroupCombo;
        CallbackButton& m_saveButton;
        CallbackButton& m_deleteButton;
        IPidSelectionListener& m_pidSelectionListener;

        // access control
        bool m_canUsePidGroups = false;

        // Station ID of the schematic that is currently active
        std::string m_currentLocation;

        // Schematic XPos we're currently communicating with - use as an ID
        unsigned int m_schematicID = 99999;

        //TES831 Events not logged!
        TA_Base_Core::AuditMessageSender* m_auditMessageSender = nullptr;

        TA_Base_Core::ConfigUpdateMessageSender* m_configUpdateSender = nullptr;  // TD9158
    };

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}
