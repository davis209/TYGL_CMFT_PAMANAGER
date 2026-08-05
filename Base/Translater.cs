using System;						// Contain all other namespaces as subordinates
using System.Reflection;			// Retrieve information about assemblies, modules, members, parameters, and other entities in managed code by examining their metadata. 
//using Oracle.DataAccess.Client;		// Provide Oracle Data Provider object which expose inherited, provider-specific, interface implementations of methods and properties
////using ste.scada.common;				// Provide commonly used classes for the creation of MFT
using System.Collections.Generic;
using System.Data;
using ste.common.exceptions;
using ste.common.util;
using ste.common.eventmanagement;
using ste.common.Forms;

namespace ste.pa.pamanager
{
	/// <summary>
	/// Translate the various ID to their equivalent Names, and vice versa.
	/// </summary>
	public class Translater
	{
		private static string fileName_ = MethodBase.GetCurrentMethod().DeclaringType.Name;
		private static string localSql_ = "";

		/// <summary>
		/// Static constructor 
		/// </summary>
		static Translater()
		{
		}

		/// <summary>
		/// Given Station Index, returns Station Location ID
		/// </summary>
		/// <param name="stnIndex">Station Index</param>
		/// <returns>Primary Key in SYS_LOCATION table</returns>
		public static int stnIndex_to_locationId(int stnIndex)
		{
            dbConnEnum dbConn = dbConnEnum.ErrNoConn;

            // Nothing to validate : stnIndex
            localSql_ = " SELECT LOCATION_ID FROM PA_CONFIG WHERE STATION_NO=" + stnIndex;

            var queries = new List<SqlQuery> { new SqlQuery { CommandText = localSql_ } };
            DataSet ds = Program.dbLock.FetchData(queries, ref dbConn);

            if (ds.Tables != null || ds.Tables.Count > 0)
            {
                foreach (DataRow retrievedRow in ds.Tables[0].Rows)
                {
                    return int.Parse(retrievedRow[0].ToString());
                }
            }
            return -1;
        }

		/// <summary>
		/// Given location Id, returns the name of the location
		/// </summary>
		/// <param name="locId">Primary Key in DB. Station Index. (e.g. 1, 2, 3)</param>
		/// <returns>Station location name. (e.g. A01)</returns>
		public static string locId_to_locationNo(int locId)
		{
            dbConnEnum dbConn = dbConnEnum.ErrNoConn;

			// Nothing to validate : stnIndex
			localSql_ = " SELECT NAME FROM LOCATION WHERE PKEY=" + locId.ToString();

            var queries = new List<SqlQuery> { new SqlQuery { CommandText = localSql_ } };
            DataSet ds = Program.dbLock.FetchData(queries, ref dbConn);

            if (ds.Tables != null || ds.Tables.Count > 0)
            {
                foreach (DataRow retrievedRow in ds.Tables[0].Rows)
                {
                    return retrievedRow[0].ToString();
                }
            }

            return string.Empty;
		}

		/// <summary>
		/// Given Station Index, returns Station Number
		/// </summary>
		/// <param name="stnIndex">Station Index. (e.g. 1, 2, 3)</param>
		/// <returns>Station number.</returns>
		public static int stnIndex_to_StnNo(int stnIndex)
		{
			dbConnEnum dbConn = dbConnEnum.ErrNoConn;
			int stnNo = 0;

			// Nothing to validate : stnIndex
			localSql_ = " SELECT STATION_NO FROM PA_CONFIG WHERE LOCATION_ID=" + stnIndex;
            var queries = new List<SqlQuery> { new SqlQuery { CommandText = localSql_ } };
            DataSet ds = Program.dbLock.FetchData(queries, ref dbConn);

            if (ds.Tables != null || ds.Tables.Count > 0)
            {
                foreach (DataRow retrievedRow in ds.Tables[0].Rows)
                {
                    stnNo = Int32.Parse(retrievedRow[0].ToString());
                    break;
                }
            }

            return stnNo;
		}

		/// <summary>
		/// Retrieves location ID of profile 
		/// </summary>
		/// <returns>Location Id of profile</returns>
		public static int getProfileLocationId()
		{
			int locationId = 0;
			dbConnEnum dbConn = dbConnEnum.ErrNoConn;

			string profileName = ScadaAuthentication.getLoginProfile(Program.dbLock, true);

			localSql_ = " SELECT LOCATION_ID FROM USR_PROFILE WHERE PROFILE_NAME = '" + profileName + "'";
            var queries = new List<SqlQuery> { new SqlQuery { CommandText = localSql_ } };
            DataSet ds = Program.dbLock.FetchData(queries, ref dbConn);

            if (ds.Tables != null || ds.Tables.Count > 0)
            {
                foreach (DataRow retrievedRow in ds.Tables[0].Rows)
                {
                    locationId = Int32.Parse(retrievedRow[0].ToString());
                    break;
                }
            }

            return locationId;
		}

		/// <summary>
		/// Gets the Zone Type given location Id and Zone Num
		/// </summary>
		/// <param name="locationId">Location ID</param>
		/// <param name="zoneNo">Zone number</param>
		/// <returns>Location Id(PK in SYS_LOCATION) of the current logged in user</returns>
		public static string Get_Zone_Type(string locationId, string zoneNo)
		{
			// Nothing to validate : locationId, zoneNo
			int typeId = 0;
			dbConnEnum dbConn = dbConnEnum.ErrNoConn;

			try
			{
				localSql_ = " SELECT ZONE_TYPE_ID FROM PA_ZONES WHERE LOCATION_ID=" + locationId + " AND ZONE_NO=" + zoneNo;
                var queries = new List<SqlQuery> { new SqlQuery { CommandText = localSql_ } };
                DataSet ds = Program.dbLock.FetchData(queries, ref dbConn);

                if (ds.Tables != null || ds.Tables.Count > 0)
                {
                    foreach (DataRow retrievedRow in ds.Tables[0].Rows)
                    {
                        return retrievedRow[0].ToString();
                    }
                }
            }
			catch (Exception ex)
			{
				Program.WriteEventLog("[ERROR] Connection: " + dbConn.ToString() + " Exception: " + ex.Message, fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				throw;
			}

			return typeId.ToString();
		}

	}
}
