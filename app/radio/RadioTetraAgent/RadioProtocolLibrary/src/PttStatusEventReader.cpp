

#include "PttStatusEventReader.h"
#include "RadioEvent.h"
#include "RadioProtocol.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_App
{
	PttStatusEventReader::PttStatusEventReader(RadioEvent& event) 
		: RadioEventReader(event, EVENT_PTT_STATUS)
	{
	}

	PttStatusEventReader::~PttStatusEventReader()
	{
	}
}