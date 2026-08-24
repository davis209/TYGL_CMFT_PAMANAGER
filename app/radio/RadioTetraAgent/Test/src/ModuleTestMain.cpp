/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/Test/src/ModuleTestMain.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This is the main program file used to provide the unit test harness for the Train
  * agent.
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // _MSC_VER

#include <iostream>
#include "core/utilities/src/DebugUtil.h"
#include "cots/CppUnit/src/TestRunner.h"
#include "core/utilities/src/RunParams.h"

//#include "MessagePublicationManagerTest.h"

#include "RadioInterfaceTest.h"
#include "RadioSessionInterfaceTest.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;


void displayUsage(const char* appName)
{
	std::cout << std::endl
		  << "Usage: " << appName << " with the following parameters: "
		  << std::endl
		  << "\t--notify-hosts=<hostname(s)>" << std::endl
		  << "\t--db-connection=<user:pass:service>" << std::endl
		  << "\t--session-id=<your session id>" << std::endl
		  << "where <hostname(s)> is a list of notification "
		  << "service hosts (optionally with ports)." << std::endl
		  << "eg:" << std::endl
		  << appName << " --NotifyHosts=jupiter --DbConnection=transact:its:its" 
		  << std::endl
		  << appName << " --NotifyHosts=jupiter:1234,jupiter:4321 --DbConnection=transact:its:its"
		  << std::endl;

}



int main( int argc, char **argv)
{
	// Set the debug information so we don't get the debug to standard output.
	DebugUtil::getInstance().setLevel(DebugUtil::DebugDebug);
	DebugUtil::getInstance().setFile("./RadioInterfaceModuleTest.log");
	
	// Get the parse parameters - used for some tests e.g. using Notification service 
    RunParams::getInstance().set(RPARAM_LOCATIONKEY, "1");
		if(argc > 1)	 
	{
		RunParams::getInstance().parseCmdLine( argc, argv );
	}
	else
	{
		    RunParams::getInstance().set(RPARAM_DBCONNECTION, "tra_dev2:unittest:unittest");
			RunParams::getInstance().set(RPARAM_NOTIFYHOSTS, "localhost");
			RunParams::getInstance().set(RPARAM_SESSIONID, "1");
	}


    //
	// Parse args for correctness
	//
	if ( ! RunParams::getInstance().isSet(RPARAM_NOTIFYHOSTS) ) 
	{
		std::cout << std::endl << "Notification Host not specified." << std::endl;
		displayUsage(argv[0]);
		return 0;
	}

	if ( ! RunParams::getInstance().isSet(RPARAM_DBCONNECTION) ) 
	{
		std::cout << std::endl << "Database connection not specified." << std::endl;
		displayUsage(argv[0]);
		return 0;
	}

	if ( ! RunParams::getInstance().isSet(RPARAM_SESSIONID) ) 
	{
		std::cout << std::endl << "Session not specified." << std::endl;
		displayUsage(argv[0]);
		return 0;
	}

    
    TestRunner runner;
    

	//////////////////////
	// CCTV IDL Testing //
	//////////////////////

    RadioInterfaceTest			testcase1("RadioInterfaceTest");
    RadioSessionInterfaceTest	testcase2("RadioSessionInterfaceTest");
    runner.addSuite( testcase1.suite() );      
    runner.addSuite( testcase2.suite() );      

    try
    {
        // Run config file. Make sure it is in the cfg directory.
        runner.begin( "./cfg/ModuleConfig.ini" );
    }
    catch (...)
    {
		std::cout << "Caught an unhandled exception" << std::endl;
	}

    return 0;
}


