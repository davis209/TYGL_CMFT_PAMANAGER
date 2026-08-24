/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/Test/stub/Radio.cpp $
  * @author:  Kent Yip
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * Radio class stub.
  *
  */


#include "core/types/src/ta_types.h"
#include "Radio.h"
#include "core/utilities/src/DebugUtil.h"

#include "DataManager.h"

using namespace TA_IRS_App;


Radio::Radio(IRadioCallback& callback) 
      :	m_radioTransactionManager(0),
	    m_radioConnectionManager(0),
		m_radioResourceManager(0),
		m_radioAuthorisationManager(0),
		m_radioMonitorManager(0),
		m_ISCSRadioSession(0),
        m_callback(callback),
        m_directoryCallback(0),
        m_backgroundProcessor(0),
        m_radioCircuitModeManager(0)
{
    this->m_radioCircuitModeManager = new RadioCircuitModeManager(this);
}


Radio::~Radio()
{
    delete this->m_radioCircuitModeManager;
}


IRadioCallback& Radio::getRadioCallback()
{
    return this->m_callback;
}


RadioCircuitModeManager* Radio::getRadioCircuitModeManager()
{
	return this->m_radioCircuitModeManager;
}

/**
* sends a CircuitData message to the train identified in the message.  
*/
References Radio::sendTrainCircuitData( const CircuitModeData& data )
{
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                 "Received data to transfer.  ENTRY" );
    References references = DataManager::getDataManager().getCallReference( data.m_destination );
    DataManager::getDataManager().setLastReceivedPacket( data.m_destination, data.m_data );
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                 "Received data to transfer.  EXIT" );
    return references; 
}


/**
* Connection failure.  If primary then switch to seconday is required.
* @param    isPrimary
*/
void Radio::onConnectionFail(bool isPrimary)
{
    // Not used in this unit test.
}


/**
* Connection resumed.
*/
void Radio::onConnectionResumed(bool isPrimary)
{
    // Not used in this unit test.
}



/**
* This method will receive the incoming events from the radio system.  
* It will notify waiting RadioSession of a new event through the RadioTransactionManager.  
* It will process all unsolited events.
* @param    event
*/
void Radio::onReceiveEvent(TA_IRS_App::RadioEvent& event)
{
    // Not used in this unit test.
}


//void Radio::sessionLoggedOut(unsigned long sessionRef)
void Radio::sessionLoggedOut( ta_uint32  sessionRef)
{
}
