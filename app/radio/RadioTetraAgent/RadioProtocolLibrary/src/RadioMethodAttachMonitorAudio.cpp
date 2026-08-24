/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodAttachMonitorAudio.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Implementation of the Class RadioMethodAttachMonitorAudio
  *
  */

#include "core/types/src/ta_types.h"
#include "RadioMethodAttachMonitorAudio.h"
#include "RadioProtocol.h"

using namespace TA_IRS_App;

#define ATTACHMONITORAUDIO_METHOD_LENGTH        12
// #define ATTACHMONITORAUDIO_VCALLREF_POSITION    0
#define ATTACHMONITORAUDIO_AUDIOREF_A_POSITION  4
#define ATTACHMONITORAUDIO_AUDIOREF_B_POSITION  8



/**
* Constructor
* @param    sessionRef
*/
RadioMethodAttachMonitorAudio::RadioMethodAttachMonitorAudio
(   RadioSessionReference sessionRef,
    ta_uint32 voiceCallRef,
    ta_uint32 audioRefA,
    ta_uint32 audioRefB)
    :RadioMethodCallReference(ATTACHMONITORAUDIO_METHOD_LENGTH, METHOD_ATTACHMONITORAUDIO, sessionRef, voiceCallRef)
{
    //
    setParameterLong(ATTACHMONITORAUDIO_AUDIOREF_A_POSITION, audioRefA);
    //
    setParameterLong(ATTACHMONITORAUDIO_AUDIOREF_B_POSITION, audioRefB);
}

