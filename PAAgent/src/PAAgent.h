/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/PAAgent.h $
  * @author:   Andy Parker
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#if !defined(PAAgent_85831B60_1CBB_43c8_A752_5EB30E0DE611__INCLUDED_)
#define PAAgent_85831B60_1CBB_43c8_A752_5EB30E0DE611__INCLUDED_

#include "BroadcastSchedulerService.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include "bus/generic_agent/src/IGenericAgentUser.h"

  // Forward declarations.
namespace TA_Base_Bus
{
	class GenericAgent;
}

namespace TA_IRS_App
{
	class PAAgent : public virtual TA_Base_Bus::IGenericAgentUser
	{
	public:

		PAAgent(int argc, char* argv[]);

		virtual ~PAAgent();

		/**
		  * agentTerminate
		  *
		  * This method is called by the GenericAgent when it is time to
		  * terminate.
		  */
		virtual void agentTerminate();

		/**
		  * createAllEntities
		  *
		  * [optional]
		  *
		  * This allows for bulk entity creation.
		  * It is called first (before any calls to createEntity) to allow agents to
		  * optimise entity creation. If true is returned, the entities are already created
		  * if false is returned, then createEntity is called one by one for each entity.
		  * Also entityCreationComplete will be called at the end of the creation
		  *
		  * Note that even if using this method, createEntity should still be implemented
		  * as it is used for entity updates
		  *
		  * @param agentEntityData the agent's entity data
		  * @param entityDataList a list of shared pointers to all entities
		  * @param createdEntities the entities created to return to generic agent
		  *
		  * @return true if entity creation was done, false if createEntity should be used
		  *
		  */
		virtual bool createAllEntities(TA_Base_Core::IEntityDataPtr agentEntityData,
			const TA_Base_Core::SharedIEntityDataList& entityDataList,
			TA_Base_Bus::IEntityList& createdEntities);

		/**
		  * entityCreationComplete
		  *
		  * [optional]
		  *
		  * this is called back after
		  * createEntity has been called for all entities.
		  * The client can do any clean up of temporary data.
		  */
		virtual void entityCreationComplete();

		/**
		  * createEntity
		  *
		  * This method is called by the GenericAgent to allow the child entities to be
		  * created.  These entities are then managed by the generic agent.
		  *
		  * @param Entity data retrieved from the database.
		  *
		  * @return The constructed entity
		  */
		virtual TA_Base_Bus::IEntity* createEntity(TA_Base_Core::IEntityDataPtr entityData);
		//throw(TA_Base_Core::InvalidPasConfigurationException);


	/**
	  * agentSetMonitor
	  *
	  * This method is called by GenericAgent when the Alarm Agent operating mode is
	  * set to Monitor.
	  */
		virtual void agentSetMonitor();


		/**
		  * agentSetControl
		  *
		  * This method is called by GenericAgent when the Alarm Agent operating mode is
		  * set to Control.
		  */
		virtual void agentSetControl();

		/**
		  * notifyGroupOffline
		  *
		  * This request is passed on by GenericAgent when it receives a call to
		  * notifyGroupOffline from the Process Monitor.
		  *
		  * @param The group name
		  */
		virtual void notifyGroupOffline(const std::string& group);

		/**
		  * notifyGroupOnline
		  *
		  * This request is passed on by GenericAgent when it receives a call to
		  * notifyGroupOnline from the Process Monitor.
		  *
		  * @param The group name
		  */
		virtual void notifyGroupOnline(const std::string& group);

		/**
		  * registerForStateUpdates
		  *
		  * This pure virtual method forces agents to subscribe for either
		  * state updates or state update requests, depending on whether or not they
		  * are operating in monitor or control mode.
		  */
		virtual void registerForStateUpdates();


		/**
		  * receiveSpecialisedMessage
		  *
		  * This method is called whenever the Agent receives a state update or
		  * state update request (depending on their mode of operation)
		  *
		  */
		virtual void receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message);

		/**
		  * receiveSpecialisedMessage
		  *
		  * This method is called whenever the Monitor Agent receives a datapoint state update
		  *
		  */
		  //virtual void receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message); //limin, failover issue

		  /**
			* processOnlineUpdate
			*
			* This is called by Generic Agent whenever it gets an online update that it
			* cant handle itself.
			*
			* @param The update event
			*/
		void processOnlineUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent);

		/**
		  * run
		  *
		  * This method calls GenericAgent::run() which blocks until the System
		  * Controller shuts us down
		  */
		void run();

	protected:
		//TD18706,jianghp, handle the operation mode
		virtual void checkOperationMode() {};

	protected:


		TA_Base_Bus::GenericAgent*               m_genericAgent;
		TA_Base_Core::EOperationMode             m_operationMode;
		TA_IRS_App::BroadcastSchedulerService*   m_schedulerService;

		static const std::string MONITORED_PROCESS_NAME_APPEND;
		static const std::string AGENT_NAME_APPEND;
	};

} // namespace TA_IRS_App

#endif // !defined(PAAgent_A7EE4E27_1FF9_4bb8_BBB7_6B1F2C675F52__INCLUDED_)

