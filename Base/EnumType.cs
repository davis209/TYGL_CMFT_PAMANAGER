namespace ste.pa.pamanager
{
	/// <summary>
	/// Object to hold the various enum types
	/// </summary>
	public class EnumType
	{
		/// <summary>
		/// The various events that are raised by PA Manager and its event ID
		/// </summary>
		public enum PaEvent
		{
			PaStart = 1001,
			PaStop = 1002,
			PaLiveBroadcast = 2000,
			PaLiveBroadcastFail = 2001,
			PaDvaBroadcast = 2002,
			PaDvaBroadcastFail = 2003,
			PaDvaCyclicBroadcast = 2004,
			PaDvaCyclicBroadcastFail = 2005,
			PaDvaPlayback = 2006,
			PaDvaPlaybackFail = 2007,
			PaAdhocAdd = 2008,
			PaAdhocAddFail = 2009,
			PaAdhocEdit = 2010,
			PaAdhocEditFail = 2011,
			PaAdhocDelete = 2012,
			PaAdhocDeleteFail = 2013,
			PaZoneAdd = 2014,
			PaZoneEdit = 2016,
			PaZoneDelete = 2017,
			PaSysConfig = 2018,
			PaMftConfigSc = 2019,
			PaBgmStart = 2020,
			PaBgmStartFail = 2021,
			PaBgmStop = 2022,
			PaBgmStopFail = 2023,
		}

		/// <summary>
		/// Packet Type and its equivalent value in ASCII
		/// </summary>
		public enum PacketType
		{
			Write = 87,
			Read = 82,
		}

		/// <summary>
		/// Packet Error code as returned by PAS
		/// </summary>
		public enum PacketError
		{
			PAS_ERR_OK = 0,
			PAS_ERR_GENERIC = 1,
			PAS_ERR_BUSY_MESSAGE_SEQ = 2,
			PAS_ERR_BUSY_SOURCE = 3,
			PAS_ERR_BUSY_RECORDING = 4,
			PAS_ERR_BUSY_BROADCASTING = 5,
			PAS_ERR_INVALID_ANNOUNCE = 6,
			PAS_ERR_NO_FLAG = 7,
		}

		/// <summary>
		/// Flag Index as defined in Table 560
		/// </summary>
		public enum FlagIndex
		{
			Flag202 = 1,
			Flag204 = 2,
			Flag1204 = 3,
			Flag302 = 4,
			Flag303 = 5,
			Flag1303 = 6,
			Flag590 = 9,
			Flag603 = 10,
			Flag604 = 11,
			Flag610 = 12,
			Flag1610 = 13,
		}

		/// <summary>
		/// Message Type and its equivalent value in ASCII
		/// </summary>
		public enum MessageType
		{
			Emergency = 69,
			Normal = 78,
			None = 88,
		}

		/// <summary>
		/// The state of the Zone depending on the sourcecode
		/// </summary>
		public enum EZoneScState
		{
			// ScMatch: Given Sourcecode matches Zone's sourcecode
			ScMatch = 0,

			// ScNotDefine: Given Sourcecode doesn't match that of Zone's sourcecode. (Zone's sourcecode is 0)
			ScNotDefine = 1,

			// MismatchSc: Given Sourcecode doesn't match that of Zone's sourcecode. (Zone's sourcecode is NOT 0)
			MismatchSc = 2,

			// FaultyZone: Zone Amplifier is down (take precedence over mis-matched sourcecode)
			FaultyZone = 3,
		}

		/// <summary>
		/// The various state of a zone during a broadcast
		/// </summary>
		public enum EZoneBcState
		{
			// Given Sourcecode matches Zone's sourcecode, and Zone Amplifier is working
			Normal = 1,

			// Zone Amplifier is down (Sourcecode checking not required)
			Failed = 2,

			//  Given Sourcecode doesn't match that of Zone's sourcecode. (Zone's sourcecode is NOT 0)
			Overridden = 3,

			// Zone broadcast was aborted. (Zone's sourcecode is 0)
			Aborted = 4,

			Completed = 5,
			Initiated = 6,
			PendingBroadcast = 7,
		}

		/// <summary>
		/// The various stages in a Live Broadcast
		/// </summary>
		public enum LiveBcStatus
		{
			Starting = 0,
			Initiated = 1,
			InProgress = 2,
			BcEnded = 3,
			BcAborted = 4,
			BcNoWorkingZone = 5,
			Timeout = 6,
			BcEndedFailZone = 7,
			Unknown = 8,
		}

		/// <summary>
		/// The various Zone stages in a Live Broadcast
		/// </summary>
		public enum LiveBcZoneStatus
		{
			InProgress = 1,
			ZoneFail = 2,
			ZoneOverridden = 3,
			zoneAborted = 4,
			ZoneCompleted = 5,
			ZoneInitiated = 6,
		}

		/// <summary>
		/// The various stages in a DVA Broadcast
		/// </summary>
		public enum DvaBcStatus
		{
			Initiated = 0,
			InProgress = 1,
			Unknown2 = 2,
			DwellTime = 3,
			NoAvailChn = 4,
			DvaMsgInUseInRecord = 5,
			DvaChnOverride = 6,
			Unknown7 = 7,
			BcFinishAdnormal = 8,
			BcFinishNoFail = 9,
			BcFinishGotFail = 10,
			ReqFailed = 11,
			NoWorkingZones = 12,
		}

        public enum StationStatus
        {
            Offline = 0,
            Busy = 1,
            Availabel = 2
        }

        /// <summary>
        /// The various stage in a Adhoc recording session
        /// </summary>
        public enum AdhocRecordStage
		{
			Initiated = 1,
			InProgess = 2,
			Completed = 3,
			Unknown = 4,
		}

		/// <summary>
		/// The various exception code
		/// </summary>
		public enum ExceptionCode
		{
			NoDbConn = 0,
			PasError = 1,
			GeneralException = 2,
			PasReplyTimeout = 3,
			PasNoConn = 4,
		}

	}

}
