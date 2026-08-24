/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/CeaseTXEventReader.h $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class encapsulates the Radio API Cease TX Event.
  *
  */

///////////////////////////////////////////////////////////
//  CeaseTXEventReader.h
//  Implementation of the Class CeaseTXEventReader
//  Created on:      29-Nov-2003 09:57:09 AM
///////////////////////////////////////////////////////////

#if !defined(CeaseTXEventReader_INCLUDED_)
#define CeaseTXEventReader_INCLUDED_

#include "core/types/src/ta_types.h"
#include "RadioEventReader.h"
#include <vector>

namespace TA_IRS_App
{
	class CeaseTXEventReader : public RadioEventReader 
	{
		public:
			CeaseTXEventReader(RadioEvent& event);
			virtual ~CeaseTXEventReader();

			ta_uint32 getCallReference();

		private:

			CeaseTXEventReader(const CeaseTXEventReader& theReader);
            
            static const unsigned int CALL_REFERENCE_POSITION;
	};
};
#endif // !defined(CeaseTXEventReader_INCLUDED_)
