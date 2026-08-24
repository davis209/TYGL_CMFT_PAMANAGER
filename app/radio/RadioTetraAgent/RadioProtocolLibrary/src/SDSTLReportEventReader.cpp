

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

#include "SDSTLReportEventReader.h"

using namespace TA_IRS_App;

const ta_uint32 SDSTLReportEventReader::EVENT_SDSTL_REPORT_PARAMETER_LENGTH = 4;


SDSTLReportEventReader::SDSTLReportEventReader(RadioEvent & event)
: RadioEventReader(event, EVENT_SDSTL_REPORT, EVENT_INCOMING_SDS_PARAMETER_LENGTH)
{
	
}