/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/Test/src/RadioSessionInterfaceTest.cpp $
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


const char * TEST_TSI_1 = "751-7-525";
const char * TEST_TSI_2 = "755-7-525";
const char * TEST_TSI_3 = "756-7-525";
//const char * TEST_LOGIN_NAME = "OCC_TCO"; 
const char * TEST_LOGIN_NAME = "op1"; 
const char * TEST_GTSI_1 = "2000";
const char * TEST_GTSI_2 = "1000";
const char * TEST_GTSI_3 = "2002-7-525";
const char * PATCH_NAME = "ModuleTestPatch";

#include "core/types/src/ta_types.h"
#include "RadioSessionInterfaceTest.h"
#include "MyCorbaStuff.h"

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "core/exceptions/src/AssertException.h"
#include "core/message/types/MessageTypes.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "bus/radio/PTTMonitorLibrary/src/PTTButton.h"
#include "bus/radio/PTTMonitorLibrary/src/IPTTMonitoring.h"

using TA_Base_Core::TransactiveException;
using TA_Base_Core::AssertException;



class PTTMonitor : public TA_IRS_Bus::IPTTMonitoring
{
	public:
		virtual void onPTTActivated() {};
		virtual void onPTTReleased() {};
		virtual void onPTTGranted() {};
		virtual void onPTTQueued() {};
		virtual void onPTTCease() {};
		virtual void onPTTBusy() {};
		virtual void onPTTMonitoring() {};
};




//////////////////////
// Test Constructor //
//////////////////////

void RadioSessionInterfaceTest::testRadioSessionLogin()
{
	try
	{
	   m_radioSessionObject->sessionLogin(TEST_LOGIN_NAME,m_sessionID.c_str());
	}
	catch (TA_Base_Bus::IRadioSessionCorbaDef::radioAPIException& e)
	{
		std::string msg = "Radio API exception: ";
		msg += e.what;
		TA_ASSERT(false,msg);
	}
	catch (...)
	{
		Assert(false);
	}
}

//*********************
// Try each call type *
//*********************

void RadioSessionInterfaceTest::testMakeDeleteFullDuplexCall()
{
	try
	{
		// Setup a new call
//	   long callID = m_radioSessionObject->makeCall(TEST_TSI_1,
	   ta_int32 callID = m_radioSessionObject->makeCall(TEST_TSI_1,
								TA_Base_Bus::IRadioSessionCorbaDef::SingleCall,
								true,
								false,
								true,
								m_sessionID.c_str());
	   TA_ASSERT(callID!=0,"Call ID is invalid");

	   Sleep(5000);

		// Close the call
	    m_radioSessionObject->releaseCall(callID,m_sessionID.c_str());
	}
	catch (TA_Base_Bus::IRadioSessionCorbaDef::radioAPIException& e)
	{
		std::string msg = "Radio API exception: ";
		msg += e.what;
		TA_ASSERT(false,msg);
	}
	catch (...)
	{
		Assert(false);
	}
}

void RadioSessionInterfaceTest::testMakeDeleteEmergencyFullDuplexCall()
{
	try
	{
		// Setup a new call
//	   long callID = m_radioSessionObject->makeCall(TEST_TSI_1,
	   ta_int32 callID = m_radioSessionObject->makeCall(TEST_TSI_1,
       
								TA_Base_Bus::IRadioSessionCorbaDef::SingleCall,
								true,
    							true,
								true,
								m_sessionID.c_str());
	   TA_ASSERT(callID!=0,"Call ID is invalid");

	   Sleep(5000);

		// Close the call
	    m_radioSessionObject->releaseCall(callID,m_sessionID.c_str());
	}
	catch (TA_Base_Bus::IRadioSessionCorbaDef::radioAPIException& e)
	{
		std::string msg = "Radio API exception: ";
		msg += e.what;
		TA_ASSERT(false,msg);
	}
	catch (...)
	{
		Assert(false);
	}
}


void RadioSessionInterfaceTest::testMakeHoldResumeDeleteFullDuplexCall()
{
	try
	{
		// Setup a new call
//		long callID = m_radioSessionObject->makeCall(TEST_TSI_1,
	 ta_int32  callID = m_radioSessionObject->makeCall(TEST_TSI_1,
								TA_Base_Bus::IRadioSessionCorbaDef::SingleCall,
								true,
								false,
								true,
								m_sessionID.c_str());
		TA_ASSERT(callID!=0,"Call ID is invalid");
		Sleep(5000);
		m_radioSessionObject->holdCall(callID,m_sessionID.c_str());
		Sleep(5000);
		m_radioSessionObject->resumeCall(callID,m_sessionID.c_str());
		Sleep(5000);
	    m_radioSessionObject->releaseCall(callID,m_sessionID.c_str());
	}
	catch (TA_Base_Bus::IRadioSessionCorbaDef::radioAPIException& e)
	{
		std::string msg = "Radio API exception: ";
		msg += e.what;
		TA_ASSERT(false,msg);
	}
	catch (...)
	{
		Assert(false);
	}
}


void RadioSessionInterfaceTest::testIncludeCall()
{
	try
	{
		// Setup a new call
//		long callID = m_radioSessionObject->makeCall(TEST_TSI_1,
	 ta_int32  callID = m_radioSessionObject->makeCall(TEST_TSI_1,
								TA_Base_Bus::IRadioSessionCorbaDef::SingleCall,
								true,
								false,
								true,
								m_sessionID.c_str());
		TA_ASSERT(callID!=0,"Call ID is invalid");
		m_radioSessionObject->includeSubscriber(callID,TEST_TSI_2,m_sessionID.c_str());
	    //m_radioSessionObject->selectCall(callID,m_sessionID.c_str());
	    //m_radioSessionObject->ceaseTX(m_sessionID.c_str());
		Sleep(2000);
	    //m_radioSessionObject->demandTX(priority,m_sessionID.c_str(),queueID);

		Sleep(5000);
	    m_radioSessionObject->releaseCall(callID,m_sessionID.c_str());
	}
	catch (TA_Base_Bus::IRadioSessionCorbaDef::radioAPIException& e)
	{
		std::string msg = "Radio API exception: ";
		msg += e.what;
		TA_ASSERT(false,msg);
	}
	catch (...)
	{
		Assert(false);
	}
}

void RadioSessionInterfaceTest::testMakeAdjustVolumeDeleteFullDuplexCall()
{
	try
	{
		// Setup a new call
//		long callID = m_radioSessionObject->makeCall(TEST_TSI_1,
	 ta_int32  callID = m_radioSessionObject->makeCall(TEST_TSI_1,
								TA_Base_Bus::IRadioSessionCorbaDef::SingleCall,
								true,
								false,
								true,
								m_sessionID.c_str());
		TA_ASSERT(callID!=0,"Call ID is invalid");

		m_radioSessionObject->adjustCallVolume(callID,true,m_sessionID.c_str());
	    Sleep(1000);
		m_radioSessionObject->adjustCallVolume(callID,true,m_sessionID.c_str());
	    Sleep(1000);
		m_radioSessionObject->adjustCallVolume(callID,true,m_sessionID.c_str());
	    Sleep(1000);
		m_radioSessionObject->adjustCallVolume(callID,true,m_sessionID.c_str());
	    Sleep(1000);
		m_radioSessionObject->adjustCallVolume(callID,false,m_sessionID.c_str());
	    Sleep(1000);
		m_radioSessionObject->adjustCallVolume(callID,false,m_sessionID.c_str());
	    Sleep(1000);
		m_radioSessionObject->adjustCallVolume(callID,false,m_sessionID.c_str());
	    Sleep(1000);
		m_radioSessionObject->adjustCallVolume(callID,false,m_sessionID.c_str());
	    Sleep(1000);
	    m_radioSessionObject->releaseCall(callID,m_sessionID.c_str());
	}
	catch (TA_Base_Bus::IRadioSessionCorbaDef::radioAPIException& e)
	{
		std::string msg = "Radio API exception: ";
		msg += e.what;
		TA_ASSERT(false,msg);
	}
	catch (...)
	{
		Assert(false);
	}
}

void RadioSessionInterfaceTest::testMakeDeleteHalfDuplexCall()
{
	try
	{
		// Setup a new call
//		long callID = m_radioSessionObject->makeCall(TEST_GTSI_1,
	 ta_int32  callID = m_radioSessionObject->makeCall(TEST_GTSI_1,
								TA_Base_Bus::IRadioSessionCorbaDef::GroupUnacknowledgeCall,
								false,
								false,
								true,
								m_sessionID.c_str());
		TA_ASSERT(callID!=0,"Call ID is invalid");
	    m_radioSessionObject->releaseCall(callID,m_sessionID.c_str());
	}
	catch (TA_Base_Bus::IRadioSessionCorbaDef::radioAPIException& e)
	{
		std::string msg = "Radio API exception: ";
		msg += e.what;
		TA_ASSERT(false,msg);
	}
	catch (...)
	{
		Assert(false);
	}
}

void RadioSessionInterfaceTest::testMakeDeleteAcknowlegeHalfDuplexCall()
{
	try
	{
		// Setup a new call
//		long callID = m_radioSessionObject->makeCall(TEST_GTSI_1,
	 ta_int32  callID = m_radioSessionObject->makeCall(TEST_GTSI_1,
								TA_Base_Bus::IRadioSessionCorbaDef::GroupAcknowledgeCall,
								false,
								false,
								true,
								m_sessionID.c_str());
		TA_ASSERT(callID!=0,"Call ID is invalid");
	    m_radioSessionObject->releaseCall(callID,m_sessionID.c_str());
	}
	catch (TA_Base_Bus::IRadioSessionCorbaDef::radioAPIException& e)
	{
		std::string msg = "Radio API exception: ";
		msg += e.what;
		TA_ASSERT(false,msg);
	}
	catch (...)
	{
		Assert(false);
	}
}

void RadioSessionInterfaceTest::testMakeDeleteBroadcastHalfDuplexCall()
{
	try
	{
		// Setup a new call
//		long callID = m_radioSessionObject->makeCall(TEST_GTSI_1,
	 ta_int32  callID = m_radioSessionObject->makeCall(TEST_GTSI_1,
								TA_Base_Bus::IRadioSessionCorbaDef::GroupBroadcastCall,
								false,
								false,
								true,
								m_sessionID.c_str());
		TA_ASSERT(callID!=0,"Call ID is invalid");
	    m_radioSessionObject->releaseCall(callID,m_sessionID.c_str());
	}
	catch (TA_Base_Bus::IRadioSessionCorbaDef::radioAPIException& e)
	{
		std::string msg = "Radio API exception: ";
		msg += e.what;
		TA_ASSERT(false,msg);
	}
	catch (...)
	{
		Assert(false);
	}
}

//*******************************
// Test out Demand and Cease TX *
//*******************************

void RadioSessionInterfaceTest::testMakeDeleteTXHalfDuplexCall()
{
	try
	{
//		unsigned long queueID;
		 ta_uint32  queueID;
		unsigned int  priority = 1;

		// Setup a new call
//		long callID = m_radioSessionObject->makeCall(TEST_GTSI_1,
	 ta_int32  callID = m_radioSessionObject->makeCall(TEST_GTSI_1,
								TA_Base_Bus::IRadioSessionCorbaDef::GroupUnacknowledgeCall,
								false,
								false,
								true,
								m_sessionID.c_str());
		TA_ASSERT(callID!=0,"Call ID is invalid");
	    m_radioSessionObject->selectCall(callID,m_sessionID.c_str());
	    m_radioSessionObject->ceaseTX(m_sessionID.c_str());
		Sleep(2000);
	    m_radioSessionObject->demandTX(priority,m_sessionID.c_str(),queueID);
		Sleep(2000);
	    m_radioSessionObject->ceaseTX(m_sessionID.c_str());
		Sleep(2000);
	    m_radioSessionObject->releaseCall(callID,m_sessionID.c_str());
	}
	catch (TA_Base_Bus::IRadioSessionCorbaDef::radioAPIException& e)
	{
		std::string msg = "Radio API exception: ";
		msg += e.what;
		TA_ASSERT(false,msg);
	}
	catch (...)
	{
		Assert(false);
	}
}


void RadioSessionInterfaceTest::testMakeDeletePTTHalfDuplexCall()
{
	try
	{
		// Setup a new call -  use single (groups must be to groups)
//		long callID = m_radioSessionObject->makeCall(TEST_GTSI_1,
	 ta_int32  callID = m_radioSessionObject->makeCall(TEST_GTSI_1,
								TA_Base_Bus::IRadioSessionCorbaDef::GroupUnacknowledgeCall,
								false,
								false,
								true,
								m_sessionID.c_str());
		TA_ASSERT(callID!=0,"Call ID is invalid");
	    m_radioSessionObject->selectCall(callID,m_sessionID.c_str());

		// Stop default TX demand or TCPserver will lock up
	    m_radioSessionObject->ceaseTX(m_sessionID.c_str());

		PTTMonitor    monitor;
		TA_IRS_Bus::PTTButton pttButton(monitor);
		pttButton.startMonitoring(callID,m_sessionID.c_str(),"COM1");
		Sleep(20000);
		Sleep(20000);
		Sleep(20000);
		pttButton.endMonitoring();
	    m_radioSessionObject->releaseCall(callID,m_sessionID.c_str());
	}
	catch (TA_Base_Bus::IRadioSessionCorbaDef::radioAPIException& e)
	{
		std::string msg = "Radio API exception: ";
		msg += e.what;
		TA_ASSERT(false,msg);
	}
	catch (...)
	{
		Assert(false);
	}
}
//**********************
// SDS Message sending *
//**********************

void RadioSessionInterfaceTest::testSendSDS()
{
	try
	{
		// Setup a new call
		m_radioSessionObject->sendTextMessage(TEST_GTSI_1,"This is a test text message to group 2000",
								m_sessionID.c_str());
	}
	catch (TA_Base_Bus::IRadioSessionCorbaDef::radioAPIException& e)
	{
		std::string msg = "Radio API exception: ";
		msg += e.what;
		TA_ASSERT(false,msg);
	}
	catch (...)
	{
		Assert(false);
	}
}



//*******************************
// Test out Multi-select        *
// Three group calls - join 2   *
//*******************************
void RadioSessionInterfaceTest::testRadioSessionMultiSelect()
{
	try
	{
//		unsigned long queueID;
		 ta_uint32  queueID;
		unsigned int  priority = 1;
//		long          callID[3];
	 ta_int32           callID[3];

		// Setup a new calls
		callID[0] = m_radioSessionObject->makeCall(TEST_GTSI_1,
								TA_Base_Bus::IRadioSessionCorbaDef::GroupUnacknowledgeCall,
								false,
								false,
								true,
								m_sessionID.c_str());
		callID[1] = m_radioSessionObject->makeCall(TEST_GTSI_2,
								TA_Base_Bus::IRadioSessionCorbaDef::GroupUnacknowledgeCall,
								false,
								false,
								true,
								m_sessionID.c_str());
		callID[2] = m_radioSessionObject->makeCall(TEST_GTSI_3,
								TA_Base_Bus::IRadioSessionCorbaDef::GroupUnacknowledgeCall,
								false,
								false,
								true,
								m_sessionID.c_str());
		TA_ASSERT(callID[0]!=0,"Call 1 ID is invalid");
		TA_ASSERT(callID[1]!=0,"Call 2 ID is invalid");
		TA_ASSERT(callID[2]!=0,"Call 3 ID is invalid");

		// Select two calls only
	    m_radioSessionObject->selectCall(callID[1],m_sessionID.c_str());
	    m_radioSessionObject->selectCall(callID[2],m_sessionID.c_str());

		// Talk with selected groups
	    m_radioSessionObject->demandTX(priority,m_sessionID.c_str(),queueID);
		Sleep(5000);
	    m_radioSessionObject->ceaseTX(m_sessionID.c_str());
		Sleep(2000);

		// Change to first and last call
		m_radioSessionObject->deselectCall(callID[1],m_sessionID.c_str());
	    m_radioSessionObject->selectCall(callID[0],m_sessionID.c_str());

		// Talk with selected groups
	    m_radioSessionObject->demandTX(priority,m_sessionID.c_str(),queueID);
		Sleep(5000);
	    m_radioSessionObject->ceaseTX(m_sessionID.c_str());
		Sleep(2000);

		// Release all calls
	    m_radioSessionObject->releaseCall(callID[0],m_sessionID.c_str());
	    m_radioSessionObject->releaseCall(callID[1],m_sessionID.c_str());
	    m_radioSessionObject->releaseCall(callID[2],m_sessionID.c_str());
	}
	catch (TA_Base_Bus::IRadioSessionCorbaDef::radioAPIException& e)
	{
		std::string msg = "Radio API exception: ";
		msg += e.what;
		TA_ASSERT(false,msg);
	}
	catch (...)
	{
		Assert(false);
	}
};


//******************************
// Dynamic group setup and use *
//******************************
void RadioSessionInterfaceTest::testMakeDeleteDGNACall()
{
//	unsigned long queueID;
	 ta_uint32  queueID;
	unsigned int  priority = 1;

	try
	{
		TA_Base_Bus::IRadioSessionCorbaDef::DestinationTSIList   tsiList;

		// Create dynamic group
		tsiList.length(3);
		tsiList[0] = TEST_TSI_1;
		tsiList[1] = TEST_TSI_2;
		tsiList[2] = TEST_TSI_3;

		const char *  dgnaTSI = m_radioSessionObject->makeDynamicGroup(tsiList, m_sessionID.c_str());

		Sleep(3000);  // Let DGNA setup!

		// Setup a new call
//		long callID = m_radioSessionObject->makeCall(dgnaTSI,
	 ta_int32  callID = m_radioSessionObject->makeCall(dgnaTSI,
								TA_Base_Bus::IRadioSessionCorbaDef::GroupUnacknowledgeCall,
								false,
								false,
								true,
								m_sessionID.c_str());
		TA_ASSERT(callID!=0,"Call ID is invalid");

	    m_radioSessionObject->selectCall(callID,m_sessionID.c_str());

		// Talk with selected groups
	    m_radioSessionObject->ceaseTX(m_sessionID.c_str());
	    m_radioSessionObject->demandTX(priority,m_sessionID.c_str(),queueID);

	    m_radioSessionObject->releaseCall(callID,m_sessionID.c_str());
	}
	catch (TA_Base_Bus::IRadioSessionCorbaDef::radioAPIException& e)
	{
		std::string msg = "Radio API exception: ";
		msg += e.what;
		TA_ASSERT(false,msg);
	}
	catch (...)
	{
		Assert(false);
	}
}

//******************
// Call forwarding *
//******************

void RadioSessionInterfaceTest::testGetCallForwarding()
{
	try
	{
	   TA_Base_Bus::IRadioSessionCorbaDef::CallForwardDetails*  details;
	   details = m_radioSessionObject->getCallForwarding();
	   //TA_ASSERT(strlen(details->destinationTSI)<1,"Forwarding already present");
	}
	catch (TA_Base_Bus::IRadioSessionCorbaDef::radioAPIException& e)
	{
		std::string msg = "Radio API exception: ";
		msg += e.what;
		TA_ASSERT(false,msg);
	}
	catch (...)
	{
		Assert(false);
	}
}

void RadioSessionInterfaceTest::testSetCallForwarding()
{
	try
	{
		// Set the forwarding
	   TA_Base_Bus::IRadioSessionCorbaDef::CallForwardDetails  newDetails;
	   newDetails.destinationTSI = TEST_TSI_1;
	   newDetails.forwardAction = TA_Base_Bus::IRadioSessionCorbaDef::ForwardAlways;
	   m_radioSessionObject->setCallForwarding(newDetails,m_sessionID.c_str());

	   // Check it is set
	   TA_Base_Bus::IRadioSessionCorbaDef::CallForwardDetails*  details;
	   details = m_radioSessionObject->getCallForwarding();
	   TA_ASSERT(details->forwardAction==TA_Base_Bus::IRadioSessionCorbaDef::ForwardAlways,"Forwarding type incorrect");
	   TA_ASSERT(strcmp(details->destinationTSI,TEST_TSI_1)==0,"Forwarding to incorrect destination");

	   // Reset to none
	   newDetails.destinationTSI = std::string("").c_str();
	   newDetails.forwardAction = TA_Base_Bus::IRadioSessionCorbaDef::NeverForward;
	   m_radioSessionObject->setCallForwarding(newDetails,m_sessionID.c_str());
	}
	catch (TA_Base_Bus::IRadioSessionCorbaDef::radioAPIException& e)
	{
		std::string msg = "Radio API exception: ";
		msg += e.what;
		TA_ASSERT(false,msg);
	}
	catch (...)
	{
		Assert(false);
	}
}



//*****************
// Group Patching *
//*****************

void RadioSessionInterfaceTest::testCreateGroupPatch()
{
	try
	{
	   TA_Base_Bus::IRadioSessionCorbaDef::PatchList  patchList;
	   patchList.length(2);
	   patchList[0] = TEST_GTSI_1;
	   patchList[1] = TEST_GTSI_2;
	   m_radioSessionObject->createPatch(patchList,PATCH_NAME,m_sessionID.c_str());
	}
	catch (TA_Base_Bus::IRadioSessionCorbaDef::radioAPIException& e)
	{
		std::string msg = "Radio API exception: ";
		msg += e.what;
		TA_ASSERT(false,msg);
	}
	catch (...)
	{
		Assert(false);
	}
}


void RadioSessionInterfaceTest::testDeleteGroupPatch()
{
	try
	{
	   m_radioSessionObject->deletePatch(PATCH_NAME,m_sessionID.c_str());
	}
	catch (TA_Base_Bus::IRadioSessionCorbaDef::radioAPIException& e)
	{
		std::string msg = "Radio API exception: ";
		msg += e.what;
		TA_ASSERT(false,msg);
	}
	catch (...)
	{
		Assert(false);
	}
}


//*********************
// Finish the session *
//*********************

void RadioSessionInterfaceTest::testRadioSessionLogout()
{
	try
	{
	   m_radioSessionObject->sessionLogout(m_sessionID.c_str());
	}
	catch (TA_Base_Bus::IRadioSessionCorbaDef::radioAPIException& e)
	{
		std::string msg = "Radio API exception: ";
		msg += e.what;
		TA_ASSERT(false,msg);
	}
	catch (...)
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

void RadioSessionInterfaceTest::setUp( const std::string & INFile , const std::string & OUTFile )
{
	// Get the object
	m_radioSessionObject.setEntityName("RadioSession_OCC_MFT1");
//	Assert(!m_radioSessionObject->_non_existent());

	// Set up session ID
	m_sessionID = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);

}


void RadioSessionInterfaceTest::tearDown( const std::string & INFile )
{
}


void RadioSessionInterfaceTest::setUpSuite( const std::string & inFile )
{
	MyCorbaStuff::initialiseCORBA();
}


void RadioSessionInterfaceTest::tearDownSuite()
{
	MyCorbaStuff::shutdownCORBA();
}


TestSuite * RadioSessionInterfaceTest::suite ()
{
    TestSuite * testSuite = new TestSuite("RadioSessionInterfaceTest");

	//////////////////////
	// Test Constructor //
	//////////////////////

	testSuite->addTest(new TestCaller< RadioSessionInterfaceTest >("testRadioSessionLogin",&RadioSessionInterfaceTest::testRadioSessionLogin,this));
	testSuite->addTest(new TestCaller< RadioSessionInterfaceTest >("testMakeDeleteFullDuplexCall",&RadioSessionInterfaceTest::testMakeDeleteFullDuplexCall,this));
	testSuite->addTest(new TestCaller< RadioSessionInterfaceTest >("testMakeDeleteEmergencyFullDuplexCall",&RadioSessionInterfaceTest::testMakeDeleteEmergencyFullDuplexCall,this));
	testSuite->addTest(new TestCaller< RadioSessionInterfaceTest >("testMakeHoldResumeDeleteFullDuplexCall",&RadioSessionInterfaceTest::testMakeHoldResumeDeleteFullDuplexCall,this));
	testSuite->addTest(new TestCaller< RadioSessionInterfaceTest >("testIncludeCall",&RadioSessionInterfaceTest::testIncludeCall,this));
	testSuite->addTest(new TestCaller< RadioSessionInterfaceTest >("testMakeAdjustVolumeDeleteFullDuplexCall",&RadioSessionInterfaceTest::testMakeAdjustVolumeDeleteFullDuplexCall,this));
	testSuite->addTest(new TestCaller< RadioSessionInterfaceTest >("testMakeDeleteHalfDuplexCall",&RadioSessionInterfaceTest::testMakeDeleteHalfDuplexCall,this));
	testSuite->addTest(new TestCaller< RadioSessionInterfaceTest >("testMakeDeleteTXHalfDuplexCall",&RadioSessionInterfaceTest::testMakeDeleteTXHalfDuplexCall,this));
	testSuite->addTest(new TestCaller< RadioSessionInterfaceTest >("testMakeDeletePTTHalfDuplexCall",&RadioSessionInterfaceTest::testMakeDeletePTTHalfDuplexCall,this));
	testSuite->addTest(new TestCaller< RadioSessionInterfaceTest >("testMakeDeleteAcknowlegeHalfDuplexCall",&RadioSessionInterfaceTest::testMakeDeleteAcknowlegeHalfDuplexCall,this));
	testSuite->addTest(new TestCaller< RadioSessionInterfaceTest >("testMakeDeleteBroadcastHalfDuplexCall",&RadioSessionInterfaceTest::testMakeDeleteBroadcastHalfDuplexCall,this));
	testSuite->addTest(new TestCaller< RadioSessionInterfaceTest >("testSendSDS",&RadioSessionInterfaceTest::testSendSDS,this));
	testSuite->addTest(new TestCaller< RadioSessionInterfaceTest >("testRadioSessionMultiSelect",&RadioSessionInterfaceTest::testRadioSessionMultiSelect,this));
	testSuite->addTest(new TestCaller< RadioSessionInterfaceTest >("testMakeDeleteDGNACall",&RadioSessionInterfaceTest::testMakeDeleteDGNACall,this));
	testSuite->addTest(new TestCaller< RadioSessionInterfaceTest >("testGetCallForwarding",&RadioSessionInterfaceTest::testGetCallForwarding,this));
	testSuite->addTest(new TestCaller< RadioSessionInterfaceTest >("testSetCallForwarding",&RadioSessionInterfaceTest::testSetCallForwarding,this));
	testSuite->addTest(new TestCaller< RadioSessionInterfaceTest >("testCreateGroupPatch",&RadioSessionInterfaceTest::testCreateGroupPatch,this));
	testSuite->addTest(new TestCaller< RadioSessionInterfaceTest >("testDeleteGroupPatch",&RadioSessionInterfaceTest::testDeleteGroupPatch,this));

	testSuite->addTest(new TestCaller< RadioSessionInterfaceTest >("testRadioSessionLogout",&RadioSessionInterfaceTest::testRadioSessionLogout,this));

    return testSuite;
}
