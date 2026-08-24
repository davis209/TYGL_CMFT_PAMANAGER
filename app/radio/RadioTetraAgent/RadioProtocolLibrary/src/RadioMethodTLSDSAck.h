// RadioMethodTLSDSAck.h: interface for the RadioMethodTLSDSAck class.
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


#if !defined(AFX_RADIOMETHODTLSDSACK_H__ECB0578A_ABC2_4942_B84E_0D8C327266AD__INCLUDED_)
#define AFX_RADIOMETHODTLSDSACK_H__ECB0578A_ABC2_4942_B84E_0D8C327266AD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RadioMethod.h"
#include "RadioMethodCallReference.h"
#include "core/types/src/ta_types.h"
#include <vector>
#include <string>

namespace TA_IRS_App
{
	class RadioMethodTLSDSAck : public RadioMethodCallReference  
	{
	public:
		RadioMethodTLSDSAck(RadioSessionReference &session, RadioCallReference & call);
		virtual ~RadioMethodTLSDSAck();

		void setAckReceived(); 
		void setAckConsumed();
	private:
		RadioMethodTLSDSAck();
		RadioMethodTLSDSAck& operator = (const RadioMethodTLSDSAck & that);
	private:
		static const ta_uint32 DELIVERY_STATUS_START_POS;
		static const ta_uint32 SDSTL_PARAMETER_LENGTH;
	};
}
#endif // !defined(AFX_RADIOMETHODTLSDSACK_H__ECB0578A_ABC2_4942_B84E_0D8C327266AD__INCLUDED_)
