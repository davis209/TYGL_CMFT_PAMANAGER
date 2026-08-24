/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_TIP/4669/transactive/app/radio/RadioTetraAgent/src/RadioResetSocketTask.cpp $
  * @author  
  * @version $Revision: #1 $
  * Last modification : $DateTime: $
  * Last modified by : $Author: $
  *
  * Declares an abstract base class for elements that can appear on the
  * RadioBackgroundProcessor request queue.
  *
  */
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "app/radio/RadioTetraAgent/src/RadioResetSocketTask.h"
#include "app/radio/RadioTetraAgent/src/RadioTcpServer.h"

namespace TA_IRS_App
{

    RadioResetSocketTask::RadioResetSocketTask(RadioTcpServer * radio)
        : RadioTask(radio)
    {
        m_taskName = "RadioResetSocketTask";
    }

    RadioResetSocketTask::~RadioResetSocketTask()
    {
    }

    void RadioResetSocketTask::perform()
    {
        FUNCTION_ENTRY("perform()");

        getRadio().doResetSocket();
        
        FUNCTION_EXIT;
    }
} // namespace TA_IRS_App
