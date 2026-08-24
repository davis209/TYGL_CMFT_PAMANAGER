/**
  * The source code in this file is the property of
  * MI Services Group and is not for redistribution
  * in any form.
  *
  * Source : $Source: /cvs/1TS-9999-T/code/transactive/app/radio/RadioTetraAgent/src/Attic/RadioTask.cpp,v $
  * @author  R. van Hugten
  * @version $Revision: #1 $
  * Last modification : $Date: 2008/11/28 $
  * Last modified by : $Author: builder $
  *
  * Defines a base class for elements that can appear on the
  * RadioBackgroundProcessor task queue.
  *
  */

#include "app/radio/RadioTetraAgent/src/RadioTask.h"
#include "app/radio/RadioTetraAgent/src/RadioFallbackException.h"
#include "app/radio/RadioTetraAgent/src/RadioSessionException.h"
#include "app/radio/RadioTetraAgent/src/RadioSessionNotInitialisedException.h"
#include "app/radio/RadioTetraAgent/src/RadioMethodTimeoutException.h"

#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App
{
    using TA_Base_Core::DebugUtil;

    RadioTask::RadioTask(RadioTcpServer * radio)
        : m_radio(*radio),
          m_taskName("")
    {
    }

    RadioTask::~RadioTask()
    {
    }

    RadioTcpServer & RadioTask::getRadio()
    {
        return m_radio; 
    }

    const RadioTcpServer & RadioTask::getRadio() const
    {
        return m_radio; 
    }
    
    void RadioTask::executeWorkItem()
    {
        try
        {
            perform();
        }
        catch(RadioSessionNotInitialisedException&)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Leaked RadioSessionNotInitialisedException", m_taskName.c_str());
        }
        catch(RadioSessionException&)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Leaked RadioSessionException", m_taskName.c_str());
        }
        catch(RadioMethodTimeoutException&)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Leaked RadioMethodTimeoutException", m_taskName.c_str());
        }
        catch(RadioFallbackException&)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Leaked RadioFallbackException", m_taskName.c_str());
        }
        catch(TA_Base_Core::TransactiveException&)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Leaked TransactiveException", m_taskName.c_str());
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Leaked ...", m_taskName.c_str());
        }
    }
    
    
} // namespace TA_IRS_App
