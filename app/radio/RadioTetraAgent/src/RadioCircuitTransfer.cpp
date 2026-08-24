/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/RadioCircuitTransfer.cpp $
  * @author  A. Radics
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2008/11/28 16:26:01 $
  * Last modified by : $Author: builder $
  *
  * 
  */

#include "core/types/src/ta_types.h"
#include <ace/Time_Value.h>
#include <ace/OS.h>

#include "app/radio/RadioTetraAgent/src/RadioCircuitTransfer.h"
#include "app/radio/RadioTetraAgent/src/Radio.h"
#include "app/radio/RadioTetraAgent/src/RadioSession.h"
#include "app/radio/RadioTetraAgent/src/RadioSessionException.h"

#include "app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodSendCircuitData.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App
{


    RadioCircuitTransfer::RadioCircuitTransfer( ICircuitModeManager* circuitModeManager,
                                                Radio& radio,
                                                const Data& data,
                                                const std::string& destinationTsi,
												const std::string& destIPAddress,
                                                ta_uint32 xonXoffTimeout,
                                                ta_uint32 circuitDataFramesPerPacket )
    : m_circuitModeManager(circuitModeManager),
      m_radio(radio),
      m_destinationTsi(destinationTsi),
	  m_destIPAddress(destIPAddress),
      m_data(data),
      m_XONXOFFTimeout( xonXoffTimeout ),
      m_circuitDataFramesPerPacket( circuitDataFramesPerPacket ),
      m_flowControl(XON),
      m_incomingDataCapacityReceived(false),
      m_cancelled(false),
      m_disconnected(false),
      m_inProgress(false),
      m_success(true),
      m_failReason(""),
      m_semaphore(0),
      m_callReference(0)
    {
        LOG_GENERIC( SourceInfo,
                 TA_Base_Core::DebugUtil::DebugInfo,
				 "New circuit mode transfer for destination: %s, IPAddress: %s",
                 destinationTsi.c_str(),destIPAddress.c_str() );
    }

    RadioCircuitTransfer::~RadioCircuitTransfer()
    {
        LOG_GENERIC( SourceInfo,
             TA_Base_Core::DebugUtil::DebugDebug,
             "Deleted circuit mode transfer for destination: %s, IPAddress: %s",
             m_destinationTsi.c_str(), m_destIPAddress.c_str() );
		cancelTransfer();
    }

    void RadioCircuitTransfer::setFlowControl( EXStatus xStatus )
    {
        LOG_GENERIC( SourceInfo,
             TA_Base_Core::DebugUtil::DebugInfo,
             "Flow control for circuit mode transfer for destination: %s set to %s from %s",
             m_destinationTsi.c_str(),
             ( XON == xStatus ) ? "XON" : "XOFF",
             ( XON == m_flowControl ) ? "XON" : "XOFF");
             
        TA_Base_Core::ThreadGuard guard(m_eventLock);
        
        // only has any effect if it changed
        if ( m_flowControl != xStatus )
        {
            m_flowControl = xStatus;
            
            // the thread can continue if flow control has been turned on
            // and there is capacity
            if ( ( true == m_incomingDataCapacityReceived ) &&
                 ( XON == m_flowControl ) )
            {
                m_semaphore.post();
            }
        }
    }
    
    void RadioCircuitTransfer::receiveIncomingDataCapacity(ta_int32 capacity)
    {
        LOG_GENERIC( SourceInfo,
             TA_Base_Core::DebugUtil::DebugInfo,
             "Capacity event received for circuit mode transfer for destination: %s capacity is %d",
             m_destinationTsi.c_str(),
             capacity );

        TA_Base_Core::ThreadGuard guard(m_eventLock);
        
        bool hadCapacity = m_incomingDataCapacityReceived;
        m_incomingDataCapacityReceived = (capacity > 180); /*( RadioMethodSendCircuitData::SENDCIRCUITDATA_PACKET_SIZE * m_circuitDataFramesPerPacket ) );*/
        
        // the thread can continue if flow control is on and there is capacity
        // but protect against signalling the semaphore twice in case of multiple capacity events
        if ( ( XON == m_flowControl ) &&
             ( true == m_incomingDataCapacityReceived ) &&
             ( false == hadCapacity ) )
        {
            m_semaphore.post();
        }
    }
    
    void RadioCircuitTransfer::cancelTransfer()
    {
        // just in case the transfer has completed and just needs to be closed
        if ( false == m_inProgress )
        {
            closeTransfer();
        }
        
        // this caters for the case that the transfer is still running
        m_cancelled = true;
        m_semaphore.post();
    }
    
    void RadioCircuitTransfer::terminate()
    {
        cancelTransfer();
    }
    
    void RadioCircuitTransfer::callDisconnected()
    {
        m_disconnected = true;
        m_semaphore.post();
    }
    
    std::string RadioCircuitTransfer::getDestinationTsi()
    {
        return m_destinationTsi;
    }
    
    RadioReference RadioCircuitTransfer::getCallReference()
    {
        return m_callReference;
    }

    bool RadioCircuitTransfer::isRunning()
    {
        return m_inProgress;
    }

    bool RadioCircuitTransfer::wasSuccessful()
    {
        return m_success;
    }

    std::string RadioCircuitTransfer::getFailureReason()
    {
        return m_failReason;
    }
    
    void RadioCircuitTransfer::run()
    {
        m_inProgress = true;
        
        try
        {
            // frame the data
            std::vector<Data> frames = frameData();
            unsigned int currentFrame = 0;
            unsigned int totalFrames = frames.size();
            
			TA_Base_Core::Thread::sleep(2000);

            // setup the call
            m_callReference = m_radio.setupDataCall(m_destinationTsi, m_destIPAddress);
            m_circuitModeManager->setCallReference( this );
            
            while (currentFrame < totalFrames)
            {
                {
                    TA_Base_Core::ThreadGuard guard( m_eventLock );
                    
                    // another capacity event is needed
                    m_incomingDataCapacityReceived = false;
                }
                    
                // send currentFrame
                m_radio.sendCircuitDataFrame( m_callReference, frames[currentFrame] );
                
                // increment frame counter
                currentFrame++;

                // wait for the semaphore to be signalled or expire
                // receiving an XON, XOFF, or capacity event will allow this to progress
                timedWait();
                
                // check for a cancelled call
                if ( true == m_cancelled )
                {
                    LOG_GENERIC( SourceInfo,
                         TA_Base_Core::DebugUtil::DebugError,
                         "Cancelled circuit mode transfer for destination: %s. %d of %d packets sent",
                         m_destinationTsi.c_str(),
                         currentFrame-1, totalFrames );
                     
                    m_success = false;
                    m_failReason = "Transfer Cancelled";
                    break;
                }
                
                // check for unsolicited disconnection
                if ( true == m_disconnected )
                {
                    LOG_GENERIC( SourceInfo,
                         TA_Base_Core::DebugUtil::DebugError,
                         "Circuit mode transfer for destination: %s has been disconnected. %d of %d packets sent",
                         m_destinationTsi.c_str(),
                         currentFrame-1, totalFrames );
                         
                    m_success = false;
                    m_failReason = "Transfer Disconnected";
                    
                    // no need to close call, just delete reference
                    try
                    {
                        m_radio.clearDataCallReference( m_callReference );
                    }
                    catch (...)
                    {
                        // do not care about the error, the real error is the disconnection
                        // this is only done to prevent resource leaks in radio
                    }
                    m_circuitModeManager->clearCallReference( m_callReference );
                    m_callReference = 0;
                    break;
                }
                
                // check flow control and capacity event status
                EXStatus flowControl;
                bool incomingDataCapacityReceived;
                {
                    TA_Base_Core::ThreadGuard guard( m_eventLock );
                    flowControl = m_flowControl;
                    incomingDataCapacityReceived = m_incomingDataCapacityReceived;
                }
                    
                if ( false == incomingDataCapacityReceived )
                {
                    LOG_GENERIC( SourceInfo,
                                 TA_Base_Core::DebugUtil::DebugError,
                                 "Circuit mode transfer for destination: %s has not received a positive capacity event. %d of %d packets sent",
                                 m_destinationTsi.c_str(),
                                 currentFrame-1, totalFrames );
                    
                    // transfer failed - timeout
//                     m_success = false;
//                     m_failReason = "Transfer Timeout";
//                     break;
                }
                
                // check for flow control
                if ( XOFF == flowControl )
                {
                    // cant send until XON
                    // ensure that we wait the whole timeout
                    // as it might be the case the XOFF was received not
                    // ta_int32 before the semaphore expired
                    timedWait();
                    
                    TA_Base_Core::ThreadGuard guard( m_eventLock );
                    
                    // still off?
                    if ( XOFF == m_flowControl )
                    {
                        LOG_GENERIC( SourceInfo,
                             TA_Base_Core::DebugUtil::DebugError,
                             "Circuit mode transfer for destination: %s has had flow control off for too long. %d of %d packets sent",
                             m_destinationTsi.c_str(),
                             currentFrame-1, totalFrames );
                         
                        // transfer failed due to XOFF timeout
                        m_success = false;
                        m_failReason = "XON/XOFF Timeout";
                        break;
                    }
                }
            }
        }
        // any exceptions raise error
        catch (RadioSessionException& rse)
        {
            m_success = false;
            m_failReason = rse.what();
        }
        catch (TA_Base_Core::TransactiveException& te)
        {
            m_success = false;
            m_failReason = te.what();
        }
        catch( ... )
        {
            m_success = false;
            m_failReason = "Error during data download";
        }
        
        // send the notification of transfer complete to the clients
        m_circuitModeManager->sendTransferResult( m_destinationTsi, m_success, m_failReason );
        m_inProgress = false;
        
		Thread::sleep(3000); //Ensure all data is transfered
		closeTransfer();
        // if the transfer failed, or was cancelled, then the connection can be closed now.
        //if ( false == m_success )
        //{
        //    closeTransfer();
        //}

    }
    
    void RadioCircuitTransfer::timedWait()
    {
        m_semaphore.timedWait( m_XONXOFFTimeout * 1000 );
    }
    
    
    std::vector<RadioCircuitTransfer::Data> RadioCircuitTransfer::frameData()
    {
        std::vector<Data> packets;
        
        ta_uint32 packetSize = RadioMethodSendCircuitData::SENDCIRCUITDATA_MESSAGE_LENGTH * m_circuitDataFramesPerPacket;
        // how many packets needed to fit the data
        ta_uint32 totalPackets = m_data.size() / packetSize;
        if ( ( m_data.size() % packetSize ) != 0 )
        {
            ++totalPackets;
        }
        
        // for each packet
        for( unsigned int packetNo = 0; packetNo < totalPackets; ++packetNo )
        {
            Data packet;

            // calculate the offset in the data for the first byte of this packet            
            ta_uint32 fromOffset = packetSize * (packetNo);
            ta_uint32 toOffset = packetSize * (packetNo+1);
            if ( toOffset > m_data.size() )
            {
                // this is the last packet that needs padding
                toOffset = m_data.size();
            }

            for ( unsigned int dataIndex = fromOffset; dataIndex < toOffset; ++dataIndex)
            {
                packet.push_back( m_data[dataIndex] );
            }
            
            // pad the last packet
            if ( packet.size() < packetSize )
            {
                // find out how many padding characters need to be added
                ta_uint32 paddingCharacters = RadioMethodSendCircuitData::SENDCIRCUITDATA_MESSAGE_LENGTH -
                                                  ( packet.size() % RadioMethodSendCircuitData::SENDCIRCUITDATA_MESSAGE_LENGTH );
                
                for ( unsigned int padIndex = 0;
                      padIndex < paddingCharacters; ++padIndex )
                {
                    packet.push_back('\0');
                }
            }
            
            packets.push_back(packet);
        }

        return packets;
    }
    
    
    void RadioCircuitTransfer::closeTransfer()
    {
        // close the call if it is still open
        if (0 != m_callReference)
        {
            try
            {
                m_radio.endDataCall( m_callReference );
            }
            catch (RadioSessionException& rse)
            {
                m_success = false;
                // if it failed to end, dont overwrite the initial failur reason
                if (m_failReason == "")
                {
                    m_failReason = rse.what();
                }                
            }
            catch (TA_Base_Core::TransactiveException& te)
            {
                m_success = false;
                // if it failed to end, dont overwrite the initial failur reason
                if (m_failReason == "")
                {
                    m_failReason = te.what();
                }                
            }
            catch ( ... )
            {
                m_success = false;
                // if it failed to end, dont overwrite the initial failur reason
                if (m_failReason == "")
                {
                    m_failReason = "Error ending data download";
                }
            }
            
            m_circuitModeManager->clearCallReference( m_callReference );
            m_callReference = 0;
        }
        
        // signal CM Manager that it is safe to remove this transfer
        m_circuitModeManager->transferComplete( m_destinationTsi );
    }

}
