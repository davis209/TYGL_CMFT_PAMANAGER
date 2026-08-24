/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodAuthoriseCall.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class encapsulates the Radio Radio Authorise Call method.
  *
  */
///////////////////////////////////////////////////////////
//  RadioMethodAuthoriseCall.cpp
//  Implementation of the Class RadioMethodAuthoriseCall
//  Created on:      29-Nov-2003 09:57:09 AM
///////////////////////////////////////////////////////////

#include "RadioMethodAuthoriseCall.h"
#include "RadioProtocol.h"
#include "core/utilities/src/TAAssert.h"

using namespace TA_IRS_App;

#define AUTHORISE_CALL_PARAMETER_LENGTH      12
#define AUTHORISE_CALL_MODE_START            4
#define AUTHORISE_CALL_REASON_START          8

#define CAD_AUTHORISE    1
#define CAD_REJECT       0
#define CAD_REASON_NOT_DEFINED   0


RadioMethodAuthoriseCall::RadioMethodAuthoriseCall(RadioSessionReference sessionRef, RadioCallReference callRef,
												   bool authorise) 
                   : RadioMethodCallReference(AUTHORISE_CALL_PARAMETER_LENGTH,METHOD_AUTHORISE_CALL,sessionRef,callRef)
{
	if (authorise)
	{
		setParameterLong(AUTHORISE_CALL_MODE_START,CAD_AUTHORISE);
	}
	else
	{
		setParameterLong(AUTHORISE_CALL_MODE_START,CAD_REJECT);
	}

	setParameterLong(AUTHORISE_CALL_REASON_START,CAD_REASON_NOT_DEFINED);

    setParameterLong( 0, callRef );//limin++ CALL REFERENCE
}



RadioMethodAuthoriseCall::~RadioMethodAuthoriseCall()
{
}


