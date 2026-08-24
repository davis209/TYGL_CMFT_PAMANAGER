/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodAttachMonitorAudio.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Created on:      07-Nov-2003 11:04:01
  *
  * Modification history:
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * RadioMethodAttachMonitorAudio.h
  *
  */

#if !defined(RadioMethodAttachMonitorAudio_H)
#define RadioMethodAttachMonitorAudio_H

#include "core/types/src/ta_types.h"
#include "RadioMethodCallReference.h"


namespace TA_IRS_App
{
    const ta_uint32 MONITOR_AUDIO_NOT_ACTIVE = 0;

	/**
	* Monitor the given subscriber.
	*/
	class RadioMethodAttachMonitorAudio : public RadioMethodCallReference
	{
		public:

            /**
             * Constructor
             *
             * @param sessionRef    the usual session reference
             * @param voiceCallRef  a Voice CallReference
             * @param audioRefA     a valid AudioReference
             * @param audioRefB     if the Voice call is fullduplex, this should
             *                      be a valid AudioReference, otherwise
             *                      MONITOR_AUDIO_NOT_ACTIVE (=0)
             */
			RadioMethodAttachMonitorAudio(  RadioSessionReference sessionRef,
                                            ta_uint32 voiceCallRef,
                                            ta_uint32 audioRefA,
                                            ta_uint32 audioRefB);

		public:
	};
};

#endif // !defined(RadioMethodAttachMonitorAudio_H)
