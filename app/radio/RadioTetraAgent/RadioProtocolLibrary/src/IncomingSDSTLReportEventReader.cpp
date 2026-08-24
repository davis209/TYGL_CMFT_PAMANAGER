

/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $Workfile:$
* @author:  Haipeng Jiang
* @version: $Revision: 1.1.2.5 $
*
* Last modification: $Date: 2006/07/20 03:36:42 $
* Last modified by:  $Author: robertvh $
*
* <description>
*/



#include "IncomingSDSTLReportEventReader.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App
{
	
	const ta_uint32 IncomingSDSTLReportEventReader::DELIVERY_STATUS_START_POS = 4;
	const ta_uint32 IncomingSDSTLReportEventReader::ACKNOWLEDGE_REQUEST_START_POS = 5;
	const ta_uint32 IncomingSDSTLReportEventReader::SENDER_NUMBER_START_POS = 6;
	const ta_uint32 IncomingSDSTLReportEventReader::EVENT_INCOMING_SDSTL_REPORT_PARAMETER_LENGTH = 38;
	
	IncomingSDSTLReportEventReader::IncomingSDSTLReportEventReader(RadioEvent &radioEvent)
		: RadioEventReader(radioEvent, EVENT_INCOMING_TLSDS_REPORT, EVENT_INCOMING_SDSTL_REPORT_PARAMETER_LENGTH)
	{
		
		
	}
	
	IncomingSDSTLReportEventReader::~IncomingSDSTLReportEventReader()
	{
		
	}
	
	bool IncomingSDSTLReportEventReader::getAckRequestFlag()
	{
		FUNCTION_ENTRY("getAckRequestFlag");
        unsigned char value = getParameterChar(ACKNOWLEDGE_REQUEST_START_POS);
        EAckRequest rqst_value = static_cast<EAckRequest>(value);
        if (!((rqst_value==SDSTL_ACK_REQ)||(rqst_value==SDSTL_ACK_NOT_REQ)) )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                         "Value for Acknowledge Request Flag out of range: %d", value);

        }
		FUNCTION_EXIT;
		return (rqst_value==SDSTL_ACK_REQ);
	}
	
	
	std::string IncomingSDSTLReportEventReader::getCallingNumber()
	{
		FUNCTION_ENTRY("getCallingNumber");
		FUNCTION_EXIT;	
		return this->getParameterShortString(SENDER_NUMBER_START_POS);
	}
	
	ESDSTL_Report_DeliveryStatus IncomingSDSTLReportEventReader::getDeliveryStatus()
	{
		FUNCTION_ENTRY("getDeliveryStatus");
		FUNCTION_EXIT;	
		return static_cast<ESDSTL_Report_DeliveryStatus>(this->getParameterChar(DELIVERY_STATUS_START_POS)); 
	}
	
}