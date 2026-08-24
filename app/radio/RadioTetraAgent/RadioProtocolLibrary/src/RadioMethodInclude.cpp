/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodInclude.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class encapsulates the Radio API Include method.
  *
  */
///////////////////////////////////////////////////////////
//  RadioMethodInclude.cpp
//  Implementation of the Class RadioMethodInclude
//  Created on:      29-Nov-2003 09:57:09 AM
///////////////////////////////////////////////////////////

#include "core/types/src/ta_types.h"
#include "RadioMethodInclude.h"
#include "RadioProtocol.h"
#include "core/utilities/src/TAAssert.h"

using namespace TA_IRS_App;

#define INCLUDE_METHOD_LENGTH             36
#define DESTINATION_ITSI_START_POSITION    4


RadioMethodInclude::RadioMethodInclude(RadioSessionReference sessionRef, ta_uint32 callRef, std::string destITSI) 
                   : RadioMethodCallReference(INCLUDE_METHOD_LENGTH,METHOD_INCLUDE,sessionRef,callRef)
{
	setParameterShortString(DESTINATION_ITSI_START_POSITION,destITSI);
}



RadioMethodInclude::~RadioMethodInclude()
{
}


