/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source: /cvs/1TS-9999-T/code/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/Attic/RadioMethodUserDefinedSDS.cpp,v $
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: 1.1.2.2.38.2 $
  *
  * Last modification: $Date: 2006/02/24 07:26:06 $
  * Last modified by:  $Author: haipeng $
  *
  * This class encapsulates the Radio API SendSDS method.
  *
  */
///////////////////////////////////////////////////////////
//  RadioMethodUserDefinedSDS.cpp
//  Implementation of the Class RadioMethodUserDefinedSDS
//  Created on:      29-Nov-2003 09:57:09 AM
///////////////////////////////////////////////////////////

#include "RadioMethodUserDefinedSDS.h"
#include "RadioProtocol.h"
#include "core/utilities/src/TAAssert.h"

using namespace TA_IRS_App;

const ta_uint32 RadioMethodUserDefinedSDS::CALLED_NUMBER_POSITION = 4;
const ta_uint32 RadioMethodUserDefinedSDS::DATA_LENGTH_POSITION = 36;
const ta_uint32 RadioMethodUserDefinedSDS::PID_POSITION = 40;
const ta_uint32 RadioMethodUserDefinedSDS::DATA_POSITION = 41;
const ta_uint32 RadioMethodUserDefinedSDS::MAX_MESAGE_LENGTH = 140;
const ta_uint32 RadioMethodUserDefinedSDS::PARAMETER_LENGTH = 
	RadioMethodUserDefinedSDS::MAX_MESAGE_LENGTH + RadioMethodUserDefinedSDS::DATA_POSITION;

const unsigned short RadioMethodUserDefinedSDS::DEFAULT_PID = 71;

RadioMethodUserDefinedSDS::RadioMethodUserDefinedSDS(RadioSessionReference sessionRef, RadioCallReference callRef) 
	: RadioMethodCallReference(PARAMETER_LENGTH, METHOD_USERDEFINED_SDS,sessionRef,callRef)
{
    setParameterLong( 0, callRef ); // limin++ CALL REFERENCE
	setPID(DEFAULT_PID); // we will set the PID defaulted to 70
}

RadioMethodUserDefinedSDS::~RadioMethodUserDefinedSDS()
{
}

/**
 * Set the destination TSI for this SDS message
 */
void RadioMethodUserDefinedSDS::setCalledNumber(const std::string & number)
{
	setParameterShortString(CALLED_NUMBER_POSITION, number);
}

/**
 * Set the message to be sent
 */
void RadioMethodUserDefinedSDS::setMessage(std::vector<unsigned char> data)
{
	TA_ASSERT(data.size()<=MAX_MESAGE_LENGTH,"Maximum SDS data length exceeded.");
	setMessageSize(DATA_POSITION + data.size());
	setParameterLong(DATA_LENGTH_POSITION, data.size()+ 1);
	for (unsigned int index=0;index<data.size();index++)
	{
		setParameterByte(index+DATA_POSITION, data[index]);
	}
}

// TD10791, TD14634
const ta_uint32 RadioMethodUserDefinedSDS::getMaxSdsMessageLength()
{
    return MAX_MESAGE_LENGTH;
}

void RadioMethodUserDefinedSDS::setPID(const unsigned short PID)
{
	setParameterShort(PID_POSITION, PID);
}

