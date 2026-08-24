

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

#if !defined(SDSTLREPORTEVENTREADER_H)
#define SDSTLREPORTEVENTREADER_H


#include "RadioProtocol.h"
#include "RadioEventReader.h"

#include <string>
#include <vector>

namespace TA_IRS_App
{
	
	class SDSTLReportEventReader : public RadioEventReader
	{
	public:
		SDSTLReportEventReader (RadioEvent & event);

	private:
		static const ta_uint32 EVENT_SDSTL_REPORT_PARAMETER_LENGTH;
	private:
		SDSTLReportEventReader ();
		SDSTLReportEventReader  & operator = (SDSTLReportEventReader  & that);
	};
}

#endif