#region Header Block
/// <classname>PaFunctions</classname> 
/// <description>Main function is to pack the data to be sent to PAS</description> 
/// <filename>PaFunctions.cs</filename> 
/// <authors>Tay Yu Hui</authors> 
#endregion

using System;						// Contain all other namespaces as subordinates
using System.Collections.Generic;	// Allow subitems to be contained in a variety of containers
using System.Net.Sockets;			// Implements the Windows Sockets (Winsock) interface to control access to the network.
using System.Reflection;			// Retrieve information about assemblies, modules, members, parameters, and other entities in managed code by examining their metadata. 
using System.Threading;				// Enable multithreaded programming

namespace ste.pa.pamanager
{
	/// <summary>
	/// Contains common PA functions methods
	/// </summary>
	public class PaFunctions
	{
		/// <summary>
		/// Reference to a method used when there is a change in the connection state to PA System
		/// </summary>
		/// <param name="linkStatus">status of the socket connection</param>
		public delegate void PaSysConnChange(bool linkStatus);

		/// <summary>
		/// The callback method used in the delegate
		/// </summary>
		public PaSysConnChange EventPaSysConnChange = null;

		// Stores the received packet with the tableNo as the Key
		private static Dictionary<int, byte[]> readReplyDic_ = new Dictionary<int, byte[]>();
		private static object readCacheLock_ = new object();

		// Stores the received packet with the tableNo as the Key
		private static Dictionary<int, byte[]> writeReplyDic_ = new Dictionary<int, byte[]>();
		private static object writeCacheLock_ = new object();

		// Connection status to PAS
		private static bool ConnectionStatus_ = false;
		private static bool primaryIp_ = true;

		private static string fileName_ = MethodBase.GetCurrentMethod().DeclaringType.Name;

		private static Thread monitorReceiveThread_ = null;
		private static Thread monitorSocketThread_ = null;

		private static DateTime preZoneUpdateTime_ = DateTime.Now;
		private static DateTime preFlagUpdateTime_ = DateTime.Now;

		/// <summary>
		/// Static constructor 
		/// </summary>
		static PaFunctions()
		{
		}

		#region PAS Socket
		/// <summary>
		/// Method to initialize socket and read in config.ini
		/// </summary>
		/// <returns>Reference to socket class object</returns>
		public SocketCls initPaSocket()
		{
			Program.WriteEventLog("[INFO] Start Socket initialization.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

			SocketCls socketClient = new SocketCls();

			// Initial TCP/IP Client (as client)
			//socketClient.EventSocketAlive_ += socketAlive;
			socketClient.EventSocketErr += socketError;
			socketClient.EventDataArrival += socketDataArrival;

			return socketClient;
		}

		/// <summary>
		/// Switch on/off the connect PA System socket thread
		/// </summary>
		/// <param name="stateFlag">bool: true to start thread to connect to PA System, otherwise false</param>
		public void setMonitorSocketThreadState(bool stateFlag)
		{
			// Nothing to validate : stateFlag
			if (stateFlag)
			{
				if (monitorSocketThread_ == null)
				{
					Program.WriteEventLog("[INFO] monitorSocketThread - started...", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
					monitorSocketThread_ = new Thread(new ThreadStart(connectPaSysSocket));
					monitorSocketThread_.Name = "MonitorThread";
					monitorSocketThread_.IsBackground = true;
					monitorSocketThread_.Start();
				}
				else if (monitorSocketThread_.ThreadState != ThreadState.Background)
				{
					Program.WriteEventLog("[INFO] monitorSocketThread - restarting...", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
					monitorSocketThread_ = new Thread(new ThreadStart(connectPaSysSocket));
					monitorSocketThread_.Name = "MonitorThread";
					monitorSocketThread_.IsBackground = true;
					monitorSocketThread_.Start();
				}
			}
			else
			{
				cleanUp();
			}
		}

		/// <summary>
		/// Method to connect to PA System and start the receiving socket thread once connection is established 
		/// </summary>
		private void connectPaSysSocket()
		{
			try
			{
				Program.WriteEventLog("[INFO] Starting thread to connect to PA System.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

				attemptSocketConnection();

				// PA System connected, proceed to start socket listening thread
				if (monitorReceiveThread_ == null)
				{
					monitorReceiveThread_ = new Thread(new ThreadStart(DataObject.socketClient.ReceiveThread));
					monitorReceiveThread_.Name = "MonitorReceiveThread";
					monitorReceiveThread_.Start();
					monitorReceiveThread_.IsBackground = true;
					Program.WriteEventLog("[INFO] Monitor Receive Thread was not found. - Creating.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				}
				else if (monitorReceiveThread_.ThreadState != ThreadState.Background)
				{
					monitorReceiveThread_ = new Thread(new ThreadStart(DataObject.socketClient.ReceiveThread));
					monitorReceiveThread_.Name = "MonitorReceiveThread2";
					monitorReceiveThread_.Start();
					monitorReceiveThread_.IsBackground = true;
					Program.WriteEventLog("[INFO] Monitor Receive Thread was stopped. - Restarting.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				}

				socketAlive();
				Program.WriteEventLog("[INFO] Successfully connected to PA System", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
			}
			catch (Exception ex)
			{
				Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
			}
		}

		/// <summary>
		/// Will keep on trying to connect to PA System (either primary or secondary system)
		/// </summary>
		private void attemptSocketConnection()
		{
			while (!ConnectionStatus_)
			{
				if (primaryIp_)
				{
					try
					{
						Program.WriteEventLog("[INFO] Trying to connect to [Pri] PA System. Connecting...", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
						ConnectionStatus_ = DataObject.socketClient.initSocket(Program.paSysAddPri, Program.paSysPort);
                        if (!ConnectionStatus_)
                        {
                            primaryIp_ = false;
                        }
					}
					catch (SocketException ex)
					{
						Program.WriteEventLog("[ERROR] [SocketException] [Pri] SocketErrorCode: [" + ex.SocketErrorCode + "] Exception: " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

						if (ex.SocketErrorCode == SocketError.ConnectionRefused)
						{
							Program.WriteEventLog("[ERROR] [SocketException] [Pri] Connection Refused.. switching to Secondary IP. ", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
						}
						else
						{
							Program.WriteEventLog("[ERROR] [SocketException] [Pri] Retry socket connection to [" +
													Program.paSysAddPri + ":" + Program.paSysPort +
													"] ...", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
						}
                        ConnectionStatus_ = false;
                        primaryIp_ = false;
					}
					catch (Exception ex)
					{
						Program.WriteEventLog("[ERROR] [Exception] Source: " + ex.Source + " Message: " + ex.Message + " Exception: " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
                        ConnectionStatus_ = false;
                        primaryIp_ = false;					
                    }
				}
				else
				{
					try
					{
						Program.WriteEventLog("[INFO] Trying to connect to [Sec] PA System. Connecting...", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
						ConnectionStatus_ = DataObject.socketClient.initSocket(Program.paSysAddSec, Program.paSysPort);
                        if (!ConnectionStatus_)
                        {
                            primaryIp_ = true;
                        }
					}
					catch (SocketException ex)
					{
						Program.WriteEventLog("[ERROR] [SocketException] [Sec] SocketErrorCode: [" + ex.SocketErrorCode + "] Exception: " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

						if (ex.SocketErrorCode == SocketError.ConnectionRefused)
						{
							Program.WriteEventLog("[ERROR] [SocketException] [Sec] Connection Refused.. switching to Primary IP. ", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
						}
						else
						{
							Program.WriteEventLog("[ERROR] [SocketException] [Sec] Retry socket connection to [" +
													Program.paSysAddSec + ":" + Program.paSysPort +
													"] ...", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
						}
                        ConnectionStatus_ = false;
                        primaryIp_ = true;

					}
					catch (Exception ex)
					{
						Program.WriteEventLog("[ERROR] [Exception] Source: " + ex.Source + " Message: " + ex.Message + " Exception: " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
                        ConnectionStatus_ = false;
                        primaryIp_ = true;
                    }
				}

				Thread.Sleep(Program.pasConnectionIntervalMs);
			}

			Program.WriteEventLog("[INFO] PA System successfully connected.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
		}

		/// <summary>
		/// Method to check the PA System connection status
		/// </summary>
		public static bool Check_PAS_Connection_Status
		{
			get { return (ConnectionStatus_); }
		}

		#region Delegate Methods
		/// <summary>
		/// Method called whenever a connection to the PA System is finally established
		/// </summary>
		private void socketAlive()
		{
			ConnectionStatus_ = true;
			EventPaSysConnChange(true);
		}

		/// <summary>
		///  Method called whenever connection to the PA System is down
		/// </summary>

		public void socketError()
		{
			Program.WriteEventLog("[ERROR] Possible Socket disconnection! Try to reconnect...", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
			ConnectionStatus_ = false;
			setMonitorSocketThreadState(true);

			EventPaSysConnChange(false);
		}

		/// <summary>
		/// Method called whenever data arrives at the listening socket
		/// </summary>
		/// <param name="receivedPacketIn">byte[] containing all received byte data</param>
		public void socketDataArrival(byte[] receivedPacketIn)
		{
			// Nothing to validate : receivedPacketIn
			if (receivedPacketIn == null)
			{
				Program.WriteEventLog("[ERROR] receivedPacketIn is null.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				return;
			}

			try
			{
				int expectedPacketSize = 0;

				if (receivedPacketIn.Length != 0)
				{
					// Just append to Dataobject receiving buffer					
					lock (DataObject.receivingBufferLock)
					{
						// Just append to Dataobject receiving buffer					
						for (int i = 0; i < receivedPacketIn.Length; i++)
						{
							DataObject.receivingBuffer.Add(receivedPacketIn[i]);
						}

						while (DataObject.receivingBuffer.Count > 0)
						{
							#region Check Packet Type, and determine the packet size
							if (DataObject.receivingBuffer[0] == (int)EnumType.PacketType.Write)
							{
								// is W type, Reply packet is of size 4
								expectedPacketSize = 4;
							}
							else if (DataObject.receivingBuffer[0] == (int)EnumType.PacketType.Read)
							{
								// is R type, Reply packet is dynamic size
								if (DataObject.receivingBuffer.Count >= 5)
								{
									expectedPacketSize = Util.Byte_To_Int(DataObject.receivingBuffer[3], DataObject.receivingBuffer[4]) + Program.WRITE_HEADER_SIZE;
								}
								else
								{
									// Packet header not yet received fully, cannot determine packet size
									expectedPacketSize = 0;
									break;
								}
							}
							else
							{
								// Something wrong with buffer. Clearing everything.
								Program.WriteDebugEventLog("[ERROR] Buffer error... clearing...", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
								DataObject.receivingBuffer.Clear();
								expectedPacketSize = 0;
								break;
							}
							#endregion

							if (expectedPacketSize > 0 && DataObject.receivingBuffer.Count >= expectedPacketSize)
							{
								// Create temp byte[] object
								byte[] receivedPacket = new byte[expectedPacketSize];

								// Copy out the "full" packet from receiving buffer to temp buffer
								DataObject.receivingBuffer.CopyTo(0, receivedPacket, 0, expectedPacketSize);

								// Remove read buffer from receiving buffer
								DataObject.receivingBuffer.RemoveRange(0, expectedPacketSize);
								expectedPacketSize = 0;

								if (receivedPacket[0] == (int)EnumType.PacketType.Read)
								{
									processReadPacket(receivedPacket);
								}
								else if (receivedPacket[0] == (int)EnumType.PacketType.Write)
								{
									processWritePacket(receivedPacket);
								}
								else
								{
									// Unknown packet
								}
							}
							else
							{
								// Packet not fully received, do nth
								Program.WriteDebugEventLog("[WARNING] Packet not fully received... break...", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
								break;
							}

							if (DataObject.receivingBuffer.Count > 0)
							{
								Program.WriteDebugEventLog("[WARNING] Receive buffer not empty... reloop!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
							}

						}
					}
				}
				else
				{
					Program.WriteDebugEventLog("[DEBUG] Empty Packet received. not doing anything", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				}
			}
			catch (Exception ex)
			{
				Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
			}
		}

		/// <summary>
		/// Process the Read reply packet
		/// </summary>
		/// <param name="receivedPacketIn">byte array of packet received from PAS</param>
		private void processReadPacket(byte[] receivedPacketIn)
		{
			if (receivedPacketIn == null)
			{
				Program.WriteEventLog("[ERROR] receivedPacketIn is null.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				return;
			}

			lock (readCacheLock_)
			{
				int locTableNo = Util.Byte_To_Int(receivedPacketIn[1], receivedPacketIn[2]);
				Program.WriteDebugEventLog("[DEBUG] [READ] Insert table [" + locTableNo + "]", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

				if (readReplyDic_.ContainsKey(locTableNo))
				{
					// Dictionary already contains a copy of the received table, remove it first
					readReplyDic_.Remove(locTableNo);
				}

				// Add received packet to dictionary
				readReplyDic_.Add(locTableNo, receivedPacketIn);
			}

			// Set Flag to inform TableRequest that PAS has repsonsed
			DataObject.mrReadReplyEvent_.Set();
		}

		/// <summary>
		/// Process the Write reply packet
		/// </summary>
		/// <param name="receivedPacketIn">byte array of packet received from PAS</param>
		private void processWritePacket(byte[] receivedPacketIn)
		{
			if (receivedPacketIn == null)
			{
				Program.WriteEventLog("[ERROR] receivedPacketIn is null.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				return;
			}

			lock (writeCacheLock_)
			{
				int locTableNo = Util.Byte_To_Int(receivedPacketIn[1], receivedPacketIn[2]);
				Program.WriteDebugEventLog("[DEBUG] [WRITE] Insert table [" + locTableNo + "]", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

				if (writeReplyDic_.ContainsKey(locTableNo))
				{
					Program.WriteEventLog("[ERROR] [WRITE] Possible problem as [" + locTableNo + "] is removed from dictionary. ", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
					writeReplyDic_.Remove(locTableNo);
				}

				// Add received packet to dictionary
				writeReplyDic_.Add(locTableNo, receivedPacketIn);
			}

			// Set Flag to inform TableWrite that PAS has repsonsed
			DataObject.mrWriteReplyEvent_.Set();
		}
		#endregion

		#endregion

		/// <summary>
		/// The cleanUp method is for freeing up resource that was held by the class during runtime.
		/// </summary>
		private void cleanUp()
		{
			Program.WriteEventLog("[CLEANUP] " + MethodInfo.GetCurrentMethod().Name + " executed!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

			// Abort 
			if (monitorReceiveThread_ != null)
			{
				DataObject.socketClient.cleanUp();

				monitorReceiveThread_.Abort();
				monitorReceiveThread_.Join();
				Program.WriteEventLog("[INFO] monitorReceiveThread finished aborting.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
			}

			if (monitorSocketThread_ != null && monitorSocketThread_.IsAlive)
			{
				Program.WriteEventLog("[INFO] Aborting monitorSocketThread...", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				monitorSocketThread_.Abort();
			}
			else
			{
				Program.WriteEventLog("[INFO] monitorSocketThread - is already dead.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
			}

		}

		/****************************************************************************************************************************
		 * All communications with PA System goes through using the TableWrite and TableRead Method
		 * **************************************************************************************************************************/
		#region Most Basic PA Methods
		/// <summary>
		/// Request for tables from PAS
		/// </summary>
		/// <param name="tableNo">Requested table number</param>
		/// <param name="packetSize">Expected packet size (include 5 bytes for packet header)</param>
		/// <returns>Reply from PAS, include 5 bytes for packet header.</returns>
		/// <exception cref="SocketNotConnException">Throw SocketNotConnException if socket is not connected</exception>
		/// <exception cref="TimeOutException">Throw TimeOutException if timeout reached, request cannot be sent or if any other error occurs</exception>
		public static byte[] TableRequest(int tableNo, int packetSize)
		{
			// Nothing to validate : tableNo, packetSize
			int uniqueId = -1;
			bool sentSuccess = false;
			DateTime startTime = DateTime.Now;

			string requestMode = "R";
			byte[] bufferStream = new byte[3];
			byte[] receivedPacket = null;
			byte tableMsb = new byte();
			byte tableLsb = new byte();

			byte[] requestModeByte = null;

			try
			{
				if (!Check_PAS_Connection_Status)
				{
					throw new SocketNotConnException();
				}

				// To prevent flooding PA Fepc with too much data request
				if (tableNo == 200 || tableNo == 1200)
				{
					preZoneUpdateTime_ = DateTime.Now;
				}

				if (tableNo == 560)
				{
					preFlagUpdateTime_ = DateTime.Now;
				}

				uniqueId = DataObject.getUniqueCounter();

				Program.WriteDebugEventLog("[DEBUG] [UID: " + uniqueId + "] [Table Request] " + tableNo + " R - Preparing", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

				#region Preparing request packet
				requestModeByte = System.Text.ASCIIEncoding.ASCII.GetBytes(requestMode);
				Util.IntToByte(tableNo, out tableMsb, out tableLsb);

				bufferStream[0] = requestModeByte[0];
				bufferStream[1] = tableMsb;
				bufferStream[2] = tableLsb;
				#endregion

				lock (DataObject.chnLock)
				{
					sentSuccess = DataObject.socketClient.Send(bufferStream, 3);

					if (sentSuccess)
					{
						Program.writePacketLog("[INFO] [UID: " + uniqueId + "] [Table Request] [Sending successful] Table: [" + tableNo.ToString() + "]", bufferStream);

						while (true)
						{
							// Blocks till any reply is received
							bool pasSignal = DataObject.mrReadReplyEvent_.WaitOne(Program.pacTimeoutMs, false);

							if (pasSignal == false)
							{
								Program.WriteDebugEventLog("[ERROR] [UID: " + uniqueId + "] [Table Request] [Receiving Response] [FAILED] [Event Timeout] Table: [" + tableNo.ToString() + "]", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
								throw new TimeOutException(tableNo.ToString());
							}

							lock (readCacheLock_)
							{
								if (readReplyDic_.ContainsKey(tableNo))
								{
									// Table received
									receivedPacket = (byte[])readReplyDic_[tableNo].Clone();

									Program.writePacketLog("[INFO] [UID: " + uniqueId + "] [Table Request] [Receiving Response] [OK] Table: [" + tableNo.ToString() + "]", receivedPacket);

									if (tableNo != 200 && tableNo != 1200 && tableNo != 560)
									{
										readReplyDic_.Remove(tableNo);
									}

									DataObject.mrReadReplyEvent_.Reset();

									break;
								}
								else
								{
									double timeToSleep = 0;

									if (Program.reachInterval(startTime, Program.pacTimeoutMs, out timeToSleep))
									{
										// Means timeout occured
										Program.WriteDebugEventLog("[ERROR] [UID: " + uniqueId + "]  [Table Request] [Receiving Response] [FAILED] [Timeout] Table: [" + tableNo.ToString() + "]", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
										throw new TimeOutException(tableNo.ToString());
									}
									else
									{
										// Continue waiting
										Program.WriteDebugEventLog("[DEBUG] [WAIT] [UID: " + uniqueId + "]  [Table Request] [Receiving Response] [FAILED] Table: [" + tableNo.ToString() + "] Table not received! Keep waiting.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
										continue;
									}
								}
							}
						}
					}
					else
					{
						// Sending of request failed! DO NOT auto retry.
						Program.WriteDebugEventLog("[ERROR] [UID: " + uniqueId + "] [Table Request] [Sending failed] Table: [" + tableNo.ToString() + "] ", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
						throw new TimeOutException(tableNo.ToString());
					}
				}
			}
			catch (Exception ex)
			{
				Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				throw new TimeOutException(tableNo.ToString());
			}

			return receivedPacket;
		}

		/// <summary>
		/// Send and Receive Write request.
		/// </summary>
		/// <param name="tableNo">Table number</param>
		/// <param name="tableSize">Table size</param>
		/// <param name="dataToBeSend">byte[] containing the date to be sent</param>
		/// <param name="callingFunction">Name of the calling function</param>
		/// <returns>Reply from PA System</returns>
		/// <exception cref="SocketNotConnException">Throw SocketNotConnException if socket is not connected</exception>
		/// <exception cref="TimeOutException">Throw TimeOutException if timeout reached, request cannot be sent or if any other error occurs</exception>
		public static byte[] TableWrite(int tableNo, int tableSize, byte[] dataToBeSend, string callingFunction)
		{
			// Nothing to validate : tableNo, tableSize, dataToBeSend, callingFunction
			int uniqueId = -1;
			bool sentSuccess = false;
			DateTime startTime = DateTime.Now;

			string tableRequestMode = "W";
			byte tableMsb = new byte();
			byte tableLsb = new byte();
			byte tableSizeMsb = new byte();
			byte tableSizeLsb = new byte();
			byte[] tableRequestModeByte = null;
			byte[] bufferStream = null;
			byte[] receivedPacket = null;

			try
			{
				if (!Check_PAS_Connection_Status)
				{
					throw new SocketNotConnException();
				}

				uniqueId = DataObject.getUniqueCounter();

				Program.WriteDebugEventLog("[DEBUG] [UID: " + uniqueId + "]" + tableNo + " W - Preparing...", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

				#region Preparing write packet
				tableRequestModeByte = System.Text.ASCIIEncoding.ASCII.GetBytes(tableRequestMode);
				bufferStream = new byte[tableSize + Program.WRITE_HEADER_SIZE];

				Util.IntToByte(tableNo, out tableMsb, out tableLsb);
				Util.IntToByte(tableSize, out tableSizeMsb, out tableSizeLsb);

				// Message header
				bufferStream[0] = tableRequestModeByte[0];
				bufferStream[1] = tableMsb;
				bufferStream[2] = tableLsb;
				bufferStream[3] = tableSizeMsb;
				bufferStream[4] = tableSizeLsb;

				// Message data
				for (int i = 0; i < dataToBeSend.Length; i++)
				{
					bufferStream[i + 5] = dataToBeSend[i];
				}
				#endregion

				lock (DataObject.chnLock)
				{
					sentSuccess = DataObject.socketClient.Send(bufferStream, bufferStream.Length);

					if (sentSuccess)
					{
						Program.writePacketLog("[INFO] [UID: " + uniqueId + "] [Table Write] [Sending successful]  Table: [" + tableNo.ToString() + "]", bufferStream);

						while (true)
						{
							// Blocks till any reply is received
							bool pasSignal = DataObject.mrWriteReplyEvent_.WaitOne(Program.pacTimeoutMs, false);

							if (pasSignal == false)
							{
								Program.WriteDebugEventLog("[ERROR] [UID: " + uniqueId + "]  [Table Write] [Receiving Response] [FAILED] [Event Timeout] Table: [" + tableNo.ToString() + "]", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
								throw new TimeOutException(tableNo.ToString());
							}

							lock (writeCacheLock_)
							{
								if (writeReplyDic_.ContainsKey(tableNo))
								{
									// Table received
									receivedPacket = (byte[])writeReplyDic_[tableNo].Clone();
									Program.writePacketLog("[INFO] [UID: " + uniqueId + "] [Table Write] [Receiving Response] [OK] Table: [" + tableNo.ToString() + "]", receivedPacket);

									// Clear packet from dictionary
									writeReplyDic_.Remove(tableNo);

									DataObject.mrWriteReplyEvent_.Reset();

									break;
								}
								else
								{
									double timeToSleep = 0;

									if (Program.reachInterval(startTime, Program.pacTimeoutMs, out timeToSleep))
									{
										// Means timeout occured
										Program.WriteDebugEventLog("[ERROR] [UID: " + uniqueId + "]  [Table Write] [Receiving Response] [FAILED] [Timeout] Table: [" + tableNo.ToString() + "]", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
										throw new TimeOutException(tableNo.ToString());
									}
									else
									{
										// Continue waiting
										Program.WriteDebugEventLog("[ERROR] [WAIT] [UID: " + uniqueId + "]  [Table Write] [Receiving Response] [FAILED] Table: [" + tableNo.ToString() + "] Table not received! Keep waiting.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
										continue;
									}

								}
							}

						}
					}
					else
					{
						// Sending of write failed! DO NOT auto retry.
						Program.WriteDebugEventLog("[ERROR] [UID: " + uniqueId + "] [Table Write] [Sending failed] Table: [" + tableNo.ToString() + "] ", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
						throw new TimeOutException(tableNo.ToString());
					}
				}
			}
			catch (Exception ex)
			{
				Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				throw new TimeOutException(tableNo.ToString());
			}

			return receivedPacket;
		}

		#endregion

		/****************************************************************************************************************************
		 * Write Only Tables consists of the following,
		 * 201 / 1201	: Request for Broadcast
		 * 301 / 1301	: Messages sequence broadcast request
		 * 350			: Messages Sequence Specification
		 * 560			: Modification Flags
		 * 630			: Ad-Hoc Recording
		 * **************************************************************************************************************************/
		#region Write Only Tables
		/// <summary>
		/// Request for Broadcast (201/1201 W) 
		/// </summary>
		/// <param name="tableNo">Table number</param>
		/// <param name="sourceCode">Source code from List 50</param>
		/// <param name="zoneList">List of station zones to be broadcasted</param>
		/// <param name="abortBroadcast">true if abort broadcast</param>
		/// <param name="announceId">return the announceId being used for the current broadcast request</param>
		/// <returns>Reply from PAS, if error return byte array of equivalent size with first element set to 0xFF</returns>
		/// <exception cref="Exception">Throw exception when the given tableNo is wrong</exception>
		/// <exception cref="SocketNotConnException">Throw SocketNotConnException if socket is not connected</exception>
		/// <exception cref="TimeOutException">Throw TimeOutException if timeout reached, request cannot be sent or if any other error occurs</exception>
		public static byte[] Request_for_Broadcast(int tableNo, int sourceCode, byte[] zoneList, bool abortBroadcast, out int announceId)
		{
			// Nothing to validate : tableNo, sourceCode, zoneList, abortBroadcast, announceId

			/// Request for Broadcast (1201/201): ISCS client to send a request for broadcast for a list of station zones. 
			/// Attributes includes the source code, announce id (a running number), abort_broadcast indication (1=abort)
			/// and zone list containing the selected zones for broadcast

			// Table 201 - [REQUEST FOR BROADCAST]
			// Table 1201 - [GLOBAL REQUEST FOR BROADCAST]
			int tableSize = 0;
			int elementSize = 1;
			byte[] bufferStream = null;

			// Get announceId
			if (abortBroadcast)
			{
				announceId = 0;
				Program.WriteEventLog("[INFO] Aborting Broadcast", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
			}
			else
			{
				announceId = UserFunctions.getNextAnnounceId();
			}

			// Calculate the size of the table (excluding message header)
			if (tableNo == 201)
			{
				tableSize = (Program.MAXZONEID * elementSize) / Program.BITS_PER_BYTE + 2;
			}
			else if (tableNo == 1201)
			{
				tableSize = (Program.MAXSTNID * Program.MAXZONEID * elementSize) / Program.BITS_PER_BYTE + 2;
			}
			else
			{
				Program.WriteEventLog("[ERROR] Neither Table 201 nor 1201 detected.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				throw new Exception("TableNo must be 201 or 1201!");
			}

			Program.WriteDebugEventLog("[DEBUG] " + tableNo + " W - Preparing", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

			bufferStream = new byte[tableSize];

			// Filling in write request packet
			bufferStream[0] = Convert.ToByte(sourceCode);
			bufferStream[1] = Convert.ToByte(announceId);

			for (int i = 2; i < tableSize; i++)
			{
				bufferStream[i] = zoneList[i - 2];
			}

			Program.WriteDebugEventLog("[DEBUG] " + tableNo + " W - Prepare done", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

			return TableWrite(tableNo, tableSize, bufferStream, MethodInfo.GetCurrentMethod().Name);
		}

		/// <summary>
		/// Messages Sequence Broadcast Request (301/1301 W)
		/// </summary>
		/// <param name="dvaMsgSeq">DVA Message Sequence containing all details to be send to PAS</param>
		/// <returns>byte[] containing the reply of the dva broadcast request</returns>
		/// <exception cref="SocketNotConnException">Throw SocketNotConnException if socket is not connected</exception>
		/// <exception cref="TimeOutException">Throw TimeOutException if timeout reached, request cannot be sent or if any other error occurs</exception>
		public static byte[] Message_Sequence_Broadcast_Request(DvaMessageSequenceStruct dvaMsgSeq)
		{
			// Nothing to validate : dvaMsgSeq

			/// Messages Sequence Broadcast Request (1301/301): ISCS client to send a message seq for DVA broadcast.
			/// Attributes includes the message seq id(1-4), announce id, period of cyclic broadcast, Start and end time, event trigger, Coverage, and abort DVA bc (1 for abort).
			int tableNo = 0;
			int tableSize = 0;
			int elementSize = 1;
			byte[] bufferStream = null;

			byte[] periodSecs = new byte[2];
			byte[] startTime = new byte[4];
			byte[] stopTime = new byte[4];

			if (dvaMsgSeq.announceId == 0)
			{
				// Abort
				periodSecs = new byte[] { 0x0, 0x0 };
				startTime = new byte[] { 0x0, 0x0, 0x0, 0x0 };
				stopTime = new byte[] { 0x0, 0x0, 0x0, 0x0 };
			}
			else
			{
				// Not abort
				periodSecs = Util.IntTo2Byte(dvaMsgSeq.periodSecs);
				startTime = Util.DateTime_to_Byte(dvaMsgSeq.startTime);
				stopTime = Util.DateTime_to_Byte(dvaMsgSeq.stopTime);
			}

			if (Program.occMode)
			{
				tableNo = 1301;
				tableSize = ((Program.MAXSTNID * Program.MAXZONEID * elementSize) / Program.BITS_PER_BYTE) + 13;
			}
			else
			{
				tableNo = 301;
				tableSize = (Program.MAXZONEID * elementSize / Program.BITS_PER_BYTE) + 13;
			}

			bufferStream = new byte[tableSize];

			// Filling in write request packet
			bufferStream[0] = Convert.ToByte(dvaMsgSeq.msgSeqNo);
			bufferStream[1] = Convert.ToByte(dvaMsgSeq.announceId);
			bufferStream[2] = periodSecs[0];
			bufferStream[3] = periodSecs[1];
			bufferStream[4] = startTime[0];
			bufferStream[5] = startTime[1];
			bufferStream[6] = startTime[2];
			bufferStream[7] = startTime[3];
			bufferStream[8] = stopTime[0];
			bufferStream[9] = stopTime[1];
			bufferStream[10] = stopTime[2];
			bufferStream[11] = stopTime[3];
			bufferStream[12] = Convert.ToByte(dvaMsgSeq.eventTrigger);

			// Zones Coverage
			for (int i = 13; i < tableSize; i++)
			{
				if (dvaMsgSeq.announceId == 0)
				{
					bufferStream[i] = 0x0;
				}
				else
				{
					bufferStream[i] = dvaMsgSeq.zoneCoverage[i - 13];
				}
			}

			return TableWrite(tableNo, tableSize, bufferStream, MethodInfo.GetCurrentMethod().Name);
		}

		/// <summary>
		/// Messages Sequence Specification (350 W)
		/// </summary>
		/// <param name="msgSeqId">Index of concerned message in the message sequence table 303</param>
		/// <param name="chimeFlag">1=got chime, otherwise 0</param>
		/// <param name="dvaMessage">Size of 8 byte containing 4 DVA message number</param>
		/// <param name="dwellTime">Interval between 2 DVA message(unsigned word integer)</param>
		/// <returns>Reply from PAS, if error return byte array of equivalent size with first element set to 0xFF</returns>
		/// <exception cref="SocketNotConnException">Throw SocketNotConnException if socket is not connected</exception>
		/// <exception cref="TimeOutException">Throw TimeOutException if timeout reached, request cannot be sent or if any other error occurs</exception>
		public static byte[] Message_Sequence_Specification(int msgSeqId, int chimeFlag, int[] dvaMessage, int dwellTime)
		{
			// Nothing to validate :  msgSeqId, chimeFlag, dvaMessage, dwellTime

			/// Messages Sequence Specification (350): 
			/// ISCS client to send a message seq specification to PA to define the sequence of DVA messages to be broadcast. 
			/// Up to 4 DVA msg can be included in each seq.
			/// Attributes includes the message seq id(1-4), chime, DVA messages (up to 4), dwell time (time interval between each DVA)

			int tableNo = 350;
			int tableSize = 12;
			byte[] errorInput = new byte[4];
			byte[] bufferStream = new byte[tableSize];
			byte[] dwellTimeTmp = new byte[2];

			Program.WriteEventLog("[INFO] 350 W - Message Seq Specification", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

			//Error in msg sequence id
			if (msgSeqId > Program.MAXMSGSEQ)
			{
				Program.WriteEventLog("[ERROR] No Available Message Sequence ID", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				errorInput[0] = 0xFF;

				return errorInput;
			}

			// Filling in write request packet
			bufferStream[0] = Convert.ToByte(msgSeqId);
			bufferStream[1] = Convert.ToByte(chimeFlag);

			for (int k = 2; k < 10; k = k + 2)
			{
				byte[] dvaMsgByte = Util.IntTo2Byte(dvaMessage[(k / 2) - 1]);

				bufferStream[k] = dvaMsgByte[0];
				bufferStream[k + 1] = dvaMsgByte[1];
			}

			dwellTimeTmp = Util.IntTo2Byte(dwellTime);

			bufferStream[10] = dwellTimeTmp[0];
			bufferStream[11] = dwellTimeTmp[1];

			return TableWrite(tableNo, tableSize, bufferStream, MethodInfo.GetCurrentMethod().Name);
		}

		/// <summary>
		/// Modification Flags (560 W)
		/// </summary>
		/// <param name="flagData">Contains the flag details to be modified</param>
		/// <returns>Reply from PAS, if error return byte array of equivalent size with first element set to 0xFF</returns>
		/// <exception cref="SocketNotConnException">Throw SocketNotConnException if socket is not connected</exception>
		/// <exception cref="TimeOutException">Throw TimeOutException if timeout reached, request cannot be sent or if any other error occurs</exception>
		public static byte[] ModificationFlags(bool[] flagData)
		{
			// Nothing to validate : flagData

			/// Modification Flags (560): ISCS client to reply back the Mod flags table from PAS with the 
			/// relevant flags unchecked when the flagged changes have been viewed and updated by the ISCS. 

			int tableSize = 8;
			byte[] dataToBeSend = DataManipulation.BoolToByteArray_Flag(flagData);
			return TableWrite(560, tableSize, dataToBeSend, MethodInfo.GetCurrentMethod().Name);
		}
		#endregion

		/****************************************************************************************************************************
		 * Read and Write Only Tables consists of the following,
		 * 604	: Ad Hoc DVA Messages type
		 * **************************************************************************************************************************/
		#region Read and Write Only Tables
		/// <summary>
		/// Ad Hoc DVA Messages type (604)
		/// </summary>
		/// <param name="dataToBeSend">Contains the Message type (X, E, N) of the Ad Hoc messages to the PAS</param>
		/// <returns>Reply from PAS, if error return byte array of equivalent size with first element set to 0xFF</returns>
		/// <exception cref="SocketNotConnException">Throw SocketNotConnException if socket is not connected</exception>
		/// <exception cref="TimeOutException">Throw TimeOutException if timeout reached, request cannot be sent or if any other error occurs</exception>
		public static byte[] Adhoc_Update_DVA_MsgType(byte[] dataToBeSend)
		{
			// Nothing to validate : dataToBeSend

			/// Ad Hoc DVA Messages type (604): 
			/// ISCS to send the message type (X, E, N) of the Ad Hoc messages to the PAS

			int tableNo = 604;
			int tableSize = Program.NBADHOC;

			return TableWrite(tableNo, tableSize, dataToBeSend, MethodInfo.GetCurrentMethod().Name);
		}
		#endregion

		/// <summary>
		/// Update the falg table in cache
		/// </summary>
		public static void UpdateFlagTable()
		{
			Program.WriteDebugEventLog("[DEBUG] Call to update flag table executed.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

			byte[] flagDataIn = UserFunctions.TableRequest_ExcludeHeader(560, 8);
			DataObject.Set_Flag_Data(DataManipulation.ByteToBoolArray(flagDataIn));
		}

		/// <summary>
		/// Decide whether to poll PA System for Zone status
		/// </summary>
		/// <returns>bool: true if thread is to continue polling</returns>
		public static bool continuePollZone(out double timeToSleep)
		{
			if (Program.reachInterval(preZoneUpdateTime_, Program.fastPollIntervalMs, out timeToSleep) == false)
			{
				// Minimum poll interval NOT reached - do not poll - do not waste CPU power to update
				return false;
			}
			else
			{
				// Interval reached, continue to poll
				return true;
			}
		}

		/// <summary>
		/// Decide whether to poll PA System for Flag table
		/// </summary>
		/// <returns>bool: true if thread is to continue polling</returns>
		public static bool continuePollFlag(out double timeToSleep)
		{
			//double timeToSleep = 0;

			if (Program.reachInterval(preFlagUpdateTime_, Program.fastPollIntervalMs, out timeToSleep) == false)
			{
				// Minimum poll interval NOT reached - do not poll - do not waste CPU power to update
				return false;
			}
			else
			{
				// Interval reached, continue to poll
				return true;
			}
		}

	}
}
