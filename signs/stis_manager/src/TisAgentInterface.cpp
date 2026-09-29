/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File$
 * @author:  Ripple
 * @version: $Revision$
 *
 * Last modification: $DateTime$
 * Last modified by:  $Author$
 *
 */
// CachedConfig.cpp: implementation of the CachedConfig class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TisAgentInterface.h"
#include "core/synchronisation/src/ThreadGuard.h"

using namespace TA_Base_Core;

TisAgentInterface* TisAgentInterface::m_me = nullptr;
TA_Base_Core::NonReEntrantThreadLockable TisAgentInterface::m_lock;

TisAgentInterface::TisAgentInterface()
    : m_stisClient{"--server=local-tis-agent --sync-all"}
{
}

TisAgentInterface::~TisAgentInterface()
{
}

TisAgentInterface* TisAgentInterface::getInstance()
{
    if (0 == m_me)
    {
        // Double checking to prevent multiple threads
        // creating multiple instances.

        TA_THREADGUARD(m_lock);

        if (0 == m_me)
        {
            m_me = new TisAgentInterface();
        }
    }

    return m_me;
}

void TisAgentInterface::removeInstance()
{
    //
    // Guard this to prevent multiple threads atempting
    // to delete/create simultaneously
    //
    TA_THREADGUARD(m_lock);

    if (m_me != NULL)
    {
        delete m_me;
        m_me = NULL;
    }
}

STISClient& TisAgentInterface::getSTISClient()
{
    return m_stisClient;
}
