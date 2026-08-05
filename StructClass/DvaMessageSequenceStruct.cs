using System;	// Contain all other namespaces as subordinates

namespace ste.pa.pamanager
{
	/// <summary>
	/// Object to store Dva Message Sequence related attributes
	/// </summary>
	public class DvaMessageSequenceStruct
	{
		/// <summary>
		/// Message slot used fpr broadcasting
		/// </summary>
		public int msgSeqNo = 0;

		/// <summary>
		/// DVA Source Code 
		/// </summary>
		public int dvaSourceCode = 0;

		#region Table 301/1301
		/// <summary>
		/// Announce Id
		/// </summary>
		public int announceId = 0;

		/// <summary>
		/// Period Broadcast in seconds
		/// </summary>
		public int periodSecs = 0;

		/// <summary>
		/// Start time of broadcast
		/// </summary>
		public DateTime startTime = new DateTime();

		/// <summary>
		/// End time of broadcast
		/// </summary>
		public DateTime stopTime = new DateTime();

		/// <summary>
		/// Is this broadcast triggered by an event?
		/// </summary>
		public int eventTrigger = 0;

		/// <summary>
		/// Selected zones for broadcast
		/// </summary>
		public byte[] zoneCoverage = null;
		#endregion

		#region Table 350
		/// <summary>
		/// Play Chime before broadcast starts?
		/// </summary>
		public int chimeFlag = 0;

		/// <summary>
		/// Messages selected for broadcast
		/// </summary>
		public int[] dvaMessage = null;

		/// <summary>
		/// Dwell time for broadcast
		/// </summary>
		public int dwellTime = 0;
		#endregion

		/// <summary>
		/// Constructor for DvaMessageSequenceStruct related attributes object initialization
		/// </summary>
		public DvaMessageSequenceStruct()
		{
			msgSeqNo = 0;
			dvaSourceCode = 0;
			announceId = 0;
			periodSecs = 0;
			startTime = new DateTime();
			stopTime = new DateTime();
			eventTrigger = 0;
			zoneCoverage = null;
			chimeFlag = 0;
			dvaMessage = new int[Program.MAXDVAPERSEQ];
			dwellTime = 0;
		}

		/// <summary>
		/// Override the default Dva Message Sequence ToString method
		/// </summary>
		/// <returns>The customized string to be displayed</returns>
		public override string ToString()
		{
			string tmpStr = "";
			for (int i = 0; i < zoneCoverage.Length; i++)
			{
				tmpStr = tmpStr + zoneCoverage[i] + ".";
			}

			return " Source Code: [" + dvaSourceCode +
					"] SeqId: [" + msgSeqNo +
					"] Chime: [" + chimeFlag +
					"] DVA Message: [" + dvaMessage[0] + ", " + dvaMessage[1] + ", " + dvaMessage[2] + ", " + dvaMessage[3] +
					"] Dwell: [" + dwellTime +
					"] Announce: [" + announceId +
					"] Period: [" + periodSecs +
					"] Start: [" + startTime +
					"] Stop: [" + stopTime +
					"] EventTriggered: [" + eventTrigger +
					"] ZoneCoverage: " + tmpStr +
					"]";
		}
	}
}
