/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodChangeReference.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class encapsulates the Radio API SendSDS method.
  *
  */
///////////////////////////////////////////////////////////
//  RadioMethodChangeReference.cpp
//  Implementation of the Class RadioMethodChangeReference
//  Created on:      29-Nov-2003 09:57:09 AM
///////////////////////////////////////////////////////////

#include "RadioMethodChangeReference.h"
#include "RadioProtocol.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/TransactiveException.h"
#include "app/radio/RadioTetraAgent/src/CallForwardingDetails.h"

#include "ACEXML/common/Transcode.h"
#include <sstream>
#include <algorithm>

#define RADIOMETHODCHANGEREFERENCE_SPRINTF_DEPRECATED 1

namespace TA_IRS_App
{   
    const unsigned int RadioMethodChangeReference::CHANGE_REFERENCE_BASE_LENGTH     = 4;
    const unsigned int RadioMethodChangeReference::DETAILS_PARAMETER_START_POSITION = 4;

    const std::string RadioMethodChangeReference::DETAILS_SEPARATOR = ",";
    const std::string RadioMethodChangeReference::KEY_SEPARATOR     = "\n";
    const std::string RadioMethodChangeReference::VECTOR_SEPARATOR  = ";";

    // SubscriberReference keys
    const std::string RadioMethodChangeReference::TSI                  = "K_TSI";
    const std::string RadioMethodChangeReference::NAME                 = "K_NAME";
	const std::string RadioMethodChangeReference::MNEMONIC			   = "K_MNEMONIC";
    const std::string RadioMethodChangeReference::PERMISSION           = "K_PERMISSIONS";
    const std::string RadioMethodChangeReference::SUBSCRIPTION_STATUS  = "K_SUBSCRIPTION_STATUS";

    const std::string RadioMethodChangeReference::DISABLE_DGNA         = "K_SUPPLEMENTARY_SERVICES=-Y";
    const std::string RadioMethodChangeReference::ENABLE_DGNA          = "K_SUPPLEMENTARY_SERVICES=+Y";
    const int RadioMethodChangeReference::DGNA_NAME_MAX_LENGTH         = 32; //ref TcpServer Charpter7.5.4

    const std::string RadioMethodChangeReference::GROUP_OPERATING_AREA = "K_GROUP_OP_AREA=";
    const std::string RadioMethodChangeReference::ENABLE_GOA           = "K_SUPPLEMENTARY_SERVICES=+G";

    const std::string RadioMethodChangeReference::ENABLE_FORWARDING    = "K_SUPPLEMENTARY_SERVICES=+W";
    const std::string RadioMethodChangeReference::DISABLE_FORWARDING   = "K_SUPPLEMENTARY_SERVICES=-W";
    const std::string RadioMethodChangeReference::FORWARD_UNCONDITIONAL= "K_FORWARD_UNCONDITIONAL=";
    const std::string RadioMethodChangeReference::FORWARD_NOT_REACHABLE= "K_FORWARD_NOT_REACHABLE=";
    const std::string RadioMethodChangeReference::TYPE                              = "K_TYPE";

    // For SubscriberReference K_TYPE is Patch
    const std::string RadioMethodChangeReference::PATCH_ID             = "K_PATCH_ID";
    const std::string RadioMethodChangeReference::OWNER_DISPATCHER     = "K_OWNER_DISPATCHER";
    const std::string RadioMethodChangeReference::PATCH_HANG_TIME      = "K_PATCH_HANG_TIMER";

    // SearchReference keys
    const std::string RadioMethodChangeReference::MEMBERS              = "K_MEMBERS";
    const std::string RadioMethodChangeReference::SEARCH_MODE          = "K_SEARCH_MODE";

    // AudioReference keys
    const std::string RadioMethodChangeReference::LEFT_VOLUME          = "K_VOLUME_L";
    const std::string RadioMethodChangeReference::RIGHT_VOLUME         = "K_VOLUME_R";
    const std::string RadioMethodChangeReference::MIC_ON               = "K_MIC_ON=Y";
    const std::string RadioMethodChangeReference::MIC_OFF              = "K_MIC_ON=N";

    // SdsReference and SessionReference
    const std::string RadioMethodChangeReference::CODE_PAGE            = "K_CODEPAGE";
    const std::string RadioMethodChangeReference::DEFAULT_CODE_PAGE    = "K_DEFAULT_CODEPAGE";

    // specific key values
    // For SubscriberReference K_TYPE is Patch
    const std::string RadioMethodChangeReference::PATCH_TYPE           = "P";
    const std::string RadioMethodChangeReference::PATCH_HANG_TIME_VALUE= "0";

    // SessionReference, K_SEARCH_MODE values
    const std::string RadioMethodChangeReference::FULL_SEARCH          = "FULL";
    const std::string RadioMethodChangeReference::NORMAL_SEARCH        = "NORMAL";

	//Attribute for Subscriber Management
	const std::string RadioMethodChangeReference::SUBSRIBER_STATUS     = "K_SUBSCRIBER_STATUS=";

	//Emulated Circuit Data Call
	const std::string RadioMethodChangeReference::CIRCUIT_DATAMODE		= "K_CIRCUIT_DATA_MODE=";
	const std::string RadioMethodChangeReference::ECID_DEST_IPADDRESS   = "K_ECD_DEST_IP_ADDR=";

	const std::string RadioMethodChangeReference::DGNA_ORG_ID1 = "K_ORG_ID1";
	const std::string RadioMethodChangeReference::DGNA_ORG_ID2 = "K_ORG_ID2";
	const std::string RadioMethodChangeReference::DGNA_ORG_ID3 = "K_ORG_ID3";
	const std::string RadioMethodChangeReference::DGNA_ORG_ID4 = "K_ORG_ID4";
	const std::string RadioMethodChangeReference::DGNA_ORG_ID5 = "K_ORG_ID5";

	std::string convertWideChar(unsigned short w)
	{
		unsigned char highByte = static_cast<unsigned char>((w & 0xFF00) >> 8);
		unsigned char lowByte = static_cast<unsigned char>(w & 0xFF);
		char ret[8] = { 0 };
		sprintf(ret, "\\%02X%02X", highByte, lowByte);
		return ret;

	}

    RadioMethodChangeReference::RadioMethodChangeReference(RadioSessionReference sessionRef, RadioCallReference callRef, bool isCallRef/*=false*/) 
                       : RadioMethodCallReference(CHANGE_REFERENCE_BASE_LENGTH,METHOD_CHANGE_REFERENCE
                                                  ,sessionRef,callRef)
    {	
		if (false == isCallRef)
			setParameterLong( 0, callRef ); // Override for none call reference
    }



    RadioMethodChangeReference::~RadioMethodChangeReference()
    {
    }


    void RadioMethodChangeReference::setForwardingDetails(const CallForwardingDetails& details)
    {
		FUNCTION_ENTRY("setForwardingDetails");
        // Determine if forwarding has been enabled or disabled
        if (TA_IRS_App::CallForwardingDetails::NeverForward != details.forwardAction)
        {
            // If enabled, start with the enable string
            std::string detailsString = ENABLE_FORWARDING + KEY_SEPARATOR;

            // Then add the specification for the TYPE of forwarding
			detailsString += FORWARD_NOT_REACHABLE;
            if (TA_IRS_App::CallForwardingDetails::ForwardNotReachable == details.forwardAction)
            {
				detailsString += std::string(details.destinationTSI) + VECTOR_SEPARATOR + std::string(details.destinationTSI);
            }
			else
			{
				detailsString += "NULL" + VECTOR_SEPARATOR + "NULL";
			}


            detailsString += KEY_SEPARATOR + FORWARD_UNCONDITIONAL;
            if (TA_IRS_App::CallForwardingDetails::ForwardAlways == details.forwardAction)
            {
				detailsString += std::string(details.destinationTSI) + VECTOR_SEPARATOR + std::string(details.destinationTSI)+ VECTOR_SEPARATOR + std::string(details.destinationTSI);
            }
			else
			{
				detailsString += "NULL" + VECTOR_SEPARATOR + "NULL" + VECTOR_SEPARATOR + "NULL";
			}

            // And then finish off the details.
            detailsString += KEY_SEPARATOR;

            // Resize the method
            setMessageSize(CHANGE_REFERENCE_BASE_LENGTH + 4 + detailsString.size());

            // Add the string
            setParameterLongString(DETAILS_PARAMETER_START_POSITION,detailsString);
        }
        else
        {
            // Forwarding disabled, so only using the static text. Before adding it, need to resize the message.
            setMessageSize(CHANGE_REFERENCE_BASE_LENGTH + 4 + DISABLE_FORWARDING.size() + KEY_SEPARATOR.size());
            setParameterLongString(DETAILS_PARAMETER_START_POSITION,DISABLE_FORWARDING + KEY_SEPARATOR);
        }
		FUNCTION_EXIT;
    }

    void RadioMethodChangeReference::setPatchDetails(const std::string& controllingITSI,
        const std::vector<std::string>& patchList,const std::string& patchName)
    {
		FUNCTION_ENTRY("setPatchDetails");

        //trim spaces in patch ID
        std::string newPatchName = patchName;
        // newPatchName.erase(std::remove(newPatchName.begin(), newPatchName.end(), ' '), newPatchName.end() );

        // Adding a patch requires setting a number of specific variables.
        std::string detailsString = TYPE + "=" + PATCH_TYPE + KEY_SEPARATOR;
        detailsString += PATCH_ID + "=" + newPatchName + KEY_SEPARATOR;
        detailsString += OWNER_DISPATCHER + "=" + controllingITSI + KEY_SEPARATOR;
        detailsString += PATCH_HANG_TIME + "=" + PATCH_HANG_TIME_VALUE + KEY_SEPARATOR;

        // And now the members - first add the key and the "=" sign.
        detailsString += MEMBERS + "=";
        
        // Now loop through each member on the patch list and add it.
        for (unsigned int i = 0;i < patchList.size();i++)
        {
            detailsString += "+" + patchList[i];
            if (i+1 != patchList.size())
            {
                detailsString += DETAILS_SEPARATOR;
            }
        }

        // and the vectoral item ends with a new line
        detailsString += KEY_SEPARATOR;

        setMessageSize(CHANGE_REFERENCE_BASE_LENGTH + 4 + detailsString.size());
        setParameterLongString(DETAILS_PARAMETER_START_POSITION,detailsString);

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
            "setPatchDetails() - parameter string: \"%s\"", detailsString.c_str());        

		FUNCTION_EXIT;
    }

    void RadioMethodChangeReference::updateDynamicGroup(const std::string& name, const std::string& memberList, bool encodeName /*= true*/)
    {
        FUNCTION_ENTRY("updateDynamicGroup");
        int maxLenofName = name.size();
        if (maxLenofName > DGNA_NAME_MAX_LENGTH)
        {
            maxLenofName = DGNA_NAME_MAX_LENGTH;
        }

        std::string nameGroup;
        if (encodeName)
        {
            ACEXML_UTF16* wcharName = new ACEXML_UTF16[maxLenofName + 1];

            int nConsumedLength = ACEXML_Transcoder::utf8s2utf16s(name.c_str(), wcharName, maxLenofName + 1);
            if (nConsumedLength < 0)
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "createDynamicGroup() - converting to UTF 16 string failed, error code: \"%lu\"", nConsumedLength);

                throw TA_Base_Core::TransactiveException("converting to UTF 16 string failed");
            }

            for (int i = 0; i < nConsumedLength; i++)
            {
                nameGroup += convertWideChar(wcharName[i]);
            }

            delete[] wcharName;
        }
        else
        {
            nameGroup = name;
        }


        //format is: "K_TSI=1000-2-3\nK_NAME=groupname\nK_TYPE=G\nK_SUBSCRIPTION_STATUS=E\nK_PERMSSIONS=+R+A\n"

        std::string detailsString = NAME + "=" + nameGroup + KEY_SEPARATOR // nameGroup
            + MNEMONIC + "=" + nameGroup + KEY_SEPARATOR; // nameGroup

        // Adding a patch requires setting a number of specific variables.
        if (!memberList.empty())
            detailsString += MEMBERS + "=" + memberList + KEY_SEPARATOR;

        setMessageSize(CHANGE_REFERENCE_BASE_LENGTH + 4 + detailsString.size());
        setParameterLongString(DETAILS_PARAMETER_START_POSITION, detailsString);

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "updateDynamicGroup() - parameter string: \"%s\"", detailsString.c_str());

        FUNCTION_EXIT;
    }


	void RadioMethodChangeReference::setGroupMembers(const std::string& memberList)
    {
		FUNCTION_ENTRY("setGroupMembers");
        // Adding a patch requires setting a number of specific variables.
        std::string detailsString = MEMBERS + "=" + memberList + KEY_SEPARATOR;
        setMessageSize(CHANGE_REFERENCE_BASE_LENGTH + 4 + detailsString.size());
        setParameterLongString(DETAILS_PARAMETER_START_POSITION,detailsString);
    
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
             "setGroupMembers() - parameter string: \"%s\"", detailsString.c_str());
		FUNCTION_EXIT;
    }


    void RadioMethodChangeReference::setSearchType(const RadioMethodChangeReference::ESearchType searchType)
    {
		FUNCTION_ENTRY("setSearchType");
        // Setting the search mode is a relativley simple call. K_SEARCH_MODE=full or normal depending on the
        // type desired.
        std::string detailsString = SEARCH_MODE + "=" + ((searchType == FULL) ? FULL_SEARCH : NORMAL_SEARCH) + KEY_SEPARATOR;
        setMessageSize(CHANGE_REFERENCE_BASE_LENGTH + 4 + detailsString.size());
        setParameterLongString(DETAILS_PARAMETER_START_POSITION,detailsString);
		FUNCTION_EXIT;
    }

    void RadioMethodChangeReference::setAudioVolume(int leftVolume,int rightVolume, MicrophoneState micState)
    {
        FUNCTION_ENTRY("setAudioVolume");

#if defined RADIOMETHODCHANGEREFERENCE_SPRINTF_DEPRECATED
        std::ostringstream detailsStream;
        detailsStream << LEFT_VOLUME << "=" << leftVolume << KEY_SEPARATOR
                      << RIGHT_VOLUME << "=" << rightVolume << KEY_SEPARATOR;

        switch (micState)
        {
            case MicOn:
                detailsStream << MIC_ON << KEY_SEPARATOR;
                break;
            case MicOff:
                detailsStream << MIC_OFF << KEY_SEPARATOR;
                break;
            case MicNoChange:
            default:
                break;
        }
        
        std::string detailsString = detailsStream.str();
#else
        char  lVolumeText[30] = {0};
        char  rVolumeText[30] = {0};

		sprintf(lVolumeText,"%d",leftVolume);
        sprintf(rVolumeText,"%d",rightVolume);
        std::string detailsString = LEFT_VOLUME + "=" + lVolumeText + KEY_SEPARATOR +
                                    RIGHT_VOLUME + "=" + rVolumeText + KEY_SEPARATOR;
        switch (micState)
        {
            case MicOn:
                detailsString.append(MIC_ON);
                detailsString.append(KEY_SEPARATOR);
                break;
            case MicOff:
                detailsString.append(MIC_OFF);
                detailsString.append(KEY_SEPARATOR);
                break;
            case MicNoChange:
            default:
                break;
        }
#endif // #if defined RADIOMETHODCHANGEREFERENCE_SPRINTF_DEPRECATED

        setMessageSize(CHANGE_REFERENCE_BASE_LENGTH + 4 + detailsString.size());
        setParameterLongString(DETAILS_PARAMETER_START_POSITION,detailsString);

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
             "setAudioVolume() - parameter string: \"%s\"", detailsString.c_str());
        FUNCTION_EXIT;
    }

    void RadioMethodChangeReference::setCharacterEncoding(ECodePage codePage)
    {
        FUNCTION_ENTRY("setCharacterEncoding");
        char  codePageText[30] = {0};
        
        sprintf(codePageText, "%lu", static_cast<ta_uint32>(codePage));
        
        std::string detailsString = CODE_PAGE + "=" + codePageText + KEY_SEPARATOR;
        
        setMessageSize(CHANGE_REFERENCE_BASE_LENGTH + 4 + detailsString.size());
        setParameterLongString(DETAILS_PARAMETER_START_POSITION, detailsString);
        
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "setCharacterEncoding() - parameter string: \"%s\"", detailsString.c_str());
        FUNCTION_EXIT;
    }
    
    void RadioMethodChangeReference::setDefaultCharacterEncoding(ECodePage codePage)
    {
        FUNCTION_ENTRY("setDefaultCharacterEncoding");
        char  codePageText[30] = {0};
        
        sprintf(codePageText, "%lu", static_cast<ta_uint32>(codePage));
        
        std::string detailsString = DEFAULT_CODE_PAGE + "=" + codePageText + KEY_SEPARATOR;
        
        setMessageSize(CHANGE_REFERENCE_BASE_LENGTH + 4 + detailsString.size());
        setParameterLongString(DETAILS_PARAMETER_START_POSITION, detailsString);
        
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "setDefaultCharacterEncoding() - parameter string: \"%s\"", detailsString.c_str());
        FUNCTION_EXIT;
    }
    
    void RadioMethodChangeReference::disableDynamicGroup()
    {
        FUNCTION_ENTRY("disableDynamicGroup");

        std::string detailsString = DISABLE_DGNA + KEY_SEPARATOR;
        setMessageSize(CHANGE_REFERENCE_BASE_LENGTH + 4 + detailsString.size());
        setParameterLongString(DETAILS_PARAMETER_START_POSITION, detailsString);
    
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "disableDynamicGroup() - parameter string: \"%s\"", detailsString.c_str());
        
        FUNCTION_EXIT;
    }

    void RadioMethodChangeReference::enableDynamicGroup()
    {
        FUNCTION_ENTRY("enableDynamicGroup");

        std::string detailsString = ENABLE_DGNA + KEY_SEPARATOR;
        setMessageSize(CHANGE_REFERENCE_BASE_LENGTH + 4 + detailsString.size());
        setParameterLongString(DETAILS_PARAMETER_START_POSITION, detailsString);

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "enableDynamicGroup() - parameter string: \"%s\"", detailsString.c_str());
        FUNCTION_EXIT;
    }

    void RadioMethodChangeReference::createDynamicGroup(const std::string& name, const std::string& itsi, const std::string& memberList, bool encodeName /*= true*/)
    {
        FUNCTION_ENTRY("createDynamicGroup");

        std::string nameGroup;
        std::string nmemonic;

        if (encodeName)
        {
            int maxLenofName = name.size();
            if (maxLenofName > DGNA_NAME_MAX_LENGTH)  // Wide char is limited to 16
            {
                maxLenofName = DGNA_NAME_MAX_LENGTH;
            }

            std::string tempNmemonic = name;
            if (name.size() > 7)
                tempNmemonic = name.substr(0, 6);

            //todo... should move this block of code to helper class, but there is no design to allow doing that.
            ACEXML_UTF16* wcharName = new ACEXML_UTF16[maxLenofName + 1];
            ACEXML_UTF16* wcharNnemonic = new ACEXML_UTF16[7];

            int nConsumedLength = ACEXML_Transcoder::utf8s2utf16s(name.c_str(), wcharName, maxLenofName + 1);
            if (nConsumedLength < 0)
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "createDynamicGroup() - converting to UTF 16 string failed, error code: \"%lu\"", nConsumedLength);

                throw TA_Base_Core::TransactiveException("converting to UTF 16 string failed");
            }

            for (int i = 0; i < nConsumedLength; i++)
            {
                nameGroup += convertWideChar(wcharName[i]);
            }

            int nTempNmenomicLen = ACEXML_Transcoder::utf8s2utf16s(tempNmemonic.c_str(), wcharNnemonic, 7);
            if (nTempNmenomicLen < 0)
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "createDynamicGroup() - converting to UTF 16 string failed, error code: \"%lu\"", nTempNmenomicLen);

                throw TA_Base_Core::TransactiveException("converting to UTF 16 string failed");
            }

            for (int j = 0; j < nTempNmenomicLen; j++)
            {
                nmemonic += convertWideChar(wcharNnemonic[j]);
            }

            delete[] wcharName;
            delete[] wcharNnemonic;
        }
        else
        {
            nameGroup = name;
            nmemonic = name;
        }


        //format is: "K_TSI=1000-2-3\nK_NAME=groupname\nK_TYPE=G\nK_SUBSCRIPTION_STATUS=E\nK_PERMSSIONS=+R+A\n"

        std::string detailsString = ENABLE_DGNA + KEY_SEPARATOR //enable the DGNA
            + TSI + "=" + itsi + KEY_SEPARATOR
            + NAME + "=" + nameGroup + KEY_SEPARATOR // nameGroup
            + MNEMONIC + "=" + nmemonic + KEY_SEPARATOR // nameGroup
            + TYPE + "=" + "G" + KEY_SEPARATOR
            + SUBSCRIPTION_STATUS + "=" + "E" + KEY_SEPARATOR
            + PERMISSION + "=" + "+U+A+B+V+S" + KEY_SEPARATOR;

        if (!memberList.empty())
            detailsString += MEMBERS + "=" + memberList + KEY_SEPARATOR;

        detailsString += DGNA_ORG_ID1 + "=0" + KEY_SEPARATOR;
        detailsString += DGNA_ORG_ID2 + "=0" + KEY_SEPARATOR;
        detailsString += DGNA_ORG_ID3 + "=0" + KEY_SEPARATOR;
        detailsString += DGNA_ORG_ID4 + "=0" + KEY_SEPARATOR;
        detailsString += DGNA_ORG_ID5 + "=0" + KEY_SEPARATOR;

        setMessageSize(CHANGE_REFERENCE_BASE_LENGTH + 4 + detailsString.size());
        setParameterLongString(DETAILS_PARAMETER_START_POSITION, detailsString);

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "createDynamicGroup() - parameter string: \"%s\"", detailsString.c_str());


        FUNCTION_EXIT;
    }


     void RadioMethodChangeReference::changeRegionalMembership(const ScnBaseStationMap& scnMap)
     {
        std::stringstream opa;
        for ( ScnBaseStationMap::const_iterator pos = scnMap.begin();
              pos != scnMap.end(); ++pos)
        {
            opa << pos->first << "#" << pos->second << ";";
        }

        // if the last char is the vector separator, remove it
        std::string opaParam(opa.str());
        if(opaParam.at(opaParam.length()-1) == ';')
        {
            opaParam.erase(opaParam.length()-1);
        }

        std::string detailsString = ENABLE_GOA + KEY_SEPARATOR + GROUP_OPERATING_AREA + opaParam + KEY_SEPARATOR;

        setMessageSize(CHANGE_REFERENCE_BASE_LENGTH + 4 + detailsString.size());
        setParameterLongString(DETAILS_PARAMETER_START_POSITION, detailsString);

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "changeRegionalMembership() - parameter string: \"%s\"", detailsString.c_str());
     }

     unsigned char RadioMethodChangeReference::getSerialiseTransactionLevel() const
    {
        return SERIALISATION_LEVEL_2;
    }

	 void RadioMethodChangeReference::setSubsriberMngemtStatus(SubscDataTypes::RadioSubMngmentStatus subStatus)
	 {
		 FUNCTION_ENTRY("setSubsriberMngemtStatus");

		 int valueAttribute = static_cast<int>(subStatus);
		 std::ostringstream subsStatusVal;
		 subsStatusVal << valueAttribute;

		 std::string detailsString = SUBSRIBER_STATUS + subsStatusVal.str();
		 setMessageSize(CHANGE_REFERENCE_BASE_LENGTH + 4 + detailsString.size());
		 setParameterLongString(DETAILS_PARAMETER_START_POSITION,detailsString);

		 LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			 "setSubsriberMngemtStatus() - parameter string: \"%s\"", detailsString.c_str());

		 FUNCTION_EXIT;
	 }

	 void RadioMethodChangeReference::setEmuDataCallIPAddress(const std::string& destIPAdress)
	 {
		 FUNCTION_ENTRY("setEmuDataCallIPAddress");
		 
		 // Set to Emulated Circuit Data Call
		 std::string detailsString = CIRCUIT_DATAMODE + "F";
		 // Key separator
		 detailsString += KEY_SEPARATOR;
		 // Destination IPAddress
		 detailsString += ECID_DEST_IPADDRESS + destIPAdress;
		 detailsString += KEY_SEPARATOR;		

		 setMessageSize(CHANGE_REFERENCE_BASE_LENGTH + 4 + detailsString.size());
		 setParameterLongString(DETAILS_PARAMETER_START_POSITION,detailsString);

		 LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			 "setEmuDataCallIPAddress() - parameter string: \"%s\"", detailsString.c_str());

		 FUNCTION_EXIT;
	 }
}

