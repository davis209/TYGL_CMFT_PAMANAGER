/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodDeselect.h $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class encapsulates the Radio API Deselect method.
  *
  */

///////////////////////////////////////////////////////////
//  RadioMethodDeselect.h
//  Implementation of the Class RadioMethodDeselect
//  Created on:      29-Nov-2003 09:57:09 AM
///////////////////////////////////////////////////////////

#if !defined(RadioMethodDeselect__INCLUDED_)
#define RadioMethodDeselect__INCLUDED_

#include "RadioMethodCallReference.h"
#include <vector>

namespace TA_IRS_App
{
	class RadioMethodDeselect : public RadioMethodCallReference 
	{
		public:
			RadioMethodDeselect(RadioSessionReference sessionRef, RadioCallReference callRef);
			virtual ~RadioMethodDeselect();

		public:

		private:
			RadioMethodDeselect(const RadioMethodDeselect& theRadioMethodSelect);
	};
};
#endif // !defined(RadioMethodDeselect__INCLUDED_)
