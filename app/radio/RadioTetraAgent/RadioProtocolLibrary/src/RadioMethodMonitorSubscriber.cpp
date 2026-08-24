/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodMonitorSubscriber.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Implementation of the Class RadioMethodMonitorSubscriber
  *
  */

#include "core/types/src/ta_types.h"
#include "RadioMethodMonitorSubscriber.h"
#include "RadioProtocol.h"

using namespace TA_IRS_App;

#define MONITORSUBSCRIBER_METHOD_LENGTH  37
#define MONITORSUBSCRIBER_TSI_POSITION   0
#define MONITORSUBSCRIBER_MONREF_POSITION   32
#define MONITORSUBSCRIBER_OP_POSITION   36



/**
* Constructor
* @param    sessionRef
*/
RadioMethodMonitorSubscriber::RadioMethodMonitorSubscriber(RadioSessionReference sessionRef, const char * tsi, MonitorOperation operation, ta_int32 monitorRef)
    :RadioMethod(MONITORSUBSCRIBER_METHOD_LENGTH, METHOD_MONITORSUBSCRIBER, sessionRef)
{
    // TSI SHORTSTRING position is 0
	setParameterShortString(MONITORSUBSCRIBER_TSI_POSITION,tsi);
    //
    setParameterLong(MONITORSUBSCRIBER_MONREF_POSITION, monitorRef);
    // 
    setParameterByte(MONITORSUBSCRIBER_OP_POSITION, (unsigned char)operation);
}


/** 
  * RadioMethodMonitorSubscriber::~RadioMethodMonitorSubscriber
  *
  * Descructor
  *
  */
RadioMethodMonitorSubscriber::~RadioMethodMonitorSubscriber()
{

}



