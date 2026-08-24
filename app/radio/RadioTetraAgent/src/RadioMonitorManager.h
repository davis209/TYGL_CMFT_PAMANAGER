#ifndef RADIO_MONITOR_MANAGER_H
#define RADIO_MONITOR_MANAGER_H
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/RadioMonitorManager.h $
  * @author  R. van Hugten
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2008/11/28 16:26:01 $
  * Last modified by : $Author: builder $
  *
  * Definition of the RadioMonitorManager class
  */

#include "core/types/src/ta_types.h"
#include <string>
#include <vector>
#include <map>
#include <list>

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "app/radio/RadioTetraAgent/src/RadioTypes.h"
#include "app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioProtocolLibrary.h"
#include "bus/radio/RadioAgentCorbaDef_4669/idl/src/IRadioCorbaDef.h"

/**
 * This class manages monitoring activities
 **/
namespace TA_IRS_App
{
    class Radio;
    class IRadioCallback;
    class RadioBackgroundProcessor;


    class RadioMonitorManager
    {

    public:
        /**
         * Constructor
         **/
        RadioMonitorManager(Radio & radio, IRadioCallback& callback, RadioBackgroundProcessor * bgProcessor);


        /** Adds the given monitor reference to the map
         * @param monitorRef the new monitor reference
         **/
        void addMonitor(MonitorReference monitorRef);


        /** Removes the given monitor reference from the map
         * @param monitorRef the ref to delete
         **/
        void removeMonitor(MonitorReference monitorRef);

        /**
         *
         */
        bool isMonitored(MonitorReference monitorRef);


        /** Searches for the MonitorReference that corresponds to the given
         * Call Reference/ID.  Looks at all three call references in each
         * MonitoredCallDetailsType.
         *
         * @param   callId  the call reference to search for
         * @return  the found monitor reference, or zero if the callId was not
         *          found.
         **/
        MonitorReference findMonitor(CallID callId);


        /** Deals with the given subscriber activity.
         * @param monitorRef the new monitor reference
         * @param callId     the reference of the call
         **/
        void subscriberActivity(MonitorReference monitorRef, CallID callId, TA_Base_Bus::IRadioCorbaDef::MonitoredCallDetailsType * monCallDetails);


        /** Check for if the given call is being listened to
         *
         * @param   callId  the reference of the call to check
         * @return  true if the call is being monitored, ie. it is both a call
         *          that a monitored subscriber is involved in, and the
         *          operator is neither listening nor joined/included.
         **/
        bool isMonitoredCall(CallID callId);


        /** Checks if the given call can be listened to.
         * Currently a synonym for isMonitoredCall()
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
        bool startListenCall(CallID monCallId, CallID listenCallId);

        /** Check for if the given call is being listened to
         *
         * @param   callId  the reference of the call to check
         * @return  true if the call is being monitored, ie. it is both a call
         *          that a monitored subscriber is involved in, and the
         *          operator is listening.
         **/
        bool isListenCall(CallID callId);

        /** Marks the given call back down to just being monitored.
         *
         * @param   callId  the call ref of the call to not listen to
         * @return  false if the call could not be found, or is not being
         *          listened to
         **/
        bool stopListenCall(CallID callId);


        /** Returns the Call ID that should be used for the Join Method API
         * call. Only really relevant for Group/Conference calls.
         *
         * If the original call was a group call, the MonitorCall API Method
         * will return a second Call Id that "must be used from this point
         * for any operation made by dispatcher". That is stored in the 
         * TA_IRS_App::MonitoredCallDetailsType.  In Figure 6-14 however, the Join
         * Method is shown as taking the original call reference "(Ref-1)".
         * Also, the Join Method documentation states that the CallRef argument
         * is to be "Active CallReference handle", implying similar.
         *
         * This method can be used in place of "canJoinCall", although that
         * changes the semantics of the code (from "if canJoinCall then" to
         * "if getJoinCallID is not zero then (use the result...)")
         *
         * @param   callId  the call reference of the call to join
         * @return  the call reference to be used in the Join Method
         **/
        CallID getJoinCallId(CallID callId);


        /** Moves the
         *
         * @param   listenCallId    the reference of the call that was joined to
         * @param   callId          the reference of the new group call
         * @return  true if joining was successful
         **/
        bool joinCall(CallID listenCallId, CallID callId, bool included);

        /**
         *
         * @param 
         * @param 
         * @return
         **/
		void applyFullState(const RadioMonitorManagerType& state);
		void clearFullState();
		void getFullState(RadioMonitorManagerType& state);
		void sendStateUpdate(MonitorReference	&monitorRef);



    private:
        ///
        /// A struct for holding the call references for the various states
        /// that a monitored call can go through
        struct MonitoredCallProgression
        {
            EMonitorStatusType	monitorStatus;

            /// A monitored call can have three call references :
            /// 1. The original call, provided by the SubscriberActivity event
            CallID				monitorCall;

            /// 2. Listening to a call, provided by the MonitorCall event
            ///    This will be different to the monitorCall id if the call is
            ///    a group/conference call.
            CallID				listenCall;

            /// 3. After joining an individual or group call, from the Join or
            ///    Include event.
            CallID				joinedCall;

            /// Cached for re-use with each change of call status
            TA_Base_Bus::IRadioCorbaDef::MonitoredCallDetailsType * monitoredCallDetails;
        };

        ///
        ///
        typedef std::list< MonitoredCallProgression > MonitoredCallList;


        ///
        ///
        struct MonitoredSubscriberDetails
        {
            ///
            MonitorReference	monitorRef;

            ///
            MonitoredCallList   monitoredCalls;

            ///
            // bool findAnyCall(CallID callId, MonitoredCallProgression & mcp);
            MonitoredCallProgression & findAnyCall(CallID callId, bool & found);

            ///
            // bool findMonitoredCall(CallID callId, MonitoredCallProgression & mcp);
            MonitoredCallProgression & findMonitoredCall(CallID callId, bool & found);

            ///
            // bool findListenedCall(CallID callId, MonitoredCallProgression & mcp);
            MonitoredCallProgression & findListenedCall(CallID callId, bool & found);

            ///
            // bool findJoinableCall(CallID callId, MonitoredCallProgression & mcp);
            MonitoredCallProgression & findJoinableCall(CallID callId, bool & found);

            ///
            // bool findJoinedCall(CallID callId, MonitoredCallProgression & mcp);
            MonitoredCallProgression & findJoinedCall(CallID callId, bool & found);

			// 
			bool removeCall(CallID callId);


            ///
            void cleanup();

        };

        ///
        /// type definition for a Map of MonitoredSubscriberDetails key'd by monitorRef
        typedef std::map< ta_int32 , MonitoredSubscriberDetails * > MonitoredActivityMap;

        ///
        /// Map of MonitoredCallDetails key'd by monitorRef
        MonitoredActivityMap                    m_monitoredActivities;

        ///
        /// Lock for m_monitoredActivities
        TA_Base_Core::ReEntrantThreadLockable        m_monitoredLock;

        ///
        /// main Radio object
        Radio &                                 m_radio;

        ///
        /// Radio Callback
        IRadioCallback &                        m_radioCallback;

        ///
        ///
        static MonitoredCallProgression NullMonitoredCallProgression;

        friend struct MonitoredSubscriberDetails;

		// Full State Hierachy
		std::vector<TA_IRS_App::MonitoredSubscriberDetailsType>	m_radioMonitorSubscriberStack;

    };
}

#endif // #ifndef RADIO_MONITOR_MANAGER_H
