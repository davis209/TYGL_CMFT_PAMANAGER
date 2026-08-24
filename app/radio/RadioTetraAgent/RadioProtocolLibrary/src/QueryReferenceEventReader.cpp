/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/QueryReferenceEventReader.cpp $
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
//  QueryReferenceEventReader.cpp
//  Implementation of the Class QueryReferenceEventReader
//  Created on:      29-Nov-2003 09:57:09 AM
///////////////////////////////////////////////////////////

#include "core/types/src/ta_types.h"
#include "QueryReferenceEventReader.h"
#include "RadioMethodQueryReference.h"
#include "RadioEvent.h"
#include "RadioProtocol.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include <iostream>

namespace TA_IRS_App
{
    const unsigned int QueryReferenceEventReader::CALL_REFERENCE_PARAMETER_POSITION        =   0;
    const unsigned int QueryReferenceEventReader::KEY_PARAMETER_POSITION                   =   4;
    const unsigned int QueryReferenceEventReader::DETAILS_LENGTH_PARAMETER_POSITION        =   8;
    const unsigned int QueryReferenceEventReader::DETAILS_STRING_PARAMETER_START_POSITION  =  12;

    QueryReferenceEventReader::QueryReferenceEventReader(RadioEvent& event) : 
    RadioEventReader(event,EVENT_QUERY_REFERENCE)
    {
    }



    QueryReferenceEventReader::~QueryReferenceEventReader()
    {
    }


    const ta_uint32 QueryReferenceEventReader::getCallReference()
    {
        return ENCODE_REFERENCE( getParameterLong(CALL_REFERENCE_PARAMETER_POSITION) ); // limin++ CALL REFERENCE
    }

    const ta_uint32 QueryReferenceEventReader::getQueryKey()
    {
        return getParameterLong(KEY_PARAMETER_POSITION);
    }

    const std::string QueryReferenceEventReader::getDetails()
    {
        // Get the length of the string
        ta_uint32 stringLength = getParameterLong(DETAILS_LENGTH_PARAMETER_POSITION);

        // Set up a string to place the characters in.
        std::string result("");

        for (unsigned int i = 0;i < stringLength;i++)
        {
			char nextChar = getParameterChar(DETAILS_STRING_PARAMETER_START_POSITION + i);
            if ((nextChar!='\n')&&(nextChar!='\0'))
			{
				result += nextChar;
			}
        }

        return result;
    }

	const std::vector<std::string> QueryReferenceEventReader::getDetailsList(char separator)
    {
        // Get the length of the string
        ta_uint32 stringLength = getParameterLong(DETAILS_LENGTH_PARAMETER_POSITION);

        // Set up a string to place the characters in.
		std::vector<std::string>  resultList;
        std::string               result("");

        for ( unsigned int i = 0 ; i<stringLength ; i++ )
        {
			char  nextChar = getParameterChar(DETAILS_STRING_PARAMETER_START_POSITION + i);
			if (nextChar=='\0') continue;
			if (nextChar==separator)
			{
				resultList.push_back(result);
				result = "";
			}
			else
			{
				result += nextChar;
			}
        }

		if (!result.empty())
		{
			resultList.push_back(result);
		}

        return resultList;
    }


int QueryReferenceEventReader::getDetailsKeyValueMap(QueryKeyValueMap & keyValues)
{
    const int delim1 = 0x0A; // '\n' isn't quite cross-platform
    const int delim2 = '=';

    ta_uint32 str_len = getParameterLong(DETAILS_LENGTH_PARAMETER_POSITION);
    std::string value_str;
    std::string key_str = "";
    bool is_value = false;

    for (unsigned int i=0 ; i<str_len-1 ; i++ )
    {
        char nextChar = getParameterChar(DETAILS_STRING_PARAMETER_START_POSITION+i);
        if (nextChar=='\0') continue; // ignore null characters
        if (nextChar==delim1)
        {
            TA_ASSERT( key_str.size()>0 , "Key string not defined for Query result" );
            TA_ASSERT( value_str.size()>0 , "Value string not defined for Query result" );

            ta_uint32 keyNum = lookupKeyStringValue(key_str);
            if (keyNum>0)
            {
                keyValues[keyNum] = value_str;
            }
            else
            {
                LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
                    "getDetailsKeyValueMap() - Warning: no number associated with key=%s", key_str.c_str());
                std::cerr << "Warning: no number associated with key=" << key_str << std::endl;
            }

            is_value = false;
            key_str = "";
            value_str = "";
        }
        else if (nextChar==delim2)
        {
            is_value = true;
        }
        else
        {
            if (is_value)
            {
                value_str += nextChar;
            }
            else // (is_key)
            {
                key_str += nextChar;
            }
        }

    }

    return keyValues.size();
}

ta_uint32 QueryReferenceEventReader::lookupKeyStringValue(const std::string & keyString)
{
    initialiseKeyStringValueMap();

    QueryKeyStringValueMap::iterator fnd = m_keyStringValueMap.find(keyString);

    if ( fnd != m_keyStringValueMap.end() )
    {
        return (*fnd).second;
    }
    
    return 0;
}


void QueryReferenceEventReader::initialiseKeyStringValueMap()
{
    // only initialise once. Ever (the map is static)
    if (!m_keyStringValueMap.empty())
    {
        return;
    }

    m_keyStringValueMap["K_CALL_TYPE"]              = RadioMethodQueryReference::K_CALL_TYPE;
    m_keyStringValueMap["K_FULL_DUPLEX"]            = RadioMethodQueryReference::K_FULL_DUPLEX;
    m_keyStringValueMap["K_COMM_TYPE"]              = RadioMethodQueryReference::K_COMM_TYPE;
    m_keyStringValueMap["K_SLOTS"]                  = RadioMethodQueryReference::K_SLOTS;
    m_keyStringValueMap["K_CALLING"]                = RadioMethodQueryReference::K_CALLING;
    m_keyStringValueMap["K_CALLED"]                 = RadioMethodQueryReference::K_CALLED;
    m_keyStringValueMap["K_AREA_SELECTION"]         = RadioMethodQueryReference::K_AREA_SELECTION;
    m_keyStringValueMap["K_AUDIO"]                  = RadioMethodQueryReference::K_AUDIO;
    m_keyStringValueMap["K_STATUS"]                 = RadioMethodQueryReference::K_STATUS;
    m_keyStringValueMap["K_REGION"]                 = RadioMethodQueryReference::K_REGION ;
    m_keyStringValueMap["K_ORGANIZATION"]           = RadioMethodQueryReference::K_ORGANISATION ;
    m_keyStringValueMap["K_DISCONNECT_REASON"]      = RadioMethodQueryReference::K_DISCONNECT_REASON;
    m_keyStringValueMap["K_CALL_ALL_OPTIONS"]       = RadioMethodQueryReference::K_CALL_ALL_OPTIONS;
    m_keyStringValueMap["K_CALLER_PHONE_NUMBER"]    = RadioMethodQueryReference::K_CALLER_PHONE_NUMBER;
    m_keyStringValueMap["K_CIRCUIT_DATA_MODE"]      = RadioMethodQueryReference::K_CIRCUIT_DATA_MODE;
    m_keyStringValueMap["K_PRIORITY"]               = RadioMethodQueryReference::K_PRIORITY;
    m_keyStringValueMap["K_CALLCONNECT_TIME"]       = RadioMethodQueryReference::K_CALLCONNECT_TIME;
#ifdef COPY_AND_PASTE_THIS
    m_keyStringValueMap["K_"]                       = RadioMethodQueryReference::K_ ;
#endif // #ifdef COPY_AND_PASTE_THIS
}

QueryReferenceEventReader::QueryKeyStringValueMap QueryReferenceEventReader::m_keyStringValueMap;

} // namespace TA_IRS_App
