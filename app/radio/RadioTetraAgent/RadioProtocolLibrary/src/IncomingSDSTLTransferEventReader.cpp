

/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $Workfile:$
* @author:  Haipeng Jiang
* @version: $Revision: 1.1.2.8 $
*
* Last modification: $Date: 2006/09/13 08:08:55 $
* Last modified by:  $Author: robertvh $
*
* <description>
*/


#include "IncomingSDSTLTransferEventReader.h"
#include "IncomingSDSTLTransferExtendedEventReader.h"
#include "RadioEvent.h"

#include "core/utilities/src/DebugUtil.h"

#include <sstream>

#define INCOMINGSDSTLTRANSFEREVENTREADER_VERBOSELOGGING

namespace TA_IRS_App
{


    const ta_uint32 IncomingSDSTLTransferEventReader::SDS_REFERENCE_START_POS = 0;
    const ta_uint32 IncomingSDSTLTransferEventReader::CALLING_NUMBER_START_POS = 4;
    const ta_uint32 IncomingSDSTLTransferEventReader::CALLED_NUMBER_START_POS = 36;
    const ta_uint32 IncomingSDSTLTransferEventReader::PROTOCOL_START_POS = 68;
    const ta_uint32 IncomingSDSTLTransferEventReader::TIMESTAMP_MONTH_START_POS = 69;
    const ta_uint32 IncomingSDSTLTransferEventReader::TIMESTAMP_DAY_START_POS = 70;
    const ta_uint32 IncomingSDSTLTransferEventReader::TIMESTAMP_HOUR_START_POS = 71;
    const ta_uint32 IncomingSDSTLTransferEventReader::TIMESTAMP_MINUTE_START_POS = 72;
    const ta_uint32 IncomingSDSTLTransferEventReader::RESPONSE_REQUESTED_START_POS = 73;
    const ta_uint32 IncomingSDSTLTransferEventReader::SHORT_MODE_START_POS = 74;
    // these following 2 values are for reference only - use m_dataStartPosition
    // and m_dataStartPosition+4 respectively, instead
    const ta_uint32 IncomingSDSTLTransferEventReader::DATA_LENGTH_START_POS = 75;
    const ta_uint32 IncomingSDSTLTransferEventReader::DATA_START_POS = 79;

    IncomingSDSTLTransferEventReader::IncomingSDSTLTransferEventReader(RadioEvent & radioEvent)
        : IncomingSDSEventReader(radioEvent, EVENT_INCOMING_TLSDS_TRANSFER)
        , m_dataStartPosition(IncomingSDSTLTransferEventReader::DATA_LENGTH_START_POS)
    {
    }

    IncomingSDSTLTransferEventReader::IncomingSDSTLTransferEventReader(RadioEvent & radioEvent, EventType eventType, ta_uint32 dataStartPos)
        : IncomingSDSEventReader(radioEvent, eventType)
        , m_dataStartPosition(dataStartPos)
    {
    }

    IncomingSDSTLTransferEventReader::~IncomingSDSTLTransferEventReader()
    {

    }

    std::string IncomingSDSTLTransferEventReader::getCallingNumber() const
    {
        FUNCTION_ENTRY("getCallingNumber");
        FUNCTION_EXIT;
        return this->getParameterShortString(CALLING_NUMBER_START_POS);
    }

    std::string IncomingSDSTLTransferEventReader::getCalledNumber() const
    {
        FUNCTION_ENTRY("getCalledNumber");
        FUNCTION_EXIT;
        return this->getParameterShortString(CALLED_NUMBER_START_POS);
    }

    ESDSTLProtocol IncomingSDSTLTransferEventReader::getProtocol() const
    {
        FUNCTION_ENTRY("getProtocol");
        FUNCTION_EXIT;
        return static_cast<ESDSTLProtocol>(this->getParameterChar(PROTOCOL_START_POS));
    }

    tm IncomingSDSTLTransferEventReader::getTimestamp() const
    {
        FUNCTION_ENTRY("getTimestamp");
        tm temp;
        temp.tm_mon = static_cast<int>(this->getParameterChar(TIMESTAMP_MONTH_START_POS));
        temp.tm_mday = static_cast<int>(this->getParameterChar(TIMESTAMP_DAY_START_POS));
        temp.tm_hour = static_cast<int>(this->getParameterChar(TIMESTAMP_HOUR_START_POS));
        temp.tm_min = static_cast<int>(this->getParameterChar(TIMESTAMP_MINUTE_START_POS));

        FUNCTION_EXIT;
        return temp;
    }

    bool IncomingSDSTLTransferEventReader::getReceiveRecieptFlag() const
    {
        FUNCTION_ENTRY("getReceiveRecieptFlag");
        ESDSTLResponseRequested response = static_cast<ESDSTLResponseRequested>(getParameterChar(RESPONSE_REQUESTED_START_POS));
        if ( response == SDSTL_REPREQ_ONRECEIVE ||
            response == SDSTL_REPREQ_ONRECEIVE_ONCONSUME )
        {
            FUNCTION_EXIT;
            return true;
        }
        FUNCTION_EXIT;
        return false;
    }

    bool IncomingSDSTLTransferEventReader::getComsumedRecieptFlag() const
    {
        FUNCTION_ENTRY("getComsumedRecieptFlag");
        ESDSTLResponseRequested response = static_cast<ESDSTLResponseRequested>(getParameterChar(RESPONSE_REQUESTED_START_POS));
        if ( response == SDSTL_REPREQ_ONCONSUME ||
            response == SDSTL_REPREQ_ONRECEIVE_ONCONSUME )
        {
            FUNCTION_EXIT;
            return true;
        }
        FUNCTION_EXIT;
        return false;
    }

    ESDSTLReportType IncomingSDSTLTransferEventReader::getReportType() const
    {
        FUNCTION_ENTRY("getReportType");
        FUNCTION_EXIT;
        return static_cast<ESDSTLReportType>(this->getParameterChar(SHORT_MODE_START_POS));
    }

    std::string IncomingSDSTLTransferEventReader::getMessageText() const
    {
        FUNCTION_ENTRY("getMessageText");

		ta_uint32 dataLen = getParameterLong(m_dataStartPosition);
        // actual data starts after the length DWORD
        const ta_uint32 data_start_pos = m_dataStartPosition+4;

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


    ta_uint32 IncomingSDSTLTransferEventReader::getSDSReference() const
    {
        FUNCTION_ENTRY("getSDSReference");
        FUNCTION_EXIT;
        return getParameterLong(SDS_REFERENCE_START_POS);
    }

    bool IncomingSDSTLTransferEventReader::isText() const
    {
        // this function is essentially for !isPreCoded()
        FUNCTION_ENTRY("isText");
        FUNCTION_EXIT;
        return (!isPreCoded());
    }

    bool IncomingSDSTLTransferEventReader::isTrainMessage() const
    {
        FUNCTION_ENTRY("isTrainMessage");
        FUNCTION_EXIT;
        return (getParameterChar(m_dataStartPosition+4)==0x09);
    }

    std::string IncomingSDSTLTransferEventReader::getSenderITSI() const
    {
        FUNCTION_ENTRY("getSenderITSI");
        FUNCTION_EXIT;
        return (getCallingNumber());
    }

    std::string IncomingSDSTLTransferEventReader::getDestinationITSI() const
    {
        FUNCTION_ENTRY("getDestinationITSI");
        FUNCTION_EXIT;
        return (getCalledNumber());
    }


    std::vector<unsigned char> IncomingSDSTLTransferEventReader::getMessageData() const
    {
        FUNCTION_ENTRY("getMessageData");
        std::vector<unsigned char>   messageData;

        // Get the length of the data
		ta_uint32 dataLen = getParameterLong(m_dataStartPosition);
        // actual data starts after the length DWORD
        const ta_uint32 data_start_pos = m_dataStartPosition+4;

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

    bool IncomingSDSTLTransferEventReader::isPreCoded() const
    {
        FUNCTION_ENTRY("isPreCoded");
        FUNCTION_EXIT;
        return (false);
    }

    unsigned short IncomingSDSTLTransferEventReader::getPreCodedStatus() const
    {
        FUNCTION_ENTRY("getPreCodedStatus");
        FUNCTION_EXIT;
        return (0);
    }

}
