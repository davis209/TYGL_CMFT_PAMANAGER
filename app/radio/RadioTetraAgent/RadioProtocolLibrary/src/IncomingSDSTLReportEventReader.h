/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Workfile:$
  * @author:  Haipeng Jiang
  * @version: $Revision: 1.1.2.4 $
  *
  * Last modification: $Date: 2006/07/24 07:40:38 $
  * Last modified by:  $Author: robertvh $
  *
  * <description>
  */
#if !defined(INCOMINGSDSTLREPORTEVENTREADER_H)
#define INCOMINGSDSTLREPORTEVENTREADER_H

#include "RadioProtocol.h"
#include "RadioEventReader.h"
#include "core/types/src/ta_types.h"
#include <string>
#include <vector>

namespace TA_IRS_App
{
	
	class IncomingSDSTLReportEventReader: public RadioEventReader
	{
	public:
		IncomingSDSTLReportEventReader(RadioEvent & radioEvent);
		virtual ~IncomingSDSTLReportEventReader();
		
		ESDSTL_Report_DeliveryStatus getDeliveryStatus();
		bool getAckRequestFlag(); 
		std::string getCallingNumber();  
	private:
		IncomingSDSTLReportEventReader();
		IncomingSDSTLReportEventReader& operator = (const IncomingSDSTLReportEventReader & that);
	private:
		static const ta_uint32 DELIVERY_STATUS_START_POS;
		static const ta_uint32 ACKNOWLEDGE_REQUEST_START_POS;
		static const ta_uint32 SENDER_NUMBER_START_POS;
		static const ta_uint32 EVENT_INCOMING_SDSTL_REPORT_PARAMETER_LENGTH;
	};
	
}

#endif
