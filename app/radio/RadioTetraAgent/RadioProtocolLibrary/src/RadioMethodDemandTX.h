/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodDemandTX.h $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class encapsulates the Radio API Demand TX method.
  *
  */

///////////////////////////////////////////////////////////
//  RadioMethodDemandTX.h
//  Implementation of the Class RadioMethodDemandTX
//  Created on:      29-Nov-2003 09:57:09 AM
///////////////////////////////////////////////////////////

#if !defined(RadioMethodDemandTX__INCLUDED_)
#define RadioMethodDemandTX__INCLUDED_

#include "core/types/src/ta_types.h"
#include "RadioMethod.h"
#include <vector>

namespace TA_IRS_App
{
	class RadioMethodDemandTX : public RadioMethod 
	{
		public:
			RadioMethodDemandTX(RadioSessionReference sessionRef, ta_uint32 priority);
			virtual ~RadioMethodDemandTX();

		public:

		private:
			RadioMethodDemandTX(const RadioMethodDemandTX& theMethod);
	};
};
#endif // !defined(RadioMethodDemandTX__INCLUDED_)
