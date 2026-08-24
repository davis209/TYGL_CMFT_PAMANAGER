/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/DetachMonitorAudioEventReader.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Implementation of DetachMonitorAudio Event Reader
  *
  */
#include "core/types/src/ta_types.h"
#include "DetachMonitorAudioEventReader.h"

namespace TA_IRS_App
{

    DetachMonitorAudioEventReader::DetachMonitorAudioEventReader(RadioEvent& event)
           : RadioEventReader(event,EVENT_DETACHMONITORAUDIO,DETACHMONITORAUDIO_EVENT_LENGTH)
    {
    }

    DetachMonitorAudioEventReader::~DetachMonitorAudioEventReader()
    {
    }

    ta_uint32 DetachMonitorAudioEventReader::getCallReference() const
    {
        return ENCODE_REFERENCE( getParameterLong(CALL_REFERENCE_START) ); // limin++ CALL REFERENCE
    }

    ta_uint32 DetachMonitorAudioEventReader::getAudioReferenceA() const
    {
        return getParameterLong(AUDIO_REFERENCE_A_START);
    }

    ta_uint32 DetachMonitorAudioEventReader::getAudioReferenceB() const
    {
        return getParameterLong(AUDIO_REFERENCE_B_START);
    }

    const unsigned int DetachMonitorAudioEventReader::DETACHMONITORAUDIO_EVENT_LENGTH   = 12;
    const unsigned int DetachMonitorAudioEventReader::CALL_REFERENCE_START              = 0;
    const unsigned int DetachMonitorAudioEventReader::AUDIO_REFERENCE_A_START           = 4;
    const unsigned int DetachMonitorAudioEventReader::AUDIO_REFERENCE_B_START           = 8;
} // namespace TA_IRS_App


