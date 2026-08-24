/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/RadioCircuitModeManager.cpp $
 * @author:  A. Radics
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 *
 * This class is responsible for managing circuit mode transfers
 */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "core/types/src/ta_types.h"
#include "app/radio/RadioTetraAgent/src/RadioCircuitModeManager.h"
#include "app/radio/RadioTetraAgent/src/Radio.h"
#include "app/radio/RadioTetraAgent/src/RadioSession.h"
#include "app/radio/RadioTetraAgent/src/IRadioCallback.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


namespace TA_IRS_App
{

    RadioCircuitModeException::RadioCircuitModeException(const std::string& errorText) throw()
					  : TA_Base_Core::TransactiveException(errorText)
    {}


    RadioCircuitModeException::~RadioCircuitModeException() throw()
    {}

    RadioCircuitModeManager::RadioCircuitModeManager( Radio& radio )
     : m_radio(radio),
       m_running(false),
       m_XONXOFFTimeout( 140 ),    // default for now - is overwritten in setEntity
       m_circuitDataFramesPerPacket( 1 )
    {
        FUNCTION_ENTRY("RadioCircuitModeManager()");
        FUNCTION_EXIT;
    }
        
    RadioCircuitModeManager::~RadioCircuitModeManager()
    {
        FUNCTION_ENTRY("~RadioCircuitModeManager()");
        
        // delete everything
        // everything should be stopped
        TA_ASSERT(m_running == false, "Cannot stop this without shutting it down");

        // there should be no need to delete anything
        // it should be all cleaned up
        TA_ASSERT(m_transferListByTsi.size() == 0, "The thread has been shut down but transfers remain");
        TA_ASSERT(m_transferListByCallId.size() == 0, "The thread has been shut down but transfers remain");
        TA_ASSERT(m_removeList.size() == 0, "The thread has been shut down but transfers remain to be removed");

        FUNCTION_EXIT;
    }

    void RadioCircuitModeManager::setParameters(ta_uint32 xonXoffTimeout,
                                                 ta_uint32 circuitDataFramesPerPacket )
    {
        m_XONXOFFTimeout = xonXoffTimeout;
        m_circuitDataFramesPerPacket = circuitDataFramesPerPacket;
    }


    void RadioCircuitModeManager::terminate()
    {
        FUNCTION_ENTRY("terminate()");
        
        // cancel all transfers
        // let them shut down cleanly
        {
            TA_THREADGUARD(m_transferListLock);
            for (std::map< std::string, RadioCircuitTransfer* >::iterator iter = m_transferListByTsi.begin();
            iter != m_transferListByTsi.end(); ++iter)
            {
                iter->second->cancelTransfer();
            }
        }

        m_running = false;
        m_semaphore.post();

        FUNCTION_EXIT;
    }


    void RadioCircuitModeManager::addRadioCircuitTransfer( 
        const RadioCircuitTransfer::Data& data,
        const std::string& destinationTsi,
		const std::string& destIPAddress)
    {
        FUNCTION_ENTRY("addRadioCircuitTransfer()");

        if (false == m_running)
        {
            // no more can be started
            LOG_GENERIC( SourceInfo,
                 TA_Base_Core::DebugUtil::DebugError,
                 "New circuit mode transfer for destination: %s rejected, agent starting up or shutting down",
                 destinationTsi.c_str() );

            TA_THROW(RadioCircuitModeException("Data transfer can not be started. Agent in startup or shutdown."));
        }

        // aquire lock
        TA_THREADGUARD(m_transferListLock);
        
        // check for existing transfers
        std::map< std::string, RadioCircuitTransfer* >::iterator findIter = m_transferListByTsi.find(destinationTsi);

        if (findIter == m_transferListByTsi.end())
        {
            // add transfer
            RadioCircuitTransfer* newTransfer =
                new RadioCircuitTransfer( this,
                                          m_radio,
                                          data,
                                          destinationTsi,
										  destIPAddress,
                                          m_XONXOFFTimeout,
                                          m_circuitDataFramesPerPacket );

            m_transferListByTsi[destinationTsi] = newTransfer;
            m_transferListByTsi[destinationTsi]->start();
        }
        else
        {
            // log an error and throw exception
            LOG_GENERIC( SourceInfo,
                 TA_Base_Core::DebugUtil::DebugError,
                 "New circuit mode transfer for destination: %s rejected, existing transfer already in progress",
                 destinationTsi.c_str() );

            TA_THROW(RadioCircuitModeException("Existing data transfer already in progress"));
        }

        FUNCTION_EXIT;
    }


    void RadioCircuitModeManager::removeRadioCircuitTransfer( const std::string& destinationTsi )
    {
        FUNCTION_ENTRY("removeRadioCircuitTransfer()");

        // aquire lock
        TA_THREADGUARD(m_transferListLock);
        
        // check for existing transfers
        std::map< std::string, RadioCircuitTransfer* >::iterator findIter = m_transferListByTsi.find(destinationTsi);

        if (findIter != m_transferListByTsi.end())
        {
            // cancel transfer
            findIter->second->cancelTransfer();
        }
        else
        {
            // log an error and throw exception
            LOG_GENERIC( SourceInfo,
                 TA_Base_Core::DebugUtil::DebugError,
                 "No circuit mode transfer for destination: %s found to cancel",
                 destinationTsi.c_str() );

            TA_THROW(RadioCircuitModeException("No data transfer in progress to cancel"));
        }

        FUNCTION_EXIT;
    }

    void RadioCircuitModeManager::processIncomingCircuitModeDataCapacity(
        RadioReference callReference,
        ta_int32 freeBytes)
    {
        FUNCTION_ENTRY("processIncomingCircuitModeDataCapacity()");

        // aquire lock
        TA_THREADGUARD(m_transferListLock);
        
        // check for existing transfers
        std::map< RadioReference, RadioCircuitTransfer* >::iterator findIter 
            = m_transferListByCallId.find( callReference );

        if (findIter != m_transferListByCallId.end())
        {
            findIter->second->receiveIncomingDataCapacity( freeBytes );
        }
        else
        {
            LOG_GENERIC( SourceInfo,
                 TA_Base_Core::DebugUtil::DebugError,
                 "No circuit mode transfer for call id: %d found to send capacity to",
                 callReference );
        }

        FUNCTION_EXIT;
    }


    void RadioCircuitModeManager::processIncomingCircuitModeData(
        RadioReference callReference,
        RadioCircuitTransfer::EXStatus status )
    {
        FUNCTION_ENTRY("processIncomingCircuitModeData()");

        // aquire lock
        TA_THREADGUARD(m_transferListLock);
        
        // check for existing transfers
        std::map< RadioReference, RadioCircuitTransfer* >::iterator findIter 
            = m_transferListByCallId.find( callReference );

        if (findIter != m_transferListByCallId.end())
        {
            findIter->second->setFlowControl( status );
        }
        else
        {
            LOG_GENERIC( SourceInfo,
                 TA_Base_Core::DebugUtil::DebugError,
                 "No circuit mode transfer for call id: %d found to send XON/XOFF to",
                 callReference );
        }

        FUNCTION_EXIT;
    }

    void RadioCircuitModeManager::processCircuitModeDisconnection(
            RadioReference callReference )
    {
        FUNCTION_ENTRY("processCircuitModeDisconnection()");

        // aquire lock
        TA_THREADGUARD(m_transferListLock);
        
        // check for existing transfers
        std::map< RadioReference, RadioCircuitTransfer* >::iterator findIter 
            = m_transferListByCallId.find( callReference );

        if (findIter != m_transferListByCallId.end())
        {
            findIter->second->callDisconnected();
        }
        else
        {
            LOG_GENERIC( SourceInfo,
                 TA_Base_Core::DebugUtil::DebugError,
                 "No circuit mode transfer for call id: %d found to send disconnection to",
                 callReference );
        }

        FUNCTION_EXIT;
    }

    bool RadioCircuitModeManager::isRadioCircuitTransfer(ta_uint32 callRef, std::string & tsi )
    {
        FUNCTION_ENTRY("isRadioCircuitTransfer()");

        // aquire lock
        TA_THREADGUARD(m_transferListLock);
        
        // check for existing transfers
        std::map< RadioReference, RadioCircuitTransfer* >::iterator findIter 
            = m_transferListByCallId.find( callRef );

        if (findIter != m_transferListByCallId.end())
        {
            tsi = findIter->second->getDestinationTsi();

            FUNCTION_EXIT;
            return true;
        }
        else
        {
            LOG_GENERIC( SourceInfo,
                 TA_Base_Core::DebugUtil::DebugDebug,
                 "No circuit mode transfer for call id: %d found. Not a circuit transfer.",
                 callRef );
        }

        FUNCTION_EXIT;
        return false;
    }

    void RadioCircuitModeManager::run()
    {
        FUNCTION_ENTRY("run()");

        m_running = true;

        // while not shutting down
        // also keep running while any transfers are closing down
        while (m_running || m_transferListByTsi.size() > 0)
        {
            // process those to be deleted
            {
                TA_THREADGUARD(m_removeListLock);

                while ( false == m_removeList.empty() )
                {
                // get the next one to remove
                    std::string transferTsi = m_removeList.front();
                m_removeList.pop();

                    RadioCircuitTransfer* transfer = NULL;

                    // find the transfer in the maps and remove it
                    {
                        TA_THREADGUARD(m_transferListLock);

                        RadioReference callRef = 0;
                    // delete from the TSI map (the master one)
                    {
                        // check for existing transfers
                            std::map< std::string, RadioCircuitTransfer* >::iterator findIter = m_transferListByTsi.find( transferTsi );

                            if ( m_transferListByTsi.end() != findIter )
                        {
                                // take the pointer
                                transfer = findIter->second;

                                // get the call ID
                                callRef = transfer->getCallReference();
                            // cancel transfer
                            m_transferListByTsi.erase( findIter );
                        }
                    }

                    // delete from the call ID map if it is in there
                        if ( 0 != callRef )
                    {
                        std::map< RadioReference, RadioCircuitTransfer* >::iterator findIter 
                            = m_transferListByCallId.find( callRef );

                            if ( m_transferListByCallId.end() != findIter )
                        {
                            m_transferListByCallId.erase( findIter );
                        }
                    }
                }

                // delete it, but ensure it is dead first
                    if ( NULL != transfer )
                    {
                transfer->terminateAndWait();
                delete transfer;
                        transfer = NULL;
                    }
                }
            }

            // wait on semaphore again
            m_semaphore.wait();
        }

        FUNCTION_EXIT;
    }


    void RadioCircuitModeManager::transferComplete( const std::string& transferTsi )
    {
        FUNCTION_ENTRY("transferComplete()");

        {
        TA_THREADGUARD(m_removeListLock);

        // queue the transfer for deleting
            m_removeList.push( transferTsi );
        }

        m_semaphore.post();

        FUNCTION_EXIT;
    }

    void RadioCircuitModeManager::setCallReference(RadioCircuitTransfer* startedTransfer)
    {
        FUNCTION_ENTRY("setCallReference()");
        
        TA_THREADGUARD(m_transferListLock);
        m_transferListByCallId[ startedTransfer->getCallReference() ] = startedTransfer;

        FUNCTION_EXIT;
    }

    void RadioCircuitModeManager::clearCallReference(RadioReference oldCallReference)
    {
        FUNCTION_ENTRY("clearCallReference()");

        // aquire lock
        TA_THREADGUARD(m_transferListLock);
        
        // check for existing transfers
        std::map< RadioReference, RadioCircuitTransfer* >::iterator findIter 
            = m_transferListByCallId.find( oldCallReference );

        if (findIter != m_transferListByCallId.end())
        {
            m_transferListByCallId.erase(findIter);
        }

        FUNCTION_EXIT;
    }
    
    
    void RadioCircuitModeManager::sendTransferResult(const std::string& tsi, bool success, const std::string& failReason)
    {
        FUNCTION_ENTRY("sendTransferResult()");
        
        // send success/failure
        m_radio.getRadioCallback().onCircuitModeActivity( tsi.c_str(),
                                                          success,
                                                          failReason.c_str() );
        
        FUNCTION_EXIT;
    }

}

