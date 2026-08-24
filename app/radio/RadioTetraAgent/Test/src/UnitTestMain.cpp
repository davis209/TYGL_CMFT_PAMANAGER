/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/Test/src/UnitTestMain.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This is the main program file used to provide the unit test harness for the Radio
  * agent.
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // _MSC_VER

#include <iostream>
#include "core/utilities/src/DebugUtil.h"
#include "cots/CppUnit/src/TestRunner.h"


using TA_Base_Core::DebugUtil;

#include "RadioCircuitModeManagerUnitTest.h"

#include "ace/ACE.h"

#include "RadioCircuitModeManager.h"

#include <iostream>


int main( int argc, char **argv)
{
	// Set the debug information so we don't get the debug to standard output.
	DebugUtil::getInstance().setLevel(DebugUtil::DebugDebug);
	DebugUtil::getInstance().setFile("./RadioAgentUnitTest.log");
    
    TestRunner runner;

    RadioCircuitModeManagerUnitTest suite1( "RadioCircuitModeManagerUnitTest" );
    runner.addSuite( suite1.suite() );

    try
    {
        // Run config file. Make sure it is in the cfg directory.
        runner.begin( "./cfg/UnitConfig.ini" );
    }
    catch (...)
    {
		std::cout << "Caught an unhandled exception" << std::endl;
	}

    return 0;
}


