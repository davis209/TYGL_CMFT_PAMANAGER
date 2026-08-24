/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodSendSDS.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class encapsulates the Radio API SendSDS method.
  *
  */
///////////////////////////////////////////////////////////
//  RadioMethodSendSDS.cpp
//  Implementation of the Class RadioMethodSendSDS
//  Created on:      29-Nov-2003 09:57:09 AM
///////////////////////////////////////////////////////////

#include "RadioMethodSendSDS.h"
#include "RadioProtocol.h"
#include "core/utilities/src/TAAssert.h"

using namespace TA_IRS_App;

#define SEND_SDS_LENGTH                  229
#define SEND_SDS_DESTINATION_START         4
#define SEND_SDS_DATA_TYPE_START          36
#define SEND_SDS_DATA_LENGTH_START        40
#define SEND_SDS_DATA_START               44
#define SEND_SDS_PRE_CODE_START           40  // Yes it is the same as above as it is a union.

#define SDS_DATA_TYPE_SDS_TEXT     0         // Ajp to check these 
#define SDS_DATA_TYPE_PRE_CODED    1

// TD10791
//#define MAX_SDS_MESAGE_LENGTH      140   
const ta_uint32 RadioMethodSendSDS::MAX_SDS_MESAGE_LENGTH = 140;
//

RadioMethodSendSDS::RadioMethodSendSDS(RadioSessionReference sessionRef, RadioCallReference callRef) 
                   : RadioMethodCallReference(SEND_SDS_LENGTH,METHOD_SEND_SDS,sessionRef,callRef)
{
    setParameterLong( 0, callRef ); // limin++ CALL REFERENCE
}



RadioMethodSendSDS::~RadioMethodSendSDS()
{
}


/**
 * Set the destination TSI for this SDS message
 */
void RadioMethodSendSDS::setDestination(std::string destination)
{
	setParameterShortString(SEND_SDS_DESTINATION_START,destination);
}


/**
 * Set the message to be sent
 */
void RadioMethodSendSDS::setMessage(std::vector<unsigned char> data)
{
	TA_ASSERT(data.size()<=MAX_SDS_MESAGE_LENGTH,"Maximum SDS data length exceeded.");

	setParameterLong(SEND_SDS_DATA_TYPE_START,SDS_DATA_TYPE_SDS_TEXT);
	setParameterLong(SEND_SDS_DATA_LENGTH_START,data.size());
	for (unsigned int index=0;index<data.size();index++)
	{
		setParameterByte(index+SEND_SDS_DATA_START,data[index]);
	}
}


/**
 * Set the code to be sent.  Note, any preset message will be ignored.
 */
void RadioMethodSendSDS::setPreCode(short precode)
{
	setParameterLong(SEND_SDS_DATA_TYPE_START,SDS_DATA_TYPE_PRE_CODED);
	setParameterShort(SEND_SDS_PRE_CODE_START,precode);
}

// TD10791, TD14634
const ta_uint32 RadioMethodSendSDS::getMaxSdsMessageLength()
{
    return MAX_SDS_MESAGE_LENGTH;
}

