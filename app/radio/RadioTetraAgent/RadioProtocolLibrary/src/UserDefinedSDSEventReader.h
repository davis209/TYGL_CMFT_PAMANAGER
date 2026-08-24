
#ifndef __USER_DEFINED_SDS_EVENT_READER_H_INCLUDED__
#define __USER_DEFINED_SDS_EVENT_READER_H_INCLUDED__

#include "RadioProtocol.h"
#include "RadioEventReader.h"


#include <string>
#include <vector>

namespace TA_IRS_App
{
	class UserDefinedSDSEventReader: public RadioEventReader
	{
	public:
		UserDefinedSDSEventReader(RadioEvent & event);

	private:
		static const ta_uint32 EVENT_SDSTL_TRANSFER_PARAMETER_LENGTH;
	private:
		UserDefinedSDSEventReader();
		UserDefinedSDSEventReader& operator = (UserDefinedSDSEventReader & that);
	};
}

#endif //__USER_DEFINED_SDS_EVENT_READER_H_INCLUDED__
