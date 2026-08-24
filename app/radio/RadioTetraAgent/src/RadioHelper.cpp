/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/RadioHelper.cpp $
  * @author  R. van Hugten
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2008/11/28 16:26:01 $
  * Last modified by : $Author: builder $
  *
  * Defines a utility class for methods useful to several RadioTetraAgent classes
  *
  */
#include "core/types/src/ta_types.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "app/radio/RadioTetraAgent/src/RadioHelper.h"

#include "bus/radio/RadioAgentCorbaDef_4669/idl/src/IRadioCorbaDef.h"
#include "ace/OS.h"
#include <iostream>

namespace TA_IRS_App
{

    //
    // Utility function to get the string value from the map, checking for its existence
    char * RadioHelper::getCorbaStringValue(const QueryKeyValueMap & keyValueMap, ta_uint32 key)
    {
        FUNCTION_ENTRY("getCorbaStringValue()");
        char * the_str = 0;
        QueryKeyValueMap::const_iterator fnd = keyValueMap.find(key);

        //TA_ASSERT(fnd != keyValueMap.end(), "Key identifier not found");
        if (fnd != keyValueMap.end())
        {
            the_str = CORBA::string_dup( (*fnd).second.c_str() );
        }

        FUNCTION_EXIT;
        return the_str;
    }

    //
    // Utility function to get the string value from the map, checking for its existence
    std::string RadioHelper::getStdStringValue(const QueryKeyValueMap & keyValueMap, ta_uint32 key)
    {
        FUNCTION_ENTRY("getStdStringValue()");
        std::string the_str = "";
        QueryKeyValueMap::const_iterator fnd = keyValueMap.find(key);

        //TA_ASSERT(fnd != keyValueMap.end(), "Key identifier not found");
        if (fnd != keyValueMap.end())
        {
            the_str = (*fnd).second;
        }
        
        FUNCTION_EXIT;
        return (the_str);
    }

    bool RadioHelper::isOTETrue(const std::string & boolstr)
    {
        // We take a small shortcut: uppercase the first character, and
        // if it is one of T/Y/1, the value is true, otherwise false.
        char c1 = boolstr[0];
        if (c1>='a') c1-=0x20;
        if ((c1=='Y') || (c1=='T') || (c1=='1'))
        {
            return true;
        }

        // else
        return false;
    }


    //
    // Trying to ensure that strtok() and hence tokenise() is used not used in a
    // re-entrant situation, we use a non-reentrant thread lock which asserts if
    // tokenise is used incorrectly.  Because the strtok data that we are preventing
    // reentrant access to is static, we also make the lock so.
    static TA_Base_Core::NonReEntrantThreadLockable s_strtokLock;

    int RadioHelper::tokenise(const char * s, std::vector<std::string> & tokens, const char * delimiters, int maxtokens)
    {
        TA_Base_Core::ThreadGuard strtokGuard(s_strtokLock);

        if (maxtokens<2)
        {
            tokens.push_back(std::string(s));
            return tokens.size();
        }

        int numchars = strlen(s)+1;

        if (numchars > 1024)
        {
            std::cerr << "WARNING: truncating input to 1024 characters" << std::endl;
            numchars=1024;
        }

        char s_[1024];
        strncpy(s_,s,1024); // pads with zeros if numchars<1024
        s_[numchars] = 0;
        int numtokens=0;

        char * token = strtok(s_, delimiters);
        while (( token != 0 ) && (numtokens < maxtokens-1))
        {
            tokens.push_back(token);
            numtokens++;

            token = strtok(0, delimiters);
        }

        //
        // add the remainder of the string, if there is any left and we stopped
        // because we were about to exceed the maxtokens limit.
        if ((token!=0) && (numtokens >= maxtokens-1))
        {
            int tokenlen = strlen(token);
            // remove the string terminator and replace it with the first delimiter
            // BUT ONLY if the end of the string is not nigh anyway
            if ( (token + tokenlen) < (s_ + numchars -1) )
            {
                token[tokenlen] = delimiters[0];
            }

            tokens.push_back(token);
        }

        return tokens.size();
    }


    time_t RadioHelper::getCurrentTime()
    {
        time_t now;
        now = ACE_OS::time(0);

        struct tm ltime;
        ACE_OS::localtime_r(&now, &ltime);


        time_t localnow;
        localnow = ACE_OS::mktime(&ltime);

        return localnow;
    }

    std::string RadioHelper::getCurrentTimeString(time_t * ltime)
    {
        time_t lltime;

        if (0 == ltime)
        {
            lltime = getCurrentTime();
        }
        else
        {
            lltime = *ltime;
        }

        struct tm today;
        ACE_OS::localtime_r(&lltime, &today);

        char szBuffer[100];
        ACE_OS::strftime(szBuffer,sizeof(szBuffer),"%d-%b %H:%M:%S",&today);

        return std::string(szBuffer);
    }


    RadioHelper::RadioHelper()
    {
    }

    RadioHelper::~RadioHelper()
    {
    }
} // namespace TA_IRS_App
