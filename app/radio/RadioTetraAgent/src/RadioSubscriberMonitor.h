#ifndef RADIO_MONITOR_MANAGER_H
#define RADIO_MONITOR_MANAGER_H
/**
  * The source code in this file is the property of
  * MI Services Group and is not for redistribution
  * in any form.
  *
  * Source : $Source: /cvs/1TS-9999-T/code/transactive/app/radio/RadioTetraAgent/src/Attic/RadioSubscriberMonitor.h,v $
  * @author  R. van Hugten
  * @version $Revision: 1.1.2.5 $
  * Last modification : $Date: 2006/12/07 18:24:02 $
  * Last modified by : $Author: robertvh $
  *
  * Definition of the RadioSubscriberMonitor class
  */

#include <map>
#include <list>
#include <string>
#include <vector>

#include "app/radio/RadioTetraAgent/src/RadioTypes.h"
#include "app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioProtocolLibrary.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

/**
 * This class monitors subscriber monitoring activities. By that, it means that the 
 * actual management of monitoring is in the Radio System. This class keeps an
 * internal state so as to provide queries and a modicum of behavioural integrity
 * for the RadioMonitor GUI.
 * 
 * That means that this class is updated *after* successful events from the Radio
 * System, either of un- or solicited.  There are 5 flows to consider :
 * 
 * - commencement of monitoring of a subscriber, either MON_START or MON_ONE_SHOT
 * - cesation of monitoring, via MON_STOP (either of un- or solicited)
 * - request to listen
 * - request to join/include
 * - unsolicited subscriberactivity
 *
 * Note that according to anecdotal reports from architectural level ppl at Selex,
 * subscriber monitoring is tangential functionality, so unsolicited
 * SubscriberActivity events are generated for everything that happens to a
 * monitored subscriber's calls. Which includes subscriberActivity events for
 * the implicit termination/setup of any calls caused by listening or joining.
 * 
 * To coordinate activities, it uses 2 internal structures and 3 varieties of function.
 *
 * struct MonitoredCallInformation holds the call references for the various
 * states that a monitored call can go through, including a copy of the corba
 * MonitoredCallDetailsType struct that can be sent to the RadioMonitor GUI.
 *
 * struct MonitoredSubscriberDetails holds those pieces of information associated
 * with a monitor reference and provides methods for searching that information.
 *
 * MonitorReference functions allow for adding, removing and finding a monitor reference
 *      void addMonitor(MonitorReference monitorRef);
 *      void removeMonitor(MonitorReference monitorRef);
 *      MonitorReference findMonitor(CallID callId);
 *      bool isMonitored(MonitorReference monitorRef);
 *
 *
 * Status Update functions are used when a call has changed state
 *      bool startedListenCall(CallID monCallId, CallID listenCallId);
 *      bool stoppedListenCall(CallID callId);
 *      bool joinedCall(CallID listenCallId, CallID callId, bool included);
 *      bool terminatedCall(CallID callId);
 *      void subscriberActivity(MonitorReference monitorRef, CallID callId, TA_IRS_App::MonitoredCallDetailsType * monitoredCall);
 *
 *
 * Status Query functions
 *      bool isMonitoredCall(CallID callId);
 *      bool canListenCall(CallID callId);
 *      bool isListenCall(CallID callId);
 *      CallID getJoinCallId(CallID callId);
 **/
namespace TA_IRS_App
{
    class IRadioCallback;
    class RadioTcpServer;
    struct RadioMonitorManagerType;
    struct MonitoredSubscriberDetailsType;

    class RadioSubscriberMonitor
    {

    public:
        /**
         * Constructor
         **/
        //RadioSubscriberMonitor(Radio & tcpServer);
        RadioSubscriberMonitor(RadioTcpServer & tcpServer);


        /** Adds the given monitor reference to the map
         * @param monitorRef the new monitor reference
         **/
        void addMonitor(MonitorReference monitorRef, SessionReference sessionRef, const char * subscriberTsi);


        /** Removes the given monitor reference from the map
         * @param monitorRef the ref to delete
         **/
        void removeMonitor(const char * subscriberTsi);


        /** Removes the given monitor reference from the map
         * @param subscriberTsi the subscriber to stop monitoring
         **/
        void removeMonitor(MonitorReference monitorRef);

        /** Searches for the MonitorReference that corresponds to the given
         * Subscriber TSI
         *
         * @param   subscriberTsi the tsi of the subscriber to search for
         * @return  the found monitor reference, or zero if the callId was not
         *          found.
         **/
        MonitorReference findMonitor(const char * subscriberTsi);

        /** Searches for the MonitorReference that corresponds to the given
         * Call Reference/ID.  Looks at all three call references in each
         * MonitoredCallDetailsType.
         *
         * @param   callId  the call reference to search for
         * @return  the found monitor reference, or zero if the callId was not
         *          found.
         **/
        MonitorReference findMonitor(CallID callId);
       
		/**
		*set the operation mode as control
		**/
		void setToControlMode () ;

		/**
		*set the operation mode as monitor
		**/
		void setToMonitorMode () ; 

        /** Determines if the given reference is being monitored
         *
         * @param monitorRef the ref to search for
         */
        bool isMonitored(MonitorReference monitorRef);


        /** Deals with the given subscriber activity. This overloaded version is
         * called when the call is already known, ie. the MonitoredCallDetails
         * are already established and populated.
         *
         * @param monitorRef the new monitor reference
         * @param callId     the reference of the call
         * @param isCallStart true if this subscriber activity is for the start
         *                   of a call, false if for CALL_END
         **/
        void subscriberActivity(MonitorReference monitorRef, CallID callId, bool isCallStart);

        /** Deals with the given subscriber activity. This overloaded version is
         * called when the MonitoredCallDetails have been freshly populated because
         * they did not previously exist.
         *
         * @param monCallDetails the call details
         */
        void subscriberActivity(MonitoredCallDetailsType * monCallDetails);

        /** Checks if the given monitorRef already has details regarding the
         * specified call
         */
        bool isKnownCall(MonitorReference monitorRef, CallID callId);


        /** Check for if the given call is being listened to
         *
         * @param   callId  the reference of the call to check
         * @return  true if the call is being monitored, ie. it is both a call
         *          that a monitored subscriber is involved in, and the
         *          operator is neither listening nor joined/included.
         **/
        bool isMonitoredCall(CallID callId);


        /** Checks if the given call can be listened to.
         * Currently synonymous with isMonitoredCall()
         *
         * @param   callId  the call reference of the call
         * @return  true if the call is monitored but not listened or
         *          joined/included.
         **/
        bool canListenCall(CallID callId);


        /** Marks the given call as being monitored, sends the RadioMonitor GUI
         * an update.
         *
         * @param callId the call ref of the new call
         * @return false if monitor ref could not be found that corresponds
         *         to the call, or the call is already being listened
         **/
        bool startedListenCall(CallID monCallId, CallID listenCallId);

        /** Marks the given call back down to just being monitored.
         *
         * @param   callId  the call ref of the call to not listen to
         * @return  false if the call could not be found, or is not being
         *          listened to
         **/
        bool stoppedListenCall(CallID callId);

        /** Check for if the given call is being listened to
         *
         * @param   callId  the reference of the call to check
         * @return  true if the call is being monitored, ie. it is both a call
         *          that a monitored subscriber is involved in, and the
         *          operator is listening.
         **/
        bool isListenCall(CallID callId);


        /** Returns the Call ID that should be used for the Join Method API
         * call. Mostly relevant for Group/Conference calls.
         *
         * If the original call was a group call, the MonitorCall API Method
         * will return a second Call Id that "must be used from this point
         * for any operation made by dispatcher". That is stored in the 
         * TA_IRS_App::MonitoredCallDetailsType.  In Figure 6-14 however, the Join
         * Method is shown as taking the original call reference "(Ref-1)".
         * Also, the Join Method documentation states that the CallRef argument
         * is to be "Active CallReference handle", implying similar.
         *
         * @param   callId  the call reference of the call to join
         * @return  the call reference to be used in the Join Method
         **/
        CallID getJoinCallId(CallID callId);

        /** Marks the given call for all relevant monitorRefs as pending a join.
         * This means that when the terminatedCall comes in, the call should not
         * be removed, because it will eventually be replaced with a Group Call.
         *
         * @param   callId  the call that will be terminated to replaced by a
         *                  group call
         * @return  true    if at least one call was found to be monitored with
         *                  the given callId
         */
        bool designateAsPendingInclude(CallID callId);

        /** Moves the status of the call to Joined, updates the call references
         *
         * @param   listenCallId    the reference of the call that was joined to
         * @param   callId          the reference of the new group call
         * @return  true if joining was successful
         **/
        bool joinedCall(CallID listenCallId, CallID callId, bool included);

        /** Removes the call from the subscriber's
         *
         * @param   callId          the reference of the disconnected call
         * @return
         **/
        bool terminatedCall(CallID callId);

        /** Accessor for the monitorStatus of the Call in the given monitor */
        EMonitorStatusType getCallMonitorStatus(MonitorReference monitorRef, CallID callId);
		ECallSource getCallSource(MonitorReference monitorRef, CallID callId);

        // /////////////////////////////////////////////////////
        // 
        // STATE UPDATE
        // 
        // /////////////////////////////////////////////////////
		void applyFullState(const RadioMonitorManagerType& state);
		void clearFullState();
		void getFullState(RadioMonitorManagerType& state);
		void sendStateUpdate(MonitorReference& monitorRef);

		bool isInControl ()
		{
			return m_isInControl ;
		}
		
		MonitoredSubscriberList getMonitoredSubscribers(SessionReference sessionRef);
		void removeMonitoringbySessionRef(SessionReference sessionRef);

    private:
        void notifySubscriberActivity(MonitoredCallDetailsType * monitoredCallDetails);
        
        /** A struct for holding the call references for the various states
         *  that a monitored call can go through
         */
        struct MonitoredCallInformation
        {
            EMonitorStatusType	monitorStatus;

            /// A monitored call can have three call references :
            /// 1. The original call, provided by the SubscriberActivity event
            CallID				monitorCall;

            /// 2. Listening to a call, provided by the MonitorCall event
            ///    This will be different to the monitorCall id if the call has
            ///    changed from single to group/conference call.
            CallID				listenCall;

            /// 3. After joining an individual or group call, from the Join or
            ///    Include event.
            CallID				joinedCall;

            ///
            /// Cached for re-use with each change of call status
            MonitoredCallDetailsType * monitoredCallDetails;

            /** user-specified constructor so as to implement proper initialiser list */
            MonitoredCallInformation();

            /** one of our data members is a pointer so we should delete it properly  */
            ~MonitoredCallInformation();
        };

        ///
        ///
        typedef std::list< MonitoredCallInformation > MonitoredCallList;


        /**
         * Details associated with a MonitorReference
         */
        struct MonitoredSubscriberDetails
        {
            ///
            MonitorReference	monitorRef;

			// Owner Radio Session Ref
			SessionReference sessionRef; 

            ///
            std::string         subscriberTsi;

            ///
            MonitoredCallList   monitoredCalls;

            /**
             * search for a call with the given id, amongst the monitoredCalls
             *
             * @param callId the given call id to search for
             * @param found  the outgoing parameter, set to true if a call with
             *               the given id was found, false otherwise
             * @return       the MonitoredCallInformation if found, a reference
             *               to the NullMonitoredCallInformation otherwise
             */
            MonitoredCallInformation & findAnyCall(CallID callId, bool & found);

            /**
             * search for a call only matching monitoredCalls, with the given id,
             * amongst the monitoredCalls
             *
             * @param callId the given call id to search for
             * @param found  the outgoing parameter, set to true if a call with
             *               the given id was found, false otherwise
             * @return       the MonitoredCallInformation if found, a reference
             *               to the NullMonitoredCallInformation otherwise
             */
            MonitoredCallInformation & findMonitoredCall(CallID callId, bool & found);

            /**
             * search for a call only matching listenedCalls, with the given id,
             * amongst the monitoredCalls
             *
             * @param callId the given call id to search for
             * @param found  the outgoing parameter, set to true if a call with
             *               the given id was found, false otherwise
             * @return       the MonitoredCallInformation if found, a reference
             *               to the NullMonitoredCallInformation otherwise
             */
            MonitoredCallInformation & findListenedCall(CallID callId, bool & found);

            /**
             * search for a call either a monitoredCall or listenedCall, with
             * the given id, amongst the monitoredCalls
             *
             * @param callId the given call id to search for
             * @param found  the outgoing parameter, set to true if a call with
             *               the given id was found, false otherwise
             * @return       the MonitoredCallInformation if found, a reference
             *               to the NullMonitoredCallInformation otherwise
             */
            MonitoredCallInformation & findJoinableCall(CallID callId, bool & found);

            /**
             * search for a call only matching joinedCalls, with the given id,
             * amongst the monitoredCalls
             *
             * @param callId the given call id to search for
             * @param found  the outgoing parameter, set to true if a call with
             *               the given id was found, false otherwise
             * @return       the MonitoredCallInformation if found, a reference
             *               to the NullMonitoredCallInformation otherwise
             */
            MonitoredCallInformation & findJoinedCall(CallID callId, bool & found);

            /** Accessor for the monitorStatus of the Call */
            EMonitorStatusType getCallMonitorStatus(CallID callId);

            /** removes the MonitoredCallInformation if any of its calls match the given. */
			bool removeCall(CallID callId);

            /**  */
            void cleanup();

		

            /** specify constructor so as to implement proper initialiser list */
            MonitoredSubscriberDetails();
        };

        ///
        /// type definition for a Map of MonitoredSubscriberDetails key'd by monitorRef
        typedef std::map< MonitorReference, MonitoredSubscriberDetails * > MonitoredSubscriberMap;

        ///
        /// Map of MonitoredCallDetails key'd by monitorRef
        MonitoredSubscriberMap                  m_monitoredSubscribers;

        ///
        /// ThreadGuard lock for m_monitoredSubscribers
        TA_Base_Core::ReEntrantThreadLockable   m_monitoredLock;

        ///
        /// CallInformation returned when actual call is not found
        static MonitoredCallInformation NullMonitoredCallInformation;

        friend struct MonitoredSubscriberDetails;

		// Full State Hierachy
		std::vector<TA_IRS_App::MonitoredSubscriberDetailsType>	m_radioMonitorSubscriberStack;

        RadioTcpServer &                        m_tcpServer;

		bool m_isInControl ;

    };

}

#endif // #ifndef RADIO_MONITOR_MANAGER_H
