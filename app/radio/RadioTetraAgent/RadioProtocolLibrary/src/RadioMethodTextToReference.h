/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodTextToReference.h $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class encapsulates the Radio API Text to Reference method.
  *
  */

///////////////////////////////////////////////////////////
//  RadioMethodTextToReference.h
//  Implementation of the Class RadioMethodTextToReference
//  Created on:      29-Nov-2003 09:57:09 AM
///////////////////////////////////////////////////////////

#if !defined(RadioMethodTextToReference__INCLUDED_)
#define RadioMethodTextToReference__INCLUDED_

#include "RadioMethod.h"
#include <vector>

namespace TA_IRS_App
{
	class RadioMethodTextToReference : public RadioMethod
	{
		public:
			RadioMethodTextToReference(RadioSessionReference sessionRef, std::string stringReference);
			virtual ~RadioMethodTextToReference();

		public:

		private:
			RadioMethodTextToReference(const RadioMethodTextToReference& theMethod);
	};
};
#endif // !defined(RadioMethodTextToReference__INCLUDED_)
