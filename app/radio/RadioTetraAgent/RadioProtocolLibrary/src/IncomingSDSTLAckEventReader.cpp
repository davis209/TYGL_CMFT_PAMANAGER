

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Workfile:$
  * @author:  Haipeng Jiang
  * @version: $Revision: 1.1.2.3 $
  *
  * Last modification: $Date: 2006/07/20 03:36:42 $
  * Last modified by:  $Author: robertvh $
  *
  * <description>
  */

#include "IncomingSDSTLAckEventReader.h"
#include "RadioSDSTLTypes.h"
#include "core/utilities/src/DebugUtil.h"

using namespace TA_IRS_App;

const ta_uint32 IncomingSDSTLAckEventReader::DELIVERY_STATUS_START_POS = 4;
const ta_uint32 IncomingSDSTLAckEventReader::EVENT_INCOMING_SDSTL_PARAMETER_LENGTH = 5;

IncomingSDSTLAckEventReader::IncomingSDSTLAckEventReader(RadioEvent &radioEvent)
: RadioEventReader(radioEvent, EVENT_INCOMING_TLSDS_ACK, EVENT_INCOMING_SDSTL_PARAMETER_LENGTH)
{
	
}

IncomingSDSTLAckEventReader::~IncomingSDSTLAckEventReader()
{
	
}

bool IncomingSDSTLAckEventReader::getAckConsumedFlag()
{
	FUNCTION_ENTRY("getAckConsumedFlag");
	if (static_cast<ESDSTL_Ack_DeliveryStatus>(getParameterChar(DELIVERY_STATUS_START_POS)) == SDSTL_STOK_ACK_REPORT_MSG_CONSUMED)
	{
		FUNCTION_EXIT;
		return true;
	}
	FUNCTION_EXIT;
	return false;
}

bool IncomingSDSTLAckEventReader::getAckReceivedFlag()
{
	FUNCTION_ENTRY("getAckReceivedFlag");
	if (static_cast<ESDSTL_Ack_DeliveryStatus>(getParameterChar(DELIVERY_STATUS_START_POS)) == SDSTL_STOK_ACK_REPORT_MSG_RECEIVED)
	{
		FUNCTION_EXIT;
		return true;
	}
	FUNCTION_EXIT;
	return false;
}