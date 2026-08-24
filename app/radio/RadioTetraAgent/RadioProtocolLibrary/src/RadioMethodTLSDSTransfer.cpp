//
//////////////////////////////////////////////////////////////////////

/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $Workfile:$
* @author:  Haipeng Jiang
* @version: $Revision: 1.1.2.7.4.1 $
*
* Last modification: $Date: 2007/11/09 08:43:28 $
* Last modified by:  $Author: zhangyan $
*
* <description>
*/


#include "RadioMethodTLSDSTransfer.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "ACEXML/common/Transcode.h"
#include <sstream>

namespace TA_IRS_App
{
	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////
	
	const ta_uint32 RadioMethodTLSDSTransfer::CALLED_NUMBER_POSITION = 4;
	const ta_uint32 RadioMethodTLSDSTransfer::PROTOCOL_POSITION = 36;
	const ta_uint32 RadioMethodTLSDSTransfer::TIMESTAMP_FLAG_POSITION = 37;
	const ta_uint32 RadioMethodTLSDSTransfer::REPORTED_REQUEST_POSITION = 38;
	const ta_uint32 RadioMethodTLSDSTransfer::SERVICE_SELECTION_POSITION = 39;
	const ta_uint32 RadioMethodTLSDSTransfer::DATA_LENGTH_POSITION = 40;
	const ta_uint32 RadioMethodTLSDSTransfer::DATA_POSITION = 44;
	const ta_uint32 RadioMethodTLSDSTransfer::MAX_SDSTL_MESAGE_LENGTH = 140;
	
	const ta_uint32 RadioMethodTLSDSTransfer::PARAMETER_LENGTH =
		RadioMethodTLSDSTransfer::MAX_SDSTL_MESAGE_LENGTH + RadioMethodTLSDSTransfer::DATA_POSITION;
	
	RadioMethodTLSDSTransfer::RadioMethodTLSDSTransfer(RadioSessionReference session, RadioCallReference & call)
		: RadioMethodCallReference(PARAMETER_LENGTH, METHOD_TLSDS_TRANSFER, session, call)
	{
        setParameterLong( 0, call ); // limin++ CALL REFERENCE
	}
	
	RadioMethodTLSDSTransfer::~RadioMethodTLSDSTransfer()
	{
		
	}
	
	
	void RadioMethodTLSDSTransfer::setCalledNumber(const std::string & number)  
	{
		FUNCTION_ENTRY("setCalledNumber");
		setParameterShortString(CALLED_NUMBER_POSITION,number);
		FUNCTION_EXIT;
	}
	
	void RadioMethodTLSDSTransfer::setProtocol(ESDSTLProtocol protocol)
	{
		FUNCTION_ENTRY("setProtocol");
		setParameterByte(PROTOCOL_POSITION, protocol);
		FUNCTION_EXIT;
		
	}
	
	void RadioMethodTLSDSTransfer::setRequestTimestampFlag(bool flag)
	{
		FUNCTION_ENTRY("setRequestTimestampFlag");
		setParameterByte(TIMESTAMP_FLAG_POSITION, flag);
		FUNCTION_EXIT;
		
	}
	
	void RadioMethodTLSDSTransfer::setReceiveReceiptFlag(ESDSTLResponseRequested flag)
	{
		FUNCTION_ENTRY("setReceiveReceiptFlag");
		setParameterByte(REPORTED_REQUEST_POSITION, flag);
		FUNCTION_EXIT;
		
	}
	
	void RadioMethodTLSDSTransfer::setServiceSelection(ESDSTLServiceSelection selection)
	{
		FUNCTION_ENTRY("setServiceSelection");
		setParameterByte(SERVICE_SELECTION_POSITION, selection);
		FUNCTION_EXIT;
	}
	
    std::string convertWideChar_M(unsigned short w)
    {
        unsigned char highByte = static_cast<unsigned char>((w & 0xFF00) >> 8);
        unsigned char lowByte = static_cast<unsigned char>(w & 0xFF);
        char ret[8] = {0};
        sprintf(ret, "\\%02X%02X", highByte, lowByte);
        return ret;
		
    }
	void RadioMethodTLSDSTransfer::setMessage(const std::vector<unsigned char> & text)
	{
		FUNCTION_ENTRY("setMessage");
		TA_ASSERT(text.size() <= MAX_SDSTL_MESAGE_LENGTH, "SDS-TL message length exceeds byte-limit");
		
		std::ostringstream osString;
		std::vector<unsigned char>::const_iterator it = text.begin();
		for (; it != text.end(); ++it)
		{
			osString << (*it);
		}
		std::string detailsString = osString.str();
		
		setMessageSize(DATA_POSITION + detailsString.size());
		setParameterLongString(DATA_LENGTH_POSITION, detailsString);
		
		// wss a+ Requested by rob - Following affects scripts interpretation. No impact on code itself
		//LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
		//	"setMessage() - data: \"%s\"", detailsString.c_str());
		
		FUNCTION_EXIT;
	}
}

