/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RequestAuthoriseEventReader.h $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class encapsulates the Radio API Request Authorise Event.
  *
  */

///////////////////////////////////////////////////////////
//  RequestAuthoriseEventReader.h
//  Implementation of the Class RequestAuthoriseEventReader
//  Created on:      29-Nov-2003 09:57:09 AM
///////////////////////////////////////////////////////////

#if !defined(RequestAuthoriseEventReader_INCLUDED_)
#define RequestAuthoriseEventReader_INCLUDED_

#include "core/types/src/ta_types.h"
#include "RadioEventReader.h"
#include <vector>

namespace TA_IRS_App
{
	class RequestAuthoriseEventReader : public RadioEventReader 
	{
		public:
			RequestAuthoriseEventReader(RadioEvent& event);
			virtual ~RequestAuthoriseEventReader();

			ta_uint32 getCADReference();
			ta_uint32 getCADReason();

		private:

			RequestAuthoriseEventReader(const RequestAuthoriseEventReader& theReader);
            
            static const unsigned int CAD_REFERENCE_POSITION;
            static const unsigned int CAD_REASON_POSITION;
	};
};
#endif // !defined(RequestAuthoriseEventReader_INCLUDED_)
