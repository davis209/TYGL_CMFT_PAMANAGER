// GetLastErrorEventReader.cpp: implementation of the GetLastErrorEventReader class.
//
//////////////////////////////////////////////////////////////////////

#include "GetLastErrorEventReader.h"

#include "core/utilities/src/DebugUtil.h"

#include <sstream>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace TA_IRS_App
{
	
	
	
	const ta_uint32 GetLastErrorEventReader::MAJOR_ERROR_START_POS = 0;
	const ta_uint32 GetLastErrorEventReader::MINOR_ERROR_START_POS = 4;
	const ta_uint32 GetLastErrorEventReader::ERRORDETAIL_START_POS = 8;
	
	GetLastErrorEventReader::GetLastErrorEventReader(RadioEvent & event)
		: RadioEventReader(event, EVENT_GETLASTERRROR)
	{
		
	}
	
	GetLastErrorEventReader::~GetLastErrorEventReader()
	{
		
	}
	
	ta_uint32 GetLastErrorEventReader::getMajorErrorCode()
	{
		FUNCTION_ENTRY("getMajorErrorCode");
		FUNCTION_EXIT;
		return this->getParameterLong(MAJOR_ERROR_START_POS);
	}

	ta_uint32 GetLastErrorEventReader::getMinorErrorCode()
	{
		FUNCTION_ENTRY("getMinorErrorCode");
		FUNCTION_EXIT;
		return this->getParameterLong(MINOR_ERROR_START_POS);
	}
	
	std::string GetLastErrorEventReader::getErrorDetails()
	{
		FUNCTION_ENTRY("getErrorDetails");

		std::ostringstream  details;
		ta_uint32 length = getParameterLong(ERRORDETAIL_START_POS);
		//actually, the realy start position of the characters is offset 4 chars to read, 
		//the head 4 bytes store the message length.		
		for (ta_uint32 i=ERRORDETAIL_START_POS+4 ; i<length ; i++)
		{
			unsigned char charTemp = getParameterChar(i);

			if (charTemp != 0x00)
			{
				details << charTemp;
			}
            else
            {
                break;
            }
		}
		
		std::string detailstr(details.str());
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "getErrorDetails() - data: \"%s\"", detailstr.c_str());
		
		FUNCTION_EXIT;
		return detailstr;
	}
}