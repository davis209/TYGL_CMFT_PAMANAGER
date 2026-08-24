/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodAttachToGroup.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Implementation of the Class RadioMethodAttachToGroup
  *
  */

#include "RadioMethodAttachToGroup.h"
#include "RadioProtocol.h"

using namespace TA_IRS_App;

#define ATTACHTOGROUP_METHOD_LENGTH  32
#define ATTACHTOGROUP_TSI_POSITION   0


/**
* Constructor
* @param sessionRef     The radio session reference
* @param gtsi           The tsi of the group to attach to
*/
RadioMethodAttachToGroup::RadioMethodAttachToGroup(RadioSessionReference sessionRef, const char * gtsi)
    :RadioMethod(ATTACHTOGROUP_METHOD_LENGTH, METHOD_ATTACHTOGROUP, sessionRef)
{
    // TSI SHORTSTRING position is 0
	setParameterShortString(ATTACHTOGROUP_TSI_POSITION,gtsi);
}


