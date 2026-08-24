// RadioMethodTLSDSAck.cpp: implementation of the RadioMethodTLSDSAck class.
//
//////////////////////////////////////////////////////////////////////

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Workfile:$
  * @author:  Haipeng Jiang
  * @version: $Revision: 1.1.2.2 $
  *
  * Last modification: $Date: 2006/03/10 10:50:38 $
  * Last modified by:  $Author: haipeng $
  *
  * <description>
  */


#include "RadioMethodTLSDSAck.h"
#include "core/utilities/src/DebugUtil.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
using namespace TA_IRS_App;


const ta_uint32 RadioMethodTLSDSAck::DELIVERY_STATUS_START_POS = 4;
const ta_uint32 RadioMethodTLSDSAck::SDSTL_PARAMETER_LENGTH = 5;

RadioMethodTLSDSAck::RadioMethodTLSDSAck(RadioSessionReference &session, RadioCallReference & call)
: RadioMethodCallReference(SDSTL_PARAMETER_LENGTH,METHOD_TLSDS_ACK, session, call)
{

}

RadioMethodTLSDSAck::~RadioMethodTLSDSAck()
{

}

void RadioMethodTLSDSAck::setAckReceived()
{
	FUNCTION_ENTRY("setAckReceived");
	setParameterByte(DELIVERY_STATUS_START_POS, SDSTL_STOK_ACK_REPORT_MSG_RECEIVED);
	FUNCTION_EXIT;
}

void RadioMethodTLSDSAck::setAckConsumed()
{
	FUNCTION_ENTRY("setAckConsumed");
	setParameterByte(DELIVERY_STATUS_START_POS, SDSTL_STOK_ACK_REPORT_MSG_CONSUMED);
	FUNCTION_EXIT;;
}