#ifndef IRadioTetraAgentPeerState_h
#define IRadioTetraAgentPeerState_h

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/IRadioTetraAgentPeerState.h $
  * @author  John Dalin
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2008/11/28 16:26:01 $
  * Last modified by : $Author: builder $
  *
  * This interface defines the methods used to retrieve the state of the
  * peer Radio Tetra Agent for control/monitor agent synchronisation.
  */

#include "app/radio/RadioTetraAgent/src/RadioSynchronisationTypes.h"

namespace TA_IRS_App
{
	class IRadioTetraAgentPeerState
	{
		public:
            /**
              * getFullPeerState
              *
              * Called to retrieve the peer Radio Tetra Agent state
              *
              * @param state - The state that is to be returned
              * @return True: Successfully returned the state
              *         False: Failed to return the state
              */
			virtual bool getFullPeerState(RadioTetraAgentType& state) = 0;

			// TD9016 Called to apply a full state update on the Radio Tetra Agent
			virtual void applyFullState(const RadioTetraAgentType& state) = 0;

			virtual void getPeerCallStackHistory() = 0;
	};   // class IRadioTetraAgentPeerState
};  // Namespace TA_IRS_App

#endif
