/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/QueryReferenceEventReader.h $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class encapsulates the Radio API Change Reference method.
  *
  */

///////////////////////////////////////////////////////////
//  QueryReferenceEventReader.h
//  Implementation of the Class QueryReferenceEventReader
//  Created on:      29-Nov-2003 09:57:09 AM
///////////////////////////////////////////////////////////

#if !defined(QueryReferenceEventReader_B874D102_C2B6_4e5a_A4DC_D9D9081591EF__INCLUDED_)
#define QueryReferenceEventReader_B874D102_C2B6_4e5a_A4DC_D9D9081591EF__INCLUDED_

#include "core/types/src/ta_types.h"
#include "RadioEventReader.h"

#include <vector>
#include <string>
#include <map>

namespace TA_IRS_App
{
    typedef std::map< ta_uint32 , std::string > QueryKeyValueMap;

    /**
     * QueryReferenceEventReader is a RadioEventReader that provides access
     * to the parameters returned in a QueryReferenceEvent
     */
	class QueryReferenceEventReader : public RadioEventReader 
	{
		public:
			QueryReferenceEventReader(RadioEvent& event);
			virtual ~QueryReferenceEventReader();

            const ta_uint32 getCallReference();
            const ta_uint32 getQueryKey();

            const std::string getDetails();
			const std::vector<std::string> getDetailsList(char separator);
            int getDetailsKeyValueMap(QueryKeyValueMap & keyValues);

		private:
			QueryReferenceEventReader(const QueryReferenceEventReader& theQueryReferenceEventReader);
            
            static const unsigned int CALL_REFERENCE_PARAMETER_POSITION;
            static const unsigned int KEY_PARAMETER_POSITION;
            static const unsigned int DETAILS_LENGTH_PARAMETER_POSITION;
            static const unsigned int DETAILS_STRING_PARAMETER_START_POSITION;

            ///
            /// Type definition for a map of "K_..." query key name strings with
            /// their corresponding values.
            typedef std::map< std::string, ta_uint32 > QueryKeyStringValueMap;

            ///
            /// Map of query key values key'd by name string
            static QueryKeyStringValueMap m_keyStringValueMap;

            /** Utility function to lookup the values in the m_keyStringValueMap
             *
             * @param  the string of the key to lookup
             * @return the key value corresponding to the keyString if it exists
             *         in the map, 0 (unused value) otherwise
             */
            ta_uint32 lookupKeyStringValue(const std::string & keyString);

            /** Run once the first time lookupKeyStringValue is executed,
             * initialises the static m_keyStringValueMap.
             */
            void initialiseKeyStringValueMap();
	};
};
#endif // !defined(QueryReferenceEventReader_B874D102_C2B6_4e5a_A4DC_D9D9081591EF__INCLUDED_)
