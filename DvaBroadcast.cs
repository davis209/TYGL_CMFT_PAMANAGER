#region Header Block
/// <classname>DvaBroadcast</classname> 
/// <description>Form to show the status of a DVA Broadcast</description> 
/// <filename>DvaBroadcast.cs</filename> 
/// <authors>Tay Yu Hui</authors> 
/// <version>00.00.00.00</version> 
/// <changes>Nil</changes> 
/// <todo>Nil</todo> 
/// <copyright>Copyright @ 2006-2010 Singapore Technologies Electronics Ltd</copyright>
#endregion
using System;						// Contain all other namespaces as subordinates
using System.Collections.Generic;	// Allow subitems to be contained in a variety of containers 
using System.ComponentModel;		// Provide classes used to implement the behavior of components and controls
using System.Reflection;			// Retrieve information about assemblies, modules, members, parameters, and other entities in managed code by examining their metadata. 
using System.Threading;				// Enable multithreaded programming
using System.Windows.Forms;			// Contain classes for creating Window-based applications
//using ste.scada.common;				// Provide commonly used classes for the creation of MFT 
using ste.common.exceptions;
using ste.common.util;
using ste.common.Forms;
using ste.dmd.cos;

namespace ste.pa.pamanager
{
	/// <summary>
	/// Form to show the status of a DVA Broadcast
	/// </summary>
	public partial class DvaBroadcast : Form
	{
		/// <summary>
		/// Reference to a method used to resume the fast poll checking
		/// </summary>
		public delegate void resumeBcCheckDelegate();

		/// <summary>
		/// The callback method used in the resume the fast poll checking delegate
		/// </summary>
		public resumeBcCheckDelegate resumeBcCheck = null;

		/// <summary>
		/// Reference to a method used to pause the fast poll checking
		/// </summary>
		public delegate void pauseBcCheckDelegate();

		/// <summary>
		/// The callback method used in the pause fast poll checking delegate
		/// </summary>
		public pauseBcCheckDelegate pauseBcCheck = null;

		private Thread threadUpdateProgress_ = null;

		private object lockList_ = new object();
		private delegate void ListBoxClearCallback(Control oControl);
		private delegate void ListBoxAddCallback(Control oControl, string msgIn);
		private delegate void DgvDataSourceCallback(Control oControl, DgvDisplayStruct[] dgvDisplayStruct);

		private bool canCloseForm_ = true;
		private bool newExecute_ = false;
		private bool abortDva_ = false;
		private bool dvaInitCompleted_ = false;

		private string stnName_ = "";
		private string fileName_ = MethodBase.GetCurrentMethod().DeclaringType.Name;

		// Setting of DVA Message to be sent
		private DvaMessageSequenceStruct dvaBcSettings_ = new DvaMessageSequenceStruct();

		// Announce ID currently in the interested PAS Source
		private int pasScAnnounceId_ = 0;

		// Status of DVA Broadcast
		private static EnumType.DvaBcStatus prevPaDvaStatus_ = EnumType.DvaBcStatus.Initiated;
		private static EnumType.DvaBcStatus paDvaStatus_ = EnumType.DvaBcStatus.Initiated;

		// Array Size
		private static int maxOccZone_ = Program.MAXSTNID * Program.MAXZONEID;
		private static int stnByteArraySize_ = Program.MAXZONEID / Program.BITS_PER_BYTE;
		private static int occByteArraySize_ = Program.MAXSTNID * Program.MAXZONEID / Program.BITS_PER_BYTE;

		// Common attributes
		private List<DgvDisplayStruct> successList_ = new List<DgvDisplayStruct>();
		private List<DgvDisplayStruct> failedList_ = new List<DgvDisplayStruct>();

		// Selected broadcast zones
		private bool[] selBcZone_ = null;

		// Previous completed broadcast zones
		private bool[] completedZone_ = null;

		// Playing Zone
		private bool[] playingZone_ = null;

		// Previous failed zones
		private bool[] failZone_ = null;

		// Previous failed zones status
		private EnumType.EZoneBcState[] failZoneStatus_ = null;

		// Selected zones that fails during the broadcast
		private bool[] failedSelZones_ = null;

		// STIS PIDS emergency display
		private ICosDmdServer stisAgent_ = null;
		private Tuple<int[], int> stisLocationAndScheduleIds_ = null;
		private object stisLock_ = new object();

		private string sysErr_ = "DVA Broadcast";
		private string errFormLoadFail_ = "DVA Broadcast Form fails to load!";
		private string mbOverwriteFail_ = "DVA Broadcast CANNOT be overwritten!";
		private string mbWarnBcStillInProgress_ = "Broadcast still in progress.";
		private string mbBcAbortFail_ = "DVA Broadcast Abort FAIL!";
		private string mbCfmAbort_ = "Aborting broadcast in Failed zones?";
		private string mbRetryFail_ = "Error in retrying broadcast. Please try again!";

		private string statusBcAborted_ = "DVA Broadcast Aborted.";

		// Chime setting
		private string lbClosingForm_ = "Closing DVA Broadcast window.";
		private string lbChimeOff_ = "Off";
		private string lbChimeOn_ = "On";

		// Zone Status
		private string dgvLoc_ = "Location";
		private string dgvZone_ = "Zone";
		private string dgvStatus_ = "Status";
		private string dgvIndex_ = "Index";
		private string zoneInProgress_ = "In progress";
		private string zoneFailed_ = "Failed zone";
		private string zoneOverridden_ = "Overridden";
		private string zoneAborted_ = "Aborted";
		private string zoneComplete_ = "Completed";
		private string zoneInit_ = "Initiated";
		private string zonePending_ = "Pending Broadcast";

		/// <summary>
		/// Static constructor 
		/// </summary>
		static DvaBroadcast()
		{
		}

		/// <summary>
		/// This is a constructor to initialize the DvaBroadcast class and its variables and components.
		/// </summary>
		/// <param name="dvaBcSettings">Contains the DVA Broadcast setting to be displayed</param>
		public DvaBroadcast(DvaMessageSequenceStruct dvaBcSettings)
		{
			// Nothing to validate : dvaBcSettings
			this.dvaBcSettings_ = dvaBcSettings;

			newExecute_ = true;

			InitializeComponent();
		}

		/// <summary>
		/// This method sets the variables to its localized equivalent text.
		/// </summary>
		private void localizeForm()
		{
			System.Globalization.CultureInfo ci = new System.Globalization.CultureInfo(Program.defaultLanguage);
			Localization.setCulture(ci);
			Localization.setApplicationName(Program.moduleName);

			if (Localization.configure())
			{
				this.Text = Localization.localize("DVA_FORM_NAME", this.Text);
				sysErr_ = Localization.localize("DVA_FORM_NAME", sysErr_);

				errFormLoadFail_ = Localization.localize("DVA_FORM_LOAD_FAIL", errFormLoadFail_);
				mbOverwriteFail_ = Localization.localize("DVA_MB_OVERWRITE_FAIL", mbOverwriteFail_);
				mbWarnBcStillInProgress_ = Localization.localize("DVA_MB_BC_INPROGRESS", mbWarnBcStillInProgress_);
				mbBcAbortFail_ = Localization.localize("DVA_MB_ABORTED_FAIL", mbBcAbortFail_);
				mbCfmAbort_ = Localization.localize("DVA_MB_CFM_ABORT", mbCfmAbort_);

				lbClosingForm_ = Localization.localize("DVA_BCSTATUS_CLOSING", lbClosingForm_);
				lbChimeOff_ = Localization.localize("DVA_CHIME_OFF", lbChimeOff_);
				lbChimeOn_ = Localization.localize("DVA_CHIME_ON", lbChimeOn_);

				statusBcAborted_ = Localization.localize("DVA_BCSTATUS_ABORTED", statusBcAborted_);

				zoneInProgress_ = Localization.localize("ZONESTATUS_INPROGRESS", zoneInProgress_);
				zoneFailed_ = Localization.localize("ZONESTATUS_FAILEDZONE", zoneFailed_);
				zoneOverridden_ = Localization.localize("ZONESTATUS_OVERRIDDEN", zoneOverridden_);
				zoneAborted_ = Localization.localize("ZONESTATUS_ABORTED", zoneAborted_);
				zoneComplete_ = Localization.localize("ZONESTATUS_COMPLETED", zoneComplete_);
				zoneInit_ = Localization.localize("ZONESTATUS_INITIATED", zoneInit_);
				zonePending_ = Localization.localize("DVA_ZONESTATUS_PENDING", zonePending_);

				but_Dva_Retry_Sel.Text = Localization.localize("BUT_RETRY", but_Dva_Retry_Sel.Text);
				but_Dva_Retry_All.Text = Localization.localize("BUT_RETRYALL", but_Dva_Retry_All.Text);
				//but_overwrite.Text = Localization.localize("BUT_OVERWRITE", but_overwrite.Text);
				but_abort.Text = Localization.localize("BUT_ABORT", but_abort.Text);
				but_close.Text = Localization.localize("BUT_CLOSE", but_close.Text);
				grpBox_Dva_Zone_Progress.Text = Localization.localize("GP_ZONE_PROGRESS", grpBox_Dva_Zone_Progress.Text);
				grpBox_Dva_Zone_Failure.Text = Localization.localize("GP_ZONE_FAILURE", grpBox_Dva_Zone_Failure.Text);
				grpBox_Dva_Bc_config.Text = Localization.localize("GP_BC_CONFIG", grpBox_Dva_Bc_config.Text);
				grpBox_Dva_Bc_status.Text = Localization.localize("GP_BC_STATUS", grpBox_Dva_Bc_status.Text);

				label_start.Text = Localization.localize("LB_START", label_start.Text);
				label_stop.Text = Localization.localize("LB_STOP", label_stop.Text);
				label_chime.Text = Localization.localize("LB_CHIME", label_chime.Text);
				label_dwell.Text = Localization.localize("LB_DWELL", label_dwell.Text);
				label_period.Text = Localization.localize("LB_PERIOD", label_period.Text);
				label_selMsg.Text = Localization.localize("LB_SELMSG", label_selMsg.Text);

				dgvLoc_ = Localization.localize("DGV_LOC", dgvLoc_);
				dgvZone_ = Localization.localize("DGV_ZONE", dgvZone_);
				dgvStatus_ = Localization.localize("DGV_STATUS", dgvStatus_);
				dgvIndex_ = Localization.localize("DGV_INDEX", dgvIndex_);

				mbRetryFail_ = Localization.localize("MB_RETRY_FAIL", mbRetryFail_);
			}
		}

		/// <summary>
		/// Method invoked when the DVA Broadcast form is loaded
		/// </summary>
		/// <param name="sender">Control that raised the event</param>
		/// <param name="e">Additional information about the event that was raised</param>
		/// <exception cref="TimeOutException">Handle timeout exception</exception>
		/// <exception cref="TableException">Handle table exception</exception>
		/// <exception cref="Exception">Handle generic exception</exception>
		private void DvaBroadcast_Load(object sender, EventArgs e)
		{
			// Nothing to validate : sender, e
			try
			{
				localizeForm();

				if (Program.occMode)
				{
					selBcZone_ = new bool[maxOccZone_];
					completedZone_ = new bool[maxOccZone_];
					playingZone_ = new bool[maxOccZone_];
					failZone_ = new bool[maxOccZone_];
					failZoneStatus_ = new EnumType.EZoneBcState[maxOccZone_];
				}
				else
				{
					stnName_ = Translater.locId_to_locationNo(Program.profileLocIndex);

					selBcZone_ = new bool[Program.MAXZONEID];
					completedZone_ = new bool[Program.MAXZONEID];
					playingZone_ = new bool[Program.MAXZONEID];
					failZone_ = new bool[Program.MAXZONEID];
					failZoneStatus_ = new EnumType.EZoneBcState[Program.MAXZONEID];
				}

				if (backgroundWorker_Dva_SubmitRequest.IsBusy != true)
				{
					backgroundWorker_Dva_SubmitRequest.RunWorkerAsync();
				}
			}
			catch (Exception ex)
			{
				Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				Program.MessageBox_Error(errFormLoadFail_, sysErr_);
				canCloseForm_ = true;

				this.Close();
			}
		}

		/// <summary>
		/// Operator clicks the window Close button 
		/// </summary>
		/// <param name="sender">Control that raised the event</param>
		/// <param name="e">Additional information about the event that was raised</param>
		private void DVABroadcast_FormClosing(object sender, FormClosingEventArgs e)
		{
			// Nothing to validate : sender, e
			if (backgroundWorker_Dva_SubmitRequest.IsBusy)
			{
				backgroundWorker_Dva_SubmitRequest.CancelAsync();
			}

			if (abortDva_ == false && canCloseForm_ == false)
			{
				// Display "Broadcast still in progress."
				Program.MessageBox_Warn(mbWarnBcStillInProgress_, sysErr_, this);
				e.Cancel = true;
			}
			else
			{
				if (paDvaStatus_ == EnumType.DvaBcStatus.BcFinishAdnormal ||
					paDvaStatus_ == EnumType.DvaBcStatus.BcFinishNoFail)
				{
					if (noFailZones())
					{
						if (UserFunctions.DVA_Abort_MsgSeq(dvaBcSettings_.msgSeqNo))
						{
							Program.WriteEventLog("[INFO] DVA - Message Sequence Slot No " + dvaBcSettings_.msgSeqNo + " abort successful.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
						}
						else
						{
							Program.WriteEventLog("[ERROR] DVA - Message Sequence Slot No " + dvaBcSettings_.msgSeqNo + " abort FAILED!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
						}
					}
					else
					{
						askOperatorToClose(e);
					}

				}
				else if (paDvaStatus_ == EnumType.DvaBcStatus.BcFinishGotFail)
				{
					askOperatorToClose(e);
				}
				else
				{
					// Note: Recheck here, in the case where the form is immediately close after first load
					Program.MessageBox_Warn(mbWarnBcStillInProgress_ + ".!", sysErr_, this);
					Program.WriteEventLog("[ERROR] Unknown PA Dva Status: " + paDvaStatus_, fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				}

				cleanUp();
			}
		}

		/// <summary>
		/// Ask operator if he/she want to close the form
		/// </summary>
		/// <param name="e">The form closing event</param>
		/// <since>00.03.18.02</since>
		private void askOperatorToClose(FormClosingEventArgs e)
		{
			// Nothing to validate : e
			if (!abortDva_ && STEMessageBox.Show(mbCfmAbort_, sysErr_, Program.defaultLanguage, MsgBoxButton.YesNo, MessageBoxIcon.Question) == DialogResult.No)
			{
				// Operator dun want to close form
				e.Cancel = true;
			}
			else
			{
				// Operator wants to close form
				if (UserFunctions.DVA_Abort_MsgSeq(dvaBcSettings_.msgSeqNo))
				{
					Program.WriteEventLog("[INFO] DVA - Message Sequence Slot No " + dvaBcSettings_.msgSeqNo + " abort successful.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				}
				else
				{
					Program.WriteEventLog("[ERROR] DVA - Message Sequence Slot No " + dvaBcSettings_.msgSeqNo + " abort FAILED!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				}

			}
		}

		/// <summary>
		/// Method invoked when form is closing
		/// </summary>
		/// <exception cref="Exception">Handle generic exception</exception>
		private void cleanUp()
		{
			Program.WriteEventLog("[CLEANUP] " + MethodInfo.GetCurrentMethod().Name + " executed!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
			DeleteEmergencyMsgInStis();
			UpdateProgress_Thread_Stop();
		}

		private void ConnectToStisServer()
		{
			stisAgent_ = (ICosDmdServer)NamingManager.Instance.getActiveAgentRef("StisServer", 1);

			if (stisAgent_ == null)
			{
				Program.WriteEventLog("[ERROR] STIS agent is null, failed to connect to STIS server", fileName_);
			}
		}

		private void SendEmergencyMsgToStis()
		{
			int retryCount = 0;

			try
			{
				lock (stisLock_)
				{
					string[] selDvaMsg = UserFunctions.Query_MsgNo_To_MsgLabel(dvaBcSettings_.dvaMessage);
					string message = string.Join(" ", selDvaMsg);

					if (string.IsNullOrEmpty(message))
						return;

					// Extract distinct station indices that have selected zones
					List<int> locationIds = new List<int>();

					if (Program.occMode)
					{
						for (int i = 0; i < Program.MAXSTNID; i++)
						{
							for (int j = 0; j < Program.MAXZONEID; j++)
							{
								if (selBcZone_[i * Program.MAXZONEID + j])
								{
									locationIds.Add(i + 1);
									break;
								}
							}
						}
					}
					else
					{
						locationIds.Add(Program.profileLocIndex);
					}

					if (locationIds.Count == 0)
						return;

					int[] locationId = locationIds.ToArray();

					while (retryCount < 2)
					{
						if (stisAgent_ != null)
						{
							Program.WriteEventLog(string.Format("[INFO] Send to STIS server. Location id - {0}, message - {1}",
								string.Join(", ", locationId), message), fileName_);

							int scheduleId = stisAgent_.cosDisplayPaEmgMsg(locationId, message.Trim(), 0);
							stisLocationAndScheduleIds_ = Tuple.Create(locationId, scheduleId);

							Program.WriteEventLog(string.Format("[INFO] Done sending to STIS server. Schedule Id {0}", scheduleId), fileName_);
							break;
						}
						else
						{
							ConnectToStisServer();
							retryCount++;
						}
					}
				}
			}
			catch (Exception ex)
			{
				Program.WriteEventLog("[ERROR] Exception - send emergency message to STIS: " + ex.Message, fileName_);
			}
		}

		private void DeleteEmergencyMsgInStis()
		{
			int retryCount = 0;

			try
			{
				if (stisLocationAndScheduleIds_ == null)
					return;

				while (retryCount < 2 && stisAgent_ == null)
				{
					ConnectToStisServer();
					retryCount++;
				}

				if (stisAgent_ != null)
				{
					Program.WriteEventLog(string.Format("[INFO] Delete emergency message in STIS. Location id - {0}, scheduleId - {1}",
						string.Join(", ", stisLocationAndScheduleIds_.Item1), stisLocationAndScheduleIds_.Item2), fileName_);

					stisAgent_.cosDeletePaEmgMsg(stisLocationAndScheduleIds_.Item1, stisLocationAndScheduleIds_.Item2);
					stisLocationAndScheduleIds_ = null;
				}
			}
			catch (Exception ex)
			{
				Program.WriteEventLog("[ERROR] Exception - delete emergency message in STIS: " + ex.Message, fileName_);
			}
		}

		/// <summary>
		/// Occurs when RunWorkerAsync is called.
		/// </summary>
		/// <param name="sender">Control that raised the event</param>
		/// <param name="e">Additional information about the event that was raised</param>
		/// <exception cref="NoOracleDBException">Handle No Oracle DB Exception</exception>
		/// <exception cref="TimeOutException">Handle the timeout exception</exception>
		/// <exception cref="Exception">Handle generic exception</exception>
		private void backgroundWorker_Dva_SubmitRequest_DoWork(object sender, DoWorkEventArgs e)
		{
			// Nothing to validate : sender, e
			int connFailCounter_ = 0;
			int maxFailAttempt_ = 10;
			byte[] defineMsgFlag = null;

			string[] eventParam = null;
			string[] eventParamLocal = null;

			string bcReqFailReason = "";
			string bcReqFailReasonLocal = "";

			try
			{
				// Reset everything at the start of every new broadcast
				ListBoxClear(listBox_Dva_Bc_status);

				prevPaDvaStatus_ = EnumType.DvaBcStatus.Initiated;
				paDvaStatus_ = EnumType.DvaBcStatus.Initiated;

				but_abort.Enabled = true;

				abortDva_ = false;
				dvaInitCompleted_ = false;

				// Disable all retry buttons until broadcast is completed
				but_Dva_Retry_All.Enabled = false;
				but_Dva_Retry_Sel.Enabled = false;

				Program.WriteEventLog("[INFO] Submit request thread started! (Using message sequence " + dvaBcSettings_.msgSeqNo + ")", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

				Display_Bc_Info();
				Determine_Bc_Zones();

				// 303 R - Check for free message descriptor - was done before this form was launched
				// Start Update thread first
				UpdateProgress_Thread_Start();

				while (true)
				{
					if (backgroundWorker_Dva_SubmitRequest.CancellationPending == true)
					{
						Program.WriteEventLog("[INFO] Thread is being cancelled!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
						e.Cancel = true;
						break;
					}

					// 350 W - Define sequence message to be broadcast
					defineMsgFlag = PaFunctions.Message_Sequence_Specification(dvaBcSettings_.msgSeqNo, dvaBcSettings_.chimeFlag, dvaBcSettings_.dvaMessage, dvaBcSettings_.dwellTime);

					if (defineMsgFlag[0] != 255)
					{
						if (Convert.ToInt32(defineMsgFlag[3]) == 0)
						{
							// No Error in 350 W, proceed to send 301/1301 W
							byte[] receivedData = PaFunctions.Message_Sequence_Broadcast_Request(dvaBcSettings_);

							// 301 W -  Start request to broadcast message, and define the time and coverage
							if (Convert.ToInt32(receivedData[3]) == 0)
							{
								Program.WriteEventLog("[INFO] DVA broadcast request successful.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

								string chimeStatus = "";
								string chimeStatusLocal = "";
								string msgIdStr = DataManipulation.getMsgIdStr(dvaBcSettings_);

								if (dvaBcSettings_.chimeFlag == 1)
								{
									chimeStatus = Localization.GetEventMessage("EVT_LB_HAVE", "en-US");
									chimeStatusLocal = Localization.GetEventMessage("EVT_LB_HAVE", "zh-TW");
								}
								else
								{
									chimeStatus = Localization.GetEventMessage("EVT_LB_NO_HAVE", "en-US");
									chimeStatusLocal = Localization.GetEventMessage("EVT_LB_NO_HAVE", "zh-TW");
								}

								// DVA Cyclic Broadcast Request succesfully submitted
								if (Program.occMode)
								{
									//{{OPERATOR}} @ {{MACHINE}} submitted a DVA Broadcast request. Station {{STNNO}}, Zone: {{ZONEID}}, From {{STARTTIME}} to {{ENDTIME}}, Chime: {{CHIME}}, MSG: {{MSGID}}, PERIOD: {{PERIOD}}, DWELL: {{DWELL}}
									for (int i = 0; i < Program.MAXSTNID; i++)
									{
										if (Program.stnNoToStnNameDic_.ContainsKey(i + 1))
										{
											string stnIdStr = Program.stnNoToStnNameDic_[i + 1];
											string zoneIdString = DataManipulation.Zone_BoolToString(selBcZone_, i);

											if (zoneIdString != "")
											{
												eventParam = new string[] { "", "", stnIdStr, zoneIdString, dvaBcSettings_.startTime.ToString(), dvaBcSettings_.stopTime.ToString(), chimeStatus, msgIdStr, dvaBcSettings_.periodSecs.ToString(), dvaBcSettings_.dwellTime.ToString() };
												eventParamLocal = new string[] { "", "", stnIdStr, zoneIdString, dvaBcSettings_.startTime.ToString(), dvaBcSettings_.stopTime.ToString(), chimeStatusLocal, msgIdStr, dvaBcSettings_.periodSecs.ToString(), dvaBcSettings_.dwellTime.ToString() };

												Program.logEvent(EnumType.PaEvent.PaDvaBroadcast, eventParam, eventParamLocal);
											}
										}
									}
								}
								else
								{
									string stnIdStr = Translater.locId_to_locationNo(Program.profileLocIndex);

									eventParam = new string[] { "", "", stnIdStr, DataManipulation.Zone_BoolToString(selBcZone_, 0), dvaBcSettings_.startTime.ToString(), dvaBcSettings_.stopTime.ToString(), chimeStatus, msgIdStr, dvaBcSettings_.periodSecs.ToString(), dvaBcSettings_.dwellTime.ToString() };
									eventParamLocal = new string[] { "", "", stnIdStr, DataManipulation.Zone_BoolToString(selBcZone_, 0), dvaBcSettings_.startTime.ToString(), dvaBcSettings_.stopTime.ToString(), chimeStatusLocal, msgIdStr, dvaBcSettings_.periodSecs.ToString(), dvaBcSettings_.dwellTime.ToString() };

									Program.logEvent(EnumType.PaEvent.PaDvaBroadcast, eventParam, eventParamLocal);
								}

								SendEmergencyMsgToStis();
								dvaInitCompleted_ = true;
								break;
							}
							else if (Convert.ToInt32(receivedData[3]) == (int)EnumType.PacketError.PAS_ERR_INVALID_ANNOUNCE)
							{
								connFailCounter_++;

								if (connFailCounter_ == maxFailAttempt_)
								{
									paDvaStatus_ = EnumType.DvaBcStatus.ReqFailed;
									bcReqFailReason = DvaBcStatus(paDvaStatus_, "en-US") + " : " + Localization.GetEventMessage("EVT_DVA_BC_REQ_FAILED", "en-US");
									bcReqFailReasonLocal = DvaBcStatus(paDvaStatus_, "zh-TW") + " : " + Localization.GetEventMessage("EVT_DVA_BC_REQ_FAILED", "zh-TW");

									Program.WriteEventLog("[ERROR] Invalid Announce ID... max retry reached!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
									break;
								}
								else
								{
									// Announce ID in use, auto retry using another announce ID
									Program.WriteEventLog("[ERROR] Retry send request. Error: " + Program.returnPacketError(receivedData[3], "en-US"), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
									dvaBcSettings_.announceId = UserFunctions.getNextAnnounceId();
								}
							}
							else
							{
								// Got error in 301 / 1301 W Broadcast Request, no retry 
								paDvaStatus_ = EnumType.DvaBcStatus.ReqFailed;
								bcReqFailReason = Localization.GetEventMessage("EVT_DVA_BC_REQ_FAILED", "en-US") + " : " + Program.returnPacketError(receivedData[3], "en-US");
								bcReqFailReasonLocal = Localization.GetEventMessage("EVT_DVA_BC_REQ_FAILED", "zh-TW") + " : " + Program.returnPacketError(receivedData[3], "zh-TW");

								Program.WriteEventLog("[ERROR] 301 W Error: " + bcReqFailReason, fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
								break;
							}
						}
						else
						{
							// Got error in 350 W Broadcast Request, no retry 
							paDvaStatus_ = EnumType.DvaBcStatus.ReqFailed;
							bcReqFailReason = DvaBcStatus(paDvaStatus_, "en-US") + " : " + Program.returnPacketError(defineMsgFlag[3], "en-US");
							bcReqFailReasonLocal = DvaBcStatus(paDvaStatus_, "zh-TW") + " : " + Program.returnPacketError(defineMsgFlag[3], "zh-TW");

							Program.WriteEventLog("[ERROR] 350 W Error: " + bcReqFailReason, fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
							break;
						}
					}
					else
					{
						connFailCounter_++;

						if (connFailCounter_ == maxFailAttempt_)
						{
							paDvaStatus_ = EnumType.DvaBcStatus.ReqFailed;
							bcReqFailReason = DvaBcStatus(paDvaStatus_, "en-US") + " : " + Localization.GetEventMessage("EVT_NO_PAS_LINK", "en-US");
							bcReqFailReasonLocal = DvaBcStatus(paDvaStatus_, "zh-TW") + " : " + Localization.GetEventMessage("EVT_NO_PAS_LINK", "zh-TW");

							Program.WriteEventLog("[ERROR] " + Program.LB_PasConnectionFail, fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
							break;
						}
					}
				}

				// Stop update thread if request fails
				if (dvaInitCompleted_ == false)
				{
					paDvaStatus_ = EnumType.DvaBcStatus.ReqFailed;

					if (Program.defaultLanguage == "en-US")
					{
						ListBoxAppend(listBox_Dva_Bc_status, bcReqFailReason);
					}
					else
					{
						ListBoxAppend(listBox_Dva_Bc_status, bcReqFailReasonLocal);
					}

					eventParam = new string[] { "", "", bcReqFailReason };
					eventParamLocal = new string[] { "", "", bcReqFailReasonLocal };
					Program.logEvent(EnumType.PaEvent.PaDvaBroadcastFail, eventParam, eventParamLocal);

					if (Program.occMode)
					{
						DVA_Stage_Error_OCC();
					}
					else
					{
						DVA_Stage_Error();
					}

					UpdateProgress_Thread_Stop();
				}
			}
			catch (NoOracleDBException ex)
			{
				Program.WriteEventLog("[ERROR] [NoOracleDBException] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				ListBoxAppend(listBox_Dva_Bc_status, Program.returnExceptionString(EnumType.ExceptionCode.NoDbConn));
			}
			catch (TimeOutException ex)
			{
				Program.WriteEventLog("[ERROR] [TimeOutException] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				ListBoxAppend(listBox_Dva_Bc_status, Program.returnExceptionString(EnumType.ExceptionCode.PasError));
			}
			catch (Exception ex)
			{
				Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				ListBoxAppend(listBox_Dva_Bc_status, Program.returnExceptionString(EnumType.ExceptionCode.GeneralException));
			}
		}

		/// <summary>
		/// Occurs when the background operation has completed, has been canceled, or has raised an exception.
		/// </summary>
		/// <param name="sender">Control that raised the event</param>
		/// <param name="e">Additional information about the event that was raised</param>
		/// <exception cref="Exception">Handle generic exception</exception>
		private void backgroundWorker_Dva_SubmitRequest_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
		{
			// Nothing to validate : sender, e
			try
			{
				// Start timer to update progress
				if (e.Cancelled)
				{
					Program.WriteEventLog("[INFO] Operator aborted the DVA Request!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
					UpdateProgress_Thread_Stop();
				}
				else
				{
					if (dvaInitCompleted_)
					{
						//pauseBcCheck();
						Program.WriteEventLog("[INFO] Submit request completed! Do not stop Update Thread.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
					}
					else
					{
						canCloseForm_ = true;
						Program.WriteEventLog("[ERROR] Submit request failed!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
					}
				}
			}
			catch (Exception ex)
			{
				canCloseForm_ = true;
				Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
			}
		}

		/// <summary>
		/// Display on HMI the Bc information
		/// </summary>
		private void Display_Bc_Info()
		{
			string[] selDvaMsg = UserFunctions.Query_MsgNo_To_MsgLabel(dvaBcSettings_.dvaMessage);

			displayConfigListBox(ref listBox_period, dvaBcSettings_.periodSecs.ToString());
			displayConfigListBox(ref listBox_start, dvaBcSettings_.startTime.ToLongDateString() + " " + dvaBcSettings_.startTime.ToLongTimeString());
			displayConfigListBox(ref listBox_stop, dvaBcSettings_.stopTime.ToLongDateString() + " " + dvaBcSettings_.stopTime.ToLongTimeString());
			displayConfigListBox(ref listBox_chime, DvaChimeStatus(dvaBcSettings_.chimeFlag + 1));
			displayConfigListBox(ref listBox_dwell, dvaBcSettings_.dwellTime.ToString());

			// Show the requested messages 
			listBox_msg_no.Items.Clear();
			for (int i = 0; i < selDvaMsg.Length; i++)
			{
				listBox_msg_no.Items.Add(selDvaMsg[i]);
			}

		}

		/// <summary>
		/// Determine the zones to be broadcasted
		/// </summary>
		private void Determine_Bc_Zones()
		{
			if (Program.occMode)
			{
				if (!noFailZones() && !newExecute_)
				{
					selBcZone_ = (bool[])failedSelZones_.Clone();
				}
				else
				{
					selBcZone_ = (bool[])DataObject.Selected_OCC_Zone.Clone();
				}

				// Update zone coverage before sending request to PAS
				dvaBcSettings_.zoneCoverage = DataManipulation.BoolToByteArray(selBcZone_);
			}
			else
			{
				if (!noFailZones() && !newExecute_)
				{
					selBcZone_ = (bool[])failedSelZones_.Clone();
				}
				else
				{
					selBcZone_ = (bool[])DataObject.Selected_Station_Zone.Clone();
				}

				// Update zone coverage before sending request to PAS
				dvaBcSettings_.zoneCoverage = DataManipulation.BoolToByteArray(selBcZone_);
			}
		}

		/// <summary>
		/// Start Update Progress Thread
		/// </summary>
		/// <exception cref="Exception">Handle generic exception</exception>
		private void UpdateProgress_Thread_Start()
		{
			try
			{
				Program.WriteEventLog("[INFO] Start Update progress thread first.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				pauseBcCheck();

				threadUpdateProgress_ = new Thread(new ThreadStart(UpdateProgress));
				threadUpdateProgress_.Name = "DvaProgress";
				threadUpdateProgress_.IsBackground = true;
				threadUpdateProgress_.Start();

				canCloseForm_ = false;
			}
			catch (Exception ex)
			{
				Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
			}
		}

		/// <summary>
		/// Stop Update Progress Thread
		/// </summary>
		/// <exception cref="ThreadAbortException">Handle the thread abort exception</exception>
		/// <exception cref="Exception">Handle generic exception</exception>
		private void UpdateProgress_Thread_Stop()
		{
			try
			{
				canCloseForm_ = true;
				resumeBcCheck();

				if (threadUpdateProgress_ != null)
				{
					if ((threadUpdateProgress_.ThreadState & (ThreadState.Stopped | ThreadState.Unstarted)) == 0) //.Running)
					{
						threadUpdateProgress_.Abort();
						Program.WriteEventLog("[INFO] Aborting Update Thread! [" + threadUpdateProgress_.Name + "]", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
					}
				}
			}
			catch (Exception ex)
			{
				Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
			}
		}

		/// <summary>
		/// Update the progress of the DVA broadcast
		/// </summary>
		/// <exception cref="TableException">Handle Table exception</exception>
		/// <exception cref="TimeOutException">Handle Timeout exception</exception>
		/// <exception cref="Exception">Handle generic exception</exception>
		private void UpdateProgress()
		{
			try
			{
				Program.WriteEventLog("[INFO] Update Progress Thread started!", MethodInfo.GetCurrentMethod().Name);

				// Stage - Initiated 
				ListBoxAppend(listBox_Dva_Bc_status, DvaBcStatus(paDvaStatus_));

				if (Program.occMode)
				{
					Refresh_Progress_Occ();
				}
				else
				{
					Refresh_Progress_Station();
				}

			}
			catch (ThreadAbortException ex)
			{
				Program.WriteEventLog("[INFO] [ThreadAbortException] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
			}
			catch (TimeOutException ex)
			{
				Program.WriteEventLog("[ERROR] [TimeOutException] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				Program.MessageBox_Error(Program.pasReplyTimeOut, sysErr_);
				this.Close();
			}
			catch (TableException ex)
			{
				Program.WriteEventLog("[ERROR] [TableException] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				Program.MessageBox_Error(Program.MB_FormEx, sysErr_);
				this.Close();
			}
			catch (Exception ex)
			{
				Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				Program.MessageBox_Error(Program.MB_FormEx, sysErr_);
				this.Close();
			}
		}

		/// <summary>
		/// Retry selected failed zone
		/// </summary>
		/// <param name="sender">Control that raised the event</param>
		/// <param name="e">Additional information about the event that was raised</param>
		/// <exception cref="NoOracleDBException">Handle No Oracle DB Exception</exception>
		/// <exception cref="Exception">Handle generic exception</exception>
		private void but_Dva_Retry_Sel_Click(object sender, EventArgs e)
		{
			// Nothing to validate : sender, e
			DataGridViewSelectedRowCollection failZoneRowCollection = null;

			try
			{
				bool abortSuccess = false;
				bool[] localFailedZone = null;

				newExecute_ = false;

				if (!Program.occMode)
				{
					// Station
					localFailedZone = new bool[Program.MAXZONEID];
				}
				else
				{
					// OCC
					localFailedZone = new bool[Program.MAXZONEID * Program.MAXSTNID];
				}

				failZoneRowCollection = dataGridView_Dva_Failure.SelectedRows;

				// Set operator selected failed zone to true
				for (int i = 0; i < failZoneRowCollection.Count; i++)
				{
					DataGridViewCellCollection dgvCell = failZoneRowCollection[i].Cells;

					localFailedZone[Int32.Parse(dgvCell[3].Value.ToString())] = true;

					Program.WriteEventLog("[INFO] Selected Failed zone index to replay: " + dgvCell[3].Value.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				}

				failedSelZones_ = (bool[])localFailedZone.Clone();

				if (!abortDva_)
				{
					// Abort previous broadcast
					abortSuccess = UserFunctions.DVA_Abort_MsgSeq(dvaBcSettings_.msgSeqNo);
				}
				else
				{
					// Previous broadcast not aborted by operator
					abortSuccess = true;
				}

				// Continue if the clearing of the message slot is successful
				if (abortSuccess)
				{
					// Submit DVA Broadcast
					if (backgroundWorker_Dva_SubmitRequest.IsBusy == false)
					{
						backgroundWorker_Dva_SubmitRequest.RunWorkerAsync();
					}
				}
				else
				{
					Program.MessageBox_Error(mbRetryFail_, sysErr_);
				}
			}
			catch (Exception ex)
			{
				Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				Program.MessageBox_Error(Program.MB_GeneralEx, sysErr_);
			}
		}

		/// <summary>
		/// Retry DVA Broadcasts in all areas
		/// </summary>
		/// <param name="sender">Control that raised the event</param>
		/// <param name="e">Additional information about the event that was raised</param>
		/// <exception cref="NoOracleDBException">Handle No Oracle DB Exception</exception>
		/// <exception cref="Exception">Handle generic exception</exception>
		private void but_Dva_Retry_All_Click(object sender, EventArgs e)
		{
			// Nothing to validate : sender, e
			try
			{
				bool abortSuccess = false;

				newExecute_ = false;

				if (!abortDva_)
				{
					// Abort previous broadcast
					abortSuccess = UserFunctions.DVA_Abort_MsgSeq(dvaBcSettings_.msgSeqNo);
				}
				else
				{
					// Previous broadcast not aborted by operator
					abortSuccess = true;
				}

				// Continue if the clearing of the message slot is successful
				if (abortSuccess)
				{
					// Submit DVA Broadcast
					if (!backgroundWorker_Dva_SubmitRequest.IsBusy)
					{
						backgroundWorker_Dva_SubmitRequest.RunWorkerAsync();
					}
				}
				else
				{
					Program.MessageBox_Error(mbRetryFail_, sysErr_);
				}
			}
			catch (Exception ex)
			{
				Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				Program.MessageBox_Error(Program.MB_GeneralEx, sysErr_);
			}
		}

		/// <summary>
		/// Abort existing DVA broadcast
		/// </summary>
		/// <param name="sender">Control that raised the event</param>
		/// <param name="e">Additional information about the event that was raised</param>
		/// <exception cref="NoOracleDBException">Handle No Oracle DB Exception</exception>
		/// <exception cref="Exception">Handle generic exception</exception>
		private void button_abort_Click(object sender, EventArgs e)
		{
			// Nothing to validate : sender, e

			try
			{
				bool dvaAborted = false;

				if (dvaInitCompleted_)
				{
					dvaAborted = UserFunctions.DVA_Abort_MsgSeq(dvaBcSettings_.msgSeqNo);
				}
				else
				{
					// Nothing to abort - just update progress status
					dvaAborted = true;
				}

				if (dvaAborted)
				{
					// "DVA Broadcast Aborted"

					UpdateProgress_Thread_Stop();

					if (backgroundWorker_Dva_SubmitRequest.WorkerSupportsCancellation == true)
					{
						backgroundWorker_Dva_SubmitRequest.CancelAsync();
					}

					ListBoxAppend(listBox_Dva_Bc_status, statusBcAborted_);

					paDvaStatus_ = EnumType.DvaBcStatus.BcFinishGotFail;

					but_abort.Enabled = false;
					but_Dva_Retry_All.Enabled = true;
					but_Dva_Retry_Sel.Enabled = true;

					abortDva_ = true;

					if (Program.occMode)
					{
						DVA_Stage_Abort_OCC();
					}
					else
					{
						DVA_Stage_Abort();
					}
				}
				else
				{
					// "DVA Broadcast Abort FAIL!" - Ignore
					Program.MessageBox_Error(mbBcAbortFail_, sysErr_);
				}
			}
			catch (Exception ex)
			{
				Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				Program.MessageBox_Error(Program.MB_GeneralEx, sysErr_);
			}
		}

		/// <summary>
		/// Operator clicks the Close button 
		/// </summary>
		/// <param name="sender">Control that raised the event</param>
		/// <param name="e">Additional information about the event that was raised</param>
		private void button_close_Click(object sender, EventArgs e)
		{
			// Nothing to validate : sender, e
			this.Close();
		}

		#region Common HMI Handling codes
		/// <summary>
		/// Display data in the relevant ListBox
		/// </summary>
		/// <param name="listBoxObj">Reference to the listbox</param>
		/// <param name="strData">Value to be displayed within listbox</param>
		/// <exception cref="Exception">Handle generic exception</exception>
		private void displayConfigListBox(ref ListBox listBoxObj, string strData)
		{
			// Nothing to validate : listBoxObj, strData
			try
			{
				listBoxObj.Items.Clear();
				listBoxObj.Items.Add(strData);
			}
			catch (Exception ex)
			{
				Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
			}
		}

		private string DvaBcStatus(EnumType.DvaBcStatus statusCode)
		{
			return DvaBcStatus(statusCode, Program.defaultLanguage);
		}

		/// <summary>
		/// Decode the meaning of the DVA status - 0-based index
		/// </summary>
		/// <param name="statusCode">Status code of the DVA status</param>
		/// <returns>DVA status message</returns>
		private string DvaBcStatus(EnumType.DvaBcStatus statusCode, string languageCode)
		{
			// Nothing to validate :  statusCode
			string bcStatus = "";

			switch ((int)statusCode)
			{
				case 0:
					bcStatus = Localization.GetEventMessage("EVT_DVA_BCSTATUS_INIT", languageCode);
					break;
				case 1:
					bcStatus = Localization.GetEventMessage("EVT_DVA_BCSTATUS_IN_PROGRESS", languageCode);
					break;
				case 2:
					break;
				case 3:
					bcStatus = Localization.GetEventMessage("EVT_DVA_BCSTATUS_DWELL_TIME", languageCode);
					break;
				case 4:
					bcStatus = Localization.GetEventMessage("EVT_DVA_BCSTATUS_NO_AVAIL_CHANNEL", languageCode);
					break;
				case 5:
					bcStatus = Localization.GetEventMessage("EVT_DVA_BCSTATUS_MSG_IN_USE", languageCode);
					break;
				case 6:
					bcStatus = Localization.GetEventMessage("EVT_DVA_BCSTATUS_CHANNEL_OVERRIDDEN", languageCode);
					break;
				case 7:
					break;
				case 8:
					bcStatus = Localization.GetEventMessage("EVT_DVA_BCSTATUS_FINISH_ADNORMALLY", languageCode);
					break;
				case 9:
					bcStatus = Localization.GetEventMessage("EVT_DVA_BCSTATUS_FINISH_NO_FAILURE", languageCode);
					break;
				case 10:
					bcStatus = Localization.GetEventMessage("EVT_DVA_BCSTATUS_FINISH_FAILURE", languageCode);
					break;
				case 11:
					bcStatus = Localization.GetEventMessage("EVT_DVA_BC_REQ_FAILED", languageCode);
					break;
				case 12:
					bcStatus = Localization.GetEventMessage("EVT_BC_FAILED_NO_WORKING_ZONES", languageCode);
					break;
				default:
					bcStatus = "Unknown Status (Error code 1)";
					break;
			}
			return bcStatus;
		}

		/// <summary>
		/// Decode the meaning of the DVA zone status
		/// </summary>
		/// <param name="statusCode">Status code of the DVA zone status</param>
		/// <returns>DVA zone status message</returns>
		private string DvaBcZoneStatus(EnumType.EZoneBcState statusCode)
		{
			// Nothing to validate : statusCode
			string statusMsg = "";

			switch ((int)statusCode)
			{
				case 1:
					statusMsg = zoneInProgress_;
					break;
				case 2:
					statusMsg = zoneFailed_;
					break;
				case 3:
					statusMsg = zoneOverridden_;
					break;
				case 4:
					statusMsg = zoneAborted_;
					break;
				case 5:
					statusMsg = zoneComplete_;
					break;
				case 6:
					statusMsg = zoneInit_;
					break;
				case 7:
					statusMsg = zonePending_;
					break;
				default:
					statusMsg = "Unknown Status (Error code 2)";
					break;
			}

			return statusMsg;
		}

		/// <summary>
		/// Decode the meaning of the DVA chim status
		/// </summary>
		/// <param name="statusCode">Status code of the DVA chime status</param>
		/// <returns>DVA chim status message</returns>
		private string DvaChimeStatus(int statusCode)
		{
			// Nothing to validate : statusCode
			string statusMsg = "";

			switch (statusCode)
			{
				case 1:
					statusMsg = lbChimeOff_;
					break;
				case 2:
					statusMsg = lbChimeOn_;
					break;

				default:
					statusMsg += "--";
					break;
			}

			return statusMsg;
		}
		#endregion

		#region Station Codes
		/// <summary>
		/// Station - Refresh the broadcast progress display
		/// </summary>
		/// <exception cref="TableException">Throws Table Exception if received table is empty</exception>
		/// <exception cref="SocketNotConnException">Handle Socket Not Connected exception</exception>
		/// <exception cref="Exception">Handle generic exception</exception>
		private void Refresh_Progress_Station()
		{
			bool zoneReachInProgress_ = false;

			// Zone broadcast status - Workaround if all selected bc zones are not working
			if (DVA_Stage_A())
			{
				Program.WriteEventLog("[ERROR] All selected bc zones are detected to be failure!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

				paDvaStatus_ = EnumType.DvaBcStatus.NoWorkingZones;
				ListBoxAppend(listBox_Dva_Bc_status, DvaBcStatus(paDvaStatus_));

				DVA_Stage_Error();

				UpdateProgress_Thread_Stop();
				return;
			}

			while (true)
			{
				if (dvaInitCompleted_)
				{
					// Program will hang here, if all selected zones are failed zone, timeout cannot be used
					CheckFlag202();

					// Check Flag 302 after checking Table 202 and 204, possible bug here as PAS Announce ID is only checked in Stage B
					if (UserFunctions.checkFlagStatus(EnumType.FlagIndex.Flag302))
					{
						Program.WriteEventLog("[INFO] Flag 302 set!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

						// Whenever the broadcast starts, check the source announceId
						// 302 R - Check PA broadcast progress status
						paDvaStatus_ = UserFunctions.DVA_Check_Broadcast_Status(dvaBcSettings_.msgSeqNo);
						UserFunctions.clearSelectedFlag(EnumType.FlagIndex.Flag302);
						Program.WriteEventLog("[INFO] Flag 302 cleared!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

						Program.WriteEventLog("[INFO] New DVA Status: " + DvaBcStatus(paDvaStatus_, "en-US"), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

						// Display status only if there is a change
						if (prevPaDvaStatus_ != paDvaStatus_)
						{
							if (paDvaStatus_ == EnumType.DvaBcStatus.BcFinishNoFail && noFailZones() == false)
							{
								// Overwrite 
								paDvaStatus_ = EnumType.DvaBcStatus.BcFinishGotFail;
							}

							ListBoxAppend(listBox_Dva_Bc_status, DvaBcStatus(paDvaStatus_));

							prevPaDvaStatus_ = paDvaStatus_;
						}

						if (paDvaStatus_ == EnumType.DvaBcStatus.DvaChnOverride || paDvaStatus_ == EnumType.DvaBcStatus.DvaMsgInUseInRecord ||
							paDvaStatus_ == EnumType.DvaBcStatus.NoAvailChn)
						{
							// Problem in the starting of the DVA broadcast
							DVA_Stage_Error();
							break;
						}

						if (paDvaStatus_ == EnumType.DvaBcStatus.Initiated)
						{
							DVA_Stage_A();
						}
						else if (paDvaStatus_ == EnumType.DvaBcStatus.InProgress)
						{
							// PAS Announce ID checked in Stage B
							DVA_Stage_B(pasScAnnounceId_);
							zoneReachInProgress_ = true;
						}
						else if (paDvaStatus_ == EnumType.DvaBcStatus.BcFinishAdnormal ||
								 paDvaStatus_ == EnumType.DvaBcStatus.BcFinishNoFail ||
								 paDvaStatus_ == EnumType.DvaBcStatus.BcFinishGotFail)
						{
							if (zoneReachInProgress_ || paDvaStatus_ == EnumType.DvaBcStatus.BcFinishNoFail)
							{
								// Bc duration too short, that it "skips" in progress status
								DVA_Stage_C();
							}
							else
							{
								DVA_Stage_Error();
							}
							break;
						}
						else
						{
							// For those dwell time, etc.
							Program.WriteDebugEventLog("[DEBUG] paDvaStatus: " + paDvaStatus_, fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
						}
					}
					else if (paDvaStatus_ == EnumType.DvaBcStatus.InProgress)
					{
						// Keep updating during broadcast progress 
						DVA_Stage_B(pasScAnnounceId_);
					}
					else
					{
						// AnnounceId is 0 or flag 202 or 204 not set
						// Display Blank DGV
						//displayDvaBcInfo();
					}
				}

				Thread.Sleep(Program.dvaBcPoll);
			}

			UpdateProgress_Thread_Stop();
		}

		/// <summary>
		/// Stage A - Broadcast not yet started.
		/// </summary>
		/// <returns>bool: true if all selected bc zones are failure, otherwise false</returns>
		private bool DVA_Stage_A()
		{
			bool allSelAreFailure = true;

			Program.WriteEventLog("[INFO] Stage A - Broadcast not yet started, display pending zones", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
			EnumType.EZoneBcState[] zoneBcState = UserFunctions.Check_ZoneBcStatus(Convert.ToByte(dvaBcSettings_.dvaSourceCode), selBcZone_);

			// Loop thru all zones to check if broadcast is successful
			for (int i = 0; i < Program.MAXZONEID; i++)
			{
				if (selBcZone_[i])
				{
					// Check when amp are down. 
					if (zoneBcState[i] == EnumType.EZoneBcState.Failed)
					{
						playingZone_[i] = false;
						failZone_[i] = true;
						failZoneStatus_[i] = EnumType.EZoneBcState.Failed;
					}
					else
					{
						playingZone_[i] = true;
						failZone_[i] = false;
						failZoneStatus_[i] = EnumType.EZoneBcState.Normal;

						allSelAreFailure = false;
					}
				}
			}

			displayDvaBcInfo();

			return allSelAreFailure;
		}

		/// <summary>
		/// Stage B - Broadast in progress
		/// </summary>
		/// <param name="pasScAnnounceId">Annouce ID in the Source as read from PAS</param>
		private void DVA_Stage_B(int pasScAnnounceId)
		{
			// Nothing to validate : pasScAnnounceId

			Program.WriteEventLog("[INFO] Stage B - Broadcast in progress.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
			bool[] playingZoneBk = null;
			bool[] failZoneBk = null;
			EnumType.EZoneBcState[] failZoneStatusBk = null;

			// Make a copy of all zones data in case there is a change in Table 302 during the midst of update
			playingZoneBk = (bool[])playingZone_.Clone();
			failZoneBk = (bool[])failZone_.Clone();
			failZoneStatusBk = (EnumType.EZoneBcState[])failZoneStatus_.Clone();

			if (pasScAnnounceId == dvaBcSettings_.announceId)
			{
				EnumType.EZoneBcState[] zoneBcState = UserFunctions.Check_ZoneBcStatus(Convert.ToByte(dvaBcSettings_.dvaSourceCode), selBcZone_);

				// Loop thru all zones to check if broadcast is successful
				for (int i = 0; i < Program.MAXZONEID; i++)
				{
					if (selBcZone_[i] && !failZone_[i])
					{
						{
							//Program.WriteEventLog("[INFO] [Stage B] - Initial update ignored. Continue update.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

							// Due to the hardware limitation of PAS, the following 3 stages are taken to be normal as long as the msg slot is in progress
							if (zoneBcState[i] == EnumType.EZoneBcState.Normal ||
								zoneBcState[i] == EnumType.EZoneBcState.Aborted ||
								zoneBcState[i] == EnumType.EZoneBcState.Overridden)
							{
								playingZone_[i] = true;
								failZone_[i] = false;
								failZoneStatus_[i] = EnumType.EZoneBcState.Normal;
							}
							else
							{
								playingZone_[i] = false;
								failZone_[i] = true;
								failZoneStatus_[i] = zoneBcState[i];
							}
						}
					}
				}
			}
			else
			{
				// AnnounceId not set
				if (pasScAnnounceId == 0)
				{
					Program.WriteEventLog("[ERROR] Announcement Id in Source not Set! SHOULD NOT HAPPEN. Assume bc haven start.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				}
				else
				{
					Program.WriteEventLog("[ERROR] Announcement Id in Source does NOT match!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

					// Source was overwritten
					for (int i = 0; i < Program.MAXZONEID; i++)
					{
						if (selBcZone_[i] && !failZone_[i])
						{
							playingZone_[i] = false;
							failZone_[i] = true;
							failZoneStatus_[i] = EnumType.EZoneBcState.Overridden;
						}
					}
				}
			}

			displayDvaBcInfo();
		}

		/// <summary>
		/// Stage C - Broadcast Ended
		/// </summary>
		private void DVA_Stage_C()
		{
			//byte[] failedZoneByte_ = new byte[stnByteArraySize_];

			Program.WriteEventLog("[INFO] Stage C - Broadcast Ended.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

			// Stores completed zones
			for (int i = 0; i < Program.MAXZONEID; i++)
			{
				if (selBcZone_[i] && !failZone_[i])
				{
					completedZone_[i] = true;
				}
			}

			displayDvaBcInfo();

			// Broadcast finished, re-enable the retry buttons and stop timer
			dvaInitCompleted_ = false;

			// Disable abort button
			but_abort.Enabled = false;

			// Enable retry buttons when broadcast is completed, and there is fail zones
			if (!noFailZones())
			{
				but_Dva_Retry_Sel.Enabled = true;
				but_Dva_Retry_All.Enabled = true;
			}
			else
			{
				but_Dva_Retry_Sel.Enabled = false;
				but_Dva_Retry_All.Enabled = false;
			}

			// Store failed zones 
			failedSelZones_ = failZone_;
		}

		/// <summary>
		/// Stage Error - DvaChnOverride or DvaMsgInUseInRecord or NoAvailChn encountered or
		/// Broadcast status never reach in progress at all, so set all zones as failed
		/// </summary>
		private void DVA_Stage_Error()
		{
			byte[] failedZoneByte_ = new byte[stnByteArraySize_];

			Program.WriteEventLog("[INFO] Stage Error - Error in starting!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

			// Set all to fail
			for (int i = 0; i < Program.MAXZONEID; i++)
			{
				if (selBcZone_[i])
				{
					playingZone_[i] = false;

					failZone_[i] = true;
					failZoneStatus_[i] = EnumType.EZoneBcState.Failed;
				}
			}

			displayDvaBcInfo();

			// Broadcast finished, re-enable the retry buttons and stop timer
			dvaInitCompleted_ = false;

			// Disable abort button
			but_abort.Enabled = false;

			// Enable retry buttons when broadcast is completed, and there is fail zones
			if (!noFailZones())
			{
				but_Dva_Retry_Sel.Enabled = true;
				but_Dva_Retry_All.Enabled = true;
			}
			else
			{
				but_Dva_Retry_Sel.Enabled = false;
				but_Dva_Retry_All.Enabled = false;
			}

			// Store failed zones 
			failedSelZones_ = failZone_;
		}

		/// <summary>
		/// Stage Abort - Broadcast is aborted - Display aborted zones
		/// </summary>
		/// <exception cref="Exception">Handle generic exception</exception>
		private void DVA_Stage_Abort()
		{
			try
			{
				// Broadcast aborted

				// Loop thru all zones and just set them to Aborted
				for (int i = 0; i < Program.MAXZONEID; i++)
				{
					if (selBcZone_[i] && !failZone_[i])
					{
						playingZone_[i] = false;
						failZone_[i] = true;
						failZoneStatus_[i] = EnumType.EZoneBcState.Aborted;
					}
				}

				displayDvaBcInfo();

				// Broadcast finished, re-enable the retry buttons and stop timer
				dvaInitCompleted_ = false;

				// Disable abort button
				but_abort.Enabled = false;

				// Enable retry buttons when broadcast is completed, and there is fail zones
				if (!noFailZones())
				{
					but_Dva_Retry_Sel.Enabled = true;
					but_Dva_Retry_All.Enabled = true;
				}
				else
				{
					but_Dva_Retry_Sel.Enabled = false;
					but_Dva_Retry_All.Enabled = false;
				}

				// Store failed zones 
				failedSelZones_ = failZone_;
			}
			catch (Exception ex)
			{
				Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
			}
		}
		#endregion

		#region OCC Codes
		/// <summary>
		/// OCC - Get the real time zone status data from PAS
		/// </summary>
		/// <exception cref="TableException">Throws Table Exception if received table is empty</exception>
		/// <exception cref="SocketNotConnException">Handle Socket Not Connected exception</exception>
		/// <exception cref="Exception">Handle generic exception</exception>
		private void Refresh_Progress_Occ()
		{
			bool zoneReachInProgress_ = false;

			// Zone broadcast status - Workaround if all selected bc zones are not working
			if (DVA_Stage_A_OCC())
			{
				Program.WriteEventLog("[ERROR] All selected bc zones are detected to be failure!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

				paDvaStatus_ = EnumType.DvaBcStatus.NoWorkingZones;
				ListBoxAppend(listBox_Dva_Bc_status, DvaBcStatus(paDvaStatus_));

				DVA_Stage_Error_OCC();

				UpdateProgress_Thread_Stop();
				return;
			}

			while (true)
			{
				if (dvaInitCompleted_)
				{
					// Read changes in announce ID
					CheckFlag202();

					if (UserFunctions.checkFlagStatus(EnumType.FlagIndex.Flag302))
					{
						Program.WriteEventLog("[INFO] Flag 302 set!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

						// Whenever the broadcast starts, check the source announceId
						// 302 R - Check PA broadcast progress status
						paDvaStatus_ = UserFunctions.DVA_Check_Broadcast_Status(dvaBcSettings_.msgSeqNo);
						UserFunctions.clearSelectedFlag(EnumType.FlagIndex.Flag302);
						Program.WriteEventLog("[INFO] Flag 302 cleared!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

						Program.WriteEventLog("[INFO] New DVA Status: " + DvaBcStatus(paDvaStatus_, "en-US"), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

						// Display status only if there is a change
						if (prevPaDvaStatus_ != paDvaStatus_)
						{
							if (paDvaStatus_ == EnumType.DvaBcStatus.BcFinishNoFail && noFailZones() == false)
							{
								// Overwrite 
								paDvaStatus_ = EnumType.DvaBcStatus.BcFinishGotFail;
							}

							ListBoxAppend(listBox_Dva_Bc_status, DvaBcStatus(paDvaStatus_));
							prevPaDvaStatus_ = paDvaStatus_;
						}

						if (paDvaStatus_ == EnumType.DvaBcStatus.DvaChnOverride || paDvaStatus_ == EnumType.DvaBcStatus.DvaMsgInUseInRecord ||
							paDvaStatus_ == EnumType.DvaBcStatus.NoAvailChn)
						{
							// Problem in the starting of the DVA broadcast
							DVA_Stage_Error_OCC();
							break;
						}

						if (paDvaStatus_ == EnumType.DvaBcStatus.Initiated)
						{
							DVA_Stage_A_OCC();
						}
						else if (paDvaStatus_ == EnumType.DvaBcStatus.InProgress)
						{
							DVA_Stage_B_OCC(pasScAnnounceId_);
							zoneReachInProgress_ = true;
						}
						else if (paDvaStatus_ == EnumType.DvaBcStatus.BcFinishAdnormal ||
									paDvaStatus_ == EnumType.DvaBcStatus.BcFinishNoFail ||
									paDvaStatus_ == EnumType.DvaBcStatus.BcFinishGotFail)
						{
							if (zoneReachInProgress_ || paDvaStatus_ == EnumType.DvaBcStatus.BcFinishNoFail)
							{
								// Bc duration too short, that it "skips" in progress status
								DVA_Stage_C_OCC();
							}
							else
							{
								DVA_Stage_Error_OCC();
							}

							break;
						}
						else
						{
							// For those dwell time, etc. Do nth.
							Program.WriteDebugEventLog("[DEBUG] paDvaStatus: " + paDvaStatus_, fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
						}
					}
					else if (paDvaStatus_ == EnumType.DvaBcStatus.InProgress)
					{
						// Keep updating during broadcast progress 
						DVA_Stage_B_OCC(pasScAnnounceId_);
					}
					else
					{
						// announceId is 0 or flag 202 or 1204 not set
						// Display Blank DGV
						//displayDvaBcInfoOcc();
					}
				}

				Thread.Sleep(Program.dvaBcPoll);
			}

			UpdateProgress_Thread_Stop();
		}

		/// <summary>
		/// 202 R - Read latest announcement ID
		/// </summary>
		/// <exception cref="TimeOutException">Handle timeout exception</exception>
		/// <exception cref="Exception">Handle generic exception</exception>
		private void CheckFlag202()
		{
			// Update the source announceId
			if (UserFunctions.checkFlagStatus(EnumType.FlagIndex.Flag202))
			{
				try
				{
					Program.WriteEventLog("[INFO] Flag 202 set!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

					// Read Latest Announce Id currently in use by source
					pasScAnnounceId_ = UserFunctions.Get_Source_AnnouncementID(dvaBcSettings_.dvaSourceCode);
					UserFunctions.clearSelectedFlag(EnumType.FlagIndex.Flag202);

					Program.WriteEventLog("[INFO] NEW PAS Announce ID: " + pasScAnnounceId_, fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				}
				catch (TimeOutException ex)
				{
					Program.WriteEventLog("[ERROR] [TimeOutException] Cannot read new PAS Announce ID! Details: " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				}
				catch (Exception ex)
				{
					Program.WriteEventLog("[ERROR] [Exception] Cannot read new PAS Announce ID! Details: " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				}
			}
		}

		/// <summary>
		/// Stage A - Broadcast not yet started.
		/// </summary>
		/// <returns>bool: true if all selected broadcast zones are failure</returns>
		private bool DVA_Stage_A_OCC()
		{
			bool allSelAreFailure = true;

			Program.WriteEventLog("[INFO] Stage A - Broadcast not yet started, display pending zones", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
			EnumType.EZoneBcState[] zoneBcState = UserFunctions.Check_ZoneBcStatus(dvaBcSettings_.dvaSourceCode, selBcZone_);

			for (int stnIndex = 0; stnIndex < Program.MAXSTNID; stnIndex++)
			{
				for (int z = 0; z < Program.MAXZONEID; z++)
				{
					int occZoneIndex = (stnIndex * Program.MAXZONEID) + z;

					if (selBcZone_[occZoneIndex])
					{
						// Check when amp are down.
						if (zoneBcState[occZoneIndex] == EnumType.EZoneBcState.Failed)
						{
							playingZone_[occZoneIndex] = false;
							failZone_[occZoneIndex] = true;
							failZoneStatus_[occZoneIndex] = EnumType.EZoneBcState.Failed;
						}
						else
						{
							playingZone_[occZoneIndex] = true;
							failZone_[occZoneIndex] = false;
							failZoneStatus_[occZoneIndex] = EnumType.EZoneBcState.Normal;

							allSelAreFailure = false;
						}
					}
				}
			}

			displayDvaBcInfoOcc();

			return allSelAreFailure;
		}

		/// <summary>
		/// Stage B - Broadast in progress
		/// </summary>
		/// <param name="pasScAnnounceId">Annouce ID in the Source as read from PAS</param>
		private void DVA_Stage_B_OCC(int pasScAnnounceId)
		{
			// Nothing to validate : pasScAnnounceId
			Program.WriteEventLog("[INFO] Stage B - Broadcast in progress.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

			if (pasScAnnounceId == dvaBcSettings_.announceId)
			{
				EnumType.EZoneBcState[] zoneBcState = UserFunctions.Check_ZoneBcStatus(dvaBcSettings_.dvaSourceCode, selBcZone_);

				// Loop thru all zones to check if broadcast is successful
				for (int stnIndex = 0; stnIndex < Program.MAXSTNID; stnIndex++)
				{
					for (int z = 0; z < Program.MAXZONEID; z++)
					{
						int occZoneIndex = (stnIndex * Program.MAXZONEID) + z;

						if (selBcZone_[occZoneIndex] && !failZone_[occZoneIndex])
						{
							// Due to the hardware limitation of PAS, the following 3 stages are taken to be normal as long as the msg slot is in progress
							if (zoneBcState[occZoneIndex] == EnumType.EZoneBcState.Normal ||
								zoneBcState[occZoneIndex] == EnumType.EZoneBcState.Aborted ||
								zoneBcState[occZoneIndex] == EnumType.EZoneBcState.Overridden)
							{
								playingZone_[occZoneIndex] = true;
								failZone_[occZoneIndex] = false;
								failZoneStatus_[occZoneIndex] = EnumType.EZoneBcState.Normal;
							}
							else
							{
								playingZone_[occZoneIndex] = false;
								failZone_[occZoneIndex] = true;
								failZoneStatus_[occZoneIndex] = zoneBcState[occZoneIndex];
							}
						}

					}
				}
			}
			else
			{
				// AnnounceId not set
				if (pasScAnnounceId == 0)
				{
					Program.WriteEventLog("[ERROR] Announcement Id in Source not Set! SHOULD NOT HAPPEN. Try to read again... next time.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				}
				else
				{
					Program.WriteEventLog("[ERROR] Announcement Id in Source does NOT match!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

					// Source was overwritten
					for (int stnIndex = 0; stnIndex < Program.MAXSTNID; stnIndex++)
					{
						for (int z = 0; z < Program.MAXZONEID; z++)
						{
							int occZoneIndex = (stnIndex * Program.MAXZONEID) + z;

							if (selBcZone_[occZoneIndex] && !failZone_[occZoneIndex])
							{
								playingZone_[occZoneIndex] = false;
								failZone_[occZoneIndex] = true;
								failZoneStatus_[occZoneIndex] = EnumType.EZoneBcState.Overridden;
							}
						}
					}

				}
			}

			displayDvaBcInfoOcc();
		}

		/// <summary>
		/// Stage C - Broadcast Ended
		/// </summary>
		private void DVA_Stage_C_OCC()
		{
			byte[] failedZoneByteOcc_ = new byte[occByteArraySize_];

			Program.WriteEventLog("[INFO] Stage C - Broadcast Ended.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

			// Stores completed zones
			for (int stnIndex = 0; stnIndex < Program.MAXSTNID; stnIndex++)
			{
				for (int z = 0; z < Program.MAXZONEID; z++)
				{
					int occZoneIndex = (stnIndex * Program.MAXZONEID) + z;

					if (selBcZone_[occZoneIndex] && !failZone_[occZoneIndex])
					{
						completedZone_[occZoneIndex] = true;
					}
				}
			}

			displayDvaBcInfoOcc();

			// Broadcast finished, re-enable the retry buttons and stop timer
			dvaInitCompleted_ = false;

			// Disable abort button
			but_abort.Enabled = false;

			// Enable retry buttons when broadcast is completed, and there is fail zones
			if (!noFailZones())
			{
				but_Dva_Retry_Sel.Enabled = true;
				but_Dva_Retry_All.Enabled = true;
			}
			else
			{
				but_Dva_Retry_Sel.Enabled = false;
				but_Dva_Retry_All.Enabled = false;
			}

			// Store failed zones 	
			failedSelZones_ = failZone_;

		}

		/// <summary>
		/// Stage Error - DvaChnOverride or DvaMsgInUseInRecord or NoAvailChn encountered
		/// </summary>
		private void DVA_Stage_Error_OCC()
		{
			byte[] failedZoneByteOcc_ = new byte[occByteArraySize_];

			Program.WriteEventLog("[INFO] Stage Error - Error in starting!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

			// Set all to fail
			for (int stnIndex = 0; stnIndex < Program.MAXSTNID; stnIndex++)
			{
				for (int z = 0; z < Program.MAXZONEID; z++)
				{
					int occZoneIndex = (stnIndex * Program.MAXZONEID) + z;

					if (selBcZone_[occZoneIndex])
					{
						playingZone_[occZoneIndex] = false;

						failZone_[occZoneIndex] = true;
						failZoneStatus_[occZoneIndex] = EnumType.EZoneBcState.Failed;
					}
				}
			}

			displayDvaBcInfoOcc();

			// Broadcast finished, re-enable the retry buttons and stop timer
			dvaInitCompleted_ = false;

			// Disable abort button
			but_abort.Enabled = false;

			// Enable retry buttons when broadcast is completed, and there is fail zones
			if (!noFailZones())
			{
				but_Dva_Retry_Sel.Enabled = true;
				but_Dva_Retry_All.Enabled = true;
			}
			else
			{
				but_Dva_Retry_Sel.Enabled = false;
				but_Dva_Retry_All.Enabled = false;
			}

			// Store failed zones 	
			failedSelZones_ = failZone_;

		}

		/// <summary>
		/// Stage Abort - Broadcast is aborted - Display aborted zones
		/// </summary>
		/// <exception cref="Exception">Handle generic exception</exception>
		private void DVA_Stage_Abort_OCC()
		{
			try
			{
				// Broadcast aborted
				// Loop thru all zones and just set them to Aborted
				for (int stnIndex = 0; stnIndex < Program.MAXSTNID; stnIndex++)
				{
					for (int z = 0; z < Program.MAXZONEID; z++)
					{
						int occZoneIndex = (stnIndex * Program.MAXZONEID) + z;

						if (selBcZone_[occZoneIndex] && !failZone_[occZoneIndex])
						{
							// May be overwritten, etc.
							playingZone_[occZoneIndex] = false;
							failZone_[occZoneIndex] = true;
							failZoneStatus_[occZoneIndex] = EnumType.EZoneBcState.Aborted;
						}
					}
				}

				displayDvaBcInfoOcc();

				// Broadcast finished, re-enable the retry buttons and stop timer
				dvaInitCompleted_ = false;

				// Disable abort button
				but_abort.Enabled = false;

				// Enable retry buttons when broadcast is completed, and there is fail zones
				if (!noFailZones())
				{
					but_Dva_Retry_Sel.Enabled = true;
					but_Dva_Retry_All.Enabled = true;
				}
				else
				{
					but_Dva_Retry_Sel.Enabled = false;
					but_Dva_Retry_All.Enabled = false;
				}

				// Store failed zones 	
				failedSelZones_ = failZone_;
			}
			catch (Exception ex)
			{
				Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
			}
		}
		#endregion

		/// <summary>
		/// Function to display DVA broadcast progress
		/// </summary>
		/// <exception cref="InvalidOperationException">Handle Invalid Operation Exception</exception>
		/// <exception cref="Exception">Handle generic exception</exception>
		private void displayDvaBcInfo()
		{
			lock (lockList_)
			{
				successList_.Clear();
				failedList_.Clear();

				for (int i = 0; i < Program.MAXZONEID; i++)
				{
					if (completedZone_[i])
					{
						// "Completed" in right datagrid
						DgvDisplayStruct dgvRow = new DgvDisplayStruct();
						dgvRow.stnName = DataObject.zoneArray[i].stnName;
						dgvRow.zoneName = DataObject.zoneArray[i].zoneName;
						dgvRow.zoneStatus = DvaBcZoneStatus(EnumType.EZoneBcState.Completed);
						dgvRow.zoneIndex = i;
						successList_.Add(dgvRow);
					}
					else if (playingZone_[i])
					{
						DgvDisplayStruct dgvRow = new DgvDisplayStruct();
						dgvRow.stnName = DataObject.zoneArray[i].stnName;
						dgvRow.zoneName = DataObject.zoneArray[i].zoneName;
						dgvRow.zoneIndex = i;

						//paDvaStatus_ = UserFunctions.DVA_Check_Broadcast_Status(dvaBcSettings_.msgSeqNo);
						if (paDvaStatus_ == EnumType.DvaBcStatus.Initiated)
						//if (startTimeDT > currTimeDt)
						{
							// "Pending Broadcast" in right datagrid
							dgvRow.zoneStatus = DvaBcZoneStatus(EnumType.EZoneBcState.PendingBroadcast);
						}
						else if (paDvaStatus_ == EnumType.DvaBcStatus.BcFinishAdnormal ||
									paDvaStatus_ == EnumType.DvaBcStatus.BcFinishNoFail)
						{
							// "Completed" in right datagrid
							dgvRow.zoneStatus = DvaBcZoneStatus(EnumType.EZoneBcState.Completed);
						}
						else
						{
							// "In progress" in right datagrid
							dgvRow.zoneStatus = DvaBcZoneStatus(EnumType.EZoneBcState.Normal);
						}

						successList_.Add(dgvRow);
					}
					else
					{
						// Do nothing
					}
					//else //comment on 25 nov - see what happen

					if (failZone_[i])
					{
						// Failed Zones in left datagrid
						DgvDisplayStruct dgvRow = new DgvDisplayStruct();
						dgvRow.stnName = DataObject.zoneArray[i].stnName;
						dgvRow.zoneName = DataObject.zoneArray[i].zoneName;
						dgvRow.zoneStatus = DvaBcZoneStatus(failZoneStatus_[i]);
						dgvRow.zoneIndex = i;
						failedList_.Add(dgvRow);
					}
				}

				successList_.Sort();
				failedList_.Sort();
			}

			try
			{
				DgvDisplayStruct[] successListArray = null;
				DgvDisplayStruct[] failedListArray = null;

				lock (lockList_)
				{
					successListArray = (DgvDisplayStruct[])successList_.ToArray().Clone();
					failedListArray = (DgvDisplayStruct[])failedList_.ToArray().Clone();
				}

				dgvDataSourceCallback(dataGridView_Dva_Progress, successListArray);
				dgvDataSourceCallback(dataGridView_Dva_Failure, failedListArray);

			}
			catch (InvalidOperationException ex)
			{
				// Happens when the form is being closed
				UpdateProgress_Thread_Stop();
				Program.WriteEventLog("[ERROR] [InvalidOperationException] Aborting thread due to Invalid Operation! Details: " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
			}
			catch (Exception ex)
			{
				Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
			}

		}

		/// <summary>
		/// Timer to check OCC mode DVA
		/// </summary>
		/// <exception cref="InvalidOperationException">Handle Invalid Operation Exception</exception>
		/// <exception cref="Exception">Handle generic exception</exception>
		private void displayDvaBcInfoOcc()
		{
			DgvDisplayStruct[] successListArray = null;
			DgvDisplayStruct[] failedListArray = null;

			lock (lockList_)
			{
				successList_.Clear();
				failedList_.Clear();

				for (int stnIndex = 0; stnIndex < Program.MAXSTNID; stnIndex++)
				{
					for (int z = 0; z < Program.MAXZONEID; z++)
					{
						int stnZoneIndex = (stnIndex * Program.MAXZONEID) + z;

						if (completedZone_[stnZoneIndex])
						{
							// "Completed" in right datagrid						
							DgvDisplayStruct dgvRow = new DgvDisplayStruct();
							dgvRow.stnName = DataObject.stnArray[stnZoneIndex].stnName;
							dgvRow.zoneName = DataObject.stnArray[stnZoneIndex].zoneName;
							dgvRow.zoneStatus = DvaBcZoneStatus(EnumType.EZoneBcState.Completed);
							dgvRow.zoneIndex = stnZoneIndex;
							successList_.Add(dgvRow);
						}
						else if (playingZone_[stnZoneIndex])
						{
							DgvDisplayStruct dgvRow = new DgvDisplayStruct();
							dgvRow.stnName = DataObject.stnArray[stnZoneIndex].stnName;
							dgvRow.zoneName = DataObject.stnArray[stnZoneIndex].zoneName;
							dgvRow.zoneIndex = stnZoneIndex;

							if (paDvaStatus_ == EnumType.DvaBcStatus.Initiated)
							{
								// "Pending Broadcast" in right datagrid
								dgvRow.zoneStatus = DvaBcZoneStatus(EnumType.EZoneBcState.PendingBroadcast);
							}
							else if (paDvaStatus_ == EnumType.DvaBcStatus.BcFinishAdnormal ||
									paDvaStatus_ == EnumType.DvaBcStatus.BcFinishNoFail)
							{
								// "Completed" in right datagrid
								dgvRow.zoneStatus = DvaBcZoneStatus(EnumType.EZoneBcState.Completed);
							}
							else
							{
								// In progress
								dgvRow.zoneStatus = DvaBcZoneStatus(EnumType.EZoneBcState.Normal);
							}

							successList_.Add(dgvRow);
						}
						else
						{
							// Do nothing
						}

						if (failZone_[stnZoneIndex])
						{
							// Failed Zones in left datagrid
							DgvDisplayStruct dgvRow = new DgvDisplayStruct();
							dgvRow.stnName = DataObject.stnArray[stnZoneIndex].stnName;
							dgvRow.zoneName = DataObject.stnArray[stnZoneIndex].zoneName;
							dgvRow.zoneStatus = DvaBcZoneStatus(failZoneStatus_[stnZoneIndex]);
							dgvRow.zoneIndex = stnZoneIndex;
							failedList_.Add(dgvRow);
						}
					}
				}

				successList_.Sort();
				failedList_.Sort();

				successListArray = (DgvDisplayStruct[])successList_.ToArray().Clone();
				failedListArray = (DgvDisplayStruct[])failedList_.ToArray().Clone();
			}

			try
			{
				dgvDataSourceCallback(dataGridView_Dva_Progress, successListArray);
				dgvDataSourceCallback(dataGridView_Dva_Failure, failedListArray);
			}
			catch (InvalidOperationException ex)
			{
				UpdateProgress_Thread_Stop();
				Program.WriteEventLog("[ERROR] [InvalidOperationException] Aborting thread due to Invalid Operation! Details: " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
			}
			catch (Exception ex)
			{
				Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
			}
		}

		/// <summary>
		/// Check if it is true that there is no failed zones.
		/// </summary>
		/// <returns>bool: true if there is no failed zones, otherwise false</returns>
		private bool noFailZones()
		{
			lock (lockList_)
			{
				if (failedList_.Count != 0)
				{
					Program.WriteDebugEventLog("[DEBUG] Failed zones detected!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
					return false;
				}
				else
				{
					Program.WriteDebugEventLog("[DEBUG] NO failed zones detected!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
					return true;
				}
			}
		}

		/// <summary>
		/// Clear everything from the listbox
		/// </summary>
		/// <param name="oControl">Name of the control</param>
		/// <exception cref="Exception">Handle generic exception</exception>
		private void ListBoxClear(Control oControl)
		{
			// Nothing to validate : oControl
			try
			{
				if (oControl.InvokeRequired)
				{
					ListBoxClearCallback d = new ListBoxClearCallback(ListBoxClear);
					oControl.Invoke(d, new object[] { oControl });
				}
				else
				{
					ListBox listBoxObj = oControl as ListBox;
					listBoxObj.Items.Clear();

				}
			}
			catch (Exception ex)
			{
				Program.WriteEventLog("[ERROR] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
			}
		}

		/// <summary>
		/// Append to Listbox
		/// </summary>
		/// <param name="oControl">Object refence to control</param>
		/// <param name="msgIn">Message to be appended</param>
		/// <exception cref="Exception">Handle generic exception</exception>
		private void ListBoxAppend(Control oControl, string msgIn)
		{
			try
			{
				if (oControl.InvokeRequired)
				{
					ListBoxAddCallback d = new ListBoxAddCallback(ListBoxAppend);
					oControl.Invoke(d, new object[] { oControl, msgIn });
				}
				else
				{
					ListBox listBoxObj = oControl as ListBox;
					listBoxObj.Items.Add(msgIn);

					while (listBoxObj.Items.Count > 100)
					{
						listBoxObj.Items.RemoveAt(0);
					}

					listBoxObj.SelectedIndex = listBoxObj.Items.Count - 1;

				}
			}
			catch (Exception ex)
			{
				Program.WriteEventLog("[ERROR] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
			}
		}

		/// <summary>
		/// Method called when a change in the datagridview datasource is required
		/// </summary>
		/// <param name="oControl">Reference to the control object</param>
		/// <param name="dgvDisplayStruct">Reference to the new datasource</param>
		/// <exception cref="Exception">Handle generic exception</exception>
		private void dgvDataSourceCallback(Control oControl, DgvDisplayStruct[] dgvDisplayStruct)
		{
			try
			{
				if (oControl.InvokeRequired)
				{
					DgvDataSourceCallback d = new DgvDataSourceCallback(dgvDataSourceCallback);
					oControl.Invoke(d, new object[] { oControl, dgvDisplayStruct });
				}
				else
				{
					DataGridView dgvObj = oControl as DataGridView;

					dgvObj.DataSource = dgvDisplayStruct;

					// Loc, Zone, Status, Index
					dgvObj.Columns[0].HeaderText = dgvLoc_;
					dgvObj.Columns[1].HeaderText = dgvZone_;
					dgvObj.Columns[2].HeaderText = dgvStatus_;
					dgvObj.Columns[3].HeaderText = dgvIndex_;

					// AutoSize columns
					dgvObj.Columns[0].AutoSizeMode = DataGridViewAutoSizeColumnMode.AllCells;
					dgvObj.Columns[1].AutoSizeMode = DataGridViewAutoSizeColumnMode.AllCells;
					dgvObj.Columns[2].AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill;

					if (Program.showIndex)
					{
						dgvObj.Columns[3].AutoSizeMode = DataGridViewAutoSizeColumnMode.AllCells;
					}
					else
					{
						dgvObj.Columns[3].AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
						dgvObj.Columns[3].Visible = false;
					}
				}
			}
			catch (Exception ex)
			{
				Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
			}
		}
	}
}