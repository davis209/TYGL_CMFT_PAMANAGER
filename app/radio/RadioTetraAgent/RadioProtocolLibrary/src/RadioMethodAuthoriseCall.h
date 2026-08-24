/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodAuthoriseCall.h $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class encapsulates the Radio API Authorise Call method.
  *
  */

///////////////////////////////////////////////////////////
//  RadioMethodAuthoriseCall.h
//  Implementation of the Class RadioMethodAuthoriseCall
//  Created on:      29-Nov-2003 09:57:09 AM
///////////////////////////////////////////////////////////

#if !defined(RadioMethodAuthoriseCall__INCLUDED_)
#define RadioMethodAuthoriseCall__INCLUDED_

#include "RadioMethodCallReference.h"
#include <vector>

namespace TA_IRS_App
{
	class RadioMethodAuthoriseCall : public RadioMethodCallReference 
	{
		public:
			RadioMethodAuthoriseCall(RadioSessionReference sessionRef, RadioCallReference callRef,
				                      bool authorise);
			virtual ~RadioMethodAuthoriseCall();

		public:

		private:
			RadioMethodAuthoriseCall(const RadioMethodAuthoriseCall& theRadioMethod);
	};
};
#endif // !defined(RadioMethodAuthoriseCall__INCLUDED_)
