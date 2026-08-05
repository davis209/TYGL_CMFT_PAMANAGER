#region Header Block
/// <classname>DataManipulation</classname> 
/// <description>Methods to manipulate byte arrays, etc.</description> 
/// <filename>DataManipulation.cs</filename> 
/// <authors>Tay Yu Hui</authors> 
#endregion

using System;				// Contain all other namespaces as subordinates
using System.Reflection;	// Retrieve information about assemblies, modules, members, parameters, and other entities in managed code by examining their metadata.

namespace ste.pa.pamanager
{
	/// <summary>
	/// Methods to manipulate byte arrays, etc.
	/// </summary>
	public class DataManipulation
	{
		private static string fileName_ = MethodBase.GetCurrentMethod().DeclaringType.Name;

		/// <summary>
		/// Static constructor 
		/// </summary>
		static DataManipulation()
		{
		}

		/// <summary>
		/// Convert the whole bool array to its equivalent byte array
		/// </summary>
		/// <param name="boolArray">bool array to be converted to byte array</param>
		/// <returns>bool array in byte array format</returns>
		public static byte[] BoolToByteArray(bool[] boolArray)
		{
			// Nothing to validate : boolArray

			// Create a clone to operate on, else the flipBoolArray() will affect the original array
			bool[] boolArrayClone = (bool[])boolArray.Clone();

			if (boolArrayClone.Length % 8 != 0)
			{
				Program.WriteEventLog("[ERROR] boolArray is not in multiple of 8.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
			}

			int byteCount = boolArrayClone.Length / 8;
			byte[] byteOut = new byte[byteCount];

			flipBoolArray(boolArrayClone);

			for (int byteIndex = 0; byteIndex < byteCount; byteIndex++)
			{
				int startIndex = ((byteIndex + 1) * 8) - 1;
				int byteSum = 0;

				for (int bit = 0; bit < 8; bit++)
				{
					if (boolArrayClone[startIndex - bit])
					{
						int tmpInt = (int)Math.Pow(2, bit);
						byteSum += tmpInt;
					}
				}

				byteOut[byteIndex] = (byte)byteSum;
			}

			return (byte[])byteOut.Clone();
		}

		/// <summary>
		/// Generate a list of zones selected for broadcast based on given station index
		/// </summary>
		/// <param name="boolArray">bool array containing selected zones of all stations</param>
		/// <param name="stnOffset">Offset of station within the bool array</param>
		/// <returns>List of zones that are selected for broadcast</returns>
		public static string Zone_BoolToString(bool[] boolArray, int stnOffset)
		{
			string zoneIdString = "";

			// Create a clone to operate on, else the flipBoolArray() will affect the original array
			bool[] boolArrayClone = (bool[])boolArray.Clone();
			int startIndex = Program.MAXZONEID * stnOffset;
			int endIndex = startIndex + Program.MAXZONEID;

			for (int i = startIndex; i < endIndex; i++)
			{
				if (boolArrayClone[i] == true)
				{
					zoneIdString += (i + 1) + ", ";
				}
			}

			return zoneIdString;
		}

		/// <summary>
		/// Convert the whole flag Bool array to its equivalent byte array
		/// </summary>
		/// <param name="flagBoolArray">bool array to be converted to byte array</param>
		/// <returns>bool array in byte array format</returns>
		public static byte[] BoolToByteArray_Flag(bool[] flagBoolArray)
		{
			// Nothing to validate : flagBoolArray

			// Create a clone to operate on, else the flipBoolArray() will affect the original array
			bool[] boolArrayClone = (bool[])flagBoolArray.Clone();

			if (boolArrayClone.Length % 8 != 0)
			{
				Program.WriteEventLog("[ERROR] boolArray is not in multiple of 8.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
			}

			int byteCount = boolArrayClone.Length / 8;
			byte[] byteOut = new byte[byteCount];

			// Flip Array in section of 8 bits
			int sectionCount = boolArrayClone.Length / Program.BITS_PER_BYTE;

			for (int i = 0; i < sectionCount; i++)
			{
				int offsetHead = i * Program.BITS_PER_BYTE;
				Array.Reverse(boolArrayClone, offsetHead, Program.BITS_PER_BYTE);
			}

			for (int byteIndex = 0; byteIndex < byteCount; byteIndex++)
			{
				int startIndex = ((byteIndex + 1) * 8) - 1;
				int byteSum = 0;

				for (int bit = 0; bit < 8; bit++)
				{
					if (boolArrayClone[startIndex - bit])
					{
						int tmpInt = (int)Math.Pow(2, bit);
						byteSum += tmpInt;
					}
				}

				byteOut[byteIndex] = (byte)byteSum;
			}

			return (byte[])byteOut.Clone();
		}

		/// <summary>
		/// Method to convert input byte[] to bool[], no shifting of the bits done
		/// </summary>
		/// <param name="byteArray">input byte array</param>
		/// <returns>equivalent bool array</returns>
		public static bool[] ByteToBoolArray(byte[] byteArray)
		{
			bool[] boolArray = null;

			// Nothing to validate : byteArray
			try
			{
				int dataLen = byteArray.Length * Program.BITS_PER_BYTE;
				boolArray = new bool[dataLen];

				string dataStr = "";

				for (int i = 0; i < byteArray.Length; i++)
				{
					string tmpStr = Convert.ToString(byteArray[i], 2);

					string tmpBit = padZeros(8, tmpStr);
					tmpBit = ReverseString(tmpBit);

					dataStr += tmpBit;
				}

				for (int i = 0; i < dataLen; i++)
				{
					if (dataStr.Substring(i, 1) == "1")
					{
						boolArray[i] = true;
					}
					else
					{
						boolArray[i] = false;
					}
				}

				Program.WriteDebugEventLog("[DEBUG] Flag: " + dataStr, fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

				//	flipBoolArray(boolArray);
			}
			catch (Exception ex)
			{
				Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
			}

			return boolArray;
		}

		/// <summary>
		/// Method to convert input byte[] to bool[], no shifting of the bits done
		/// </summary>
		/// <param name="byteArray">input byte array</param>
		/// <returns>equivalent bool array</returns>
		public static bool[] ByteToBoolArray_Zone(byte[] byteArray)
		{
			// Nothing to validate : byteArray
			bool[] boolArray = null;

			try
			{
				int dataLen = byteArray.Length * Program.BITS_PER_BYTE;
				boolArray = new bool[dataLen];

				string dataStr = "";

				for (int i = 0; i < byteArray.Length; i++)
				{
					string tmpStr = Convert.ToString(byteArray[i], 2);
					string tmpBit = padZeros(8, tmpStr);
					//tmpBit = ReverseString(tmpBit);

					dataStr += tmpBit;
				}

				for (int i = 0; i < dataLen; i++)
				{
					if (dataStr.Substring(i, 1) == "1")
					{
						boolArray[i] = true;
					}
					else
					{
						boolArray[i] = false;
					}
				}

				flipBoolArray(boolArray);
				//	flipBoolArray(boolArray);
			}
			catch (Exception ex)
			{
				Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
			}

			return boolArray;
		}

		/// <summary>
		/// Receives string and returns the string with its letters reversed.
		/// </summary>
		/// <param name="inputStr">String to be reversed</param>
		/// <returns>Reversed string</returns>
		private static string ReverseString(string inputStr)
		{
			// Nothing to validate : inputStr
			char[] charArr = inputStr.ToCharArray();
			Array.Reverse(charArr);

			return new string(charArr);
		}

		/// <summary>
		/// Convert integer to string format with the required number of string length padded with zero
		/// </summary>
		/// <param name="padCount">Required string count</param>
		/// <param name="bitString">Integer number to be converted</param>
		/// <returns>Padded format of integer value in string format</returns>
		private static string padZeros(int padCount, string bitString)
		{
			// Nothing to validate : padCount, bitString
			int intValue = int.Parse(bitString);
			string formatString = "{0:D" + padCount + "}";

			return string.Format(formatString, intValue);
		}

		/// <summary>
		/// Flip the bool values around in sections of 8 bytes
		/// </summary>
		/// <param name="boolArray">boolArray to be flip</param>
		/// <returns>Flipped boolArray</returns>
		private static void flipBoolArray(bool[] boolArray)
		{
			// Nothing to validate : boolArray
			int sectionCount = boolArray.Length / Program.MAXZONEID;
			//Console.WriteLine("flipBoolArray >> sectionCount: " + sectionCount);

			for (int i = 0; i < sectionCount; i++)
			{
				int offsetHead = i * Program.MAXZONEID;
				Array.Reverse(boolArray, offsetHead, Program.MAXZONEID);
			}
		}

		/// <summary>
		/// To transfer the input array (excluding the message header), to the local array
		/// </summary>
		/// <param name="tableSize">Size of the data (excluding the header size)y</param>
		/// <param name="receivedPacket">Packet received from PAS</param>
		/// <returns>Received Data with message header removed</returns>
		public static byte[] byteTransferExcludeHeader(int tableSize, byte[] receivedPacket)
		{
			// Nothing to validate : tableSize, receivedPacket
			try
			{
				byte[] localArray = new byte[tableSize];

				//first 5 bytes are message header
				for (int i = 0; i < tableSize; i++)
				{
					localArray[i] = receivedPacket[i + 5];
				}

				return localArray;
			}
			catch (Exception ex)
			{
				Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				return null;
			}
		}

		/// <summary>
		/// To transfer the required data of length "dataLength", from starting offset "dataArrayOffset" from array dataArray
		/// </summary>
		/// <param name="dataArrayOffset">Starting offset location of data to be copied</param>
		/// <param name="dataLength">Length of data to be copied, counted from dataArrayOffset</param>
		/// <param name="inputArray">Byte array of original data</param>
		/// <returns>Byte[] array of length "dataLength" from "dataArrayOffset"</returns>
		public static byte[] Variable_Byte_Transfer(int dataArrayOffset, int dataLength, byte[] inputArray)
		{
			// Nothing to validate : dataArrayOffset, dataLength, inputArray
			try
			{
				byte[] newArray = new byte[dataLength];

				for (int i = 0; i < dataLength; i++)
				{
					newArray[i] = inputArray[i + dataArrayOffset];
				}

				return newArray;
			}
			catch (Exception ex)
			{
				Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				return null;
			}
		}

		/// <summary>
		/// Append all selected dva message number to a string
		/// </summary>
		/// <param name="dvaMsgSeq">Dva Message Structure used in dva request</param>
		/// <returns>String containing all selected dva message number</returns>
		public static string getMsgIdStr(DvaMessageSequenceStruct dvaMsgSeq)
		{
			string msgIdStr = "";

			for (int i = 0; i < dvaMsgSeq.dvaMessage.Length; i++)
			{
				if (dvaMsgSeq.dvaMessage[i] != 0)
				{
					msgIdStr += dvaMsgSeq.dvaMessage[i].ToString() + ", ";
				}
			}

			return msgIdStr;
		}

	}
}
