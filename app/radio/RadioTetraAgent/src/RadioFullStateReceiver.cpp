/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/RadioFullStateReceiver.cpp $
  * @author  John Dalin
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2008/11/28 16:26:01 $
  * Last modified by : $Author: builder $
  *
  * The RadioFullStateReceiver is a class that is used to retrieve the full state
  * from the peer Radio Tetra Agent that is in control mode. It applies the full state
  * onto the Radio Tetra Agent.
  *
  */

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/radio/RadioTetraAgent/src/RadioFullStateReceiver.h"

namespace TA_IRS_App
{

    RadioFullStateReceiver::RadioFullStateReceiver(IRadioTetraAgentState& radioState,
                                                   IRadioTetraAgentPeerState& radioPeerState,
                                                   RadioStateUpdateReceiver& suReceiver)
                                                   : m_terminate(false)
                                                   , m_acquiring(false)
                                                   , m_acquired(false)
                                                   , m_radioTetraAgentState(radioState)
                                                   , m_radioTetraAgentPeerState(radioPeerState)
                                                   , m_radioStateUpdateReceiver(suReceiver)
                                                   
    {
        FUNCTION_ENTRY("Constructor");

        start();

        FUNCTION_EXIT;
    }
    
    RadioFullStateReceiver::~RadioFullStateReceiver()
    {
        FUNCTION_ENTRY("Destructor");

        terminateAndWait();

        FUNCTION_EXIT;
    }
    
    void RadioFullStateReceiver::run()
    {
        FUNCTION_ENTRY("run");

        RadioTetraAgentType state;

        while (!m_terminate)
        {
            bool acquiring;

            {
                TA_Base_Core::ThreadGuard guard(m_lockable);

                acquiring = m_acquiring;

                if (acquiring)
                {
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "***** START ACQUISITION *****" );
                    // Get the state from the peer agent
                    if (m_radioTetraAgentPeerState.getFullPeerState(state))
                    {
						LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "applyFullState" );
                        // if state retrieval did not cause an error then apply state to the agent
                        //m_radioTetraAgentState.applyFullState(state); 
						
						// TD9016 move applyFullState() to IRadioTetraAgentPeerState to prevent deadlock in RadioTetraAgent class
						m_radioTetraAgentPeerState.applyFullState(state);
                        m_acquired = true;
                        m_acquiring = false;
                        m_radioStateUpdateReceiver.resumeProcessing();
                    }
					else 
					{
						LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Failed to get full peer state" );
						m_acquiring = false;
					}
                    // wake up threads waiting on acquisition completion or failure
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "wake up threads waiting on acquisition completion or failure" );
                    m_waitForAcquisitionCondition.broadcast();
                }
            }

            if (!acquiring)
            {
                m_runCondition.wait();
                continue;
            }
        }

        FUNCTION_EXIT;
    }
    
    void RadioFullStateReceiver::terminate()
    {
        FUNCTION_ENTRY("terminate");

        TA_Base_Core::ThreadGuard guard(m_lockable);

        m_terminate = true;
        m_runCondition.signal();
        m_waitForAcquisitionCondition.broadcast();

        FUNCTION_EXIT;
    }
    
    void RadioFullStateReceiver::startAcquisition()
    {
        FUNCTION_ENTRY("startAcquisition");

        TA_Base_Core::ThreadGuard guard(m_lockable);

        if (!m_acquiring)
        {
            m_acquiring = true;
            m_runCondition.signal();
        }

        FUNCTION_EXIT;
    }
    
    void RadioFullStateReceiver::stopAcquisition()
    {
        FUNCTION_ENTRY("stopAcquisition");

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "***** STOP ACQUISITION *****" );
        TA_Base_Core::ThreadGuard guard(m_lockable);

        if (m_acquiring)
        {
            m_acquiring = false;
        }

        FUNCTION_EXIT;
    }
    
    bool RadioFullStateReceiver::waitForAcquisition()
    {
        FUNCTION_ENTRY("waitForAcquisition");

        if (!m_terminate)
        {
            bool acquiring;

            {
                TA_Base_Core::ThreadGuard guard(m_lockable);

                acquiring = m_acquiring;
            }

            if (acquiring)
            {
                // wait until acquisition successful or failed
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "wait until acquisition successful or failed" );
                m_waitForAcquisitionCondition.wait();
            }
        }
        
        {
            TA_Base_Core::ThreadGuard guard(m_lockable);
            
            return m_acquired;
        }

        FUNCTION_EXIT;
    }

    bool RadioFullStateReceiver::isAcquiring()
    {
        FUNCTION_ENTRY("isAcquiring");
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "isAcquiring" );
        TA_Base_Core::ThreadGuard guard(m_lockable);

        return m_acquiring;

        FUNCTION_EXIT;
    }

    bool RadioFullStateReceiver::hasAcquired()
    {
        FUNCTION_ENTRY("hasAcquired");

        TA_Base_Core::ThreadGuard guard(m_lockable);

        return m_acquired;

        FUNCTION_EXIT;
    }
} // namespace TA_IRS_App
