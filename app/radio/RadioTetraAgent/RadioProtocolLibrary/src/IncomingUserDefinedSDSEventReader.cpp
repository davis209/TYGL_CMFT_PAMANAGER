///////////////////////////////////////////////////////////
//  IncomingUserDefinedSDSEventReader.cpp
//  Implementation of the Class IncomingUserDefinedSDSEventReader
//  Created on:      27-Nov-2003 02:41:51 PM
///////////////////////////////////////////////////////////

#include "IncomingUserDefinedSDSEventReader.h"
#include "RadioProtocol.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

#define INCOMINGSDSTLTRANSFEREVENTREADER_VERBOSELOGGING

using namespace TA_IRS_App;

const ta_uint32 IncomingUserDefinedSDSEventReader::SDS_REFERENCE_START_POS = 0;
const ta_uint32 IncomingUserDefinedSDSEventReader::CALLING_NUMBER_START_POS = 4;
const ta_uint32 IncomingUserDefinedSDSEventReader::CALLED_NUMBER_START_POS = 36;

// these following 2 values are for reference only - use m_dataStartPosition
// and m_dataStartPosition+4 respectively, instead
const ta_uint32 IncomingUserDefinedSDSEventReader::DATA_LENGTH_START_POS = 68;
const ta_uint32 IncomingUserDefinedSDSEventReader::PID_POS = 72;
const ta_uint32 IncomingUserDefinedSDSEventReader::DATA_START_POS = 73;


IncomingUserDefinedSDSEventReader::IncomingUserDefinedSDSEventReader(RadioEvent& event)
: IncomingSDSEventReader(event, EVENT_INCOMING_USERDEFINED_SDS)
, m_dataStartPosition(IncomingUserDefinedSDSEventReader::DATA_LENGTH_START_POS)
{

}


IncomingUserDefinedSDSEventReader::IncomingUserDefinedSDSEventReader(RadioEvent& event, EventType eventType)
: IncomingSDSEventReader(event, eventType)
{
}

IncomingUserDefinedSDSEventReader::~IncomingUserDefinedSDSEventReader()
{

}

ta_uint32 IncomingUserDefinedSDSEventReader::getSDSReference() const
{
	FUNCTION_ENTRY("getSDSReference");
	FUNCTION_EXIT;
	return getParameterLong(SDS_REFERENCE_START_POS);
}

bool IncomingUserDefinedSDSEventReader::isText() const
{
	// this function is essentially for !isPreCoded()
	FUNCTION_ENTRY("isText");
	FUNCTION_EXIT;
	return (!isPreCoded());
}

bool IncomingUserDefinedSDSEventReader::isTrainMessage() const
{
	FUNCTION_ENTRY("isTrainMessage");
	FUNCTION_EXIT;
	return (getParameterChar(m_dataStartPosition+5)==0x09);
}

std::string IncomingUserDefinedSDSEventReader::getSenderITSI() const
{
	FUNCTION_ENTRY("getSenderITSI");
	FUNCTION_EXIT;
	return this->getParameterShortString(CALLING_NUMBER_START_POS);
}

std::string IncomingUserDefinedSDSEventReader::getDestinationITSI() const
{
	FUNCTION_ENTRY("getDestinationITSI");
	FUNCTION_EXIT;
	return this->getParameterShortString(CALLED_NUMBER_START_POS);
}

std::string IncomingUserDefinedSDSEventReader::getMessageText() const
{
	FUNCTION_ENTRY("getMessageText");
	// Get the length of the data // subtract 1 to exclude the PID data
	ta_uint32 dataLen = (getParameterLong(m_dataStartPosition)-1);
	// actual data starts after the length DWORD
	const ta_uint32 data_start_pos = m_dataStartPosition+5;

	std::string textMessage;
	textMessage.reserve(dataLen);

	// The basic_string append and operator+= appear to be broken on
	// STLPort - using either
	// 
	//      s.append(1,charTemp);
	//
	// OR
	//
	//      s += charTemp;
	//
	// crashes the agent.  As a workaround, we use a single-byte char array
	// which experience shows is reliable.
	//
	char buff[4] = {' ','\0','\0','\0'};

#if defined INCOMINGSDSTLTRANSFEREVENTREADER_VERBOSELOGGING
	std::string hexMessage = "";
	hexMessage.reserve(dataLen*3); // 2 for the hex chars + 1 for each space
	char hexTmp[8] = {0};

	std::string asciiMessage = "";
	asciiMessage.reserve(dataLen);
#endif // #if defined INCOMINGSDSTLTRANSFEREVENTREADER_VERBOSELOGGING

	// Get the message
	for (ta_uint32 index = data_start_pos;
		index < (data_start_pos + dataLen);
		index++)
	{
		char charTemp = getParameterChar(index);
		buff[0] = charTemp;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"getMessageText() textMessage.append(\"%s\") (%d)", buff, (int)charTemp);
		textMessage.append(buff);

#if defined INCOMINGSDSTLTRANSFEREVENTREADER_VERBOSELOGGING
		int c1 = (charTemp & 0xF0) >> 8;
		int c2 = (charTemp & 0x0F);
		sprintf(hexTmp, "%.1X%.1X ", c1, c2);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"getMessageText() hexMessage.append(\"%s\")", hexTmp);
		hexMessage.append(hexTmp);

		if (charTemp<' ')
		{
			buff[0] = '.';
		}
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"getMessageText() asciiMessage.append(\"%s\")", buff);
		asciiMessage.append(buff);
#endif // #if defined INCOMINGSDSTLTRANSFEREVENTREADER_VERBOSELOGGING
	}

#if defined INCOMINGSDSTLTRANSFEREVENTREADER_VERBOSELOGGING
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
		"getMessageText() dataLen=%lu dataLenStartPos=%lu dataStartPos=%lu",
		dataLen, m_dataStartPosition, data_start_pos);
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
		"getMessageText() data=\"%s\"", hexMessage.c_str() );
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
		"getMessageText() ascii=\"%s\"", asciiMessage.c_str() );
#endif // #if defined INCOMINGSDSTLTRANSFEREVENTREADER_VERBOSELOGGING

	FUNCTION_EXIT;
	return (textMessage);
}

std::vector<unsigned char> IncomingUserDefinedSDSEventReader::getMessageData() const
{
	FUNCTION_ENTRY("getMessageData");
	std::vector<unsigned char>   messageData;

	// Get the length of the data // subtract 1 to exclude the PID data
	ta_uint32 dataLen = (getParameterLong(m_dataStartPosition) - 1);
	// actual data starts after the length DWORD
	const ta_uint32 data_start_pos = m_dataStartPosition+5;

	// Get the message
	for (ta_uint32 index = data_start_pos;
		index < (data_start_pos + dataLen);
		index++)
	{
		messageData.push_back(getParameterChar(index));
	}

	FUNCTION_EXIT;
	return messageData;
}

bool IncomingUserDefinedSDSEventReader::isPreCoded() const
{
	FUNCTION_ENTRY("isPreCoded");
	FUNCTION_EXIT;
	return (false);
}

unsigned short IncomingUserDefinedSDSEventReader::getPreCodedStatus() const
{
	FUNCTION_ENTRY("getPreCodedStatus");
	FUNCTION_EXIT;
	return (0);
}

unsigned short IncomingUserDefinedSDSEventReader::getPID() const
{
	FUNCTION_ENTRY("getPID");
	FUNCTION_EXIT;
	return getParameterShort(PID_POS);
}