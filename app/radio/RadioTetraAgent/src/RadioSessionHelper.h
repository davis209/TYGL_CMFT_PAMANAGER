#ifndef RADIO_SESSION_HELPER_H
#define RADIO_SESSION_HELPER_H
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/TYGL_CMFT_TIP/J155/transactive/app/radio/RadioTetraAgent/src/RadioSessionHelper.h $
  * @author  R. van Hugten
  * @version $Revision: #3 $
  * Last modification : $DateTime: 2023/10/16 18:31:43 $
  * Last modified by : $Author: CM $
  *
  * Implementation of the RadioSessionHelper class
  */

#ifdef __WIN32__
#pragma warning(disable:4290) // C++ Exception Specification ignored
#endif


#include <string>
#include <vector>
#include "app/radio/RadioTetraAgent/src/RadioTypes.h"
#include "app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioProtocolLibrary.h"
#include "bus/radio/RadioAgentCorbaDef_4669/idl/src/IRadioCorbaDef.h"
#include "RadioFallbackException.h"

/**
* This class manages a single radio session.
* @author Andy Parker
* @version 1.0
* @generator Enterprise Architect
* @created 30-Oct-2003 08:20:31
*/
namespace TA_IRS_App
{
    class RadioTcpServer;
    struct CallForwardingDetails;
    class RadioEvent;
    class RadioMethod;
    class RadioSessionHelperInitialisationThread;
    class RadioMethodNewReference;
    class IRadioSessionHelperServant;


    class RadioSessionHelper
    {

    public:
        RadioSessionHelper(SessionReference sessionRef, RadioTcpServer & radioSvr);



        SessionReference getSessionRef();


        /**
         * Get a new reference for use with the radio commands
         */
        RadioReference getNewReference(RadioMethodNewReference::ReferenceType referenceType);


        /**
         * deleteReference
         *
         * Deletes the reference passed to it. Cannot be used by subscriber references.
         *
         * @param reference The reference to delete
         * @param refType   The type of the reference
         */
        void deleteReference(RadioReference reference, RadioMethodDeleteReference::ReferenceType refType);

        /**
         * Safely handles deletion of the reference and event if the API operation fails
         *
         */
        void changeReference(RadioMethodChangeReference * changeReferenceMethod, RadioReference reference, RadioMethodDeleteReference::ReferenceType refType);

        /**
         * Query the details from a reference.
         *
         * @param queryRef  The reference to query
         * @param key       the Query Key to use in the QueryReference
         * @return          the value of the result of the query
         */
        std::string queryReference(RadioReference queryRef, RadioMethodQueryReference::EKeys key);


        /**
         * Query the details from a reference, returning multiple values as a list
         *
         * @param queryRef  The reference to query
         * @param key       the Query Key to use in the QueryReference
         * @return          the value of the result of the query, broken up
         *                  into a vector of string
         */
        std::vector<std::string> queryReferenceAsList(RadioReference queryRef, RadioMethodQueryReference::EKeys key, char delimiter='\n');


        /**
         * Query the details from a reference, returning multiple values in a map
         *
         * @param queryRef  The reference to query
         * @param key       the Query Key to use in the QueryReference
         * @param queryVal  the value of the result of the query, broken up
         *                  into a vector of string
         */
        void queryReferenceAsMap(RadioReference queryRef, RadioMethodQueryReference::EKeys key, QueryKeyValueMap & queryMap);


        /**
         * This function will return a subscriber reference which can be used for direct
         * manipulation of the associated record.
         *
         * Remember to free the new subscriber reference when you have finished with it!
         */
        SubscriberReference convertTextToSubscriberReference(std::string textReference);


        /**
         * This function will return a search result reference for a specified search
         *
         * Remember to free the new search reference when you have finished with it!
         */
        SearchResultReference performSubscriberSearch (std::string searchCriteria,
                RadioMethodSearchSubscribers::EView view = RadioMethodSearchSubscribers::NORMAL);



        /**
         * Farnarcles around searching subscribers and querying to determine
         * the reference of the subscriber with the given TSI.
         *
         * @param tsi the TSI of the subscriber which reference to retrieve
         */
        SubscriberReference getSubscriberReference(const std::string & tsi);

        /**
         * Farnarcles around searching subscribers and querying to determine
         * the name of the subscriber with the given TSI.
         *
         * @param tsi the TSI of the subscriber which name to retrieve
         */
        std::string getSubscriberName(const std::string & tsi);


        /**
         * Farnarcles around searching subscribers and querying to determine
         * the K_TYPE of the subscriber with the given TSI.
         *
         * @param tsi the TSI of the subscriber which type to retrieve
         */
        std::string getSubscriberType(const std::string & tsi);


        /**
         * Farnarcles around searching subscribers and querying to determine
         * the given attribute of the subscriber with the given TSI.
         *
         * @param tsi the TSI of the subscriber which attribute to retrieve
         */
        std::string getSubscriberAttribute(const std::string & tsi, RadioMethodQueryReference::EKeys key);

        /**
         * Performs a SubscriberSearch and queries the TCPServer to determine
         * the details of the subscriber with the given TSI.  The details to
         * retrieve are specified in the keys of the QueryKeyValueMap.
         *
         * @param callId    the CallReference of the call which details to retrieve
         * @param queryMap  the map of CallRef EKeys and result strings.
         * @return          the number of details successfully populated
         */
		ta_int32 getCallDetails(CallID callId, QueryKeyValueMap & queryMap);

        /**
         * Performs a SubscriberSearch and queries the TCPServer to determine
         * the details of the CAD Request with the given reference.  The details
         * to retrieve are specified in the keys of the QueryKeyValueMap.
         *
         * @param cadRef    the CADReference of the call which details to retrieve
         * @param queryMap  the map of CADRef EKeys and result strings.
         * @return          the number of details successfully populated
         */
        ta_int32 getCADDetails(CADReference cadRef, QueryKeyValueMap & queryMap);


        /**
         * Performs a SubscriberSearch and queries the TCPServer to determine
         * the details of the subscriber with the given TSI.  The details to
         * retrieve are specified in the keys of the QueryKeyValueMap.
         *
         * @param subscriberRef the reference of the subscriber which details to retrieve
         * @param queryMap  the map of subscriber EKeys and result strings.
         * @return          the number of details successfully populated
         */
		ta_int32 getSubscriberDetails(SubscriberReference subscriberRef, QueryKeyValueMap & queryMap);



        /**
         * Performs a SubscriberSearch and queries the TCPServer to determine
         * the details of the subscriber with the given TSI.  The details to
         * retrieve are specified in the keys of the QueryKeyValueMap.
         *
         * @param tsi       the TSI of the subscriber which details to retrieve
         * @param queryMap  the map of subscriber EKeys and result strings.
         * @return          the number of details successfully populated
         */
		ta_int32 getSubscriberDetails(const std::string & tsi, QueryKeyValueMap & queryMap);


        /**
         *
         */
		ta_int32 getGroupMemberReferences(const std::string & gtsi, std::vector<std::string> & groupMembers);


        /**
         * Queries the RCS for details of all the participants in a group call.
         *
         * @param gtsi the TSI of the group which call participant to populate
         *             the data with
         * @param details the data to populate with participants
         *
         */
		ta_int32 populateCallParticipantData(const std::string & gtsi, TA_IRS_App::CallDetailsType * details, ECallType callType );


        /**
         * Does the same job as the other overloaded function of this name,
         * however the contents of the details are corba and overall there
         * wasn't a neat way of templating or refactoring the common behaviour
         * between the two.
         *
         * @param gtsi the TSI of the group which call participant to populate
         *             the data with
         * @param details the data to populate with participants
         */
		ta_int32 populateCallParticipantData(const std::string & gtsi, TA_Base_Bus::IRadioCorbaDef::MonitoredCallDetailsType * details );


        /**
         * Uses the given vector of TSIs to populate the CallDetails
         *
         * @param tsiVector list of call participants' tsi's
         * @param details   the data to populate with participants
         * @return the number of call participants 
         */
		ta_int32 populateCallParticipantData(const std::vector<std::string> & tsiVector, TA_IRS_App::CallDetailsType * details);

        /**
         * Populates the MonitoredCallDetailsType with :
         * callTime
         * callCategory
         * callType
         * callerTSI
         * calledTSI
         * isEmergency
         *
         * Assumes already populated :
         * callID
         */
        void populateMonitoredCallDetails(MonitoredCallDetailsType * monitoredCallDetails);

        /**
         * Populates the MonitoredCallDetailsType with :
         * callerName
         * callerLocation
         * calledName
         * calledLocation
         *
         * Assumes already populated :
         * callerTSI
         * calledTSI
         */
        void populateMonitoredCallSubscriberDetails(MonitoredCallDetailsType * monitoredCallDetails);
		

        /**
         * Simple test of if one of the members of the vector matches the given
         * string
         */
        bool isPresent(const std::string & text, std::vector<std::string> textList);

        ECallType convert2CallType(const std::string & callTypeStr);

        std::string getFallbackReplacementString() const;

        TA_IRS_App::ERadioUserType getCADRadioUserType(const std::string & typestr, const std::string & organisation);


        /**
         * Performs a SubscriberSearch and queries the TCPServer to determine
         * the details of the subscriber with the given TSI.  The details to
         * retrieve are specified in the keys of the QueryKeyValueMap.
         *
         * @param callId    the CallReference of the call which details to retrieve
         * @param queryMap  the map of CallRef EKeys and result strings.
         * @return          the number of details successfully populated
         */
        ta_int32 getReferenceDetails(RadioReference ref, QueryKeyValueMap & queryMap);

        /**
         * Detects if the Tsi is fully HLR-compliant vis-a-vis having the full
         * XXXX-10-466 tsi form. Returns a suitable tsi query based on this, as
         *
         */
        std::string getTsiQuery(const std::string & tsi);

        /**
         * Detects if the Tsi is already HLR-compliant vis-a-vis having the full XXXX-10-466
         */
        std::string fixTsiForQuery(const std::string & tsi);

        /**
         * Converts a shortform Tsi eg. "306" into the full-form eg. "306-10-466"
         * This is necessary for subscriber queries
         */
        std::string getTsiWithSuffix(const std::string& tsiToConvert);

        /**
         * getTsiWithoutSuffix
		 *
		 * Converts the provided Tsi into a Tsi without site suffix.
		 * If the tsi is already in "short" form, i.e. without suffix, no conversion is done.
		 * If the tsi is with suffix, the suffix is stripped.
		 *
		 * @param tsiToConvert		The Tsi that is to be converted
		 * @return					the converted Tsi
		 */
		std::string getTsiWithoutSuffix(const std::string& tsiToConvert);

    private:
        /**
        * Send the method to the radio and get the reply event.  This method simply calls the parent (m_radio) sendMethod.
        * @param    method the RadioMethod to send
        * @return   the RadioEvent that the RCS replied with
        */
        RadioEvent * sendMethod(RadioMethod & method);


        /**
         * Cute utility function to throw a RadioFallbackException
         * if m_radioSvr.inFallback() returns true
         */
        void checkRadioFallback();


        ///
        /// RadioTcpServer - used to send out the radio messages
        RadioTcpServer & m_radioSvr;


        ///
        /// RadioTcpServer API session reference
        SessionReference m_sessionRef;

    };
}

#endif // #ifndef RADIO_SESSION_HELPER_H
