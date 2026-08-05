using System;							// Contain all other namespaces as subordinates
using System.Net;						// Simple programming interface for many of the protocols used on networks today
using System.Net.NetworkInformation;	// Provides access to network traffic data, network address information, and notification of address changes for the local computer
using System.Net.Sockets;				// Implements the Windows Sockets (Winsock) interface to control access to the network.
using System.Reflection;				// Retrieve information about assemblies, modules, members, parameters, and other entities in managed code by examining their metadata. 
using System.Threading;					// Enable multithreaded programming

namespace ste.pa.pamanager
{
	/// <summary>
	/// Contains socket related methods
	/// </summary>
	public class SocketCls
	{
		#region Declaration
		private int bufferSize_ = 51300;
		private bool terminateConn_ = false;
		private string fileName_ = MethodBase.GetCurrentMethod().DeclaringType.Name;
		private Socket socketObj = null;
		private DateTime prevSendTime_ = DateTime.Now;

		/// <summary>
		/// Reference to a method used when there is data arriving at the receiving socket
		/// </summary>
		/// <param name="receivedData">Data received</param>
		public delegate void SocketDataArrival(byte[] receivedData);

		/// <summary>
		/// The callback method used in the delegate
		/// </summary>
		public SocketDataArrival EventDataArrival = null;

		/// <summary>
		/// Reference to a method used when there is an error at the socket
		/// </summary>
		public delegate void SocketConnError();

		/// <summary>
		/// The callback method used in the delegate
		/// </summary>
		public SocketConnError EventSocketErr = null;
		#endregion

		/// <summary>
		/// Initialize a TCP Client connection
		/// </summary>
		/// <param name="ipAddr">IP Address of host to connect to</param>
		/// <param name="portNo">Port Number to connect to</param>
		/// <returns>bool: true if socket can be connected, otherwise false</returns>
		public bool initSocket(String ipAddr, int portNo)
		{
			// Nothing to validate : ipAddr, portNo
			if (canPing(ipAddr))
			{
				// IPAddress and IPEndPoint represent the endpoint that will receive the request.
				IPAddress hostAddress = IPAddress.Parse(ipAddr);
				IPEndPoint hostEndPoint = new IPEndPoint(hostAddress, portNo);

				// Creates the Socket to send data over a TCP connection.
				socketObj = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

				// Connect to the host using its IPEndPoint.
				socketObj.Connect(hostEndPoint);
				terminateConn_ = false;
				return true;
			}
			else
			{
				Program.WriteEventLog("[ERROR] '" + ipAddr + "' ping failed!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				return false;
			}
		}

		/// <summary>
		/// Try to ping a given IP address
		/// </summary>
		/// <param name="ipToPing">IP address to ping</param>
		/// <returns>bool: true if IP can be ping, otherwise false</returns>
		public bool canPing(string ipToPing)
		{
			// Nothing to validate : ipToPing
			bool pingResult = false;
			Ping pingSender = new Ping();
			int timeOut = 500;

			try
			{
				PingReply pingReply = pingSender.Send(ipToPing, timeOut);

				if (pingReply.Status == IPStatus.Success)
				{
					pingResult = true;
				}
			}
			catch (Exception ex)
			{
				Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
			}

			return pingResult;

		}

		/// <summary>
		///  Following method sends size bytes stored in the packetToSend buffer from the offset position. 
		/// </summary>
		/// <param name="packetToSend">Data packet to be sent</param>
		/// <param name="packetSize">size of packet to be sent</param>
		/// <returns>bool: true if packet sent succesfully, otherwise false</returns>
		/// <exception cref="SocketException">Handle Socket exception</exception>
		/// <exception cref="Exception">Handle generic exception</exception>
		public bool Send(byte[] packetToSend, int packetSize)
		{
			// Nothing to validate : packetToSend, packetSize
			int startTickCount = Environment.TickCount;
			int byteSent = 0;	// how many bytes is already sent
			int offSet = 0;

			do
			{
				try
				{
					if (socketObj.Connected)
					{
						if (socketObj.Poll(10, SelectMode.SelectWrite))
						{
							byteSent += socketObj.Send(packetToSend, offSet + byteSent, packetSize - byteSent, SocketFlags.None);
							Program.WriteDebugEventLog("[DEBUG] Byte of size [" + byteSent + "] sent.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
						}
						//EventSocketAlive_();
					}
					else
					{
						Program.WriteEventLog("[ERROR] Socket not connected.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
						EventSocketErr();
						return false;
					}
				}
				catch (SocketException ex)
				{
					if (ex.SocketErrorCode == SocketError.WouldBlock ||
						ex.SocketErrorCode == SocketError.IOPending ||
						ex.SocketErrorCode == SocketError.NoBufferSpaceAvailable)
					{
						// socket buffer is probably full, wait and try again
						Thread.Sleep(100);
					}
					else
					{
						//any other serious error occur
						Program.WriteEventLog("[ERROR] [SocketException] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
						EventSocketErr();
						return false;
					}
				}
				catch (Exception ex)
				{
					Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
					return false;
				}
			}
			while (byteSent < packetSize);

			return true;
		}

		/// <summary>
		/// TCP Socket Receiving Thread
		/// </summary>
		/// <exception cref="SocketException">Handle Socket exception</exception>
		/// <exception cref="ThreadAbortException">Suppress Thread Abort Exception</exception>
		/// <exception cref="Exception">Handle generic exception</exception>

		public void ReceiveThread()
		{
			try
			{
				Program.WriteEventLog("[INFO] Starting receive thread to listen for packet from PA System.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

				// Set the socket buffer size, to reduce the need to append packet together
				socketObj.ReceiveBufferSize = bufferSize_;

				while (!terminateConn_)
				{
					try
					{
						//socket is in read mode
						if (socketObj.Connected == false)
						{
							Program.WriteEventLog("[ERROR] Socket was disconnected. Stopping Thread!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
							terminateConn_ = true;
						}
						else if (socketObj.Available != 0)
						{
							Program.WriteDebugEventLog("[DEBUG] A packet was received!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

							byte[] tmpArray = new byte[bufferSize_];
							int socketAvail = socketObj.Available;

							int receivedByteCount = socketObj.Receive(tmpArray, socketObj.Available, SocketFlags.None);

							if (receivedByteCount != 0)
							{
								byte[] receivedPacket = new byte[receivedByteCount];

								Array.Copy(tmpArray, 0, receivedPacket, 0, receivedByteCount);

								//EventSocketAlive_();
								EventDataArrival(receivedPacket);
							}
						}
						////}
						//else
						//{
						//    Program.WriteEventLog("[ERROR] Socket was disconnected. Thread stopped!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
						//    terminateConn_ = true;
						//    EventSocketErr();
						//}
					}
					catch (SocketException ex)
					{
						if (ex.SocketErrorCode == SocketError.WouldBlock ||
							ex.SocketErrorCode == SocketError.IOPending ||
							ex.SocketErrorCode == SocketError.NoBufferSpaceAvailable)
						{
							// socket buffer is probably empty, wait and try again
							Thread.Sleep(100);
						}
						else
						{
							// any other serious error occur
							terminateConn_ = true;
							EventSocketErr();
							Program.WriteEventLog("[ERROR] [SocketException] Code 1: " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
							//return;
						}
					}
					catch (Exception ex)
					{
						Program.WriteEventLog("[ERROR] [Exception]" + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
						terminateConn_ = true;
						EventSocketErr();
						//return;
					}

					Thread.Sleep(100);
				}

				Program.WriteEventLog("[INFO] Receive Thread ending.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
			}
			catch (ThreadAbortException ex)
			{
				Program.WriteEventLog("[ERROR] [ThreadAbortException] " + ex.Message, fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
			}
			catch (Exception ex)
			{
				Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
			}
		}

		/// <summary>
		/// Close the TCP socket thread
		/// </summary>
		public void cleanUp()
		{
			terminateConn_ = true;

			if (socketObj != null)
			{
				Program.WriteEventLog("[INFO] Closing Socket!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				socketObj.Close();
				Program.WriteEventLog("[INFO] Socket closed!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
			}
		}

	}
}
