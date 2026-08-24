/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodAttachAudio.cpp $
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
//  RadioMethodAttachAudio.cpp
//  Implementation of the Class RadioMethodAttachAudio
//  Created on:      29-Nov-2003 09:57:09 AM
///////////////////////////////////////////////////////////

#include "core/types/src/ta_types.h"
#include "RadioMethodAttachAudio.h"
#include "RadioProtocol.h"
#include "core/utilities/src/TAAssert.h"

using namespace TA_IRS_App;

#define ATTACH_AUDIO_LENGTH               8
#define AUDIO_REFERENCE_START_POSITION    4


RadioMethodAttachAudio::RadioMethodAttachAudio(RadioSessionReference sessionRef, ta_uint32 callRef,   ta_uint32 audioRef) 
                   : RadioMethodCallReference(ATTACH_AUDIO_LENGTH,METHOD_ATTACH_AUDIO,sessionRef,callRef)
{
	setParameterLong(AUDIO_REFERENCE_START_POSITION,audioRef);
}



RadioMethodAttachAudio::~RadioMethodAttachAudio()
{
}


