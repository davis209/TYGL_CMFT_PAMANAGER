

#ifndef __PTT_STATUS_EVENT_READER_H_INCLUDED__

#include "RadioEventReader.h"
#include <vector>

namespace TA_IRS_App
{
	class PttStatusEventReader : public RadioEventReader 
	{
	public:
		PttStatusEventReader(RadioEvent& event);
		virtual ~PttStatusEventReader();
	private:
		PttStatusEventReader(const PttStatusEventReader& theReader);

	};
};

#endif //__PTT_STATUS_EVENT_READER_H_INCLUDED__