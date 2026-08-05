using System;	// Contain all other namespaces as subordinates

namespace ste.pa.pamanager
{
	/// <summary>
	/// Object to store Datagridview display related attributes
	/// </summary>
	public class DgvDisplayStruct : IComparable
	{
		/// <summary>
		/// Station Name
		/// </summary>
		public string stnName { get; set; }

		/// <summary>
		/// Zone Name
		/// </summary>
		public string zoneName { get; set; }

		/// <summary>
		/// Zone Status
		/// </summary>
		public string zoneStatus { get; set; }

		/// <summary>
		/// Zone Index
		/// </summary>
		public int zoneIndex { get; set; }

		/// <summary>
		/// Default Constructor
		/// </summary>
		public DgvDisplayStruct()
		{
			stnName = "";
			zoneName = "";
			zoneStatus = "";
			zoneIndex = -1;
		}

		///// <summary>
		///// Override the default DgvDisplayStruct ToString method
		///// </summary>
		///// <returns>The customized string to be displayed</returns>
		///// <since>00.03.01.00</since>
		//public override string ToString()
		//{
		//    return "Station: [" + stnName + "] Zone: [" + zoneName + "] Status: [" + zoneStatus + "] Index: [" + zoneIndex + "]";
		//}

		/// <summary>
		/// Compares the current instance with another object of the same type 
		/// </summary>
		/// <param name="objIn">Object to be compared</param>
		/// <returns>An integer that indicates whether the current instance precedes, 
		/// follows, or occurs in the same position in the sort order as the other object.
		/// </returns>
		public int CompareTo(object objIn)
		{
			// Nothing to validate : objIn
			DgvDisplayStruct tempDisUnit = (DgvDisplayStruct)objIn;

			if (this.zoneIndex > tempDisUnit.zoneIndex)
			{
				return 1;
			}
			else if (this.zoneIndex < tempDisUnit.zoneIndex)
			{
				return -1;
			}
			else
			{
				return 0;
			}

		}
	}
}
