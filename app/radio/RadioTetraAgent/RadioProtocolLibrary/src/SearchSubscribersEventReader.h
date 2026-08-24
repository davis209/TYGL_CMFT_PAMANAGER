/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/SearchSubscribersEventReader.h $
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
//  SearchSubscribersEventReader.h
//  Implementation of the Class SearchSubscribersEventReader
//  Created on:      29-Nov-2003 09:57:09 AM
///////////////////////////////////////////////////////////

#if !defined(SearchSubscribersEventReader_B874D102_C2B6_4e5a_A4DC_D9D9081591EF__INCLUDED_)
#define SearchSubscribersEventReader_B874D102_C2B6_4e5a_A4DC_D9D9081591EF__INCLUDED_

#include "core/types/src/ta_types.h"
#include "RadioEventReader.h"
#include <vector>

namespace TA_IRS_App
{
	class SearchSubscribersEventReader : public RadioEventReader 
	{
		public:
			SearchSubscribersEventReader(RadioEvent& event);
			virtual ~SearchSubscribersEventReader();

            const ta_uint32 getSubscriberReferenceResultSet();

		private:
			SearchSubscribersEventReader(const SearchSubscribersEventReader& theSearchSubscribersEventReader);
            
            static const unsigned int SUBSCRIBER_REFERENCE_RESULT_PARAMETER_POSITION;
	};
};
#endif // !defined(SearchSubscribersEventReader_B874D102_C2B6_4e5a_A4DC_D9D9081591EF__INCLUDED_)
