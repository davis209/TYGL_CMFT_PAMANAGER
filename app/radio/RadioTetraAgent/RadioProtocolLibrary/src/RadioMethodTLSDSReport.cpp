// RadioMethodTLSDSReport.cpp: implementation of the RadioMethodTLSDSReport class.
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


#include "RadioMethodTLSDSReport.h"

#include "core/utilities/src/DebugUtil.h"

using namespace TA_IRS_App;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


const ta_uint32 RadioMethodTLSDSReport::DELIVERY_STATUS_START_POS = 4;
const ta_uint32 RadioMethodTLSDSReport::ACK_REQUEST_START_POS = 5;
const ta_uint32 RadioMethodTLSDSReport::REPORT_TYPE_START_POS = 6;
const ta_uint32 RadioMethodTLSDSReport::SDSTL_REPORT_PARAMETER_LENGTH = 7;

RadioMethodTLSDSReport::RadioMethodTLSDSReport(RadioSessionReference &session, RadioCallReference & call)
: RadioMethodCallReference(SDSTL_REPORT_PARAMETER_LENGTH, METHOD_TLSDS_REPORT, session, call)
{

}

RadioMethodTLSDSReport::~RadioMethodTLSDSReport()
{

}

void RadioMethodTLSDSReport::setDeliveryStatus(ESDSTL_Report_DeliveryStatus status)
{
	FUNCTION_ENTRY("setDeliveryStatus");
	setParameterByte(DELIVERY_STATUS_START_POS, status);
	FUNCTION_EXIT;
}

void RadioMethodTLSDSReport::setAckRequestFlag(bool flag)
{
	FUNCTION_ENTRY("setAckRequestFlag");
	setParameterByte(ACK_REQUEST_START_POS, flag);
	FUNCTION_EXIT;
}

void RadioMethodTLSDSReport::setReportType(ESDSTLReportType reportType)
{
	FUNCTION_ENTRY("setReportType");
	setParameterByte(REPORT_TYPE_START_POS, reportType);
	FUNCTION_EXIT;
}
