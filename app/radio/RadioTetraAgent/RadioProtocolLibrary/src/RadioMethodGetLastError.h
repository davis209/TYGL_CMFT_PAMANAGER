// RadioMethodGetLastError.h: interface for the RadioMethodGetLastError class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RADIOMETHODGETLASTERROR_H__8B3D3244_930C_4447_9E24_5F128A0E9C28__INCLUDED_)
#define AFX_RADIOMETHODGETLASTERROR_H__8B3D3244_930C_4447_9E24_5F128A0E9C28__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RadioMethodCallReference.h"
#include "core/types/src/ta_types.h"
#include <vector>
#include <string>

namespace TA_IRS_App
{
	
	class RadioMethodGetLastError : public RadioMethod  
	{
	public:
		RadioMethodGetLastError(RadioSessionReference session);
		virtual ~RadioMethodGetLastError();
		
		void setRowsPerEvent(ta_uint32 rows);
		void setFields(const std::vector<std::string>& arrFields);
	private:
		RadioMethodGetLastError(RadioMethodGetLastError &);
		RadioMethodGetLastError & operator = (RadioMethodGetLastError &);
	private:
		static const ta_uint32 GET_LAST_ERROR_BASE_LENGTH;
		static const ta_uint32 GET_LAST_ERROR_ROWSPEREVENT_POS;
		static const ta_uint32 GET_LAST_ERROR_FIELDS_POS;
	};

}
#endif // !defined(AFX_RADIOMETHODGETLASTERROR_H__8B3D3244_930C_4447_9E24_5F128A0E9C28__INCLUDED_)
