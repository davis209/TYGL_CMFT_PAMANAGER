

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Workfile:$
  * @author:  Haipeng Jiang
  * @version: $Revision: 1.1.2.1 $
  *
  * Last modification: $Date: 2006/03/17 11:04:39 $
  * Last modified by:  $Author: haipeng $
  *
  * <description>
  */

#if !defined(SIMPLETEXTSDSEVENTREADER_H)
#define SIMPLETEXTSDSEVENTREADER_H

#include "RadioProtocol.h"
#include "RadioEventReader.h"

#include <string>
#include <vector>

namespace TA_IRS_App
{
	
	class SimpleTextSDSEventReader: public RadioEventReader
	{
	public:

		SimpleTextSDSEventReader(RadioEvent & event);
		
	private:
		static const ta_uint32 EVENT_SIMPLE_TEXT_SDS_PARAMETER_LENGTH;
	private:
		SimpleTextSDSEventReader();
		SimpleTextSDSEventReader & operator = (SimpleTextSDSEventReader & that);
	};
	
}

#endif