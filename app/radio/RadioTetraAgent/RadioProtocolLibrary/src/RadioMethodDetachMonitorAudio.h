/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodDetachMonitorAudio.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#if !defined(RadioMethodDetachMonitorAudio_H)
#define RadioMethodDetachMonitorAudio_H

#include "core/types/src/ta_types.h"
#include "RadioMethodCallReference.h"


namespace TA_IRS_App
{
	/**
	* Monitor the given subscriber.
	*/
	class RadioMethodDetachMonitorAudio : public RadioMethodCallReference
	{
		public:

            /**
             * Constructor
             *
             * @param sessionRef    the usual session reference
             * @param monCallRef    a MonitorCallReference
             */
			RadioMethodDetachMonitorAudio(RadioSessionReference sessionRef, ta_uint32 monCallRef);
	};
};

#endif // !defined(RadioMethodDetachMonitorAudio_H)
