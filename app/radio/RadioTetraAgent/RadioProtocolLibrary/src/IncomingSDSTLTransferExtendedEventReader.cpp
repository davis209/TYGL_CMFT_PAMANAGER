

/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $Workfile:$
* @author:  Haipeng Jiang
* @version: $Revision: 1.1.2.6 $
*
* Last modification: $Date: 2006/08/23 08:01:57 $
* Last modified by:  $Author: robertvh $
*
* <description>
*/

#include "IncomingSDSTLTransferExtendedEventReader.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_App
{
	
	const ta_uint32 IncomingSDSTLTransferExtendedEventReader::CODE_PAGE_START_POS = 75;
	const ta_uint32 IncomingSDSTLTransferExtendedEventReader::DATA_LENGTH_START_POS = 76; 
	const ta_uint32 IncomingSDSTLTransferExtendedEventReader::DATA_START_POS = 80; 
	
	
	IncomingSDSTLTransferExtendedEventReader::IncomingSDSTLTransferExtendedEventReader(RadioEvent & radioEvent)
    : IncomingSDSTLTransferEventReader(radioEvent, EVENT_INCOMING_TLSDS_TRANSFER_EX, IncomingSDSTLTransferExtendedEventReader::DATA_LENGTH_START_POS)
	{
		FUNCTION_ENTRY("IncomingSDSTLTransferExtendedEventReader constructor()");
		FUNCTION_EXIT;
	}

	IncomingSDSTLTransferExtendedEventReader::~IncomingSDSTLTransferExtendedEventReader()
	{

	}
	
	ECodePage IncomingSDSTLTransferExtendedEventReader::getCharacterEncoding() 
	{
		FUNCTION_ENTRY("getCharacterEncoding");
		FUNCTION_EXIT;
		return static_cast<ECodePage>(this->getParameterChar(CODE_PAGE_START_POS));
	}
	
}
