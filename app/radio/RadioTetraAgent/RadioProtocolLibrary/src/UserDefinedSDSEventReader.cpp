

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Workfile:$
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: 1.1.2.2.24.1 $
  *
  * Last modification: $Date: 2007/04/12 03:19:53 $
  * Last modified by:  $Author: zhouyuan $
  *
  * <description>
  */

#include "UserDefinedSDSEventReader.h"

using namespace TA_IRS_App;

const ta_uint32 UserDefinedSDSEventReader::EVENT_SDSTL_TRANSFER_PARAMETER_LENGTH = 4;

UserDefinedSDSEventReader::UserDefinedSDSEventReader(RadioEvent & event)
: RadioEventReader(event, EVENT_USER_DEFINED_SDS, EVENT_SDSTL_TRANSFER_PARAMETER_LENGTH)
{

}