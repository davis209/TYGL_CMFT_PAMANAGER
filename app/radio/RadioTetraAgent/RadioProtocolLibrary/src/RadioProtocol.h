/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioProtocol.h $
  * @author:  Andy Parker
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/05/18 10:25:59 $
  * Last modified by:  $Author: builder $
  *
  * This header file defines the common types and constants used by the Marconi radio
  * protocol.  Only the library should include this file to ensure that the Marconi
  * specifics are separated from the Transactive applications.
  *
  */
///////////////////////////////////////////////////////////
//
//  RadioProtocol.h
//  This header defines all the radio system types and
//  constants
//  Created on:      04-Nov-2003 13:28:33
//  Original author: Andy Parker
//  
///////////////////////////////////////////////////////////
//  Modification history:
//  
//
///////////////////////////////////////////////////////////

#if !defined(RadioProtocol_H)
#define RadioProtocol_H

#include "core/types/src/ta_types.h"
#include "RadioSDSTLTypes.h"
namespace TA_IRS_App
{
	// Type details
	typedef ta_uint32 RadioEventType;
    typedef ta_uint32   RadioReference;
    typedef RadioReference CallReference;
    typedef RadioReference SDSReference;
    typedef RadioReference SessionReference;
    typedef RadioReference MonitorReference;
    typedef RadioReference CADReference;
    typedef RadioReference SubscriberReference;
    typedef RadioReference AudioReference;
    typedef RadioReference SearchResultReference;

    const SessionReference InternalUnassignedReference = 0xFFFFFFFF;

    #define EVENT_INCOMING_SDS_PARAMETER_LENGTH  261

	#define SHORTSTRING_LENGTH    32 

	// Radio Method Definitions
	#define METHOD_ATTACH_SESSION      2
	#define METHOD_DETACH_SESSION      3
	#define METHOD_INITIALISE          5
	#define METHOD_LOGIN               7
	#define METHOD_LOGOUT              8
    #define METHOD_QUERY_REFERENCE    10
    #define METHOD_CHANGE_REFERENCE   11
    #define METHOD_SEARCH_SUBSCRIBER  12
	#define METHOD_NEW_REFERENCE      13
    #define METHOD_TEXT_2SR           14
    #define METHOD_DELETE_REFERENCE   15
	#define METHOD_DELETE_SUBSCRIBER  16
	#define METHOD_SELECT             17
	#define METHOD_DESELECT           18
	#define METHOD_DEMAND_TX          19
	#define METHOD_CEASE_TX           20
	#define METHOD_SETUP_CALL         21
	#define METHOD_ANSWER_CALL        22
	#define METHOD_DISCONNECT         23
	#define METHOD_SEND_SDS           24
	#define METHOD_ATTACH_AUDIO       25
	#define METHOD_DETACH_AUDIO       26
	#define METHOD_MONITORSUBSCRIBER  27
	#define METHOD_FORCECALLTERMINATION 28
	#define METHOD_MONITORCALL        29
	#define METHOD_INCLUDE            32
	#define METHOD_AUTHORISE_CALL     33
    #define METHOD_GET_GROUP_DETAILS  35
    #define METHOD_CONVERT_TO_DB_TIME 38
    #define METHOD_ATTACHTOGROUP      39
	#define METHOD_SENDCIRCUITDATA    41
    #define METHOD_SENDDTMF                 47
	#define METHOD_TEXT_TO_REFERENCE  48
	#define METHOD_ATTACHMONITORAUDIO 50
	#define METHOD_JOIN               51
	#define METHOD_DETACHMONITORAUDIO 52
    #define METHOD_TLSDS_TRANSFER           53
    #define METHOD_TLSDS_REPORT             54
    #define METHOD_TLSDS_ACK                55
    #define METHOD_SIMPLE_TEXT_SDS          56
    #define METHOD_GENERATE_TONE_SEQUENCE   57
    #define METHOD_STOP_TONE_SEQUENCE       58
    #define METHOD_CONVERTTOTIME            59
    #define METHOD_GETLASTERRROR            60
    #define METHOD_REGISTERFORNOTIFY        61
    #define METHOD_SIMPLE_GPS_SDS           66
    #define METHOD_ATTACH_SESSION_EXT       88
	#define METHOD_CHECKCONNECTION          77
	#define METHOD_USERDEFINED_SDS			67 //++Noel
	#define METHOD_CHANGE_PASSWORD			68

	// Radio Event Definitions
	#define EVENT_SYSTEM_ERROR       0xA000
	#define EVENT_INCOMING_CALL      0xA001
	#define EVENT_INCOMING_SDS       0xA002
	#define EVENT_CALL_STATUS        0xA003
	#define EVENT_SUBSCRIBERACTIVITY 0xA004
	#define EVENT_INCOMINGCIRCUITDATA 0xA005
	#define EVENT_CIRCUITDATACAPACITY 0xA006
    #define EVENT_REQUEST_AUTHORISE             0xA009
    #define EVENT_GROUPCALLACK                  0xA00A
    #define EVENT_INCOMING_TLSDS_TRANSFER       0xA010
    #define EVENT_INCOMING_TLSDS_REPORT         0xA011
    #define EVENT_INCOMING_TLSDS_ACK            0xA012
    #define EVENT_INCOMING_TLSDS_TRANSFER_EX    0xA016
    #define EVENT_INCOMING_SIMPLE_TEXT_SDS      0xA013
	#define EVENT_KEEPALIVE                     0xA022
	#define EVENT_INCOMING_USERDEFINED_SDS		0xA020 //++Noel
	#define EVENT_PTT_STATUS					0xA050 


	#define EVENT_ATTACH_SESSION     0x8000 + METHOD_ATTACH_SESSION 
	#define EVENT_DETACH_SESSION     0x8000 + METHOD_DETACH_SESSION
	#define EVENT_INITIALISE         0x8000 + METHOD_INITIALISE 
	#define EVENT_LOGIN              0x8000 + METHOD_LOGIN 
    #define EVENT_QUERY_REFERENCE    0x8000 + METHOD_QUERY_REFERENCE
    #define EVENT_CHANGE_REFERENCE   0x8000 + METHOD_CHANGE_REFERENCE
    #define EVENT_SEARCH_SUBSCRIBER  0x8000 + METHOD_SEARCH_SUBSCRIBER
	#define EVENT_NEW_REFERENCE      0x8000 + METHOD_NEW_REFERENCE 
    #define EVENT_TEXT_2SR           0x8000 + METHOD_TEXT_2SR
    #define EVENT_DELETE_REFERENCE   0x8000 + METHOD_DELETE_REFERENCE
    #define EVENT_DELETE_SUBSCRIBER  0x8000 + METHOD_DELETE_SUBSCRIBER
	#define EVENT_SETUP_CALL         0x8000 + METHOD_SETUP_CALL 
	#define EVENT_DISCONNECT         0x8000 + METHOD_DISCONNECT 
	#define EVENT_SEND_SDS           0x8000 + METHOD_SEND_SDS
    #define EVENT_AUTHORISE_CALL     0x8000 + METHOD_AUTHORISE_CALL
    #define EVENT_GET_GROUP_DETAILS  0x8000 + METHOD_GET_GROUP_DETAILS
    #define EVENT_CONVERT_TO_DB_TIME 0x8000 + METHOD_CONVERT_TO_DB_TIME
	#define EVENT_SELECT             0x8000 + METHOD_SELECT          
	#define EVENT_DESELECT           0x8000 + METHOD_DESELECT          
	#define EVENT_DEMAND_TX          0x8000 + METHOD_DEMAND_TX         
	#define EVENT_CEASE_TX           0x8000 + METHOD_CEASE_TX 
    #define EVENT_ATTACHTOGROUP      0x8000 + METHOD_ATTACHTOGROUP
	#define EVENT_SENDCIRCUITDATA    0x8000 + METHOD_SENDCIRCUITDATA
	#define EVENT_TEXT_TO_REFERENCE  0x8000 + METHOD_TEXT_TO_REFERENCE 
	#define EVENT_MONITORSUBSCRIBER  0x8000 + METHOD_MONITORSUBSCRIBER
	#define EVENT_FORCECALLTERMINATION 0x8000 + METHOD_FORCECALLTERMINATION
	#define EVENT_MONITORCALL        0x8000 + METHOD_MONITORCALL
	#define EVENT_INCLUDE            0x8000 + METHOD_INCLUDE 
	#define EVENT_ATTACHMONITORAUDIO 0x8000 + METHOD_ATTACHMONITORAUDIO 
	#define EVENT_JOIN               0x8000 + METHOD_JOIN
	#define EVENT_DETACHMONITORAUDIO 0x8000 + METHOD_DETACHMONITORAUDIO 
    #define EVENT_SDSTL_ACK             0x8000 + METHOD_TLSDS_ACK
    #define EVENT_SDSTL_TRANSFER        0x8000 + METHOD_TLSDS_TRANSFER
    #define EVENT_SDSTL_REPORT          0x8000 + METHOD_TLSDS_REPORT
    #define EVENT_SIMPLE_TEXT_SDS       0x8000 + METHOD_SIMPLE_TEXT_SDS
    #define EVENT_GETLASTERRROR         0x8000 + METHOD_GETLASTERRROR
    #define EVENT_ATTACH_SESSION_EXT    0x8000 + METHOD_ATTACH_SESSION_EXT
    #define EVENT_CHECKCONNECTION       0x8000 + METHOD_CHECKCONNECTION
	#define EVENT_USER_DEFINED_SDS		0x8000 + METHOD_USERDEFINED_SDS	
	#define EVENT_CHANGE_PASSWORD		0x8000 + METHOD_CHANGE_PASSWORD

	//***************
	// Status Codes *
	//***************
	
	// Information
	#define STATUS_OK                            0
	#define STATUS_DEFERED                       1
	#define STATUS_DEMANDTX_QUEUED               2
	#define STATUS_DEMANDTX_TIMEOUT              3
	#define STATUS_TXTAKEN                       4
	#define STATUS_QUEUED                        6
   	#define STATUS_ALERTED                       7
	#define STATUS_CHANGED                       8
	#define STATUS_OK_FALLBACK                   9
	#define STATUS_OK_MORE_DATA_AVAILABLE       10
	#define STATUS_OK_PARTIAL_TRANSFER          11
	#define STATUS_OK_INCLUDE_DISABLED          12
	#define STATUS_INTERCEPTED                  13
	#define STATUS_ACCEPTED                     14
	#define STATUS_REJECTED                     15
	#define STATUS_DIVERTED                     16
	#define STATUS_ANSWERED                     17
	#define STATUS_RESTARTED					24
	#define STATUS_STARTED						25
	#define STATUS_STOPPED						26

	// Errors
	#define STATUS_KO_SESSION_NOT_FOUND				-1
	#define STATUS_KO_DEMAND_TX_FAILED				-2
	#define STATUS_KO_SWITCH_NOT_REACHABLE          -3
	#define STATUS_KO_INVALID_AUDIO_LINK			-4
	#define STATUS_KO_NOT_INITITALISED				-5
	#define STATUS_KO_BAD_USERNAME					-6
	#define STATUS_KO_BAD_PASSWORD					-7
	#define STATUS_KO_NOT_LOGGED					-8
	#define STATUS_KO_NOT_ALLOWED					-9
	#define STATUS_KO_INVALID_REFERENCE				-10
	#define STATUS_KO_INVALID_KEY					-11
	#define STATUS_KO_CHANGE_REFERENCE				-12
	#define STATUS_KO_NO_RESOURCE					-13
	#define STATUS_KO_SETUP_CALL_FAILED				-14
	#define STATUS_KO_SUBSCRIBER_NOT_FOUND          -15
	#define STATUS_KO_DEST_NOT_VALID				-16
	#define STATUS_KO_SRC_NOT_VALID					-17
	#define STATUS_KO_INCLUDE_FAILED				-18
	#define STATUS_KO_SC_NOT_CREATED				-19
	#define STATUS_KO_GWLD_LINK_LOST				-20
	#define STATUS_KO_INVALID_REFERENCE_TYPE		-21
	#define STATUS_KO_INVALID_FIELD					-22
	#define STATUS_KO_SUBSCRIBER_ALREADY_EXISTS     -23
	#define STATUS_KO_INVALID_GROUP					-24
	#define STATUS_KO_OPERATOR_NOT_ATTACHED         -25
	#define STATUS_KO_ATTACH_FAILED					-26
	#define STATUS_KO_DETACH_FAILED					-27
	#define STATUS_KO_QUEUE_FULL					-28
	#define STATUS_KO_INCOMPATIBLE_VERSION          -29
	#define STATUS_KO_MESSAGE_TOO_LONG				-30
	#define STATUS_KO_DBMS_ERROR					-31
	#define STATUS_KO_SEND_SDS_FAILED				-32
	#define STATUS_KO_AUTHORISE_FAIL				-37
	#define STATUS_KO_REJECTED						-38
	#define STATUS_KO_RTT_SERVER_NOT_CONNECTED      -39
	#define STATUS_KO_INVALID_NUMBER_OF_REF         -40
    #define STATUS_KO_TOO_MANY_RECORDS_FOUND        -43
	#define STATUS_KO_HW_NOT_ALLOCATED				-41
	#define STATUS_KO_AUDIOLINK_NOTAVAILABLE		-54
	#define STATUS_KO_METHOD_TIMEOUT				-60
    #define STATUS_KO_CHANGE_PASSWORD               -61
	#define STATUS_KO_BAD_CATEGORY					-138
	#define STATUS_KO_NE_NACK						-6666
	#define STATUS_KO_SERVER_NOT_FOUND				-7777
	#define STATUS_KO_TOO_MANY_ATTACH_SESSIONS      -8888
    #define STATUS_KO_PARTIAL_TRANSFER				-9999


}
 

#endif // !defined(RadioProtocol_H)
