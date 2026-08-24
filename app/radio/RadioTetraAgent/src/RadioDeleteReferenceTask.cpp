/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/RadioDeleteReferenceTask.cpp $
  * @author  R. van Hugten
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2008/11/28 16:26:01 $
  * Last modified by : $Author: builder $
  *
  * Declares an abstract base class for elements that can appear on the
  * RadioBackgroundProcessor request queue.
  *
  */
#include "core/types/src/ta_types.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "app/radio/RadioTetraAgent/src/RadioDeleteReferenceTask.h"
#include "app/radio/RadioTetraAgent/src/RadioTcpServer.h"
#include "app/radio/RadioTetraAgent/src/RadioSessionHelper.h"
#include "app/radio/RadioTetraAgent/src/RadioSessionException.h"

namespace TA_IRS_App
{

    RadioDeleteReferenceTask::RadioDeleteReferenceTask
    (   RadioTcpServer * radio,
        TA_IRS_App::SessionReference sessionRef,
        ta_uint32 delRef,
        RadioMethodDeleteReference::ReferenceType refType)
        : RadioTask(radio)
        , m_sessionRef(sessionRef)
        , m_deleteRef(delRef)
        , m_refType(refType)
    {
        m_taskName = "RadioDeleteReferenceTask";
    }

    RadioDeleteReferenceTask::~RadioDeleteReferenceTask()
    {
    }

    void RadioDeleteReferenceTask::perform()
    {
        FUNCTION_ENTRY("perform()");

        try
        {
            RadioSessionHelper helper(m_sessionRef, getRadio());
            helper.deleteReference(m_deleteRef, m_refType);
        }
        catch (RadioSessionException rse)
        {
            LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugWarn,
                    "Encountered Radio API error while deleting reference for unsolicited disconnect (session %d, call %lu)",
                    m_sessionRef, m_deleteRef);
        }
        // delete helper;
        
        FUNCTION_EXIT;
    }
} // namespace TA_IRS_App
