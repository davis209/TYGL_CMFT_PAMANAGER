/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/RadioEventSubscriber.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2010/05/10 14:39:08 $
  * Last modified by:  $Author: builder $
  *
  */
///////////////////////////////////////////////////////////
//  RadioEventSubscriber.cpp
//  Implementation of the Class RadioEventSubscriber
//  Created on:      26-Nov-2003 03:40:30 PM
///////////////////////////////////////////////////////////

#if defined(_MSC_VER)
#pragma warning (disable:4786)
#pragma warning (disable:4503)
#endif // defined _MSC_VER

#include "core/types/src/ta_types.h"
#include "bus/security/security_utilities/src/SecurityUtilities.h"
#include "RadioEventSubscriber.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DutyAgentEntityData.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/data_access_interface/src/ISession.h"
#include "bus/security/rights_agent/IDL/src/IRightsAgentCorbaDef.h"
#include "RadioSessionServant.h"

	  
	  //#include "bus/radio/RadioAgentCorbaDef/idl/src/IRadioCorbaDef.h"

using namespace TA_IRS_App;

/**
 * constructor  default constructor
 */
RadioEventSubscriber::RadioEventSubscriber(RadioSessionServant& radioSessionServant, ta_uint32 consoleID,
	ta_uint32 locationKey, TA_Base_Bus::SessionUpdateSubject& sessionUpdateSubject)
					 : TA_Base_Bus::ISessionUpdateObserver(sessionUpdateSubject),
					   m_radioSessionServant(radioSessionServant),
					   m_consoleID(consoleID),
					   m_locationKey(locationKey)
{
}



void RadioEventSubscriber::startSubscription()
{
 	FUNCTION_ENTRY("startSubscription");

	attach();

	FUNCTION_EXIT;
}

void RadioEventSubscriber::stopSubscription()
{
 	FUNCTION_ENTRY("stopSubscription");

	detach();

	FUNCTION_EXIT;
}

void RadioEventSubscriber::sessionBegin( const std::string& session, ta_uint32 user,
			                       ta_uint32 profile, ta_uint32 location,
								   ta_uint32 workstation )
{
	FUNCTION_ENTRY("sessionBegin");
	if (workstation==m_consoleID)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
			"Session Event Begin for: %s, matches workstation %d, logging in", session.c_str(), m_consoleID);
		m_radioSessionServant.sessionBegin(session);

        m_session = session; // limin++ CL-21361
	}
	FUNCTION_EXIT;
}

void RadioEventSubscriber::sessionEnd( const std::string& session )
{
	FUNCTION_ENTRY("sessionEnd");
	ta_uint32 consoleID;
	try
	{
		//TA_Base_Core::ISession* sessionData = TA_Base_Core::SessionAccessFactory::getInstance().getSession(session);
		//TA_ASSERT(sessionData!=NULL,"No session found");
		//consoleID = sessionData->getConsoleKey();
		//delete sessionData;
	}
	catch (...)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
		 "Unable to determine console for session %s",session.c_str());
		return;
	}

	//if (consoleID==m_consoleID)
	//{
        // limin+- CL-21361
        if ( m_session == session )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "Session Event End for: %s, matches workstation %d, logging out", session.c_str(), m_consoleID);
            m_radioSessionServant.sessionEnd(session);
        }
        else
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                "Session not match [session: %s][m_session: %s][console: %d], ignore session end event", session.c_str(), m_session.c_str(), consoleID );
        }
        // +-limin CL-21361
	//}
	FUNCTION_EXIT;
}

void RadioEventSubscriber::setCurrentSessionID(const std::string& session )
{
	FUNCTION_ENTRY("setCurrentSessionID");
	m_session = session;
	FUNCTION_EXIT;
}
