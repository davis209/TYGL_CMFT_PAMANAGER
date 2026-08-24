#ifndef RADIOSERVANT_H
#define RADIOSERVANT_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/RadioServant.h $
  * @author  A. Parker
  * @version $Revision: #5 $
  * Last modification : $DateTime: 2009/05/18 10:25:59 $
  * Last modified by : $Author: builder $
  *
  * 
  */

#include "core/types/src/ta_types.h"
#include "core/alarm/src/AlarmHelper.h"
#include "core/corba/src/ServantBase.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/RadioGlobalEntityData.h" //TD14696

#include "core/message/src/AuditMessageSender.h"

#include "bus/radio/RadioAgentCorbaDef_4669/idl/src/IRadioCorbaDef.h"
#include "bus/trains/TrainCorbaProxy/src/TrainOperationModeCorbaProxy.h"
#include "bus/generic_agent/src/IEntity.h"

#include "app/radio/RadioTetraAgent/src/IRadioCallback.h"
#include "app/radio/RadioTetraAgent/src/RadioTypes.h"
#include "app/radio/RadioTetraAgent/src/RadioSynchronisationTypes.h"
#include "app/radio/RadioTetraAgent/src/RadioServerLinkStatusUpdateThread.h"

namespace TA_Base_Bus
{
    class ItaScada;
}

namespace TA_Base_Core
{
    class AlarmHelper;
}

namespace TA_IRS_App
{
	class Radio;
    class IRadioDirectoryCallback;
	class RadioStatusSummary;

	class RadioServant : public virtual POA_TA_Base_Bus::IRadioCorbaDef,
						 public virtual TA_Base_Core::ServantBase,
						 public virtual TA_Base_Bus::IEntity,
						 public virtual TA_IRS_App::IRadioCallback
	{
		public:

        RadioServant( /*TA_Base_Bus::ItaScada& scada*/ );
			virtual ~RadioServant();

			void setEntity(TA_Base_Core::IEntityData& EntityData, TA_Base_Core::RadioGlobalEntityData& globalEntity);
            void startServerLinkStatusThread() { m_serverLinkStatusThread->start(); };//limin, called by RadioTetraAgent after running control

			TA_IRS_App::Radio& getRadio() { return *m_radio; };

			void setAllSubscribers(TA_Base_Bus::IRadioCorbaDef::SubscriberList& list);
			void setAllGroups(TA_Base_Bus::IRadioCorbaDef::GroupList& list);
			void setAllPatches(TA_Base_Bus::IRadioCorbaDef::PatchList& list);
			void setAllSDS(TA_Base_Bus::IRadioCorbaDef::PredefinedSDSMessageList& list);
			void setLastUpdateTime(std::string time);

			// IEntity Interface Functions
			bool isValid(void);
			void start(void);
			void stop(void);
			void update(const class TA_Base_Core::ConfigUpdateDetails &details);
			void remove(void);
			void setToControlMode(void);
			void setToMonitorMode(void);
			void onTerminate(bool peerAvailable);

			// Exported Agent Functions through IRadio IDL interface
			TA_Base_Bus::IRadioCorbaDef::CallList*          getCallList(ConsoleID consoleId);
			TA_Base_Bus::IRadioCorbaDef::CallDetailsType*  getCallDetails(ta_int32 callID,ConsoleID consoleId);
			TA_Base_Bus::IRadioCorbaDef::ECallCategory     getCallCategory(ta_int32 callID,ConsoleID consoleId);
			char *  getCallText(ta_int32 callID, ConsoleID );
			virtual void sendSDSMessage(const char* trainITSI, const TA_Base_Bus::IRadioCorbaDef::MessageListType& messageList);
			virtual void sendData(const char* ITSI, const char* IPAddress, const TA_Base_Bus::IRadioCorbaDef::CircuitDataType& data, bool useXON, const char * sessionId);
			virtual void cancelSendData(const char* ITSI);
			virtual void addCallDriverRequest(unsigned char trainID,const char* trainTSI);
			virtual void removeCallDriverRequest(unsigned char trainID);

			TA_Base_Bus::IRadioCorbaDef::PredefinedSDSMessage* getByShortName(const char* shortName);
			TA_Base_Bus::IRadioCorbaDef::PredefinedSDSMessageList* getSDSList(bool readWrite);
			virtual char* getMessage(const char* shortName);
			virtual void deleteMessage(const char* shortName);
			virtual void createMessage(const char* shortName, const char* message, bool isNew);
			virtual void updateMessage(const char* shortName, const char* message);

			virtual TA_Base_Bus::IRadioCorbaDef::SelectedCallList* getList(const char* mft);
			virtual void addTetraid(const char* mft, const char* tsiNumber, const char* callId, const char* location);

			virtual TA_Base_Bus::IRadioCorbaDef::CallStackHistoryList* getCSHList(const char* profile, ta_uint32 locationKey, bool readWrite);
			virtual TA_Base_Bus::IRadioCorbaDef::CallStackHistoryList* getCSHListByType(const char* profile, ::CORBA::ULong locationKey, ::TA_Base_Bus::IRadioCorbaDef::EStackCallType callType, ::CORBA::Boolean readWrite);

			virtual void addCallStackEntry(const char* profile, ta_uint32 locationKey, ta_uint32 timestamp, TA_Base_Bus::IRadioCorbaDef::EOriginType originType, TA_Base_Bus::IRadioCorbaDef::EStackCallType callType, const char* identifier, const char* location, bool isEmergency, const char* itsi);
			virtual void updateCallStackHistory(const CallStackHistoryList& callStackHistory, bool isRetrieved);
			virtual void removeCallStackEntry(ta_uint32 timestamp, const char* tsi);

			virtual char* getOwnerFromPatchMember(const char* TSI);
			virtual bool getGroupIdsFromSubId(const TA_Base_Bus::IRadioCorbaDef::strList& list, const char* patchrasubs);
			virtual bool getSubNamesFromSubId(const TA_Base_Bus::IRadioCorbaDef::strList& list, const char* TSI);
			virtual void addGroupToPatch(const char* patchSubTsi, const char* groupSubTsi);
			virtual void deletePatch(const char* TSI);
			virtual bool groupPatchInsertDb(const char* displayName, const char* profileName, const char* timestamp);
			virtual char* getOwnerITSI(const char* patchname);

            // Fallback & Degraded IDL support
            virtual bool inFallback();
            virtual void resetConnection();


			virtual TA_Base_Bus::IRadioCorbaDef::RadioStatusSummary* RadioServant::getRadioStatusSummary();

			void	setDataPointLink(TA_Base_Bus::DataPoint* dp);

			// Call back functions for Radio object
			void onCallStatusChange(const TA_IRS_App::CallDetailsType& details);
			void onTrainEvent(std::string trainTSI,std::vector<unsigned char> messageData);
			
			virtual void onTXChange(const TXStateInfo txInfo);
			virtual void onPTTBtnStatus(const PTTBtnStateInfo pttBtnInfo);
			virtual void updateGX(std::string sessionId, std::string message, ConsoleID consoleId );

            virtual void onSubscriberActivity(TA_IRS_App::MonitoredCallDetailsType* subscriberActivityCallDetails);
            virtual void submitAlarm( const TA_Base_Core::MessageType& type, const TA_Base_Core::DescriptionParameters& dp );
			virtual void closeAlarm( const TA_Base_Core::MessageType& type); // TD9023

            virtual void onRadioFallback(bool isFallback);

            virtual void onCircuitModeActivity(const char * ITSI, bool successful, const char * failReason);
			virtual void sendCommsMgsNewCallStack(const char* profile, ta_uint32 locationKey, ta_uint32 timestamp, TA_Base_Bus::IRadioCorbaDef::EOriginType originType, TA_Base_Bus::IRadioCorbaDef::EStackCallType callType, const char* identifier, const char* location, bool isEmergency, const char* tsi);
			virtual void sendCommsMgsDeleteCallStack(ta_uint32 timestamp, const std::string tsi);

			// Help functions

            /** Facade for the identically named Radio function
             *  @param directoryCallback the pointer to the callback interface
             */
            void setDirectoryCallback(IRadioDirectoryCallback * directoryCallback);

            // state synchronisation functions
            void applyFullState(const RadioServantType& state);
            void clearFullState();
            void getFullState(RadioServantType& state);
            void updateCallInRadioCallStack(const CallDetailsType& callDetails);
            void removeCallFromRadioCallStack(CallID callID, ta_uint32 consoleID);
			/**
			  * updateRadioMonitoredCallProgression
			  *
			  * @ param radioSessionData - Radio Monitored Call Progression data to update
			  *
			  * Synchronisation wth the peer agent
			  */
			void updateRadioMonitoredCallProgression(ta_uint32 monitorReference);

			void removeMonitoredSubscriberDetails(ta_uint32 monitorReference, ta_int32 serverIndex);

			void removeSessionFromAudioMap(SessionID sessionReference, ta_int32 serverIndex);
			
			void updateAudioEntryInSessionMap(SessionID sessionReference, 
													  AudioReference audioReference,
												      const AudioEntryType& audioEntry);
			void updateRadioResourceManagerVars( SessionID sessionReference, CallReference callReference,
												int currentVolume, ta_int32 serverIndex);
			
			void updateAuthorisationRequest(const AuthorisationRequestType& authorisationRequest, ta_int32 serverIndex);
			
			void removeAuthorisationRequest(CallID callID, ta_int32 serverIndex);

			void updateConsoleSessionServers(ta_uint32 svrIdx, ta_int32 consoleID, ta_uint32 sessionRef);
		    //*************************************************
            // IRadioDirectory - Speed Dial Interface Methods *
            //*************************************************            
            virtual TA_Base_Bus::IRadioCorbaDef::SpeedDialList* getSpeedDialSet(const char* setName);
            virtual void updateSpeedDialSet(const TA_Base_Bus::IRadioCorbaDef::SpeedDialSet& setDetails, const char* sessionID);
            virtual void updateSpeedDial(const char* setName, const TA_Base_Bus::IRadioCorbaDef::SpeedDialEntry& newEntry, const char* sessionID);
            virtual char* getUserSetName(const char* sessionID,TA_Base_Bus::IRadioCorbaDef::EUserSetTypeMatch setTypeMatch);
				//************************************
			// IRadioDirectory Interface Methods *
			//************************************
			virtual char*											 getOCCLastUpdateTime();
			virtual TA_Base_Bus::IRadioCorbaDef::GroupList*			 getAllGroups();
			virtual TA_Base_Bus::IRadioCorbaDef::PatchList*			 getAllPatches();
			virtual TA_Base_Bus::IRadioCorbaDef::SubscriberList*     getAllSubscribers();
			virtual TA_Base_Bus::IRadioCorbaDef::SubscriberList*     getGroupSubscribers();
            virtual TA_Base_Bus::IRadioCorbaDef::SubscriberList*     getPatchSubscribers();
            virtual TA_Base_Bus::IRadioCorbaDef::SubscriberList*     getIndividualSubscribers();
            virtual TA_Base_Bus::IRadioCorbaDef::SubscriberDetails*  getSubscriberDetails(const char* TSI);
            virtual TA_Base_Bus::IRadioCorbaDef::SubscriberList*     getGroupMembers(const char* TSI);
            virtual TA_Base_Bus::IRadioCorbaDef::SubscriberList*     getPatchMembers(const char* TSI);
            virtual TA_Base_Bus::IRadioCorbaDef::SubscriberList*     searchSubscriber(const char* searchText);
			virtual CORBA::Boolean sendTLSDSMessage(const char* trainITSI, const TA_Base_Bus::IRadioCorbaDef::MessageListType& messageList, CORBA::String_out status);
			virtual CORBA::Boolean sendUserDefinedSDSMessage(const char* trainITSI, const TA_Base_Bus::IRadioCorbaDef::MessageListType& messageList, CORBA::String_out status);
	
			//****************************************
			// IRadioSessionCorbaDef Interface Methods *
			//****************************************
			virtual ta_int32 getSessionReference(const char * sessionId, const char* sessionName);
			virtual void holdCall(ta_int32 callId, const char * sessionId, const char* sessionName);
			virtual void resumeCall(ta_int32 callId, const char * sessionId, const char* sessionName);
			virtual void answerCall(ta_int32 callID, const char * sessionId, const char* sessionName);
			virtual void releaseCall(ta_int32 callID, const char * sessionId, const char* sessionName);

			virtual ta_int32 startMonitoring(const char * tsi, const char * sessionID, const char* sessionName);
			virtual void stopMonitoring(const char * tsi, const char * sessionID, const char* sessionName);
			virtual CORBA::Long oneShotMonitoring(const char* TSI, const char* sessionID, const char* sessionName);
			virtual TA_Base_Bus::IRadioSessionCorbaDef::MonitoredSubscriberList* getMonitoredSubscribers(const char* sessionID, const char* sessionName);
			virtual TA_Base_Bus::IRadioSessionCorbaDef::MonitoredGroupList* getMonitoredGroup(const char* sessionID, const char* sessionName);

			virtual void getActiveCalls(const char * tsi, const char * sessionID, const char* sessionName);
			virtual ta_int32 startListening(ta_int32 callId, const char * sessionId, const char* sessionName);
			virtual void stopListening(ta_int32 callId, const char * sessionId, const char* sessionName);
			virtual ta_int32 joinCall(ta_int32 callId, const char * sessionId, const char* sessionName);
			virtual void terminateCall(ta_int32 callId, const char * sessionId, bool bIsMonitored, const char* sessionName);

			virtual ta_int32 includeSubscriber(ta_int32 callId, const char * subscriber, const char * sessionId, const char* sessionName);
			virtual void selectCall(ta_int32 callId, const char * sessionId, const char* sessionName);
			virtual void deselectCall(ta_int32 callId, const char * sessionId, const char* sessionName);
			virtual void adjustCallVolume(ta_int32 callIds, bool increase, const char * SessionId, const char* sessionName);
			void sessionLogin(const char * loginName, const char * sessionId, const char* sessionName);

			virtual CORBA::Boolean sendTextMessage(const char* destinationTSI, const TA_Base_Bus::IRadioSessionCorbaDef::MessageListType& message, const char* sessionId, CORBA::String_out status, const char* sessionName);
			//virtual CORBA::Boolean sendUserDefinedSDS(const char* destinationTSI, const TA_Base_Bus::IRadioSessionCorbaDef::MessageListType& messageList, const char* sessionId, CORBA::String_out status);
			virtual ta_int32 makeCall(const char * destinationTSI, TA_Base_Bus::IRadioCorbaDef::ECallType calltype, bool isFullDuplex, TA_Base_Bus::IRadioCorbaDef::ECallPriority priority, bool hook, const char * sessionId, bool isInvisibleToGUIApps, ta_int32 locationKey, const char* sessionName);
			virtual char * makeDynamicGroup(const char* dgnaTSI, const char* dgnaName, const TA_Base_Bus::IRadioSessionCorbaDef::DestinationTSIList& destTSIs, const char * sessionId, const char* sessionName);
			virtual void updateDynamicGroup(const char* dgnaTSI, const char* dgnaName, const ::TA_Base_Bus::IRadioSessionCorbaDef::DestinationTSIList& destTSIs, const char* sessionID, const char* sessionName);
			virtual void deleteDynamicGroup(const char* dgnaTSI, const char* sessionID, const char* sessionName);
			virtual void setCallForwarding(const TA_Base_Bus::IRadioSessionCorbaDef::CallForwardDetails& callForwardDetails, const char * sessionId, const char* sessionName);
			virtual TA_Base_Bus::IRadioSessionCorbaDef::CallForwardDetails* getCallForwarding(const char* sessionName);
			virtual void createSessionPatch(const TA_Base_Bus::IRadioSessionCorbaDef::PatchList& list, const char* patchName, const char * SessionId, const char* sessionName);
			virtual void deleteSessionPatch(const char* patchName, const char * SessionId, const char* sessionName);
			virtual void sessionInitialisation(ta_int32 location, ta_int32 console);
			virtual ::CORBA::Boolean getRadioSessionIsLogin(const char* sessionName);
			virtual char* getSessionTsi(const char* sessionName);
			//******************************************
			// IRadioDirectoryCorbaDef Interface Methods *
			//******************************************
			virtual ta_int32 getLastDirectorySynchronisationTime();

			unsigned short getFlexibleTimeOutValue();
			void demandTX(::CORBA::Short priority, const char* sessionID, ::CORBA::ULong& queuedID, const char* sessionName);
			void ceaseTX(const char* sessionID, const char* sessionName);
	private:
			
			std::string         m_objectName;
			ta_uint32        m_EntityKey;
			ta_uint32    	m_entityLocation;
			ta_uint32 		m_entitySubsystem;
			ta_uint32 		m_entityTypeKey;
			std::string			m_assetName;

			TA_IRS_App::Radio*      m_radio;

			bool                m_active;
			unsigned short m_flexibleTimeout;
			//for the TD17094
            //TA_Base_Core::AlarmHelper& m_alarmHelper;

			TA_Base_Core::AuditMessageSender* m_auditMessageSender; //TD13048
            std::string         m_primaryServers;
            std::string         m_secondaryServers;

            // Raymond Pau++
            //TA_Base_Bus::ItaScada& m_scada;
            // ++Raymond Pau
			TA_IRS_Bus::TrainOperationModeCorbaProxyPtr m_trainAgentProxy;

			RadioServerLinkStatusUpdateThread*		m_serverLinkStatusThread;
	};  // Class RadioServant
}; // Namespace TA_IRS_App

#endif
