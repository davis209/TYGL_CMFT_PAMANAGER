namespace ste.pa.pamanager
{
	/// <summary>
	/// Object to store Zone related attributes
	/// </summary>
	public class ZoneStruct
	{
		/// <summary>
		/// Station Name
		/// </summary>
		public string stnName = "";

		/// <summary>
		/// Zone Name
		/// </summary>
		public string zoneName = "";

		/// <summary>
		/// Zone Category 
		/// </summary>
		public string zoneCategory = "";

		/// <summary>
		/// Zone Type
		/// </summary>
		public string zoneType = "";

        /// <summary>
        /// Zone Type Id
        /// </summary>
        public int zoneTypeId = 0;

        /// <summary>
        /// Default Constructor
        /// </summary>
        public ZoneStruct()
		{
			stnName = "";
			zoneName = "";
			zoneCategory = "";
			zoneType = "";
            zoneTypeId = 0;
		}

        public byte zoneTypeIdBitValue
        {
            get
            {
                if(zoneTypeId == 1)
                {
                    return 1;
                }
                else if(zoneTypeId == 2)
                {
                    return 2;
                }
                else if(zoneTypeId == 3)
                {
                    return 4;
                }
                else if (zoneTypeId == 4)
                {
                    return 8;
                }
                return 0;
            }
        }

		///// <summary>
		///// Override the default Dva Message Sequence ToString method
		///// </summary>
		///// <returns>The customized string to be displayed</returns>
		//public override string ToString()
		//{
		//    return "Station [" + stnName +
		//            "] Zone [" + zoneName +
		//            "] Category [" + zoneCategory +
		//            "] Type [" + zoneType +
		//            "] Description [" + zoneDesc +
		//            "]";
		//}
	}
}
