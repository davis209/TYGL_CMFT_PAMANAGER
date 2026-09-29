/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File$
 * @author:  Adam Radics
 * @version: $Revision$
 *
 * Last modification: $DateTime$
 * Last modified by:  $Author$
 *
 * Manages the PID list and PID groups
 */

#include "stdafx.h"
#include "core/types/src/ta_types.h"
#include "PidSelectionManager.h"
#include "CreateNewGroupDlg.h"
#include "UserMessages.h"
#include "helperfun.h"
#include "CurrentDisplayMessagesDlg.h"
#include "app/signs/common_library/src/STISAuditMessage.h"
#include "app/signs/common_library/src/stis_protocol/STISClient.h"
#include "app/signs/stis_manager/src/GraphworxComms.h"
#include "bus/generic_gui/src/TransactiveMessage.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/CodeConverter.h"
#include "core/utility/src/core/StdEx.h"
#include <vector>
#include "core/utility/src/core/algorithm/strings.h"
#include "core/utility/src/core/fixed_length_data/all.h"
#include "core/utility/src/core/Vector.h"
#include "core/utility/src/base_ex/RunParamsEx.h"
#include "core/utility/src/base_ex/EntityAccessFactoryEx.h"
#include "core/utility/src/base_ex/LocationAccessFactoryEx.h"
#include "core/data_access_interface/src/Location.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/EntityTypeException.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/data_access_interface/tis_agent/src/PidGroupsAccessFactory.h"
#include "core/data_access_interface/tis_agent/src/IPidGroup.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/ConfigUpdate_MessageTypes.h"
#include "core/configuration_updates/src/OnlineUpdateListener.h"
#include "core/configuration_updates/src/ConfigUpdateDetails.h"
#include "core/utility/src/base_ex/LocationAccessFactoryEx.h"
#include "core/utility/src/base_ex/DescriptionParametersEx.h"
#include <boost/tokenizer.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <algorithm>
#include <iomanip>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std::literals;
using namespace boost::lambda;
using namespace boost::adaptors;
using namespace st2::string_literals;
using st::fixed_length_data::DigitString;
using TA_Base_Ex::RunParamsEx;
using TA_Base_Ex::EntityAccessFactoryEx;
using TA_Base_Ex::LocationEx;
using TA_Base_Ex::DescriptionParametersEx;
using namespace TA_Base_Core;
using namespace TA_IRS_Core;
using TA_IRS_App::STIS_PROTOCOL::STISClient;
using PID = TA_IRS_App::STIS_UTILITY::PID;

namespace TA_IRS_App
{
    using std::placeholders::_1;

    volatile int PidSelectionManager::DONTGOFRONT_TAG = 0;
    ACE_Thread_Mutex PidSelectionManager::Tag_mutex;

    PidSelectionManager::PidSelectionManager(PidListCtrl& pidListCtrl,
                                             PidTreeCtrl& pidTreeCtrl,
                                             SimpleTreeFilterCtrl& location_filter,
                                             SimpleTreeFilterCtrl& area_filter,
                                             PidGroupCombo& pidGroupCombo,
                                             CallbackButton& saveButton,
                                             CallbackButton& deleteButton,
                                             IPidSelectionListener& pidSelectionListener)
        : m_pidGroupMap(),
        m_activeGroupName(PidGroupCombo::HMIGroupName),
        m_pidListCtrl(pidListCtrl),
        m_pidTreeCtrl(pidTreeCtrl),
        m_location_filter(location_filter),
        m_area_filter(area_filter),
        m_pidGroupCombo(pidGroupCombo),
        m_saveButton(saveButton),
        m_deleteButton(deleteButton),
        m_pidSelectionListener(pidSelectionListener),
        m_canUsePidGroups(false),
        m_dbOk(false),
        m_schematicID(99999),
        m_auditMessageSender(NULL),
        m_configUpdateSender(NULL) // TD9158
    {
        LOG_INFO("PID Selection Manager constructor");
        // set the default group
        m_activeGroupName = PidGroupCombo::HMIGroupName;

        m_saveButton.EnableWindow(m_canUsePidGroups);
        m_deleteButton.EnableWindow(m_canUsePidGroups);
    }

    void PidSelectionManager::init()
    {
        // child controls
        m_pidListCtrl.setPidSelectionManager(this);
        m_pidGroupCombo.setPidSelectionManager(this);
        m_saveButton.setCallback(this);
        m_deleteButton.setCallback(this);

        // 1. init filters

        m_location_filter.init(LocationEx::get_all_location_display_names_by_order());
        m_location_filter.ShowWindow(SW_HIDE);

        m_area_filter.init(PID::get_areas());
        m_area_filter.ShowWindow(SW_HIDE);

        // 2. init pid tree

        m_pidTreeCtrl.set_location_filter(m_location_filter.get_selection());
        m_pidTreeCtrl.set_area_filter(m_area_filter.get_selection());
        m_pidTreeCtrl.init();
        m_pidTreeCtrl.on_selection_change(std::bind(&PidSelectionManager::on_selection_change, this, _1));
        m_pidTreeCtrl.on_double_click(std::bind(&PidSelectionManager::on_double_click, this, _1));

        // add the default HMI pid group
        m_pidGroupMap[PidGroupCombo::HMIGroupName];

        //read and add the saved groups in the param to the m_pidGroupMap data member
        readAndAddGroups(); //to change...

        // Register for the PID selection/deselection
        RunParams::getInstance().registerRunParamUser(this, "Select");
        RunParams::getInstance().registerRunParamUser(this, "Deselect");
        RunParams::getInstance().registerRunParamUser(this, "SynchroniseSelected");
        RunParams::getInstance().registerRunParamUser(this, "Display");

        // This is redundant unless the STISManager crashes after selections have already been made
        GraphworxComms::getInstance().resetGraphworxDisplaysToInactive();

        for (auto param : {"Display", "Select"})
        {
            if (auto v = RunParamsEx::get_optional(param))
            {
                onRunParamChange(param, *v);
            }
        }

        m_auditMessageSender = MessagePublicationManager::getInstance().getAuditMessageSender(TISAudit::Context);

        // No longer need to process these as the GUI is initialising
        // since the GUI must let the schematic know it is ready for
        // selections/deselections via the activation method

        m_configUpdateSender = MessagePublicationManager::getInstance().getConfigUpdateMessageSender(ConfigUpdate::Context);  // TD9158 ~ added

        registerForConfigChanges(); // TD9158 ~ added
    }

    PidSelectionManager::~PidSelectionManager()
    {
        RunParams::getInstance().deregisterRunParamUser(this);
        m_pidListCtrl.setPidSelectionManager(NULL);
        m_pidGroupCombo.setPidSelectionManager(NULL);
        m_saveButton.setCallback(NULL);
        m_deleteButton.setCallback(NULL);

        // clear all PID goups
        TA_THREADGUARD(m_pidGroupMapLock);

        for (PidGroupMap::iterator iter = m_pidGroupMap.begin();
             iter != m_pidGroupMap.end(); iter++)
        {
            iter->second.clear();
        }

        m_pidGroupMap.clear();

        if (IsWindow(m_pidListCtrl.m_hWnd))
        {
            m_pidListCtrl.DeleteAllItems();
        }

        GraphworxComms::getInstance().resetGraphworxDisplaysToInactive();
    }

    void PidSelectionManager::setPermissions(bool canModifyPidGroups)
    {
        m_canUsePidGroups = canModifyPidGroups;
        setButtonStates();

        //if the location associated with the sessionId has changed we need to refresh the
        //data
        //return the location associated with the current session
        std::uint32_t newLocation = getSessionLocation();

        auto& mgr = PidGroupsAccessFactory::getInstance();
        std::uint32_t oldLocation = mgr.getLocation();

        if (oldLocation != newLocation)
        {
            mgr.setLocation(newLocation);

            readAndAddGroups();
        }
    }

    void PidSelectionManager::savePidGroup()
    {
        //if the data read/write was bugger last time try to read it
        if (!m_dbOk)
        {
            readAndAddGroups();

            //and make them hit the button again
            //todo this is a bit of a hack but it is the simple solution to sync
            return;
        }

        CreateNewGroupDlg dialog("旅客資訊管理器",
                                 "輸入 PID 群組的名稱",
                                 "",
                                 AfxGetMainWnd());

        if (dialog.DoModal() != IDOK)
        {
            return;
        }

        if (dialog.m_AccessFactoryFailure)
        {
            handlePidReadWriteError("Unidentified failure occurred while saving PID group");
            return;
        }

        TA_THREADGUARD(m_pidGroupMapLock);

        std::string newGroupName = dialog.getName();
        auto isModify = dialog.isModify();

        //these are the pids that will be in the new group
        auto pidNames = st::transform_to_vector(m_pidGroupMap[m_activeGroupName], std::mem_fn(&PID::asset));

        auto& mgr = PidGroupsAccessFactory::getInstance();

        try
        {
            //API REF:
            //void createPidGroup( const std::string& name,
            //                   std::vector<std::string> pidNames );
            mgr.createPidGroup(newGroupName, pidNames);
        }
        catch (DatabaseException& ex)
        {
            std::string error("Failure occurred while saving PID group:\n");
            error += ex.what();

            handlePidReadWriteError(error);
            return;
        }
        catch (...)
        {
            handlePidReadWriteError("Unidentified failure occurred while saving PID group");
            return;
        }

        // TES831 Events not logged!
        DescriptionParametersEx desc;
        desc.emplace_back("name", newGroupName);
        desc.emplace_back("PID list", st::join(pidNames, ","));

        if (m_auditMessageSender != NULL)
        {
            //Maochun Sun++
            //TD12781

            /*
            m_auditMessageSender->sendAuditMessage( TISAudit::STISPidGroupCreated, getSessionLocation(),
                desc,
                "", // Further description text
                "",
                "", // alarm ID - not required
                "", // incident key - not required
                ""); // event key - not required
            */

            std::string entityName = RunParams::getInstance().get(RPARAM_ENTITYNAME);
            IEntityData* guiEntity = EntityAccessFactory::getInstance().getEntity(entityName);
            TA_ASSERT(guiEntity != NULL, "EntityAccessFactory returned a NULL entity and yet did not throw an exception.");

            isModify
                ? STISAuditMessage::modifyPIDGroup(newGroupName, st::join(pidNames, ","))
                : STISAuditMessage::createPIDGroup(newGroupName, st::join(pidNames, ","))
                ;
#if 0
            m_auditMessageSender->sendAuditMessage(isModify ? TISAudit::STISPidGroupModified : TISAudit::STISPidGroupCreated, guiEntity->getKey(),
                                                   desc,
                                                   "", // Further description text
                                                   RunParams::getInstance().get(RPARAM_SESSIONID),
                                                   "", // alarm ID - not required
                                                   "", // incident key - not required
                                                   ""); // event key - not required
#endif

            //++Maochun Sun
            //TD12781
        }

        //TES831 Events not logged!

        //now refresh the data
        // readAndAddGroups(); // TD9158 ~ refresh only upon receiving the event

        // TD9158 ~ added notification for other STIS Managers
        submitConfigUpdate(EModificationType::Create);

        //TD16312
        //zhou yuan++
        readAndAddGroups();
        //++zhou yuan
    }

    void PidSelectionManager::deletePidGroup()
    {
        TA_THREADGUARD(m_pidGroupMapLock);

        //if the data read/write was bugger last time try to read it
        if (!m_dbOk)
        {
            readAndAddGroups();

            //and make them hit the button again
            return;
        }

        // TD14164 ++
        /*int responce = m_pidGroupCombo.MessageBox( _T("Are you sure you wish to delete the selected group?"),
             _T("Delete Confirmation"), MB_YESNO );*/
        TA_Base_Bus::TransActiveMessage userMsg;
        CString fieldName = "選取的群組";
        userMsg << fieldName;
        UINT responce = userMsg.showMsgBox(IDS_UW_610023);
        // ++ TD14164

        if (responce != IDYES) { return; }

        auto& mgr = PidGroupsAccessFactory::getInstance();

        const std::string deletedPidGroupName(m_activeGroupName);

        try
        {
            //API REF:
            //void deletePidGroup( const std::string& name );
            mgr.deletePidGroup(m_activeGroupName);
        }
        catch (DatabaseException& ex)
        {
            std::string error("Failure occurred while deleting PID group:\n");
            error += ex.what();

            handlePidReadWriteError(error);
            return;
        }
        catch (...)
        {
            handlePidReadWriteError("Unidentified failure occurred while deleting PID group");
            return;
        }

        // TES831 Events not logged!
        std::string pidGroupNameStr(m_activeGroupName);

        DescriptionParameters desc;
        NameValuePair pidGroupName("name", pidGroupNameStr);

        desc.push_back(&pidGroupName);

        if (m_auditMessageSender != NULL)
        {
            //Maochun Sun++
            //TD12781

            /*
            m_auditMessageSender->sendAuditMessage( TISAudit::STISPidGroupCreated, getSessionLocation(),
                desc,
                "", // Further description text
                "",
                "", // alarm ID - not required
                "", // incident key - not required
                ""); // event key - not required
            */

            std::string entityName = RunParams::getInstance().get(RPARAM_ENTITYNAME);
            IEntityData* guiEntity = EntityAccessFactory::getInstance().getEntity(entityName);
            TA_ASSERT(guiEntity != NULL, "EntityAccessFactory returned a NULL entity and yet did not throw an exception.");

            STISAuditMessage::deletePIDGroup(m_activeGroupName);
#if 0
            m_auditMessageSender->sendAuditMessage(TISAudit::STISPidGroupDeleted, guiEntity->getKey(),
                                                   desc,
                                                   "", // Further description text
                                                   RunParams::getInstance().get(RPARAM_SESSIONID),
                                                   "", // alarm ID - not required
                                                   "", // incident key - not required
                                                   ""); // event key - not required
#endif

            //++Maochun Sun
            //TD12781
        }

        //TES831 Events not logged!

        //now refresh the data
        // readAndAddGroups(); // TD9158 ~ refresh only upon receiving the event

        // TD9158 ~ added notification for other STIS Managers
        submitConfigUpdate(EModificationType::Delete);

        //TD16312
        //zhou yuan++
        readAndAddGroups();
        //++zhou yuan
    }

    void PidSelectionManager::buttonPressed(CallbackButton* button)
    {
        if (button == &m_saveButton)
        {
            savePidGroup();
        }
        else if (button == &m_deleteButton)
        {
            deletePidGroup();
        }
    }

    void PidSelectionManager::pidGroupComboChanged()
    {
        // get the current selection
        m_activeGroupName = m_pidGroupCombo.getCurrentGroupName();

        // refresh the PID list
        refreshPIDList();

        //      // TES831 Events not logged!
        //      std::string pidGroupNameStr(m_activeGroupName);
        //      std::stringstream selectedPIDNamesStream;
        //
        //
        //      //create a vector of the asset names of these pids:
        //        PidList selectedPids = m_pidGroupMap[m_activeGroupName];
        //      PidList::iterator itPid = selectedPids.begin();
        //      for(; itPid != selectedPids.end(); itPid++ )
        //      {
        //          //selectedPIDNamesStream << (*itPid).name;
        //          selectedPIDNamesStream << (*itPid).assetId;
        //
        //      }
        //
        //      DescriptionParameters desc;
        //      NameValuePair pidGroupName("Name", pidGroupNameStr );
        //      NameValuePair location( "PID LIST", selectedPIDNamesStream.str() );
        //
        //      desc.push_back(&pidGroupName);
        //      desc.push_back(&location);
        //
        //      if (m_auditMessageSender != NULL)
        //        {
        //          //Maochun Sun++
        //          //TD12781
        //
        //          /*
        //          m_auditMessageSender->sendAuditMessage( TISAudit::STISPidGroupCreated, getSessionLocation(),
        //              desc,
        //              "", // Further description text
        //              "",
        //              "", // alarm ID - not required
        //              "", // incident key - not required
        //              ""); // event key - not required
        //          */
        //
        //          std::string entityName = RunParams::getInstance().get(RPARAM_ENTITYNAME);
        //          IEntityData* guiEntity = EntityAccessFactory::getInstance().getEntity( entityName );
        //          TA_ASSERT ( guiEntity != NULL, "EntityAccessFactory returned a NULL entity and yet did not throw an exception." );
        //
        //            m_auditMessageSender->sendAuditMessage( TISAudit::STISPidGroupModified, guiEntity->getKey(),
        //              desc,
        //              "", // Further description text
        //              RunParams::getInstance().get(RPARAM_SESSIONID),
        //              "", // alarm ID - not required
        //              "", // incident key - not required
        //              ""); // event key - not required
        //
        //          //++Maochun Sun
        //          //TD12781
        //        }
        //      //TES831 Events not logged!

        setButtonStates();
    }

    void PidSelectionManager::pidSelectionChanged()
    {
        m_pidSelectionListener.pidSelectionChanged(m_pidListCtrl.GetItemCount(), m_pidListCtrl.GetFirstSelectedItemPosition());
    }

    void PidSelectionManager::onRunParamChange(const std::string& name, const std::string& value)
    {
        // The PID selection map is being updated
        // this app only regist four runparam "Select" "Deselect" "SynchroniseSelected" "Display"
        if (st::any_of_iequal({"Select", "Deselect", "SynchroniseSelected"}, name))
        {
            ACE_GUARD(ACE_Thread_Mutex, ace_mon, Tag_mutex);
            ++DONTGOFRONT_TAG;
        }

        TA_THREADGUARD(m_pidGroupMapLock);

        std::string runParamReceived = name + " " + value;

        // Break up into the header elements and the PID list
        // i.e Station, XPos,
        auto valueParts = st::splitted(value, ",");
        auto stationID = valueParts[0];
        auto newSchematicID = std::stoi(valueParts[1]);

        // Check for basic validity of Select/Deselect/Display param,
        // based on the number of items in the value
        if (st::any_of_iequal("Select, Deselect,Display"_csv, name) && valueParts.size() != 3)
        {
            LOG_ERROR("Invalid %s RunParam given to STIS Manager: %s", name, value);
            return;
        }

        auto is_all = [](auto s) { return st::any_of_iequal("?,??,???,????,?????"_csv, s); };

        if (boost::iequals(name, "Display"))
        {
            if (boost::iequals(valueParts[2], "SHOW"))
            {
                // If this --display=SHOW is for a different station or from a different schematic
                // remove the existing selections and set the new location
                if ((m_currentLocation != stationID) || (m_schematicID != newSchematicID))
                {
                    deselectAllPIDs();

                    m_currentLocation = stationID;
                    // Activate the new schematic
                    m_schematicID = newSchematicID;

                    // Remove all existing PIDs
                    m_pidListCtrl.DeleteAllItems();

                    // Deactivate the current schematic
                    GraphworxComms::getInstance().resetGraphworxDisplaysToInactive();

                    activateSchematic(m_schematicID);
                }

                // Post a message to the CSTISManagerDlg window

                //ShowWindow(SW_RESTORE);
                //m_initialDisplay = true;
                //m_wantToShow = true;

                // unsuppress messages
                UserMessages::getInstance().setMessageSuppression(false);
            }
            else
            {
                // removed for consistency with the PA manager:

                // Removed as per Propweb case #3429
                // If we hide window whenever we get a HIDE commmand, when
                // the pamanager is displayed, and user leaves then goes back to
                // the schematic, we DON'T want the PA Manager hiding as the schematic
                // is openened - the PA Manager is hidden on startup by default
                // so the Display=HIDE command is now redundant..

                //ShowWindow(SW_MINIMIZE);
                //m_wantToShow = false;
            }
        }
        else if (boost::iequals(name, "Select"))
        {
            if ((stationID == m_currentLocation) && (newSchematicID == m_schematicID))
            {
                try
                {
                    auto pidEntityName = valueParts[2];
                    // tokenize the PID list
                    auto parts = st::splitted(pidEntityName, ".");

                    // 1 part means a station
                    // 2 parts is not valid
                    // 3 parts is a level
                    // 4 parts is a PID

                    if (parts.size() == 1)
                    {
                        // '???' (all) or a station id eg 'DBG'

                        if (is_all(parts[0]))  // select everything
                        {
                            // remove_pids all PIDs
                            deselectAllPIDs();

                            // Resolve and add all PIDs to the list of selected PIDs
                            addAllPIDs();
                        }
                        else // This is a single station - e.g. 'DBG'
                        {
                            addAllStationPIDs(parts[0]);
                        }
                    }
                    else if (parts.size() == 3)
                    {
                        // If the first three characters are '???'
                        // then this must be a specific level on ALL stations
                        // e.g. ???.TIS.Basement1
                        // Add all PIDs at the specific level on all stations
                        if (is_all(parts[0]))
                        {
                            addLevelAtAllStations(parts[2]);
                        }
                        else
                        {
                            // this is a specific level at a specific station
                            addLevelAtStation(parts[0], parts[2]);
                        }
                    }
                    else if (parts.size() == 4)
                    {
                        // specific PID
                        addPID(pidEntityName);
                    }
                    else
                    {
                        LOG_ERROR("Invalid Select RunParam given to STIS Manager %s", value);
                    }
                }
                catch (TransactiveException& te)
                {
                    // database or data exception
                    // TD14164 ++
                    /*std::stringstream error;
                    error << "Error while selecting items " << te.what();*/
                    // ++ TD14164

                    LOG_EXCEPTION("TransactiveException", te.what());

                    // TD14164 ++
                    /*AfxMessageBox(error.str().c_str());*/
                    TA_Base_Bus::TransActiveMessage::show(IDS_UE_070125, "selecting", te.what());
                    // ++ TD14164
                }
                catch (...)
                {
                }
            }
        }
        else if (boost::iequals(name, "Deselect"))
        {
            if ((stationID == m_currentLocation) && (newSchematicID == m_schematicID))
            {
                try
                {
                    // tokenize the PID list
                    std::string pidEntityName = valueParts[2];
                    auto parts = st::splitted(pidEntityName, ".");

                    // 1 part means a station
                    // 2 parts is not valid
                    // 3 parts is a level
                    // 4 parts is a PID

                    if (parts.size() == 1)
                    {
                        // '???' (all) or a station id eg 'DBG'

                        if (is_all(parts[0]))  // remove_pids everything
                        {
                            // remove_pids all PIDs
                            deselectAllPIDs();
                        }
                        else // This is a single station - e.g. 'DBG'
                        {
                            deselectAllStationPIDs(parts[0]);
                        }
                    }
                    else if (parts.size() == 3)
                    {
                        // If the first three characters are '???'
                        // then this must be a specific level on ALL stations
                        // e.g. ???.TIS.Basement1
                        // Deselect all PIDs at the specific level on all stations
                        if (is_all(parts[0]))
                        {
                            deselectLevelAtAllStations(parts[2]);
                        }
                        else
                        {
                            // this is a specific level at a specific station
                            deselectLevelAtStation(parts[0], parts[2]);
                        }
                    }
                    else if (parts.size() == 4)
                    {
                        // specific PID
                        deselectPID(pidEntityName);
                    }
                    else
                    {
                        LOG_ERROR("Invalid Deselect RunParam given to STIS Manager %s", value);
                    }
                }
                catch (TransactiveException& te)
                {
                    // database or data exception
                    // TD14164 ++
                    /*std::stringstream error;
                    error << "Error while deselecting items " << te.what();*/
                    // ++ TD14164

                    LOG_EXCEPTION("TransactiveException", te.what());

                    /*AfxMessageBox(error.str().c_str());*/
                    // TD14164 ++
                    TA_Base_Bus::TransActiveMessage::show(IDS_UE_070125, "deselecting", te.what());
                    // ++ TD14164
                }
                catch (...)
                {
                }
            }
        }
        else if (boost::iequals(name, "SynchroniseSelected"))
        {
            if ((stationID == m_currentLocation) && (newSchematicID == m_schematicID))
            {
                /*
                int numberOfPIDs;

                std::stringstream numPIDStream( valueParts[2] );

                numPIDStream >> numberOfPIDs;
                */
                boost::for_each(st::splitted(valueParts[3], ";"), [&](auto& pid) { this->addPID(pid); });
            }
        }

        // Refresh the gui's PID list.  Populate the 'Station' and 'PID' list from the
        // list of selected PIDs we have.

        // This is a CORBA thread - updating the PID list from here buggers up the list
        //refreshPIDList();

        // force a refresh by "re-selecting" the current group
        m_pidGroupCombo.PostMessage(WM_COMMAND, MAKEWPARAM(m_pidGroupCombo.GetDlgCtrlID(), CBN_SELCHANGE),
                                    reinterpret_cast<LPARAM>(m_pidGroupCombo.GetSafeHwnd()));
    }

    void PidSelectionManager::addAllPIDs()
    {
        LOG_INFO("Adding all PIDs to the HMI selection");

        // get all pids
        // search on *.TIS.*
        auto pids = getAllPIDs("%.TIS.%");

        // add the PIDs to the HMI group
        addPIDsToSelection(pids, PidGroupCombo::HMIGroupName);
    }

    void PidSelectionManager::deselectAllPIDs()
    {
        LOG_INFO("Removing all PIDs from the HMI selection");

        // just clear the HMI map
        m_pidGroupMap[PidGroupCombo::HMIGroupName].clear();
    }

    void PidSelectionManager::addAllStationPIDs(const std::string& theStation)
    {
        LOG_INFO("Adding all %s station PIDs to the HMI selection", theStation);

        // get all pids for the station
        // search on STN.TIS.*
        auto pids = getAllPIDs(theStation + ".TIS.%");

        // add the PIDs to the HMI group
        addPIDsToSelection(pids, PidGroupCombo::HMIGroupName);
    }

    void PidSelectionManager::deselectAllStationPIDs(const std::string& theStation)
    {
        LOG_INFO("Removing all %s station PIDs from the HMI selection", theStation);
        // remove the PIDs from the HMI group
        auto& pids = m_pidGroupMap[PidGroupCombo::HMIGroupName];
        boost::remove_erase_if(pids, [&](auto& pid) { return pid.station == theStation; });
    }

    void PidSelectionManager::addLevelAtAllStations(const std::string& levelName)
    {
        LOG_INFO("Adding all %s level PIDs to the HMI selection", levelName);

        // get all pids for the level
        // search on *.TIS.level.*
        auto pids = getAllPIDs("%.TIS." + levelName + ".%");

        // add the PIDs to the HMI group
        addPIDsToSelection(pids, PidGroupCombo::HMIGroupName);
    }

    void PidSelectionManager::deselectLevelAtAllStations(const std::string& levelName)
    {
        LOG_INFO("Removing all %s level PIDs from the HMI selection", levelName);
        // remove the PIDs from the HMI group
        auto& pids = m_pidGroupMap[PidGroupCombo::HMIGroupName];
        boost::remove_erase_if(pids, [&](auto& pid) { return pid.level == levelName; });
    }

    void PidSelectionManager::addLevelAtStation(const std::string& station, const std::string& levelName)
    {
        LOG_INFO("Adding all %s station %s level PIDs to the HMI selection", station, levelName);

        // get all pids for the level
        // search on STN.TIS.level.*
        auto pids = getAllPIDs(station + ".TIS." + levelName + ".%");

        // add the PIDs to the HMI group
        addPIDsToSelection(pids, PidGroupCombo::HMIGroupName);
    }

    void PidSelectionManager::deselectLevelAtStation(const std::string& station, const std::string& levelName)
    {
        LOG_INFO("Removing all %s station %s level PIDs from the HMI selection", station, levelName);
        auto& pids = m_pidGroupMap[PidGroupCombo::HMIGroupName];
        boost::remove_erase_if(pids, [&](auto& pid) { return pid.station == station && pid.level == levelName; });
    }

    void PidSelectionManager::addPID(const std::string& pidAssetName)
    {
        FUNCTION_ENTRY("addPID");

        if (auto pid = PID::from_entity_name(pidAssetName))
        {
            addPIDsToSelection({pid}, PidGroupCombo::HMIGroupName);
        }

        FUNCTION_EXIT;
    }

    void PidSelectionManager::deselectPID(const std::string& pidAssetName)
    {
        // remove the PID from the HMI group
        auto& pids = m_pidGroupMap[PidGroupCombo::HMIGroupName];
        boost::remove_erase_if(pids, [&](auto& pid) { return pid.asset == pidAssetName; });
    }

    std::vector<PID> PidSelectionManager::getAllPIDs(std::string token)
    {
        auto entities = EntityAccessFactoryEx::getEntitiesOfTypeWithNameLikeToken(DataNodeEntityData::getStaticType(), token);
        return PID::from_entity_names(st::transform_to_vector(entities, [&](auto& e) { return e->getName(); }));
    }

    void PidSelectionManager::addPIDsToSelection(std::vector<PID> pidList, std::string pidGroup)
    {
        for (auto& pid : pidList)
        {
            st::push_back_if_none_of(m_pidGroupMap[pidGroup], pid, [&](auto& pid2)
            {
                auto duplicated = pid.name == pid2.name && pid.station == pid2.station;
                LOG_DEBUG_IF(duplicated, "addPIDsToSelection(): duplicate pid %s, will not add to group %s", pid.name, pidGroup);
                LOG_DEBUG_IF_NOT(duplicated, "addPIDsToSelection(): add pid %s to group %s", pid.name, pidGroup);
                return duplicated;
            });
        }
    }

    void PidSelectionManager::refreshPIDList()
    {
        TA_THREADGUARD(m_pidGroupMapLock);

        m_pidListCtrl.DeleteAllItems();

        // Helper: Convert UTF-8 to Unicode (wide string) for MFC display
        auto utf8ToWide = [](const std::string& utf8) -> std::wstring {
            if (utf8.empty()) return std::wstring();
            int wlen = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, NULL, 0);
            if (wlen <= 1) return std::wstring();
            std::wstring wstr(wlen - 1, 0);
            MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, &wstr[0], wlen);
            return wstr;
        };

        // Helper: Insert item using Unicode directly
        auto insertItemW = [](CListCtrl& list, int index, const std::wstring& text) -> int {
            LVITEMW lvi = {0};
            lvi.mask = LVIF_TEXT;
            lvi.iItem = index;
            lvi.iSubItem = 0;
            lvi.pszText = const_cast<LPWSTR>(text.c_str());
            return (int)::SendMessageW(list.m_hWnd, LVM_INSERTITEMW, 0, (LPARAM)&lvi);
        };

        // Helper: Set item text using Unicode directly
        auto setItemTextW = [](CListCtrl& list, int index, int subItem, const std::wstring& text) {
            LVITEMW lvi = {0};
            lvi.mask = LVIF_TEXT;
            lvi.iItem = index;
            lvi.iSubItem = subItem;
            lvi.pszText = const_cast<LPWSTR>(text.c_str());
            ::SendMessageW(list.m_hWnd, LVM_SETITEMTEXTW, index, (LPARAM)&lvi);
        };

        for (auto&& pid : m_pidGroupMap[m_activeGroupName])
        {
            // Database stores UTF-8, convert to Unicode for display
            std::wstring stationNameW = utf8ToWide(LocationEx::to_display_name(pid.station));
            std::wstring pidNameW = utf8ToWide(pid.name);
            int pos = insertItemW(m_pidListCtrl, PidListCtrl::LOCATION, stationNameW);
            setItemTextW(m_pidListCtrl, pos, PidListCtrl::PID_NAME, pidNameW);

            // set the pid entity key as the item data for sorting
            m_pidListCtrl.SetItemData(pos, pid.extra.entity);
            LOG_DEBUG("refreshPIDList(): add pid %s to group %s", pid.name, m_activeGroupName);
        }

        pidSelectionChanged();
    }

    void PidSelectionManager::populateDisplayDestination(DestinationList& destinationList)
    {
        // a map of levels to PIDs
        using PidMap = std::map<std::string, std::vector<std::string>>;
        PidMap destinationMap;

        // for each selected PID, add to the map
        {
            TA_THREADGUARD(m_pidGroupMapLock);

            for (auto&& pid : m_pidGroupMap[m_activeGroupName])
            {
                destinationMap[pid.station].push_back(pid.id);
            }
        }

        try
        {
            // if all stations we selected have all pids selected
            // this will remain true
            bool allPidsSoFar = true;

            // now go through the map and see if any stations have all PIDs
            for (auto&& [station, pids] : destinationMap)
            {
                // if the list has all PIDs for that station
                if (auto all = getAllPIDs(station + ".TIS.%"); all.size() == pids.size())
                {
                    pids.clear();
                    // no pids means the entire station
                    LOG_INFO("Display request for station %s simplified to all pids", station);
                }
                else
                {
                    allPidsSoFar = false;
                }
            }

            // if all stations selected have all pids selected
            if (allPidsSoFar)
            {
                // check whether all stations are selected.
                // then there will be all pids at all stations
                // "ASTN" is all stations

                // all stations is all locations minus the occ and the depot
                if (auto allLocations = LocationEx::getAllLocations(); (allLocations.size() - 2) == destinationMap.size())
                {
                    destinationList.resize(1);
                    destinationList[0].station_id = "ASTN";
                    LOG_INFO("Display request simplified to all pids at all stations");
                    return;
                }
            }
        }
        catch (...)
        {
            LOG_EXCEPTION("...", "Caught exception while optimising display request. Will use non optimised version.");
        }

        // make the sequence the right length
        destinationList.resize(destinationMap.size());

        // now convert the map to level structures
        // for each destination - check for all level and all pids
        int i = 0;

        for (auto iter = destinationMap.begin(); iter != destinationMap.end(); ++iter)
        {
            // first is the station name
            destinationList[i].station_id = iter->first.c_str();
            auto& pids = iter->second;
            destinationList[i].pid_list.assign(pids.begin(), pids.end());
            i++;
        }
    }

    DestinationList PidSelectionManager::populateDisplayDestination()
    {
        DestinationList dests;
        populateDisplayDestination(dests);
        return dests;
    }

    std::vector<PID> PidSelectionManager::get_selected_pids()
    {
        return m_pidGroupMap[m_activeGroupName];
    }

    std::pair<bool, bool> PidSelectionManager::is_lcd_led_selected()
    {
        auto& pids = m_pidGroupMap[m_activeGroupName];
        auto lcd = boost::algorithm::any_of(pids, [](auto& pid) { return pid.is_lcd(); });
        auto led = boost::algorithm::any_of(pids, [](auto& pid) { return pid.is_led(); });
        return {lcd, led};
    }

    /*

    TES 880 - no longer a different format for clear and display messages

    void PidSelectionManager::populateClearDestination( STISDestination& destination )
    {
        // get the selected item
        POSITION selection = m_pidListCtrl.GetFirstSelectedItemPosition();
        int nItem = m_pidListCtrl.GetNextSelectedItem(selection);

        TA_ASSERT( nItem > -1, "Clear buttons were enabled when nothing is selected" );

        // from the selected item, get the PID structure
        bool found = false;

        TA_THREADGUARD(m_pidGroupMapLock);

        PidList::iterator findIter;
        for( findIter = m_pidGroupMap[m_activeGroupName].begin();
        findIter != m_pidGroupMap[m_activeGroupName].end(); findIter++ )
        {
            if( (findIter->name.compare(m_pidListCtrl.GetItemText(nItem, 0)) == 0 ) &&
                (findIter->station.compare(m_pidListCtrl.GetItemText(nItem, 1)) == 0) )
            {
                found = true;
                break;
            }
        }

        TA_ASSERT( found, "Selected PID is not in internal PID list" );

        destination.station = findIter->station.c_str();
        destination.pids.length( 1 );
        destination.pids[0] = findIter->name.c_str();
    }
    */

    void PidSelectionManager::processShowCommand(const std::string& showCommand)
    {
        std::vector<std::string> commandParts;

        commandParts = tokenizeString(showCommand, ",");

        // TODO - complete
    }

    void PidSelectionManager::activateSchematic(unsigned int schematicID)
    {
        bool schematicUpdated = false;

        // Now we've finished loading, we may activate the graphworx display we're operating for
        try
        {
            schematicUpdated = GraphworxComms::getInstance().activateGraphworxDisplay(schematicID);
        }
        catch (const ValueNotSetException& e)
        {
            LOG_EXCEPTION("ValueNotSetException", e.what());
        }
        catch (...)
        {
            // Silently log any error communicating with the actual schematic, may still run
            LOG_EXCEPTION("Unknown", "While activating schematic");
        }

        /*
            #ifdef _DEBUG
                // Perform an override if a debug session
                if (CachedConfig::getInstance()->getSessionId().compare("debug") == 0)
                {
                    schematicUpdated = true;
                }
            #endif

                if (!schematicUpdated)
                {
                    //PaErrorHandler::displayModalError(PaErrorHandler::ERROR_SCHEMATIC_COMMS);
                    //PostQuitMessage(0);
                }
        */
    }

    //read the groups that you are entitled to see given the location
    //currently associated with your session id
    //throws DatabaseException
    PidSelectionManager::PidGroupMap PidSelectionManager::readGroupsFromDataBase()
    {
        auto& mgr = PidGroupsAccessFactory::getInstance();

        //you must tell the PidGroupsAccessFactory what location you are getting PidGroups for
        mgr.setLocation(getSessionLocation());

        //read the groups from the db as required - may throw DatabaseException
        auto groupData = mgr.getPidGroups();

        //for each IPidGroup retrieved convert it to a PidGroupMapPair and add to the PidGroupMap
        PidGroupMap daMap;

        for (auto group : groupData)
        {
            std::string groupName = group->getName();
            TA_ASSERT(!groupName.empty(), "failed !groupName.empty()");
            LOG_DEBUG("readGroupsFromDataBase(): groupName=%s, pids: %s", groupName, group->getPidNames());

            //get vector of PIDs in group via their entity names
            auto v = PID::from_entity_names(group->getPidNames());
            daMap.emplace(groupName, PidList{v.begin(), v.end()});
        }

        LOG_INFO("%d groups have been read from the database.", daMap.size());
        return daMap;
    }

    void PidSelectionManager::readAndAddGroups()
    {
        //read the saved groups from the db and add them to m_pidGroupMap
        try
        {
            //read the groups from the db
            auto savedGroups = readGroupsFromDataBase();

            //copy the schematic group into the the groups just read
            //if we found the schematic group add it to the groups retrieved
            if (auto it = m_pidGroupMap.find(PidGroupCombo::HMIGroupName); it != m_pidGroupMap.end())
            {
                savedGroups.insert(*it);
            }

            //now swap new data for old
            m_pidGroupMap.swap(savedGroups);

            //update the combo box:
            std::vector<std::string> groupNames;
            groupNames.push_back(PidGroupCombo::HMIGroupName);

            for (auto&& [groupName, _] : m_pidGroupMap)
            {
                if (PidGroupCombo::HMIGroupName != groupName)
                {
                    groupNames.push_back(groupName);
                }
            }

            m_pidGroupCombo.updateGroupNames(groupNames);

            setButtonStates();

            m_dbOk = true;
        }
        catch (DatabaseException& ex)
        {
            handlePidReadWriteError("Failure occurred while reading saved PID groups:\n"s + ex.what());
            return;
        }
        catch (...)
        {
            handlePidReadWriteError("Unidentified failure occurred while reading saved PID groups");
            return;
        }

        LOG_INFO("m_pidGroupMap now contains %d groups", m_pidGroupMap.size());
    }

    void PidSelectionManager::handlePidReadWriteError(const std::string& error)
    {
        m_dbOk = false;

        setButtonStates();

        //remove all groups accept the schematic related group from the local data
        //read the groups from the db
        PidGroupMap justOne;

        //copy the schematic group into justOne
        PidGroupMap::iterator iterToSchematicGroup = m_pidGroupMap.find(PidGroupCombo::HMIGroupName);

        if (iterToSchematicGroup != m_pidGroupMap.end())
        {
            justOne.insert(*iterToSchematicGroup);
        }

        //now swap new data for old
        m_pidGroupMap.swap(justOne);

        //update the combo box:
        PidGroupMap::iterator iter = m_pidGroupMap.begin();
        std::vector<std::string> groupNames;

        for (; iter != m_pidGroupMap.end(); iter++)
        {
            std::string groupName = iter->first;
            groupNames.push_back(groupName);
        }

        m_pidGroupCombo.updateGroupNames(groupNames);

        displayDbError(error);

        setButtonStates();

        m_dbOk = true;
    }

    void PidSelectionManager::displayDbError(const std::string& error)
    {
        // TD14164 ++
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << error.c_str();
        CString errMsg = userMsg.constructMessage(IDS_UE_020071);
        /*m_pidGroupCombo.MessageBox( error.c_str(), _T("Database failure"));*/
        m_pidGroupCombo.MessageBox(errMsg, _T("Database failure"));
        // ++ TD14164
    }

    std::uint32_t PidSelectionManager::getSessionLocation()
    {
        //todo verify that this updates on override and is the logical (vs.)
        //physical location

        std::stringstream locationKeyStream;
        locationKeyStream << RunParams::getInstance().get(RPARAM_LOCATIONKEY);
        std::uint32_t locationKey = 0;
        locationKeyStream >> locationKey;
        return locationKey;
    }

    void PidSelectionManager::setButtonStates()
    {
        // enable/disable the save button
        m_saveButton.EnableWindow(m_canUsePidGroups);

        //if we can delete groups and we don't have the schematic group
        //selected then enable the delete button - else disable
        if (m_canUsePidGroups && m_activeGroupName != PidGroupCombo::HMIGroupName)
        {
            m_deleteButton.EnableWindow(true);
        }
        else
        {
            m_deleteButton.EnableWindow(false);
        }
    }

    // TD9158 ~ added method definition
    void PidSelectionManager::processUpdate(const ConfigUpdateDetails& updateEvent)
    {
        FUNCTION_ENTRY("processUpdate");

        TA_ASSERT(updateEvent.getType() == STIS_PID_GROUP, _T("Invalid config update message."));

        const std::uint32_t sessionLocationKey = updateEvent.getKey();

        TA_ASSERT(sessionLocationKey > 0, _T("Invalid session location key."));

        // refresh our pid group list
        if (sessionLocationKey == getSessionLocation())
        {
            TA_THREADGUARD(m_pidGroupMapLock);

            // force a  reload of data from database
            auto& mgr = PidGroupsAccessFactory::getInstance();
            mgr.invalidate();

            // refresh current map and combo box
            readAndAddGroups();

            LOG_DEBUG(_T("Pid group successfully refreshed."));
        }

        FUNCTION_EXIT;
    }

    // TD9158 ~ added method definition
    void PidSelectionManager::registerForConfigChanges()
    {
        FUNCTION_ENTRY("registerForConfigChanges");

        try
        {
            OnlineUpdateListener::getInstance().registerAllInterests(STIS_PID_GROUP, *this);
        }
        catch (...)
        {
            LOG_EXCEPTION("...", "Failed to register to PID Group config update notification.");
        }

        FUNCTION_EXIT;
    }

    // TD9158 ~ added method definition
    void PidSelectionManager::unregisterFromConfigChanges()
    {
        FUNCTION_ENTRY("unregisterFromConfigChanges");

        try
        {
            OnlineUpdateListener::getInstance().deregisterAllInterests(STIS_PID_GROUP, *this);
        }
        catch (...)
        {
            LOG_EXCEPTION("...", "Failed to unregister from PID Group config update notification.");
        }

        FUNCTION_EXIT;
    }

    // TD9158 ~ added method definition
    void PidSelectionManager::submitConfigUpdate(EModificationType modificationType)
    {
        FUNCTION_ENTRY("submitConfigUpdate");

        TA_ASSERT(NULL != m_configUpdateSender, "m_configUpdateSender is NULL");

        // no need to specify changes as receiver will reload directly from db
        std::vector<std::string> changes;

        try
        {
            m_configUpdateSender->sendConfigUpdateMessage(
                ConfigUpdate::ConfigPaZoneGroup /*ConfigStisPidGroup*/,  // Message Type
                getSessionLocation(),           // Key of changed item
                modificationType,               // EModificationType (upd/del)
                changes,                        // Desc of changes (col names)
                NULL);                          // FilterableData

            LOG_DEBUG("Config update for PID Group change was successfully sent.");
        }
        catch (...)
        {
            LOG_EXCEPTION("...", "Failed to submit the config update for PID Group change.");
        }

        FUNCTION_EXIT;
    }

    void PidSelectionManager::on_selection_change(const std::vector<PID>& pids)
    {
        deselectAllPIDs();

        for (auto& pid : pids)
        {
            addPID(pid.asset);
        }

        refreshPIDList();
    }

    void PidSelectionManager::on_double_click(const PID& pid)
    {
        try
        {
            auto report = STISClient::instance().submit_M50_CurrentDisplayMessageTemplateRequest(pid.station, pid.id);
            CurrentDisplayMessagesDlg::show_dialog(std::make_shared<decltype(report)>(std::move(report)));
        }
        catch (std::exception& e)
        {
            UserMessages::getInstance().displayError(st2::format("Failed to get current display message and template for %s[%s]: %s", LocationEx::to_display_name(pid.station), pid.name, e.what()));
        }
    }
}
