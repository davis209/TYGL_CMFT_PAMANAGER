#ifndef RadioFullStateReceiver_h
#define RadioFullStateReceiver_h
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/RadioFullStateReceiver.h $
  * @author:  John Dalin
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * The RadioFullStateReceiver is a class that is used to retrieve the full state
  * from the peer Radio Tetra Agent that is in control mode. It applies the full state
  * onto the Radio Tetra Agent.
  */

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/Condition.h"
#include "app/radio/RadioTetraAgent/src/IRadioTetraAgentState.h"
#include "app/radio/RadioTetraAgent/src/IRadioTetraAgentPeerState.h"
#include "app/radio/RadioTetraAgent/src/RadioStateUpdateReceiver.h"

namespace TA_IRS_App
{

    class RadioFullStateReceiver : public TA_Base_Core::Thread
    {
        public:
            /**
              * Constructor
              *
              * @param radioState - The radio agent state of this process
              * @param suReceiver - The state update message receiver
              */
            RadioFullStateReceiver(IRadioTetraAgentState& radioState,
                                   IRadioTetraAgentPeerState& radioPeerState,
                                   RadioStateUpdateReceiver& suReceiver);
            
            /**
              * Destructor
              */
            virtual ~RadioFullStateReceiver();
            
            /**
              * run
              * 
              * The thread run method.
              */
            virtual void run();
            
            /**
              * terminate
              *
              * The thread terminate method.
              */
            void terminate();

            
            /** startAcquisition
              * Starts acquiring the full state from the peer agent.
              * The Radio Full State Receiver will continue to acquire the full state
              * from the peer agent until it is successful or the stopAcquisition
              * function is called.
              */
            void startAcquisition();

            /** stopAcquisition
              * Stops acquiring the full state from the peer agent if it is currently
              * doing so.
              */
            void stopAcquisition();

            /** waitForAcquisition
              * Waits until the RadioFullStateReceiver has completed or failed
              * a full state acquisition. If the acquisition has failed, the
              * FullStateReceiver will continue to acquire the state until it
              * is successful. If acquisition is no longer necessary call stopAcquisition.
              * e.g. if (!receiver.waitForAcquisition())
                            stopAcquisition();
              * @return True: acquisition successful, False, acquisition failed
              */
            bool waitForAcquisition();

            /**
              * isAcquiring
              *
              * @return Is the Radio Full State Receiver currently acquiring the state?
              */
            bool isAcquiring();

            /**
              * hasAcquired
              *
              * @return Has the Radio Full State Receiver acquired the state?
              */
            bool hasAcquired();

        private:

            bool m_terminate; // for terminating the thread

            // Is the FullStateReceiver currently acquiring the full state?
            bool m_acquiring;

            // Since calling startAcquisition, has the full state been acquired?
            bool m_acquired;

            IRadioTetraAgentState& m_radioTetraAgentState;

            IRadioTetraAgentPeerState& m_radioTetraAgentPeerState;

            RadioStateUpdateReceiver& m_radioStateUpdateReceiver;

            TA_Base_Core::ReEntrantThreadLockable m_lockable;

            TA_Base_Core::Condition m_runCondition;

            TA_Base_Core::Condition m_waitForAcquisitionCondition;

    }; // class FullStateReceiver
};  // namespace TA_IRS_App

#endif // #ifndef FullStateReceiver_h
