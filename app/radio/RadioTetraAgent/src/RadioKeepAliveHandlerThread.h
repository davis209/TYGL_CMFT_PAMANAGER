// RadioKeepAliveHandlerThread.h: interface for the RadioKeepAliveHandlerThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RADIOKEEPALIVEHANDLERTHREAD_H__FC693178_5B92_4045_9003_934B820311B4__INCLUDED_)
#define AFX_RADIOKEEPALIVEHANDLERTHREAD_H__FC693178_5B92_4045_9003_934B820311B4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/types/src/ta_types.h"
#include "app/radio/RadioTetraAgent/RadioProtocolLibrary/src/KeepAliveEventReader.h"
#include "app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodCheckConnection.h"

#include "core/threads/src/Thread.h"
#include "core/timers/src/TimedWaitSemaphore.h"

namespace TA_IRS_App
{
	class RadioTCPConnection;

	class RadioKeepAliveHandlerThread : public TA_Base_Core::Thread
	{
	public:

		RadioKeepAliveHandlerThread( RadioTCPConnection * tcpConnection, ta_uint32 interval = 3, ta_uint32 expireTime = 10 );

		virtual ~RadioKeepAliveHandlerThread();

		virtual void run();
		virtual void terminate();

		void startCheckConnection(bool bForceRestart = false);
		void stopCheckConnection();

		void setInterval(ta_uint32 interval );

		void setExpirePeriod(ta_uint32 expireTime );

		bool processEvent( TA_IRS_App::RadioEvent event );

	private:

		enum requestedState
		{
			NotRequeseted,
			RequestStartCheck,
			RequestStopCheck,
			KeepAliveStarted,
			KeepAliveStopped
		};

		bool								m_terminate;
		requestedState						m_checkConnProcessState;
		RadioTCPConnection *				m_tcpConnection;
		ta_uint32 						m_interval;
		ta_uint32 						m_expireTime;
		TA_Base_Core::TimedWaitSemaphore	m_semaphore;
		time_t								m_checkConnTimeStart;
		ta_uint16							m_maxRetry;
		ta_uint16							m_retryCount;
	};
};

#endif // !defined(AFX_RADIOKEEPALIVEHANDLERTHREAD_H__FC693178_5B92_4045_9003_934B820311B4__INCLUDED_)
