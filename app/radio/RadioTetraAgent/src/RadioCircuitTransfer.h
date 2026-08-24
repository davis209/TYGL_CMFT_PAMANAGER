/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/RadioCircuitTransfer.h $
  * @author  A. Radics
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2008/11/28 16:26:01 $
  * Last modified by : $Author: builder $
  *
  * 
  */

#ifndef RADIO_CIRCUIT_TRANSFER_H
#define RADIO_CIRCUIT_TRANSFER_H

#include "core/types/src/ta_types.h"
#include "app/radio/RadioTetraAgent/RadioProtocolLibrary/src/NewReferenceEventReader.h"

#include "core/timers/src/TimedWaitSemaphore.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/threads/src/Thread.h"


#include <string>
#include <vector>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace TA_IRS_App
{
    class RadioCircuitTransfer;
    class Radio;
    class RadioSession;

    class ICircuitModeManager
    {
    public:
        virtual void setCallReference(RadioCircuitTransfer* startedTransfer) = 0;
        virtual void clearCallReference(RadioReference oldCallReference) = 0;
        virtual void sendTransferResult(const std::string& tsi, bool success, const std::string& failReason) = 0;
        virtual void transferComplete( const std::string& transferTsi ) = 0;
    };

    /**
     * A RadioCircuitTransfer is a complete data transmission to the Radio
     * hardware.
     *
     * It is responsible for providing the data in discrete data packets for
     * transmission.
     */
    class RadioCircuitTransfer : public TA_Base_Core::Thread
    {
    public:
    
        typedef std::vector<unsigned char> Data;
    
        /** X flow control enumeration. */
        enum EXStatus
        {
            XON,
            XOFF
        };
        
        RadioCircuitTransfer( ICircuitModeManager* circuitModeManager,
                              Radio& radio,
                              const Data& data,
                              const std::string& destinationTsi,
							  const std::string& destIPAddress,
                              ta_uint32 xonXoffTimeout,
                              ta_uint32 circuitDataFramesPerPacket );

        /**
         * Destructs an instance of this class.
         */
        virtual ~RadioCircuitTransfer();

        /**
         * Sets the XON/XOFF flow control.
         */
        void setFlowControl( EXStatus xStatus );
        
        /**
         * Receives the IncomingDataCapacity event
         */
        void receiveIncomingDataCapacity(ta_int32 capacity);
        
        /**
         * Force the transfer to be stopped
         */
        void cancelTransfer();
        virtual void terminate();
        
        /**
         * Stop sending packets, the call was disconnected
         */
        void callDisconnected();
        
        /**
         * Gets the destination of the task.
         */
        std::string getDestinationTsi();
        
        /** 
         * Get the call reference
         */
        RadioReference getCallReference();

        /**
         * Returns true if stillin progress
         */
        bool isRunning();

        /**
         * Returns true if completed transfer finished successfully
         */
        bool wasSuccessful();

        /**
         * If the transfer failed, get the reason why, otherwise ""
         */
        std::string getFailureReason();
        
        virtual void run();

    private:
    
        void timedWait();
        
        std::vector<Data> frameData();
        
        void closeTransfer();


        ICircuitModeManager* m_circuitModeManager;
        Radio&          m_radio;
        std::string     m_destinationTsi;
		std::string		m_destIPAddress;
        Data            m_data;
        ta_uint32    m_XONXOFFTimeout;
        ta_uint32    m_circuitDataFramesPerPacket;
        
        EXStatus        m_flowControl;
        bool            m_incomingDataCapacityReceived;
        bool            m_cancelled;
        bool            m_disconnected;
        bool            m_inProgress;
        bool            m_success;
        std::string     m_failReason;
        
        TA_Base_Core::TimedWaitSemaphore m_semaphore;
        
        TA_Base_Core::ReEntrantThreadLockable   m_eventLock;

        
        RadioReference m_callReference;
    };
}

#endif
