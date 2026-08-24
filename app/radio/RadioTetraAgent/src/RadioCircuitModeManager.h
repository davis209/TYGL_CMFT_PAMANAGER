/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/RadioCircuitModeManager.h $
  * @author  A. Radics
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2008/11/28 16:26:01 $
  * Last modified by : $Author: builder $
  *
  * Radio Circuit Mode Manager VII.
  * No buggering around this time.
  * A simple container, queues and holds onto transfers.
  */

#ifndef RADIO_CIRCUITMODE_MANAGER_H
#define RADIO_CIRCUITMODE_MANAGER_H

#include "core/types/src/ta_types.h"
#include "app/radio/RadioTetraAgent/src/RadioCircuitTransfer.h"

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/Semaphore.h"
#include "core/exceptions/src/TransactiveException.h"

#include <map>
#include <queue>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



namespace TA_IRS_App
{

    class RadioCircuitModeException : public TA_Base_Core::TransactiveException
	{
		public:
			RadioCircuitModeException(const std::string& errorText) throw();
			virtual ~RadioCircuitModeException() throw();
	};

	class RadioCircuitModeManager : public ICircuitModeManager,
                                    public TA_Base_Core::Thread
	{

	public:

        RadioCircuitModeManager( Radio& radio );
        
        ~RadioCircuitModeManager();

        void setParameters(ta_uint32 xonXoffTimeout,
                            ta_uint32 circuitDataFramesPerPacket );

        /**
         * Adds a Radio Circuit Transfer that is to be processed.
         *
         * @exception RadioCircuitModeException if a transfer exists for this destination
         */
        void addRadioCircuitTransfer( const RadioCircuitTransfer::Data& data,
                                      const std::string& destinationTsi,
									  const std::string& destIPAddress);

        /** 
         * Removes a Radio Circuit Transfer that was to be processed.
         *
         * @exception RadioCircuitModeException if a transfer doesnt exist for this destination
         */
        void removeRadioCircuitTransfer( const std::string& destinationTsi );

        /**
         * Processes an incoming RadioCircuitDataCapacity event.  This will
         * contain flow control for a Task.
         */
        void processIncomingCircuitModeDataCapacity(
            RadioReference callReference,
            ta_int32 freeBytes);

        /**
         * Processes an incoming RadioCircuitData event.  This will contain 
         * XON/XOFF flow control details for a Task.
         */
        void processIncomingCircuitModeData(
            RadioReference callReference,
            RadioCircuitTransfer::EXStatus status );

        /**
         * Processes an incoming disconnection event.
         */
        void processCircuitModeDisconnection(
            RadioReference callReference );

        /** allows us to receive an unsolicited disconnect properly if 
         *  a callRef is for one of our circuit transfers
         *  @param  callRef the reference of the call to check
         *  @param  tsi     an output parameter, populated with the tsi of the
         *                  call, to be used with removeRadioCctXfer
         *  @return true    if the callRef corresponds to a ciruit transfer,
         *                  false otherwise
         */
        bool isRadioCircuitTransfer(ta_uint32 callRef, std::string & tsi );

        /**
         * Queued adding and removal of transfers
         */
        virtual void run();

        /**
         * Terminate this thread and all transfers
         */
        virtual void terminate();

        /**
         * Transfers signal when theyre done
         */
        virtual void transferComplete( const std::string& transferTsi );

        virtual void setCallReference(RadioCircuitTransfer* startedTransfer);
        virtual void clearCallReference(RadioReference oldCallReference);
        
        virtual void sendTransferResult(const std::string& tsi, bool success, const std::string& failReason);

	private:

        Radio&                              m_radio;
        
        // The radio circuit transfers in progress
        std::map< std::string,
                  RadioCircuitTransfer* >   m_transferListByTsi;
        std::map< RadioReference,
                  RadioCircuitTransfer*>    m_transferListByCallId;
		
        TA_Base_Core::ReEntrantThreadLockable m_transferListLock;

        // This is a list of finished transfers to remove from the list
        std::queue< std::string > m_removeList;
        TA_Base_Core::ReEntrantThreadLockable m_removeListLock;

        TA_Base_Core::Semaphore             m_semaphore;
        bool                                m_running;

        /**
         * The XON/XOFF timeout value.  This is configurable through the
         * ConfigurationEditor.
         */
        ta_uint32                        m_XONXOFFTimeout;
        ta_uint32                        m_circuitDataFramesPerPacket;
	};

}

#endif
