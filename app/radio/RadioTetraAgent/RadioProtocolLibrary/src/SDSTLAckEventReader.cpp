

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

#include "SDSTLAckEventReader.h"

using namespace TA_IRS_App;

const ta_uint32 SDSTLAckEventReader::EVENT_SDSTL_ARK_PARAMETER_LENGTH = 4;

SDSTLAckEventReader::SDSTLAckEventReader(RadioEvent & event)
: RadioEventReader(event, EVENT_SDSTL_ACK, EVENT_SDSTL_ARK_PARAMETER_LENGTH)
{
	
}