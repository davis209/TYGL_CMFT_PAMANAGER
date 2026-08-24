

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Workfile:$
  * @author:  Haipeng Jiang
  * @version: $Revision: 1.1.2.2 $
  *
  * Last modification: $Date: 2006/03/10 10:50:39 $
  * Last modified by:  $Author: haipeng $
  *
  * <description>
  */

#if !defined(SDSTLACKEVENTREADER_H)
#define SDSTLACKEVENTREADER_H

#include "RadioProtocol.h"
#include "RadioEventReader.h"
#include "core/types/src/ta_types.h"
#include <string>
#include <vector>

namespace TA_IRS_App
{
	
	class SDSTLAckEventReader: public RadioEventReader
	{
	public:
		SDSTLAckEventReader(RadioEvent & event);

	private:
		static const ta_uint32 EVENT_SDSTL_ARK_PARAMETER_LENGTH;
	private:
		SDSTLAckEventReader();
		SDSTLAckEventReader & operator = (SDSTLAckEventReader & that);
	};
	
}

#endif