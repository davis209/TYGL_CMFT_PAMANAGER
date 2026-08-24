// RadioMethodGetLastError.cpp: implementation of the RadioMethodGetLastError class.
//
//////////////////////////////////////////////////////////////////////

#include "RadioMethodGetLastError.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace TA_IRS_App
{
    // (DWORD(Reference)) + (Initial Length DWORD of LONGSTRING(Details))
	const ta_uint32 RadioMethodGetLastError::GET_LAST_ERROR_BASE_LENGTH = 8;
	const ta_uint32 RadioMethodGetLastError::GET_LAST_ERROR_ROWSPEREVENT_POS = 0;
	const ta_uint32 RadioMethodGetLastError::GET_LAST_ERROR_FIELDS_POS = 4;

	RadioMethodGetLastError::RadioMethodGetLastError(RadioSessionReference session)
		: RadioMethod(GET_LAST_ERROR_BASE_LENGTH, METHOD_GETLASTERRROR, session)
	{
		setParameterLong(GET_LAST_ERROR_ROWSPEREVENT_POS, 0);
		setParameterLong(GET_LAST_ERROR_FIELDS_POS, 0);
	}
	
	RadioMethodGetLastError::~RadioMethodGetLastError()
	{
		
	}
	
	void RadioMethodGetLastError::setRowsPerEvent(ta_uint32 rows)
	{
		FUNCTION_ENTRY("setRowsPerEvent");
		setParameterLong(GET_LAST_ERROR_ROWSPEREVENT_POS, rows);
		FUNCTION_EXIT;
	}

	void RadioMethodGetLastError::setFields(const std::vector<std::string> & arrFields)
	{
		FUNCTION_ENTRY("setFields");
		std::string strDetails;
		
		for ( std::vector<std::string>::const_iterator it = arrFields.begin() ;
              it != arrFields.end() ;
              ++it)
		{
			strDetails += (*it);
			strDetails += '\n';
		}

		setMessageSize(GET_LAST_ERROR_BASE_LENGTH + strDetails.size());
		setParameterLongString(GET_LAST_ERROR_FIELDS_POS, strDetails);
		
		FUNCTION_EXIT;
	}
}