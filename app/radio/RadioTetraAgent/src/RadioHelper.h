#ifndef RADIO_HELPER_H
#define RADIO_HELPER_H
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/RadioHelper.h $
  * @author  R. van Hugten
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2008/11/28 16:26:01 $
  * Last modified by : $Author: builder $
  *
  */
#include "core/types/src/ta_types.h"
#include <string>


#include "app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioProtocolLibrary.h"

namespace TA_IRS_App
{
    class Radio;

    /**
     * Utility class containing static functions with no session- or radio-
     * specific functionality
     */
    class RadioHelper
    {
        public:
            /**
             * Utility function to get the string value from the map, checking
             * for its existence and returning a string_dup'd corba string if
             * it does.
             *
             */
            static char * getCorbaStringValue
                (const QueryKeyValueMap & keyValueMap,
                ta_uint32 key);

            /**
             * Utility function to get the string value from the map, checking
             * for its existence and returning a std::string if it does.
             *
             * @param   keyValueMap the QueryKeyValueMap as populated by queryAllCallDetails,
             *          or other QueryReference with K_KEY_ALL_* key
             * @param   the K_* key
             * @return  the std::string value of the specified key
             */
            static std::string getStdStringValue
                (const QueryKeyValueMap & keyValueMap,
                ta_uint32 key);


            /**
             * Utility function to ascertain the truth-fulness of given string,
             * according to OTE-defined truth values
             *
             * For Boolean DataTypes, true can be any of
             * "Y","y","YES","yes","1","TRUE","true","T","t"
             *
             * Similarly, false can be any of
             * "N","n","NO","no","0","FALSE","false","F","f"
             *
             * @param   boolstr the string to test truth of
             * @return  true if the string represents truth, false otherwise
             */
            static bool isOTETrue(const std::string & boolstr);


            /**
              * Breaks the given string into tokens according to the delimiters.
              * Uses strtok().
              *
              * Ripped straight from Base bus/generic_server_simulator/src/GssUtility
              * rather than link against the extra library on Solaris
              *
              * @note applies an arbitrary limit of 1024 characters to the input
              *       string (which matches the CommandMonitor line length limit)
              * 
              * @param s the string to tokenise
              * @param tokens the resulting list of strings
              * @param delimiters the delimiters given to strtok
              * @param maxtokens the maximum number of tokens into which to break
              *         the string. When the limit is one token from being reached,
              *         the untokenised remainder of the string is placed into the
              *         final token. Defaults to 0xFFFF, which given the input line
              *         limit essentially means no limit to the tokens.
              * @return the number of tokens
              */
            static int tokenise(const char * s, std::vector<std::string> & tokens, const char * delimiters=" ", int maxtokens=0xFFFF);

            /**
             *
             */
            static time_t getCurrentTime();


            /**
             * returns a string of the current time, as strftime(... ,"%d-%b %H:%M:%S", ...);
             */
            static std::string getCurrentTimeString(time_t * ltime);



        private:
            /**
             * Constructor "hidden", since all methods are static this class is
             * not intended for instantiation
             */
            RadioHelper();

            /**
             * Destructor "hidden", since all methods are static this class is
             * not intended for instantiation or destruction
             */
            ~RadioHelper();


    }; // class RadioHelper
} // namespace TA_IRS_App

#endif // #ifndef RADIO_HELPER_H
