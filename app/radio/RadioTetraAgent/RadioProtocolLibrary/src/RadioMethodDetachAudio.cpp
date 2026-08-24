/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodDetachAudio.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class encapsulates the Radio API Detach Audio method.
  *
  */
///////////////////////////////////////////////////////////
//  RadioMethodDetachAudio.cpp
//  Implementation of the Class RadioMethodDetachAudio
//  Created on:      29-Nov-2003 09:57:09 AM
///////////////////////////////////////////////////////////

#include "core/types/src/ta_types.h"
#include "RadioMethodDetachAudio.h"
#include "RadioProtocol.h"
#include "core/utilities/src/TAAssert.h"

using namespace TA_IRS_App;

#define DETACH_AUDIO_LENGTH              4


RadioMethodDetachAudio::RadioMethodDetachAudio(RadioSessionReference sessionRef, ta_uint32 callRef, ta_uint32 audioRef) 
                   : RadioMethodCallReference(DETACH_AUDIO_LENGTH,METHOD_DETACH_AUDIO,sessionRef,callRef)
{
}



RadioMethodDetachAudio::~RadioMethodDetachAudio()
{
}


