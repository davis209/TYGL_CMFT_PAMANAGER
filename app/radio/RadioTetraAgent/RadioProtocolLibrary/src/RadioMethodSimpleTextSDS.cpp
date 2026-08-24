/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source: /cvs/1TS-9999-T/code/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/Attic/RadioMethodSimpleTextSDS.cpp,v $
  * @author:  Robert van Hugten
  * @version: $Revision: 1.1.2.4 $
  *
  * Last modification: $Date: 2006/03/27 04:27:55 $
  * Last modified by:  $Author: haipeng $
  *
  * This class encapsulates the Radio API SendSDS method.
  *
  */
///////////////////////////////////////////////////////////
//  RadioMethodSimpleTextSDS.cpp
//  Implementation of the Class RadioMethodSimpleTextSDS
//  Created on:      29-Nov-2003 09:57:09 AM
///////////////////////////////////////////////////////////

#include "RadioMethodSimpleTextSDS.h"
#include "RadioProtocol.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using namespace TA_IRS_App;

// TD10791
const ta_uint32 RadioMethodSimpleTextSDS::MAX_SDS_MESAGE_LENGTH = 140;
//

const ta_uint32 RadioMethodSimpleTextSDS::SIMPLE_SDS_DESTINATION_START =  4;
const ta_uint32 RadioMethodSimpleTextSDS::SIMPLE_SDS_DATA_TYPE_START   =  36;
const ta_uint32 RadioMethodSimpleTextSDS::SIMPLE_SDS_DATA_LENGTH_START =  40;
const ta_uint32 RadioMethodSimpleTextSDS::SIMPLE_SDS_DATA_START        =  44;
const ta_uint32 RadioMethodSimpleTextSDS::SIMPLE_SDS_LENGTH            =  
RadioMethodSimpleTextSDS::MAX_SDS_MESAGE_LENGTH + RadioMethodSimpleTextSDS::SIMPLE_SDS_DATA_START;


RadioMethodSimpleTextSDS::RadioMethodSimpleTextSDS(RadioSessionReference sessionRef, RadioCallReference callRef) 
: RadioMethodCallReference(SIMPLE_SDS_LENGTH, METHOD_SIMPLE_TEXT_SDS, sessionRef, callRef)
{
	FUNCTION_ENTRY("RadioMethodSimpleTextSDS");
	FUNCTION_EXIT;
}



RadioMethodSimpleTextSDS::~RadioMethodSimpleTextSDS()
{
}


/**
 * Set the destination TSI for this SDS message
 */
void RadioMethodSimpleTextSDS::setCalledNumber(std::string destination)
{
	FUNCTION_ENTRY("setCalledNumber");
	setParameterShortString(SIMPLE_SDS_DESTINATION_START, destination);
	FUNCTION_EXIT;
}


/**
 * Set the message to be sent
 */
void RadioMethodSimpleTextSDS::setMessage(std::vector<unsigned char> data)
{
	FUNCTION_ENTRY("setMessage");
	TA_ASSERT(data.size() <= MAX_SDS_MESAGE_LENGTH,"Maximum SDS data length exceeded.");

	setParameterLong(SIMPLE_SDS_DATA_LENGTH_START, data.size());
	for (unsigned int index = 0; index < data.size(); index++)
	{
		setParameterByte(index + SIMPLE_SDS_DATA_START, data[index]);
	}
	FUNCTION_EXIT;
}


// TD10791, TD14634
const ta_uint32 RadioMethodSimpleTextSDS::getMaxSdsMessageLength()
{
	FUNCTION_ENTRY("getMaxSdsMessageLength");
    return MAX_SDS_MESAGE_LENGTH;
	FUNCTION_EXIT;
}

