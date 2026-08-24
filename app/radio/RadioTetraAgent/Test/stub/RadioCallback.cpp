/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/Test/stub/RadioCallback.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
#include "RadioCallback.h"




void RadioCallback:: onCallStatusChange(const TA_IRS_App::CallDetailsType& details) {}


void RadioCallback:: onTrainEvent(std::string trainTSI,std::vector<unsigned char> messageData){}


void RadioCallback:: onRadioConnectionFailure(bool isPrimary){}

void RadioCallback:: onRadioConnectedRestored(bool isPrimary){}


void RadioCallback:: onTXChange(const TA_IRS_App::TXStateInfo txInfo){}


void RadioCallback:: onAuthorisationChange(const TA_IRS_App::EAction action,const TA_IRS_App::AuthorisationRequestType& details){}


void RadioCallback:: onSubscriberActivity(const TA_Base_Bus::IRadioCorbaDef::MonitoredCallDetailsType & callDetails){}


void RadioCallback::submitAlarm( const TA_Base_Core::MessageType& type, const TA_Base_Core::DescriptionParameters& dp )  {}
