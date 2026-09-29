/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/stis_manager/src/STISPredefinedMessages.cpp $
 * @author:  Adam Radics
 * @version: $Revision: #4 $
 *
 * Last modification: $DateTime: 2009/04/08 13:44:49 $
 * Last modified by:  $Author: builder $
 *
 * This is used to load the current STIS pre-defined messages.
 * Messages are organised into normal and emergency priorities, and
 * indexed by name and Id.
 *
 */

#include "stdafx.h"
#include "core/types/src/ta_types.h"
#include "boost/shared_ptr.hpp"
#include "STISPredefinedMessages.h"
#include "UserMessages.h"
#include "TisAgentInterface.h"

#include "bus/signs_4669/tis_agent_access/src/TISAgentAccessFactory.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include "core/threads/src/Thread.h"

#include "core/data_access_interface/tis_agent_4669/src/PredefinedMessageLibraryAccessFactory.h"
#include "core/data_access_interface/tis_agent_4669/src/PredefinedMessageLibraryTable.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/ObjectResolutionException.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
//#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
//
//#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
//#include "core/exceptions/src/ScadaProxyException.h"

#include <algorithm>

using TA_Base_Core::DebugUtil;
using TA_Base_Core::DataException;
using TA_Base_Core::DatabaseException;
using TA_Base_Core::ObjectResolutionException;
using TA_Base_Core::ILocation;
using TA_Base_Core::LocationAccessFactory;
using TA_Base_Bus::TISAgentAccessFactory;
using TA_Base_Core::IPredefinedMessageLibraryPtr;
using TA_IRS_App::STIS_PROTOCOL::STISClient;
using namespace boost::adaptors;
using namespace boost::algorithm;

namespace TA_IRS_App
{
    ////hongran++ TD17500
    //// All tis agent will just register the central lib version upates
    //   const std::string STISPredefinedMessages::NEXT_LIBRARY_VERSION_DP_NAME("OCC.TIS.ISCS.ISCS.aiiTISC-NextSTISLibraryVersion-CDB");
    //   const std::string STISPredefinedMessages::CURRENT_LIBRARY_VERSION_DP_NAME("OCC.TIS.ISCS.ISCS.aiiTISC-CurrentSTISLibraryVersion-CDB");
    //   const std::string STISPredefinedMessages::LIBRARY_SYNCHRONISED_DP_NAME(".TIS.ISCS.ISCS.diiTISC-StationLibrariesSynchronised");
    ////++hongran TD17500

    //int STISPredefinedMessages::m_referenceCount = 0;  // TD11310

    STISPredefinedMessages* STISPredefinedMessages::getInstance()
    {
        static STISPredefinedMessages m_theClassInstance;
        return &m_theClassInstance;
    }

    STISPredefinedMessages::STISPredefinedMessages()
    {
        FUNCTION_ENTRY("STISPredefinedMessages");

        // register for library version changes
        registerForLibraryChanges();

        LOG_DEBUG("start STISClient");

        // the pre-defined messages will be loaded once
        // the data point proxies are initialised
        FUNCTION_EXIT;
    }

    STISPredefinedMessages::~STISPredefinedMessages()
    {
        FUNCTION_ENTRY("STISPredefinedMessages");
        //      // stop scada factory
        //// 2008-07-18 cleart the datapointproxysmartptr
        //// the name of the current and next library datapoints at this location
        //      m_currentLibraryDp.reset();
        //      m_nextLibraryDp.reset();

        //// OCC library version datapoints
        //      m_currentOCCLibraryDp.reset();
        //      m_nextOCCLibraryDp.reset();
        //      m_librarySynchronisedDp.reset();

        // 2008-07-18 to solve the scadaproxyfactory singleton usage
        //TA_Base_Bus::ScadaProxyFactory::removeInstance();         //change removeInstance ahead

        LOG_DEBUG("stop STISClient");
        STISClient::instance().stop();

        FUNCTION_EXIT
    }

    void STISPredefinedMessages::registerCurrentVersionUser(CWnd* window)
    {
        if (window)
        {
            m_currentWindowsToNotify.push_back(window);
        }
    }

    void STISPredefinedMessages::deregisterCurrentVersionUser(CWnd* window)
    {
        m_currentWindowsToNotify.remove(window);
    }

    void STISPredefinedMessages::registerNextVersionUser(CWnd* window)
    {
        if (window != NULL)
        {
            m_nextWindowsToNotify.push_back(window);
        }
    }

    void STISPredefinedMessages::deregisterNextVersionUser(CWnd* window)
    {
        m_nextWindowsToNotify.remove(window);
    }

    void STISPredefinedMessages::registerLibrarySynchronisedUser(CWnd* window)
    {
        if (window != NULL)
        {
            m_syncWindowsToNotify.push_back(window);
        }
    }

    void STISPredefinedMessages::deregisterLibrarySynchronisedUser(CWnd* window)
    {
        m_syncWindowsToNotify.remove(window);
    }

    void STISPredefinedMessages::notifyWindowsOfCurrentVersionChange()
    {
        postMessageToWindows(m_currentWindowsToNotify, WM_UPDATE_CURRENT_STIS_VERSION);
    }

    void STISPredefinedMessages::notifyWindowsOfNextVersionChange()
    {
        postMessageToWindows(m_nextWindowsToNotify, WM_UPDATE_NEXT_STIS_VERSION);
    }

    void STISPredefinedMessages::notifyWindowsOfLibrarySynchronisationChange()
    {
        postMessageToWindows(m_syncWindowsToNotify, WM_UPDATE_LIBRARY_SYNCHRONISED);
    }

    void STISPredefinedMessages::postMessageToWindows(thread_safe::vector<CWnd*>& windows, int message)
    {
        windows.for_each([&](auto w) { w->PostMessage(message); });
    }

    const TA_Base_Core::PredefinedMessage* STISPredefinedMessages::getNormalMessageById(int messageId)
    {
        auto it = m_normalMessageIndexFromId.find(messageId);
        return it != m_normalMessageIndexFromId.end()
            ? &m_normalSTISPredefinedMessages[it->second]
            : nullptr
            ;
    }

    const TA_Base_Core::PredefinedMessage* STISPredefinedMessages::getEmergencyMessageById(int messageId)
    {
        auto it = m_emergencyMessageIndexFromId.find(messageId);
        return it != m_emergencyMessageIndexFromId.end()
            ? &m_emergencySTISPredefinedMessages[it->second]
            : nullptr
            ;
    }

    void STISPredefinedMessages::registerForLibraryChanges()
    {
    }

    void STISPredefinedMessages::loadSTISPredefinedMessages()
    {
        LOG_DEBUG("load STIS Predefined Messages");

        try
        {
            // build the pre-defined message maps
            // clear them first
            m_normalSTISPredefinedMessages.clear();
            m_normalMessageIndexFromId.clear();
            m_emergencySTISPredefinedMessages.clear();
            m_emergencyMessageIndexFromId.clear();

            // get the STIS message libraries
            /*TA_Base_Core::IPredefinedMessageLibrary* STISMessageLibrary =
                TA_Base_Core::PredefinedMessageLibraryAccessFactory::getInstance().getPredefinedMessageLibrary(m_currentSTISLibraryVersion,
                                                                                                          TA_Base_Core::TA_TISAgentDAI::LIBRARY_TYPE_STIS);*/

            IPredefinedMessageLibraryPtr STISMessageLibrary = STISClient::instance().load_message_library();

            if (!STISMessageLibrary)
            {
                LOG_ERROR("loadSTISPredefinedMessages(): can not load message library");
                return;
            }

            LOG_DEBUG("load STIS Predefined Messages complete");

            // a vector of pointers to pre-defined message structures
            TA_Base_Core::IPredefinedMessageLibrary::PredefinedMessageList predefinedMessages;
            predefinedMessages = STISMessageLibrary->getMessages();

            // index count
            unsigned int normalIndex = 0;
            unsigned int emergencyIndex = 0;

            for (TA_Base_Core::IPredefinedMessageLibrary::PredefinedMessageList::iterator messageIter = predefinedMessages.begin();
                 messageIter != predefinedMessages.end(); messageIter++)
            {
                // sort by severity
                if ((*messageIter)->librarySection == TA_Base_Core::NORMAL_SECTION)
                {
                    // add to the normal priority pre-defined messages
                    m_normalSTISPredefinedMessages.push_back(*(*messageIter));

                    // index by the tag (id)
                    m_normalMessageIndexFromId[(*messageIter)->messageTag] = normalIndex;

                    // increment the index
                    normalIndex++;
                }
                else if ((*messageIter)->librarySection == TA_Base_Core::EMERGENCY_SECTION)
                {
                    // add to the normal priority pre-defined messages
                    m_emergencySTISPredefinedMessages.push_back(*(*messageIter));

                    // index by the tag (id)
                    m_emergencyMessageIndexFromId[(*messageIter)->messageTag] = emergencyIndex;

                    // increment the index
                    emergencyIndex++;
                }
                else
                {
                    LOG_ERROR("Unknown library section %d. Ignoring message with tag %d", (*messageIter)->librarySection, (*messageIter)->messageTag);
                }
            }

            // get the default adhoc attributes
            m_defaultLedAttributes = STISMessageLibrary->getDefaultSTISLedAttributes();
            m_defaultPlasmaAttributes = STISMessageLibrary->getDefaultSTISPlasmaAttributes();

            /*delete STISMessageLibrary;
            STISMessageLibrary = NULL;*/
        }
        catch (const DataException& de)
        {
            LOG_EXCEPTION("TA_Base_Core::DataException", de.what());

            // this means there is an error with the data

            // if theres no data in the database
            if (de.getFailType() == DataException::NO_VALUE)
            {
                // this is only an error if the load attempt was for a valid version
                if (m_currentSTISLibraryVersion > 0)
                {
                    UserMessages::getInstance().displayErrorOnce(UserMessages::UNABLE_TO_LOAD_PREDEFINED, UserMessages::ERROR_NO_PREDEFINED_IN_DB);
                }
            }
            // otherwise its a general error
            else
            {
                UserMessages::getInstance().displayErrorOnce(UserMessages::UNABLE_TO_LOAD_PREDEFINED, UserMessages::ERROR_LOADING_PREDEFINED);
            }
        }
        catch (const DatabaseException& dbe)
        {
            LOG_EXCEPTION("TA_Base_Core::DatabaseException", dbe.what());

            // this means that there is a problem with the database

            UserMessages::getInstance().displayErrorOnce(UserMessages::UNABLE_TO_LOAD_PREDEFINED, UserMessages::ERROR_LOADING_PREDEFINED);
        }
        catch (...)
        {
            LOG_EXCEPTION("Unknown", "catch TA_Base_Core::DatabaseException,STisManger otherException");
        }
    }
}
