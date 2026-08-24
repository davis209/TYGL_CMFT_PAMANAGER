

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Workfile:$
  * @author:  Haipeng Jiang
  * @version: $Revision: 1.1.2.2.24.1 $
  *
  * Last modification: $Date: 2007/04/12 03:19:53 $
  * Last modified by:  $Author: zhouyuan $
  *
  * <description>
  */

#include "SDSTLTransferEventReader.h"

using namespace TA_IRS_App;

const ta_uint32 SDSTLTransferEventReader::EVENT_SDSTL_TRANSFER_PARAMETER_LENGTH = 4;

SDSTLTransferEventReader::SDSTLTransferEventReader(RadioEvent & event)
: RadioEventReader(event, EVENT_SDSTL_TRANSFER, EVENT_SDSTL_TRANSFER_PARAMETER_LENGTH)
{
	
}