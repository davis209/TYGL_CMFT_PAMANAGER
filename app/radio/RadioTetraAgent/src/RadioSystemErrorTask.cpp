/**
  * The source code in this file is the property of
  * MI Services Group and is not for redistribution
  * in any form.
  *
  * Source : $Source: /cvs/1TS-9999-T/code/transactive/app/radio/RadioTetraAgent/src/Attic/RadioSystemErrorTask.cpp,v $
  * @author  R. van Hugten
  * @version $Revision: 1.1.2.1 $
  * Last modification : $Date: 2007/10/12 12:52:24 $
  * Last modified by : $Author: robertvh $
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

#include "app/radio/RadioTetraAgent/src/RadioSystemErrorTask.h"
#include "app/radio/RadioTetraAgent/src/RadioTcpServer.h"
#include "app/radio/RadioTetraAgent/src/RadioSessionServant.h"

namespace TA_IRS_App
{

    RadioSystemErrorTask::RadioSystemErrorTask(RadioTcpServer * radio, const TA_IRS_App::RadioEvent & event)
        : RadioTask(radio)
		, m_event(*(new RadioEvent(event)))
    {
        m_taskName = "RadioSystemErrorTask";
    }

    RadioSystemErrorTask::~RadioSystemErrorTask()
    {
        delete &(m_event);
    }

    void RadioSystemErrorTask::perform()
    {
        FUNCTION_ENTRY("perform()");

        RadioEventReader * eventReader = m_event.getEventReader();
        SystemErrorEventReader * systemErrorReader = dynamic_cast<SystemErrorEventReader*>(eventReader);
        if (systemErrorReader != 0)
        {
            getRadio().onSystemError(m_event, systemErrorReader);
        }
        else
        {
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
				"Could not create SystemErrorEventReader from event given as SystemError event!");
        }
        
        FUNCTION_EXIT;
    }


} // namespace TA_IRS_App
