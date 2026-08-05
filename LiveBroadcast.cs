#region Header Block
/// <classname>LiveBroadcast</classname> 
/// <description>Form to show the status of a LIVE Broadcast</description> 
/// <filename>LiveBroadcast.cs</filename> 
/// <authors>Tay Yu Hui</authors> 
/// <version>00.03.00.00</version> 
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
////using ste.scada.common;				// Provide commonly used classes for the creation of MFT
using ste.common.exceptions;
using ste.common.util;
using ste.common.Forms;

namespace ste.pa.pamanager
{
	/// <summary>
	/// Form to show the status of a LIVE Broadcast
	/// </summary>
	public partial class LiveBroadcast : Form
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

		/// <summary>
		/// The time the live broadcast request is first submitted
		/// </summary>
		private DateTime liveRequestSubmitTime_ = DateTime.Now;

		/// <summary>
		/// Announce ID used in this current live broadcast
		/// </summary>
		private int announceId_ = 0;

		/// <summary>
		/// Maximum number of failed attempts for Live bc request
		/// </summary>
		private int maxFailAttempt_ = 6;

		private Thread threadUpdateProgress_ = null;
		private ManualResetEvent liveBcReqEvent_ = new ManualResetEvent(false);
		private string fileName_ = MethodBase.GetCurrentMethod().DeclaringType.Name;

		private delegate void ListBoxClearCallback(Control oControl);
		private delegate void ListBoxAddCallback(Control oControl, string msgIn);
		private delegate void DgvDataSourceCallback(Control oControl, DgvDisplayStruct[] dgvDisplayStruct);

		private bool pttPressed_ = false;
		private bool liveInitCompleted_ = false;
		private bool initUpdateIgnored_ = false;

		// Common
		private EnumType.LiveBcStatus liveBcStage_ = EnumType.LiveBcStatus.Initiated;
		private EnumType.LiveBcStatus preLiveBcStage_ = EnumType.LiveBcStatus.Unknown;

		private bool replayOnFailedZone_ = false;

		private static object lockList_ = new object();

		private List<DgvDisplayStruct> successList_ = new List<DgvDisplayStruct>();
		private List<DgvDisplayStruct> failedList_ = new List<DgvDisplayStruct>();

		// Store a list of failed zones
		private bool[] failSelZone_ = null;

		// Store requested broadcast zones that are sent to PAS
		private bool[] reqBcZones_ = new bool[Program.MAXZONEID];

		#region Station
		/// <summary>
		///  Store failed selected zones
		/// </summary>
		private bool[] selectedZonesFail_ = new bool[Program.MAXZONEID];

		/// <summary>
		/// Store selected broadcast zones
		/// </summary>
		private bool[] selBcZone_ = new bool[Program.MAXZONEID];

		/// <summary>
		/// Store previous completed zones
		/// </summary>
		private bool[] completedZone_ = new bool[Program.MAXZONEID];

		/// <summary>
		/// Store previous failed zones
		/// </summary>
		private bool[] failZone_ = new bool[Program.MAXZONEID];

		/// <summary>
		/// Store previous failed zones status
		/// </summary>
		private EnumType.LiveBcZoneStatus[] failZoneStatus_ = new EnumType.LiveBcZoneStatus[Program.MAXZONEID];

		/// <summary>
		/// Store the previous zone status
		/// </summary>
		private EnumType.EZoneBcState[] preZoneState_ = new EnumType.EZoneBcState[Program.MAXZONEID];
		#endregion

		#region OCC
		private static int maxOccZone_ = Program.MAXSTNID * Program.MAXZONEID;

		/// <summary>
		/// Store failed selected station zones
		/// </summary>
		private bool[] selectedZonesFailOcc_ = new bool[maxOccZone_];

		/// <summary>
		/// Store selected broadcast zones
		/// </summary>
		private bool[] selBcZoneOcc_ = new bool[maxOccZone_];

		/// <summary>
		/// Store previous completed zones
		/// </summary>
		private bool[] completedZoneOcc_ = new bool[maxOccZone_];

		/// <summary>
		/// Store previous failed zones
		/// </summary>
		private bool[] failZoneOcc_ = new bool[maxOccZone_];

		/// <summary>
		/// Store previous failed zones status
		/// </summary>
		private EnumType.LiveBcZoneStatus[] failZoneStatusOcc_ = new EnumType.LiveBcZoneStatus[maxOccZone_];
		#endregion

		// Messages
		private string sysErr_ = "Live Broadcast";
		private string mbBcInit_ = "Live broadcast initiated. Please press PTT";
		private string mbBcInProgress_ = "Live broadcast in progress";
		private string mbBcEnded_ = "Live broadcast ended (No failure)";
		private string mbBcEndedFailedZone_ = "Live broadcast ended (Got failed zones.)";
		private string mbBcAbort_ = "Live broadcast aborted";
		private string mbBcNoWorkingZones_ = "No working broadcast zones available!";
		private string mbBcTimeout_ = "Timeout in waiting for operator to press PTT button.";
		private string zoneInProgress_ = "In progress";
		private string zoneFailed_ = "Failed zone";
		private string zoneOverridden_ = "Overridden";
		private string zoneAborted_ = "Aborted";
		private string zoneComplete_ = "Completed";
		private string zoneInit_ = "Initiated";
		private string dgvLoc_ = "Location";
		private string dgvZone_ = "Zone";
		private string dgvStatus_ = "Status";
		private string dgvIndex_ = "Index";
		private string lbReleasePtt_ = "Please release the PTT button!";
		private string mbErrReleasePtt_ = "Live Broadcast initiation failed! Please release the PTT button.";
		private string mbCfmAbort_ = "Aborting broadcast in Failed zones?";

		/// <summary>
		/// Static constructor 
		/// </summary>
		static LiveBroadcast()
		{
		}

		/// <summary>
		/// This is a constructor to initialize the LiveBroadcast class and its variables and components.
		/// </summary>
		public LiveBroadcast()
		{
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
				this.Text = Localization.localize("LIVE_FORM_NAME", this.Text);
				sysErr_ = Localization.localize("LIVE_FORM_NAME", sysErr_);

				grpBox_Live_Zone_Progress.Text = Localization.localize("GP_BC_PROGRESS", grpBox_Live_Zone_Progress.Text);
				grpBox_Live_Zone_Failure.Text = Localization.localize("GP_BC_FAILURE", grpBox_Live_Zone_Failure.Text);
				groupBox_liveBCstatus.Text = Localization.localize("GP_LIVE_BC_STATUS", groupBox_liveBCstatus.Text);
				but_Live_Retry_Sel.Text = Localization.localize("BUT_RETRY", but_Live_Retry_Sel.Text);
				but_Live_Retry_All.Text = Localization.localize("BUT_RETRYALL", but_Live_Retry_All.Text);
				but_close.Text = Localization.localize("BUT_CLOSE", but_close.Text);

				zoneInProgress_ = Localization.localize("ZONESTATUS_INPROGRESS", zoneInProgress_);
				zoneFailed_ = Localization.localize("ZONESTATUS_FAILEDZONE", zoneFailed_);
				zoneOverridden_ = Localization.localize("ZONESTATUS_OVERRIDDEN", zoneOverridden_);
				zoneAborted_ = Localization.localize("ZONESTATUS_ABORTED", zoneAborted_);
				zoneComplete_ = Localization.localize("ZONESTATUS_COMPLETED", zoneComplete_);
				zoneInit_ = Localization.localize("ZONESTATUS_INITIATED", zoneInit_);

				dgvLoc_ = Localization.localize("DGV_LOC", dgvLoc_);
				dgvZone_ = Localization.localize("DGV_ZONE", dgvZone_);
				dgvStatus_ = Localization.localize("DGV_STATUS", dgvStatus_);
				dgvIndex_ = Localization.localize("DGV_INDEX", dgvIndex_);

				mbBcInit_ = Localization.localize("EVT_LIVE_PRESS_PTT", mbBcInit_);
				mbBcInProgress_ = Localization.localize("EVT_LIVE_IN_PROGRESS", mbBcInProgress_);
				mbBcEnded_ = Localization.localize("EVT_LIVE_ENDED", mbBcEnded_);
				mbBcEndedFailedZone_ = Localization.localize("EVT_LIVE_ENDED_FAILZONE", mbBcEndedFailedZone_);
				mbBcAbort_ = Localization.localize("EVT_LIVE_ABORTED", mbBcAbort_);
				mbBcNoWorkingZones_ = Localization.localize("EVT_STATUS_NO_WORKING_ZONES", mbBcNoWorkingZones_);
				mbBcTimeout_ = Localization.localize("EVT_LIVE_BC_TIMEOUT", mbBcTimeout_);

				lbReleasePtt_ = Localization.localize("ERROR_RELEASE_PTT", lbReleasePtt_);
				mbErrReleasePtt_ = Localization.localize("ERROR_INIT_RELEASE_PTT", mbErrReleasePtt_);
				mbCfmAbort_ = Localization.localize("DVA_MB_CFM_ABORT", mbCfmAbort_);
			}
		}

		/// <summary>
		/// Method invoked when the form loads
		/// </summary>
		/// <param name="sender">Control that raised the event</param>
		/// <param name="e">Additional information about the event that was raised</param>
		/// <exception cref="TimeOutException">Throw TimeoutException when timeout occurs</exception>
		/// <exception cref="TableException">Handle Table exception</exception>
		/// <exception cref="Exception">Handle generic exception</exception>
		private void LiveBroadcast_Load(object sender, EventArgs e)
		{
			// Nothing to validate : sender, e
			try
			{
				localizeForm();

				// 590 R - Check PTT button status on load
				bool pttFlag_ = isPttPressed();

				if (pttFlag_)
				{
					// Prompt operator to release PTT button, before exit application
					Program.MessageBox_Error(mbErrReleasePtt_, sysErr_);
					this.Close();
				}
				else
				{
					// Reset everything at the start of every new broadcast
					completedZoneOcc_ = new bool[maxOccZone_];
					failZoneOcc_ = new bool[maxOccZone_];
					completedZone_ = new bool[Program.MAXZONEID];
					failZone_ = new bool[Program.MAXZONEID];

					// Start Thread to submit request to PAS
					if (backgroundWorker_submitRequest.IsBusy == false)
					{
						backgroundWorker_submitRequest.RunWorkerAsync();
					}
				}
			}
			catch (Exception ex)
			{
				Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				Program.MessageBox_Error(Program.MB_FormEx, sysErr_);
				this.Close();
			}
		}

		/// <summary>
		/// Check if the PTT is being pressed
		/// </summary>
		/// <returns>bool: true if PTT console is pressed, otherwise false</returns>
		private bool isPttPressed()
		{
			bool pptPressed = UserFunctions.PTT_Button_Pressed(Convert.ToInt32(DataObject.Read_PTT_Source_Code()));
			UserFunctions.clearSelectedFlag(EnumType.FlagIndex.Flag590);

			return pttPressed_;
		}

		/// <summary>
		/// Method invoked when the form is closing
		/// </summary>
		/// <param name="sender">Control that raised the event</param>
		/// <param name="e">Additional information about the event that was raised</param>
		private void LiveBroadcast_FormClosing(object sender, FormClosingEventArgs e)
		{
			try
			{
				// Nothing to validate : sender, e
				if (replayOnFailedZone_ || but_Live_Retry_Sel.Enabled == true)
				{
					if (STEMessageBox.Show(mbCfmAbort_, sysErr_, Program.defaultLanguage, MsgBoxButton.YesNo, MessageBoxIcon.Question) == DialogResult.No)
					{
						Program.WriteEventLog("[INFO] Operator refuses to abort Live BC on failed zones!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
						e.Cancel = true;
					}
					else
					{
						Program.WriteEventLog("[INFO] Operator aborts Live BC on failed zones!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

						resumeBcCheck();

						if (liveInitCompleted_)
						{
							Abort_Broadcast();
						}

						cleanUp();
					}
				}
				else
				{
					// Just abort, in case the user nv start pressing the PTT button
					resumeBcCheck();

					if (liveInitCompleted_)
					{
						Abort_Broadcast();
					}

					cleanUp();
				}
			}
			catch (Exception ex)
			{
				Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
			}
		}

		/// <summary>
		/// Retry broadcast to selected unsuccessful zones - Selected zones
		/// </summary>
		/// <param name="sender">Control that raised the event</param>
		/// <param name="e">Additional information about the event that was raised</param>
		/// <exception cref="NoOracleDBException">Handle No Oracle DB Exception</exception>
		/// <exception cref="Exception">Handle generic exception</exception>
		private void but_Live_Retry_Sel_Click(object sender, EventArgs e)
		{
			// Nothing to validate : sender, e
			DataGridViewSelectedRowCollection selFailZones = null;

			try
			{
				selFailZones = dataGridView_Bc_Failure.SelectedRows;

				bool[] localFailedZone = null;

				if (Program.occMode)
				{
					// OCC
					localFailedZone = new bool[Program.MAXSTNID * Program.MAXZONEID];
				}
				else
				{
					// Station
					localFailedZone = new bool[Program.MAXZONEID];
				}

				// Set operator selected failed zone to true
				for (int i = 0; i < selFailZones.Count; i++)
				{
					DataGridViewCellCollection dgvCell = selFailZones[i].Cells;
					localFailedZone[Int16.Parse(dgvCell[3].Value.ToString())] = true;

					Program.WriteEventLog("[INFO] Zone: " + Int16.Parse(dgvCell[3].Value.ToString()) + " selected to replay!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				}

				failSelZone_ = (bool[])localFailedZone.Clone();

				replayOnFailedZone_ = true;

				liveBcStage_ = EnumType.LiveBcStatus.Initiated;

				if (backgroundWorker_submitRequest.IsBusy != true)
				{
					backgroundWorker_submitRequest.RunWorkerAsync();
				}
			}
			catch (Exception ex)
			{
				Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				Program.MessageBox_Error(Program.MB_GeneralEx, sysErr_);
			}
		}

		/// <summary>
		/// Retry broadcast to all unsuccessful zones
		/// </summary>
		/// <param name="sender">Control that raised the event</param>
		/// <param name="e">Additional information about the event that was raised</param>
		/// <exception cref="Exception">Handle generic exception</exception>
		private void but_Live_Retry_All_Click(object sender, EventArgs e)
		{
			// Nothing to validate : sender, e
			replayOnFailedZone_ = true;

			liveBcStage_ = EnumType.LiveBcStatus.Initiated;

			if (backgroundWorker_submitRequest.IsBusy == false)
			{
				backgroundWorker_submitRequest.RunWorkerAsync();
			}
		}

		/// <summary>
		/// Operator clicks the Close button
		/// </summary>
		/// <param name="sender">Control that raised the event</param>
		/// <param name="e">Additional information about the event that was raised</param>
		private void but_closeClick(object sender, EventArgs e)
		{
			// Nothing to validate : sender, e
			this.Close();
		}

		/// <summary>
		/// The cleanUp method is for freeing up resource that was held by the class during runtime.
		/// </summary>
		private void cleanUp()
		{
			// Nothing to validate : sender, e
			Program.WriteEventLog("[CLEANUP] " + MethodInfo.GetCurrentMethod().Name + " executed!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

			// Cancel background operation if it is still running
			if (backgroundWorker_submitRequest.IsBusy)
			{
				backgroundWorker_submitRequest.CancelAsync();
			}

			UpdateProgress_Thread_Stop();
		}

		/// <summary>
		/// Submit LIVE broadcast request to PAS
		/// </summary>
		/// <param name="sender">Control that raised the event</param>
		/// <param name="e">Additional information about the event that was raised</param>
		/// <exception cref="TimeOutException">Throw TimeoutException when timeout occurs</exception>
		/// <exception cref="TableException">Handle Table exception</exception>
		/// <exception cref="Exception">Handle generic exception</exception>
		private void backgroundWorker_submitRequest_DoWork(object sender, DoWorkEventArgs e)
		{
			// Nothing to validate : sender, e
			try
			{
				Program.WriteEventLog("[INFO] Live Broadcast Request Started.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

				liveBcStage_ = EnumType.LiveBcStatus.Initiated;

				// Clear listbox
				listboxClear(listBox_Live_Bc_Status);

				// Disable all retry buttons until broadcast is completed
				but_Live_Retry_All.Enabled = false;
				but_Live_Retry_Sel.Enabled = false;

				// Show Blank DGV
				Refresh_DGV();

				liveInitCompleted_ = false;
				UpdateProgress_Thread_Start();

				if (Program.occMode)
				{
					#region OCC
					// Check if there is any failed zones to replay broadcast
					if (replayOnFailedZone_)
					{
						selBcZoneOcc_ = (bool[])failSelZone_.Clone();
						Program.WriteEventLog("[INFO] Failed Zones Available - Broadcasting on Failed Zones", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
					}
					else
					{
						selBcZoneOcc_ = (bool[])DataObject.Selected_OCC_Zone.Clone();
						Program.WriteEventLog("[INFO] No Failed Zones - Broadcasting on Selected Zones", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
					}

					submitRequest(1201, selBcZoneOcc_);
					reqBcZones_ = (bool[])selBcZoneOcc_.Clone();
					#endregion
				}
				else
				{
					#region Station
					// Check if there is any failed zones to replay broadcast
					if (replayOnFailedZone_)
					{
						selBcZone_ = (bool[])failSelZone_.Clone();
						Program.WriteEventLog("[INFO] Failed Zones Available - Broadcasting on Failed Zones", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
					}
					else
					{
						selBcZone_ = (bool[])DataObject.Selected_Station_Zone.Clone();
						Program.WriteEventLog("[INFO] No Failed Zones - Broadcasting on Selected Zones", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
					}

					submitRequest(201, selBcZone_);
					reqBcZones_ = (bool[])selBcZone_.Clone();
					#endregion
				}
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
		/// Submit Request to PA System
		/// </summary>
		/// <param name="tableNo">Table number to be used</param>
		/// <param name="selectedZones">Selected broadcast zones</param>
		private void submitRequest(int tableNo, bool[] selectedZones)
		{
			// Nothing to validate : tableNo, selectedZones
			byte[] selZoneByte = DataManipulation.BoolToByteArray(selectedZones);
			byte[] bcReqStatus = null;
			string[] eventParam = null;
			string[] eventParamLocal = null;

			// Reset all Live broadcast settings
			liveBcStage_ = EnumType.LiveBcStatus.Initiated;
			int connFailCounter = 0;
			replayOnFailedZone_ = false;

			while (true)
			{
				// 201/1201 W 
				bcReqStatus = PaFunctions.Request_for_Broadcast(tableNo, DataObject.Read_PTT_Source_Code(), selZoneByte, false, out announceId_);

				Program.WriteEventLog("[INFO] [LIVE] New announceID: " + announceId_, fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

				// Check if there is any errors in Live request
				if (Convert.ToInt16(bcReqStatus[0]) != 255)
				{
					if (Convert.ToInt16(bcReqStatus[3]) == 0)
					{
						// No error in write request
						liveInitCompleted_ = true;
						liveBcReqEvent_.Set();

						Program.WriteEventLog("[INFO] PA LIVE broadcast request successful.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

						liveRequestSubmitTime_ = DateTime.Now;

						eventParam = new string[] { "", "" };
						eventParamLocal = new string[] { "", "" };
						Program.logEvent(EnumType.PaEvent.PaLiveBroadcast, eventParam, eventParamLocal);

						break;
					}
					else if (Convert.ToInt16(bcReqStatus[3]) == (int)EnumType.PacketError.PAS_ERR_INVALID_ANNOUNCE)
					{
						// Announce ID in use, auto retry using another announce ID
						Program.WriteEventLog("[ERROR] Retry send request. Error: " + Program.returnPacketError(bcReqStatus[3], "en-US"), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
					}
					else
					{
						// Got error in Broadcast Request, no retry 
						Program.WriteEventLog("[ERROR] Try [" + connFailCounter + "] Error: " + Program.returnPacketError(bcReqStatus[3], Program.defaultLanguage), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

						eventParam = new string[] { "", "", Program.returnPacketError(bcReqStatus[3], "en-US") };
						eventParamLocal = new string[] { "", "", Program.returnPacketError(bcReqStatus[3], "zh-TW") };
						Program.logEvent(EnumType.PaEvent.PaLiveBroadcastFail, eventParam, eventParamLocal);

						listboxAppend(listBox_Live_Bc_Status, Program.returnPacketError(bcReqStatus[3], Program.defaultLanguage));

						break;
					}
				}
				else
				{
					connFailCounter++;

					if (connFailCounter == maxFailAttempt_)
					{
						eventParam = new string[] { "", "", Localization.GetEventMessage("EVT_NO_PAS_LINK", "en-US") };
						eventParamLocal = new string[] { "", "", Localization.GetEventMessage("EVT_NO_PAS_LINK", "zh-TW") };
						Program.logEvent(EnumType.PaEvent.PaLiveBroadcastFail, eventParam, eventParamLocal);

						// Fail to send Live broadcast request to PA System
						Program.MessageBox_Error(Program.LB_PasConnectionFail, sysErr_);
						break;
					}
				}
			}
		}

		/// <summary>
		/// Occurs when the background operation has completed, has been canceled, or has raised an exception.
		/// </summary>
		/// <param name="sender">Control that raised the event</param>
		/// <param name="e">Additional information about the event that was raised</param>
		/// <exception cref="Exception">Handle generic exception</exception>
		private void backgroundWorker_submitRequest_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
		{
			// Nothing to validate : sender, e
			try
			{
				if (e.Cancelled)
				{
					Program.WriteEventLog("[INFO] Operator aborted the DVA Request!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
					UpdateProgress_Thread_Stop();
				}
				else
				{
					if (liveInitCompleted_)
					{
						Program.WriteEventLog("[INFO] Submit request completed! Do not stop Update Thread.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
					}
					else
					{
						Program.WriteEventLog("[INFO] Submit request failed!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
						UpdateProgress_Thread_Stop();
					}
				}
			}
			catch (Exception ex)
			{
				Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
			}
		}

		/// <summary>
		/// Start Update Progress Thread
		/// </summary>
		/// <exception cref="Exception">Handle generic exception</exception>
		private void UpdateProgress_Thread_Start()
		{
			Program.WriteEventLog("[INFO] Start Update progress thread first.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
			initUpdateIgnored_ = false;

			threadUpdateProgress_ = new Thread(new ThreadStart(UpdateProgress));
			threadUpdateProgress_.Name = "LiveProgress";
			threadUpdateProgress_.IsBackground = true;
			threadUpdateProgress_.Start();
		}

		/// <summary>
		/// Stop Update Progress Thread
		/// </summary>
		/// <exception cref="Exception">Handle generic exception</exception>
		private void UpdateProgress_Thread_Stop()
		{
			try
			{
				if (threadUpdateProgress_ != null)
				{
					if ((threadUpdateProgress_.ThreadState & (ThreadState.Stopped | ThreadState.Unstarted)) == 0) //if (threadUpdateProgress_.ThreadState == ThreadState.Running)
					{
						resumeBcCheck();

						Program.WriteEventLog("[INFO] Aborting Update Thread! [" + threadUpdateProgress_.Name + "]", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
						threadUpdateProgress_.Abort();
					}
				}
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
		/// Update the progress of the LIVE broadcast
		/// </summary>
		/// <exception cref="TimeOutException">Throw TimeoutException when timeout occurs</exception>
		/// <exception cref="Exception">Handle generic exception</exception>
		private void UpdateProgress()
		{
			try
			{
				// Proceed only after a request has been successfully submitted
				liveBcReqEvent_.WaitOne(Program.pacTimeoutMs);

				while (true)
				{
					if (liveInitCompleted_)
					{
						pauseBcCheck();

						if (Program.occMode)
						{
							Update_Progress_Occ();
						}
						else
						{
							Update_Progress_Station();
						}
					}

					Thread.Sleep(Program.liveBcPttPollSec);
				}
			}
			catch (ThreadAbortException ex)
			{
				Program.WriteEventLog("[ERROR] [ThreadAbortException] " + ex.Message, fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
			}
			catch (Exception ex)
			{
				Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				this.Close();
			}
		}

		#region Station Codes
		/// <summary>
		/// Station mode - Update Live Broadcast Progress 
		/// </summary>
		/// <exception cref="Exception">Handle generic exception</exception>
		private void Update_Progress_Station()
		{
			// Live broadcast in progress
			if (liveBcStage_ != preLiveBcStage_)
			{
				if (liveBcStage_ == EnumType.LiveBcStatus.BcEnded)
				{
					if (dataGridView_Bc_Failure.Rows.Count != 0)
					{
						liveBcStage_ = EnumType.LiveBcStatus.BcEndedFailZone;
						Program.WriteEventLog("[INFO] Live bc ended, failed zone detected. Override stage to [BcEndedFailZone].", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
					}
				}

				listboxAppend(listBox_Live_Bc_Status, LiveBcStatus(liveBcStage_, Program.defaultLanguage));
				preLiveBcStage_ = liveBcStage_;
			}

			if (liveBcStage_ == EnumType.LiveBcStatus.Initiated)
			{
				Live_StageA();
			}
			else if (liveBcStage_ == EnumType.LiveBcStatus.InProgress)
			{
				Live_StageB();
			}
			else if (liveBcStage_ == EnumType.LiveBcStatus.BcEnded || liveBcStage_ == EnumType.LiveBcStatus.BcEndedFailZone)
			{
				Live_StageC();
				Abort_Broadcast();
			}
			else if (liveBcStage_ == EnumType.LiveBcStatus.BcNoWorkingZone)
			{
				string[] eventParam = new string[] { "", "", LiveBcStatus(liveBcStage_, "en-US") };
				string[] eventParamLocal = new string[] { "", "", LiveBcStatus(liveBcStage_, "zh-TW") };
				Program.logEvent(EnumType.PaEvent.PaLiveBroadcastFail, eventParam, eventParamLocal);

				Live_StageD();
				Abort_Broadcast();
			}
			else if (liveBcStage_ == EnumType.LiveBcStatus.Timeout)
			{
				string[] eventParam = new string[] { "", "", LiveBcStatus(liveBcStage_, "en-US") };
				string[] eventParamLocal = new string[] { "", "", LiveBcStatus(liveBcStage_, "zh-TW") };
				Program.logEvent(EnumType.PaEvent.PaLiveBroadcastFail, eventParam, eventParamLocal);

				Live_StageTimeout();
				Abort_Broadcast();
			}
			else
			{
				// Do nothing
			}

		}

		/// <summary>
		/// Stage A - Live Broadcast initiated.
		/// Just show all selected zones in the datagridview.
		/// No reading of any tables done.
		/// </summary>
		private void Live_StageA()
		{
			int bcZoneCount = 0;

			Program.WriteEventLog("[INFO] [Live] [Stage A] Live Broadcast initiated.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
			EnumType.EZoneBcState[] zoneBcState = UserFunctions.Check_ZoneBcStatus(DataObject.Read_PTT_Source_Code(), selBcZone_);

			lock (lockList_)
			{
				successList_.Clear();
				failedList_.Clear();

				// Loop thru all zones - initialize display
				for (int i = 0; i < Program.MAXZONEID; i++)
				{
					// If failed zones is selected for broadcast, dun put in failed datagrid
					if (selBcZone_[i])
					{
						// Do some checking on the selected zones 
						if (zoneBcState[i] == EnumType.EZoneBcState.Failed)
						{
							failZone_[i] = true;
							failZoneStatus_[i] = EnumType.LiveBcZoneStatus.ZoneFail;
						}
						else
						{
							// "Initiated" in right datagrid
							DgvDisplayStruct dgvRow = new DgvDisplayStruct();
							dgvRow.stnName = DataObject.zoneArray[i].stnName;
							dgvRow.zoneName = DataObject.zoneArray[i].zoneName;
							dgvRow.zoneStatus = LiveBcZoneStatus(EnumType.LiveBcZoneStatus.ZoneInitiated);
							dgvRow.zoneIndex = i;
							successList_.Add(dgvRow);

							bcZoneCount++;

							// Set failZone to un-failed, if operator chooses to rebroadcast on that zone
							failZone_[i] = false;
							failZoneStatus_[i] = (EnumType.LiveBcZoneStatus)((int)EnumType.EZoneBcState.Normal);
						}
					}

					if (failZone_[i])
					{
						// Unselected Failed Zones placed in left datagrid
						DgvDisplayStruct dgvRow = new DgvDisplayStruct();
						dgvRow.stnName = DataObject.zoneArray[i].stnName;
						dgvRow.zoneName = DataObject.zoneArray[i].zoneName;
						dgvRow.zoneStatus = LiveBcZoneStatus(failZoneStatus_[i]);
						dgvRow.zoneIndex = i;

						failedList_.Add(dgvRow);
					}

					// "Completed" in right datagrid
					if (completedZone_[i])
					{
						DgvDisplayStruct dgvRow = new DgvDisplayStruct();
						dgvRow.stnName = DataObject.zoneArray[i].stnName;
						dgvRow.zoneName = DataObject.zoneArray[i].zoneName;
						dgvRow.zoneStatus = LiveBcZoneStatus(EnumType.LiveBcZoneStatus.ZoneCompleted);
						dgvRow.zoneIndex = i;

						successList_.Add(dgvRow);
					}
				}
			}

			Refresh_DGV();

			if (bcZoneCount == 0)
			{
				// No working zone
				liveBcStage_ = EnumType.LiveBcStatus.BcNoWorkingZone;
			}
			else
			{
				// LOOP  till PTT is pressed
				while (true)
				{
					// 590 R - Check PTT status only when there is a change in 590 Flag status
					if (UserFunctions.checkFlagStatus(EnumType.FlagIndex.Flag590))
					{
						pttPressed_ = UserFunctions.PTT_Button_Pressed(DataObject.Read_PTT_Source_Code());

						// Go to Stage 1 only when PPT is pressed else continue looping
						if (pttPressed_)
						{
							Program.WriteEventLog("[INFO] [Live] [Stage A] PTT [" + DataObject.Read_PTT_Source_Code() + "] pressed!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
							liveBcStage_ = EnumType.LiveBcStatus.InProgress;
							UserFunctions.clearSelectedFlag(EnumType.FlagIndex.Flag590);
							break;
						}
						else
						{
							Program.WriteEventLog("[INFO] [Live] [Stage A] PTT [" + DataObject.Read_PTT_Source_Code() + "] NOT pressed!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
							UserFunctions.clearSelectedFlag(EnumType.FlagIndex.Flag590);
						}

					}

					if (liveBcStartTimeout())
					{
						liveBcStage_ = EnumType.LiveBcStatus.Timeout;
						break;
					}

					Thread.Sleep(Program.liveBcPttPollSec);
				}

			}

		}

		/// <summary>
		/// Check if the Live Broadcast request timeout has occured - if yes, may be due to
		/// All selected zones are faulty, or
		/// Operator did not press the PTT button on time
		/// </summary>
		/// <returns>bool: true if the timeout is reached, otherwise false</returns>
		private bool liveBcStartTimeout()
		{
			long elapsedTicks = DateTime.Now.Ticks - liveRequestSubmitTime_.Ticks;
			TimeSpan elapsedSpan = new TimeSpan(elapsedTicks);

			if (elapsedSpan.TotalMilliseconds > Program.liveBcTimeoutMs)
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		/// <summary>
		/// Stage B - Live Broadcast in progress
		/// </summary>
		private void Live_StageB()
		{
			int bcZoneCount = 0;

			// Store zone source code status
			byte[] zoneStatus = new byte[Program.MAXZONEID];

			Program.WriteEventLog("[INFO] [Live] [Stage B] - Live Broadcast in progress.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

			while (true)
			{
				// 590 R - Check PTT status only when there is a change in 590 Flag status
				if (UserFunctions.checkFlagStatus(EnumType.FlagIndex.Flag590))
				{
					pttPressed_ = UserFunctions.PTT_Button_Pressed(DataObject.Read_PTT_Source_Code());
					UserFunctions.clearSelectedFlag(EnumType.FlagIndex.Flag590);

					if (!pttPressed_)
					{
						// PTT was released, stop loop
						Program.WriteEventLog("[INFO] [Live] [Stage B] PTT WAS RELEASED!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
						break;
					}
				}

				if (pttPressed_)
				{
					bcZoneCount = Live_StageB_Normal();

				}

				Thread.Sleep(Program.liveBcPttPollSec);

			} // End While

			// PTT button is released
			if (bcZoneCount == 0)
			{
				Program.WriteEventLog("[INFO] [Live] [Stage B] - PTT Released - No Broadcast Zones - To Stage D", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				liveBcStage_ = EnumType.LiveBcStatus.BcNoWorkingZone;
			}
			else
			{
				Program.WriteEventLog("[INFO] [Live] [Stage B] - PTT Released - Got Broadcast Zones - To Stage C", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				liveBcStage_ = EnumType.LiveBcStatus.BcEnded;
			}

		}

		/// <summary>
		/// Stage B, Live Broadcast in progress, Source code matches
		/// </summary>
		/// <returns>number of zone that is broadcasting</returns>
		private int Live_StageB_Normal()
		{
			int bcZoneCount = 0;

			lock (lockList_)
			{
				successList_.Clear();
				failedList_.Clear();

				EnumType.EZoneBcState[] zoneBcState = UserFunctions.Check_ZoneBcStatus(DataObject.Read_PTT_Source_Code(), selBcZone_);

				// Loop thru all zones to check if broadcast is successful
				for (int i = 0; i < Program.MAXZONEID; i++)
				{
					// Do nothing for previously Completed zones
					if (completedZone_[i])
					{
						//Program.WriteEventLog("[INFO] [Live] [Stage B] completedZone_: " + i, fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

						DgvDisplayStruct dgvRow = new DgvDisplayStruct();
						dgvRow.stnName = DataObject.zoneArray[i].stnName;
						dgvRow.zoneName = DataObject.zoneArray[i].zoneName;
						dgvRow.zoneStatus = LiveBcZoneStatus(EnumType.LiveBcZoneStatus.ZoneCompleted);
						dgvRow.zoneIndex = i;
						successList_.Add(dgvRow);

						failZone_[i] = false;
						failZoneStatus_[i] = (EnumType.LiveBcZoneStatus)((int)zoneBcState[i]);
					}
					else if (selBcZone_[i])
					{
						if (initUpdateIgnored_ == false)
						{
							Program.WriteDebugEventLog("[DEBUG] [Live] [Stage B] Ignore till zone sc is not '0', due to the hardware limitation of PAS.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

							if (zoneBcState[i] == EnumType.EZoneBcState.Failed)
							{
								// Is a failed zone no matter if the initial update is wrong
								failZone_[i] = true;
								failZoneStatus_[i] = (EnumType.LiveBcZoneStatus)((int)zoneBcState[i]);
							}
							else if (zoneBcState[i] == EnumType.EZoneBcState.Overridden || zoneBcState[i] == EnumType.EZoneBcState.Normal)
							{
								// Is not an aborted zone, means the initial wrong status is over				
								initUpdateIgnored_ = true;
								Program.WriteEventLog("[INFO] [Live] [Stage B] Initial wrong update over.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
							}
							else
							{
								// Stop looping since the rest of the zone state is already wrong
								//break;
							}
						}
						else
						{
							// If zone has failed previously, it will still be placed in failed list
							if (zoneBcState[i] == EnumType.EZoneBcState.Normal && failZone_[i] == false)
							{
								// Zone is normal and has not failed before

								// Current BC Zone status will be displayed
								DgvDisplayStruct dgvRow = new DgvDisplayStruct();
								dgvRow.stnName = DataObject.zoneArray[i].stnName;
								dgvRow.zoneName = DataObject.zoneArray[i].zoneName;
								dgvRow.zoneStatus = LiveBcZoneStatus(EnumType.LiveBcZoneStatus.InProgress);
								dgvRow.zoneIndex = i;
								successList_.Add(dgvRow);

								failZone_[i] = false;
								failZoneStatus_[i] = (EnumType.LiveBcZoneStatus)((int)zoneBcState[i]);
								bcZoneCount++;
							}
							else
							{
								// Newly failed zone will be placed in failZone
								// Store the zone number of failed zones, and the failure reason (Failed zone : 2, Overridden : 3, Aborted : 4)
								failZone_[i] = true;
								failZoneStatus_[i] = (EnumType.LiveBcZoneStatus)((int)zoneBcState[i]);
							}
						}
					}
					else
					{
						// Do nothing
					}

					// Display failed zones
					if (failZone_[i])
					{
						// Failed Zones in left datagrid
						DgvDisplayStruct dgvRow = new DgvDisplayStruct();
						dgvRow.stnName = DataObject.zoneArray[i].stnName;
						dgvRow.zoneName = DataObject.zoneArray[i].zoneName;
						dgvRow.zoneStatus = LiveBcZoneStatus(failZoneStatus_[i]);
						dgvRow.zoneIndex = i;
						failedList_.Add(dgvRow);
					}
				}
			}

			Refresh_DGV();
			return bcZoneCount;
		}

		/// <summary>
		/// Stage C - Live Broadcast ended
		/// </summary>
		private void Live_StageC()
		{
			Program.WriteEventLog("[INFO] [Live] [Stage C] - Live Broadcast ended.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

			// To clear all previous zone status
			lock (lockList_)
			{
				successList_.Clear();
				failedList_.Clear();

				for (int i = 0; i < Program.MAXZONEID; i++)
				{
					// if selected zone is not in failZone, assume it completed broadcast sucessfully
					if (selBcZone_[i] && !failZone_[i])
					{
						completedZone_[i] = true;
					}

					// "Completed" in right datagrid
					if (completedZone_[i])
					{
						DgvDisplayStruct dgvRow = new DgvDisplayStruct();
						dgvRow.stnName = DataObject.zoneArray[i].stnName;
						dgvRow.zoneName = DataObject.zoneArray[i].zoneName;
						dgvRow.zoneStatus = LiveBcZoneStatus(EnumType.LiveBcZoneStatus.ZoneCompleted);
						dgvRow.zoneIndex = i;
						successList_.Add(dgvRow);
					}

					if (failZone_[i])
					{
						// Failed Zones in left datagrid					
						DgvDisplayStruct dgvRow = new DgvDisplayStruct();
						dgvRow.stnName = DataObject.zoneArray[i].stnName;
						dgvRow.zoneName = DataObject.zoneArray[i].zoneName;
						dgvRow.zoneStatus = LiveBcZoneStatus(failZoneStatus_[i]);
						dgvRow.zoneIndex = i;
						failedList_.Add(dgvRow);
					}
				}
			}

			Refresh_DGV();

			failSelZone_ = (bool[])failZone_.Clone();

			// Enable retry buttons when broadcast is completed
			if (failedList_.Count > 0)
			{
				but_Live_Retry_Sel.Enabled = true;
				but_Live_Retry_All.Enabled = true;
			}
			else
			{
				but_Live_Retry_All.Enabled = false;
				but_Live_Retry_Sel.Enabled = false;
			}

			liveInitCompleted_ = false;
		}

		/// <summary>
		/// Stage D - No working zones
		/// </summary>
		/// <since>00.03.01.00</since>
		private void Live_StageD()
		{
			Program.WriteEventLog("[INFO] Live Stage D - No working zones.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

			failSelZone_ = (bool[])failZone_.Clone();

			// Enable retry buttons when broadcast is completed
			if (failedList_.Count > 0)
			{
				but_Live_Retry_Sel.Enabled = true;
				but_Live_Retry_All.Enabled = true;
			}
			else
			{
				but_Live_Retry_All.Enabled = false;
				but_Live_Retry_Sel.Enabled = false;
			}

			liveInitCompleted_ = false;
		}

		/// <summary>
		/// Error Stage - Something wrong with the LIVE broadcast
		/// </summary>
		/// <since>00.03.18.02</since>
		private void Live_StageTimeout()
		{
			lock (lockList_)
			{
				Program.WriteEventLog("[INFO] [Live] [Stage Timeout]", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

				successList_.Clear();
				failedList_.Clear();

				// Set all selected zones as overridden
				for (int i = 0; i < Program.MAXZONEID; i++)
				{
					// "Completed" in right datagrid
					if (completedZone_[i])
					{
						DgvDisplayStruct dgvRow = new DgvDisplayStruct();
						dgvRow.stnName = DataObject.zoneArray[i].stnName;
						dgvRow.zoneName = DataObject.zoneArray[i].zoneName;
						dgvRow.zoneStatus = LiveBcZoneStatus(EnumType.LiveBcZoneStatus.ZoneCompleted);
						dgvRow.zoneIndex = i;
						successList_.Add(dgvRow);
					}
					else if (selBcZone_[i])
					{
						failZone_[i] = true;
						failZoneStatus_[i] = (EnumType.LiveBcZoneStatus)((int)EnumType.EZoneBcState.Failed);
					}
					else
					{
						// Do nothing
					}

					if (failZone_[i])
					{
						// Failed Zones in left datagrid					
						DgvDisplayStruct dgvRow = new DgvDisplayStruct();
						dgvRow.stnName = DataObject.zoneArray[i].stnName;
						dgvRow.zoneName = DataObject.zoneArray[i].zoneName;
						dgvRow.zoneStatus = LiveBcZoneStatus(failZoneStatus_[i]);
						dgvRow.zoneIndex = i;
						failedList_.Add(dgvRow);
					}
				}
			}

			failSelZone_ = (bool[])failZone_.Clone();

			// Enable retry buttons when broadcast is completed
			if (failedList_.Count > 0)
			{
				but_Live_Retry_Sel.Enabled = true;
				but_Live_Retry_All.Enabled = true;
			}
			else
			{
				but_Live_Retry_All.Enabled = false;
				but_Live_Retry_Sel.Enabled = false;
			}

			liveInitCompleted_ = false;

			Refresh_DGV();
		}
		#endregion

		#region OCC Codes
		/// <summary>
		/// OCC mode - Update Live Broadcast Progress 
		/// </summary>
		/// <exception cref="Exception">Handle generic exception</exception>
		private void Update_Progress_Occ()
		{
			if (liveBcStage_ != preLiveBcStage_)
			{
				if (liveBcStage_ == EnumType.LiveBcStatus.BcEnded)
				{
					if (dataGridView_Bc_Failure.Rows.Count != 0)
					{
						liveBcStage_ = EnumType.LiveBcStatus.BcEndedFailZone;
						Program.WriteEventLog("[INFO] Live bc ended, failed zone detected. Override stage to [BcEndedFailZone].", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
					}
				}

				listboxAppend(listBox_Live_Bc_Status, LiveBcStatus(liveBcStage_, Program.defaultLanguage));
				preLiveBcStage_ = liveBcStage_;
			}

			if (liveBcStage_ == EnumType.LiveBcStatus.Initiated)
			{
				Live_Occ_StageA();
			}
			else if (liveBcStage_ == EnumType.LiveBcStatus.InProgress)
			{
				Live_Occ_StageB();
			}
			else if (liveBcStage_ == EnumType.LiveBcStatus.BcEnded || liveBcStage_ == EnumType.LiveBcStatus.BcEndedFailZone)
			{
				Live_Occ_StageC();
				Abort_Broadcast();
			}
			else if (liveBcStage_ == EnumType.LiveBcStatus.BcNoWorkingZone)
			{
				string[] eventParam = new string[] { "", "", LiveBcStatus(liveBcStage_, "en-US") };
				string[] eventParamLocal = new string[] { "", "", LiveBcStatus(liveBcStage_, "zh-TW") };
				Program.logEvent(EnumType.PaEvent.PaLiveBroadcastFail, eventParam, eventParamLocal);

				Live_Occ_StageD();
				Abort_Broadcast();
			}
			else if (liveBcStage_ == EnumType.LiveBcStatus.Timeout)
			{
				string[] eventParam = new string[] { "", "", LiveBcStatus(liveBcStage_, "en-US") };
				string[] eventParamLocal = new string[] { "", "", LiveBcStatus(liveBcStage_, "zh-TW") };
				Program.logEvent(EnumType.PaEvent.PaLiveBroadcastFail, eventParam, eventParamLocal);

				Live_Occ_StageTimeout();
				Abort_Broadcast();
			}
			else
			{
				// Do nothing
			}
		}

		/// <summary>
		/// Stage A - Live Broadcast initiated.
		/// Just show all selected zones in the datagridview.
		/// No reading of any tables done.
		/// </summary>
		private void Live_Occ_StageA()
		{
			int bcZoneCount = 0;

			Program.WriteEventLog("[INFO] [Live] [Stage A] Live Broadcast initiated.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
			EnumType.EZoneBcState[] zoneBcState = UserFunctions.Check_ZoneBcStatus(DataObject.Read_PTT_Source_Code(), selBcZoneOcc_);

			lock (lockList_)
			{
				successList_.Clear();
				failedList_.Clear();

				// Loop thru all zones - initialize display
				for (int stnIndex = 0; stnIndex < Program.MAXSTNID; stnIndex++)
				{
					for (int z = 0; z < Program.MAXZONEID; z++)
					{
						int stnZoneIndex = (stnIndex * Program.MAXZONEID) + z;

						// If failed zones is selected for broadcast, dun put in failed datagrid
						if (selBcZoneOcc_[stnZoneIndex])
						{
							// Do some checking on the selected zones 
							if (zoneBcState[stnZoneIndex] == EnumType.EZoneBcState.Failed)
							{
								failZoneOcc_[stnZoneIndex] = true;
								failZoneStatusOcc_[stnZoneIndex] = EnumType.LiveBcZoneStatus.ZoneFail;
							}
							else
							{
								// "Initiated" in right datagrid (Check from the back of the array)
								DgvDisplayStruct dgvRow = new DgvDisplayStruct();
								dgvRow.stnName = DataObject.stnArray[stnZoneIndex].stnName;
								dgvRow.zoneName = DataObject.stnArray[stnZoneIndex].zoneName;
								dgvRow.zoneStatus = LiveBcZoneStatus(EnumType.LiveBcZoneStatus.ZoneInitiated);
								dgvRow.zoneIndex = stnZoneIndex;
								successList_.Add(dgvRow);

								bcZoneCount++;

								// Set failZone to un-failed
								failZoneOcc_[stnZoneIndex] = false;
								failZoneStatusOcc_[stnZoneIndex] = (EnumType.LiveBcZoneStatus)((int)EnumType.EZoneBcState.Normal);
							}

						}

						if (failZoneOcc_[stnZoneIndex])
						{
							// Unselected Failed Zones placed in left datagrid
							DgvDisplayStruct dgvRow = new DgvDisplayStruct();
							dgvRow.stnName = DataObject.stnArray[stnZoneIndex].stnName;
							dgvRow.zoneName = DataObject.stnArray[stnZoneIndex].zoneName;
							dgvRow.zoneStatus = LiveBcZoneStatus(failZoneStatusOcc_[stnZoneIndex]);
							dgvRow.zoneIndex = stnZoneIndex;
							failedList_.Add(dgvRow);
						}

						// "Completed" in right datagrid
						if (completedZoneOcc_[stnZoneIndex])
						{
							DgvDisplayStruct dgvRow = new DgvDisplayStruct();
							dgvRow.stnName = DataObject.stnArray[stnZoneIndex].stnName;
							dgvRow.zoneName = DataObject.stnArray[stnZoneIndex].zoneName;
							dgvRow.zoneStatus = LiveBcZoneStatus(EnumType.LiveBcZoneStatus.ZoneCompleted);
							dgvRow.zoneIndex = stnZoneIndex;
							successList_.Add(dgvRow);
						}
					}
				}

			}

			Refresh_DGV();

			if (bcZoneCount == 0)
			{
				// No working zone
				liveBcStage_ = EnumType.LiveBcStatus.BcNoWorkingZone;
			}
			else
			{
				// LOOP  till PTT is pressed
				while (true)
				{
					// 590 R - Check PTT status only when there is a change in 590 Flag status
					if (UserFunctions.checkFlagStatus(EnumType.FlagIndex.Flag590))
					{
						pttPressed_ = UserFunctions.PTT_Button_Pressed(Convert.ToInt32(DataObject.Read_PTT_Source_Code()));
						UserFunctions.clearSelectedFlag(EnumType.FlagIndex.Flag590);

						// Go to Stage 1 only when PPT is pressed
						if (pttPressed_)
						{
							Program.WriteEventLog("[INFO] [Live] [Stage A] PTT " + DataObject.Read_PTT_Source_Code() + " pressed!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
							liveBcStage_ = EnumType.LiveBcStatus.InProgress;
							break;
						}
						else
						{
							Program.WriteEventLog("[INFO] [Live] [Stage A] PTT " + DataObject.Read_PTT_Source_Code() + " NOT pressed!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
						}
					}

					if (liveBcStartTimeout())
					{
						liveBcStage_ = EnumType.LiveBcStatus.Timeout;
						break;
					}

					Thread.Sleep(Program.liveBcPttPollSec);
				}

			}
		}

		/// <summary>
		/// Stage B - Live Broadcast in progress
		/// </summary>
		/// <since>00.03.02.00</since>
		private void Live_Occ_StageB()
		{
			int bcZoneCount = 0;

			// Store zone source code status
			byte[] zoneStatus = new byte[Program.MAXZONEID];

			Program.WriteEventLog("[INFO] [Live] [Stage B] Live Broadcast in progress.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

			while (true)
			{
				// 590 R - Check PTT status only when there is a change in 590 Flag status
				if (UserFunctions.checkFlagStatus(EnumType.FlagIndex.Flag590))
				{
					pttPressed_ = UserFunctions.PTT_Button_Pressed(Convert.ToInt32(DataObject.Read_PTT_Source_Code()));
					UserFunctions.clearSelectedFlag(EnumType.FlagIndex.Flag590);

					if (!pttPressed_)
					{
						// PTT was released, stop loop
						Program.WriteEventLog("[INFO] [Live] [Stage B] PTT WAS RELEASED!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
						break;
					}
				}

				if (pttPressed_)
				{
					bcZoneCount = Live_Occ_StageB_Normal();

				} // end If pttPressed

				Thread.Sleep(Program.liveBcPttPollSec);
			} // End While

			// PTT button is released
			if (bcZoneCount == 0)
			{
				Program.WriteEventLog("[INFO] [Live] [Stage B] - PTT Released - No Broadcast Zones - To Stage D", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				liveBcStage_ = EnumType.LiveBcStatus.BcNoWorkingZone;
			}
			else
			{
				Program.WriteEventLog("[INFO] [Live] [Stage B] - PTT Released - Got Broadcast Zones - To Stage C", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				liveBcStage_ = EnumType.LiveBcStatus.BcEnded;
			}

		}

		/// <summary>
		/// Stage B - Live Broadcast in progress - Source code matches
		/// </summary>
		/// <returns>number of zones that are broadcasting</returns>
		/// <since>00.03.03.00</since>
		private int Live_Occ_StageB_Normal()
		{
			int bcZoneCount = 0;

			lock (lockList_)
			{
				successList_.Clear();
				failedList_.Clear();

				EnumType.EZoneBcState[] zoneBcState = UserFunctions.Check_ZoneBcStatus(DataObject.Read_PTT_Source_Code(), selBcZoneOcc_);

				// Loop thru all zones to check if broadcast is successful
				for (int stnIndex = 0; stnIndex < Program.MAXSTNID; stnIndex++)
				{
					for (int z = 0; z < Program.MAXZONEID; z++)
					{
						int stnZoneIndex = (stnIndex * Program.MAXZONEID) + z;

						// "Completed" in right datagrid
						if (completedZoneOcc_[stnZoneIndex])
						{
							//Program.WriteEventLog("[INFO] [Live] [Stage B] completedZone_: " + stnZoneIndex, fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

							DgvDisplayStruct dgvRow = new DgvDisplayStruct();
							dgvRow.stnName = DataObject.stnArray[stnZoneIndex].stnName;
							dgvRow.zoneName = DataObject.stnArray[stnZoneIndex].zoneName;
							dgvRow.zoneStatus = LiveBcZoneStatus(EnumType.LiveBcZoneStatus.ZoneCompleted);
							dgvRow.zoneIndex = stnZoneIndex;
							successList_.Add(dgvRow);

							failZoneOcc_[stnZoneIndex] = false;
							failZoneStatusOcc_[stnZoneIndex] = (EnumType.LiveBcZoneStatus)((int)zoneBcState[stnZoneIndex]);
						}
						else if (selBcZoneOcc_[stnZoneIndex])
						{
							if (initUpdateIgnored_ == false)
							{
								Program.WriteEventLog("[INFO] [Live] [Stage B] Ignore till zone sc is not '0', due to the hardware limitation of PAS.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

								if (zoneBcState[stnZoneIndex] == EnumType.EZoneBcState.Failed)
								{
									// Is a failed zone no matter if the initial update is wrong
									failZoneOcc_[stnZoneIndex] = true;
									failZoneStatusOcc_[stnZoneIndex] = (EnumType.LiveBcZoneStatus)((int)zoneBcState[stnZoneIndex]);
								}
								else if (zoneBcState[stnZoneIndex] == EnumType.EZoneBcState.Overridden || zoneBcState[stnZoneIndex] == EnumType.EZoneBcState.Normal)
								{
									// Is not an aborted zone, means the initial wrong status is over				
									initUpdateIgnored_ = true;
								}
								else
								{
									// Stop looping since the rest of the zone state is already wrong
									//goto breakLoop;
								}
							}
							else
							{
								// If zone has failed previously, it will still be placed in failed list
								if (zoneBcState[stnZoneIndex] == EnumType.EZoneBcState.Normal && failZoneOcc_[stnZoneIndex] == false)
								{
									Program.WriteDebugEventLog("[DEBUG] [Live] [Stage B] [" + stnZoneIndex + "] [Not a prev failed zone] zoneBcState: [" + zoneBcState[stnZoneIndex] + "] failZoneOcc_: [" + failZoneOcc_[stnZoneIndex] + "]", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

									// In progress
									DgvDisplayStruct dgvRow = new DgvDisplayStruct();
									dgvRow.stnName = DataObject.stnArray[stnZoneIndex].stnName;
									dgvRow.zoneName = DataObject.stnArray[stnZoneIndex].zoneName;
									dgvRow.zoneStatus = LiveBcZoneStatus(EnumType.LiveBcZoneStatus.InProgress);
									dgvRow.zoneIndex = stnZoneIndex;
									successList_.Add(dgvRow);

									failZoneOcc_[stnZoneIndex] = false;
									failZoneStatusOcc_[stnZoneIndex] = (EnumType.LiveBcZoneStatus)((int)zoneBcState[stnZoneIndex]);
									bcZoneCount++;
								}
								else
								{
									Program.WriteDebugEventLog("[DEBUG] [Live] [Stage B] [" + stnZoneIndex + "] [Detected as failed] zoneBcState: [" + zoneBcState[stnZoneIndex] + "] failZone_: [" + failZoneOcc_[stnZoneIndex] + "]", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

									// Store the zone number of failed zones, and the failure reason (Failed zone : 2, Overridden : 3, Aborted : 4)
									failZoneOcc_[stnZoneIndex] = true;
									failZoneStatusOcc_[stnZoneIndex] = (EnumType.LiveBcZoneStatus)((int)zoneBcState[stnZoneIndex]);
								}
							}
						}
						else
						{
							// Do nothing
						}

						// Display failed zones
						if (failZoneOcc_[stnZoneIndex])
						{
							// Failed Zones in left datagrid
							DgvDisplayStruct dgvRow = new DgvDisplayStruct();
							dgvRow.stnName = DataObject.stnArray[stnZoneIndex].stnName;
							dgvRow.zoneName = DataObject.stnArray[stnZoneIndex].zoneName;
							dgvRow.zoneStatus = LiveBcZoneStatus(failZoneStatusOcc_[stnZoneIndex]);
							dgvRow.zoneIndex = stnZoneIndex;
							failedList_.Add(dgvRow);
						}
					}
				}
			}

			Refresh_DGV();

			return bcZoneCount;
		}

		/// <summary>
		/// Stage C - Live Broadcast ended
		/// </summary>
		/// <since>00.03.02.00</since>
		private void Live_Occ_StageC()
		{
			Program.WriteEventLog("[INFO] [Live] [Stage C] - Live Broadcast ended.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

			// To clear all previous zone status
			lock (lockList_)
			{
				successList_.Clear();
				failedList_.Clear();

				for (int stnIndex = 0; stnIndex < Program.MAXSTNID; stnIndex++)
				{
					for (int z = 0; z < Program.MAXZONEID; z++)
					{
						int stnZoneIndex = (stnIndex * Program.MAXZONEID) + z;

						// if selected zone is not in failZone, assume it completed broadcast sucessfully
						if (selBcZoneOcc_[stnZoneIndex] && !failZoneOcc_[stnZoneIndex])
						{
							completedZoneOcc_[stnZoneIndex] = true;
						}

						// "Completed" in right datagrid
						if (completedZoneOcc_[stnZoneIndex])
						{
							DgvDisplayStruct dgvRow = new DgvDisplayStruct();
							dgvRow.stnName = DataObject.stnArray[stnZoneIndex].stnName;
							dgvRow.zoneName = DataObject.stnArray[stnZoneIndex].zoneName;
							dgvRow.zoneStatus = LiveBcZoneStatus(EnumType.LiveBcZoneStatus.ZoneCompleted);
							dgvRow.zoneIndex = stnZoneIndex;
							successList_.Add(dgvRow);
						}

						if (failZoneOcc_[stnZoneIndex])
						{
							// Failed Zones in left datagrid					
							DgvDisplayStruct dgvRow = new DgvDisplayStruct();
							dgvRow.stnName = DataObject.stnArray[stnZoneIndex].stnName;
							dgvRow.zoneName = DataObject.stnArray[stnZoneIndex].zoneName;
							dgvRow.zoneStatus = LiveBcZoneStatus(failZoneStatusOcc_[stnZoneIndex]);
							dgvRow.zoneIndex = stnZoneIndex;
							failedList_.Add(dgvRow);
						}
					}
				}
			}

			Refresh_DGV();
			failSelZone_ = (bool[])failZoneOcc_.Clone();

			// Enable retry buttons when broadcast is completed
			if (failedList_.Count > 0)
			{
				but_Live_Retry_Sel.Enabled = true;
				but_Live_Retry_All.Enabled = true;
			}
			else
			{
				but_Live_Retry_All.Enabled = false;
				but_Live_Retry_Sel.Enabled = false;
			}

			liveInitCompleted_ = false;
		}

		/// <summary>
		/// Stage D - No working zones
		/// </summary>
		/// <since>00.03.02.00</since>
		private void Live_Occ_StageD()
		{
			Program.WriteEventLog("[INFO] [Live] [Stage D] - No working zones.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

			failSelZone_ = (bool[])failZoneOcc_.Clone();

			// Enable retry buttons when broadcast is completed
			if (failedList_.Count > 0)
			{
				but_Live_Retry_Sel.Enabled = true;
				but_Live_Retry_All.Enabled = true;
			}
			else
			{
				but_Live_Retry_All.Enabled = false;
				but_Live_Retry_Sel.Enabled = false;
			}

			liveInitCompleted_ = false;
		}

		/// <summary>
		/// Error Stage - Something wrong with the LIVE broadcast
		/// </summary>
		/// <since>00.03.18.02</since>
		private void Live_Occ_StageTimeout()
		{
			lock (lockList_)
			{
				Program.WriteEventLog("[INFO] [Live] [Stage Timeout]", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

				successList_.Clear();
				failedList_.Clear();

				// Set all selected zones as overridden
				for (int stnIndex = 0; stnIndex < Program.MAXSTNID; stnIndex++)
				{
					for (int z = 0; z < Program.MAXZONEID; z++)
					{
						int stnZoneIndex = (stnIndex * Program.MAXZONEID) + z;

						// "Completed" in right datagrid
						if (completedZoneOcc_[stnZoneIndex])
						{
							DgvDisplayStruct dgvRow = new DgvDisplayStruct();
							dgvRow.stnName = DataObject.stnArray[stnZoneIndex].stnName;
							dgvRow.zoneName = DataObject.stnArray[stnZoneIndex].zoneName;
							dgvRow.zoneStatus = LiveBcZoneStatus(EnumType.LiveBcZoneStatus.ZoneCompleted);
							dgvRow.zoneIndex = stnZoneIndex;
							successList_.Add(dgvRow);
						}
						else if (selBcZoneOcc_[stnZoneIndex])
						{
							failZoneOcc_[stnZoneIndex] = true;
							failZoneStatusOcc_[stnZoneIndex] = (EnumType.LiveBcZoneStatus)((int)EnumType.EZoneBcState.Failed);
						}
						else
						{
							// do nothing
						}

						if (failZoneOcc_[stnZoneIndex])
						{
							// Failed Zones in left datagrid					
							DgvDisplayStruct dgvRow = new DgvDisplayStruct();
							dgvRow.stnName = DataObject.stnArray[stnZoneIndex].stnName;
							dgvRow.zoneName = DataObject.stnArray[stnZoneIndex].zoneName;
							dgvRow.zoneStatus = LiveBcZoneStatus(failZoneStatusOcc_[stnZoneIndex]);
							dgvRow.zoneIndex = stnZoneIndex;
							failedList_.Add(dgvRow);
						}
					}
				}
			}

			Refresh_DGV();
			failSelZone_ = (bool[])failZoneOcc_.Clone();

			// Enable retry buttons when broadcast is completed
			if (failedList_.Count > 0)
			{
				but_Live_Retry_Sel.Enabled = true;
				but_Live_Retry_All.Enabled = true;
			}
			else
			{
				but_Live_Retry_All.Enabled = false;
				but_Live_Retry_Sel.Enabled = false;
			}

			liveInitCompleted_ = false;
		}
		#endregion

		#region Common methods
		/// <summary>
		/// Refresh the Datagridview to show successful and failed zones
		/// </summary>
		/// <exception cref="InvalidOperationException">Handle invalid operation exception</exception>
		/// <exception cref="Exception">Handle generic exception</exception>
		/// <since>00.03.01.00</since>
		private void Refresh_DGV()
		{
			try
			{
				lock (lockList_)
				{
					successList_.Sort();
					failedList_.Sort();

					dgvDataSourceChange(dataGridView_Bc_Progress, successList_.ToArray());
					dgvDataSourceChange(dataGridView_Bc_Failure, failedList_.ToArray());

				}
			}
			catch (InvalidOperationException ex)
			{
				Program.WriteEventLog("[ERROR] [InvalidOperationException] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

				UpdateProgress_Thread_Stop();
			}
			catch (Exception ex)
			{
				Program.WriteEventLog("[ERROR] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
			}

		}

		/// <summary>
		/// Abort Live Broadcast
		/// </summary>
		/// <since>00.03.02.00</since>
		private void Abort_Broadcast()
		{
			byte endStatus = 0x0;

			replayOnFailedZone_ = false;

			// 201 1201 W - Branch according to Station or OCC mode
			if (!Program.occMode)
			{
				endStatus = PaFunctions.Request_for_Broadcast(201, DataObject.Read_PTT_Source_Code(), DataManipulation.BoolToByteArray(DataObject.Selected_Station_Zone), true, out announceId_)[3];
			}
			else
			{
				endStatus = PaFunctions.Request_for_Broadcast(1201, DataObject.Read_PTT_Source_Code(), DataManipulation.BoolToByteArray(DataObject.Selected_OCC_Zone), true, out announceId_)[3];
			}

			Program.WriteEventLog("[INFO] New announceID: " + announceId_, fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

			if (endStatus != 0)
			{
				Program.WriteEventLog("[ERROR] Error in aborting Live Broadcast before closing form! Wait for PAS timeout to occur.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
			}
			liveInitCompleted_ = false;

			UpdateProgress_Thread_Stop();
		}

		/// <summary>
		/// Decode the meaning of the Live broadcast status
		/// </summary>
		/// <param name="statusCode">Status code of the Live broadcast status</param>
		/// <returns>Live broadcast status message</returns>
		private string LiveBcStatus(EnumType.LiveBcStatus statusCode, string languageCode)
		{
			// Nothing to validate : statusCode
			string statusMsg = "";

			switch ((int)statusCode)
			{
				case 1:
					statusMsg = Localization.GetEventMessage("EVT_LIVE_PRESS_PTT", languageCode);
					break;
				case 2:
					statusMsg = Localization.GetEventMessage("EVT_LIVE_IN_PROGRESS", languageCode);
					break;
				case 3:
					statusMsg = Localization.GetEventMessage("EVT_LIVE_ENDED", languageCode);
					break;
				case 4:
					statusMsg = Localization.GetEventMessage("EVT_LIVE_ABORTED", languageCode);
					break;
				case 5:
					statusMsg = Localization.GetEventMessage("EVT_STATUS_NO_WORKING_ZONES", languageCode);
					break;
				case 6:
					statusMsg = Localization.GetEventMessage("EVT_LIVE_BC_TIMEOUT", languageCode);
					break;
				case 7:
					statusMsg = Localization.GetEventMessage("EVT_LIVE_ENDED_FAILZONE", languageCode);
					break;
				default:
					statusMsg += "Unknown Status (Error code 3)";
					break;
			}
			return statusMsg;
		}

		/// <summary>
		/// Decode the meaning of the Live broadcast zone status
		/// </summary>
		/// <param name="statusCode">Status code of the Live broadcast zone status</param>
		/// <returns>Live broadcast zone status message</returns>
		private string LiveBcZoneStatus(EnumType.LiveBcZoneStatus statusCode)
		{
			// Nothing to validate : statusCode
			string statusMsg = "";

			switch ((int)statusCode)
			{
				case 0:
					statusMsg = "???";
					break;
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
				default:
					statusMsg += "Unknown Status (Error code 4)";
					break;
			}
			return statusMsg;
		}

		#endregion

		/// <summary>
		/// Clear everything from the datagrid view
		/// </summary>
		/// <param name="oControl">Reference to the control</param>
		private void listboxClear(Control oControl)
		{
			// Nothing to validate : oControl
			try
			{
				if (oControl.InvokeRequired)
				{
					ListBoxClearCallback d = new ListBoxClearCallback(listboxClear);
					oControl.Invoke(d, new object[] { oControl });
				}
				else
				{
					ListBox listBoxObj = oControl as ListBox;
					listBoxObj.Items.Clear();

					Program.WriteEventLog("[INFO] ListBox [" + listBoxObj.Name + "] was cleared!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
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
		private void listboxAppend(Control oControl, string msgIn)
		{
			// Nothing to validate : oControl, msgIn
			try
			{
				if (oControl.InvokeRequired)
				{
					ListBoxAddCallback d = new ListBoxAddCallback(listboxAppend);
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

					//Program.WriteEventLog("[INFO] An entry was added to listbox [" + listBoxObj.Name + "] !", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				}
			}
			catch (Exception ex)
			{
				Program.WriteEventLog("[ERROR] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
			}
		}

		/// <summary>
		/// Method called when there is a change in the datagridview's datasource
		/// </summary>
		/// <param name="oControl">Reference to the control</param>
		/// <param name="dgvDisplayStruct">Datasource to be used</param>
		private void dgvDataSourceChange(Control oControl, DgvDisplayStruct[] dgvDisplayStruct)
		{
			try
			{
				if (oControl.InvokeRequired)
				{
					DgvDataSourceCallback d = new DgvDataSourceCallback(dgvDataSourceChange);
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
				Program.WriteEventLog("[ERROR] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
			}
		}

	}
}
