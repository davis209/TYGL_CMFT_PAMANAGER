/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/RadioTetraAgent.h $
  * @author:   Andy Parker
  * @version:  $Revision: #3 $
  *
  * Last modification: $DateTime: 2009/03/19 17:00:25 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(RADIOTETRAAGENT)
#define RADIOTETRAAGENT

#include "core/types/src/ta_types.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"

#include "bus/generic_agent/src/GenericAgent.h"
#include "bus/generic_agent/src/IGenericAgentUser.h"
  //#include "bus/scada/ItaScada/src/CtaScada.h"

#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/scada/datapoint_library/src/DataPointFactory.h"
#include "bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointAgentCorbaDef_Impl.h"
#include "bus/scada/DataPointAgentCorbaDef_Impl/src/DataPointAccessCorbaDef_Impl.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/IDataNodeAgent.h"
#include "bus/scada/common_library/src/ScadaPersistence.h"

#include "app/radio/RadioTetraAgent/src/IRadioTetraAgentState.h"
#include "app/radio/RadioTetraAgent/src/RadioStateSynchronisationServant.h" // TD15926
#include "app/radio/RadioTetraAgent/src/RadioSessionServant.h"  // TD9016

namespace TA_Base_Bus
{
	class ScadaProxyFactory;
	class SummaryProcessor;
	class DataNode;
	class GenericAgent; // Forward Declaration
	class IEntity; // Forward Declaration
	class NotifyEventProcessor;
	class LocalUpdateEventProcessor;
	class DataNodeAgentCorbaDef_Impl;
	class DataNodeAccessCorbaDef_Impl;
}

namespace TA_Base_Core
{
	class IEntityData; // Forward Declaration
	class ConfigUpdateDetails; // Forward Declaration
	struct StateUpdateMessageCorbaDef;
}

namespace TA_IRS_App
{
	class RadioServant;
	class CallGenerator;
	class RadioSessionServant; // TD14696
	class RadioStateSynchronisationServant;

	class RadioTetraAgent : public TA_Base_Bus::IGenericAgentUser,
		public IRadioTetraAgentState
		//public TA_Base_Bus::IDataNodeAgent,
		//public TA_Base_Bus::IDataPointAgent
	{
		//typedef std::vector< TA_Base_Bus::DataNode* > DataNodeList;
		//typedef std::map< ta_uint32 , TA_Base_Bus::DataPoint* > DatapointList;

	public:
		/**
		  * Constructor
		  *
		  * @param The number of command line arguments passed in
		  * @param The command line
		  *
		  * @exception GenericAgentException thrown if GenericAgent fails to
		  *            initialise.
		  */
		RadioTetraAgent(int argc, char* argv[]);

		/**
		  * Destructor
		  */
		virtual ~RadioTetraAgent();

		/**
		  * agentTerminate
		  *
		  * This method is called by the GenericAgent when it is time to
		  * terminate.
		  */
		void agentTerminate();

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

		/**
		  * agentSetMonitor
		  *
		  * This method is called by GenericAgent when the Alarm Agent operating mode is
		  * set to Monitor.
		  */
		void agentSetMonitor();

		/**
		  * agentSetControl
		  *
		  * This method is called by GenericAgent when the Alarm Agent operating mode is
		  * set to Control.
		  */
		void agentSetControl();

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
		void registerForStateUpdates();

		/**
		  * receiveSpecialisedMessage
		  *
		  * This method is called whenever the Agent receives a state update or
		  * state update request (depending on their mode of operation)
		  */
		void receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message);

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

		void updateLocalRadio();

		//********************************************
		// IRadioTetraAgentState interface functions *
		//********************************************
		virtual void updateCallInRadioCallStack(const CallDetailsType& callDetails);
		virtual void removeCallFromRadioCallStack(CallID callID, ta_uint32 consoleID);
		virtual void updateRadioSession(ta_uint32 servantEntityKey,
			const RadioSessionType& radioSession);
		virtual void updateRadioMonitoredCallProgression(ta_uint32 monitorReference);
		virtual void removeMonitoredSubscriberDetails(ta_uint32 monitorReference, ta_int32 serverIndex);
		virtual void removeSessionFromAudioMap(SessionID sessionReference, ta_int32 serverIndex);
		virtual void updateAudioEntryInSessionMap(SessionID sessionReference,
			AudioReference audioReference,
			const AudioEntryType& audioEntry);
		virtual void updateRadioResourceManagerVars(SessionID sessionReference, CallReference callReference,
			int currentVolume, ta_int32 serverIndex);
		virtual void updateAuthorisationRequest(const AuthorisationRequestType& authorisationRequest, ta_int32 serverIndex);
		virtual void removeAuthorisationRequest(CallID callID, ta_int32 serverIndex);

		virtual void updateCallStackHistory(const CallStackHistoryList& callStackHistoryList, bool isRetrieved);
		virtual void addCallStackEntry(std::string profile, ta_uint32 locationKey, time_t timestamp, EOriginType originType, EStackCallType callType, std::string identifier, std::string location, bool isEmergency, std::string tsi);
		virtual void deleteCallStackEntry(time_t timestamp, std::string tsi);

		virtual void clearFullState();
		virtual void applyFullState(const RadioTetraAgentType& state);
		virtual void getFullState(RadioTetraAgentType& state);
		virtual void notifyStateUpdatesComplete();

		virtual void updateConnectionSwitch(EControlConnectionStatus controlConnectionStatus, ta_uint32 TcpServerIndex);

		// TD9016
		virtual RadioServant* getRadioServant() { return m_radioServant; };
		virtual std::vector<RadioSessionServant*> getRadioSessionServants() { return m_radioSessions; };
		// TD9016

		virtual void updateSubscriber(const SubscriberStateUpdate& subscriberUpdate);
		
		//********************************************
		//     IDataNodeAgent interface functions    *
		//********************************************
		//virtual TA_Base_Bus::DataNode * getDataNode (ta_uint32 entityKey );
		//virtual void getAllDataNodes (std::map < ta_uint32 , TA_Base_Bus::DataNode *>& dataNodeList );

		//********************************************
		//     IDataPointAgent interface functions   *
		//********************************************
		//virtual TA_Base_Bus::DataPoint * getDataPoint (ta_uint32 entityKey );
		//virtual void getAllDataPoints (std::map < ta_uint32 , TA_Base_Bus::DataPoint *>& dataPointList );
		//virtual TA_Base_Bus::DataPointFactory * getDataPointFactory ();

	private:
		//TD18706,jianghp, handle the operation mode
		virtual void checkOperationMode() {};
		/**
		  * registerForControlMessages
		  *
		  * This method registers for the messages we expect to receive while in
		  * control mode.
		  */
		void registerForControlMessages();

		void getSessionsForRadioLogin();

		/**
		  * registerForMonitorMessages
		  *
		  * This method registers for the messages we expect to receive while in
		  * monitor mode.
		  */
		void registerForMonitorMessages();


		/**
		  * receiveRequestForStateUpdateMsg
		  *
		  * This method is called we receive a request for
		  * state information (this should only happen if we are in Control mode)
		  */
		void receiveRequestForStateUpdateMsg();

		/**
		  * setInitialOperationMode
		  *
		  * set the inition operation mode for agent and child entities
		  */
		void setInitialOperationMode();
		void agentSetControlInThread();//limin, failover


		/**
		  * poll
		  *
		  * spawn new thread for agents not based in OCC to request for subscriber updates
		  */
		void poll();

		// Disable copy constructor and assignment operator
		RadioTetraAgent(const RadioTetraAgent& theAgent);
		RadioTetraAgent& operator=(const RadioTetraAgent &);

		// The Generic Agent
		TA_Base_Bus::GenericAgent* m_genericAgent;
		RadioServant*         m_radioServant;
		CallGenerator*        m_callGenerator;

		enum AgentMode
		{
			INITIAL,
			MONITOR,
			CONTROL
		};

		AgentMode m_agentMode;

		bool m_fullState;

		TA_Base_Core::ReEntrantThreadLockable m_lockable;

		// List of all radio sessions
		std::vector<TA_IRS_App::RadioSessionServant*> m_radioSessions;
		TA_IRS_Bus::MultiPageSdsUtils* m_sdsUtils;
		RadioStateSynchronisationServant* m_newRSSServant; // TD15926

		std::vector<TA_Base_Core::RadioSessionEntityDataPtr>  m_radioSessionData;

		//TA_Base_Bus::DataPointFactory*					m_dataPointFactory;
		//TA_Base_Bus::ScadaProxyFactory*					m_scadaProxyFactory;
		TA_Base_Bus::SummaryProcessor*					m_summaryProcessor;
		//TA_Base_Bus::ScadaPersistencePtr				m_persistence;

		//TA_Base_Bus::DataPointAgentCorbaDef_Impl*		m_dpBulkPolledInterface;
  //      TA_Base_Bus::DataPointAccessCorbaDef_Impl *		m_dpAccessInterface;
		//TA_Base_Bus::DataNodeAgentCorbaDef_Impl *		m_dnBulkPolledInterface;
		//TA_Base_Bus::DataNodeAccessCorbaDef_Impl *		m_dnAccessInterface;

		//TA_Base_Core::ReEntrantThreadLockable			m_dataNodeListLock;
		//TA_Base_Core::ReEntrantThreadLockable			m_dataPointListLock;
		//DataNodeList									m_dataNodes;
		//DatapointList									m_dataPointList;

		std::string										m_entityname;
		std::string										m_OCCentityname;

		time_t m_lastUpdateTime;
		time_t m_svrTimeOffset;
		time_t m_elapsedTime;
		time_t m_synchronisationFrequency;
		bool m_isTerminated;
		//TA_Base_Core::RadioDirectoryEntityDataPtr* radioDirectoryData;

		typedef TA_Base_Core::NamedObject<TA_Base_Bus::IRadioCorbaDef,
			TA_Base_Bus::IRadioCorbaDef_ptr,
			TA_Base_Bus::IRadioCorbaDef_var> RadioNamedObject;
		RadioNamedObject m_OCCRadioTetraAgent;
		TA_Base_Bus::DataPoint* m_linkStatusDataPoint;
	};
};

#endif // !defined(RADIOTETRAAGENT)
