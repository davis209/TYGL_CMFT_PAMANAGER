/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodTextToReference.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class encapsulates the Radio API Text To Reference method.
  *
  */
///////////////////////////////////////////////////////////
//  RadioMethodTextToReference.cpp
//  Implementation of the Class RadioMethodTextToReference
//  Created on:      29-Nov-2003 09:57:09 AM
///////////////////////////////////////////////////////////

#include "RadioMethodTextToReference.h"
#include "RadioProtocol.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using namespace TA_IRS_App;

#define TEXT_TO_REFERENCE_LENGTH       32
#define STRING_REFERENCE_START          0


RadioMethodTextToReference::RadioMethodTextToReference(RadioSessionReference sessionRef, std::string textReference) 
                   : RadioMethod(TEXT_TO_REFERENCE_LENGTH,METHOD_TEXT_TO_REFERENCE,sessionRef)
{
	FUNCTION_ENTRY("Constructor");
	setParameterShortString(STRING_REFERENCE_START,textReference);
	FUNCTION_EXIT;
}



RadioMethodTextToReference::~RadioMethodTextToReference()
{
	FUNCTION_ENTRY("Destructor");
	FUNCTION_EXIT;
}

