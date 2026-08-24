/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodAttachAudio.h $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class encapsulates the Radio API Attach Audio method.
  *
  */

///////////////////////////////////////////////////////////
//  RadioMethodAttachAudio.h
//  Implementation of the Class RadioMethodAttachAudio
//  Created on:      29-Nov-2003 09:57:09 AM
///////////////////////////////////////////////////////////

#if !defined(RadioMethodAttachAudio__INCLUDED_)
#define RadioMethodAttachAudio__INCLUDED_

#include "core/types/src/ta_types.h"
#include "RadioMethodCallReference.h"
#include <vector>

namespace TA_IRS_App
{
	class RadioMethodAttachAudio : public RadioMethodCallReference 
	{
		public:
			RadioMethodAttachAudio(RadioSessionReference sessionRef, ta_uint32 callRef, ta_uint32 audioRef);
			virtual ~RadioMethodAttachAudio();

		public:

		private:
			RadioMethodAttachAudio(const RadioMethodAttachAudio& theMethod);
	};
};
#endif // !defined(RadioMethodAttachAudio__INCLUDED_)
