#ifndef RADIO_FALLBACK_EXCEPTION_H
#define RADIO_FALLBACK_EXCEPTION_H
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/RadioFallbackException.h $
  * @author  R. van Hugten
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2008/11/28 16:26:01 $
  * Last modified by : $Author: builder $
  *
  * Declaration of the RadioFallbackException class
  *
  */

#include "app/radio/RadioTetraAgent/src/RadioException.h"

namespace TA_IRS_App
{
	class RadioFallbackException : public RadioException
	{
		public:
			RadioFallbackException() throw();
			virtual ~RadioFallbackException() throw();
	};
};

#endif // #ifndef RADIO_FALLBACK_EXCEPTION_H
