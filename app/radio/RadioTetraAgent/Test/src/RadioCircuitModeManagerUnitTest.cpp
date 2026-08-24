/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/Test/src/RadioCircuitModeManagerUnitTest.cpp $
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


#include "RadioCircuitModeManagerUnitTest.h"

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "core/exceptions/src/AssertException.h"
#include "core/message/types/MessageTypes.h"
#include "core/threads/src/Thread.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

using TA_Base_Core::TransactiveException;
using TA_Base_Core::AssertException;

#include "Radio.h"
#include "RadioCircuitModeManager.h"


#include "RadioCallback.h"
#include "DataManager.h"


#include <iostream>

void RadioCircuitModeManagerUnitTest::testAddRadioCircuitModeTransfer_OnePacket()
{
    std::cout << "testAddRadioCircuitModeTransfer_OnePacket\n";
    RadioCallback callback;
    TA_IRS_App::Radio radio( callback );

    TA_IRS_App::RadioCircuitModeManager* rcmgr = radio.getRadioCircuitModeManager();

    // Create the packet.
    Data data;
    data.push_back( 'A' );
    data.push_back( 'B' );
    data.push_back( 'C' );
    data.push_back( 'D' );
    data.push_back( 'E' );
    data.push_back( 'F' );
    data.push_back( 'G' );
    data.push_back( 'H' );
    data.push_back( 'I' );
    data.push_back( 'J' );
    data.push_back( 'K' );
    data.push_back( 'L' );
    data.push_back( 'M' );
    data.push_back( 'N' );
    data.push_back( 'O' );
    data.push_back( 'P' );
    data.push_back( 'Q' );
    data.push_back( 'R' );

    DataManager::getDataManager().setLastExpectedPacket( "12345678910",data );

    rcmgr->addRadioCircuitTransfer( data, "12345678910" );

    TA_Base_Core::Thread::sleep( 2000 );

    Assert( true == DataManager::getDataManager().didPacketsMatch("12345678910") );
    
    // Need to send a signal to the Radio to signal that another packet can be sent.
    rcmgr->processIncomingCircuitModeDataCapacity( DataManager::getDataManager().getCallReference( "12345678910" ).m_radioReference );
    std::cout << "testAddRadioCircuitModeTransfer_OnePacket\n";
}


void RadioCircuitModeManagerUnitTest::testAddRadioCircuitModeTransfer_TwoPackets()
{
    std::cout << "testAddRadioCircuitModeTransfer_TwoPackets\n";
    RadioCallback callback;
    TA_IRS_App::Radio radio( callback );

    TA_IRS_App::RadioCircuitModeManager* rcmgr = radio.getRadioCircuitModeManager();

    // Create the packet.
    Data completeData;
    completeData.push_back( 'A' );
    completeData.push_back( 'B' );
    completeData.push_back( 'C' );
    completeData.push_back( 'D' );
    completeData.push_back( 'E' );
    completeData.push_back( 'F' );
    completeData.push_back( 'G' );
    completeData.push_back( 'H' );
    completeData.push_back( 'I' );
    completeData.push_back( 'J' );
    completeData.push_back( 'K' );
    completeData.push_back( 'L' );
    completeData.push_back( 'M' );
    completeData.push_back( 'N' );
    completeData.push_back( 'O' );
    completeData.push_back( 'P' );
    completeData.push_back( 'Q' );
    completeData.push_back( 'R' );
    completeData.push_back( 'S' );
    completeData.push_back( 'T' );
    completeData.push_back( 'U' );
    completeData.push_back( 'V' );
    completeData.push_back( 'W' );
    completeData.push_back( 'X' );
    completeData.push_back( 'Y' );
    completeData.push_back( 'Z' );
    completeData.push_back( '0' );
    completeData.push_back( '1' );
    completeData.push_back( '2' );
    completeData.push_back( '3' );
    completeData.push_back( '4' );
    completeData.push_back( '5' );
    completeData.push_back( '6' );
    completeData.push_back( '7' );
    completeData.push_back( '8' );
    completeData.push_back( '9' );

    Data packet1;
    packet1.push_back( 'A' );
    packet1.push_back( 'B' );
    packet1.push_back( 'C' );
    packet1.push_back( 'D' );
    packet1.push_back( 'E' );
    packet1.push_back( 'F' );
    packet1.push_back( 'G' );
    packet1.push_back( 'H' );
    packet1.push_back( 'I' );
    packet1.push_back( 'J' );
    packet1.push_back( 'K' );
    packet1.push_back( 'L' );
    packet1.push_back( 'M' );
    packet1.push_back( 'N' );
    packet1.push_back( 'O' );
    packet1.push_back( 'P' );
    packet1.push_back( 'Q' );
    packet1.push_back( 'R' );
    Data packet2;
    packet2.push_back( 'S' );
    packet2.push_back( 'T' );
    packet2.push_back( 'U' );
    packet2.push_back( 'V' );
    packet2.push_back( 'W' );
    packet2.push_back( 'X' );
    packet2.push_back( 'Y' );
    packet2.push_back( 'Z' );
    packet2.push_back( '0' );
    packet2.push_back( '1' );
    packet2.push_back( '2' );
    packet2.push_back( '3' );
    packet2.push_back( '4' );
    packet2.push_back( '5' );
    packet2.push_back( '6' );
    packet2.push_back( '7' );
    packet2.push_back( '8' );
    packet2.push_back( '9' );

    rcmgr->addRadioCircuitTransfer( completeData, "12345678910" );


    DataManager::getDataManager().setLastExpectedPacket( "12345678910",packet1 );

    TA_Base_Core::Thread::sleep( 2000 );

    Assert( true == DataManager::getDataManager().didPacketsMatch("12345678910") );

    DataManager::getDataManager().setLastExpectedPacket( "12345678910",packet2 );

    // Need to send a signal to the Radio to signal that another packet can be sent.
    rcmgr->processIncomingCircuitModeDataCapacity( DataManager::getDataManager().getCallReference( "12345678910" ).m_radioReference );

    TA_Base_Core::Thread::sleep( 2000 );

    Assert( true == DataManager::getDataManager().didPacketsMatch("12345678910") );
    
    // Need to send a signal to the Radio to signal that another packet can be sent.
    rcmgr->processIncomingCircuitModeDataCapacity( DataManager::getDataManager().getCallReference( "12345678910" ).m_radioReference );
    std::cout << "testAddRadioCircuitModeTransfer_TwoPackets\n";
}


void RadioCircuitModeManagerUnitTest::testAddRadioCircuitModeTransfer_TwoPackets_XOFF()
{
    std::cout << "testAddRadioCircuitModeTransfer_TwoPackets_XOFF\n";
    RadioCallback callback;
    TA_IRS_App::Radio radio( callback );

    TA_IRS_App::RadioCircuitModeManager* rcmgr = radio.getRadioCircuitModeManager();

    // Create the packet.
    Data completeData;
    completeData.push_back( 'A' );
    completeData.push_back( 'B' );
    completeData.push_back( 'C' );
    completeData.push_back( 'D' );
    completeData.push_back( 'E' );
    completeData.push_back( 'F' );
    completeData.push_back( 'G' );
    completeData.push_back( 'H' );
    completeData.push_back( 'I' );
    completeData.push_back( 'J' );
    completeData.push_back( 'K' );
    completeData.push_back( 'L' );
    completeData.push_back( 'M' );
    completeData.push_back( 'N' );
    completeData.push_back( 'O' );
    completeData.push_back( 'P' );
    completeData.push_back( 'Q' );
    completeData.push_back( 'R' );
    completeData.push_back( 'S' );
    completeData.push_back( 'T' );
    completeData.push_back( 'U' );
    completeData.push_back( 'V' );
    completeData.push_back( 'W' );
    completeData.push_back( 'X' );
    completeData.push_back( 'Y' );
    completeData.push_back( 'Z' );
    completeData.push_back( '0' );
    completeData.push_back( '1' );
    completeData.push_back( '2' );
    completeData.push_back( '3' );
    completeData.push_back( '4' );
    completeData.push_back( '5' );
    completeData.push_back( '6' );
    completeData.push_back( '7' );
    completeData.push_back( '8' );
    completeData.push_back( '9' );

    Data packet1;
    packet1.push_back( 'A' );
    packet1.push_back( 'B' );
    packet1.push_back( 'C' );
    packet1.push_back( 'D' );
    packet1.push_back( 'E' );
    packet1.push_back( 'F' );
    packet1.push_back( 'G' );
    packet1.push_back( 'H' );
    packet1.push_back( 'I' );
    packet1.push_back( 'J' );
    packet1.push_back( 'K' );
    packet1.push_back( 'L' );
    packet1.push_back( 'M' );
    packet1.push_back( 'N' );
    packet1.push_back( 'O' );
    packet1.push_back( 'P' );
    packet1.push_back( 'Q' );
    packet1.push_back( 'R' );
    Data packet2;
    packet2.push_back( 'S' );
    packet2.push_back( 'T' );
    packet2.push_back( 'U' );
    packet2.push_back( 'V' );
    packet2.push_back( 'W' );
    packet2.push_back( 'X' );
    packet2.push_back( 'Y' );
    packet2.push_back( 'Z' );
    packet2.push_back( '0' );
    packet2.push_back( '1' );
    packet2.push_back( '2' );
    packet2.push_back( '3' );
    packet2.push_back( '4' );
    packet2.push_back( '5' );
    packet2.push_back( '6' );
    packet2.push_back( '7' );
    packet2.push_back( '8' );
    packet2.push_back( '9' );

    std::string call1 = "12345678910";

    rcmgr->addRadioCircuitTransfer( completeData, call1 );


    DataManager::getDataManager().setLastExpectedPacket( call1, packet1 );

    TA_Base_Core::Thread::sleep( 2000 );

    Assert( true == DataManager::getDataManager().didPacketsMatch(call1) );

    DataManager::getDataManager().setLastExpectedPacket( call1, packet2 );

    // Send the task an XOFF signal
    rcmgr->processIncomingCircuitModeData( DataManager::getDataManager().getCallReference( call1 ).m_radioReference, TA_IRS_App::XOFF );

    // Need to send a signal to the Radio to signal that another packet can be sent.
    rcmgr->processIncomingCircuitModeDataCapacity( DataManager::getDataManager().getCallReference( call1 ).m_radioReference );

    TA_Base_Core::Thread::sleep( 2000 );

    Assert( false == DataManager::getDataManager().didPacketsMatch(call1) );

    // Send the task an XON signal
    rcmgr->processIncomingCircuitModeData( DataManager::getDataManager().getCallReference( call1 ).m_radioReference, TA_IRS_App::XON );

    TA_Base_Core::Thread::sleep( 2000 );
    
    // Need to send a signal to the Radio to signal that another packet can be sent.
    rcmgr->processIncomingCircuitModeDataCapacity( DataManager::getDataManager().getCallReference( call1 ).m_radioReference );

    Assert( true == DataManager::getDataManager().didPacketsMatch(call1) );

    // Need to send a signal to the Radio to signal that another packet can be sent.
    rcmgr->processIncomingCircuitModeDataCapacity( DataManager::getDataManager().getCallReference( call1 ).m_radioReference );
    std::cout << "testAddRadioCircuitModeTransfer_TwoPackets_XOFF\n";
}


void RadioCircuitModeManagerUnitTest::testAddRadioCircuitModeTransfer_TwoTransfers_TwoPackets()
{
    std::cout << "testAddRadioCircuitModeTransfer_TwoTransfers_TwoPackets\n";
    RadioCallback callback;
    TA_IRS_App::Radio radio( callback );

    TA_IRS_App::RadioCircuitModeManager* rcmgr = radio.getRadioCircuitModeManager();

    // Create the packet.
    Data completeData;
    completeData.push_back( 'A' );
    completeData.push_back( 'B' );
    completeData.push_back( 'C' );
    completeData.push_back( 'D' );
    completeData.push_back( 'E' );
    completeData.push_back( 'F' );
    completeData.push_back( 'G' );
    completeData.push_back( 'H' );
    completeData.push_back( 'I' );
    completeData.push_back( 'J' );
    completeData.push_back( 'K' );
    completeData.push_back( 'L' );
    completeData.push_back( 'M' );
    completeData.push_back( 'N' );
    completeData.push_back( 'O' );
    completeData.push_back( 'P' );
    completeData.push_back( 'Q' );
    completeData.push_back( 'R' );
    completeData.push_back( 'S' );
    completeData.push_back( 'T' );
    completeData.push_back( 'U' );
    completeData.push_back( 'V' );
    completeData.push_back( 'W' );
    completeData.push_back( 'X' );
    completeData.push_back( 'Y' );
    completeData.push_back( 'Z' );
    completeData.push_back( '0' );
    completeData.push_back( '1' );
    completeData.push_back( '2' );
    completeData.push_back( '3' );
    completeData.push_back( '4' );
    completeData.push_back( '5' );
    completeData.push_back( '6' );
    completeData.push_back( '7' );
    completeData.push_back( '8' );
    completeData.push_back( '9' );

    Data packet1;
    packet1.push_back( 'A' );
    packet1.push_back( 'B' );
    packet1.push_back( 'C' );
    packet1.push_back( 'D' );
    packet1.push_back( 'E' );
    packet1.push_back( 'F' );
    packet1.push_back( 'G' );
    packet1.push_back( 'H' );
    packet1.push_back( 'I' );
    packet1.push_back( 'J' );
    packet1.push_back( 'K' );
    packet1.push_back( 'L' );
    packet1.push_back( 'M' );
    packet1.push_back( 'N' );
    packet1.push_back( 'O' );
    packet1.push_back( 'P' );
    packet1.push_back( 'Q' );
    packet1.push_back( 'R' );
    Data packet2;
    packet2.push_back( 'S' );
    packet2.push_back( 'T' );
    packet2.push_back( 'U' );
    packet2.push_back( 'V' );
    packet2.push_back( 'W' );
    packet2.push_back( 'X' );
    packet2.push_back( 'Y' );
    packet2.push_back( 'Z' );
    packet2.push_back( '0' );
    packet2.push_back( '1' );
    packet2.push_back( '2' );
    packet2.push_back( '3' );
    packet2.push_back( '4' );
    packet2.push_back( '5' );
    packet2.push_back( '6' );
    packet2.push_back( '7' );
    packet2.push_back( '8' );
    packet2.push_back( '9' );

    std::string call1 = "12345678910";
    std::string call2 = "10987654321";

    DataManager::getDataManager().setLastExpectedPacket( call1,packet1 );
    DataManager::getDataManager().setLastExpectedPacket( call2,packet1 );

    rcmgr->addRadioCircuitTransfer( completeData, call1 );
    rcmgr->addRadioCircuitTransfer( completeData, call2 );

    TA_Base_Core::Thread::sleep( 5000 );

    Assert( true == DataManager::getDataManager().didPacketsMatch(call1) );
    Assert( true == DataManager::getDataManager().didPacketsMatch(call2) );

    DataManager::getDataManager().setLastExpectedPacket( call1,packet2 );
    DataManager::getDataManager().setLastExpectedPacket( call2,packet2 );

    // Need to send a signal to the Radio to signal that another packet can be sent.
    rcmgr->processIncomingCircuitModeDataCapacity( DataManager::getDataManager().getCallReference( call1 ).m_radioReference );
    rcmgr->processIncomingCircuitModeDataCapacity( DataManager::getDataManager().getCallReference( call2 ).m_radioReference );

    TA_Base_Core::Thread::sleep( 5000 );

    Assert( true == DataManager::getDataManager().didPacketsMatch( call1 ) );
    Assert( true == DataManager::getDataManager().didPacketsMatch( call2 ) );
    
    // Need to send a signal to the Radio to signal that another packet can be sent.
    rcmgr->processIncomingCircuitModeDataCapacity( DataManager::getDataManager().getCallReference( call1 ).m_radioReference );
    rcmgr->processIncomingCircuitModeDataCapacity( DataManager::getDataManager().getCallReference( call2 ).m_radioReference );
    std::cout << "testAddRadioCircuitModeTransfer_TwoTransfers_TwoPackets\n";
}


void RadioCircuitModeManagerUnitTest::testAddRadioCircuitModeTransfer_TwoTransfers_TwoPackets_XOFF()
{
    std::cout << "testAddRadioCircuitModeTransfer_TwoTransfers_TwoPackets_XOFF\n";
    RadioCallback callback;
    TA_IRS_App::Radio radio( callback );

    TA_IRS_App::RadioCircuitModeManager* rcmgr = radio.getRadioCircuitModeManager();

    // Create the packet.
    Data completeData;
    completeData.push_back( 'A' );
    completeData.push_back( 'B' );
    completeData.push_back( 'C' );
    completeData.push_back( 'D' );
    completeData.push_back( 'E' );
    completeData.push_back( 'F' );
    completeData.push_back( 'G' );
    completeData.push_back( 'H' );
    completeData.push_back( 'I' );
    completeData.push_back( 'J' );
    completeData.push_back( 'K' );
    completeData.push_back( 'L' );
    completeData.push_back( 'M' );
    completeData.push_back( 'N' );
    completeData.push_back( 'O' );
    completeData.push_back( 'P' );
    completeData.push_back( 'Q' );
    completeData.push_back( 'R' );
    completeData.push_back( 'S' );
    completeData.push_back( 'T' );
    completeData.push_back( 'U' );
    completeData.push_back( 'V' );
    completeData.push_back( 'W' );
    completeData.push_back( 'X' );
    completeData.push_back( 'Y' );
    completeData.push_back( 'Z' );
    completeData.push_back( '0' );
    completeData.push_back( '1' );
    completeData.push_back( '2' );
    completeData.push_back( '3' );
    completeData.push_back( '4' );
    completeData.push_back( '5' );
    completeData.push_back( '6' );
    completeData.push_back( '7' );
    completeData.push_back( '8' );
    completeData.push_back( '9' );

    Data packet1;
    packet1.push_back( 'A' );
    packet1.push_back( 'B' );
    packet1.push_back( 'C' );
    packet1.push_back( 'D' );
    packet1.push_back( 'E' );
    packet1.push_back( 'F' );
    packet1.push_back( 'G' );
    packet1.push_back( 'H' );
    packet1.push_back( 'I' );
    packet1.push_back( 'J' );
    packet1.push_back( 'K' );
    packet1.push_back( 'L' );
    packet1.push_back( 'M' );
    packet1.push_back( 'N' );
    packet1.push_back( 'O' );
    packet1.push_back( 'P' );
    packet1.push_back( 'Q' );
    packet1.push_back( 'R' );
    Data packet2;
    packet2.push_back( 'S' );
    packet2.push_back( 'T' );
    packet2.push_back( 'U' );
    packet2.push_back( 'V' );
    packet2.push_back( 'W' );
    packet2.push_back( 'X' );
    packet2.push_back( 'Y' );
    packet2.push_back( 'Z' );
    packet2.push_back( '0' );
    packet2.push_back( '1' );
    packet2.push_back( '2' );
    packet2.push_back( '3' );
    packet2.push_back( '4' );
    packet2.push_back( '5' );
    packet2.push_back( '6' );
    packet2.push_back( '7' );
    packet2.push_back( '8' );
    packet2.push_back( '9' );

    std::string call1 = "12345678910";
    std::string call2 = "10987654321";

    rcmgr->addRadioCircuitTransfer( completeData, call1 );
    rcmgr->addRadioCircuitTransfer( completeData, call2 );


    DataManager::getDataManager().setLastExpectedPacket( call1,packet1 );
    DataManager::getDataManager().setLastExpectedPacket( call2,packet1 );

    TA_Base_Core::Thread::sleep( 2000 );

    Assert( true == DataManager::getDataManager().didPacketsMatch(call1) );
    Assert( true == DataManager::getDataManager().didPacketsMatch(call2) );

    DataManager::getDataManager().setLastExpectedPacket( call1,packet2 );
    DataManager::getDataManager().setLastExpectedPacket( call2,packet2 );

    // Send the task an XOFF signal
    rcmgr->processIncomingCircuitModeData( DataManager::getDataManager().getCallReference( call1 ).m_radioReference, TA_IRS_App::XOFF );

    // Need to send a signal to the Radio to signal that another packet can be sent.
    rcmgr->processIncomingCircuitModeDataCapacity( DataManager::getDataManager().getCallReference( call1 ).m_radioReference );
    rcmgr->processIncomingCircuitModeDataCapacity( DataManager::getDataManager().getCallReference( call2 ).m_radioReference );

    TA_Base_Core::Thread::sleep( 3000 );

    Assert( false == DataManager::getDataManager().didPacketsMatch(call1) );
    Assert( true == DataManager::getDataManager().didPacketsMatch(call2) );

    // Send the task an XON signal
    rcmgr->processIncomingCircuitModeData( DataManager::getDataManager().getCallReference(call1).m_radioReference, TA_IRS_App::XON );
    
    TA_Base_Core::Thread::sleep( 2000 );

    // Need to send a signal to the Radio to signal that another packet can be sent.
    rcmgr->processIncomingCircuitModeDataCapacity( DataManager::getDataManager().getCallReference(call1).m_radioReference );
    rcmgr->processIncomingCircuitModeDataCapacity( DataManager::getDataManager().getCallReference(call2).m_radioReference );

    TA_Base_Core::Thread::sleep( 2000 );

    Assert( true == DataManager::getDataManager().didPacketsMatch(call1) );

    // Need to send a signal to the Radio to signal that another packet can be sent.
    rcmgr->processIncomingCircuitModeDataCapacity( DataManager::getDataManager().getCallReference(call1).m_radioReference );
    std::cout << "testAddRadioCircuitModeTransfer_TwoTransfers_TwoPackets_XOFF\n";
}

void RadioCircuitModeManagerUnitTest::testAddRadioCircuitModeTransfer_OnePacket_Padding()
{
    std::cout << "testAddRadioCircuitModeTransfer_OnePacket_Padding\n";
    RadioCallback callback;
    TA_IRS_App::Radio radio( callback );

    TA_IRS_App::RadioCircuitModeManager* rcmgr = radio.getRadioCircuitModeManager();

    // Create the packet.
    Data data;
    data.push_back( 'A' );
    data.push_back( 'B' );
    data.push_back( 'C' );
    data.push_back( 'D' );
    data.push_back( 'E' );
    data.push_back( 'F' );
    data.push_back( 'G' );
    data.push_back( 'H' );
    data.push_back( 'I' );
    data.push_back( 'J' );
    data.push_back( 'K' );
    data.push_back( 'L' );
    data.push_back( 'M' );
    data.push_back( 'N' );
    data.push_back( 'O' );
    data.push_back( 'P' );
    data.push_back( 'Q' );
    Data expected;
    expected.push_back( 'A' );
    expected.push_back( 'B' );
    expected.push_back( 'C' );
    expected.push_back( 'D' );
    expected.push_back( 'E' );
    expected.push_back( 'F' );
    expected.push_back( 'G' );
    expected.push_back( 'H' );
    expected.push_back( 'I' );
    expected.push_back( 'J' );
    expected.push_back( 'K' );
    expected.push_back( 'L' );
    expected.push_back( 'M' );
    expected.push_back( 'N' );
    expected.push_back( 'O' );
    expected.push_back( 'P' );
    expected.push_back( 'Q' );
    expected.push_back( '\0' ); // The important bit

    DataManager::getDataManager().setLastExpectedPacket( "12345678910",expected );

    rcmgr->addRadioCircuitTransfer( data, "12345678910" );

    TA_Base_Core::Thread::sleep( 2000 );

    Assert( true == DataManager::getDataManager().didPacketsMatch("12345678910") );
    
    // Need to send a signal to the Radio to signal that another packet can be sent.
    rcmgr->processIncomingCircuitModeDataCapacity( DataManager::getDataManager().getCallReference( "12345678910" ).m_radioReference );
    std::cout << "testAddRadioCircuitModeTransfer_OnePacket_Padding\n";
}


void RadioCircuitModeManagerUnitTest::testRemoveRadioCircuitModeTransfer()
{
    std::cout << "testRemoveRadioCircuitModeTransfer\n";
    RadioCallback callback;
    TA_IRS_App::Radio radio( callback );

    TA_IRS_App::RadioCircuitModeManager* rcmgr = radio.getRadioCircuitModeManager();

    // Create the packet.
    Data completeData;
    completeData.push_back( 'A' );
    completeData.push_back( 'B' );
    completeData.push_back( 'C' );
    completeData.push_back( 'D' );
    completeData.push_back( 'E' );
    completeData.push_back( 'F' );
    completeData.push_back( 'G' );
    completeData.push_back( 'H' );
    completeData.push_back( 'I' );
    completeData.push_back( 'J' );
    completeData.push_back( 'K' );
    completeData.push_back( 'L' );
    completeData.push_back( 'M' );
    completeData.push_back( 'N' );
    completeData.push_back( 'O' );
    completeData.push_back( 'P' );
    completeData.push_back( 'Q' );
    completeData.push_back( 'R' );
    completeData.push_back( 'S' );
    completeData.push_back( 'T' );
    completeData.push_back( 'U' );
    completeData.push_back( 'V' );
    completeData.push_back( 'W' );
    completeData.push_back( 'X' );
    completeData.push_back( 'Y' );
    completeData.push_back( 'Z' );
    completeData.push_back( '0' );
    completeData.push_back( '1' );
    completeData.push_back( '2' );
    completeData.push_back( '3' );
    completeData.push_back( '4' );
    completeData.push_back( '5' );
    completeData.push_back( '6' );
    completeData.push_back( '7' );
    completeData.push_back( '8' );
    completeData.push_back( '9' );
    completeData.push_back( 'A' );
    completeData.push_back( 'B' );
    completeData.push_back( 'C' );
    completeData.push_back( 'D' );
    completeData.push_back( 'E' );
    completeData.push_back( 'F' );
    completeData.push_back( 'G' );
    completeData.push_back( 'H' );
    completeData.push_back( 'I' );
    completeData.push_back( 'J' );
    completeData.push_back( 'K' );
    completeData.push_back( 'L' );
    completeData.push_back( 'M' );
    completeData.push_back( 'N' );
    completeData.push_back( 'O' );
    completeData.push_back( 'P' );
    completeData.push_back( 'Q' );
    completeData.push_back( 'R' );

    Data expected;
    expected.push_back( '\0' );
    expected.push_back( '\0' );
    expected.push_back( '\0' );
    expected.push_back( '\0' );
    expected.push_back( '\0' );
    expected.push_back( '\0' );
    expected.push_back( '\0' );
    expected.push_back( '\0' );
    expected.push_back( '\0' );
    expected.push_back( '\0' );
    expected.push_back( '\0' );
    expected.push_back( '\0' );
    expected.push_back( '\0' );
    expected.push_back( '\0' );
    expected.push_back( '\0' );
    expected.push_back( '\0' );
    expected.push_back( '\0' );
    expected.push_back( '\0' );

    std::string call( "12345678910" );

    rcmgr->addRadioCircuitTransfer( completeData, call );

    DataManager::getDataManager().setLastExpectedPacket( call, expected );

    TA_Base_Core::Thread::sleep( 2000 );

    Assert( false == DataManager::getDataManager().didPacketsMatch(call) );

    rcmgr->removeRadioCircuitTransfer( call, true );

    TA_Base_Core::Thread::sleep( 2000 );

    Assert( true == DataManager::getDataManager().didPacketsMatch(call) );

    std::cout << "testRemoveRadioCircuitModeTransfer\n";
}



////////////////////////////////
// Inherited TestCase Methods //
////////////////////////////////

void RadioCircuitModeManagerUnitTest::setUp( const std::string & INFile , const std::string & OUTFile )
{

}


void RadioCircuitModeManagerUnitTest::tearDown( const std::string & INFile )
{
}


void RadioCircuitModeManagerUnitTest::setUpSuite( const std::string & inFile )
{
}


void RadioCircuitModeManagerUnitTest::tearDownSuite()
{
}


TestSuite * RadioCircuitModeManagerUnitTest::suite ()
{
    TestSuite * testSuite = new TestSuite("RadioCircuitModeManagerUnitTest");

	//////////////////////
	// Test Constructor //
	//////////////////////

    testSuite->addTest(new TestCaller< RadioCircuitModeManagerUnitTest >("testAddRadioCircuitModeTransfer_OnePacket",&RadioCircuitModeManagerUnitTest::testAddRadioCircuitModeTransfer_OnePacket,this));
    testSuite->addTest(new TestCaller< RadioCircuitModeManagerUnitTest >("testAddRadioCircuitModeTransfer_TwoPackets",&RadioCircuitModeManagerUnitTest::testAddRadioCircuitModeTransfer_TwoPackets,this));
    testSuite->addTest(new TestCaller< RadioCircuitModeManagerUnitTest >("testAddRadioCircuitModeTransfer_TwoPackets_XOFF",&RadioCircuitModeManagerUnitTest::testAddRadioCircuitModeTransfer_TwoPackets_XOFF,this));
    testSuite->addTest(new TestCaller< RadioCircuitModeManagerUnitTest >("testAddRadioCircuitModeTransfer_TwoTransfers_TwoPackets",&RadioCircuitModeManagerUnitTest::testAddRadioCircuitModeTransfer_TwoTransfers_TwoPackets,this));
    testSuite->addTest(new TestCaller< RadioCircuitModeManagerUnitTest >("testAddRadioCircuitModeTransfer_TwoTransfers_TwoPackets_XOFF",&RadioCircuitModeManagerUnitTest::testAddRadioCircuitModeTransfer_TwoTransfers_TwoPackets_XOFF,this));
    testSuite->addTest(new TestCaller< RadioCircuitModeManagerUnitTest >("testAddRadioCircuitModeTransfer_OnePacket_Padding",&RadioCircuitModeManagerUnitTest::testAddRadioCircuitModeTransfer_OnePacket_Padding,this));
    testSuite->addTest(new TestCaller< RadioCircuitModeManagerUnitTest >("testRemoveRadioCircuitModeTransfer",&RadioCircuitModeManagerUnitTest::testRemoveRadioCircuitModeTransfer,this));

    return testSuite;
}
