/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File$
 * @author:  Robin Ashcroft
 * @version: $Revision$
 *
 * Last modification: $DateTime$
 * Last modified by:  $Author$
 *
 */

#if !defined(AFX_STISMANAGERGUI_H__BB126C70_6346_4E08_9189_14B9BD4A98EF__INCLUDED_)
#define AFX_STISMANAGERGUI_H__BB126C70_6346_4E08_9189_14B9BD4A98EF__INCLUDED_

#if _MSC_VER > 1000
    #pragma once
#endif // _MSC_VER > 1000

#include "core/types/src/ta_types.h"
#include "bus/generic_gui/src/AbstractDialogGUI.h"

namespace TA_IRS_App
{
    class CSTISManagerGUI : public TA_Base_Bus::AbstractDialogGUI
    {
    public:

        CSTISManagerGUI();
        virtual ~CSTISManagerGUI();

        //TD18095, jianghp
        virtual void onInitGenericGUICompleted();

        /**
         * getApplicationType
         *
         * This returns the application type for this GUI. GenericGUI will then save
         * this in the RunParams and will use it to pass to the Control Station. In the
         * existing system the application type would need to be hard-coded into here.
         *
         * @return std::uint32_t representing the application type
         *
         */
        ta_uint32 getApplicationType();

        /**
         * checkEntity
         *
         * This will check that the entity is valid and then store a local pointer to the
         * entity.
         *
         * @param IEntityData* guiEntity - A pointer to the gui entity to check and store.
         *
         * @exceptions GenericGUIException with the type set to ENTITY_CONFIGURATION_INVALID
         *             This should be thrown if the application checks the entity and it is
         *             not configured correctly for the GUI to run.
         *
         */
        void checkEntity(TA_Base_Core::IEntityData* guiEntity);

        /**
         * checkCommandLine
         *
         * This checks that all the appropriate RunParams have been set from the command line.
         * If anything is invalid or missing then this will throw an exception.
         * All settings SHOULD be in the GUI entity and the only additional command line
         * parameter should be --control-entity.
         *
         * @exceptions GenericGUIException with the type set to COMMAND_LINE_INVALID
         *             It must throw this if the command line does not contain the correct
         *             information for the GUI to run.
         */
        void checkCommandLine();

        /**
         * entityChanged
         *
         * This will be called by GenericGUI when it receives a callback indicating that the
         * GUI entity has been modified. GenericGUI will have invalidated the entity database
         * object and will tell the GUI which items were modified. The GUI just has to retrieve
         * the details out of it and update accordingly.
         *
         * @param vector<string>& changes - This is a list of all attributes changed in the
         *                                  object.
         */
        void entityChanged(const std::vector<std::string>& changes);

        /**
         * serverIsDown
         *
         * This is called when the server the workstation is using goes down. The GUI should
         * indicate to the user that the server is down. It should also then limit operations
         * so that the user can only view existing data.
         */
        void serverIsDown();

        /**
         * serverIsUp
         *
         * This is called when the server the workstation is using starts functioning again.
         * The GUI should indicate to the user that the server is functioning. It should
         * then allow the user to perform all valid operations again.
         */
        void serverIsUp();

        /**
         * loadAllUserSettings
         *
         * This will load all the application's user preferences if configured to do so.
         * This should be called by the application itself in OnInit() or equivalent.
         *
         * @exceptions TODO:
         */
        void loadAllUserSettings();

        /**
         * saveAllUserSettings
         *
         * This will save all the application's user preferences if configured to do so.
         * This is provided in case the GUI needs to save user preferences at a predetermined
         * time. The settings are already saved when the application exits.
         *
         * @exceptions TODO:
         */
        void saveAllUserSettings();

        /**
         * changeFocus
         *
         * Implementing for IGUIApplication. The Control Station needs to alert the GUI
         * applications when they should regain the focus on the screen. This is the most
         * common focus request by the Control Station but it may also need to tell the
         * applications to minimise or maximise. This is done through this method. This takes
         * an enumeration indicating how the GUI should change it's focus. Changing the focus
         * of a window is the same for all window types so this can be implemented here.
         *
         * @param    EFocusType focus - This indicates how the window should change it's focus.
         */
        //void changeFocus(const TA_Base_Core::EFocusType focus); //TES230

        virtual void setPosition(const ta_uint32 posFlag,
                                 const ta_uint32 alignFlag,
                                 const RECT& objectDim,
                                 const RECT& boundaryDim);

    protected:

        /**
         * prepareForClose
         *
         * This method is used to prepare the GUI for closing. It should save the user settings
         * and perform any other tidying up. eg Stop any processing threads, ask the user if they
         * wish to save anything etc.
         */
        void prepareForClose();

        /**
         * createApplicationObject
         *
         * This method must set the m_application member variable. It should create a new
         * instance of the dialog/document/property sheet and assign it to m_application
         */
        virtual void createApplicationObject();

    private:

        //
        // Hide the copy constructor and assignment operator as they
        // are not needed.
        //
        CSTISManagerGUI(const CSTISManagerGUI& theCSTISManagerGUI);
        CSTISManagerGUI& operator=(const CSTISManagerGUI&);
        bool m_guiInitCompleted;
    };
} // TA_IRS_App

#endif // !defined(AFX_STISMANAGERGUI_H__BB126C70_6346_4E08_9189_14B9BD4A98EF__INCLUDED_)
