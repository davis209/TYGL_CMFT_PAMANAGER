/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/RadioResetConnectionTask.cpp $
  * @author  R. van Hugten
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2008/11/28 16:26:01 $
  * Last modified by : $Author: builder $
  *
  * Declares an abstract base class for elements that can appear on the
  * RadioBackgroundProcessor request queue.
  *
  */
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "app/radio/RadioTetraAgent/src/RadioResetConnectionTask.h"
#include "app/radio/RadioTetraAgent/src/RadioTcpServer.h"

namespace TA_IRS_App
{

    RadioResetConnectionTask::RadioResetConnectionTask(RadioTcpServer * radio)
        : RadioTask(radio)
    {
        m_taskName = "RadioResetConnectionTask";
    }

    RadioResetConnectionTask::~RadioResetConnectionTask()
    {
    }

    void RadioResetConnectionTask::perform()
    {
        FUNCTION_ENTRY("perform()");

        getRadio().doResetConnection();
        
        FUNCTION_EXIT;
    }
} // namespace TA_IRS_App
