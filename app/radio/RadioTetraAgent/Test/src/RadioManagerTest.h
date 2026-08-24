/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/Test/src/RadioManagerTest.h $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This class implements the unit tests for the
  * TrainManager class.
  *
  */

#ifndef TrainManagerTest_H
#define TrainManagerTest_H

#include "cots/CppUnit/src/TestCase.h"
#include "cots/CppUnit/src/TestFramework.h"


class TrainManagerTest : public TestCase
{
public:
	
	TrainManagerTest(const std::string & name ) : TestCase ( name )
	{
	}

	virtual ~TrainManagerTest()
	{
	}


  /**
    * suite
    * 
    * This adds all the test methods to the suite of tests and returns it.
    *
    * @return TestSuite* A collection of all tests that can be run.
    *
    */
    TestSuite * suite();

  /**
    * setUp
    * 
    * Initialises variables before each method is run.
    *
    */
    void setUp( const std::string & INFile , const std::string & OUTFile );


  /**
    * tearDown
    * 
    * Destroys variables after each method is run.
    *
    */
    void tearDown( const std::string & INFile );


  /**
    * setUpSuite
    * 
    * Sets up variables before a suite of tests are run.
    *
    */
    virtual void setUpSuite( const std::string & inFile );


  /**
    * tearDownSuite
    * 
    * Destroys variables after a suite of tests have been run.
    *
    */
    void tearDownSuite();


protected:

    //
    // The actual test methods
    //
	void testConstructor();
	void testOnTrainEventTSIChange();
	void testOnTrainEventLocationChange();
	void testOnTrainEventOA1Table();
	void testOnTrainEventATCAlarmTable();
	void testOnTrainEventCallDriver();
	void testOnTrainEventTIMSChangeOver();
	void testOnTrainEventTestCall();
	void testOnTrainEventUnhandledMessage();
	void testSendTrainCommandWithWait();
	void testGetTrainStatusManager();

};

#endif // TrainManagerTest_H