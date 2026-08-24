/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodInclude.h $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class encapsulates the Radio API Include method.
  *
  */

///////////////////////////////////////////////////////////
//  RadioMethodInclude.h
//  Implementation of the Class RadioMethodInclude
//  Created on:      29-Nov-2003 09:57:09 AM
///////////////////////////////////////////////////////////

#if !defined(RadioMethodInclude__INCLUDED_)
#define RadioMethodInclude__INCLUDED_

#include "core/types/src/ta_types.h"
#include "RadioMethodCallReference.h"
#include <vector>

namespace TA_IRS_App
{
	class RadioMethodInclude : public RadioMethodCallReference
	{
		public:
			RadioMethodInclude(RadioSessionReference sessionRef, ta_uint32 callRef, std::string DestinationITSI);
			virtual ~RadioMethodInclude();

		public:

		private:
			RadioMethodInclude(const RadioMethodInclude& theMethod);
	};
};
#endif // !defined(RadioMethodInclude__INCLUDED_)
