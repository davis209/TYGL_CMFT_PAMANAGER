 

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Workfile:$
  * @author:  Haipeng Jiang
  * @version: $Revision: 1.1.2.2 $
  *
  * Last modification: $Date: 2006/04/24 09:05:43 $
  * Last modified by:  $Author: haipeng $
  *
  * <description>
  */


#include "SimpleTextSDSEventReader.h"

using namespace TA_IRS_App;

const ta_uint32 SimpleTextSDSEventReader::EVENT_SIMPLE_TEXT_SDS_PARAMETER_LENGTH = 4;

SimpleTextSDSEventReader::SimpleTextSDSEventReader(RadioEvent & event)
: RadioEventReader(event, EVENT_SIMPLE_TEXT_SDS, EVENT_SIMPLE_TEXT_SDS_PARAMETER_LENGTH)
{
	
}