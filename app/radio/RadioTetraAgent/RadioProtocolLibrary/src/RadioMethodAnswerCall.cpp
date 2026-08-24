/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodAnswerCall.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class encapsulates the Radio API Answer Call method.
  *
  */
///////////////////////////////////////////////////////////
//  RadioMethodAnswerCall.cpp
//  Implementation of the Class RadioMethodAnswerCall
//  Created on:      29-Nov-2003 09:57:09 AM
///////////////////////////////////////////////////////////

#include "RadioMethodAnswerCall.h"
#include "RadioProtocol.h"
#include "core/utilities/src/TAAssert.h"

using namespace TA_IRS_App;

#define ANSWER_CALL_LENGTH                     4


RadioMethodAnswerCall::RadioMethodAnswerCall(RadioSessionReference sessionRef, RadioCallReference callRef) 
                   : RadioMethodCallReference(ANSWER_CALL_LENGTH,METHOD_ANSWER_CALL,sessionRef,callRef)
{
}



RadioMethodAnswerCall::~RadioMethodAnswerCall()
{
}


