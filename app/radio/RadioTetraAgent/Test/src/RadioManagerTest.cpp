/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/Test/src/RadioManagerTest.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This class implements the unit tests for the
  * TrainManagerTest class
  *
  */

// Disable "identifier truncated" warning
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // _MSC_VER

#include "core/types/src/ta_types.h"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "TrainManagerTest.h"
//#include "MyCorbaStuff.h"

#include "app/trains/train_agent/src/TrainManager.h"
#include "app/trains/train_agent/src/ITrainAgent.h"
#include "app/trains/train_agent/src/ITrainStatusUpdateMonitor.h"
#include "app/trains/train_agent/src/TrainException.h"
#include "core/exceptions/src/AssertException.h"
#include "core/message/types/MessageTypes.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/threads/src/Thread.h"

using TA_Core::TransactiveException;
using TA_Core::AssertException;


// Helper class
class TrainAgentCallbackStub : public TA_App::ITrainAgent
{
	virtual void sendMessageToTrain(std::string trainTSI,const std::vector<unsigned char> messageData)
	{};
};

class TrainStatusUpdateMonitorStub : public TA_App::ITrainStatusUpdateMonitor
{
	virtual void onTrainStatusUpdate(unsigned int trainID, EStatusField field)
	{
	};
};


//////////////////////
// Test Constructor //
//////////////////////

void TrainManagerTest::testConstructor()
{
	//
	// Test:
	//   Tests if the TrainManager can be constructed 
	//
	// Expected:
	//   No exceptions.
	//
	// Preconditions:
	//   None.
	//
	try
	{
		// Get the named object
		TrainAgentCallbackStub  trainAgentCallback;
		TrainStatusUpdateMonitorStub  trainStatusUpdateMonitor;
		TA_App::TrainManager*  manager = new TA_App::TrainManager(trainAgentCallback,trainStatusUpdateMonitor);
		Assert(manager!=NULL);
		delete manager;
	}

	// If there is an exception, the test has failed
	catch(...)
	{
		Assert(false);
	}
}


//
// Test:
//   Tests if the onTrainEvent handles a TSI change 
//
// Expected:
//   No exceptions.
//
// Preconditions:
//   None.
//
void TrainManagerTest::testOnTrainEventTSIChange()
{
	TrainAgentCallbackStub  trainAgentCallback;
	TrainStatusUpdateMonitorStub  trainStatusUpdateMonitor;
	TA_App::TrainManager*  manager = new TA_App::TrainManager(trainAgentCallback,trainStatusUpdateMonitor);
	Assert(manager!=NULL);

	try
	{
		std::vector<unsigned char>  data;
		data.push_back(7);          // Flag
		data.push_back(9);          // Message, any event type ok for this one
		data.push_back(22);         // train
		data.push_back(7);          // Length
		data.push_back(1);          // Origin
		data.push_back(0x97);		// Sumcheck 1
		data.push_back(0x4f);		// Sumcheck 2
		TA_App::TrainEvent   event("123-234-456",data);
		
		// Process the event
		manager->onTrainEvent(event);
	}
	// If there is an exception, the test has failed
	catch(...)
	{
		Assert(false);
	}

	delete manager;
}

//
// Test:
//   Tests if the onTrainEvent handles a location change 
//
// Expected:
//   No exceptions.
//
// Preconditions:
//   None.
//
void TrainManagerTest::testOnTrainEventLocationChange()
{
}

//
// Test:
//   Tests if the onTrainEvent handles a OA1 table 
//
// Expected:
//   No exceptions.
//
// Preconditions:
//   None.
//
void TrainManagerTest::testOnTrainEventOA1Table()
{
}

//
// Test:
//   Tests if the onTrainEvent handles a OA1 table 
//
// Expected:
//   No exceptions.
//
// Preconditions:
//   None.
//
void TrainManagerTest::testOnTrainEventATCAlarmTable()
{
}

//			void setTrainDetails(unsigned int trainID, unsigned long currentLocation, std::string serviceNumber, bool inBound, bool isMute);
			void setTrainDetails(unsigned int trainID,  ta_uint32  currentLocation, std::string serviceNumber, bool inBound, bool isMute);
//
// Test:
//   Tests if the onTrainEvent handles a call driver event 
//
// Expected:
//   No exceptions.
//
// Preconditions:
//   None.
//
void TrainManagerTest::testOnTrainEventCallDriver()
{
}

//
// Test:
//   Tests if the onTrainEvent handles a TIMS changeover event 
//
// Expected:
//   No exceptions.
//
// Preconditions:
//   None.
//
void TrainManagerTest::testOnTrainEventTIMSChangeOver()
{
}

//
// Test:
//   Tests if the onTrainEvent handles a test call event 
//
// Expected:
//   No exceptions.
//
// Preconditions:
//   None.
//
void TrainManagerTest::testOnTrainEventTestCall()
{
}


//
// Test:
//   Tests if the onTrainEvent handles an unknown message 
//
// Expected:
//   No exceptions.
//
// Preconditions:
//   None.
//
void TrainManagerTest::testOnTrainEventUnhandledMessage()
{
}

//
// Test:
//   Tests if the sendTrainCommandWithWait completes 
//
// Expected:
//   No exceptions.
//
// Preconditions:
//   None.
//
void TrainManagerTest::testSendTrainCommandWithWait()
{
}


//
// Test:
//   Tests if the sendTrainCommandWithWait completes 
//
// Expected:
//   No exceptions.
//
// Preconditions:
//   None.
//
void TrainManagerTest::testGetTrainStatusManager()
{
}

		

/////////////////////
// Utility Methods //
/////////////////////



////////////////////////////////
// Inherited TestCase Methods //
////////////////////////////////

void TrainManagerTest::setUp( const std::string & INFile , const std::string & OUTFile )
{
}


void TrainManagerTest::tearDown( const std::string & INFile )
{
}


void TrainManagerTest::setUpSuite( const std::string & inFile )
{
}


void TrainManagerTest::tearDownSuite()
{
}


TestSuite * TrainManagerTest::suite ()
{
    TestSuite * testSuite = new TestSuite("TrainManagerTest");

	//////////////////////
	// Test Constructor //
	//////////////////////

	testSuite->addTest(new TestCaller<TrainManagerTest>("testConstructor",&TrainManagerTest::testConstructor,this));
	testSuite->addTest(new TestCaller<TrainManagerTest>("testOnTrainEventTSIChange",&TrainManagerTest::testOnTrainEventTSIChange,this));
	testSuite->addTest(new TestCaller<TrainManagerTest>("testOnTrainEventLocationChange",&TrainManagerTest::testOnTrainEventLocationChange,this));
	testSuite->addTest(new TestCaller<TrainManagerTest>("testOnTrainEventOA1Table",&TrainManagerTest::testOnTrainEventOA1Table,this));
	testSuite->addTest(new TestCaller<TrainManagerTest>("testOnTrainEventATCAlarmTable",&TrainManagerTest::testOnTrainEventATCAlarmTable,this));
	testSuite->addTest(new TestCaller<TrainManagerTest>("testOnTrainEventCallDriver",&TrainManagerTest::testOnTrainEventCallDriver,this));
	testSuite->addTest(new TestCaller<TrainManagerTest>("testOnTrainEventTIMSChangeOver",&TrainManagerTest::testOnTrainEventTIMSChangeOver,this));
	testSuite->addTest(new TestCaller<TrainManagerTest>("testOnTrainEventTestCall",&TrainManagerTest::testOnTrainEventTestCall,this));
	testSuite->addTest(new TestCaller<TrainManagerTest>("testOnTrainEventUnhandledMessage",&TrainManagerTest::testOnTrainEventUnhandledMessage,this));
	testSuite->addTest(new TestCaller<TrainManagerTest>("testSendTrainCommandWithWait",&TrainManagerTest::testSendTrainCommandWithWait,this));
	testSuite->addTest(new TestCaller<TrainManagerTest>("testGetTrainStatusManager",&TrainManagerTest::testGetTrainStatusManager,this));

	return testSuite;
}

