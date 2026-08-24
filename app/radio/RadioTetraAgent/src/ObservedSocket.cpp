/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/C830G_TIP/830G/transactive/app/radio/RadioTetraAgent/src/ObservedSocket.cpp $
  * @author:  Wong Soh Sum
  * @version: $Revision: #6 $
  *
  * Last modification: $DateTime: 2021/11/10 12:13:46 $
  * Last modified by:  $Author: builder $
  *
  *
  * This is the socket class that will manage the connection 
  * to the RTC server.  This class works with a observer pattern, 
  * running on its own thread.  It will notify the observer when  
  * the connection is lost or established as well as when there
  * is incoming data.
  */

#include "core/types/src/ta_types.h"
#include "app/radio/RadioTetraAgent/src/ObservedSocket.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/synchronisation/src/ThreadGuard.h"

#include "core/sockets/src/InetAddress.h"
#include "core/sockets/src/ICMPMessageCodec.h"
#include "core/threads/src/QueueProcessor.h"

#include "ace/OS.h"
#include "ace/os_include/netinet/os_tcp.h"
#include <sstream>



namespace TA_Base_Core
{
	//limin++, failover
	namespace
	{
		template< typename T > class ThreadedMemberFunction : public TA_Base_Core::Thread
		{
			typedef void ( T::* MemberFunction )();
			typedef std::vector< MemberFunction > MemberFunctionList;
			
    public:
        
        static ThreadedMemberFunction& alloc( T& o, MemberFunction mf = NULL )
        {
			TA_Base_Core::ThreadGuard( lock() );
			
            static ThreadedMemberFunction* instance = NULL;
            
            instance = new ThreadedMemberFunction( o, instance );
            
            if ( NULL != mf )
            {
                instance->add( mf );
                
                instance->start();
            }
            
            return *instance;
        }
        
        void add( MemberFunction mf )
        {
            m_members.push_back( mf );
        }
        
    protected:
        
        virtual void run()
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Function Entered: ThreadedMemberFunction, there are [%d] member function(s) to call", m_members.size() );
            
            for ( typename MemberFunctionList::iterator mfIter = m_members.begin(); mfIter != m_members.end(); ++mfIter )
            {
                try
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ThreadedMemberFunction: calling [%d] member function", std::distance( m_members.begin(), mfIter ) );
                    
                    ( m_object.*( *mfIter ) )();
                }
                catch (...)
                {
                    LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "" );
                }
            }
            
            clearPrivious();
            
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Function Exited: ThreadedMemberFunction" );
        }
        
        virtual void terminate(){}
        
    private:
        
        ThreadedMemberFunction( T& o, ThreadedMemberFunction* privious )
            : m_object( o ),
			m_privious( privious )
        {
        }
		
        ThreadedMemberFunction( const ThreadedMemberFunction& );
        ThreadedMemberFunction& operator= ( const ThreadedMemberFunction& );
		
        ~ThreadedMemberFunction()
        {
            terminateAndWait();
        }
        
        void clearPrivious()
        {
            delete m_privious;
            m_privious = NULL;
        }
        
        static TA_Base_Core::NonReEntrantThreadLockable& lock()
        {
            static TA_Base_Core::NonReEntrantThreadLockable lock;
            return lock;
        }
        
    private:
        
        T& m_object;
        MemberFunctionList m_members;
        ThreadedMemberFunction* m_privious;
		};
		
		typedef ThreadedMemberFunction< ObservedSocket > ObservedSocketThreadedMemberFunction;


        // limin++ CL-21192
        typedef void ( TA_Base_Core::ObservedSocket::* ObservedSocketMemberFunction )();

        struct ObservedSocketQueueItem
        {
            ObservedSocketQueueItem( ObservedSocketMemberFunction memberFunction ) : m_memberFunction( memberFunction ) {};
            ObservedSocketMemberFunction m_memberFunction;
        };

        typedef boost::shared_ptr< ObservedSocketQueueItem > ObservedSocketQueueItemPtr;

        class ObservedSocketMemberFunctionProcessor : public TA_Base_Core::QueueProcessor< ObservedSocketQueueItem >
        {
        public:
            
            ObservedSocketMemberFunctionProcessor( TA_Base_Core::ObservedSocket* observedSocket )
                : m_observedSocket( observedSocket )
            {
                start();
            }

        protected:

            virtual void processEvent( boost::shared_ptr< ObservedSocketQueueItem > newEvent )
            {
                ( m_observedSocket->*( newEvent->m_memberFunction ) )();
            };

        private:

            TA_Base_Core::ObservedSocket* m_observedSocket;
        };
		// ++limin CL-21192
	}
//limin++, failover

	ObservedSocket::ObservedSocket( const std::string& host,
									const std::string& service,
									ISocketObserver& observer )
									: m_host( host ),
									  m_service( service ),
									  m_tcpNoDelay( DEFAULT_TCP_NO_DELAY ? 1 : 0 ),
									  m_observer( observer ),
									  m_internalDataBuffer( NULL ),
									  m_internalDataBufferSize(DEFAULT_READ_BUFFER_SIZE),
									  m_toTerminate( false ),
									  m_isConnected( false ),
									  m_isPrevConnected( false ),
									  m_sleepTime( DEFAULT_SLEEP_TIME ),
									  m_reconnectTime( DEFAULT_RECONNECT_INTERVAL ),
									  m_pingInterval(DEFAULT_PING_INTERVAL),
									  m_pingTimer(0),
									  m_isPingEnabled(false),
									  m_pingTimeoutSeconds(PING_TIMEOUT_IN_SECOND),
									  m_pingDataIndex(0),
									  m_reconnectInterval(0),
									  m_maxConRetry(DEFAULT_MAX_CON_RETRY),
									  m_curConAttempt(0)

	{
	#ifdef WIN32
		m_pid = _getpid();
	#else
		m_pid = getpid();
	#endif

		u_short portNumber(0);
		std::stringstream ssPortNumber;
		ssPortNumber << m_service;
		ssPortNumber >> portNumber;
    
		if (portNumber != 0)
		{
			// port number given
			m_remoteAddress.set( portNumber, m_host.c_str() );
		}
		else
		{
			// named port given
			m_remoteAddress.set( m_service.c_str(), m_host.c_str() );
		}

		m_socketTimeout.msec(static_cast<long>(DEFAULT_SOCKET_TIMEOUT_MILLIS));

		m_internalDataBuffer = new unsigned char[m_internalDataBufferSize];
	}


	ObservedSocket::~ObservedSocket()
	{
		m_toTerminate = true;

		delete[] m_internalDataBuffer;
	}



	ObservedSocket& ObservedSocket::setSocketTimeout(ta_uint32 socketTimeoutMillis )
	{
		m_socketTimeout.msec(static_cast<long>(socketTimeoutMillis));
    
		return *this;
	}


	ObservedSocket& ObservedSocket::setTcpNoDelay( bool noDelay )
	{
		m_tcpNoDelay = (noDelay ? 1 : 0);
    
		return *this;
	}


	ObservedSocket& ObservedSocket::setReadBufferSize(ta_uint32 bufferSize )
	{
		delete[] m_internalDataBuffer;
		m_internalDataBufferSize = bufferSize;
		m_internalDataBuffer = new unsigned char[m_internalDataBufferSize];

		return *this;
	}


	ObservedSocket& ObservedSocket::setThreadSleepTime( unsigned short sleepTimeMillis )
	{
		m_sleepTime = sleepTimeMillis;

		return *this;
	}


	ObservedSocket& ObservedSocket::setReconnectInterval( unsigned short reconnectIntervalMillis )
	{
		m_reconnectTime = reconnectIntervalMillis;

		return *this;
	}

	ObservedSocket& ObservedSocket::setPingInterval( unsigned short pingIntervalMillis )
	{
		m_pingInterval = pingIntervalMillis;
		
		return *this;
	}

	ObservedSocket& ObservedSocket::setPingOption( bool enablePing )
	{
		m_isPingEnabled = enablePing;

		return *this;
	}

	bool ObservedSocket::isPingEnabled() const
	{
		return m_isPingEnabled;
	}

	bool ObservedSocket::isConnected() const
	{
		return m_isConnected;
	}

	void ObservedSocket::clearWriteBuffer()
	{
		TA_Base_Core::ThreadGuard guard(m_writeBufferLock);
    
		// now the stl documentation says this is valid
		// but as always VC++ thinks it knows better...
		// stupid microsoft
		//m_writeBuffer.clear();

		m_writeBuffer.erase( m_writeBuffer.begin(), m_writeBuffer.end() );
	}


	void ObservedSocket::run()
	{
		connect();

		while ( m_toTerminate == false )
		{
			if ( true == m_isConnected )
			{
				// Check if there are things to write 
				// If there is, send out to server first
				{
					TA_Base_Core::ThreadGuard guard(m_writeBufferLock);
					if(m_writeBuffer.size() != 0)
					{
						sendData();
					}
				}

				// Check if there are information from remote party
				getIncomingData();
				if(m_isConnected == true && m_readBuffer.size() != 0)  // Socket is still connected
				{ 
					notifyObserverOfIncomingData();
					//if we receive any thing, it means the link is good. so no need to ping.
					m_pingTimer = 0;
				}
			}
			else // try to connect until success
			{
				if ( m_reconnectInterval > 0 )
				{ 
					TA_Base_Core::Thread::sleep( m_reconnectInterval * 1000 );

					m_reconnectInterval = 0;
				}

				connect();
			}

			if ( true == m_isConnected )
			{		
				if (m_isPingEnabled && m_pingTimer > m_pingInterval)
				{
					bool pingResult = doPing();
								
					LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug, 
						"Polling Radio Server[%s] %s", 
						m_host.c_str(), (pingResult ? "successfully" : "failed" ));
					
					if(!pingResult)
					{
						m_isConnected = false;
						ObservedSocketThreadedMemberFunction::alloc(*this, &ObservedSocket::notifyConnectionLost);
						m_isPrevConnected = m_isConnected;
					}

					m_pingTimer = 0;
				}
				// fast poll - get data quicker
				Thread::sleep(m_sleepTime);
				m_pingTimer += m_sleepTime;
			}
			else
			{
				// slower poll, give it time between attempts
				Thread::sleep(m_reconnectTime);
			}
		}
	}


	void ObservedSocket::terminate()
	{
		m_toTerminate = true;
		close();
	}

	bool ObservedSocket::connect()
	{
		bool retunValue = false;

		if ( true == m_isConnected )
		{
			return true;
		}

		close();
		TA_Base_Core::ThreadGuard guard(m_streamLock);
		m_curConAttempt++; // Increment

		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Connecting remote server attemp no. %d", m_curConAttempt);
		int status = m_connector.connect(m_clientStream, m_remoteAddress, &m_socketTimeout);
		int error = errno;

		if ( status == -1 )
		{
			retunValue = false;

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
						"Failed to connect on attempt no. %d to %s:%d. Error[%d][%s]", m_curConAttempt,
						m_remoteAddress.get_host_addr(), m_remoteAddress.get_port_number(),
						error, ACE_OS::strerror(error) );

			m_isConnected = false;

			if(m_isPrevConnected != m_isConnected && m_curConAttempt >= m_maxConRetry)
			{
				ObservedSocketThreadedMemberFunction::alloc(*this, &ObservedSocket::notifyConnectionLost);
				m_isPrevConnected = m_isConnected;
			}
		}
		else
		{
			m_isConnected = true;
			m_curConAttempt = 0;

			retunValue = true;

			m_clientStream.enable(ACE_NONBLOCK);

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"Connected to %s:%d.", m_remoteAddress.get_host_addr(), m_remoteAddress.get_port_number() );

			status = m_clientStream.set_option( IPPROTO_TCP, TCP_NODELAY, &m_tcpNoDelay, sizeof( m_tcpNoDelay ) );
			if (status != 0)
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Failed to set TCP_NODELAY to %s socket option for connection to %s:%s.",
					m_tcpNoDelay ? "TRUE" : "FALSE", m_host.c_str(), m_service.c_str() );
			}

			//status = m_clientStream.set_option(IPPROTO_TCP,TCP_KEEPALIVE,NULL,0);
			//
	/*		int tcp_timeout = 60;
			status = m_clientStream.set_option( SOL_SOCKET, SO_KEEPALIVE, &tcp_timeout, sizeof( tcp_timeout ) );
			if (status != 0)
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Failed to set SO_KEEPALIVE to %s socket option for connection to %s:%s.", tcp_timeout, m_host.c_str(), m_service.c_str() );
			}*/

			//if(m_isPrevConnected != m_isConnected)
			//{
			ObservedSocketThreadedMemberFunction::alloc(*this, &ObservedSocket::notifyConnectionEstablished);
			m_isPrevConnected = m_isConnected;
			//}
		}

		return retunValue;
	}


	bool ObservedSocket::close( int retryDelay )
	{
		TA_Base_Core::ThreadGuard guard(m_streamLock);
		bool returnValue = false;
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Closing Socket connection with delay %d", retryDelay);
		m_reconnectInterval = retryDelay;

		if (m_clientStream.close () == -1)
		{
			int error = errno;

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
						"Error closing socket connection to %s:%s, Error:[%d][%s]",
						m_host.c_str(), m_service.c_str(), error, ACE_OS::strerror(error));
		}
		else
		{
			returnValue = true;
		}

		m_isConnected = false;
		//m_isPrevConnected = false;

		return returnValue;
	}


	void ObservedSocket::sendData()
	{
		if(m_isConnected == true)
		{
			TA_Base_Core::ThreadGuard guard(m_streamLock);
			int status = m_clientStream.send_n(m_writeBuffer.c_str(), m_writeBuffer.size(), &m_socketTimeout);

			if(status <= 0 /*== -1*/)
			{
				int error = errno;

				if(isConnectionLost(error) == true || error == ETIME)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
								"Socket %s:%s send_n error: Native Error Code:[%d][%s]",
								m_host.c_str(), m_service.c_str(), error, ACE_OS::strerror(error));
					m_isPrevConnected = m_isConnected;
					m_isConnected = false;
					notifyObserverOfWriteFailed();
				}
			}
        
			// again, microsoft STL is crap
			//m_writeBuffer.clear(); // Clear the buffer only if write succeeded
			m_writeBuffer.erase( m_writeBuffer.begin(), m_writeBuffer.end() );
		}
	}

	void ObservedSocket::getIncomingData()
	{
		if(m_isConnected == true) // already connected
		{
			TA_Base_Core::ThreadGuard guard(m_streamLock);
			size_t byteCount = 0;
			int status = 0;
			size_t bytes_transferred = 0;

			// must be called before every operation - it seems it doesnt stick
			m_clientStream.enable(ACE_NONBLOCK);

			// also no timeout on read operations - otherwise they block for the timeout period
			status = m_clientStream.recv(m_internalDataBuffer, m_internalDataBufferSize);

			if (status > 0)
			{
				bytes_transferred = status;

				m_readBuffer.append(m_internalDataBuffer, bytes_transferred);

				// debugging
				if(bytes_transferred > 0)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
								"Socket %s:%s incoming %d bytes of data",
								m_host.c_str(), m_service.c_str(), bytes_transferred);
				}
			}

			m_isPrevConnected = m_isConnected;
			if(status < 0) // error
			{
				int error = errno;
				m_isConnected = !isConnectionLost(error);

				//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"Error in processing recv(),errno = %d",error);

				// only log on error
				if (false == m_isConnected) // (EWOULDBLOCK != error)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
						"Socket %s:%s status = %d, Native Error Code:[%d][%s], isConnected:%s", 
						m_host.c_str(), m_service.c_str(), status, error,
						ACE_OS::strerror(error), (m_isConnected)?"True":"False");
				}
			}
			else if (0 == status) // graceful remote close
			{
				m_isConnected = false;
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
							"Socket %s:%s closed normally",
							m_host.c_str(), m_service.c_str() );
			}

			// Remove the notify on failure to allow reconnection

			//notify observer the lost of connection if the status changes to be disconnected
			//if (false == m_isConnected)
			//{
                // limin++ CL-21192
			//	ObservedSocketThreadedMemberFunction::alloc(*this, &ObservedSocket::notifyConnectionLost);
                // ++limin++ CL-21192

			//	m_isPrevConnected = m_isConnected;
			//}
		}
	}

	void ObservedSocket::sendData(const SocketData& data)
	{
		TA_Base_Core::ThreadGuard guard(m_writeBufferLock);

		m_writeBuffer.append(data);
	}


	void ObservedSocket::sendData(const std::vector<unsigned char>& data)
	{
		TA_Base_Core::ThreadGuard guard(m_writeBufferLock);

		m_writeBuffer.append(data.begin(), data.end());
	}


	void ObservedSocket::sendData(const unsigned char* data, unsigned int dataSize)
	{
		TA_Base_Core::ThreadGuard guard(m_writeBufferLock);

		m_writeBuffer.append(data, dataSize);
	}


	void ObservedSocket::notifyConnectionLost()
	{
		TA_Base_Core::ThreadGuard guard(m_notificationLock);
		m_observer.connectionLost( this );
	}


	void ObservedSocket::notifyConnectionEstablished()
	{
		TA_Base_Core::ThreadGuard guard(m_notificationLock);
		m_observer.connectionEstablished( this );
	}


	void ObservedSocket::notifyObserverOfIncomingData()
	{
		m_observer.processReceivedData( m_readBuffer, this );
	}


	void ObservedSocket::notifyObserverOfWriteFailed()
	{
		m_observer.writeFailed( m_writeBuffer, this );
	}

	bool ObservedSocket::doPing()
	{
		bool toReturn = false;

		bool canRead = false;
		bool canWrite = false;
		

		TA_Base_Core::RawDatagramSocket pingSocket("icmp", true);
		if (!pingSocket.isOpen())
		{
			if(!pingSocket.open())
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Could not open socket for pinging");
				return toReturn;
			}
		}

		double current = 0;
		double sentTime = 0;

		if(m_pingDataIndex > PING_INDEX_MAX_BOUND)
		{
			m_pingDataIndex = 0;
		}
		unsigned int count = m_pingDataIndex;
		//get the start ping time
		time_t timeSent = 0;
		time(&timeSent);
		sentTime = timeSent;

		do {
			//at mixmium, send 4 package at one time
			if (count < m_pingDataIndex + 5)
			{
				count ++;
			}        

			//API REF:
				// wait for io on only one socket.  One of read or write must be
				// true otherwise function will return immediately with false.
				// static bool waitForOneIO(SocketType *socket, bool read, bool write,
				//					 bool &canRead, bool &canWrite,
				//					 unsigned int timeMillis);
				//
			// wait in 200ms blocks so we can bail out on
			// m_tellThreadToStop in a reasonably responsive manner.
			//this call sets the canRead and canWrite
			if (TA_Base_Core::SocketSet<TA_Base_Core::IDatagramSocket>::waitForOneIO(&pingSocket,
					true, true, canRead, canWrite, 200))
			{
				if (canWrite && (count < m_pingDataIndex + 5) )
				{
					//note that the address obj is modified by the send method
					TA_Base_Core::InetAddress ina(m_host.c_str());
                
					//construct a datagram
					TA_Base_Core::Datagram datagram;

					//fill out packet structure so that it is ECHO, i.e. a "ping"
					TA_Base_Core::ICMPMessageCodec::getInstance()->constructECHO(datagram, m_pid, count);

					//if the sending of this datagram fails...
					//note that the address obj is modified by the send method
					if (!pingSocket.send(ina, datagram))
					{

						const std::string& name = ina.getHostName();
						LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "failed to send to ping for %s", name.c_str());

					}

				} //end if (canWrite)

				//if a ping responce has arrived...
				if (canRead)
				{
					//These variables are populated by the receive method
					TA_Base_Core::InetAddress outa;
					TA_Base_Core::Datagram datagram;

					//if we can read the responce from the socket...
					if (pingSocket.receive(outa, datagram))
					{
						unsigned short id = 0, index = 0;

						//if we can get the data out of the package...
						if (TA_Base_Core::ICMPMessageCodec::getInstance()->decodeECHO(datagram, id, index))
						{

							//if this is a datagram we sent...
							if (id == m_pid && index < count)
							{							        
								LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
								"Successfully obtained the ping reply from server");
								//get out of the while loop
								toReturn = true;
								break;                           
							}
							else
							{
								// an old or someone else's ping
								LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
									"decodeECHO() got bad id=%u or bad seq=%u", id, index);
							}
						}
						else
						{
							// this isn't necessarily bad. It could be someone else's packet.
							LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "response failed checksum.");
						}
					}
					else
					{
						LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "receive() failed");
					}
				} //end : if(canRead)
			} //end: if socket is ready to read or write

			time_t now;
			time(&now);
			current = now;
        
		}while (current - sentTime <= m_pingTimeoutSeconds || m_toTerminate);

		m_pingDataIndex = count;

		return toReturn;
	}

	bool ObservedSocket::isConnectionLost(int state) const
	{
		std::string errStr;
		switch (state)
		{
			case ETIME: 
				errStr = "RROR_SEM_TIMEOUT";
				break;
			case EWOULDBLOCK:
				errStr = "WSAEWOULDBLOCK";
				break;
			case EINPROGRESS:
				errStr = "WSAEINPROGRESS";
				break;
			case EALREADY:
				errStr = "WSAEALREADY";
				break;
			case ENOTSOCK:           
				errStr = "WSAENOTSOCK";
				break;
			case EDESTADDRREQ:
				errStr = "WSAEDESTADDRREQ";
				break;
			case EMSGSIZE:
				errStr = "WSAEMSGSIZE";
				break;
			case EPROTOTYPE:
				errStr = "WSAEPROTOTYPE";
				break;
			case ENOPROTOOPT:
				errStr = "WSAENOPROTOOPT";
				break;
			case EPROTONOSUPPORT:
				errStr = "WSAEPROTONOSUPPORT";
				break;
			case ESOCKTNOSUPPORT:
				errStr = "WSAESOCKTNOSUPPORT";
				break;
			case EOPNOTSUPP:
				errStr = "WSAEOPNOTSUPP";
				break;
			case EPFNOSUPPORT:
				errStr = "WSAEPFNOSUPPORT";
				break;
			case EAFNOSUPPORT:
				errStr = "WSAEAFNOSUPPORT";
				break;
			case EADDRINUSE:
				errStr = "WSAEADDRINUSE";
				break;
			case EADDRNOTAVAIL:
				errStr = "WSAEADDRNOTAVAIL";
				break;
			case ENETDOWN:
				errStr = "WSAENETDOWN";
				break;
			case ENETUNREACH:
				errStr = "WSAENETUNREACH";
				break;
			case ENETRESET:
				errStr = "WSAENETRESET";
				break;
			case ECONNABORTED:
				errStr = "WSAECONNABORTED";
				break;
			case ECONNRESET:
				errStr = "WSAECONNRESET";
				break;
			case ENOBUFS:
				errStr = "WSAENOBUFS";
				break;
			case EISCONN:
				errStr = "WSAEISCONN";
				break;
			case ENOTCONN:
				errStr = "WSAENOTCONN";
				break;
			case ESHUTDOWN:
				errStr = "WSAESHUTDOWN";
				break;
			case ETOOMANYREFS:
				errStr = "WSAETOOMANYREFS";
				break;
			case ETIMEDOUT:
				errStr = "WSAETIMEDOUT";
				break;
			case ECONNREFUSED:
				errStr = "WSAECONNREFUSED";
				break;
			case ELOOP:
				errStr = "WSAELOOP";
				break;
			case EHOSTDOWN:
				errStr = "WSAEHOSTDOWN";
				break;
			case EHOSTUNREACH:
				errStr = "WSAEHOSTUNREACH";
				break;
	//		case EPROCLIM:
	//			errStr = "WSAEPROCLIM";
	//			break;
			case EUSERS:
				errStr = "WSAEUSERS";
				break;
			case EDQUOT:
				errStr = "WSAEDQUOT";
				break;
			case ESTALE:
				errStr = "WSAESTALE";
				break;
			case EREMOTE:
				errStr = "WSAEREMOTE";
				break;
			default:
				errStr = "UNKNOWN";
		}

		//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Error String: %s", errStr.c_str());

		switch(state)
		{
			case ECONNABORTED:
			case ECONNREFUSED:
			case ECONNRESET:
			case EHOSTDOWN:
			case EHOSTUNREACH:
			case ENETDOWN:
			case ENETUNREACH:
			case ENOTCONN:
			case ETIMEDOUT:
				return true;
		}
		return false;
	}

} // namespace TA_Base_Core

