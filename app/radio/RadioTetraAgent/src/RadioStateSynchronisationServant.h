#ifndef RadioStateSynchronisationServant_h
#define RadioStateSynchronisationServant_h
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/RadioStateSynchronisationServant.h $
  * @author:  John Dalin
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * The RadioStateSynchronisationServant is a class that is used for synchronise state data between
  * the Radio Tetra Agent that is in control mode and the Radio Tetra Agent that is in monitor
  * mode.
  */

#include "core/types/src/ta_types.h"
#include "core/naming/src/NamedObject.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/src/BackwardsCompatibility.h"
#include "core/message/IDL/src/StateUpdateMessageCorbaDef.h"
#include "core/corba/src/ServantBase.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/data_access_interface/entity_access/src/RadioStateSynchronisationEntityData.h"
#include "bus/generic_agent/src/IEntity.h"
#include "bus/radio/RadioAgentCorbaDef_4669/idl/src/IRadioStateSynchronisationCorbaDef.h"
#include "app/radio/RadioTetraAgent/src/IRadioTetraAgentState.h"
#include "app/radio/RadioTetraAgent/src/IRadioTetraAgentPeerState.h"
#include "app/radio/RadioTetraAgent/src/RadioStateUpdateReceiver.h"
#include "app/radio/RadioTetraAgent/src/RadioFullStateReceiver.h"
#include "app/radio/RadioTetraAgent/src/Radio.h"
#include "core/data_access_interface/radio/src/RACallStackHistoryAccessFactory.h"

#include <string>

namespace TA_IRS_App
{

    class RadioStateSynchronisationServant : public virtual POA_TA_Base_Bus::IRadioStateSynchronisationCorbaDef,
                                             public virtual TA_Base_Core::ServantBase,
                                             public virtual TA_Base_Bus::IEntity,                                             
                                             public virtual TA_Base_Core::SpecialisedMessageSubscriber< TA_Base_Core::StateUpdateMessageCorbaDef >,
                                             public virtual IRadioTetraAgentPeerState
    {
        public:
            
            /**
              * Constructor
              *
              * @param radioState - The radio agent state of this process
              * @param entityData - Configuration data for this entity
              * @param radio	  - The radio of this this process
              */
            RadioStateSynchronisationServant(IRadioTetraAgentState& radioState,
                TA_Base_Core::RadioStateSynchronisationEntityDataPtr entityData,
				Radio& radio);

            /**
              * Destructor
              */
            ~RadioStateSynchronisationServant();
            void startSubscription();
            
            /** 
             * stopSubscription
             *
             * Unsubscribes to receiving state update messages from
             * the peer Radio Tetra Agent.
             */
            void stopSubscription();

			void getPeerCallStackHistory();
			void getPeerSDSList();

            // ******************************************************
            // IRadioStateSynchronisationCorbaDef interface methods *
            // ******************************************************
            virtual TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioTetraAgentType* requestFullState();
			virtual void setMonitorAgentState(CORBA::Boolean state); // TD9016
			virtual TA_Base_Bus::IRadioStateSynchronisationCorbaDef::RadioCallStackHistoryList* requestCallStackHistory();
			virtual TA_Base_Bus::IRadioStateSynchronisationCorbaDef::PredefinedSDSList* getFullSDSList(bool readWrite);

            // **************************************
            // SpecialisedMessageSubscriber methods *
            // **************************************
            virtual void receiveSpecialisedMessage(
                const TA_Base_Core::StateUpdateMessageCorbaDef& message);

            //**********************************************
            // IRadioTetraAgentPeerState interface methods *
            //**********************************************
            virtual bool getFullPeerState(RadioTetraAgentType& state);
            virtual void applyFullState(const RadioTetraAgentType& state); // TD9016

            //****************************
			// IEntity Interface Methods *
			//****************************
			virtual bool isValid();
			virtual void start();
			virtual void stop();
			virtual void update(const class TA_Base_Core::ConfigUpdateDetails& details);
			virtual void remove();
			virtual void setToControlMode();
			virtual void setToMonitorMode();

			// TD15926
			// This function is called before the agent terminates to determine whether it should 
			// detach session from the radio hardware
			bool isPeerAvailable(); 

        private:
            /** 
              * startSubscription
              *
              * Subscribes to receiving state update messages from
              * the peer Radio Tetra Agent.
              */
            /** 
              * TD9016
              *
              * This function is called when the controlling agent is invoked by the monitoring 
              * to retrieve its full state
              */
            void getFullState(RadioTetraAgentType& state);

            void setToControlModeInternal();//limin, for failover

            // The radio agent state of this process
            IRadioTetraAgentState& m_radioTetraAgentState;

            // Used to receive state update messages from the peer agent
            RadioStateUpdateReceiver m_radioStateUpdateReceiver;

            // Used to retrieve the entire radio agent state from the peer agent 
            RadioFullStateReceiver m_radioFullStateReceiver;

			Radio&          m_radio;

            // Radio agent entity data
            std::string m_objectName;
			ta_uint32 m_entityKey;
			ta_uint32 m_entityLocation;
			ta_uint32 m_entitySubsystem;
			ta_uint32 m_entityTypeKey;

            enum ServantState
            {
                INITIAL,
                CONTROL,
                MONITOR
            };

            ServantState m_state;
            bool m_subscribed;
            
            // The peer radio agent used to request full state
            typedef TA_Base_Core::NamedObject<TA_Base_Bus::IRadioStateSynchronisationCorbaDef,
                                              TA_Base_Bus::IRadioStateSynchronisationCorbaDef_ptr,
                                              TA_Base_Bus::IRadioStateSynchronisationCorbaDef_var> RadioNamedObject;
            RadioNamedObject m_peerRadioTetraAgent;

            TA_Base_Core::ReEntrantThreadLockable m_lockable;

			// TD9016
			bool m_monAgentState;
			// TD9016

    }; // class RadioStateSynchronisationServant
};  // namespace TA_IRS_App

#endif // #ifndef RadioStateSynchronisationServant_h
