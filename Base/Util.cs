using System;		// Contain all other namespaces as subordinates

namespace ste.pa.pamanager
{
	/// <summary>
	/// Class to convert the different data types into Byte format and vice versa
	/// </summary>
	public class Util
	{
		private static object fileWriteLock_ = new object();

		/// <summary>
		/// Get current time span in seconds since Jan 1970
		/// </summary>
		/// <param name="timeDt">Current Date Time</param>
		/// <returns>Current time in seconds format since 1970</returns>
		public static long GetCurrentSec(DateTime timeDt)
		{
			// Nothing to validate : timeDt
			DateTime baseDateTime = new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Utc);
			TimeSpan timeSpan = timeDt - baseDateTime;

			return (long)timeSpan.TotalSeconds;
		}

		/// <summary>
		/// Given time in seconds format, convert to DateTime format
		/// </summary>
		/// <param name="timeSeconds">Time in seconds format</param>
		/// <returns>Time in DateTime format</returns>
		public static DateTime GetDateTime(long timeSeconds)
		{
			// Nothing to validate : timeSeconds
			DateTime baseDateTime = new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Utc);
			TimeSpan secSincebaseDt = new TimeSpan(0, 0, (int)timeSeconds);

			return (DateTime)(baseDateTime + secSincebaseDt);
		}

		/// <summary>
		/// Convert an interger table number to its equivalent msb and lsb in byte
		/// </summary>
		/// <param name="intValue">Integer Value to be converted</param>
		/// <param name="msbByte">Returns the most significant bit of converted integer value</param>
		/// <param name="lsbByte">Returns the least significant bit of converted integer value</param>
		public static void IntToByte(int intValue, out byte msbByte, out byte lsbByte)
		{
			// Nothing to validate : intValue, msbByte, lsbByte
			int intMod = intValue % 256;
			msbByte = Convert.ToByte((intValue - intMod) / 256);
			lsbByte = Convert.ToByte(intMod);
		}

		/// <summary>
		/// Convert an most significant bit and least significant bit value to its equivalent integer form
		/// </summary>
		/// <param name="msbValue">Most significant bit value(MSB)</param>
		/// <param name="lsbValue">Least significant bit value(LSB)</param>
		/// <returns>The Integer value calculated from the MSB and LSB</returns>
		public static int Byte_To_Int(int msbValue, int lsbValue)
		{
			// Nothing to validate : msbValue, lsbValue
			return (msbValue * 256) + lsbValue;
		}

		/// <summary>
		/// Parse time from Byte[] format to Datetime format. (Add back umt+8 timing)
		/// </summary>
		/// <param name="dtByte">Datetime format in Byte[] form</param>
		/// <returns>Equivalent time format in DateTime format</returns>
		public static DateTime Byte_To_DateTime(Byte[] dtByte)
		{
			// Nothing to validate : dtByte
			DateTime timeDt = new DateTime();
			long timeLong = 0;

			timeLong = Convert.ToInt16(dtByte[2]) * 256 + Convert.ToInt16(dtByte[3]);
			timeLong = timeLong + Convert.ToInt16(dtByte[1]) * 256 * 256;
			timeLong = timeLong + Convert.ToInt16(dtByte[0]) * 256 * 256 * 256;
			timeLong = timeLong + Program.utcTimeOffset;

			timeDt = DateTime.SpecifyKind(Util.GetDateTime(timeLong), DateTimeKind.Utc);

			return timeDt;
		}

		/// <summary>
		/// Parse date and time value to equivalent DateTime format
		/// </summary>
		/// <param name="dateValue">DateTime format of date</param>
		/// <param name="timeValue">DateTime format of time</param>
		/// <returns>DateTime format of Date and Time</returns>
		public static DateTime parseTime(DateTime dateValue, DateTime timeValue)
		{
			DateTime dateTimeDT = DateTime.Parse(dateValue.ToShortDateString() + " " + timeValue.ToLongTimeString());
			DateTime dateTimeDT2 = DateTime.SpecifyKind(dateTimeDT, DateTimeKind.Utc);

			return dateTimeDT2;
		}

		/// <summary>
		/// Parse time value in long format to equivalent format in Byte[]
		/// </summary>
		/// <param name="dateTimeValue">Date Time in seconds format</param>
		/// <returns>Byte array of size 4 containing the timeValue</returns>
		public static byte[] parseTime(long dateTimeValue)
		{
			// Nothing to validate : dateTimeValue
			byte[] timeByte = new byte[4];

			int timeInt1 = (int)(dateTimeValue % 256);
			int tmpInt2 = (int)((dateTimeValue - timeInt1) / 256);
			int timeInt2 = (int)((tmpInt2 % 256));
			int tmpInt3 = (int)((tmpInt2 - timeInt2) / 256);
			int timeInt3 = (int)((tmpInt3 % 256));
			int timeInt4 = (int)((tmpInt3 - timeInt3) / 256);

			timeByte[3] = Convert.ToByte(timeInt1);
			timeByte[2] = Convert.ToByte(timeInt2);
			timeByte[1] = Convert.ToByte(timeInt3);
			timeByte[0] = Convert.ToByte(timeInt4);

			return timeByte;
		}

		/// <summary>
		/// Convert DateTime value to byte[] equivalent
		/// </summary>
		/// <param name="dateTimeValue">Time in DateTime format</param>
		/// <returns>Time in Byte form</returns>
		public static byte[] DateTime_to_Byte(DateTime dateTimeValue)
		{
			// Nothing to validate : dateTimeValue
			long startTimeLg = GetCurrentSec(dateTimeValue) - Program.utcTimeOffset; //190710
			byte[] dateTimeByte = parseTime(startTimeLg);

			return dateTimeByte;
		}

		/// <summary>
		/// Convert an integer number to its equivalent byte[] value
		/// </summary>
		/// <param name="intValue">Integer Value to be converted</param>
		/// <returns>Integer value in Byte form</returns>
		public static byte[] IntTo2Byte(int intValue)
		{
			// Nothing to validate : intValue
			byte[] byteValue = new byte[2];

			int intMod = intValue % 256;
			byteValue[0] = Convert.ToByte((intValue - intMod) / 256);
			byteValue[1] = Convert.ToByte(intMod);

			return byteValue;
		}

		/// <summary>
		/// Check if the receivedPacket indicates that the PAS timeout
		/// </summary>
		/// <param name="receivedPacket">Packet received from PAS</param>
		/// <returns>bool: true if timeout occurs, otherwise false</returns>
		public static bool PAS_Timeout(byte[] receivedPacket)
		{
			// Nothing to validate : receivedPacket
			if (receivedPacket[0] == 255)
			{
				return true;
			}
			else
			{
				return false;
			}
		}

	}
}
