/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodQueryReference.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class encapsulates the Radio API SendSDS method.
  *
  */
///////////////////////////////////////////////////////////
//  RadioMethodQueryReference.cpp
//  Implementation of the Class RadioMethodQueryReference
//  Created on:      29-Nov-2003 09:57:09 AM
///////////////////////////////////////////////////////////

#include "core/types/src/ta_types.h"
#include "RadioMethodQueryReference.h"
#include "RadioProtocol.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App
{
    const unsigned int RadioMethodQueryReference::QUERY_REFERENCE_LENGTH           = 8;
    const unsigned int RadioMethodQueryReference::REFERENCE_PARAMETER_POSITION     = 0;
    const unsigned int RadioMethodQueryReference::KEY_PARAMETER_POSITION           = 4;

    RadioMethodQueryReference::RadioMethodQueryReference(RadioSessionReference sessionRef) 
                       : RadioMethod(QUERY_REFERENCE_LENGTH,METHOD_QUERY_REFERENCE,sessionRef)
    {
    }



    RadioMethodQueryReference::~RadioMethodQueryReference()
    {
    }


    void RadioMethodQueryReference::setQueryType(const EKeys key)
    {
		FUNCTION_ENTRY("setQueryType");
        // Determine if forwarding has been enabled or disabled
        setParameterLong(KEY_PARAMETER_POSITION,key);        
		FUNCTION_EXIT;
    }
    
    void RadioMethodQueryReference::setQueryReference(ta_uint32 reference)
    {
		FUNCTION_ENTRY("setQueryReference");
        setParameterLong(REFERENCE_PARAMETER_POSITION,reference);
		FUNCTION_EXIT;
    }
}

