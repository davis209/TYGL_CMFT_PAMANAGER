/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/DemandTXEventReader.h $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class encapsulates the Radio API Demand TX Event.
  *
  */

///////////////////////////////////////////////////////////
//  DemandTXEventReader.h
//  Implementation of the Class DemandTXEventReader
//  Created on:      29-Nov-2003 09:57:09 AM
///////////////////////////////////////////////////////////

#if !defined(DemandTXEventReader_INCLUDED_)
#define DemandTXEventReader_INCLUDED_

#include "core/types/src/ta_types.h"
#include "RadioEventReader.h"
#include <vector>

namespace TA_IRS_App
{
	class DemandTXEventReader : public RadioEventReader 
	{
		public:
			DemandTXEventReader(RadioEvent& event);
			virtual ~DemandTXEventReader();

			ta_uint32 getCallReference();
			std::string   getUserTSI();
			std::string   getTPI();

		private:

			DemandTXEventReader(const DemandTXEventReader& theReader);
            
            static const unsigned int CALL_REFERENCE_POSITION;
            static const unsigned int USER_TSI_POSITION;
            static const unsigned int TPI_POSITION;
            static const unsigned int PRIORITY_POSITION;
            static const unsigned int EXT_NUMBER_POSITION;
	};
};
#endif // !defined(DemandTXEventReader_INCLUDED_)
