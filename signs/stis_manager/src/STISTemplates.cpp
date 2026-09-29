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
 * This is used to load the current STIS templates.
 * Messages are organised into normal and emergency priorities, and
 * indexed by name and Id.
 *
 */

#include "stdafx.h"
#include "core/types/src/ta_types.h"
#include "boost/shared_ptr.hpp"
#include "STISTemplates.h"
#include "UserMessages.h"
#include "TisAgentInterface.h"

#include "bus/signs_4669/tis_agent_access/src/TISAgentAccessFactory.h"

#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/threads/src/Thread.h"

#include "core/data_access_interface/tis_agent_4669/src/TemplateLibraryAccessFactory.h"
#include "core/data_access_interface/tis_agent_4669/src/ITemplateLibrary.h"
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
using TA_Base_Core::ITemplateLibraryPtr;

namespace TA_IRS_App
{
    ////hongran++ TD17500
    //// All tis agent will just register the central lib version upates
    //   const std::string STISTemplates::NEXT_LIBRARY_VERSION_DP_NAME("OCC.TIS.ISCS.ISCS.aiiTISC-NextSTISLibraryVersion-CDB");
    //   const std::string STISTemplates::CURRENT_LIBRARY_VERSION_DP_NAME("OCC.TIS.ISCS.ISCS.aiiTISC-CurrentSTISLibraryVersion-CDB");
    //   const std::string STISTemplates::LIBRARY_SYNCHRONISED_DP_NAME(".TIS.ISCS.ISCS.diiTISC-StationLibrariesSynchronised");
    ////++hongran TD17500

    //int STISTemplates::m_referenceCount = 0;  // TD11310

    STISTemplates* STISTemplates::getInstance()
    {
        static STISTemplates m_theClassInstance;
        return &m_theClassInstance;
    }

    STISTemplates::STISTemplates()
        : m_currentSTISTemplateVersion(0),
        m_nextSTISTemplateVersion(0),
        m_librariesAreSynchronised(false),
        m_currentWindowsToNotify(),
        m_nextWindowsToNotify(),
        m_syncWindowsToNotify(),
        m_normalLcdTemplates(),
        m_emergencyLcdTemplates(),
        m_normalLedTemplates(),
        m_emergencyLedTemplates(),
        m_normalLcdIndexFromId(),
        m_emergencyLcdIndexFromId(),
        m_normalLedIndexFromId(),
        m_emergencyLedIndexFromId()
    {
        FUNCTION_ENTRY("STISTemplates");
        // register for library version changes
        registerForLibraryChanges();

        // the templates will be loaded once
        // the data point proxies are initialised
        FUNCTION_EXIT;
    }

    STISTemplates::~STISTemplates()
    {
        FUNCTION_ENTRY("STISTemplates");
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
        FUNCTION_EXIT
    }

    void STISTemplates::registerCurrentVersionUser(CWnd* window)
    {
        if (window != NULL)
        {
            TA_THREADGUARD(m_windowListLock);

            // add it to the map
            m_currentWindowsToNotify.push_back(window);
        }
    }

    void STISTemplates::deregisterCurrentVersionUser(CWnd* window)
    {
        TA_THREADGUARD(m_windowListLock);

        // find the map entry
        std::vector<CWnd*>::iterator findIter = std::find(m_currentWindowsToNotify.begin(),
                                                          m_currentWindowsToNotify.end(),
                                                          window);

        // if found, delete it
        if (findIter != m_currentWindowsToNotify.end())
        {
            m_currentWindowsToNotify.erase(findIter);
        }
    }

    void STISTemplates::registerNextVersionUser(CWnd* window)
    {
        if (window != NULL)
        {
            TA_THREADGUARD(m_windowListLock);

            // add it to the map
            m_nextWindowsToNotify.push_back(window);
        }
    }

    void STISTemplates::deregisterNextVersionUser(CWnd* window)
    {
        TA_THREADGUARD(m_windowListLock);

        // find the map entry
        std::vector<CWnd*>::iterator findIter = std::find(m_nextWindowsToNotify.begin(),
                                                          m_nextWindowsToNotify.end(),
                                                          window);

        // if found, delete it
        if (findIter != m_nextWindowsToNotify.end())
        {
            m_nextWindowsToNotify.erase(findIter);
        }
    }

    void STISTemplates::registerLibrarySynchronisedUser(CWnd* window)
    {
        if (window != NULL)
        {
            TA_THREADGUARD(m_windowListLock);

            // add it to the map
            m_syncWindowsToNotify.push_back(window);
        }
    }

    void STISTemplates::deregisterLibrarySynchronisedUser(CWnd* window)
    {
        TA_THREADGUARD(m_windowListLock);

        // find the map entry
        std::vector<CWnd*>::iterator findIter = std::find(m_syncWindowsToNotify.begin(),
                                                          m_syncWindowsToNotify.end(),
                                                          window);

        // if found, delete it
        if (findIter != m_syncWindowsToNotify.end())
        {
            m_syncWindowsToNotify.erase(findIter);
        }
    }

    void STISTemplates::notifyWindowsOfCurrentVersionChange()
    {
        TA_THREADGUARD(m_windowListLock);

        postMessageToWindows(m_currentWindowsToNotify, WM_UPDATE_CURRENT_STIS_TEMPLATE_VERSION);
    }

    void STISTemplates::notifyWindowsOfNextVersionChange()
    {
        TA_THREADGUARD(m_windowListLock);

        postMessageToWindows(m_nextWindowsToNotify, WM_UPDATE_NEXT_STIS_TEMPLATE_VERSION);
    }

    void STISTemplates::notifyWindowsOfLibrarySynchronisationChange()
    {
        TA_THREADGUARD(m_windowListLock);

        postMessageToWindows(m_syncWindowsToNotify, WM_UPDATE_TEMPLATE_LIBRARY_SYNCHRONISED);
    }

    void STISTemplates::postMessageToWindows(const std::vector<CWnd*>& windows, int message)
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

    const TA_Base_Core::Template* STISTemplates::getNormalLcdTemplateById(int templateId)
    {
        TA_THREADGUARD(m_templateLock);

        // find the message by id

        std::map<int, unsigned int>::const_iterator messageIndexIter =
            m_normalLcdIndexFromId.find(templateId);

        // if the message was found
        if (messageIndexIter != m_normalLcdIndexFromId.end())
        {
            // return a pointer to the template at the index we have found
            return &(m_normalLcdTemplates[messageIndexIter->second]);
        }
        else
        {
            // not found
            return NULL;
        }
    }

    const TA_Base_Core::Template* STISTemplates::getEmergencyLcdTemplateById(int templateId)
    {
        TA_THREADGUARD(m_templateLock);

        // find the message by id

        std::map<int, unsigned int>::const_iterator messageIndexIter =
            m_emergencyLcdIndexFromId.find(templateId);

        // if the message was found
        if (messageIndexIter != m_emergencyLcdIndexFromId.end())
        {
            // return a pointer to the template at the index we have found
            return &(m_emergencyLcdTemplates[messageIndexIter->second]);
        }
        else
        {
            // not found
            return NULL;
        }
    }

    const TA_Base_Core::Template* STISTemplates::getNormalLedTemplateById(int templateId)
    {
        TA_THREADGUARD(m_templateLock);

        // find the message by id

        std::map<int, unsigned int>::const_iterator messageIndexIter =
            m_normalLedIndexFromId.find(templateId);

        // if the message was found
        if (messageIndexIter != m_normalLedIndexFromId.end())
        {
            // return a pointer to the template at the index we have found
            return &(m_normalLedTemplates[messageIndexIter->second]);
        }
        else
        {
            // not found
            return NULL;
        }
    }

    const TA_Base_Core::Template* STISTemplates::getEmergencyLedTemplateById(int templateId)
    {
        TA_THREADGUARD(m_templateLock);

        // find the message by id

        std::map<int, unsigned int>::const_iterator messageIndexIter =
            m_emergencyLedIndexFromId.find(templateId);

        // if the message was found
        if (messageIndexIter != m_emergencyLedIndexFromId.end())
        {
            // return a pointer to the template at the index we have found
            return &(m_emergencyLedTemplates[messageIndexIter->second]);
        }
        else
        {
            // not found
            return NULL;
        }
    }

    void STISTemplates::registerForLibraryChanges()
    {
        //      try
        //      {
        //            // get the location name
        //            std::stringstream locationKeyStream;
        //            locationKeyStream << TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY);
        //             std::uint32_t locationKey = 0;
        //            locationKeyStream >> locationKey;
        //
        //            ILocation* location = LocationAccessFactory::getInstance().getLocationByKey(locationKey);
        //
        //            // insert it into the map
        //            m_locationName = location->getName();
        //
        //            // clean up
        //            delete location;
        //      }
        //      catch( ... )
        //      {
        //
        //      }
        //
        //        try
        //        {
        //            // Establish the current and next STIS predefined message library versions
        //            // this will provide the initial error if it fails
        //          //libo++
        //          try
        //          {
        //              CORBA_CALL_RETURN(m_currentSTISLibraryVersion,TISAgentAccessFactory::getInstance().getOccSTISAgent(),getCurrentCDBSTISMessageLibraryVersion,());
        //          }
        //          catch (TA_Base_Core::ObjectResolutionException& ex)
        //          {
        //              LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());
        //              CORBA_CALL_RETURN(m_currentSTISLibraryVersion,TISAgentAccessFactory::getInstance().getOccSTISAgent(),getCurrentCDBSTISMessageLibraryVersion,());
        //          }
        //
        //          try
        //          {
        //              CORBA_CALL_RETURN(m_librariesAreSynchronised,TISAgentAccessFactory::getInstance().getOccSTISAgent(),isStationLibrarySynchronisationComplete,());
        //          }
        //          catch (TA_Base_Core::ObjectResolutionException& ex)
        //          {
        //              LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());
        //              CORBA_CALL_RETURN(m_librariesAreSynchronised,TISAgentAccessFactory::getInstance().getOccSTISAgent(),isStationLibrarySynchronisationComplete,());
        //          }
        //          try
        //          {
        //              CORBA_CALL_RETURN(m_nextSTISLibraryVersion,TISAgentAccessFactory::getInstance().getOccSTISAgent(),getNextCDBSTISMessageLibraryVersion,());
        //          }
        //          catch (TA_Base_Core::ObjectResolutionException& ex)
        //          {
        //              LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());
        //              CORBA_CALL_RETURN(m_nextSTISLibraryVersion,TISAgentAccessFactory::getInstance().getOccSTISAgent(),getNextCDBSTISMessageLibraryVersion,());
        //          }
        //          //++libo
        //
        //        }
        //        catch (const DataException& de)
        //        {
        //            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DataException", de.what() );
        //
        //            UserMessages::getInstance().displayErrorOnce(UserMessages::UNABLE_TO_LOAD_PREDEFINED, UserMessages::ERROR_NO_AGENT_NO_PREDEFINED);
        //        }
        //        catch (const DatabaseException& dbe)
        //        {
        //            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DatabaseException", dbe.what() );
        //
        //            // this means that there is a problem with the database
        //
        //            UserMessages::getInstance().displayErrorOnce(UserMessages::UNABLE_TO_LOAD_PREDEFINED, UserMessages::ERROR_LOADING_PREDEFINED);
        //        }
        //        catch ( const TA_Base_Core::ObjectResolutionException& ore )
        //      {
        //            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::ObjectResolutionException", ore.what() );
        //
        //            UserMessages::getInstance().displayErrorOnce(UserMessages::UNABLE_TO_LOAD_PREDEFINED, UserMessages::ERROR_NO_AGENT_NO_PREDEFINED);
        //      }
        //        catch( const CORBA::Exception& cex )
        //        {
        //          LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA",
        //                 CorbaUtil::exceptionToString( cex ) );
        //
        //            UserMessages::getInstance().displayErrorOnce(UserMessages::UNABLE_TO_LOAD_PREDEFINED, UserMessages::ERROR_NO_AGENT_NO_PREDEFINED);
        //        }
        //      catch( ... )
        //      {
        //          LOG( SourceInfo, DebugUtil::ExceptionCatch, " catch TA_Base_Core::DatabaseException,STisManger otherException");
        //      }
        //
        //        try
        //        {
        //            // TD11310 ~ added calls to access factories
        //            TA_Base_Core::EntityAccessFactory& entityAccessFactory =
        //                TA_Base_Core::EntityAccessFactory::getInstance();
        //
        //            // build the datapoint names
        //          //hongran++ TD17500
        //            std::string currentLibraryDpName = CURRENT_LIBRARY_VERSION_DP_NAME;
        //            std::string nextLibraryDpName = NEXT_LIBRARY_VERSION_DP_NAME;
        //            std::string librarySyncDpName = "OCC" + LIBRARY_SYNCHRONISED_DP_NAME;
        //          //++hongran TD17500
        //
        //          // create data point proxies
        //          TA_Base_Core::IEntityData* entityDate = NULL;
        //          entityDate = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(currentLibraryDpName);
        //
        //          boost::shared_ptr<TA_Base_Core::DataPointEntityData> currentLibraryDp(dynamic_cast<TA_Base_Core::DataPointEntityData*>(entityDate));
        //
        //          TA_ASSERT(currentLibraryDp.get(), "can't create boost::shared_ptr<TA_Base_Core::DataPointEntityData> befroe create proxy.");
        //
        //          m_scadaProxyFactory.createDataPointProxy(currentLibraryDp, *this, m_currentLibraryDp); // TD11310 ~ edited
        //
        //          entityDate = NULL;
        //          entityDate = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(nextLibraryDpName);
        //
        //          boost::shared_ptr<TA_Base_Core::DataPointEntityData> nextLibraryDp(dynamic_cast<TA_Base_Core::DataPointEntityData*>(entityDate));
        //
        //          TA_ASSERT(nextLibraryDp.get(), "can't create boost::shared_ptr<TA_Base_Core::DataPointEntityData> befroe create proxy.");
        //
        //          m_scadaProxyFactory.createDataPointProxy(nextLibraryDp, *this, m_nextLibraryDp); // TD11310 ~ edited
        //
        //          entityDate = NULL;
        //          entityDate = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(librarySyncDpName);
        //
        //          boost::shared_ptr<TA_Base_Core::DataPointEntityData> librarySyncDp(dynamic_cast<TA_Base_Core::DataPointEntityData*>(entityDate));
        //
        //          TA_ASSERT(librarySyncDp.get(), "can't create boost::shared_ptr<TA_Base_Core::DataPointEntityData> befroe create proxy.");
        //
        //          m_scadaProxyFactory.createDataPointProxy(librarySyncDp, *this, m_librarySynchronisedDp); // TD11310 ~ edited
        //
        //            m_scadaProxyFactory.setProxiesToControlMode();  // TD11310 ~ edited
        //        }
        //        catch (const DataException& de)
        //        {
        //            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DataException", de.what() );
        //        }
        //        catch (const DatabaseException& dbe)
        //        {
        //            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DatabaseException", dbe.what() );
        //        }
        //        catch (...)
        //        {
        //            // data point proxy creation error
        //            LOG( SourceInfo, DebugUtil::ExceptionCatch, "...", "Error creating datapoint proxies" );
        //        }
    }

    //  void STISTemplates::processEntityUpdateEvent(std::uint32_t entityKey,
    //                                                        TA_Base_Bus::ScadaEntityUpdateType updateType)
    //  {
    //      // this means either the next library version has changed, or the current version has changed

    //      // check the type of update
    ////TD 14395
    ////zhou yuan++
    ////when create the datapoint proxy, it will receive the callback of type TA_Base_Bus::ConfigAvailable
    //      if ( updateType == TA_Base_Bus::ValueStateUpdate ||
    //           updateType == TA_Base_Bus::AlarmSummaryUpdate || //limin++, TD20740
    //           updateType == TA_Base_Bus::AckSummaryUpdate ||   //limin++, TD20740
    //           updateType == TA_Base_Bus::ConfigAvailable )
    //      {
    //          // determine which datapoint changed

    //          if ( m_currentLibraryDp->getEntityKey() == entityKey)
    //          {
    //              // an upgrade has just happened

    //              // get the new value
    //              try
    //              {
    //                  CORBA_CALL_RETURN( m_currentSTISLibraryVersion,
    //                      TISAgentAccessFactory::getInstance().getOccSTISAgent(), getCurrentCDBSTISMessageLibraryVersion, () );

    //                  // reload the libraries
    //                  loadSTISTemplates();

    //                  // tell the windows the version has changed
    //                  notifyWindowsOfCurrentVersionChange();
    //              }
    //              catch (...)
    //              {
    //                  // failed to read the value from the agent
    //                  // ignore the change, something is wrong

    //                  LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
    //                               "Received current STIS message library version change. Unable to get new value from agent.");
    //              }
    //          }
    //          else if ( m_nextLibraryDp->getEntityKey() == entityKey)
    //          {
    //              // the next version has been upgraded

    //              // get the new value
    //              try
    //              {
    //                  CORBA_CALL_RETURN( m_nextSTISLibraryVersion,
    //                      TISAgentAccessFactory::getInstance().getOccSTISAgent(), getNextCDBSTISMessageLibraryVersion, () );

    //                  // tell the windows the version has changed
    //                  notifyWindowsOfNextVersionChange();
    //              }
    //              catch (...)
    //              {
    //                  // failed to read the value from the agent
    //                  // ignore the change, something is wrong

    //                  LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
    //                               "Received next STIS message library version change. Unable to get new value from agent.");
    //              }
    //          }
    //         else if (m_librarySynchronisedDp->getEntityKey() == entityKey)
    //          {
    //              // the message libraries are synchronised (or have become not synchronised)

    //              // get the new value
    //              try
    //              {
    //                  CORBA_CALL_RETURN( m_librariesAreSynchronised,
    //                      TISAgentAccessFactory::getInstance().getOccSTISAgent(), isStationLibrarySynchronisationComplete, () );

    //                  // tell the windows the status has changed
    //                  notifyWindowsOfLibrarySynchronisationChange();
    //              }
    //              catch (...)
    //              {
    //                  // failed to read the value from the agent
    //                  // ignore the change, something is wrong

    //                  LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
    //                               "Received next STIS message library version change. Unable to get new value from agent.");
    //              }

    //          }
    //          else
    //          {
    //              LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
    //                           "Received unknown data point update for entity %d", entityKey);
    //          }
    //      }
    //  }

    void STISTemplates::loadSTISTemplates()
    {
        TA_THREADGUARD(m_templateLock);

        try
        {
            // build the template maps
            // clear them first
            m_normalLcdTemplates.clear();
            m_normalLcdIndexFromId.clear();
            m_emergencyLcdTemplates.clear();
            m_emergencyLcdIndexFromId.clear();

            m_normalLedTemplates.clear();
            m_normalLedIndexFromId.clear();
            m_emergencyLedTemplates.clear();
            m_emergencyLedIndexFromId.clear();

            // get the STIS message libraries
            /*TA_Base_Core::ITemplateLibrary* STISMessageLibrary =
                TA_Base_Core::TemplateLibraryAccessFactory::getInstance().getTemplateLibrary(m_currentSTISTemplateVersion);*/

            ITemplateLibraryPtr STISTemplateLibrary = STISClient::instance().load_template_library();

            if (!STISTemplateLibrary)
            {
                LOG_ERROR("loadSTISTemplates(): can not load display template library");
                return;
            }

            // a vector of pointers to template structures
            TA_Base_Core::ITemplateLibrary::TemplateList templates;
            templates = STISTemplateLibrary->getTemplates();

            // index count
            unsigned int normalLcdIndex = 0;
            unsigned int emergencyLcdIndex = 0;
            unsigned int normalLedIndex = 0;
            unsigned int emergencyLedIndex = 0;

            for (TA_Base_Core::ITemplateLibrary::TemplateList::iterator messageIter = templates.begin();
                 messageIter != templates.end(); messageIter++)
            {
                // sort by severity
                if ((*messageIter)->templateType == 3)
                {
                    // add to the normal priority templates
                    m_normalLcdTemplates.push_back(*(*messageIter));

                    // index by the tag (id)
                    m_normalLcdIndexFromId[(*messageIter)->templateID] = normalLcdIndex;

                    // increment the index
                    normalLcdIndex++;
                }
                else if ((*messageIter)->templateType == 1)
                {
                    // add to the normal priority templates
                    m_emergencyLcdTemplates.push_back(*(*messageIter));

                    // index by the tag (id)
                    m_emergencyLcdIndexFromId[(*messageIter)->templateID] = emergencyLcdIndex;

                    // increment the index
                    emergencyLcdIndex++;
                }
                else if ((*messageIter)->templateType == 4)
                {
                    // add to the normal priority templates
                    m_normalLedTemplates.push_back(*(*messageIter));

                    // index by the tag (id)
                    m_normalLedIndexFromId[(*messageIter)->templateID] = normalLedIndex;

                    // increment the index
                    normalLedIndex++;
                }
                else if ((*messageIter)->templateType == 2)
                {
                    // add to the normal priority templates
                    m_emergencyLedTemplates.push_back(*(*messageIter));

                    // index by the tag (id)
                    m_emergencyLedIndexFromId[(*messageIter)->templateID] = emergencyLedIndex;

                    // increment the index
                    emergencyLedIndex++;
                }
                else
                {
                    LOG_ERROR("Unknown template library typen %d. Ignoring template with ID %d",
                              (*messageIter)->templateType, (*messageIter)->templateID);
                }
            }

            // get the default adhoc attributes
            //m_defaultLedAttributes = STISMessageLibrary->getDefaultSTISLedAttributes();
            //m_defaultPlasmaAttributes = STISMessageLibrary->getDefaultSTISPlasmaAttributes();

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
                if (m_currentSTISTemplateVersion > 0)
                {
                    UserMessages::getInstance().displayErrorOnce(UserMessages::UNABLE_TO_LOAD_TEMPLATE, UserMessages::ERROR_NO_TEMPLATE_IN_DB);
                }
            }
            // otherwise its a general error
            else
            {
                UserMessages::getInstance().displayErrorOnce(UserMessages::UNABLE_TO_LOAD_TEMPLATE, UserMessages::ERROR_LOADING_TEMPLATE);
            }
        }
        catch (const DatabaseException& dbe)
        {
            LOG_EXCEPTION("TA_Base_Core::DatabaseException", dbe.what());

            // this means that there is a problem with the database

            UserMessages::getInstance().displayErrorOnce(UserMessages::UNABLE_TO_LOAD_TEMPLATE, UserMessages::ERROR_LOADING_TEMPLATE);
        }
        catch (...)
        {
            LOG_EXCEPTION("Unknown", " catch TA_Base_Core::DatabaseException,STisManger otherException");
        }
    }
} // end namespace TA_IRS_App
