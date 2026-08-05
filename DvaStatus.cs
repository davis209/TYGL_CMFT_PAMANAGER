using System;						// Contain all other namespaces as subordinates
using System.Reflection;			// Retrieve information about assemblies, modules, members, parameters, and other entities in managed code by examining their metadata. 
using System.Windows.Forms;			// Contain classes for creating Window-based applications
using System.Collections.Generic;
using System.Linq;
//using Oracle.DataAccess.Client;		// Provide Oracle Data Provider object which expose inherited, provider-specific, interface implementations of methods and properties
////using ste.scada.common;				// Provide commonly used classes for the creation of MFT
using ste.common.exceptions;
using ste.common.util;

namespace ste.pa.pamanager
{
	/// <summary>
	/// Form to show the status of all message sequence slot currently in PA System
	/// </summary>
	public partial class DvaStatus : Form
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

		//private OracleDataReader oracleDr_ = null;
		private delegate void ListBoxAddCallback(Control oControl, string msgIn);

		private int msgSeqNumtimer = 0;
		private string fileName_ = MethodBase.GetCurrentMethod().DeclaringType.Name;

		private EnumType.DvaBcStatus paDvaStatus_ = EnumType.DvaBcStatus.Initiated;
		private EnumType.DvaBcStatus lastDisplayedStatus_ = EnumType.DvaBcStatus.Initiated;
		private bool clearImmediateSlotFlag_ = false;
		private bool clearCyclicalSlotFlag_ = false;

		private string sysErr_ = "DVA Status";
		private string lbChimeOff_ = "Off";
		private string lbChimeOn_ = "On";

		private string statusInit_ = "Broadcast initiated";
		private string statusInProgress_ = "Broadcast in progress.";
		private string statusDwellTime_ = "Broadcast in between dwell period.";
		private string statusNoAvailChn_ = "Failure: No available DVA channel.";
		private string statusMsgInUse_ = "Failure: Involved DVA message currently in use for recording.";
		private string statusChnOverriden_ = "Overridden: DVA channel requested for other higher priority broadcast.";
		private string statusFinishAdnormal_ = "Broadcast forced to stop";
		private string statusFinishNoFail_ = "Broadcast completed normally. (No failure)";
		private string statusFinishGotFail_ = "Broadcast completed normally. (With failure)";
		private string statusNotControl_ = "Broadcast Status not shown in Monitor mode.";

		private string mbClearDvaSlot_ = "Please clear slot 1 or 2.";
		private string mbClearCyclicalSlot_ = "Please clear slot 3 or 4.";

		private string mbDvaAbort1_ = "DVA Message Sequence ";
		private string mbDvaAbort2_aborted_ = " aborted!";
		private string mbDvaAbort2_abortFail_ = " CANNOT be aborted!!";
		private string mbSelectSlotToAbort_ = "Please select a message slot to abort!";
		private string mbSelectSlotToLoad_ = "Please select a message slot to load!";

		private string msgSlotLabel1_ = "Message Slot ";
		private string msgSlotLabel2_ = " not in use.";

        private DvaMessageStatusStruct dvaMsgStatus_ = null;
        private string messageContent_ = string.Empty;
        private int localLocationId_ = 0;
        private int localConsoleId_ = 54;
        private string msgVersion_ = string.Empty;
        private string localLocationNo_ = string.Empty;

        private Dictionary<BroadcastStatusEnum, string> statusDescMap_ = new Dictionary<BroadcastStatusEnum, string>
        {
            { BroadcastStatusEnum.InProgress, "廣播進行中" },
            { BroadcastStatusEnum.Interrupted, "廣播被中斷" },
            { BroadcastStatusEnum.Completed, "廣播正常完成(無故障)" }
        };
        /// <summary>
        /// This is a constructor to initialize the DvaStatus class and its variables and components.
        /// </summary>
        public DvaStatus(int locationId, int consoleId, string locationNo, string msgVersion)
		{
            localLocationId_ = locationId;
            localConsoleId_ = consoleId;
            localLocationNo_ = locationNo;
            msgVersion_ = msgVersion;
            InitializeComponent();
		}

		/// <summary>
		/// Indicate which message slots to clear, if all slots are full
		/// </summary>
		/// <param name="immediateBc">bool: true if it is an immediate bc to clear</param>
		public void set_Clear_Option(bool immediateBc)
		{
			// Nothing to validate : immediateBc
			if (immediateBc)
			{
				clearImmediateSlotFlag_ = true;
			}
			else
			{
				clearCyclicalSlotFlag_ = true;
			}
		}

		/// <summary>
		/// Method invoked when the DVA Status form loads
		/// </summary>
		/// <param name="sender">Control that raised the event</param>
		/// <param name="e">Additional information about the event that was raised</param>
		private void DvaStatus_Load(object sender, EventArgs e)
		{
			localizeForm();

            PaService.Instance.OnDvaStatusChanged += PaService_OnDvaStatusChanged;
			// Nothing to validate : sender, e
			if (Program.occMode)
			{
				dataGridView_SelectedZones_Occ.Visible = true;
				dataGridView_SelectedZones.Visible = false;
			}
			else
			{
				dataGridView_SelectedZones_Occ.Visible = false;
				dataGridView_SelectedZones.Visible = true;
			}

			//if (Program.controlMode)
			//{
			//	but_abort.Enabled = true;
			//}
			//else
			//{
			//	but_abort.Enabled = false;
			//}

			if (clearImmediateSlotFlag_)
			{
				Program.MessageBox_Normal(mbClearDvaSlot_, sysErr_, this);
			}
			else if (clearCyclicalSlotFlag_)
			{
				Program.MessageBox_Normal(mbClearCyclicalSlot_, sysErr_, this);
			}
			else
			{
				// do nothing
			}

            dvaMsgStatus_ = UserFunctions.QueryLatestDvaMessageStatus(localLocationId_);
            if(dvaMsgStatus_ != null)
            {
                but_abort.Enabled = true;
                messageContent_ = $"{dvaMsgStatus_.MsgId}  {UserFunctions.QueryPreRecordedMessageLabel(dvaMsgStatus_.MsgId, msgVersion_)}";
                textBox_selected_msgcontent.Text = messageContent_;
                textBox_seat.Text = Program.seats_.FirstOrDefault(kvp => kvp.Key == dvaMsgStatus_.SeatId).Value;
                textBox_RecycleTime.Text = dvaMsgStatus_.RecycleTime.ToString();
                textBox_IntervalSec.Text = (dvaMsgStatus_.IntervalSec).ToString();

                string stationsText = "";
                string[] stationIds = dvaMsgStatus_.Stations.Split(',');
                foreach(var id in stationIds)
                {
                    stationsText += $"{Program.stnNoToStnNameDic_[int.Parse(id)]},  ";
                }
                stationsText = stationsText.TrimEnd(' ').TrimEnd(',');
                textBox_Sel_Stations.Text = stationsText;

                string languageText = "";
                if ((dvaMsgStatus_.Language & 1) > 0)
                {
                    languageText += $"{Program.langChinese_},  ";
                }
                if ((dvaMsgStatus_.Language & 2) > 0)
                {
                    languageText += $"{Program.langTw_},  ";
                }
                if ((dvaMsgStatus_.Language & 4) > 0)
                {
                    languageText += $"{Program.langHakka_},  ";
                }
                if ((dvaMsgStatus_.Language & 8) > 0)
                {
                    languageText += $"{Program.langEnglish_},  ";
                }
                languageText = languageText.TrimEnd(' ').TrimEnd(',');
                textBox_language.Text = languageText;

                string zonesText = "";
                if ((dvaMsgStatus_.Zones & 1) > 0)
                {
                    //dataGridView_SelectedZones.Rows.Add(DataObject.zoneArray[0].zoneName);
                    zonesText += $"{DataObject.zoneArray[0].zoneName},  ";
                }
                if ((dvaMsgStatus_.Zones & 2) > 0)
                {
                    //dataGridView_SelectedZones.Rows.Add(DataObject.zoneArray[1].zoneName);
                    zonesText += $"{DataObject.zoneArray[1].zoneName},  ";
                }
                if ((dvaMsgStatus_.Zones & 4) > 0)
                {
                    //dataGridView_SelectedZones.Rows.Add(DataObject.zoneArray[2].zoneName);
                    zonesText += $"{DataObject.zoneArray[2].zoneName},  ";
                }
                if ((dvaMsgStatus_.Zones & 8) > 0)
                {
                    //dataGridView_SelectedZones.Rows.Add(DataObject.zoneArray[3].zoneName);
                    zonesText += $"{DataObject.zoneArray[3].zoneName}";
                }
                zonesText = zonesText.TrimEnd(' ').TrimEnd(',');
                textBox_Sel_Zones.Text = zonesText;

                BroadcastStatusEnum status = (BroadcastStatusEnum)Enum.ToObject(typeof(BroadcastStatusEnum), dvaMsgStatus_.Status);
                ListBoxClear(listBox_Dva_Bc_status);
                ListBoxAppend(listBox_Dva_Bc_status, statusDescMap_[status]);
            }
            else
            {
                but_abort.Enabled = false;
            }
        }

        private void PaService_OnDvaStatusChanged(object sender, EventArgs e)
        {
            dvaMsgStatus_.Status = (int)PaService.Instance.BroadcastStatus;
            ListBoxClear(listBox_Dva_Bc_status);
            ListBoxAppend(listBox_Dva_Bc_status, statusDescMap_[PaService.Instance.BroadcastStatus]);
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
				this.Text = Localization.localize("DVASTATUS_FORM_NAME", this.Text);
				sysErr_ = Localization.localize("DVASTATUS_FORM_NAME", sysErr_);
				mbClearDvaSlot_ = Localization.localize("MB_CLEAR_DVA_SLOT", mbClearDvaSlot_);
				mbClearCyclicalSlot_ = Localization.localize("MB_CLEAR_CYCLICAL_SLOT", mbClearCyclicalSlot_);

				//grpBox_Main.Text = Localization.localize("DVASTATUS_FORM_NAME", grpBox_Main.Text);
				grpBox_Dva_Bc_config.Text = Localization.localize("GP_BC_CONFIG", grpBox_Dva_Bc_config.Text);
				grpBox_Sel_Zones.Text = Localization.localize("GP_SEL_ZONES", grpBox_Sel_Zones.Text);
                grpBox_Sel_Stations.Text = Localization.localize("GP_SEL_STATIONS", grpBox_Sel_Stations.Text);
                grpBox_Bc_status.Text = Localization.localize("GP_BC_STATUS", grpBox_Bc_status.Text);

				//but_load.Text = Localization.localize("BUT_LOAD", but_load.Text);
				but_abort.Text = Localization.localize("BUT_ABORT", but_abort.Text);
				but_close.Text = Localization.localize("BUT_CLOSE", but_close.Text);

				//label_msgSlot.Text = Localization.localize("LB_MSG_SLOT", label_period.Text);
				//label_period.Text = Localization.localize("LB_PERIOD", label_period.Text);
				//label_start.Text = Localization.localize("LB_START", label_start.Text);
				//label_stop.Text = Localization.localize("LB_STOP", label_stop.Text);
				//label_chime.Text = Localization.localize("LB_CHIME", label_chime.Text);
				//label_dwell.Text = Localization.localize("LB_DWELL", label_dwell.Text);
				label_selMsg.Text = Localization.localize("LB_SELMSG", label_selMsg.Text);

				lbChimeOff_ = Localization.localize("DVA_CHIME_OFF", lbChimeOff_);
				lbChimeOn_ = Localization.localize("DVA_CHIME_ON", lbChimeOn_);

				statusInit_ = Localization.localize("EVT_DVA_BCSTATUS_INIT", statusInit_);
				statusInProgress_ = Localization.localize("EVT_DVA_BCSTATUS_IN_PROGRESS", statusInProgress_);
				statusDwellTime_ = Localization.localize("EVT_DVA_BCSTATUS_DWELL_TIME", statusDwellTime_);
				statusNoAvailChn_ = Localization.localize("EVT_DVA_BCSTATUS_NO_AVAIL_CHANNEL", statusNoAvailChn_);
				statusMsgInUse_ = Localization.localize("EVT_DVA_BCSTATUS_MSG_IN_USE", statusMsgInUse_);
				statusChnOverriden_ = Localization.localize("EVT_DVA_BCSTATUS_CHANNEL_OVERRIDDEN", statusChnOverriden_);
                statusFinishAdnormal_ = Localization.localize("EVT_DVA_BCSTATUS_FORCE_STOP", statusFinishAdnormal_);
				statusFinishNoFail_ = Localization.localize("EVT_DVA_BCSTATUS_FINISH_NO_FAILURE", statusFinishNoFail_);
				statusFinishGotFail_ = Localization.localize("EVT_DVA_BCSTATUS_FINISH_FAILURE", statusFinishGotFail_);

				statusNotControl_ = Localization.localize("DVA_BCSTATUS_NOT_CONTROL", statusNotControl_);

				mbDvaAbort1_ = Localization.localize("DVA_ABORT_1", mbDvaAbort1_);
				mbDvaAbort2_aborted_ = Localization.localize("DVA_ABORT_2_ABORTED", mbDvaAbort2_aborted_);
				mbDvaAbort2_abortFail_ = Localization.localize("DVA_ABORT_2_ABORT_FAIL", mbDvaAbort2_abortFail_);

				msgSlotLabel1_ = Localization.localize("DVA_MSG_SLOT_LABEL_1", msgSlotLabel1_);
				msgSlotLabel2_ = Localization.localize("DVA_MSG_SLOT_LABEL_2", msgSlotLabel2_);

				mbSelectSlotToAbort_ = Localization.localize("MB_SELECT_SLOT_TO_ABORT", mbSelectSlotToAbort_);
                mbSelectSlotToLoad_ = Localization.localize("MB_SELECT_SLOT_TO_LOAD", mbSelectSlotToLoad_);

                // Zone List
                dataGridView_SelectedZones_Occ.Columns[0].HeaderText = Localization.localize("DGV_LOC", dataGridView_SelectedZones_Occ.Columns[0].HeaderText);
                dataGridView_SelectedZones_Occ.Columns[1].HeaderText = Localization.localize("DGV_ZONE", dataGridView_SelectedZones_Occ.Columns[1].HeaderText);

                dataGridView_SelectedZones.Columns[0].HeaderText = Localization.localize("DGV_ZONE", dataGridView_SelectedZones.Columns[0].HeaderText);

                label_seat.Text = Localization.localize("LABLE_SEAT", label_seat.Text);
                label_language.Text = Localization.localize("LABLE_PLAY_LANGUAGE", label_language.Text);
                label_recycle_time.Text = Localization.localize("LABLE_PLAY_TIMES", label_recycle_time.Text);
                label_cyclic_interval.Text = Localization.localize("LABLE_PLAY_INTERVAL", label_cyclic_interval.Text);
                label_sec.Text = Localization.localize("LB_SEC", label_sec.Text);

                statusDescMap_[BroadcastStatusEnum.InProgress] = statusInProgress_;
                statusDescMap_[BroadcastStatusEnum.Interrupted] = statusFinishAdnormal_;
                statusDescMap_[BroadcastStatusEnum.Completed] = statusFinishNoFail_;

            }
		}

		/// <summary>
		/// Method called to load the selected message sequence
		/// </summary>
		/// <param name="sender">Control that raised the event</param>
		/// <param name="e">Additional information about the event that was raised</param>
		/// <exception cref="NoOracleDBException">Handle No Oracle DB Exception</exception>
		/// <exception cref="Exception">Handle generic exception</exception>
		private void button_load_Click(object sender, EventArgs e)
		{
			// Nothing to validate : sender, e
			try
			{
				// Stop timer 
				stopTimer();

				// Must reset last known status everytime the load button is pressed
				lastDisplayedStatus_ = EnumType.DvaBcStatus.Initiated;

                int msgSeqNo = 0;//getSelectedSlotNo();

				if (msgSeqNo > 0 && msgSeqNo <= Program.MAXMSGSEQ)
				{
					populateForm(msgSeqNo);
				}
				else
				{
					Program.MessageBox_Error(mbSelectSlotToLoad_, sysErr_);
				}
			}
			catch (NoOracleDBException ex)
			{
				Program.WriteEventLog("[ERROR] [NoOracleDBException] " + ex.ToString(), fileName_ + " -> " + MethodInfo.GetCurrentMethod().Name + "()");
				Program.MessageBox_Error(Program.MB_NoDBConn, sysErr_);
			}
			catch (Exception ex)
			{
				Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + " -> " + MethodInfo.GetCurrentMethod().Name + "()");
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
		private async void button_abort_Click(object sender, EventArgs e)
		{
			// Nothing to validate : sender, e
			try
			{
                if (dvaMsgStatus_ != null)
                {
                    if(PaService.Instance.IsPaServerAvailable)
                    {
                        byte[] stationIds = dvaMsgStatus_.Stations.Split(',').Select(s => Convert.ToByte(s.Trim())).ToArray();

                        M45 cmd = new M45(Convert.ToByte(dvaMsgStatus_.SeatId), stationIds);
                        bool result = await PaService.Instance.StopBroadcast(cmd);
                        if(result)
                        {
                            dvaMsgStatus_.Status = (int)BroadcastStatusEnum.Interrupted;
                            UserFunctions.UpdateDvaMessageStatus(dvaMsgStatus_);
                            ListBoxClear(listBox_Dva_Bc_status);
                            ListBoxAppend(listBox_Dva_Bc_status, statusDescMap_[BroadcastStatusEnum.Interrupted]);
                        }
                        else
                        {
                            Program.WriteEventLog("failed to stop broadcast.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
                        }
                    }
                }
			}
			catch (Exception ex)
			{
				Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				Program.MessageBox_Error(Program.MB_GeneralEx, sysErr_);
			}
		}

		/// <summary>
		/// Return the currently selected msg slot number
		/// </summary>
		/// <returns>The number of the currently selected slot number</returns>
		/// <exception cref="ArgumentOutOfRangeException">Handle the Argument Out Of Range exception</exception>
		/// <exception cref="Exception">Handle generic exception</exception>
		//private int getSelectedSlotNo()
		//{
		//	int msgSeqId = 0;

		//	try
		//	{
		//		msgSeqId = Convert.ToInt32(cb_msgSeqSlot.Text, 10);
		//	}
		//	catch (Exception ex)
		//	{
		//		msgSeqId = 0;
		//		Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
		//	}

		//	return msgSeqId;
		//}

		/// <summary>
		/// Clear the whole form
		/// </summary>
		/// <since>00.03.18.02</since>
		private void clearForm()
		{
			// Clear everything
			//listBox_period.Items.Clear();
			//listBox_start.Items.Clear();
			//listBox_stop.Items.Clear();
			//listBox_chime.Items.Clear();
			//listBox_period.Items.Clear();
			//listBox_msg_no.Items.Clear();
			//listBox_dwell.Items.Clear();
			ListBoxClear(listBox_Dva_Bc_status);
			dataGridView_SelectedZones_Occ.Rows.Clear();
			dataGridView_SelectedZones.Rows.Clear();
		}

		/// <summary>
		/// Populate Form HMI components
		/// </summary>
		/// <param name="selMsgSeqSlotNo">Selected message sequence slot number to show</param>
		private void populateForm(int selMsgSeqSlotNo)
		{
			// Nothing to validate : selMsgSeqSlotNo
			DvaMessageSequenceStruct dvaMsgSeq = new DvaMessageSequenceStruct();

			clearForm();

			// Query Database for DVA Details - Table 303/1303	
			dvaMsgSeq = getMsgSeq(selMsgSeqSlotNo);

			if (dvaMsgSeq.dvaMessage[0] != 0)
			{
				string[] selDvaMsg = null;

				// Display the DVA broadcast attributes in the UI e.g. chime, priority...etc
				//listBox_period.Items.Add(dvaMsgSeq.periodSecs.ToString());
				//listBox_start.Items.Add(dvaMsgSeq.startTime.ToString());
				//listBox_stop.Items.Add(dvaMsgSeq.stopTime.ToString());
				//listBox_chime.Items.Add(DvaChimeStatus(dvaMsgSeq.chimeFlag));
				//listBox_dwell.Items.Add(dvaMsgSeq.dwellTime.ToString());

				// Convert DVA message numbers to message labels
				selDvaMsg = UserFunctions.Query_MsgNo_To_MsgLabel(dvaMsgSeq.dvaMessage);

				// Display the messages that is to be broadcasted 
				if (selDvaMsg != null)
				{
					//listBox_msg_no.Items.Add(selDvaMsg[0]);
					//listBox_msg_no.Items.Add(selDvaMsg[1]);
					//listBox_msg_no.Items.Add(selDvaMsg[2]);
					//listBox_msg_no.Items.Add(selDvaMsg[3]);
				}
				else
				{
					Program.WriteEventLog("[ERROR] Message is empty!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				}

				// Display selected Zones
				if (Program.occMode)
				{
					bool[] selBcZoneOcc = DataManipulation.ByteToBoolArray_Zone(dvaMsgSeq.zoneCoverage);

					for (int stnIndex = 0; stnIndex < Program.MAXSTNID; stnIndex++)
					{
						for (int z = 0; z < Program.MAXZONEID; z++)
						{
							int stnZoneIndex = (stnIndex * Program.MAXZONEID) + z;

							if (selBcZoneOcc[stnZoneIndex] && DataObject.stnArray[stnZoneIndex] != null)
							{
								string[] bcProgress = new string[] { DataObject.stnArray[stnZoneIndex].stnName, DataObject.stnArray[stnZoneIndex].zoneName };
								dataGridView_SelectedZones_Occ.Rows.Add(bcProgress);
							}
						}
					}
				}
				else
				{
					bool[] selBcZone = DataManipulation.ByteToBoolArray_Zone(dvaMsgSeq.zoneCoverage);

					for (int i = 0; i < Program.MAXZONEID; i++)
					{
                        if (selBcZone[i])
                        {
                            if (DataObject.zoneArray[i] != null)
                            {
                                string[] bcProgress = { DataObject.zoneArray[i].zoneName };
                                dataGridView_SelectedZones.Rows.Add(bcProgress);
                            }
                        }
					}
				}
			}
			else
			{
				//listBox_period.Items.Add("-");
				//listBox_start.Items.Add("-");
				//listBox_stop.Items.Add("-");
				//listBox_chime.Items.Add("-");
				//listBox_period.Items.Add("-");
				//listBox_msg_no.Items.Add("-");
				//listBox_dwell.Items.Add("-");
				//listBox_msg_no.Items.Add("-");

				ListBoxAppend(listBox_Dva_Bc_status, msgSlotLabel1_ + " " + selMsgSeqSlotNo + " " + msgSlotLabel2_);
			}
		}

		/// <summary>
		/// Get the details of the selected DVA Message Sequence Slot
		/// </summary>
		/// <param name="selectedMsgSeqNo">Number of Message Slot</param>
		/// <returns>DVA Message Sequence Details</returns>
		/// <exception cref="NoOracleDBException">Handle No Oracle DB Exception</exception>
		/// <exception cref="Exception">Handle generic exception</exception>
		private DvaMessageSequenceStruct getMsgSeq(int selectedMsgSeqNo)
		{
            return null;
			// Nothing to validate : selectedMsgSeqNo
			//DvaMessageSequenceStruct dvaMsgSeq = new DvaMessageSequenceStruct();
			//dbConnEnum dbConn = dbConnEnum.ErrNoConn;
			//byte[] selZones = null;

			//string localSql = " SELECT CHIME, DVA_MSG1, DVA_MSG2, DVA_MSG3, DVA_MSG4," +
			//					" DWELL_TIME, PERIOD, START_TIME, STOP_TIME, EVENT_TRIGGERED " +
			//					" FROM PA_DVA_MSG_SEQ WHERE MSG_SEQ_ID = " + selectedMsgSeqNo;
			//Program.dbLock.oracleQuery2(ref oracleDr_, localSql, ref dbConn);

			//if (oracleDr_.Read())
			//{
			//	if (!oracleDr_.IsDBNull(0))
			//	{
			//		dvaMsgSeq.chimeFlag = oracleDr_.GetOracleDecimal(0).ToInt32();
			//	}

			//	if (!oracleDr_.IsDBNull(1))
			//	{
			//		dvaMsgSeq.dvaMessage[0] = oracleDr_.GetOracleDecimal(1).ToInt32();
			//	}

			//	if (!oracleDr_.IsDBNull(2))
			//	{
			//		dvaMsgSeq.dvaMessage[1] = oracleDr_.GetOracleDecimal(2).ToInt32();
			//	}

			//	if (!oracleDr_.IsDBNull(3))
			//	{
			//		dvaMsgSeq.dvaMessage[2] = oracleDr_.GetOracleDecimal(3).ToInt32();
			//	}

			//	if (!oracleDr_.IsDBNull(4))
			//	{
			//		dvaMsgSeq.dvaMessage[3] = oracleDr_.GetOracleDecimal(4).ToInt32();
			//	}

			//	if (!oracleDr_.IsDBNull(5))
			//	{
			//		dvaMsgSeq.dwellTime = oracleDr_.GetOracleDecimal(5).ToInt32();
			//	}

			//	if (!oracleDr_.IsDBNull(6))
			//	{
			//		dvaMsgSeq.periodSecs = oracleDr_.GetOracleDecimal(6).ToInt32();
			//	}

			//	if (!oracleDr_.IsDBNull(7))
			//	{
			//		dvaMsgSeq.startTime = oracleDr_.GetDateTime(7);
			//	}

			//	if (!oracleDr_.IsDBNull(8))
			//	{
			//		dvaMsgSeq.stopTime = oracleDr_.GetDateTime(8);
			//	}

			//	if (!oracleDr_.IsDBNull(9))
			//	{
			//		dvaMsgSeq.eventTrigger = oracleDr_.GetOracleDecimal(9).ToInt32();
			//	}
			//}

			//// Read Selected Zones Details
			//localSql = " SELECT BYTE_INDEX, BYTE_VALUE " +
			//			" FROM PA_DVA_MSG_SEQ_ZONES WHERE MSG_SEQ_ID = " + selectedMsgSeqNo;
			//Program.dbLock.oracleQuery2(ref oracleDr_, localSql, ref dbConn);

			//if (Program.occMode)
			//{
			//	selZones = new byte[DataObject.occCoverageSize];
			//}
			//else
			//{
			//	selZones = new byte[DataObject.stnCoverageSize];
			//}

			//while (oracleDr_.Read())
			//{
			//	int byteIndex = 0;
			//	if (!oracleDr_.IsDBNull(0))
			//	{
			//		byteIndex = oracleDr_.GetOracleDecimal(0).ToInt32();
			//	}

			//	if (!oracleDr_.IsDBNull(1))
			//	{
			//		selZones[byteIndex] = Convert.ToByte(oracleDr_.GetOracleDecimal(1).ToInt32());
			//	}
			//}

			//if (oracleDr_ != null)
			//{
			//	oracleDr_.Close();
			//}

			//dvaMsgSeq.zoneCoverage = selZones;

			//return dvaMsgSeq;
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

		/// <summary>
		/// Operator clicks the window Close button 
		/// </summary>
		/// <param name="sender">Control that raised the event</param>
		/// <param name="e">Additional information about the event that was raised</param>
		private void DvaStatus_FormClosing(object sender, FormClosingEventArgs e)
		{
            // Nothing to validate : sender, e
            PaService.Instance.OnDvaStatusChanged -= this.PaService_OnDvaStatusChanged;
            if (!Can_Close_Form())
			{
				e.Cancel = true;
			}
			else
			{
				cleanUp();
			}
		}

		/// <summary>
		/// Method invoked when form is closing
		/// </summary>
		/// <returns>bool: true if form can be closed, otherwise false</returns>
		private bool Can_Close_Form()
		{
			if (clearImmediateSlotFlag_)
			{
				Program.MessageBox_Warn(mbClearDvaSlot_, sysErr_, this);
			}
			else if (clearCyclicalSlotFlag_)
			{
				Program.MessageBox_Warn(mbClearCyclicalSlot_, sysErr_, this);
			}
			else
			{
				return true;
			}

			return false;
		}

		/// <summary>
		/// Timer to update progress of the DVA Broadcast 
		/// </summary>
		/// <param name="sender">Control that raised the event</param>
		/// <param name="e">Additional information about the event that was raised</param>
		/// <exception cref="TableException">Handle Table exception</exception>
		/// <exception cref="TimeOutException">Handle Timeout exception</exception>
		/// <exception cref="Exception">Handle generic exception</exception>
		private void timer_updateprogress_Tick(object sender, EventArgs e)
		{
		}

		/// <summary>
		/// Decode the meaning of the DVA status - 0-based index
		/// </summary>
		/// <param name="statusCode">Status code of the DVA status</param>
		/// <returns>DVA status message</returns>
		private string DvaBcStatus(EnumType.DvaBcStatus statusCode)
		{
			// Nothing to validate : statusCode
			string bcStatus = "";

			switch ((int)statusCode)
			{
				case 0:
					bcStatus = statusInit_;
					break;
				case 1:
					bcStatus = statusInProgress_;
					break;
				case 2:
					break;
				case 3:
					bcStatus = statusDwellTime_;
					break;
				case 4:
					bcStatus = statusNoAvailChn_;
					break;
				case 5:
					bcStatus = statusMsgInUse_;
					break;
				case 6:
					bcStatus = statusChnOverriden_;
					break;
				case 7:
					break;
				case 8:
					bcStatus = statusFinishAdnormal_;
					break;
				case 9:
					bcStatus = statusFinishNoFail_;
					break;
				case 10:
					bcStatus = statusFinishGotFail_;
					break;
				default:
					bcStatus += "Unknown Status (Error code 1)";
					break;
			}
			return bcStatus;
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
					statusMsg = "--";
					break;
			}

			return statusMsg;
		}

		/// <summary>
		/// The cleanUp method is for freeing up resource that was held by the class during runtime.
		/// </summary>
		/// <since>00.03.11.00</since>
		private void cleanUp()
		{

		}

		/// <summary>
		/// Clear everything from the listbox
		/// </summary>
		/// <param name="oControl">Name of the control</param>
		/// <exception cref="Exception">Handle generic exception</exception>
		private void ListBoxClear(Control oControl)
		{
			ListBox listBoxObj = oControl as ListBox;
			listBoxObj.Items.Clear();

		}

		/// <summary>
		/// Append to Listbox
		/// </summary>
		/// <param name="oControl">Object refence to control</param>
		/// <param name="msgIn">Message to be appended</param>
		/// <exception cref="Exception">Handle generic exception</exception>
		private void ListBoxAppend(Control oControl, string msgIn)
		{
			ListBox listBoxObj = oControl as ListBox;
			listBoxObj.Items.Add(msgIn);

			while (listBoxObj.Items.Count >= 100)
			{
				listBoxObj.Items.RemoveAt(0);
			}

			listBoxObj.SelectedIndex = listBoxObj.Items.Count - 1;
		}

		/// <summary>
		/// Start timer to observe broadcast progress
		/// </summary>
		private void startTimer()
		{
		}

		/// <summary>
		/// Stop timer that observes the broadcast progress
		/// </summary>
		private void stopTimer()
		{
		}

	}
}