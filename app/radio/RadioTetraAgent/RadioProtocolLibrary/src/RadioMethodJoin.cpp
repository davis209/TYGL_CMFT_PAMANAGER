/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodJoin.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Implementation of the Class RadioMethodJoin
  *
  */

#include "core/types/src/ta_types.h"
#include "RadioMethodJoin.h"
#include "RadioProtocol.h"

using namespace TA_IRS_App;

#define JOIN_METHOD_LENGTH        4

/**
* Constructor
* @param    sessionRef
*/
RadioMethodJoin::RadioMethodJoin(RadioSessionReference sessionRef, ta_uint32 callRef)
    :RadioMethodCallReference(JOIN_METHOD_LENGTH, METHOD_JOIN, sessionRef, callRef)
{
}

