/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/RadioSessionTask.cpp $
  * @author:  R. van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Declares an abstract base class for elements that can appear on the
  * RadioBackgroundProcessor request queue.
  *
  */
#if defined(_MSC_VER)
#pragma warning (disable:4786)
#pragma warning (disable:4503)
#endif // defined _MSC_VER

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"


#include "app/radio/RadioTetraAgent/src/RadioSessionTask.h"
#include "app/radio/RadioTetraAgent/src/RadioTcpServer.h"
#include "app/radio/RadioTetraAgent/src/RadioSessionServant.h"
#

namespace TA_IRS_App
{

    RadioSessionTask::RadioSessionTask(RadioTcpServer * radio, RadioSessionServant& sessionservant, 
		std::string session, RadioSessionTaskType taskType)
        : RadioTask(radio),
		m_sessionServant(sessionservant),
		m_session(session),
		m_taskType(taskType)
    {
        m_taskName = "RadioSessionTask";
    }

    RadioSessionTask::~RadioSessionTask()
    {
    }

    void RadioSessionTask::perform()
    {
        FUNCTION_ENTRY("perform()");
		switch (m_taskType)
		{
		case BeginSession:
			beginSession();
			break;
		case EndSession:
			endSession();
			break;
		default:
			break;

		}
		

        FUNCTION_EXIT;
    }

	void RadioSessionTask::beginSession()
	{
        FUNCTION_ENTRY("beginSession()");

        try
        {
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"Session Login Begin for: %s", m_session.c_str());
			m_sessionServant.sessionLogin(m_session);
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"Session Login Complete for: %s", m_session.c_str());
		}
		catch (TA_Base_Bus::IRadioSessionCorbaDef::radioAPIException & radex)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"Session Login Failed for: %s, radioAPIException: ",m_session.c_str(), radex.what.in());
			return;
		}
		catch(...)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"Session Login Failed for: %s",m_session.c_str());
			return;
		}

        FUNCTION_EXIT;
	}

	void RadioSessionTask::endSession()
	{
        FUNCTION_ENTRY("endSession()");

		try
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"Session Logout for: %s", m_session.c_str());
			m_sessionServant.sessionLogout(m_session.c_str());
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"Session Logout Complete for: %s", m_session.c_str());
		}
		catch (TA_Base_Bus::IRadioSessionCorbaDef::radioAPIException & radex)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"Session Logout Failed for: %s, radioAPIException: ",m_session.c_str(), radex.what.in());
			return;
		}
		catch (...)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"Session Logout Failed for: %s",m_session.c_str());
			return;
		}

        FUNCTION_EXIT;
	}
} // namespace TA_IRS_App
