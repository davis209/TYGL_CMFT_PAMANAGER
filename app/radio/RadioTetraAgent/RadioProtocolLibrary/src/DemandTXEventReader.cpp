/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/DemandTXEventReader.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class encapsulates the Radio API DemandTX event.
  *
  */
///////////////////////////////////////////////////////////
//  DemandTXEventReader.cpp
//  Implementation of the Class DemandTXEventReader
//  Created on:      29-Nov-2003 09:57:09 AM
///////////////////////////////////////////////////////////

#include "core/types/src/ta_types.h"
#include "DemandTXEventReader.h"
#include "RadioEvent.h"
#include "RadioProtocol.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_App
{
    const unsigned int DemandTXEventReader::CALL_REFERENCE_POSITION = 0;
    const unsigned int DemandTXEventReader::USER_TSI_POSITION       = 4;
    const unsigned int DemandTXEventReader::TPI_POSITION            = 36;
    const unsigned int DemandTXEventReader::PRIORITY_POSITION       = 68;
    const unsigned int DemandTXEventReader::EXT_NUMBER_POSITION     = 72;

    DemandTXEventReader::DemandTXEventReader(RadioEvent& event) : 
    RadioEventReader(event,EVENT_DEMAND_TX)
    {
    }



    DemandTXEventReader::~DemandTXEventReader()
    {
    }


    ta_uint32 DemandTXEventReader::getCallReference()
    {
        return ENCODE_REFERENCE( getParameterLong(CALL_REFERENCE_POSITION) ); // limin++ CALL REFERENCE
    }


	std::string DemandTXEventReader::getUserTSI()
    {
        return getParameterShortString(USER_TSI_POSITION);
    }

    std::string DemandTXEventReader::getTPI()
    {
        return getParameterShortString(TPI_POSITION);
    }

}

