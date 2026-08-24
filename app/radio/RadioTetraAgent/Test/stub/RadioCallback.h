/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/Test/stub/RadioCallback.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
#include "IRadioCallback.h"




class RadioCallback : public TA_IRS_App::IRadioCallback
{
	public:
		// Called when the status of a call changes (or new call)
		virtual void onCallStatusChange(const TA_IRS_App::CallDetailsType& details);

		// Called when the status of a call changes (or new call)
		virtual void onTrainEvent(std::string trainTSI,std::vector<unsigned char> messageData);

		// Radio Communications Alarms
		virtual void onRadioConnectionFailure(bool isPrimary);
		virtual void onRadioConnectedRestored(bool isPrimary);

		// Called when the TX status is advised by the radio
        virtual void onTXChange(const TA_IRS_App::TXStateInfo txInfo);

		// Called when the authorisation stack is updated
		virtual void onAuthorisationChange(const TA_IRS_App::EAction action,const TA_IRS_App::AuthorisationRequestType& details);

        // Radio Monitor Subscriber Activity
        virtual void onSubscriberActivity(const TA_Base_Bus::IRadioCorbaDef::MonitoredCallDetailsType & callDetails);

        virtual void submitAlarm( const TA_Base_Core::MessageType& type, const TA_Base_Core::DescriptionParameters& dp );
};