#ifndef RADIOCORBATYPECONVERTER_H
#define RADIOCORBATYPECONVERTER_H

/*
 *	Converts between corba and non-corba types
 */

// TD14696

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/radio/RadioAgentCorbaDef_4669/idl/src/IRadioCorbaDef.h"
#include "bus/radio/RadioAgentCorbaDef_4669/idl/src/IRadioSessionCorbaDef.h"
#include "bus/radio/RadioAgentCorbaDef_4669/idl/src/IRadioStateSynchronisationCorbaDef.h"
#include "RadioTypes.h"
#include "RadioSynchronisationTypes.h"

namespace TA_IRS_App
{
    class RadioCorbaTypeConverter  
    {
        public:
            
            /*
             *	Corba to Normal types
             */


            static void convertERadioUserTypeCorbaToNormal(const TA_Base_Bus::IRadioSessionCorbaDef::ERadioUserType& radioUserTypeCorba,
                                                                                    ERadioUserType& radioUserType);

            static void convertEPTTStateCorbaToNormal(const TA_Base_Bus::IRadioCorbaDef::EPTTState& pttStateCorba,
                                                                           EPTTState& pttState);

            static void convertCallParticipantCorbaToNormal(const TA_Base_Bus::IRadioCorbaDef::CallParticipant& callParticipantCorba,
                                                                                 CallParticipant& callParticipant);

            static void convertECallCategoryCorbaToNormal(const TA_Base_Bus::IRadioCorbaDef::ECallCategory& callCategoryCorba,
                                                                               ECallCategory& callCategory);

            static void convertECallSourceCorbaToNormal(const TA_Base_Bus::IRadioCorbaDef::ECallSource& callSourceCorba,
                                                                             ECallSource &callSource);

            static void convertECallTypeCorbaToNormal(const TA_Base_Bus::IRadioCorbaDef::ECallType& callTypeCorba,
                                                                           ECallType& callType);

            static void convertCallDetailsTypeCorbaToNormal(const TA_Base_Bus::IRadioCorbaDef::CallDetailsType& callDetailsCorba,
                                                                                 CallDetailsType& callDetails);

            static void convertECallStatusTypeCorbaToNormal(const TA_Base_Bus::IRadioCorbaDef::ECallStatusType& callStatusTypeCorba,
                                                                                 ECallStatusType& callStatusType);
            static void convertESessionStateCorbaToNormal(const TA_Base_Bus::IRadioStateSynchronisationCorbaDef::ESessionState& sessionStateCorba,
                                                                                 ESessionState& sessionState);
                            
            static ECallPriority convertECallPriorityCorbaToNormal(const TA_Base_Bus::IRadioSessionCorbaDef::ECallPriority& callPriorityCorba);

			static void convertEOriginTypeCorbaToNormal(const TA_Base_Bus::IRadioCorbaDef::EOriginType& originTypeCorba,
																				EOriginType& originType);

			static void convertEStackCallTypeCorbaToNormal(const TA_Base_Bus::IRadioCorbaDef::EStackCallType& stackCallTypeCorba,
																				EStackCallType& stackCallType);
                    
			static void convertSubscriberNotificationTypeCorbaToNormal(const TA_Base_Bus::IRadioCorbaDef::SubscriberNotificationType& notifyTypeCorba,
																				SubscriberNotificationType& notifyType);

			static void convertSubscriberTypeCorbaToNormal(const TA_Base_Bus::IRadioCorbaDef::SubscriberType& corbaType,
				SubscriberType& subType);
            
            /*
             *	Normal to Corba types
             */

            static void convertERadioUserTypeNormalToCorba(const ERadioUserType& radioUserType,
                                                                                TA_Base_Bus::IRadioSessionCorbaDef::ERadioUserType& radioUserTypeCorba );

            static void convertEPTTStateNormalToCorba(const EPTTState& pttState,
                                                                           TA_Base_Bus::IRadioCorbaDef::EPTTState& pttStateCorba);

            static void convertCallParticipantNormalToCorba(const CallParticipant& callParticipant,
                                                                                 TA_Base_Bus::IRadioCorbaDef::CallParticipant& callParticipantCorba);

            static void convertECallCategoryNormalToCorba(const ECallCategory& callCategory,
                                                                               TA_Base_Bus::IRadioCorbaDef::ECallCategory& callCategoryCorba);

            static void convertECallSourceNormalToCorba(const ECallSource &callSource,
                                                                             TA_Base_Bus::IRadioCorbaDef::ECallSource& callSourceCorba);

            static void convertECallTypeNormalToCorba(const ECallType& callType,
                                                                           TA_Base_Bus::IRadioCorbaDef::ECallType& callTypeCorba);

            static void convertCallDetailsTypeNormalToCorba(const CallDetailsType& callDetails,
                                                                                 TA_Base_Bus::IRadioCorbaDef::CallDetailsType& callDetailsCorba);

            static void convertECallStatusTypeNormalToCorba(const ECallStatusType& callStatusType,
                                                                                 TA_Base_Bus::IRadioCorbaDef::ECallStatusType& callStatusTypeCorba);

            static void convertESessionStateNormalToCorba(const ESessionState& sessionState,
                                                                                 TA_Base_Bus::IRadioStateSynchronisationCorbaDef::ESessionState& sessionStateCorba);
                

            static TA_Base_Bus::IRadioSessionCorbaDef::ECallPriority convertECallPriorityNormalToCorba(const ECallPriority& callPriority);

            static TA_Base_Bus::IRadioCorbaDef::EMonitorStatusType convertMonitorStatusTypeToCorba(TA_IRS_App::EMonitorStatusType normalStatusType);

			static void convertEOriginTypeNormalToCorba(const EOriginType& originType,
																				TA_Base_Bus::IRadioCorbaDef::EOriginType& originTypeCorba);

			static void convertEStackCallTypeNormalToCorba(const EStackCallType& stackCallType,
																				TA_Base_Bus::IRadioCorbaDef::EStackCallType& stackCallTypeCorba);

			static void convertSubscriberNotificationTypeNormalToCorba(const SubscriberNotificationType& normalObject,
																				TA_Base_Bus::IRadioCorbaDef::SubscriberNotificationType& notifyTypeCorba);

			static void convertSubscriberTypeNormalToCorba(const SubscriberType& normalObject,
				TA_Base_Bus::IRadioCorbaDef::SubscriberType& corbaObject);

			static TA_Base_Bus::IRadioSessionCorbaDef::EMonitorStatusType convertEMonitorStatusTypeToCorba(TA_IRS_App::EMonitorStatusType monStatus);
    };		
}

#endif 
