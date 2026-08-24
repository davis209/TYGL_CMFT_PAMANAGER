

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Workfile:$
  * @author:  Haipeng Jiang
  * @version: $Revision: 1.1.2.6 $
  *
  * Last modification: $Date: 2006/09/13 08:08:55 $
  * Last modified by:  $Author: robertvh $
  *
  * <description>
  */


#include "IncomingSimpleTextSDSEventReader.h"
#include "RadioSDSTLTypes.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include <sstream>

using namespace TA_IRS_App;

const ta_uint32 IncomingSimpleTextSDSEventReader::MAX_SDS_MESAGE_LENGTH = 140;
	 
const ta_uint32 IncomingSimpleTextSDSEventReader::REFERENCE_START_POS = 0;
const ta_uint32 IncomingSimpleTextSDSEventReader::CALLING_NUMBER_START_POS = 4;
const ta_uint32 IncomingSimpleTextSDSEventReader::CALLED_NUMBER_START_POS = 36;
const ta_uint32 IncomingSimpleTextSDSEventReader::DATA_LENGTH_START_POS = 68;
const ta_uint32 IncomingSimpleTextSDSEventReader::DATA_START_POS = 72;


#define INCOMINGSIMPLETEXTSDSEVENTREADER_VERBOSELOGGING

IncomingSimpleTextSDSEventReader::IncomingSimpleTextSDSEventReader(RadioEvent &radioEvent)
: IncomingSDSEventReader(radioEvent, EVENT_INCOMING_SIMPLE_TEXT_SDS)
{
	
}

IncomingSimpleTextSDSEventReader::~IncomingSimpleTextSDSEventReader()
{
	
}

std::string IncomingSimpleTextSDSEventReader::getCallingNumber() const
{
	FUNCTION_ENTRY("getCallingNumber");
	FUNCTION_EXIT;
	return this->getParameterShortString(CALLING_NUMBER_START_POS);
}

std::string IncomingSimpleTextSDSEventReader::getCalledNumber() const
{
	FUNCTION_ENTRY("getCalledNumber");
	FUNCTION_EXIT;
	return this->getParameterShortString(CALLED_NUMBER_START_POS);
}

std::string IncomingSimpleTextSDSEventReader::getMessageText() const
{
	FUNCTION_ENTRY("getMessageText");
	ta_uint32 dataLen = getParameterLong(DATA_LENGTH_START_POS);
	TA_ASSERT(dataLen < MAX_SDS_MESAGE_LENGTH, "Data length exceeds the maximum length of the message");

    std::string textMessage;
    textMessage.reserve(dataLen);

#if defined INCOMINGSIMPLETEXTSDSEVENTREADER_VERBOSELOGGING
    std::string hexMessage;
    hexMessage.reserve(dataLen*3); // 2 for the hex chars + 1 for each space
    char hexTmp[8] = {0};

    std::string asciiMessage;
    asciiMessage.reserve(dataLen);
#endif // #if defined INCOMINGSIMPLETEXTSDSEVENTREADER_VERBOSELOGGING

    // Get the message
    for (ta_uint32 index = DATA_START_POS;
            index < (DATA_START_POS + dataLen);
            index++)
    {
        char charTemp = getParameterChar(index);
        textMessage.append(1, charTemp);

#if defined INCOMINGSIMPLETEXTSDSEVENTREADER_VERBOSELOGGING
        sprintf(hexTmp, "%0.2X ", (int)charTemp);
        hexMessage.append(hexTmp);

        if (charTemp>=' ')
        {
            asciiMessage.append(1, charTemp);
        }
        else
        {
            asciiMessage.append(1, '.');
        }
#endif // #if defined INCOMINGSIMPLETEXTSDSEVENTREADER_VERBOSELOGGING
    }

#if defined INCOMINGSIMPLETEXTSDSEVENTREADER_VERBOSELOGGING
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                    "getMessageText() dataLen=%lu dataLenStartPos=%lu dataStartPos=%lu",
                    dataLen, DATA_LENGTH_START_POS, DATA_START_POS);
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                    "getMessageText() data=\"%s\"", hexMessage.c_str() );
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                    "getMessageText() ascii=\"%s\"", asciiMessage.c_str() );
#endif // #if defined INCOMINGSIMPLETEXTSDSEVENTREADER_VERBOSELOGGING

	FUNCTION_EXIT;	
	return textMessage;
}

ta_uint32 IncomingSimpleTextSDSEventReader::getSDSReference() const
{
	FUNCTION_ENTRY("getSDSReference");
	FUNCTION_EXIT;	
	return this->getParameterLong(REFERENCE_START_POS);
}

bool IncomingSimpleTextSDSEventReader::isText() const
{
    // this function is essentially for !isPreCoded()
    FUNCTION_ENTRY("isText");
    FUNCTION_EXIT;
    return (!isPreCoded());
}

bool IncomingSimpleTextSDSEventReader::isTrainMessage() const
{
    FUNCTION_ENTRY("isTrainMessage");
    FUNCTION_EXIT;
    return (getParameterChar(DATA_START_POS)==0x09);
}

std::string IncomingSimpleTextSDSEventReader::getSenderITSI() const
{
    FUNCTION_ENTRY("getSenderITSI");
    FUNCTION_EXIT;
    return (getCallingNumber());
}

std::string IncomingSimpleTextSDSEventReader::getDestinationITSI() const
{
    FUNCTION_ENTRY("getDestinationITSI");
    FUNCTION_EXIT;
    return (getCalledNumber());
}


std::vector<unsigned char> IncomingSimpleTextSDSEventReader::getMessageData() const
{
    FUNCTION_ENTRY("getMessageData");
    std::vector<unsigned char>   messageData;

    // Get the length of the data
	ta_uint32 dataLen = getParameterLong(DATA_LENGTH_START_POS);

    // Get the message
    for (ta_uint32 index = DATA_START_POS;
            index < (DATA_START_POS + dataLen);
            index++)
    {
        messageData.push_back(getParameterChar(index));
    }

    FUNCTION_EXIT;
    return messageData;
}

bool IncomingSimpleTextSDSEventReader::isPreCoded() const
{
    FUNCTION_ENTRY("isPreCoded");
    FUNCTION_EXIT;
    return (false);
}

unsigned short IncomingSimpleTextSDSEventReader::getPreCodedStatus() const
{
    FUNCTION_ENTRY("getPreCodedStatus");
    FUNCTION_EXIT;
    return (0);
}
