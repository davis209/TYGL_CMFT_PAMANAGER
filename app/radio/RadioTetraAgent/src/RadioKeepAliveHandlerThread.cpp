// RadioKeepAliveHandlerThread.cpp: implementation of the RadioKeepAliveHandlerThread class.
//
//////////////////////////////////////////////////////////////////////

#include "core/types/src/ta_types.h"
#include "RadioTCPConnection.h"
#include "RadioKeepAliveHandlerThread.h"
#include "RadioAgentConstants.h"

#include "app/radio/RadioTetraAgent/RadioProtocolLibrary/src/KeepAliveEventReader.h"

#include "core/utilities/src/DebugUtil.h"
#include <sstream>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

using namespace TA_IRS_App;

RadioKeepAliveHandlerThread::RadioKeepAliveHandlerThread( TA_IRS_App::RadioTCPConnection * tcpConnection ,
														 ta_uint32 interval, ta_uint32 expireTime ):
m_terminate(false),
m_tcpConnection(tcpConnection),
m_interval(interval),
m_expireTime( expireTime ),
m_checkConnProcessState(NotRequeseted),
m_semaphore(0),
m_checkConnTimeStart(0),
m_maxRetry(1), // Defaulted to 1 retry only
m_retryCount(0)
{
	FUNCTION_ENTRY("RadioKeepAliveHandlerThread");
	start();
	FUNCTION_EXIT;
}

RadioKeepAliveHandlerThread::~RadioKeepAliveHandlerThread()
{
	FUNCTION_ENTRY("~RadioKeepAliveHandlerThread");

	delete m_tcpConnection;
	m_tcpConnection = NULL;

	m_terminate = true;

	FUNCTION_EXIT;
}

void RadioKeepAliveHandlerThread::run()
{
	FUNCTION_ENTRY("run");

	while( !m_terminate )
	{
		if ( m_checkConnProcessState == RequestStartCheck )
		{
			startCheckConnection();
		}
		
		LOG3(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "KeepAlive check IsPrimary: %d, CheckConnectionState = %d, ignore incoming Data = %d", m_tcpConnection->isPrimary(), m_checkConnProcessState, m_tcpConnection->getIgnoreIncomingData());
		if ( m_semaphore.timedWait( m_expireTime * 1000 ) )
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "Received Event, reset timer." );
		} 
		else
		{
			if ( (m_checkConnProcessState == KeepAliveStarted) && !m_tcpConnection->getIgnoreIncomingData() )
			{
				if (m_retryCount < m_maxRetry)
				{
					m_retryCount++;
					LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[KeepAlive] IsPrimary: %d, Timeout, reset tcp connection. retry attemp %d", m_retryCount);
					m_checkConnProcessState = NotRequeseted; 
					m_tcpConnection->resetConnection();
				}
				else
				{
					m_retryCount = 0;
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[KeepAlive] IsPrimary: %d, Timeout, inform connection manager.", m_tcpConnection->isPrimary());
					m_checkConnProcessState = NotRequeseted;
					m_tcpConnection->connectionLost();
				}

			}
			else
			{	
				if (m_checkConnProcessState == RequestStartCheck)
				{	
					// No response from Check connection.
					time_t timeCheckEnd = time(0);
					ta_uint32 timeDiff = timeCheckEnd - m_checkConnTimeStart;

					LOG5(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[CheckConnection] IsPrimary: %d, TimeStart: %d, TimeEnd: %d, TimeDiff: %d, ExpiredTime: %d",
						m_tcpConnection->isPrimary(), m_checkConnTimeStart, timeCheckEnd, timeDiff, m_expireTime);

					if (timeDiff >= m_expireTime)
					{// Timeout for Check Connection
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[CheckConnection] IsPrimary: %d, Timeout, inform connection manager.", m_tcpConnection->isPrimary());
						m_checkConnProcessState = NotRequeseted;
						m_tcpConnection->connectionLost();
					}
				}
				//m_semaphore.timedWait( m_expireTime * 1000 );
			}
		}
	}
	FUNCTION_EXIT;
}

void RadioKeepAliveHandlerThread::terminate()
{
	FUNCTION_ENTRY("terminate");

	m_terminate = true;
	
	m_semaphore.post();

	FUNCTION_EXIT;
}

void RadioKeepAliveHandlerThread::setInterval(ta_uint32 interval)
{
	m_interval = interval;
	m_checkConnProcessState = NotRequeseted;
}

void RadioKeepAliveHandlerThread::startCheckConnection(bool bForceRestart /*= false*/)
{
	FUNCTION_ENTRY("startCheckConnection");

	if (!bForceRestart && ( m_checkConnProcessState == RequestStartCheck ||
		m_checkConnProcessState == KeepAliveStarted ))
	{
		FUNCTION_EXIT;
		return;
	}

	RadioMethodCheckConnection methodCheckConnection;
	methodCheckConnection.enableCheckConnection(true);
	methodCheckConnection.setCheckInterval( m_interval );

	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "startCheckConnection" );

	m_checkConnProcessState = RequestStartCheck;

	m_tcpConnection->Send( methodCheckConnection, 0 );
	m_checkConnTimeStart = time(0);
	m_retryCount = 0; 

	FUNCTION_EXIT;
}

void RadioKeepAliveHandlerThread::stopCheckConnection() // Do not do anything now.
{
	FUNCTION_ENTRY("stopCheckConnection");

// 	if ( m_checkConnProcessState == RequestStopCheck ||
// 		m_checkConnProcessState == KeepAliveStopped )
// 	{
// 		FUNCTION_EXIT;
//		return; 
// 	}

// 	RadioMethodCheckConnection methodCheckConnection;
// 	methodCheckConnection.enableCheckConnection(false);
// 
// 	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "stopCheckConnection" );
// 
// 	m_checkConnProcessState = RequestStopCheck;
// 	m_tcpConnection->Send( methodCheckConnection );

	FUNCTION_EXIT;
}


void RadioKeepAliveHandlerThread::setExpirePeriod(ta_uint32 expireTime )
{
	FUNCTION_ENTRY("setExpirePeriod");
	m_expireTime = expireTime;
	FUNCTION_EXIT;
}

bool RadioKeepAliveHandlerThread::processEvent( TA_IRS_App::RadioEvent event )
{
	FUNCTION_ENTRY("processEvent");

	bool isKeepAlive = false;

	if ( m_checkConnProcessState == NotRequeseted )
	{
		FUNCTION_EXIT;
		return isKeepAlive;
	}
	else if ( event.getEventType() == EVENT_KEEPALIVE )
	{
		isKeepAlive = true;

		if ( TA_Base_Core::DebugUtil::getInstance().getLevel() >= TA_Base_Core::DebugUtil::DebugDebug )
		{
			RadioEventReader* eventReader = event.getEventReader();

			KeepAliveEventReader* keepAliveReader = dynamic_cast<KeepAliveEventReader*>(eventReader);

			if ( keepAliveReader != NULL )
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
					"[KeepAlive] Received keep alive message: %d", keepAliveReader->getEventCounter() );
			}
		}
	}
	else if ( event.getEventType() == EVENT_CHECKCONNECTION )
	{
		isKeepAlive = true;

		std::ostringstream outStr;

		outStr << event.getStatus() << ", ";

		switch( event.getStatus() )
		{
		case STATUS_RESTARTED:
			m_checkConnProcessState = KeepAliveStarted;
			outStr << "KeepAliveRestarted ";
			break;
		case STATUS_STARTED:
			m_checkConnProcessState = KeepAliveStarted;
			outStr << "KeepAliveStarted ";
			break;
		case STATUS_STOPPED:
			m_checkConnProcessState = KeepAliveStopped;
			outStr << "KeepAliveStopped ";
			break;
		case STATUS_KO_NOT_ALLOWED:
			m_checkConnProcessState = NotRequeseted;
			{
				if ( m_interval <= 0 )
				{
					m_interval = TA_IRS_App::RadioAgentConstants::KEEP_ALIVE_DEFAULT_INTERVAL;
					startCheckConnection();
				}
			}
			outStr << "KeepAliveRequestFailed ";
			break;
		default:
			outStr << "Error " << event.getStatus();
			break;
		}

		outStr << "." << std::endl;

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
					"[KeepAlive] Received check connection message, status: %s", outStr.str().c_str() );
	}

	m_semaphore.post();

	FUNCTION_EXIT;
	return isKeepAlive;
}
