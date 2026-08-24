// GetLastErrorEventReader.h: interface for the GetLastErrorEventReader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GETLASTERROREVENTREADER_H__5164962C_76F6_4762_B7BA_CF48364D006F__INCLUDED_)
#define AFX_GETLASTERROREVENTREADER_H__5164962C_76F6_4762_B7BA_CF48364D006F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/types/src/ta_types.h"
#include "RadioEventReader.h"

#include <string>
#include <vector>

namespace TA_IRS_App
{
	
	class GetLastErrorEventReader : public RadioEventReader  
	{
	public:
		GetLastErrorEventReader(RadioEvent & event);
		virtual ~GetLastErrorEventReader();

		ta_uint32 getMajorErrorCode();
		ta_uint32 getMinorErrorCode();
		std::string   getErrorDetails();
	private:
		static const ta_uint32 MAJOR_ERROR_START_POS;
		static const ta_uint32 MINOR_ERROR_START_POS;
		static const ta_uint32 ERRORDETAIL_START_POS;

	private:
		GetLastErrorEventReader();
		GetLastErrorEventReader& operator = (GetLastErrorEventReader & that);	
	};

}
#endif // !defined(AFX_GETLASTERROREVENTREADER_H__5164962C_76F6_4762_B7BA_CF48364D006F__INCLUDED_)
