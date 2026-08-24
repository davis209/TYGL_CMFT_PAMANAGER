/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodCeaseTX.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class encapsulates the Radio API Cease TX method.
  *
  */
///////////////////////////////////////////////////////////
//  RadioMethodCeaseTX.cpp
//  Implementation of the Class RadioMethodCeaseTX
//  Created on:      29-Nov-2003 09:57:09 AM
///////////////////////////////////////////////////////////

#include "RadioMethodCeaseTX.h"
#include "RadioProtocol.h"
#include "core/utilities/src/TAAssert.h"

using namespace TA_IRS_App;

#define CEASE_TX_LENGTH           0


RadioMethodCeaseTX::RadioMethodCeaseTX(RadioSessionReference sessionRef) 
                   : RadioMethod(CEASE_TX_LENGTH,METHOD_CEASE_TX,sessionRef)
{
}



RadioMethodCeaseTX::~RadioMethodCeaseTX()
{
}

void RadioMethodCeaseTX::addCallReference(CallReference callRef, bool setMessageImmediate)
{

	if (callRef > 100) // Limin's encoding
	{
		callRef /= 100;
	}

    if ( m_callRefs.end() == m_callRefs.find(callRef) )
    {
        m_callRefs.insert(callRef);

        if (setMessageImmediate)
        {
            setMessage();
        }
    }
}

void RadioMethodCeaseTX::addCallReferences(const std::vector<CallReference> & callRefs)
{
    for (std::vector<CallReference>::const_iterator iter = callRefs.begin() ;
            callRefs.end() != iter; iter++)
    {
        addCallReference( (*iter), false);
    }

    setMessage();
}

void RadioMethodCeaseTX::setMessage()
{
    setMessageSize(CEASE_TX_LENGTH + 4 + (4*m_callRefs.size()) );

    // first, the length dword
    setParameterLong(0, m_callRefs.size() );

    // now each dword
    int i=0;
    for ( CallReferenceSet::iterator iter = m_callRefs.begin() ;
            (iter != m_callRefs.end());
            (iter++),(i++) )
    {
        setParameterLong(4+(4*i), (*iter) );
    }
}

