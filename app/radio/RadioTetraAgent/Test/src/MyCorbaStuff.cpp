/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/Test/src/MyCorbaStuff.cpp $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This class contains a few useful methods for using CORBA
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // _MSC_VER

#include "MyCorbaStuff.h"

#include "core/corba/src/CorbaUtil.h"


void MyCorbaStuff::initialiseCORBA()
{
	TA_Base_Core::CorbaUtil::getInstance().initialise();
	TA_Base_Core::CorbaUtil::getInstance().activate();
}

void MyCorbaStuff::shutdownCORBA()
{
//	TA_Core::CorbaUtil::getInstance().shutdown();
//	TA_Core::CorbaUtil::getInstance().cleanup();
}
