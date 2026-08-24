/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/RadioBackgroundProcessor.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2010/02/23 15:56:31 $
  * Last modified by:  $Author: builder $
  *
  * This class is used to send QueryReference Methods as appropriate to
  * determine information about a subscriber activity to send in the 
  * TA_Base_Bus::IRadioCorbaDef::MonitoredCallDetailsType corba struct to the
  * RadioMonitor
  *
  * See Radio::onSubscriberActivity for comments on why we do this.
  */
#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "app/radio/RadioTetraAgent/src/RadioBackgroundProcessor.h"
#include "app/radio/RadioTetraAgent/src/Radio.h"

#include "core/threads/src/ThreadPoolSingletonManager.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

using namespace TA_IRS_App;

RadioBackgroundProcessor::RadioBackgroundProcessor(Radio * radio)
: m_radio(*radio),
  m_threadPool(TA_Base_Core::ThreadPoolSingletonManager::getInstance())
{
    FUNCTION_ENTRY("RadioBackgroundProcessor");

    //not enough threads? let it grow
    m_threadPool->setUpperLimit( 200 ); // limin++ CL-21245

    FUNCTION_EXIT;
}


RadioBackgroundProcessor::~RadioBackgroundProcessor()
{
    FUNCTION_ENTRY("~RadioBackgroundProcessor");

    TA_Base_Core::ThreadPoolSingletonManager::removeInstance(m_threadPool);
    
    FUNCTION_EXIT;
}


void RadioBackgroundProcessor::addTask(RadioTaskPtr task)
{
    FUNCTION_ENTRY("addTask()");

    m_threadPool->queueWorkItem(task);

    FUNCTION_EXIT;
}

