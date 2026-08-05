#region Header Block
/// <classname>DataObject</classname> 
/// <description>Class Object to hold data</description> 
/// <filename>DataObject.cs</filename> 
/// <authors>Tay Yu Hui</authors> 
#endregion

using System.Collections.Generic;	// Allow subitems to be contained in a variety of containers
using System.Threading;				// Enable multithreaded programming

namespace ste.pa.pamanager
{
	/// <summary>
	/// Object to store all data used by the different modules in PA Manager
	/// </summary>
	public static class DataObject
	{
		#region Private variables
		// 32 rows * size 2 element 
		private static bool[] flagTable560Data_ = new bool[64];

		private static int uniqueCounter_ = 1;
		private static int pttSourceCode_ = 0x1;

		// Locker objects
		private static object uniqueCounterLock_ = new object();
		private static object pttSourceCodeLocker_ = new object();
		private static object channelFreeLocker_ = new object();

		// DVA and Live Broadcast Zone Selection
		private static bool[] selectedStnZone_ = new bool[Program.MAXZONEID];
		private static bool[] selectedOccZone_ = new bool[Program.MAXZONEID * Program.MAXSTNID];
		#endregion

		#region Public variables
		/// <summary>
		/// Used for notification when a read packet reply is received
		/// </summary>
		public static ManualResetEvent mrReadReplyEvent_ = new ManualResetEvent(false);

		/// <summary>
		/// Used for notification when a write packet reply is received
		/// </summary>
		public static ManualResetEvent mrWriteReplyEvent_ = new ManualResetEvent(false);

		/// <summary>
		/// Size of byte array required to store all zones in all station 
		/// </summary>
		public static int occCoverageSize = (Program.MAXZONEID * Program.MAXSTNID / Program.BITS_PER_BYTE); //200

		/// <summary>
		/// Size of byte array required to store all zones in a station 
		/// </summary>
		public static int stnCoverageSize = (Program.MAXZONEID / Program.BITS_PER_BYTE); //4

		/// <summary>
		/// Object holding details of all DVA Messages
		/// </summary>
		public static DvaMessageStruct[] dvaMessages = new DvaMessageStruct[Program.MAXDVAMSG]; //1000

		/// <summary>
		/// Object holding details of all zones in a station, used in station profile
		/// </summary>
		public static ZoneStruct[] zoneArray = new ZoneStruct[Program.MAXZONEID];

		/// <summary>
		/// Object holding details of all zones in all station, used in OCC profile
		/// </summary>
		public static ZoneStruct[] stnArray = new ZoneStruct[Program.MAXSTNID * Program.MAXZONEID];

		/// <summary>
		/// Object to lock the receiving buffer
		/// </summary>
		public static object receivingBufferLock = new object();

		/// <summary>
		/// Buffer storing all received packets
		/// </summary>
		public static List<byte> receivingBuffer = new List<byte>();

		/// <summary>
		/// Object to lock the socket to make sure at any one time, only a send/receive can be done
		/// </summary>
		public static object chnLock = new object();

		/// <summary>
		/// Reference to the socket class
		/// </summary>
		public static SocketCls socketClient = null;
		#endregion

		/// <summary>
		/// Static constructor 
		/// </summary>
		static DataObject()
		{
		}

		#region Get / Set methods
		/// <summary>
		/// [Station Mode] Store the selected station zones.
		/// </summary>
		public static bool[] Selected_Station_Zone
		{
			get { return selectedStnZone_; }
			//set { selectedStnZone_ = value; }
		}

		/// <summary>
		/// [OCC Mode] Store the selected station zones 
		/// </summary>
		public static bool[] Selected_OCC_Zone
		{
			get { return selectedOccZone_; }
			//set { selectedOccZone_ = value; }
		}
		#endregion

		/// <summary>
		/// Read Flag Bits
		/// </summary>
		/// <returns>Bool array containing flag data</returns>
		public static bool[] Read_Flag_Data()
		{
			lock (flagTable560Data_.SyncRoot)
			{
				return flagTable560Data_;
			}
		}

		/// <summary>
		/// Set the Flag Data
		/// </summary>
		/// <param name="flagBoolBits">bool[] format of the Flag Data</param>
		public static void Set_Flag_Data(bool[] flagBoolBits)
		{
			// Nothing to validate : flagBoolBits
			lock (flagTable560Data_.SyncRoot)
			{
				flagTable560Data_ = flagBoolBits;
			}
		}

		/// <summary>
		/// Set source code of the PTT Console (as decided in List 50)
		/// </summary>
		/// <param name="valueIn">Value of PTT Sourcecode</param>
		public static void Set_PTT_Source_Code(int valueIn)
		{
			// Nothing to validate : valueIn
			lock (pttSourceCodeLocker_)
			{
				pttSourceCode_ = valueIn;
			}
		}

		/// <summary>
		/// Read source code of the PTT Console (as decided in List 50)
		/// </summary>
		/// <returns>Source code in byte</returns>
		public static int Read_PTT_Source_Code()
		{
			lock (pttSourceCodeLocker_)
			{
				return pttSourceCode_;
			}
		}

		/// <summary>
		/// Generate an unique counter
		/// </summary>
		/// <returns>Generated unique counter</returns>
		public static int getUniqueCounter()
		{
			lock (uniqueCounterLock_)
			{
				int valueToReturn = uniqueCounter_;

				uniqueCounter_++;
				if (uniqueCounter_ >= int.MaxValue)
				{
					uniqueCounter_ = 1;
				}

				return valueToReturn;
			}
		}

	}
}
