#ifndef RADIO_TCPSERVER_H
#define RADIO_TCPSERVER_H

/**
  * The source code in this file is the property of
  * MI Services Group and is not for redistribution
  * in any form.
  *
  * Source : $Source: /cvs/1TS-9999-T/code/transactive/app/radio/RadioTetraAgent/src/Attic/RadioTcpServer.h,v $
  * @author  R. van Hugten
  * @version $Revision: 1.1.2.15.2.4 $
  * Last modification : $Date: 2007/04/03 05:35:47 $
  * Last modified by : $Author: adamr $
  *
  * Definition of the RadioTcpServer class, which represents the interface
  * to each one of the OTE TETRA TcpServers. This is necessary to handle
  * the uniqueness of callIDs and such in a radio system
  */

#include "core/types/src/ta_types.h"
#include <list>
#include <set>

#include "app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioProtocolLibrary.h"
#include "app/radio/RadioTetraAgent/src/Radio.h"
#include "app/radio/RadioTetraAgent/src/RadioTask.h"
#include "app/radio/RadioTetraAgent/src/RadioTypes.h"
#include "app/radio/RadioTetraAgent/src/IRadioConnectionObserver.h"

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include <algorithm>

// There are two possible values for the strategy
// 1 : wait for all attached sessions to detach before calling resetConnection()
// 2 : call resetConnection() on receipt of the first SystemError with GWLDLINKLOST
// The second seems condoned by the TcpServer spec but given previous experience,
// i don't trust it - the TcpServer may still send such an unsolicited event to
// a session that has detached before it received the SystemError(-20). If this
// is received after Attach is performed, it will cause havoc.
#define WAITALLDETACH 1
#define ONFIRSTERROR 2
#define GWLDLINKLOST_RESET_STRATEGY WAITALLDETACH
// #define GWLDLINKLOST_RESET_STRATEGY ONFIRSTERROR
#define UNKNOW_LOCATION -1

namespace TA_Base_Bus
{
    class ItaScada;
}

namespace TA_IRS_App
{
//   class Radio;
	class RadioSession;
	class IRadioCallback;
    // these should be in RadioProtocol.h

    class RadioSubscriberMonitor;
    class RadioConnectionManager;
    class RadioTransactionManager;
    class RadioResourceManager;
    class RadioAuthorisationManager;
    class RadioAlarmsManager;

    // state synchronisation forward declarations
    struct TcpServerType;
    struct AudioEntryType;


	class RadioTcpServer : public IRadioConnectionStatusObserver,
						   public IRadioEventProcessor
	{

	public:
		static std::string getKeyForSessionCallID(CallID callid,SessionReference sessionRef);
		RadioTcpServer( IRadioCallback & callback,
                        ServerIndex sIndex,
                        Radio & radio,
                        //TA_Base_Bus::ItaScada& scada,
                        const std::string & primaryAddress,
                        const std::string & secondaryAddress,
                        unsigned char serialiseTransactionLevel,
                        ta_uint32 radioTransactionTimeout
                        );

		virtual ~RadioTcpServer();

		void setEntity(TA_Base_Core::IEntityData& entityData);

		//the interface for IRadioConnectionStatusObserver
		virtual void onConnectionEstablish();
		virtual void onConnectionLost();
		virtual void onSwitchConnection();
		
        /**
         * Returns the callback instance.
         */
        IRadioCallback& getRadioCallback();

        void start();

		/**
		* Activates this server class and the connectionmanager
		*/
		void setToControlMode(void);

		/**
		* De-activates this server class and the connectionmanager
		*/
		void setToMonitorMode(void);

        void onTerminate(bool peerAvailable);
        
        /**
         * Accessor for the index of this server in the overall radio array
         */
        ServerIndex getServerIndex() const;
		 ServerIndex getCRSNumber() const;

		/**
		* This function returns the radio resource manager
		*/
		RadioResourceManager& getRadioResourceManager();

        /** Accessor for the RadioSubscriberMonitor */
        RadioSubscriberMonitor & getSubscriberMonitor();

		/**
		* This method sends a method request to the radio system and waits for a reply (event).  It will be used by the RadioSession to perform its operations.
		*
		* @param    Method
		*/
		RadioEvent * sendMethod(RadioMethod& Method);
		
		// to fix the deleting reference issue
		void sendMethodNoReply (RadioMethod& Method);

		/**
         * sendMethodMultipleResponse
         *
		 * This method sends a method request to the radio system and waits for as many replys as the radio system
         * sends. The only difference between this method and the standard sendMethod is that send method will
         * only ever return a single event.
         *
		 * @param    Method The method to send
         *
         * @return A vector of the events recieved in response to the original message. A vector of size 0 
         * means that no responses were sent.
		 */
        std::vector<RadioEvent*> sendMethodMultipleResponse(RadioMethod& Method);

        /**
         * Called by the RadioServant, in turn by the RadioManager and CallBanner
         * when they are initialising, to retrieve all the current calls.
         *
         */
		void getCallList(CallList& callList);

        /** retrieves a list of calls that the given tsi is involved in. 
         * TD12462 In fallback, cannot send SDS to train if audio already established
         */
		void getTsiCallList(const std::string & tsi, bool audioOnly, CallList & callList);
		
        /**
         * Retrieve ALL the details for a call
         */
		bool getCallDetails(CallID callId, CallDetailsType& details);

        /**
		 * Like getCallDetails, but doesn't copy across the CallParticpants
		 * vector. Is a seperate function because different copying logic is used
         */
		bool getCallDetailsPartial(CallID callId, CallDetailsType& details);

		/**
		* Indicates if the selected call is an SDS call  
		*/
		bool isSDSCall(CallID callId);


		/**
		* createCompatibleTcpEvent. Create a TcpEvent that is compatible with this type of observer 
		* @param    isPrimary
		*/
		virtual ITcpEvent * createCompatibleTcpEvent();


		/**
		* This method will receive the incoming events from the radio system.  It will notify waiting RadioSession of a new event through the RadioTransactionManager.  It will process all unsolited events.
		* @param    event    event
		*/
		virtual void onReceiveEvent(TA_IRS_App::ITcpEvent& itcpEvent, time_t timeStamp = time(0));

        /**
         *
         */
        // TCPSERVER TRANSFER NOT DONE
        virtual void sessionLoggedOut(SessionReference sessionRef);
        virtual void sessionLoggedIn(SessionReference sessionRef);
        
        /**
         * @param session the RadioSession to add to the internal list
         */
        void addSession(RadioSession * session);

        /**
         * @param session the RadioSession to add to the internal list
         */
        void onSessionUpdate(SessionReference newSessionRef, SessionReference oldSessionRef);
        
        // state synchronisation functions
        void applyFullState(const TcpServerType& state);

		/**
		  * <description>
		  * 
		  * @return void 
		  * 
		  * @exception <exceptions> Optional
		  */
        void clearFullState();

		/**
		  * <description>
		  * 
		  * @return void 
		  * @param : RadioType& state
		  * 
		  * @exception <exceptions> Optional
		  */
        void getFullState(TcpServerType& state);

        /**
         * state synchronisation function
         * TODO: rename to more effectively indicate state-sync 
         */
        void updateCallInRadioCallStack(const CallDetailsType& callDetails);

        /**
         * state synchronisation function
         * TODO: rename to more effectively indicate state-sync 
         * NOTE: these statesync functions are the "sink" of the info.  The
         * "source" functions are also in this class, usually named without
         * the "Radio" portion in the name
         */
        void removeCallFromRadioCallStack(CallID callID);

        /**
         * state synchronisation function
         * TODO: rename to more effectively indicate state-sync 
         */
		void removeSessionFromAudioMap(SessionID sessionReference);

        /**
         * state synchronisation function
         * TODO: rename to more effectively indicate state-sync 
         */
		void updateAudioEntryInSessionMap(SessionID sessionReference, 
										  AudioReference audioReference,
										  const AudioEntryType& audioEntry);

        /**
         * state synchronisation function
         * TODO: rename to more effectively indicate state-sync 
         */
		void updateRadioResourceManagerVars(SessionID sessionReference, CallReference callReference,
											int currentVolume);

        // Fallback & Degraded IDL support

        /**
         *
         */
        virtual bool inFallback() const;

        /**
         * delegates to parent Radio
         */
        std::string getFallbackReplacementString();

        /**
         *
         */
        virtual void resetConnection();

		/**
         *
         */
		virtual void resetSocket();

		/**
         *
         */
        virtual void resetSession(ta_uint32 sessionRef);


		/**
		* Add a new call to the call stack
		* @param    call
		* Updated by Raghu
		*/
		void addCallToCallStack(CallDetailsType& call);
        
        /**
		* If a call is already existing in the call stack this updates its details
		* @param    call
		* Updated by Raghu
		*/
		void updateExistingCallInCallStack(CallDetailsType& call);
		
		/**
		* Update the status of a call in the call stack
		* @param    callId
		* @param	status
		* @param    sessionRef
		
		* Updated by Raghu
		*/
		bool updateCallStatusInCallStack(CallID callId, ECallStatusType status,SessionReference sessionRef, bool isTrain=false);
		
		/**
		* Removes a call from the radio call stack
		* @param    callId
		* @param    sessionRef
		* Updated by Raghu
		*/
		bool removeCallFromCallStack(CallID callId,SessionReference sessionId, bool isTrain=false);
		
		bool removeCallFromCallStack(CallID callId,bool isTrain=false);
        
        /**
         * 
         */
        Radio & getRadio();

        /**
         * 
         */
        RadioAuthorisationManager & getAuthorisationManager();

		/**
		 * finds the appropriate session to send notify change through
		 */
        void onAuthorisationChange(const TA_IRS_App::EAction action,const TA_IRS_App::AuthorisationRequestType& details);

        /**
         * 
         */
        RadioSession * findSession(SessionReference sessionRef);

		/* Synchronization functions
		*/
		void removeMonitoredSubscriberDetails(ta_uint32 monitoredReference);

		void updateAuthorisationRequest(const AuthorisationRequestType& authorisationRequest);
		void removeAuthorisationRequest(CallID callID);

		void updateControlConnectionState(EControlConnectionStatus controlConnectionStatus );
		/**
		 * isResetConnectionInProcess
		 *
		 * Method used to determine if there is an ongoing Reset Connection request
		 *
		 * @return bool : True if there is an going request else False
		 */
		bool isResetConnectionInProcess();


		bool isResetSocketInProcess();

		/**
		 * setResetConnectionInProcess
		 *
		 * Method used to set the flag for Reset Connection request
		 *
		 */
		void setResetConnectionInProcess();

		/**
		 * clearResetConnectionInProcess
		 *
		 * Method used to clear the flag for Reset Connection request
		 *
		 */
		void clearResetConnectionInProcess();

		/**
		 * isResetSessionRefInProcess
		 *
		 * Method used to determine if there is an ongoing Reset Session Reference request
		 *
		 * @param SessionReference : The sessionRef that has on going process 
		 * @return bool : True if there is an going request else False
		 */
		//bool isResetSessionRefInProcess(SessionReference sessionRef);

		/**
		 * doResetSession
		 *
		 * Method used to reset specific Session
		 *
		 */
		void doResetSession(unsigned long sessionRef);

		TA_IRS_App::CallDetailsType findActiveCall(const std::string& groupTSI);
    private:

		/**
		 * Handler for unsolicited events (e.g. incoming text message, incoming call)
		 * @param    event
		 */
		void onUnsolicitedEvent(RadioEvent& event, time_t timeStamp);
		/**
		 * Handler for solicited events - This will send them to the RadioTransactionManager for processing
		 * @param    event
		 */
		void onSolicitedEvent(RadioEvent& event, time_t timeStamp);

		void onUnhandledSolicitedEvent(RadioEvent& event);

        /**
         * 
         */
		void onIncomingVoiceCall(const IncomingCallEventReader& event);

		/**
		 * Processes unsolicited GroupCallAck events
		 */
        void onGroupCallAck(const GroupCallAckEventReader & eventReader,SessionReference sessionRef);

		/**
		 * Determine SDS type, if textual add to incoming call stack and notify, if train message pass to onTrainEvent
		 */
		void onIncomingSDS(const TA_IRS_App::IncomingSDSEventReader& eventReader);

		void onIncomingUserDefineSDS(const TA_IRS_App::IncomingUserDefinedSDSEventReader& eventReader);

		void onIncomingSimpleSDS(TA_IRS_App::IncomingSimpleTextSDSEventReader& eventReader);

		void onIncomingTLSDS(TA_IRS_App::IncomingSDSTLTransferEventReader& eventReader);
		//++ Added to support User Defined SDS
		void onIncomingUserDefinedSDS(const TA_IRS_App::IncomingUserDefinedSDSEventReader& eventReader);

        /**
		 * IncomingCircuitData Event handler. Delegates to the Radio class
		 */
		void onIncomingCircuitData(const TA_IRS_App::IncomingCircuitDataEventReader& eventReader);

		/**
         * Delegates to the Radio class
		 */
		void onCircuitDataCapacity(const TA_IRS_App::CircuitDataCapacityEventReader& eventReader);

		/**
		 * An incoming sds query message.
		 */
		void onSDSQueryMessage(const TA_IRS_App::IncomingSDSEventReader& eventReader, RadioSession & radsess);

		/**
		 * An incoming text message.
		 */
		void onTextMessage(const TA_IRS_App::IncomingSDSEventReader& eventReader);

		/**
		 * Called to process CeaseTX and DemandTX events
		 */
		bool onTXEvent(TA_IRS_App::RadioEvent& event);

		bool onPTTBtnEvent(TA_IRS_App::RadioEvent& event);

		void onDisconnect(DisconnectEventReader * disconnectReader, ta_uint32 sessionRef, time_t timeStamp);

		/**
		 * This method will handle system error messages from the radio equipment.
		 */
		void onSystemError(TA_IRS_App::RadioEvent & event, SystemErrorEventReader * seReader);

		/**
		 * This method will handle system error messages from the radio equipment.
		 */
		void onDetachSession(TA_IRS_App::RadioEvent & event, DetachSessionEventReader * seReader);

		/*
		* Processes unsolicited SubscriberActivity events
		*/
        void onSubscriberActivity(TA_IRS_App::RadioEvent & event, SubscriberActivityEventReader * saReader);

		
		/*
		* Processes unsolicited MonitorSubscriber events
		*/
        // TCPSERVER TRANSFER DONE
        void onMonitorSubscriber(TA_IRS_App::RadioEvent & event, MonitorSubscriberEventReader * msReader);
		/**
		 * This method will handle changes to radio fall back mode.  Some functions will become 
		 * unavailable therefore applications must be notified.  An alarm should be raised/closed 
		 * to alert the operator.
		 */
        void onRadioFallback(bool isFallback, EFallbackReason fallbackReason);

		/**
		 * Notify subscribers of a change of status to a call in the call stack
		 */
		void sendCallStatusChange();

        /**
         * 
         */
        void addBackgroundTask(RadioTaskPtr task, bool queued = false);

        /**
         * 
         */
        void addPendingGroupCallAck(TA_IRS_App::CallDetailsType& pendingDetails);

        /**
         * 
         */
        void doResetConnection();

		void doResetSocket();


		bool isResetSessionInProcess (ta_uint32 Ref );

        /**
         * This method will create a new threadguard for the RadioSessionReference in the 
		 * m_sessionSerialisationThreadguardMap if it doesnt already exist
         */
        void addRadioSessionToThreadguardMap(RadioMethod::RadioSessionReference radioSessionRef);

		void onCallbackCallStatusChange(TA_IRS_App::CallDetailsType& details);

		//add by HuangQi     CL-20772
		//Modified by Raghu. To accept secondparameter SessionRef
		bool addCallToPendingAddtionList(CallID callId,SessionReference sessionRef);

		void addPendingSystemErrorDetach(SessionReference sessionRef);
		void removePendingSystemErrorDetach(SessionReference sessionRef);
		ConsoleID findConsoleForSession(SessionReference sessionRef);
		void RadioTcpServer::removeCallbySessionRef(SessionReference sessionRef);
		const std::string getEventEntityName(TA_IRS_App::RadioEvent& event);

		bool isValidCallDisconnectEvent(DisconnectEventReader* disconnectReader, time_t timeStamp);
		void onIncludeCallNotifcation(SessionReference sessionRef);

    private:
        ///
        /// transaction manager for this server
		RadioTransactionManager               * m_transactionManager;

        ///
        /// connection manager for this server
		RadioConnectionManager                * m_connectionManager;

        ///
        /// audio resource manager for this server
		RadioResourceManager             * m_audioResourceManager;

        ///
        /// manages CAD requests for this server
		RadioAuthorisationManager             * m_authorisationManager;

        ///
        /// coordinates the Subscriber Monitoring activities
		RadioSubscriberMonitor                * m_subscriberMonitor;
		
        ///
        /// callback for sending notifications of connection failure
		IRadioCallback&							m_callback;

        /// 
        /// type definition for the stack of calls
		typedef std::vector< TA_IRS_App::CallDetailsType > CallStack;

        /// 
        /// 
		std::vector<TA_IRS_App::CallDetailsType>	m_radioCallStack;

        /// 
        /// 
		TA_Base_Core::ReEntrantThreadLockable		m_callStackLock;

        ///
        ///
        typedef std::map< std::string, TA_IRS_App::CallDetailsType> CallDetailsMap;

        ///
        /// When a group call Ack is made or incoming, there are two sets of
        /// details that need to be gathered to put together - 
        /// - the details of the call itself (this data member)
        /// - the details of the participants (the GroupCalAckTsiVectorsMap)
        CallDetailsMap                          m_pendingGCADetails;
        /// typedef for a map of vectors of TSIs of GroupCallAck participants,
        /// key'd by callID

        typedef std::map< std::string, TSIVector > GroupCallAckTsiVectorsMap;

        GroupCallAckTsiVectorsMap               m_pendingGCATsiVectors;

        ///
        /// threadlock for the two maps involved in GroupCallAck details
        TA_Base_Core::ReEntrantThreadLockable   m_groupCallAckLock;

        ///
        /// type definition for a set of RadioSessions
        typedef std::set< RadioSession * >      RadioSessionSet;

        ///
        /// our child radio sessions
        RadioSessionSet                         m_radioSessions;

		// This is a work around for threading
		// if a call is added then removed quickly it will actually be added after it is removed
		std::list<std::string> m_radioCallsPendingDeletion;
		std::list<std::string> m_radioCallsPendingAddition;

        ///
        /// our index in the scheme of servers (range=0..n)
        ServerIndex                             m_serverIndex;

        ///
        /// our parent radio instance
        Radio                                 & m_radio;
		
		typedef std::map<RadioMethod::RadioSessionReference, TA_Base_Core::ReEntrantThreadLockable*> SessionThreadGuardMap;
		// Threadguard to ensure serialisation of method/response
		SessionThreadGuardMap m_sessionSerialisationThreadguardMap;

        //TA_Base_Bus::ItaScada& m_scada;

        RadioAlarmsManager* m_alarmsManager;

        // address of the tcp server status datapoint
        std::string m_datapointAddress;

        unsigned char m_serialiseTransactionLevel;

        ///
        ///
        typedef std::set<SessionReference> SessionReferenceSet;

        ///
        ///
        SessionReferenceSet m_pendingSystemErrorDetachSessions;

        ///
        ///
        TA_Base_Core::NonReEntrantThreadLockable m_pendingSystemErrorDetachSessionsLock;
        volatile bool m_ResetSocketInProcess;
		volatile bool m_isResetConnectionInProcess;
		std::vector<unsigned long> m_ResetSessionQueue;
		TA_Base_Core::ReEntrantThreadLockable m_ResetConnectionLock;
		TA_Base_Core::ReEntrantThreadLockable m_ResetSessionLock;

#if GWLDLINKLOST_RESET_STRATEGY==ONFIRSTERROR
        volatile bool                            m_currentlyResettingConnection;
        TA_Base_Core::NonReEntrantThreadLockable m_currentlyResettingConnectionLock;
#endif // #if GWLDLINKLOST_RESET_STRATEGY==ONFIRSTERROR

        /// this list should be restricted, because as Scott Meyers says
        /// "in C++, friends are allowed to touch their friends private parts"
		friend class RadioSession;
        /// required for access to addCallToCallStack only
        //friend class RadioCallStackUpdateTask;
        //friend class RadioGroupCallAckTask;
        //friend class RadioSDSDetailsUpdateTask;
        /// for access to doResetConnection
        friend class RadioResetConnectionTask;
		friend class RadioResetSessionTask;
		friend class RadioResetSocketTask;
        friend class RadioSystemErrorTask;
};

    inline RadioResourceManager & RadioTcpServer::getRadioResourceManager()
    {
        return (* m_audioResourceManager);
    }

    inline RadioAuthorisationManager & RadioTcpServer::getAuthorisationManager()
    {
        return (* m_authorisationManager);
    }

    inline ServerIndex RadioTcpServer::getServerIndex() const
    {
        return m_serverIndex;
    }

	inline ServerIndex RadioTcpServer::getCRSNumber() const
	{
		return m_serverIndex+1;
	}

    inline Radio & RadioTcpServer::getRadio()
    {
        return m_radio;
    }

    inline RadioSubscriberMonitor & RadioTcpServer::getSubscriberMonitor()
    {
        return *m_subscriberMonitor;
    }

	inline bool RadioTcpServer::isResetConnectionInProcess()
	{
		return m_isResetConnectionInProcess;
	}

	inline bool RadioTcpServer::isResetSocketInProcess()
	{
		return m_ResetSocketInProcess;
	}

	inline void RadioTcpServer::setResetConnectionInProcess()
	{
		TA_Base_Core::ThreadGuard guard(m_ResetConnectionLock);
		m_isResetConnectionInProcess = true;
	}

	inline void RadioTcpServer::clearResetConnectionInProcess()
	{
		TA_Base_Core::ThreadGuard guard(m_ResetConnectionLock);
		m_isResetConnectionInProcess = false;
	}

	/*inline bool RadioTcpServer::isResetSessionRefInProcess(SessionReference sessionRef)
	{
		TA_Base_Core::ThreadGuard guard(m_ResetSessionLock);

		std::vector<unsigned long>::iterator it = std::find( m_ResetSessionQueue.begin(), m_ResetSessionQueue.end(), sessionRef );

		return it != m_ResetSessionQueue.end();
	}*/

};

#endif // #ifndef RADIO_TCPSERVER_H
