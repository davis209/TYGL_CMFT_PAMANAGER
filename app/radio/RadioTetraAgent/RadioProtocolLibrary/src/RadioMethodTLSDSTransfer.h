// RadioMethodTLSDSTransfer.h: interface for the RadioMethodTLSDSTransfer class.
//
//////////////////////////////////////////////////////////////////////

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Workfile:$
  * @author:  Haipeng Jiang
  * @version: $Revision: 1.1.2.5 $
  *
  * Last modification: $Date: 2006/04/24 09:05:43 $
  * Last modified by:  $Author: haipeng $
  *
  * <description>
  */


#if !defined(AFX_RADIOMETHODTLSDSTRANSFER_H__C850C050_4D57_47F6_9757_3B414E59225E__INCLUDED_)
#define AFX_RADIOMETHODTLSDSTRANSFER_H__C850C050_4D57_47F6_9757_3B414E59225E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RadioMethod.h"
#include "RadioMethodCallReference.h"

#include "RadioMethodCallReference.h"
#include "RadioProtocol.h"
#include "core/types/src/ta_types.h"

#include <vector>
#include <string>

namespace TA_IRS_App
{
	
	class RadioMethodTLSDSTransfer : public RadioMethodCallReference  
	{
	public:
		RadioMethodTLSDSTransfer(RadioSessionReference session, RadioCallReference & call);
		virtual ~RadioMethodTLSDSTransfer();
		
		void setCalledNumber(const std::string & number);  
		void setProtocol(ESDSTLProtocol protocol);  
		void setRequestTimestampFlag(bool flag);  
		void setReceiveReceiptFlag(ESDSTLResponseRequested flag);  
		void setServiceSelection(ESDSTLServiceSelection selection);  
		void setMessage(const std::vector<unsigned char> & text);
		
		//private:
		//	ESDSTLServiceSelection m_serviceSelection;
	private:
		RadioMethodTLSDSTransfer();
		RadioMethodTLSDSTransfer& operator = (const RadioMethodTLSDSTransfer & that);
		
		static const ta_uint32 CALLED_NUMBER_POSITION;
		static const ta_uint32 PROTOCOL_POSITION;
		static const ta_uint32 TIMESTAMP_FLAG_POSITION;
		static const ta_uint32 REPORTED_REQUEST_POSITION;
		static const ta_uint32 SERVICE_SELECTION_POSITION;
		static const ta_uint32 DATA_LENGTH_POSITION;
		static const ta_uint32 DATA_POSITION;
					 
		static const ta_uint32 MAX_SDSTL_MESAGE_LENGTH; // TD10791
		static const ta_uint32 PARAMETER_LENGTH;
	};
	
	
}
#endif // !defined(AFX_RADIOMETHODTLSDSTRANSFER_H__C850C050_4D57_47F6_9757_3B414E59225E__INCLUDED_)
