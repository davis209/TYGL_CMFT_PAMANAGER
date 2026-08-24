/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodDetachSession.h $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class encapsulates the Radio API Detach Session method.
  *
  */

///////////////////////////////////////////////////////////
//  RadioMethodDetachSession.h
//  Implementation of the Class RadioMethodDetachSession
//  Created on:      29-Nov-2003 09:57:09 AM
///////////////////////////////////////////////////////////

#if !defined(RadioMethodDetachSession__INCLUDED_)
#define RadioMethodDetachSession__INCLUDED_

#include "RadioMethod.h"

namespace TA_IRS_App
{
	class RadioMethodDetachSession : public RadioMethod 
	{
		public:
			RadioMethodDetachSession(RadioSessionReference sessionRef);
			virtual ~RadioMethodDetachSession();

		public:

		private:
			RadioMethodDetachSession(const RadioMethodDetachSession& theMethod);
	};
};
#endif // !defined(RadioMethodDetachSession__INCLUDED_)
