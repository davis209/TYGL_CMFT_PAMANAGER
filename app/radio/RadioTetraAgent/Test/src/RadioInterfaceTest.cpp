/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/Test/src/RadioInterfaceTest.cpp $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This class implements the unit tests for the
  * CommsMessageSender class found in the TA_Message library.
  *
  */

// Disable "identifier truncated" warning
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // _MSC_VER


#include "core/types/src/ta_types.h"
#include "RadioInterfaceTest.h"
#include "MyCorbaStuff.h"

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "core/exceptions/src/AssertException.h"
#include "core/message/types/MessageTypes.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

using TA_Base_Core::TransactiveException;
using TA_Base_Core::AssertException;


//////////////////////
// Test Constructor //
//////////////////////

void RadioInterfaceTest::testAuthoriseCall()
{
	try
	{
		// Get an incoming call into the simulator
		// We shall do this by piggy-backing the load on a getAuthorisation list command
		TA_Base_Bus::IRadioCorbaDef::CallAuthorisationList* list = m_radioObject->getCallAuthorisations();


		// Authorise all calls
		for (int iLoop=0;iLoop<list->length();iLoop++)
		{
//			long callID = (*list)[iLoop].callID;
		 ta_int32  callID = (*list)[iLoop].callID;
			m_radioObject->authoriseCall(callID,true,m_sessionID.c_str());
		}
		
		// No problems, test succeeded
	}

	// If there is an exception, the test has failed
	catch(...)
	{
		Assert(false);
	}
}




/////////////////////
// Utility Methods //
/////////////////////



////////////////////////////////
// Inherited TestCase Methods //
////////////////////////////////

void RadioInterfaceTest::setUp( const std::string & INFile , const std::string & OUTFile )
{
	// Get the object
	m_radioObject.setEntityName("OccRadioTetraAgent");
std::cout << "After corba set entity" << std::endl;

//	Assert(!m_radioObject->_non_existent());
	

	// Set up session ID
	m_sessionID = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);

}


void RadioInterfaceTest::tearDown( const std::string & INFile )
{
}


void RadioInterfaceTest::setUpSuite( const std::string & inFile )
{
	MyCorbaStuff::initialiseCORBA();

}


void RadioInterfaceTest::tearDownSuite()
{
	MyCorbaStuff::shutdownCORBA();
}


TestSuite * RadioInterfaceTest::suite ()
{
    TestSuite * testSuite = new TestSuite("RadioInterfaceTest");

	//////////////////////
	// Test Constructor //
	//////////////////////

	testSuite->addTest(new TestCaller< RadioInterfaceTest >("testAuthoriseCall",&RadioInterfaceTest::testAuthoriseCall,this));

    return testSuite;
}
