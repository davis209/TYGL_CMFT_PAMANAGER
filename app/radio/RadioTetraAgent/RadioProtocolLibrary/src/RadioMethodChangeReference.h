/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodChangeReference.h $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class encapsulates the Radio API Change Reference method.
  *
  */

///////////////////////////////////////////////////////////
//  RadioMethodChangeReference.h
//  Implementation of the Class RadioMethodChangeReference
//  Created on:      29-Nov-2003 09:57:09 AM
///////////////////////////////////////////////////////////

#if !defined(RadioMethodChangeReference_B874D102_C2B6_4e5a_A4DC_D9D9081591EF__INCLUDED_)
#define RadioMethodChangeReference_B874D102_C2B6_4e5a_A4DC_D9D9081591EF__INCLUDED_

#include "app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodCallReference.h"
#include "app/radio/RadioTetraAgent/RadioProtocolLibrary/src/SubscriberDataTypes.h"


#include <vector>
#include <map>
namespace TA_IRS_App
{
    struct CallForwardingDetails; // Forward declaration

	class RadioMethodChangeReference : public RadioMethodCallReference 
	{
		public:
            enum ESearchType
            {
                FULL,
                NORMAL
            };
			
            typedef enum MicrophoneState { MicOn, MicOff, MicNoChange };

			RadioMethodChangeReference(RadioSessionReference sessionRef, RadioCallReference callRef, bool isCallRef=false);
			virtual ~RadioMethodChangeReference();

            void setForwardingDetails(const CallForwardingDetails& details);

            void setPatchDetails(const std::string& controllingITSI,
								 const std::vector<std::string>& patchList,
                                 const std::string& patchName);

			void setGroupMembers(const std::string& memberList);

            void setSearchType(const ESearchType searchType);
	
			void setAudioVolume(int leftVolume,int rightVolume, MicrophoneState micState);

			virtual void setCharacterEncoding(ECodePage codePage);
			
			virtual void setDefaultCharacterEncoding(ECodePage codePage);

			//This will set the subscriber management status to enable/disable
			void setSubsriberMngemtStatus(SubscDataTypes::RadioSubMngmentStatus subStatus);

			//TD14614
			/**
			  * <description>
			  * Disables the DGNA Supplementary Service.
			  * @return void disableDynamicGroup 
			  * 
			  * @exception <exceptions> Optional
			  */
			void disableDynamicGroup ();  

			/**
			  * <description>
			  * Enables the DGNA Supplementary Service.
			  * @return void enableDynamicGroup 
			  * 
			  * @exception <exceptions> Optional
			  */
			void enableDynamicGroup ();  
			

			/**
			  * <description>
			  * create the dynamic group based on the name and the itsi
			  * @return void createDynamicGroup 
			  * @param : const std::string& name
			  * @param : const std::string& itsi
			  * 
			  * @exception <exceptions> Optional
			  */
			void createDynamicGroup (const std::string& name , const std::string& itsi, const std::string& memberList, bool encodeName = true);
			//TD14614
			void updateDynamicGroup(const std::string& name, const std::string& memberList, bool encodeName = true);

			//TD15902
			// ScnBaseStationMap: a set of <SCN,BS> pairs
			typedef std::multimap<long, long> ScnBaseStationMap;
			void changeRegionalMembership(const ScnBaseStationMap& scnMap);
			//TD15902

            /** 
              * getSerialisationLevel
              *
              * This gets the level of serialisation that the agent must be set to
              * for this method to be serialised.
              *
              * @return a level (1)
              */
            virtual unsigned char getSerialiseTransactionLevel() const;

			void setEmuDataCallIPAddress(const std::string& destIPAdress);
		private:
			RadioMethodChangeReference(const RadioMethodChangeReference& theRadioMethodChangeReference);
            
            static const unsigned int CHANGE_REFERENCE_BASE_LENGTH;
            static const unsigned int DETAILS_PARAMETER_START_POSITION;

            // String keys
            static const std::string ENABLE_FORWARDING;
            static const std::string DISABLE_FORWARDING;
            static const std::string FORWARD_UNCONDITIONAL;
            static const std::string FORWARD_NOT_REACHABLE;
            static const std::string TYPE;
            static const std::string PATCH_ID;
            static const std::string OWNER_DISPATCHER;
            static const std::string PATCH_HANG_TIME;
            static const std::string MEMBERS;
            static const std::string SEARCH_MODE;
			static const std::string LEFT_VOLUME;
			static const std::string RIGHT_VOLUME;
			static const std::string MIC_ON;
			static const std::string MIC_OFF;
			static const std::string CODE_PAGE;
			static const std::string DEFAULT_CODE_PAGE;
            // specific key values
            static const std::string PATCH_TYPE;
            static const std::string FULL_SEARCH;
            static const std::string NORMAL_SEARCH;

            // Constant values that will be replaced
            static const std::string PATCH_HANG_TIME_VALUE;

            // Constant protocol values (defined as a constant so any changes to the protocol down the line don't 
            // hurt us)
            static const std::string KEY_SEPARATOR;
            static const std::string VECTOR_SEPARATOR;
			//TD14614
			static const std::string DISABLE_DGNA; 
			static const std::string ENABLE_DGNA; 
			static const std::string TSI; 
			static const std::string NAME; 
			static const std::string MNEMONIC;
			static const std::string PERMISSION; 
			static const std::string SUBSCRIPTION_STATUS; 
			static const int         DGNA_NAME_MAX_LENGTH; 
			//TD14614

			//TD15902
			static const std::string GROUP_OPERATING_AREA;
			static const std::string ENABLE_GOA;
			static const std::string DETAILS_SEPARATOR;
			//TD15902
			static const std::string SUBSRIBER_STATUS;
			//Emulated Circuit Data Call
			static const std::string CIRCUIT_DATAMODE;
			static const std::string ECID_DEST_IPADDRESS;

			static const std::string DGNA_ORG_ID1;
			static const std::string DGNA_ORG_ID2;
			static const std::string DGNA_ORG_ID3;
			static const std::string DGNA_ORG_ID4;
			static const std::string DGNA_ORG_ID5;
	};
};
#endif // !defined(RadioMethodChangeReference_B874D102_C2B6_4e5a_A4DC_D9D9081591EF__INCLUDED_)
