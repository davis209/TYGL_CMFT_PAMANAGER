#ifndef IRADIOCALLBACK_H
#define IRADIOCALLBACK_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/IRadioCallback.h $
  * @author  A. Parker
  * @version $Revision: #2 $
  * Last modification : $DateTime: 2009/05/18 10:25:59 $
  * Last modified by : $Author: builder $
  *
  * This interface defines the methods used to notify events from the 
  * Radio class.
  */
#include "core/types/src/ta_types.h"
#include "RadioTypes.h"
#include "bus/radio/RadioAgentCorbaDef_4669/idl/src/IRadioCorbaDef.h"
#include <string>

#include "core/alarm/src/AlarmHelperManager.h"
#include "core/alarm/src/AlarmHelper.h"

namespace TA_IRS_App
{
	enum ETXState {TX_GRANTED,TX_QUEUE,TX_CEASE,TX_TIMEOUT};
	enum PTTBtnState {PTT_PRESSED = 1, PTT_RELEASED};
	struct TXStateInfo
	{
		ETXState        state;
		ta_int32             callID;         // ID for the call
		ta_int32             sessionRef;     // Session associated with notification
		ta_uint32     transactionID;  // O if unsolicited.
		std::string     userITSI;       // Source ITSI (Individual Tetra Subscriber Identity)
		std::string     tpiITSI;        // Talking Party Identification ITSI
	};

	struct PTTBtnStateInfo 
	{
		PTTBtnState state;
		long            sessionRef;     // Session associated with notification
		unsigned long   consoleKey;
	};

	class IRadioCallback 
	{
		public:
			// Called when the status of a call changes (or new call)
			virtual void onCallStatusChange(const TA_IRS_App::CallDetailsType& details) = 0;

			// Called when the status of a call changes (or new call)
			virtual void onTrainEvent(std::string trainTSI,std::vector<unsigned char> messageData) = 0;

			// Called when the TX status is advised by the radio
			virtual void onTXChange(const TXStateInfo txInfo) = 0;

			// Called when Radio Call Forwarding settings change
			virtual void updateGX(std::string sessionId, std::string message, ConsoleID consoleId) = 0;

            // Radio Monitor Subscriber Activity
            virtual void onSubscriberActivity(TA_IRS_App::MonitoredCallDetailsType* callDetails) = 0;

            // Submit an alarm
            virtual void submitAlarm( const TA_Base_Core::MessageType& type, const TA_Base_Core::DescriptionParameters& dp ) = 0;

			virtual void closeAlarm( const TA_Base_Core::MessageType& type) =0;

            // Radio Fallback notification
            virtual void onRadioFallback(bool isFallback) = 0;

            // Radio Circuit Mode notifications
            virtual void onCircuitModeActivity(const char * ITSI, bool successful, const char * failReason) = 0;

			virtual void onPTTBtnStatus(const PTTBtnStateInfo pttBtnInfo) = 0;

			virtual void sendCommsMgsNewCallStack(const char* profile, ta_uint32 locationKey, ta_uint32 timestamp, TA_Base_Bus::IRadioCorbaDef::EOriginType originType, 
				TA_Base_Bus::IRadioCorbaDef::EStackCallType callType, const char* identifier, const char* location, bool isEmergency, const char* tsi) = 0;

			virtual void sendCommsMgsDeleteCallStack(ta_uint32 timestamp, const std::string tsi) = 0;

	};   // class IRadioCallback
};  // Namespace TA_IRS_App

#endif
