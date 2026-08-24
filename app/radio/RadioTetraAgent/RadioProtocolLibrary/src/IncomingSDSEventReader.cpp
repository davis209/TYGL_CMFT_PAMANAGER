/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/IncomingSDSEventReader.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
///////////////////////////////////////////////////////////
//  IncomingSDSEventReader.cpp
//  Implementation of the Class IncomingSDSEventReader
//  Created on:      27-Nov-2003 02:41:51 PM
///////////////////////////////////////////////////////////

#include "core/types/src/ta_types.h"
#include "IncomingSDSEventReader.h"
#include "RadioProtocol.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using namespace TA_IRS_App;

#define SDS_REFERENCE_START     0
#define SDS_SENDER_START        4
#define SDS_DESTINATION_START  36
#define SDS_DATA_TYPE_START    68
#define SDS_DATA_LENGTH_START  72
#define SDS_DATA_START         76

#define SDS_STATUS_MODE         1  //?? this needs to be checked with Marconi.
#define SDS_TEXT_MODE			0

#define INCOMINGSDSEVENTREADER_VERBOSELOGGING

IncomingSDSEventReader::IncomingSDSEventReader(RadioEvent& event)
: RadioEventReader(event, EVENT_INCOMING_SDS, EVENT_INCOMING_SDS_PARAMETER_LENGTH)
{

}


IncomingSDSEventReader::IncomingSDSEventReader(RadioEvent& event, EventType eventType)
: RadioEventReader(event, eventType)
{
}

IncomingSDSEventReader::~IncomingSDSEventReader()
{

}



/**
 * Get the ITSI for the sender.
 */
std::string IncomingSDSEventReader::getSenderITSI() const
{
	return getParameterShortString(SDS_SENDER_START);
}

/**
 * Get the ITSI for the destination (receiver).
 */
std::string IncomingSDSEventReader::getDestinationITSI() const
{
	return getParameterShortString(SDS_DESTINATION_START);
}


/**
 * Determines if the event is a pre-coded status message
 *
 */
bool IncomingSDSEventReader::isPreCoded() const
{
	return (getParameterLong(SDS_DATA_TYPE_START)==SDS_STATUS_MODE);
}


/**
 * Determines if the event is a fixed text message
 *
 */
bool IncomingSDSEventReader::isText() const
{
	return (getParameterLong(SDS_DATA_TYPE_START)==SDS_TEXT_MODE);
}

/**
 * Get the text associated with the message
 */
std::string IncomingSDSEventReader::getMessageText() const
{
	TA_ASSERT(isText(),"Invalid attempt to read text from a data message.");

	ta_uint32 dataLen = getParameterLong(SDS_DATA_LENGTH_START);
	std::string textMessage;
    textMessage.reserve(dataLen);

#if defined INCOMINGSDSEVENTREADER_VERBOSELOGGING
    std::string hexMessage;
    hexMessage.reserve(dataLen*3); // 2 for the hex chars + 1 for each space
    char hexTmp[8] = {0};

    std::string asciiMessage;
    asciiMessage.reserve(dataLen);
#endif // #if defined INCOMINGSDSEVENTREADER_VERBOSELOGGING

	for (ta_uint32 index = SDS_DATA_START;index<SDS_DATA_START+dataLen;index++)
	{
		char charTemp = getParameterChar(index);
		textMessage.append(1, charTemp);

#if defined INCOMINGSDSEVENTREADER_VERBOSELOGGING
            sprintf(hexTmp, "%.2X ", (int)charTemp);
            hexMessage.append(hexTmp);

            if (charTemp>=' ')
            {
                asciiMessage.append(1, charTemp);
            }
            else
            {
                asciiMessage.append(1, '.');
            }
#endif // #if defined INCOMINGSDSEVENTREADER_VERBOSELOGGING
	}

#if defined INCOMINGSDSEVENTREADER_VERBOSELOGGING
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                    "getMessageText() dataLen=%lu dataLenStartPos=%lu dataStartPos=%lu",
                    dataLen, SDS_DATA_LENGTH_START, SDS_DATA_START);
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                    "getMessageText() data=\"%s\"", hexMessage.c_str() );
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                    "getMessageText() ascii=\"%s\"", asciiMessage.c_str() );
#endif // #if defined INCOMINGSDSEVENTREADER_VERBOSELOGGING

	return textMessage;
}


/**
 * Get the data associated with the message
 */
std::vector<unsigned char> IncomingSDSEventReader::getMessageData() const
{
	std::vector<unsigned char>   messageData;

	// Get the length of the data
	ta_uint32 dataLen = getParameterLong(SDS_DATA_LENGTH_START);
	TA_ASSERT(dataLen<EVENT_INCOMING_SDS_PARAMETER_LENGTH-SDS_DATA_START,"Invalid fixed string data length.");

	// Get the message
	for (unsigned int index = SDS_DATA_START;index<SDS_DATA_START+dataLen;index++)
	{
		messageData.push_back(getParameterChar(index));
	}

	return messageData;
}


unsigned short IncomingSDSEventReader::getPreCodedStatus() const
{
	return (getParameterShort(SDS_DATA_LENGTH_START));
}


/**
 * Determines if the SDS is a coded train message
 *
 */
bool IncomingSDSEventReader::isTrainMessage() const
{
	return (getParameterChar(SDS_DATA_START)==0x09);

}

/**
 * Get the SDS reference
 *
 */
ta_uint32 IncomingSDSEventReader::getSDSReference() const
{
	return getParameterLong(SDS_REFERENCE_START);
}
