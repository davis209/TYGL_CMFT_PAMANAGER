/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodAnswerCall.h $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class encapsulates the Radio API Answer Call method.
  *
  */

///////////////////////////////////////////////////////////
//  RadioMethodAnswerCall.h
//  Implementation of the Class RadioMethodAnswerCall
//  Created on:      29-Nov-2003 09:57:09 AM
///////////////////////////////////////////////////////////

#if !defined(RadioMethodAnswerCall__INCLUDED_)
#define RadioMethodAnswerCall__INCLUDED_

#include "RadioMethodCallReference.h"

namespace TA_IRS_App
{
	class RadioMethodAnswerCall : public RadioMethodCallReference 
	{
		public:
			RadioMethodAnswerCall(RadioSessionReference sessionRef, RadioCallReference callRef);
			virtual ~RadioMethodAnswerCall();

		public:

		private:
			RadioMethodAnswerCall(const RadioMethodAnswerCall& theMethod);
	};
};
#endif // !defined(RadioMethodAnswerCall__INCLUDED_)
