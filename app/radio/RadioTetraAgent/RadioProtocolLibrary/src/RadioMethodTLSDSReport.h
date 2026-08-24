// RadioMethodTLSDSReport.h: interface for the RadioMethodTLSDSReport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RADIOMETHODTLSDSREPORT_H__56C116D4_D2BE_477E_93D6_D45522C50FD9__INCLUDED_)
#define AFX_RADIOMETHODTLSDSREPORT_H__56C116D4_D2BE_477E_93D6_D45522C50FD9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RadioMethod.h"
#include "RadioProtocol.h"
#include "RadioMethodCallReference.h"
#include "core/types/src/ta_types.h"

#include <vector>
#include <string>


namespace TA_IRS_App
{
	class RadioMethodTLSDSReport : public RadioMethodCallReference  
	{
	public:
		RadioMethodTLSDSReport(RadioSessionReference &session, RadioCallReference & call);
		virtual ~RadioMethodTLSDSReport();
		
		void setDeliveryStatus(ESDSTL_Report_DeliveryStatus status);  
		void setAckRequestFlag(bool flag);  
		void setReportType(ESDSTLReportType reportType);  
	private:
		RadioMethodTLSDSReport();
		RadioMethodTLSDSReport& operator = (const RadioMethodTLSDSReport& that);
	private:
		static const ta_uint32 DELIVERY_STATUS_START_POS;
		static const ta_uint32 ACK_REQUEST_START_POS;
		static const ta_uint32 REPORT_TYPE_START_POS;
		static const ta_uint32 SDSTL_REPORT_PARAMETER_LENGTH;
	};
}
#endif // !defined(AFX_RADIOMETHODTLSDSREPORT_H__56C116D4_D2BE_477E_93D6_D45522C50FD9__INCLUDED_)
