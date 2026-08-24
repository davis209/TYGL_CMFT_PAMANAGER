/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_TIP/4669/transactive/app/radio/RadioTetraAgent/src/RadioResetSessionTask.cpp $
  * @author  
  * @version $Revision: #1 $
  * Last modification : $DateTime: $
  * Last modified by : $Author: $
  *
  * Declares an abstract base class for elements that can appear on the
  * RadioBackgroundProcessor request queue.
  *
  */
#include "core/types/src/ta_types.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "app/radio/RadioTetraAgent/src/RadioResetSessionTask.h"
#include "app/radio/RadioTetraAgent/src/RadioTcpServer.h"

namespace TA_IRS_App
{

    RadioResetSessionTask::RadioResetSessionTask(RadioTcpServer * radio , ta_uint32 sessionRef)
        : RadioTask(radio),
		m_sessionRef(sessionRef)
    {
        m_taskName = "RadioResetSessionTask";
    }

    RadioResetSessionTask::~RadioResetSessionTask()
    {
    }

    void RadioResetSessionTask::perform()
    {
        FUNCTION_ENTRY("perform()");

        getRadio().doResetSession( m_sessionRef );
        
        FUNCTION_EXIT;
    }
} // namespace TA_IRS_App
