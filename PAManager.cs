#region Header Block
/// <classname>PAManager</classname>
/// <description>This module allows the operator to perform Live Broadcast, DVA Broadcast,
/// Record an adhoc message, etc.
/// </description>
/// <filename>PAManager.cs</filename>
/// <authors>Tay Yu Hui</authors>
#endregion

using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Reflection;
using System.Threading;
using System.Windows.Forms;
using System.Data;
using System.ComponentModel;
using System.Linq;
using ste.pa.cos;
using ste.common.exceptions;
using ste.common.util;
using ste.common.Forms;

namespace ste.pa.pamanager
{
    /// <summary>
    /// Main GUI of PA Manager
    /// </summary>
    public partial class PAManager : Form
    {
        LoadingTask loadingTask_ = null;

        // Note: All times are stored in utc(+0) timezone format
        #region "Variables"
        //private int maxRetry = 3;
        private string fileName_ = MethodBase.GetCurrentMethod().DeclaringType.Name;
        private string localSql_ = "";

        private Thread pollZoneThread_ = null;

        private PaFunctions PA_ = new PaFunctions();    //	DO NOT REMOVE THIS, Require the init of socket
        private bool taskIsLoading_ = false;
        private bool paServerConnected_ = false;
        private System.Threading.Timer _paServiceStatusTimer = null;

        private delegate void SetCursorTypeDelegate(Cursor cursorType);

        // Flag to indicate if PA Manager is just started up or not
        //private bool startUp_ = true;

        // Stores the currently selected Station No (1-based) - for OCC mode
        private int currSelStnNo_ = 0;
        private int localLocationId_ = 0;
        private int localConsoleId_ = 54;
        private string localLocationNo_ = string.Empty;
        private string preRecordedMessageVersion_ = string.Empty;
        private string paServerPriIP_ = string.Empty;
        private string paServerSecIP_ = string.Empty;
        private int paServerPort_ = 0;

        // Stores the Zone Amp Status details from Corba
        private byte[] zoneAmpStatus_ = null;

        private DateTime preAgentZoneUpdateTime_ = DateTime.Now;

        // Map respective ID to their value
        private Dictionary<int, CheckBox> stnNoToStnCbDic_ = new Dictionary<int, CheckBox>();
        private Dictionary<int, string> catIdToNameDic_ = new Dictionary<int, string>();
        private Dictionary<int, CheckBox> catIdToCbDic_ = new Dictionary<int, CheckBox>();
        private Dictionary<int, string> typeIdToNameDic_ = new Dictionary<int, string>();
        private BindingList<PreRecordedMessageStruct> preRecordedMsgs_ = new BindingList<PreRecordedMessageStruct>();
        private BindingList<PreRecordedMessageStruct> normalPreRecordedMsgs_ = new BindingList<PreRecordedMessageStruct>();
        private BindingList<PreRecordedMessageStruct> emergencyPreRecordedMsgs_ = new BindingList<PreRecordedMessageStruct>();
        private PreRecordedMessageStruct selectedPreRecordedMsg_ = null;

        private delegate void SetConnectionStatusCallback(Control oControl, bool paConnected);
        private delegate void DgvClearCallback(Control oControl);
        private delegate void DgvAddRowCallback(Control oControl, string[] rowData);
        private delegate void SetFormTitle(Control oControl, string titleText);

        // Source Codes
        private int srcCodeDva_ = 0;
        private int srcCodeBgm_ = 0;
        private int srcCodePbZone_ = 0;

        private CheckBox[] zoneCheckBoxArray_ = new CheckBox[Program.MAXZONEID];

        // Monitor mode
        // Store the current displayed version of dva messages 
        private string[] dvaVersion_ = new string[Program.NBADHOC + 1];

        // Thread
        private object bcInProgressLock_ = new object();
        private bool bcInProgressFlag_ = false;


        // Table
        private int packetSize_ = 0;
        private byte[] receivedPacket_ = null;

        // Store the Zone Status - For both OCC and Stn mode
        private byte[] globalZoneStatusData_ = null;

        // For CC Use!!!
        private volatile bool runPollZoneThread = true;

        // Checkbox
        private Bitmap CbCheckedImage_ = null;
        private Bitmap CbUncheckedImage_ = null;
        private Assembly assemblyDetails_ = null;

        private string sysErr_ = "PA Manager";
        private string mbConfigError_ = "Unable to load Configuration File!";
        private string lbZone_ = " - Zones";
        private string lbSelect_ = "Select ";
        private string mbDvaBcReqOk_ = "DVA Cyclic Broadcast Request succesfully submitted";
        private string mbDvaBcReqFail_ = "DVA Cyclic Broadcast Request FAILED!";
        private string mbNoFreeSlot_ = "No Free Message Slot available for broadcast use. \n" +
                                        "You will be directed to the DVA Status window to free a message slot.";
        private string mbOngoingDvaBc_ = "Ongoing DVA Broadcast detected.\nDVA Broadcast window will be shown.";
        private string mbCyclicalError_ = "Emergency Message cannot be added to Cyclical Broadcast";
        private string mbCyclicalTimeError_ = "Start Time and Stop Time cannot be the same for Cyclic Broadcast!";
        private string mbNoSlotSelected_ = "Please select a adhoc slot for recording use";
        private string mbPlaybackStopFail_ = "Playback cannot be stopped! \nPlease try again!";
        private string mbPlaybackFail_ = "DVA Playback FAILED!";
        private string mbOccNoBgm_ = "BGM not available for OCC!";
        private string mbBgmStartFail_ = "BGM cannot be started!";
        private string mbBgmStopFail_ = "BGM cannot be stopped!";
        private string paControlMode_ = "PA Manager (Control Mode)";
        private string paMonitorMode_ = "PA Manager (Monitor Mode)";
        private string getLockPass_ = "Successful acquire control of PA!";
        private string getLockFail_ = "Failed to acquire control of PA!";
        private string mbPbZoneValErr_ = "Invalid Playback Zone ID!";
        private string lbHost_ = "Host ";
        private string lbHasPaLock_ = " is currently controlling PAS!";
        private string paConnected_ = "Connected";
        private string paDisconnected_ = "Disconnected";
        private string paSysUpdating_ = "PA System is currently being updated. Please try again later.";
        private string adhocEmer_ = "E.Ad Hoc message ";
        private string adhocNor_ = "N.Ad Hoc message ";
        private string lbDetails_ = "Details";
        private string lbTerminateNormal_ = "Normal";
        private string mbUncheckBeforePlayback_ = "Please untick cyclic broadcast checkbox for playback.";
        private string mbSelectZoneMsg_ = "Please select the zone.";
        private string mbSelectStationMsg_ = "Please select the station.";
        private string mbSelectMessageMsg_ = "Please select the predefined message.";
        private string cmfSendMsg_ = "Send this message?";
        public static string mbSendMsgSuccessful_ = "Message sent successfully!";
        public static string mbSendMsgFailed_ = "Failed to send message!";
        public static string mbSendCmdSuccessful_ = "Command sent successfully!";
        public static string mbSendCmdFailed_ = "Failed to send Command!";
        #endregion

        /// <summary>
        /// Static Constructor
        /// </summary>
        static PAManager()
        {
        }

        /// <summary>
        /// Method used to set running of poll zone thread to true
        /// </summary>
        /// <param name="isEnabled">bool: true if thread is to run, otherwise false</param>
        private void setRunPollZoneThread(bool isEnabled)
        {
            runPollZoneThread = isEnabled;
        }

        /// <summary>
        /// Default constructor
        /// </summary>
        public PAManager()
        {
            InitializeComponent();
            this.Shown += PAManager_Shown;
        }

        private void PAManager_Shown(object sender, EventArgs e)
        {
            this.TopMost = true;
            this.TopMost = false;
            this.Activate();
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
                this.Text = Localization.localize("PA_FORM_NAME", this.Text);
                sysErr_ = Localization.localize("PA_FORM_NAME", sysErr_);

                groupBox_zones.Text = Localization.localize("GP_ZONES", groupBox_zones.Text);
                groupBox_station.Text = Localization.localize("GP_STATION", groupBox_station.Text);
                groupBox_Prerecmsg.Text = Localization.localize("GP_PREREC_MSG", groupBox_Prerecmsg.Text);
                tabPage_normal_msg.Text = Localization.localize("LB_NORMAL_MSG", tabPage_normal_msg.Text);
                tabPage_emergency_msg.Text = Localization.localize("LB_EMERG_MSG", tabPage_emergency_msg.Text);
                groupBox_BCconfig.Text = Localization.localize("GP_BC_CONFIG", groupBox_BCconfig.Text);
                groupBox_zonelist.Text = Localization.localize("GP_ZONE_LIST", groupBox_zonelist.Text);

                button_selectall_stn.Text = Localization.localize("BUT_SELECT_ALL", button_selectall_stn.Text);
                button_unselectall_stn.Text = Localization.localize("BUT_UNSELECT_ALL", button_unselectall_stn.Text);
                button_selectall_zone.Text = Localization.localize("BUT_SELECT_ALL", button_selectall_zone.Text);
                button_unselectall_zone.Text = Localization.localize("BUT_UNSELECT_ALL", button_unselectall_zone.Text);
                button_clearOne_msg.Text = Localization.localize("BUT_CLEAR", button_clearOne_msg.Text);
                button_station_status.Text = Localization.localize("BUT_STATION_STATUS", button_station_status.Text);

                checkBox_status.Text = Localization.localize("DISCONNECTED", checkBox_status.Text);
                button_configZone.Text = Localization.localize("BUT_CONFIG", button_configZone.Text);
                btn_DVA_Version.Text = Localization.localize("DVAVERSION_FORM_NAME", btn_DVA_Version.Text);
                button_DvaStatus.Text = Localization.localize("DVASTATUS_FORM_NAME", button_DvaStatus.Text);
                button_execute.Text = Localization.localize("BUT_EXECUTE", button_execute.Text);
                button_close.Text = Localization.localize("BUT_CLOSE", button_close.Text);
                button_device_status.Text = Localization.localize("BUT_DEVICE_STATUS", button_device_status.Text);
                button_live.Text = Localization.localize("BUT_LIVE", button_live.Text);
                button_air_alarm.Text = Localization.localize("BUT_AIR_RAID", button_air_alarm.Text);


                mbConfigError_ = Localization.localize("CONFIG_FILE_ERROR", mbConfigError_);

                mbUncheckBeforePlayback_ = Localization.localize("MB_UNCHECK_FOR_PLAYBACK", mbUncheckBeforePlayback_);
                mbSelectZoneMsg_ = Localization.localize("MB_SELECT_ZONE_MSG", mbSelectZoneMsg_);
                mbSelectStationMsg_ = Localization.localize("MB_SELECT_STATION_MSG", mbSelectStationMsg_);
                mbSelectMessageMsg_ = Localization.localize("MB_SELECT_MESSAGE_MSG", mbSelectMessageMsg_);
                cmfSendMsg_ = Localization.localize("MB_CONFIRM_SEND_MSG", cmfSendMsg_);
                mbSendMsgSuccessful_ = Localization.localize("MB_SEND_MSG_SUCCESSFULLY", mbSendMsgSuccessful_);
                mbSendMsgFailed_ = Localization.localize("MB_SEND_MSG_FAILED", mbSendMsgFailed_);
                mbSendCmdSuccessful_ = Localization.localize("MB_SEND_CMD_SUCCESSFULLY", mbSendCmdSuccessful_);
                mbSendCmdFailed_ = Localization.localize("MB_SEND_CMD_FAILED", mbSendCmdFailed_);

                //checkBox_cyclicbroadcast.Text = Localization.localize("CB_CYCLICAL_BC", checkBox_cyclicbroadcast.Text);
                label_recycle_time.Text = Localization.localize("LB_RECYCLE_TIME", label_recycle_time.Text);
                label_cyclic_interval.Text = Localization.localize("LB_TIME_INTERVAL", label_cyclic_interval.Text);
                //label_minute.Text = Localization.localize("LB_MIN", label_minute.Text);
                label_sec.Text = Localization.localize("LB_SEC", label_sec.Text);

                // Zone List
                dataGridView_OCC_selectedzone.Columns[0].HeaderText = Localization.localize("DGV_LOC", dataGridView_OCC_selectedzone.Columns[0].HeaderText);
                dataGridView_OCC_selectedzone.Columns[1].HeaderText = Localization.localize("DGV_ZONE", dataGridView_OCC_selectedzone.Columns[1].HeaderText);
                dataGridView_OCC_selectedzone.Columns[2].HeaderText = Localization.localize("DGV_AREA", dataGridView_OCC_selectedzone.Columns[2].HeaderText);

                dataGridView_selectedzone.Columns[0].HeaderText = Localization.localize("DGV_ZONE", dataGridView_selectedzone.Columns[0].HeaderText);
                dataGridView_selectedzone.Columns[1].HeaderText = Localization.localize("DGV_AREA", dataGridView_selectedzone.Columns[1].HeaderText);

                checkBox_chinese.Text = Program.langChinese_;
                checkBox_taiwanese.Text = Program.langTw_;
                checkBox_hakka.Text = Program.langHakka_;
                checkBox_english.Text = Program.langEnglish_;

                label_seats.Text = Localization.localize("LABLE_SEAT", label_seats.Text);
                label_language.Text = Localization.localize("LABLE_PLAY_LANGUAGE", label_language.Text);
                label_recycle_time.Text = Localization.localize("LABLE_PLAY_TIMES", label_recycle_time.Text);
                label_cyclic_interval.Text = Localization.localize("LABLE_PLAY_INTERVAL", label_cyclic_interval.Text);
                label_peak.Text = Localization.localize("LABLE_PEAK", label_peak.Text);
                label_offpeak.Text = Localization.localize("LABLE_OFF_PEAK", label_offpeak.Text);
                label_night.Text = Localization.localize("LABLE_NIGHT", label_night.Text);
                label_record_id.Text = Localization.localize("LABLE_ID", label_record_id.Text);
                label_sec.Text = Localization.localize("TB_SECOND", label_sec.Text);

                button_volume.Text = Localization.localize("BUT_CONFIG_VOLUME", button_volume.Text);
                button_require_record.Text = Localization.localize("BUT_REQUEST_RECORD", button_require_record.Text);
                button_start_record.Text = Localization.localize("BUT_START_RECORD", button_start_record.Text);
                button_end_record.Text = Localization.localize("BUT_END_RECORD", button_end_record.Text);
                button_station_status.Text = Localization.localize("BUT_STATION_STATUS", button_station_status.Text);



                if (Program.showIndex == false)
                {
                    dataGridView_OCC_selectedzone.Columns[3].Visible = false;
                    dataGridView_selectedzone.Columns[2].Visible = false;
                }

                mbDvaBcReqOk_ = Localization.localize("MB_DVA_REQ_OK", mbDvaBcReqOk_);
                mbDvaBcReqFail_ = Localization.localize("MB_DVA_REQ_FAIL", mbDvaBcReqFail_);
                mbNoFreeSlot_ = Localization.localize("MB_DVA_NO_SLOT", mbNoFreeSlot_);
                mbOngoingDvaBc_ = Localization.localize("MB_ONGOING_BC", mbOngoingDvaBc_);
                mbCyclicalError_ = Localization.localize("MB_CYCLICAL_ERROR", mbCyclicalError_);
                mbCyclicalTimeError_ = Localization.localize("MB_TIME_ERROR", mbCyclicalTimeError_);
                mbNoSlotSelected_ = Localization.localize("MB_NO_SLOT_SELECTED", mbNoSlotSelected_);
                mbPlaybackStopFail_ = Localization.localize("MB_PLAYBACK_STOP_FAIL", mbPlaybackStopFail_);
                mbPlaybackFail_ = Localization.localize("MB_PLAYBACK_FAIL", mbPlaybackFail_);
                mbOccNoBgm_ = Localization.localize("MB_OCC_NO_BGM", mbOccNoBgm_);
                mbBgmStartFail_ = Localization.localize("MB_BGM_START_FAIL", mbBgmStartFail_);
                mbBgmStopFail_ = Localization.localize("MB_BGM_STOP_FAIL", mbBgmStopFail_);
                mbPbZoneValErr_ = Localization.localize("MB_PB_ZONE_VALUE", mbPbZoneValErr_);
                lbZone_ = Localization.localize("LB_Zone", lbZone_);
                lbSelect_ = Localization.localize("LB_SELECT", lbSelect_);
                lbHost_ = Localization.localize("LB_HOST", lbHost_);
                lbHasPaLock_ = Localization.localize("LB_HAS_LOCK", lbHasPaLock_);
                paControlMode_ = Localization.localize("MODE_CONTROL", paControlMode_);
                paMonitorMode_ = Localization.localize("MODE_MONITOR", paMonitorMode_);
                getLockPass_ = Localization.localize("LOCK_ACQUIRE_SUCCESS", getLockPass_);
                getLockFail_ = Localization.localize("LOCK_ACQUIRE_FAIL", getLockFail_);
                paConnected_ = Localization.localize("CONNECTED", paConnected_);
                paDisconnected_ = Localization.localize("DISCONNECTED", paDisconnected_);
                paSysUpdating_ = Localization.localize("PA_SYS_UPDATING", paSysUpdating_);
                adhocEmer_ = Localization.localize("EMER_ADHOC", adhocEmer_);
                adhocNor_ = Localization.localize("NORMAL_ADHOC", adhocNor_);

                if (Program.defaultLanguage == "en-US")
                {
                    adhocEmer_ += " ";
                    adhocNor_ += " ";
                }

                lbDetails_ = Localization.localize("LB_DETAILS", lbDetails_);
                lbTerminateNormal_ = Localization.localize("LB_TERMINATE_NORMAL", lbTerminateNormal_);
            }
        }

        /// <summary>
        /// Method invoked when form is first loaded.
        /// </summary>
        /// <param name="sender">Control that raised the event</param>
        /// <param name="e">Additional information about the event that was raised</param>
        public void Form_PAManager_Load(object sender, EventArgs e)
        {
            // Nothing to validate : sender, e
            try
            {
                CheckForIllegalCrossThreadCalls = false;

                localizeForm();

                startLoadingTask();

                // Place form at center of screen
                this.CenterApplicationOnScreen(Program.screenX, Program.screenY, this);

                PA_.EventPaSysConnChange += PaSysConnChange;

                this.timer_Locking.Interval = (Program.paAgentLockInterval - 5) * 1000;

                LoadCheckboxImage();

                LoadConfig();

                currSelStnNo_ = Translater.stnIndex_to_StnNo(Program.profileLocIndex);

                // Initialize PaService
                PaService.Instance.Iniatilize(paServerPriIP_, paServerSecIP_, paServerPort_, localConsoleId_);
                PaService.Instance.OnDvaStatusChanged += PaService_OnDvaStatusChanged;
                PaService.Instance.OnRadioListVersionReceived += PaService_OnRadioListVersionReceived;
                PaService.Instance.OnStationStatusReceived += PaService_OnStationStatusReceived;

                StartPaServerStatusTimer();

                #region Load Database
                Program.occMode = isOccMode();
                Program.depotMode = isDepotMode();

                initVariables();

                GetPreRecordedMessageVersion();

                Load_StnNames();
                Load_ZoneCategory();
                Load_ZoneType();
                Load_PreRecordedMessage();
                Load_Pas_SourceCode();
                loadSeats();
                Load_station_device_name();

                if (Program.occMode)
                {
                    Load_ZoneInfo_OccMode();

                    // OCC mode - Initizalize the Station checkboxes at the Station Zones Panel
                    initStnCheckbox();
                }
                else
                {
                    Load_ZoneInfo_StnMode();

                    initStnCheckbox();
                }

                initZoneCheckbox();
                initZoneGrpCheckbox();

                ConfigurePaManDisplay();

                if (!Program.occMode)
                {
                    Update_ZonePanel_Cb_Display();
                }

                Program.WriteEventLog("[INFO] PA Configuration successfully loaded from Database.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
                #endregion

                #region Initialize dva configuration controls

                listBox_prerecorded_msg.DataSource = normalPreRecordedMsgs_;
                listBox_emergency_msg.DataSource = emergencyPreRecordedMsgs_;
                listBox_prerecorded_msg.SelectedIndex = -1;
                listBox_emergency_msg.SelectedIndex = -1;
                textBox_selected_msgcontent.Text = string.Empty;
                button_clearOne_msg.Enabled = false;

                comboBox_seats.DataSource = Program.seats_;
                comboBox_seats.DisplayMember = "Value";
                comboBox_seats.ValueMember = "Key";
                comboBox_seats.SelectedIndex = 0;

                comboBox_record_id.Items.Add(60001);
                comboBox_record_id.Items.Add(60002);
                comboBox_record_id.Items.Add(60003);
                comboBox_record_id.Items.Add(60004);
                comboBox_record_id.Items.Add(60005);
                comboBox_record_id.Items.Add(60006);
                comboBox_record_id.SelectedIndex = 0;

                numericUpDown_peak.Value = 100;
                numericUpDown_off_peak.Value = 100;
                numericUpDown_night.Value = 100;

                #endregion

                setConfigButton();

                if (Program.remoteMode)
                {
                    timer_Locking.Enabled = false;
                    Program.controlMode = false;
                    setPaMode();
                }
            }
            catch (CorbaCallFailureException ex)
            {
                stopLoadingTask();

                Program.WriteEventLog("[ERROR] [CorbaCallFailureException] " + Program.MB_NoPaAgent + " Details: " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
                Program.MessageBox_Error(Program.MB_NoPaAgent, sysErr_);

                this.Close();
            }
            catch (NoOracleDBException ex)
            {
                stopLoadingTask();

                // Error loading PA Configuration from database
                Program.WriteEventLog("[ERROR] [NoOracleDBException] " + Program.MB_NoDBConn + " Details: " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
                Program.MessageBox_Error(Program.MB_NoDBConn, sysErr_);

                this.Close();
            }
            catch (Exception ex)
            {
                stopLoadingTask();

                // Error loading PA Configuration
                string[] eventParam = new string[] { "", "", Localization.GetEventMessage("EVT_GENERAL_EX", "en-US") };
                string[] eventParamLocal = new string[] { "", "", Localization.GetEventMessage("EVT_GENERAL_EX", "zh-TW") };

                Program.logEvent(EnumType.PaEvent.PaStop, eventParam, eventParamLocal);
                Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

                Program.MessageBox_Error(Program.MB_FormEx, sysErr_);

                this.Close();
            }
            finally
            {
                stopLoadingTask();
            }

        }

        private void loadSeats()
        {
            try
            {
                dbConnEnum dbConn = dbConnEnum.ErrNoConn;
                Program.seats_.Clear();
                string localSql = $"SELECT ID, SEAT_DESC, SEAT_DESC_LOCAL FROM PA_SEATS ORDER BY ID ASC";
                var queries = new List<SqlQuery> { new SqlQuery { CommandText = localSql } };
                DataSet ds = Program.dbLock.FetchData(queries, ref dbConn);

                if (ds.Tables != null || ds.Tables.Count > 0)
                {
                    foreach (DataRow retrievedRow in ds.Tables[0].Rows)
                    {
                        if (Program.defaultLanguage == "en-US")
                        {
                            Program.seats_.Add(new KeyValuePair<int, string>(int.Parse(retrievedRow[0].ToString()), retrievedRow[1].ToString()));
                        }
                        else
                        {
                            Program.seats_.Add(new KeyValuePair<int, string>(int.Parse(retrievedRow[0].ToString()), retrievedRow[2].ToString()));
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                Program.WriteEventLog(ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }
        }

        private void PaService_OnRadioListVersionReceived(object sender, string e)
        {
            if (!string.Equals(e.Trim(), preRecordedMessageVersion_.Trim()))
            {
                try
                {
                    UpdatePreRecordedMessageVersion(e.Trim());
                    preRecordedMessageVersion_ = e;
                    Load_PreRecordedMessage();
                }
                catch (Exception ex)
                {
                    Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
                }
            }

        }

        private void PaService_OnDvaStatusChanged(object sender, EventArgs e)
        {
            DvaMessageStatusStruct msgStatus = UserFunctions.QueryLatestDvaMessageStatus(localLocationId_);
            if (msgStatus != null)
            {
                msgStatus.Status = (int)PaService.Instance.BroadcastStatus;
                UserFunctions.UpdateDvaMessageStatus(msgStatus);
            }
        }

        /// <summary>
        /// Method to load configuration from config.ini
        /// </summary>
        private void LoadConfig()
        {
            string locationIndex = ConfigFile.GetINIDataString("DEFAULT_MFT_SETTING", "DEF_LOCATION_ID", "", 255, Path.Combine(Program.configFolder_, "config.ini"));

            int result = 0;
            if (int.TryParse(locationIndex, out result))
            {
                localLocationId_ = result;
                localLocationNo_ = Translater.locId_to_locationNo(result);
            }

            string strConsoleId = ConfigFile.GetINIDataString("PA_MANAGER", "CONSOLE_ID", "", 255, Path.Combine(Program.configFolder_, "config.ini"));
            if (!string.IsNullOrEmpty(strConsoleId))
            {
                int.TryParse(strConsoleId, out localConsoleId_);
            }

            dbConnEnum dbConn = dbConnEnum.ErrNoConn;
            localSql_ = " SELECT PRI_IP, SEC_IP, PORT_NO FROM PA_CONFIG WHERE LOCATION_ID=" + locationIndex;
            var queries = new List<SqlQuery> { new SqlQuery { CommandText = localSql_ } };
            DataSet ds = Program.dbLock.FetchData(queries, ref dbConn);
            if (ds.Tables != null || ds.Tables.Count > 0)
            {
                foreach (DataRow retrievedRow in ds.Tables[0].Rows)
                {
                    paServerPriIP_ = retrievedRow[0].ToString();
                    paServerSecIP_ = retrievedRow[1].ToString();
                    paServerPort_ = int.Parse(retrievedRow[2].ToString());
                    break;
                }
            }
        }

        private void GetPreRecordedMessageVersion()
        {
            dbConnEnum dbConn = dbConnEnum.ErrNoConn;
            string localSql = $"SELECT DVA_ADHOC_PREREC FROM PA_DVA_VERSION WHERE LOCATION_ID={localLocationId_}";
            var queries = new List<SqlQuery> { new SqlQuery { CommandText = localSql } };
            DataSet ds = Program.dbLock.FetchData(queries, ref dbConn);

            if (ds.Tables != null && ds.Tables.Count > 0 && ds.Tables[0].Rows.Count > 0)
            {
                foreach (DataRow retrievedRow in ds.Tables[0].Rows)
                {
                    preRecordedMessageVersion_ = retrievedRow[0].ToString();
                    break;
                }
            }
            else
            {
                localSql = $"insert into pa_dva_version value({localLocationId_}, null,null,null,null,null,null,null)";

                queries = new List<SqlQuery> { new SqlQuery { CommandText = localSql } };
                if (Program.dbLock.ExcuteNoneResultQuery(queries, ref dbConn) < 1)
                {
                    Program.WriteEventLog("[ERROR] INSERT INTO pa_dva_version", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
                    return;
                }
            }
        }

        private void UpdatePreRecordedMessageVersion(string newVersion)
        {
            dbConnEnum dbConn = dbConnEnum.ErrNoConn;
            string localSql = $"update pa_dva_version set dva_adhoc_prerec='{newVersion}' where location_id={localLocationId_}";
            var queries = new List<SqlQuery> { new SqlQuery { CommandText = localSql } };
            if (Program.dbLock.ExcuteNoneResultQuery(queries, ref dbConn) < 1)
            {
                Program.WriteEventLog("[ERROR] update pa_dva_version", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
                return;
            }
        }

        /// <summary>
        /// Method to load pre recorded messages from database
        /// </summary>
        private void Load_PreRecordedMessage()
        {
            try
            {
                dbConnEnum dbConn = dbConnEnum.ErrNoConn;
                this.preRecordedMsgs_.Clear();
                this.normalPreRecordedMsgs_.Clear();
                this.emergencyPreRecordedMsgs_.Clear();
                string localSql = $"SELECT MSG_ID,MSG_LABEL,MSG_CONTENT FROM PA_PRERECORDED_MSG WHERE VERSION='{preRecordedMessageVersion_}' ORDER BY MSG_ID";
                var queries = new List<SqlQuery> { new SqlQuery { CommandText = localSql } };
                DataSet ds = Program.dbLock.FetchData(queries, ref dbConn);

                if (ds.Tables != null || ds.Tables.Count > 0)
                {
                    foreach (DataRow retrievedRow in ds.Tables[0].Rows)
                    {
                        PreRecordedMessageStruct msg = new PreRecordedMessageStruct()
                        {
                            MsgId = int.Parse(retrievedRow[0].ToString()),
                            MsgLabel = retrievedRow[1].ToString(),
                            MsgContent = retrievedRow[2].ToString()
                        };
                        preRecordedMsgs_.Add(msg);
                        if (msg.MsgId < 666)
                        {
                            emergencyPreRecordedMsgs_.Add(msg);
                        }
                        else
                        {
                            normalPreRecordedMsgs_.Add(msg);
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                Program.WriteEventLog(ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }
        }

        /// <summary>
        /// Method to start the control thread, when the host has exclusive access to the PA System
        /// </summary>
        private void startControlThread()
        {
            Program.WriteEventLog("[INFO] Starting Control Thread.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

            // Wait for a while for all previous thread to die
            Thread.Sleep(500);

            if (pollZoneThread_ == null ||
                (pollZoneThread_ != null && pollZoneThread_.ThreadState != ThreadState.Background))
            {
                pollZoneThread_ = new Thread(new ThreadStart(Control_PollZoneThread));
                pollZoneThread_.Name = "ControlPollZoneThread";
                pollZoneThread_.IsBackground = true;
                pollZoneThread_.Start();
            }
        }

        /// <summary>
        /// Method to stop the control thread
        /// </summary>
        private void stopControlThread()
        {
            Program.WriteEventLog("[INFO] Stopping Control Thread.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

            if (pollZoneThread_ != null && pollZoneThread_.ThreadState == ThreadState.Background)
            {
                pollZoneThread_.Abort();
            }
        }

        /// <summary>
        /// Method to start the monitor thread, when the host has no exclusive access to the PA System
        /// </summary>
        private void startMonitorThread()
        {
            Program.WriteEventLog("[INFO] Starting Monitor Thread.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

            if (pollZoneThread_ == null ||
                    (pollZoneThread_ != null && pollZoneThread_.ThreadState != ThreadState.Background))
            {
                runPollZoneThread = true;
                pollZoneThread_ = new Thread(new ThreadStart(Monitor_PollZoneThread));
                pollZoneThread_.Name = "MonitorPollZoneThread";
                pollZoneThread_.IsBackground = true;
                pollZoneThread_.Start();
            }
        }

        /// <summary>
        /// Method to stop the monitor thread
        /// </summary>
        private void stopMonitorThread()
        {
            Program.WriteEventLog("[INFO] Stopping Monitor Thread.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

            if (pollZoneThread_ != null && pollZoneThread_.ThreadState == ThreadState.Background)
            {
                pollZoneThread_.Abort();
            }
        }

        /// <summary>
        /// Start the loading form GUI
        /// </summary>
        private void startLoadingTask()
        {
            taskIsLoading_ = true;
            loadingTask_ = new LoadingTask(Program.screenX, Program.screenY);
        }

        /// <summary>
        /// Start the loading form GUI in the location when the current PA Manager form is located
        /// </summary>
        private void startLoadingTask2()
        {
            taskIsLoading_ = true;
            loadingTask_ = new LoadingTask(this.Location.X, this.Location.Y);
        }

        /// <summary>
        /// Stop the loading form GUI
        /// </summary>
        private void stopLoadingTask()
        {
            taskIsLoading_ = false;
            if (loadingTask_ != null)
            {
                loadingTask_.CleanUp();
            }
        }

        /// <summary>
        /// Method invoked when form is closing.
        /// </summary>
        /// <param name="sender">Control that raised the event</param>
        /// <param name="e">Additional information about the event that was raised</param>
        private void Form_PAManager_FormClosing(object sender, FormClosingEventArgs e)
        {
            // Nothing to validate : sender, e
            try
            {
                if (taskIsLoading_ == true)
                {
                    Program.MessageBox_Warn(paSysUpdating_, sysErr_, this);
                    e.Cancel = true;
                }
                else
                {
                    // Log the Application Ends event
                    string[] eventParam = new string[] { "", "", Localization.GetEventMessage("EVT_TERMINATE_NORMAL", "en-US") };
                    string[] eventParamLocal = new string[] { "", "", Localization.GetEventMessage("EVT_TERMINATE_NORMAL", "zh-TW") };
                    Program.logEvent(EnumType.PaEvent.PaStop, eventParam, eventParamLocal);

                    cleanUp();

                    Program.WriteEventLog("[INFO] PA Manager successfully closed.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
                }
            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }
        }

        /// <summary>
        /// The cleanUp method is for freeing up resource that was held by the class during runtime.
        /// </summary>
        private void cleanUp()
        {
            Program.WriteEventLog("[INFO] cleanup executed!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

            try
            {
                PA_.setMonitorSocketThreadState(false);

                if (pollZoneThread_ != null)
                {
                    pollZoneThread_.Abort();
                    pollZoneThread_.Join();
                }

                timer_Locking.Enabled = false;

                Program.WriteEventLog("[INFO] All poll threads aborted!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }
        }

        /// <summary>
        /// Initialize variables size depending on the OCC or Station Mode
        /// </summary>
        private void initVariables()
        {
            if (Program.occMode)
            {
                zoneAmpStatus_ = new byte[Program.MAXZONEID * Program.MAXSTNID * 2];
            }
            else
            {
                zoneAmpStatus_ = new byte[Program.MAXZONEID * 2];
            }
        }

        /// <summary>
        /// Enable or disable the configuration button
        /// </summary>
        private void setConfigButton()
        {
            if (Program.rightsToRunPaManagerConfiguration())
            {
                button_configZone.Enabled = true;
            }
            else
            {
                button_configZone.Enabled = false;
                Program.WriteEventLog("[ERROR] User [" + Program.localUser + "] have NO rights to configure!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }
        }

        /// <summary>
        /// Load PA System source code from database
        /// </summary>
        private void Load_Pas_SourceCode()
        {
            int sourceCode = 0;

            // Load sourcecode of DVA
            localSql_ = " SELECT PA_DVA_SOURCECODE FROM PA_SETTING WHERE LOCATION_ID=" + Program.profileLocIndex +
                        " AND HOST_NAME='" + Program.hostName + "' ";
            srcCodeDva_ = Program.mySqlQuery_SingleInt(localSql_);
            Program.WriteDebugEventLog("[DEBUG] DVA SourceCode: " + srcCodeDva_, fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

            // Only Station got BGM function
            localSql_ = " SELECT PA_BGM_SOURCECODE FROM PA_SETTING WHERE LOCATION_ID=" + Program.profileLocIndex +
                        " AND HOST_NAME='" + Program.hostName + "' ";
            srcCodeBgm_ = Program.mySqlQuery_SingleInt(localSql_);
            Program.WriteDebugEventLog("[DEBUG] BGM SourceCode: " + srcCodeBgm_, fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

            // PTT Source code - LIST 50
            localSql_ = " SELECT PA_EQUIP_SOURCECODE FROM PA_SETTING WHERE LOCATION_ID=" + Program.profileLocIndex + " AND HOST_NAME='" + Program.hostName + "'";
            sourceCode = Program.mySqlQuery_SingleInt(localSql_);
            Program.WriteDebugEventLog("[DEBUG] PA PTT SourceCode: " + sourceCode, fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            DataObject.Set_PTT_Source_Code(sourceCode);

            // PA Playback zone
            localSql_ = " SELECT PA_PB_ZONE FROM PA_SETTING WHERE LOCATION_ID=" + Program.profileLocIndex + " AND HOST_NAME='" + Program.hostName + "'";
            srcCodePbZone_ = Program.mySqlQuery_SingleInt(localSql_);
            Program.WriteDebugEventLog("[DEBUG] PA Playback Zone: " + srcCodePbZone_, fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
        }

        /// <summary>
        /// Load the images used in the checbox
        /// </summary>
        private void LoadCheckboxImage()
        {
            Stream imageStream = null;

            assemblyDetails_ = Assembly.GetExecutingAssembly();
            imageStream = assemblyDetails_.GetManifestResourceStream("ste.pa.pamanager.Resources.Checkbox_Checked.ico");
            CbCheckedImage_ = new Bitmap(imageStream);
            imageStream = assemblyDetails_.GetManifestResourceStream("ste.pa.pamanager.Resources.Checkbox_Unchecked.ico");
            CbUncheckedImage_ = new Bitmap(imageStream);
        }

        /// <summary>
        /// Determine whether to enable occ mode
        /// </summary>
        /// <returns>bool: true if occ mode is to be enabled, otherwise false</returns>
        private bool isOccMode()
        {
            return false;
            //int occLocIndex = 0;
            //bool occMode = false;

            //// Load the OCC location index
            //localSql_ = " SELECT LOCATION_ID FROM SYS_LOCATION WHERE LOCATION_NO='OCC' ";
            //occLocIndex = Program.mySqlQuery_SingleInt(localSql_);
            //Program.WriteDebugEventLog("[DEBUG] OCC Location Index: " + occLocIndex, fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

            //if (occLocIndex == Program.profileLocIndex)
            //{
            //	occMode = true;
            //	Program.WriteEventLog("[INFO] OCC Mode", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            //}
            //else
            //{
            //	occMode = false;
            //	Program.WriteEventLog("[INFO] Stn Mode", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            //}

            //return occMode;
        }

        /// <summary>
        /// Determine whether to enable Depot mode
        /// </summary>
        /// <returns>bool: true if Depot mode is to be enabled, otherwise false</returns>
        private bool isDepotMode()
        {
            return false;
            //int cpdLocIndex = 0;
            //int lcdLocIndex = 0;
            //bool depotMode = false;

            //// Load the DPT-CPD location index
            //localSql_ = " SELECT LOCATION_ID FROM SYS_LOCATION WHERE LOCATION_NO='DPT-CPD' ";
            //cpdLocIndex = Program.mySqlQuery_SingleInt(localSql_);

            //Program.WriteDebugEventLog("[DEBUG] DPT-CPD Location Index: " + cpdLocIndex, fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

            //// Load the DPT-CPD location index
            //localSql_ = " SELECT LOCATION_ID FROM SYS_LOCATION WHERE LOCATION_NO='DPT-LCD' ";
            //lcdLocIndex = Program.mySqlQuery_SingleInt(localSql_);

            //Program.WriteDebugEventLog("[DEBUG] DPT-LCD Location Index: " + lcdLocIndex, fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");


            //if (lcdLocIndex == Program.profileLocIndex || cpdLocIndex == Program.profileLocIndex)
            //{
            //    depotMode = true;
            //    Program.WriteEventLog("[INFO] Depot Mode", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            //}
            //else
            //{
            //    depotMode = false;
            //    Program.WriteEventLog("[INFO] Not Depot Mode", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            //}

            //return depotMode;
        }

        /// <summary>
        /// Method to poll the pa zones
        /// </summary>
        private void Control_PollZoneThread()
        {
            // Poll PA System for Zone Status
            try
            {
                Program.WriteEventLog("[INFO] [Control] Start ZONE polling thread...", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

                while (PaFunctions.Check_PAS_Connection_Status)
                {
                    try
                    {
                        double timeToSleep = 0;

                        int tableNo = 0;
                        int tableSize = 0;

                        if (PaFunctions.continuePollZone(out timeToSleep) == true)
                        {
                            if (Program.occMode)
                            {
                                // Table 1200 - GLOBAL ZONE STATUS
                                tableNo = 1200;
                                tableSize = Program.MAXSTNID * Program.MAXZONEID * 2;
                                packetSize_ = tableSize + Program.WRITE_HEADER_SIZE;

                                receivedPacket_ = PaFunctions.TableRequest(tableNo, packetSize_);

                                int stnOffset = 0;
                                byte[] selStationZoneStatus = null;

                                // Element size of each zone is 2
                                globalZoneStatusData_ = DataManipulation.byteTransferExcludeHeader(tableSize, receivedPacket_);

                                // Update Global Station panel
                                Refresh_Zone_Status_Display_OccLevel(globalZoneStatusData_);

                                // Extract Relevant Station->Zone data
                                stnOffset = (currSelStnNo_ - 1) * Program.MAXZONEID * 2;
                                selStationZoneStatus = new byte[Program.MAXZONEID * 2];

                                // Loop thru the desired range in the zoneStatusData Array for the selected zones list
                                for (int i = stnOffset; i < stnOffset + Program.MAXZONEID * 2; i++)
                                {
                                    selStationZoneStatus[i - stnOffset] = globalZoneStatusData_[i];
                                }

                                // Refresh Selected Station -> Zone Status Display Panel
                                Refresh_Zone_Status_Display(selStationZoneStatus);
                            }
                            else
                            {
                                // Table 200 - ZONE STATUS
                                tableNo = 200;
                                tableSize = Program.MAXZONEID * 2;
                                packetSize_ = tableSize + Program.WRITE_HEADER_SIZE;

                                receivedPacket_ = PaFunctions.TableRequest(tableNo, packetSize_);

                                // Duplicate Relevant Zone data
                                byte[] zoneStatus = new byte[receivedPacket_.Length - Program.WRITE_HEADER_SIZE];
                                globalZoneStatusData_ = DataManipulation.byteTransferExcludeHeader(tableSize, receivedPacket_);
                                globalZoneStatusData_.CopyTo(zoneStatus, 0);

                                // Refresh Zone Status Display Panel
                                Refresh_Zone_Status_Display(zoneStatus);
                            }
                        }

                        Thread.Sleep((int)timeToSleep);
                    }
                    catch (TimeOutException ex)
                    {
                        Program.WriteEventLog("[ERROR] [TimeOutException] [1] " + ex.Message, fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
                    }
                    catch (ThreadAbortException ex)
                    {
                        Program.WriteEventLog("[ERROR] [ThreadAbortException] [1] " + ex.Message, fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
                    }
                    catch (Exception ex)
                    {
                        Program.WriteEventLog("[ERROR] [Exception] [1] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
                    }
                }

                Program.WriteEventLog("[INFO] [Control] ZONE polling thread stopped!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
                pollZoneThread_.Abort();
            }
            catch (ThreadAbortException ex)
            {
                Program.WriteEventLog("[ERROR] [ThreadAbortException] [2] " + ex.Message, fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] [Exception] [2] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }
        }

        /// <summary>
        /// Method that get the status of all zones from the PA Agent
        /// </summary>
        private void Monitor_PollZoneThread()
        {
            // Poll PA Agent for Zone Status
        }


        /// <summary>
        /// Function to read PAS Table 303/1303, and update database
        /// </summary>
        /// <returns>bool: true if PA Manager is able to read from PA System, otherwise false</returns>
        private bool Read_PAS_Update_DB()
        {
            try
            {
                Program.WriteEventLog("[INFO] Reading and updating DVA details started.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

                // Read from PAS
                byte[] msgSeqDesc = UserFunctions.Read_DVA_Message_Sequence_Description();

                // Process data
                DvaMessageSequenceStruct[] dvaMsgSegArray = UserFunctions.Parse_DVA_Msg_Seq_Desc(msgSeqDesc);

                // Update database
                for (int msgSeqIndex = 0; msgSeqIndex < Program.MAXMSGSEQ; msgSeqIndex++)
                {
                    UserFunctions.Update_PAS_DVA_MSG_SEQ(dvaMsgSegArray[msgSeqIndex]);
                }

                Program.WriteDebugEventLog("[DEBUG] Reading and updating DVA details successfully ended.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
                return true;
            }
            catch (TableException ex)
            {
                Program.WriteEventLog("[ERROR] [TableException] Failed to update 303/1303. Exception: " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }

            return false;
        }

        #region Region: Broadcast Configuration

        /// <summary>
        /// Operator clicks on the clear one message button
        /// </summary>
        /// <param name="sender">Control that raised the event</param>
        /// <param name="e">Additional information about the event that was raised</param>
        private void button_clearone_msg_Click(object sender, EventArgs e)
        {
            textBox_selected_msgcontent.Text = string.Empty;
            setClearButton();
        }

        /// <summary>
        /// Method invoked when operator checks the Cyclical Broadcast
        /// </summary>
        /// <param name="sender">Control that raised the event</param>
        /// <param name="e">Additional information about the event that was raised</param>
        //private void checkBox_cyclicbroadcast_CheckedChanged(object sender, EventArgs e)
        //{
        //    if (checkBox_cyclicbroadcast.Checked)
        //    {
        //        numericUpDown_times.Enabled = true;
        //        numericUpDown_min.Enabled = true;
        //        numericUpDown_sec.Enabled = true;
        //    }
        //    else
        //    {
        //        numericUpDown_times.Enabled = false;
        //        numericUpDown_min.Enabled = false;
        //        numericUpDown_sec.Enabled = false;
        //    }
        //}

        #endregion

        #region Region: HMI Control Buttons
        /// <summary>
        /// Operator clicks on the "Configuration" button
        /// </summary>
        /// <param name="sender">Control that raised the event</param>
        /// <param name="e">Additional information about the event that was raised</param>
        private void button_configZone_Click(object sender, EventArgs e)
        {
            // Nothing to validate : sender, e
            ConfigPa configZonesForm = new ConfigPa();
            configZonesForm.ShowDialog();
        }

        /// <summary>
        /// Operator clicks on the "DVA Version" button, displays the DVA Version window
        /// </summary>
        /// <param name="sender">Control that raised the event</param>
        /// <param name="e">Additional information about the event that was raised</param>
        private void btn_DVA_Version_Click(object sender, EventArgs e)
        {
            // Nothing to validate : sender, e
            DvaVersion dvaDetailsForm = new DvaVersion();
            dvaDetailsForm.ShowDialog();
        }

        /// <summary>
        /// Operator clicks on the "DVA Status" button
        /// </summary>
        /// <param name="sender">Control that raised the event</param>
        /// <param name="e">Additional information about the event that was raised</param>
        private void button_DvaStatus_Click(object sender, EventArgs e)
        {
            // Nothing to validate : sender, e
            try
            {
                DvaStatus dvaStatusForm = new DvaStatus(localLocationId_, localConsoleId_, localLocationNo_, preRecordedMessageVersion_);
                dvaStatusForm.resumeBcCheck += resumeBcCheck;
                dvaStatusForm.pauseBcCheck += pauseBcCheck;
                dvaStatusForm.ShowDialog();
            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
                Program.MessageBox_Error(Program.MB_GeneralEx, sysErr_);
            }
        }

        /// <summary>
        /// Operator clicks on the "Execute" button
        /// </summary>
        /// <param name="sender">Control that raised the event</param>
        /// <param name="e">Additional information about the event that was raised</param>
        /// <exception cref="Exception">Handle generic exception</exception>
        private async void button_execute_Click(object sender, EventArgs e)
        {
            try
            {
                bool stationSelected = false;
                foreach (var item in stnNoToStnCbDic_)
                {
                    if (item.Value.Checked)
                    {
                        stationSelected = true;
                        break;
                    }
                }

                if (!stationSelected)
                {
                    Program.MessageBox_Normal(mbSelectStationMsg_, sysErr_, this);
                    return;
                }

                if ((dataGridView_selectedzone.RowCount == 0) && (dataGridView_OCC_selectedzone.RowCount == 0))
                {
                    Program.MessageBox_Normal(mbSelectZoneMsg_, sysErr_, this);
                    return;
                }

                if (string.IsNullOrEmpty(textBox_selected_msgcontent.Text))
                {
                    Program.MessageBox_Normal(mbSelectMessageMsg_, sysErr_, this);
                    return;
                }

                if (STEMessageBox.Show(cmfSendMsg_, sysErr_, Program.defaultLanguage, MsgBoxButton.YesNo, MessageBoxIcon.Question) == DialogResult.No)
                {
                    return;
                }

                byte playArea = 0;

                bool[] selectedZones = DataObject.Selected_Station_Zone;
                for (int i = 0; i < Program.MAXZONEID; i++)
                {
                    if (selectedZones[i])
                    {
                        ZoneStruct paZone = DataObject.zoneArray[i];

                        playArea |= paZone.zoneTypeIdBitValue;
                    }
                }

                byte recordLanguage = 0;
                if (checkBox_chinese.Checked)
                {
                    recordLanguage |= 1;
                }
                if (checkBox_taiwanese.Checked)
                {
                    recordLanguage |= 2;
                }
                if (checkBox_hakka.Checked)
                {
                    recordLanguage |= 4;
                }
                if (checkBox_english.Checked)
                {
                    recordLanguage |= 8;
                }

                byte seatId = Convert.ToByte(comboBox_seats.SelectedValue);
                PlayItem playItem = new PlayItem((ushort)selectedPreRecordedMsg_.MsgId, recordLanguage);
                byte playCount = Convert.ToByte(numericUpDown_times.Value);
                byte playInterval = Convert.ToByte(numericUpDown_sec.Value);

                List<StationPlay> stationPlayList = new List<StationPlay>();
                string stationsString = "";
                foreach (var item in stnNoToStnCbDic_)
                {
                    if (item.Value.Checked)
                    {
                        byte stationId = Convert.ToByte(item.Value.Tag);
                        StationPlay stationPlay = new StationPlay(stationId, playArea, new List<PlayItem> { playItem }, playCount, playInterval);
                        stationPlayList.Add(stationPlay);
                        stationsString += $"{stationId},";
                    }
                }
                stationsString = stationsString.TrimEnd(',');

                //disable main window
                this.Enabled = false;

                M44 cmd = new M44((byte)seatId, stationPlayList);

                bool result = await PaService.Instance.StartBroadcast(cmd);

                if (result)
                {
                    DvaMessageStatusStruct dvaStatus = new DvaMessageStatusStruct
                    {
                        LocationId = localLocationId_,
                        ConsoleId = localConsoleId_,
                        MsgId = selectedPreRecordedMsg_.MsgId,
                        MsgVersion = preRecordedMessageVersion_,
                        RecycleTime = playCount,
                        IntervalSec = playInterval,
                        Zones = playArea,
                        Status = (int)BroadcastStatusEnum.InProgress,
                        SeatId = seatId,
                        Language = recordLanguage,
                        Stations = stationsString
                    };

                    UserFunctions.InsertDvaMessageStatus(dvaStatus);
                    Program.MessageBox_Normal(mbSendMsgSuccessful_, sysErr_, this);
                }
                else
                {
                    Program.MessageBox_Error(mbSendMsgFailed_, sysErr_);
                }
            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }
            finally
            {
                //enable main window
                this.Enabled = true;
            }
        }

        private async void QueryMsgVersion()
        {
            try
            {
                await PaService.Instance.QueryMsgVersion();
            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }
        }

        /// <summary>
        /// Operator clicks on the Close button
        /// </summary>
        /// <param name="sender">Control that raised the event</param>
        /// <param name="e">Additional information about the event that was raised</param>
        private void button_close_Click(object sender, EventArgs e)
        {
            // Nothing to validate : sender, e
            Program.WriteEventLog("[CLOSING PROGRAM]", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            this.Close();
        }

        #endregion

        #region Mode: OCC
        /// <summary>
        /// Create the CheckBox for all stations
        /// </summary>
        private void initStnCheckbox()
        {
            Padding cbPadding = new Padding();
            cbPadding.All = 5;

            stn_flowLayoutPanel.Controls.Clear();
            stn_flowLayoutPanel.AutoScroll = true;

            foreach (KeyValuePair<int, string> keyValuePair in Program.stnNoToStnNameDic_)
            {
                CheckBox stnCheckBox = new CheckBox();

                stnCheckBox.Name = "checkBox_Stn_" + keyValuePair.Key;
                stnCheckBox.Text = keyValuePair.Value;
                stnCheckBox.Tag = keyValuePair.Key;

                stnCheckBox.Width = 150;
                stnCheckBox.Height = 40;
                stnCheckBox.TextAlign = ContentAlignment.MiddleRight;
                stnCheckBox.ForeColor = Color.White;
                stnCheckBox.Font = new Font(stnCheckBox.Font.FontFamily, 12, stnCheckBox.Font.Style | FontStyle.Bold);
                stnCheckBox.Appearance = Appearance.Button;
                stnCheckBox.FlatStyle = FlatStyle.Flat;
                stnCheckBox.FlatAppearance.BorderColor = Color.Gray;
                stnCheckBox.FlatAppearance.BorderSize = 2;
                stnCheckBox.Image = CbUncheckedImage_;
                stnCheckBox.ImageAlign = ContentAlignment.MiddleLeft;
                stnCheckBox.UseVisualStyleBackColor = false;
                stnCheckBox.Margin = cbPadding;

                // Set appearance of the checkbox to green
                checkBox_Green_Uncheck(stnCheckBox);

                stnCheckBox.CheckedChanged += new EventHandler(stnCheckBox_CheckedChanged);
                stnCheckBox.Click += new EventHandler(button_stn_Click);

                stnCheckBox.Visible = true;
                stn_flowLayoutPanel.Controls.Add(stnCheckBox);

                stnNoToStnCbDic_.Add(keyValuePair.Key, stnCheckBox);
            }
        }

        /// <summary>
        /// Method invoked when the state of the checkbox changes
        /// </summary>
        /// <param name="sender">Control that raised the event</param>
        /// <param name="e">Additional information about the event that was raised</param>
        void stnCheckBox_CheckedChanged(object sender, EventArgs e)
        {
            // Nothing to validate : sender, e
            CheckBox stnCheckBox = sender as CheckBox;
            checkBox_StnCheckedEffect(ref stnCheckBox);
        }

        /// <summary>
        /// Operator clicks on the Select all zones button
        /// </summary>
        /// <param name="sender">Control that raised the event</param>
        /// <param name="e">Additional information about the event that was raised</param>
        private void button_selectall_zone_Click(object sender, EventArgs e)
        {
            // Nothing to validate : sender, e
            CheckAllZoneCb(currSelStnNo_);
            selectAllZones(currSelStnNo_);

            // Check all the zone "group" checkbox
            foreach (KeyValuePair<int, CheckBox> grpCb in catIdToCbDic_)
            {
                grpCb.Value.Checked = true;
            }

            Refresh_ZonesDgv();
        }

        /// <summary>
        /// Operator clicks on the Unselect all zones button
        /// </summary>
        /// <param name="sender">Control that raised the event</param>
        /// <param name="e">Additional information about the event that was raised</param>
        private void button_unselectall_zone_Click(object sender, EventArgs e)
        {
            // Nothing to validate : sender, e
            unCheckAllZoneCb(currSelStnNo_);
            unselectAllZones(currSelStnNo_);

            // Uncheck all the zone "group" checkbox
            foreach (KeyValuePair<int, CheckBox> grpCb in catIdToCbDic_)
            {
                grpCb.Value.Checked = false;
            }

            Refresh_ZonesDgv();
        }

        /// <summary>
        /// Station mode: Method invoked when operator clicks on the Select All button
        /// </summary>
        /// <param name="stationNo">Station number where all zones are to be selected</param>
        private void selectAllZones(int stationNo)
        {
            // Nothing to validate : stationNo
            try
            {
                int stationIndex = stationNo - 1;

                for (int i = 0; i < Program.MAXZONEID; i++)
                {
                    if (Program.occMode)
                    {
                        int zoneIndex = i + (stationIndex * Program.MAXZONEID);

                        if (DataObject.stnArray[zoneIndex] != null)
                        {
                            Update_ZoneState_Global(stationNo, i, true);
                        }
                    }
                    else
                    {
                        if (DataObject.zoneArray[i] != null)
                        {
                            Update_ZoneState(i, true);
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }
        }

        /// <summary>
        /// Check all zones checkbox within selected stationNo
        /// </summary>
        /// <param name="stationNo">Selected station number</param>
        private void CheckAllZoneCb(int stationNo)
        {
            // Nothing to validate : stationNo
            int stationIndex = stationNo - 1;

            for (int i = 0; i < Program.MAXZONEID; i++)
            {
                if (Program.occMode)
                {
                    int zoneIndex = i + (stationIndex * Program.MAXZONEID);

                    if (DataObject.stnArray[zoneIndex] != null)
                    {
                        zoneCheckBoxArray_[i].Checked = true;
                    }
                }
                else
                {
                    if (DataObject.zoneArray[i] != null)
                    {
                        zoneCheckBoxArray_[i].Checked = true;
                    }
                }
            }
        }

        /// <summary>
        /// Check all zones checkbox within selected stationNo
        /// </summary>
        /// <param name="stationNo">Selected station number</param>
        private void unCheckAllZoneCb(int stationNo)
        {
            // Nothing to validate : stationNo
            int stationIndex = stationNo - 1;

            for (int i = 0; i < Program.MAXZONEID; i++)
            {
                if (Program.occMode)
                {
                    int zoneIndex = i + (stationIndex * Program.MAXZONEID);

                    if (DataObject.stnArray[zoneIndex] != null)
                    {
                        zoneCheckBoxArray_[i].Checked = false;
                    }
                }
                else
                {
                    if (DataObject.zoneArray[i] != null)
                    {
                        zoneCheckBoxArray_[i].Checked = false;
                    }
                }
            }
        }

        /// <summary>
        /// Station mode: Method invoked when operator clicks on the Unselect All button
        /// </summary>
        /// <param name="stationNo">Station number where all zones are to be unselected</param>
        private void unselectAllZones(int stationNo)
        {
            // Nothing to validate : stationNo
            try
            {
                int stationIndex = stationNo - 1;

                for (int i = 0; i < Program.MAXZONEID; i++)
                {
                    if (Program.occMode)
                    {
                        // OCC
                        int zoneIndex = i + (stationIndex * Program.MAXZONEID);

                        if (DataObject.stnArray[zoneIndex] != null)
                        {
                            Update_ZoneState_Global(stationNo, i, false);
                        }
                    }
                    else
                    {
                        // Station
                        if (DataObject.zoneArray[i] != null)
                        {
                            Update_ZoneState(i, false);
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }
        }

        /// <summary>
        /// OCC - Operator clicks on a Station to view the zones
        /// </summary>
        /// <param name="sender">Control that raised the event</param>
        /// <param name="e">Additional information about the event that was raised</param>
        private void button_stn_Click(object sender, EventArgs e)
        {
            // Nothing to validate : sender, e
        }

        /// <summary>
        /// OCC - Refresh check status of zones in the zone panel according to selected station
        /// </summary>
        private void occRefreshZoneCbDisplay()
        {
            int startOffset = (currSelStnNo_ - 1) * Program.MAXZONEID;
            bool[] selOccZone = DataObject.Selected_OCC_Zone;

            groupBox_zones.Text = Program.stnNoToStnNameDic_[currSelStnNo_] + lbZone_;

            // Codes to display checked zones - NOT done in refresh.
            for (int i = startOffset; i < startOffset + Program.MAXZONEID; i++)
            {
                if (selOccZone[i])
                {
                    zoneCheckBoxArray_[i - startOffset].Checked = true;
                }
                else
                {
                    zoneCheckBoxArray_[i - startOffset].Checked = false;
                }
            }

            unSelectAllGrpCheckBox();

            // Loop thru all category group checkbox
            foreach (KeyValuePair<int, string> catIdPair in catIdToNameDic_)
            {
                catIdToCbDic_[catIdPair.Key].Checked = allZoneGroupChecked(catIdPair.Value);
            }
        }

        /// <summary>
        /// Uncheck all the group checkboxes
        /// </summary>
        private void unSelectAllGrpCheckBox()
        {
            foreach (KeyValuePair<int, CheckBox> catCb in catIdToCbDic_)
            {
                catCb.Value.Checked = false;
            }
        }

        /// <summary>
        /// Show Zone Panel
        /// </summary>
        private void showZonePanel()
        {
            // Hide Station panel
            groupBox_station.Visible = true;

            // Show Zone panel
            groupBox_zones.Visible = true;
            ////Button_BackToStn.Visible = true;
        }

        /// <summary>
        /// OCC - Display selected zones in the data grid List
        /// </summary>
        private void DGV_Selected_Zones_OCC()
        {
            try
            {
                bool[] selectedOccZone_ = DataObject.Selected_OCC_Zone;

                dgvRowClear(dataGridView_OCC_selectedzone);

                for (int stnIndex = 0; stnIndex < Program.MAXSTNID; stnIndex++)
                {
                    for (int zoneIndex = 0; zoneIndex < Program.MAXZONEID; zoneIndex++)
                    {
                        int stnArrayOffset = (stnIndex * Program.MAXZONEID) + zoneIndex;

                        if (selectedOccZone_[stnArrayOffset])
                        {
                            ZoneStruct paZone = DataObject.stnArray[stnArrayOffset];

                            // Station Name, Zone, Type, Index
                            string[] paZoneInfo = { paZone.stnName, paZone.zoneName, paZone.zoneType, stnArrayOffset.ToString() };
                            dataGridView_OCC_selectedzone.Rows.Add(paZoneInfo);
                        }
                    }
                }

                setExecuteButton();
            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }
        }
        #endregion

        #region Mode: Station
        /// <summary>
        /// Create the checkbox for all zones
        /// </summary>
        private void initZoneCheckbox()
        {
            Padding cbPadding = new Padding();

            // Create the maximum allowed ZONE checkbox
            zone_flowLayoutPanel.Controls.Clear();

            for (int i = 0; i < Program.MAXZONEID; i++)
            {
                if (!Program.occMode)
                {
                    // stn mode - create required amount of checkbox
                    if (DataObject.zoneArray[i] == null)
                    {
                        continue;
                    }
                }

                CheckBox zoneCheckBox = new CheckBox();
                zoneCheckBox.Name = "checkBox_Zone" + (i + 1);
                zoneCheckBox.Text = "Zone " + (i + 1);
                zoneCheckBox.Tag = (i + 1);
                zoneCheckBox.TextAlign = ContentAlignment.MiddleRight;
                zoneCheckBox.Width = 178;
                zoneCheckBox.Height = 40;
                zoneCheckBox.ForeColor = Color.White;
                zoneCheckBox.Font = new Font(zoneCheckBox.Font.FontFamily, 12, zoneCheckBox.Font.Style | FontStyle.Bold);
                zoneCheckBox.Appearance = Appearance.Button;
                zoneCheckBox.FlatStyle = FlatStyle.Flat;
                zoneCheckBox.FlatAppearance.BorderColor = Color.Gray;
                zoneCheckBox.FlatAppearance.BorderSize = 2;
                zoneCheckBox.Image = CbUncheckedImage_;
                zoneCheckBox.ImageAlign = ContentAlignment.MiddleLeft;
                zoneCheckBox.UseVisualStyleBackColor = false;
                cbPadding.All = 1;
                zoneCheckBox.Margin = cbPadding;

                // Set appearance of the checkbox to green
                // checkBox_Green_Uncheck(zoneCheckBox);
                checkBox_Red(zoneCheckBox);

                zoneCheckBox.CheckedChanged += new EventHandler(zoneCheckBox_CheckedChanged);
                zoneCheckBox.Click += new EventHandler(zoneCheckBox_Click);
                zone_flowLayoutPanel.Controls.Add(zoneCheckBox);

                zoneCheckBoxArray_[i] = zoneCheckBox;
            }
        }

        /// <summary>
        /// Method invoked when the state of the checkbox changes
        /// </summary>
        /// <param name="sender">Control that raised the event</param>
        /// <param name="e">Additional information about the event that was raised</param>
        void zoneCheckBox_CheckedChanged(object sender, EventArgs e)
        {
            // Nothing to validate : sender, e
            CheckBox zoneCheckBox = sender as CheckBox;
            checkBox_ZoneCheckedEffect(ref zoneCheckBox);
        }

        /// <summary>
        /// Operator clicks on the zone checkbox
        /// </summary>
        /// <param name="sender">Control that raised the event</param>
        /// <param name="e">Additional information about the event that was raised</param>
        void zoneCheckBox_Click(object sender, EventArgs e)
        {
            // Nothing to validate : sender, e
            CheckBox zoneCheckBox = sender as CheckBox;
            checkBoxZoneCheckedChanged(Convert.ToInt32(zoneCheckBox.Tag));

            // Unselect the groupbox related to this zone
            if (!zoneCheckBox.Checked)
            {
                if (Program.occMode)
                {
                    int stnArrayOffset = ((currSelStnNo_ - 1) * Program.MAXZONEID) + Convert.ToInt32(zoneCheckBox.Tag) - 1;

                    string categoryName = DataObject.stnArray[stnArrayOffset].zoneCategory;

                    foreach (KeyValuePair<int, string> grpCatName in catIdToNameDic_)
                    {
                        if (grpCatName.Value == categoryName)
                        {
                            catIdToCbDic_[grpCatName.Key].Checked = false;
                        }
                    }
                }
                else
                {
                    // new - coded on 9 sept 2011
                    string categoryName = DataObject.zoneArray[Convert.ToInt32(zoneCheckBox.Tag) - 1].zoneCategory;

                    foreach (KeyValuePair<int, string> grpCatName in catIdToNameDic_)
                    {
                        if (grpCatName.Value == categoryName)
                        {
                            catIdToCbDic_[grpCatName.Key].Checked = false;
                        }
                    }
                }
            }

            Refresh_ZonesDgv();
        }

        /// <summary>
        /// Create the checkbox for all zonez group
        /// </summary>
        private void initZoneGrpCheckbox()
        {
            ZoneGrp_flowLayoutPanel.Controls.Clear();

            // Only create the required Zone Group checkbox
            foreach (KeyValuePair<int, string> catName in catIdToNameDic_)
            {
                CheckBox zoneGrpCheckBox = new CheckBox();
                zoneGrpCheckBox.Name = "ZoneGrp_" + catName.Key;
                zoneGrpCheckBox.Text = lbSelect_ + catName.Value + "  ";
                zoneGrpCheckBox.Tag = catName.Key;

                zoneGrpCheckBox.Visible = true;
                zoneGrpCheckBox.TextAlign = ContentAlignment.MiddleRight;
                zoneGrpCheckBox.Width = 180;
                zoneGrpCheckBox.Height = 40;
                zoneGrpCheckBox.Appearance = Appearance.Button;
                zoneGrpCheckBox.FlatStyle = FlatStyle.System;
                zoneGrpCheckBox.Click += new EventHandler(zoneGrpCheckBox_Click);

                ZoneGrp_flowLayoutPanel.Controls.Add(zoneGrpCheckBox);

                catIdToCbDic_.Add(catName.Key, zoneGrpCheckBox);
            }

        }

        /// <summary>
        /// Operator clicks on the Zone Group Checkbox, to select all related Checkbox
        /// </summary>
        /// <param name="sender">Control that raised the event</param>
        /// <param name="e">Additional information about the event that was raised</param>
        private void zoneGrpCheckBox_Click(object sender, EventArgs e)
        {
            // Nothing to validate : sender, e
            try
            {
                CheckBox selGrpCheckBox = sender as CheckBox;

                if (Program.occMode)
                {
                    int stnZoneOffset = (currSelStnNo_ - 1) * Program.MAXZONEID;

                    // Select / unselect checkbox according to selected group
                    for (int zIndex = 0; zIndex < Program.MAXZONEID; zIndex++)
                    {
                        // Set zone check status as according to the status of the Zone Group Checkbox 
                        if (DataObject.stnArray[stnZoneOffset + zIndex] != null && DataObject.stnArray[stnZoneOffset + zIndex].zoneCategory == catIdToNameDic_[int.Parse(selGrpCheckBox.Tag.ToString())])
                        {
                            zoneCheckBoxArray_[zIndex].Checked = selGrpCheckBox.Checked;

                            Update_ZoneState_Global(currSelStnNo_, zIndex, selGrpCheckBox.Checked);
                        }
                    }
                }
                else
                {
                    //loop thru all zones to check their label, check if match with checkbox_zone_grp
                    for (int zIndex = 0; zIndex < Program.MAXZONEID; zIndex++)
                    {
                        //Set zone check status as according to the status of the Zone Group Checkbox 
                        if (DataObject.zoneArray[zIndex] != null && DataObject.zoneArray[zIndex].zoneCategory == catIdToNameDic_[int.Parse(selGrpCheckBox.Tag.ToString())])
                        {
                            zoneCheckBoxArray_[zIndex].Checked = selGrpCheckBox.Checked;

                            Update_ZoneState(zIndex, selGrpCheckBox.Checked);
                        }
                    }

                }

                Refresh_ZonesDgv();
            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }
        }

        /// <summary>
        /// Zone - Method invoked when the checkbox check state are changed (zoneNo: 1-based index)
        /// </summary>
        /// <param name="zoneNo">Zone number that was changed</param>
        private void checkBoxZoneCheckedChanged(int zoneNo)
        {
            // Nothing to validate : zoneNo
            try
            {
                int zoneIndex = zoneNo - 1;

                if (Program.occMode)
                {
                    if (zoneCheckBoxArray_[zoneIndex].Checked)
                    {
                        Update_ZoneState_Global(currSelStnNo_, zoneIndex, true);
                    }
                    else if (!zoneCheckBoxArray_[zoneIndex].Checked)
                    {
                        Update_ZoneState_Global(currSelStnNo_, zoneIndex, false);
                    }
                    else
                    {
                        // Do nothing
                    }
                }
                else
                {
                    // Check if the checkbox is checked or not
                    if (zoneCheckBoxArray_[zoneIndex].Checked)
                    {
                        Update_ZoneState(zoneIndex, true);
                    }
                    else
                    {
                        Update_ZoneState(zoneIndex, false);
                    }
                }
            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }
        }

        /// <summary>
        /// Occ - Method invoked when operator press on the Select All station button
        /// </summary>
        private void selectAllStn()
        {
            foreach (KeyValuePair<int, CheckBox> stnCb in stnNoToStnCbDic_)
            {
                // Check station checkbox
                stnCb.Value.Checked = true;

                // Select all zones within station
                selectAllZones(stnCb.Key);
            }

            Refresh_ZonesDgv();
        }

        /// <summary>
        /// Occ - Method invoked when operator press on the unselect all button
        /// </summary>
        private void unselectAllStn()
        {
            foreach (KeyValuePair<int, CheckBox> stnCb in stnNoToStnCbDic_)
            {
                stnCb.Value.Checked = false;

                unselectAllZones(stnCb.Key);
            }

            Refresh_ZonesDgv();
        }

        /// <summary>
        /// Operator clicks on select all stations button
        /// </summary>
        /// <param name="sender">Control that raised the event</param>
        /// <param name="e">Additional information about the event that was raised</param>
        private void button_selectall_stn_Click(object sender, EventArgs e)
        {
            // Nothing to validate : sender, e
            selectAllStn();
        }

        /// <summary>
        /// Operator clicks on unselect all stations button
        /// </summary>
        /// <param name="sender">Control that raised the event</param>
        /// <param name="e">Additional information about the event that was raised</param>
        private void button_unselectall_stn_Click(object sender, EventArgs e)
        {
            // Nothing to validate : sender, e
            unselectAllStn();
        }

        /// <summary>
        /// Occ - Back button to go back to the Station layout view, after operator has finished selecting the zones. 
        /// </summary>
        /// <param name="sender">Control that raised the event</param>
        /// <param name="e">Additional information about the event that was raised</param>
        private void Button_BackToStn_Click(object sender, EventArgs e)
        {
            // Nothing to validate : sender, e

            // Check if there is any selected zones, if yes, check it, otherwise uncheck it
            if (gotSelZones())
            {
                stnNoToStnCbDic_[currSelStnNo_].Checked = true;
            }
            else
            {
                stnNoToStnCbDic_[currSelStnNo_].Checked = false;
            }

            // Hide Stations, and show Zones
            groupBox_zones.Visible = false;
            groupBox_station.Visible = true;

            Refresh_Zone_Status_Display_OccLevel(zoneAmpStatus_);
        }

        /// <summary>
        /// Check if there is any zones selected for broadcast
        /// </summary>
        /// <returns>bool: true if there is zones selected for broadcast, otherwise false</returns>
        private bool gotSelZones()
        {
            if (Program.occMode)
            {
                int startOffset = (currSelStnNo_ - 1) * Program.MAXZONEID;

                // Codes to display checked zones - NOT done in refresh.
                for (int i = startOffset; i < startOffset + Program.MAXZONEID; i++)
                {
                    if (DataObject.Selected_OCC_Zone[i])
                    {
                        return true;
                    }
                }
            }
            return false;
        }

        /// <summary>
        /// Display selected zones in the zone list listboxes
        /// </summary>
        private void DGV_Selected_Zones()
        {
            try
            {
                bool[] selectedZones = DataObject.Selected_Station_Zone;

                dgvRowClear(dataGridView_selectedzone);

                // Populate the dataGridView_selectedzone
                for (int h = 0; h < Program.MAXZONEID; h++)
                {
                    if (selectedZones[h])
                    {
                        ZoneStruct paZone = DataObject.zoneArray[h];

                        // Zone, Area, Index
                        string[] paZoneInfo = { paZone.zoneName, paZone.zoneType, h.ToString() };
                        dataGridView_selectedzone.Rows.Add(paZoneInfo);
                    }
                }

                setExecuteButton();
            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }
        }

        #endregion

        #region Both OCC and Station
        /// <summary>
        /// OCC - Update the Zone Status display according to the selected station
        /// </summary>
        private void Update_ZonePanel_Cb_Display()
        {
            int tableSize = Program.MAXZONEID * 2;
            byte[] extractedZoneStatus = new byte[tableSize];

            if (zoneAmpStatus_ != null)
            {
                if (Program.occMode)
                {
                    // Extract Relevant Station -> Zone data
                    int stnOffset = (currSelStnNo_ - 1) * Program.MAXZONEID * 2;

                    // Loop thru the desired range in the zoneStatusData Array for the selected zones list
                    for (int i = stnOffset; i < stnOffset + Program.MAXZONEID * 2; i++)
                    {
                        extractedZoneStatus[i - stnOffset] = zoneAmpStatus_[i];
                    }
                }
                else
                {
                    // No need to extract data
                    extractedZoneStatus = zoneAmpStatus_;
                }

                Refresh_Zone_Status_Display(extractedZoneStatus);
            }
        }

        /// <summary>
        /// Refresh the Zone buttons' Amp with color coded status, and display relevant buttons only
        /// </summary>
        /// <param name="receivedData">byte array containing the zone status array data. (excludes 5 bytes for packet header)</param>
        private void Refresh_Zone_Status_Display(byte[] receivedData)
        {
            // Note: Each zone location takes 2 element size
            try
            {
                if (receivedData == null)
                {
                    Program.WriteEventLog("[WARN] receivedData is EMPTY!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
                    return;
                }

                if (Program.occMode)
                {
                    #region Occ
                    int stnArrayOffset = (currSelStnNo_ - 1) * Program.MAXZONEID;
                    byte[] selStationZoneStatus = new byte[Program.MAXZONEID * 2];

                    // Loop thru the desired range for the selected zones list
                    // e.g. 1100 0101 = 1010 0011, array is checked backwards
                    for (int i = 0; i < Program.MAXZONEID; i++)
                    {
                        // Connection status of Zones
                        int offSet = ((i + 1) * 2) - 1;

                        if (receivedData[offSet] >= 128)
                        {
                            // Zone Amplifier is down
                            checkBox_Red(zoneCheckBoxArray_[i]);
                        }
                        else
                        {
                            // Zone Amplifier is up
                            if (zoneCheckBoxArray_[i].Checked)
                            {
                                checkBox_Green_Check(zoneCheckBoxArray_[i]);
                            }
                            else
                            {
                                checkBox_Green_Uncheck(zoneCheckBoxArray_[i]);
                            }
                        }

                        // Hide unused station zone checkbox
                        if (DataObject.stnArray[stnArrayOffset + i] == null)
                        {
                            zoneCheckBoxArray_[i].Text = "HIDE";
                            zoneCheckBoxArray_[i].Hide();
                        }
                        else
                        {
                            zoneCheckBoxArray_[i].Text = DataObject.stnArray[(currSelStnNo_ - 1) * Program.MAXZONEID + i].zoneName;
                            zoneCheckBoxArray_[i].Show();
                        }
                    }

                    #endregion
                }
                else
                {
                    #region station
                    for (int zoneIndex = 0; zoneIndex < Program.MAXZONEID; zoneIndex++)
                    {
                        // Hide unused checkbox
                        if (DataObject.zoneArray[zoneIndex] == null)
                        {
                            // No unused checkbox are created for STN
                            //zoneCheckBoxArray_[zoneIndex].Text = "HIDE";
                            //zoneCheckBoxArray_[zoneIndex].Hide();
                        }
                        else
                        {
                            int zoneStatus = receivedData[(zoneIndex + 1) * 2 - 1];

                            // Check status of zone amplifier 
                            if (zoneStatus >= 128)
                            {
                                checkBox_Red(zoneCheckBoxArray_[zoneIndex]);
                            }
                            else
                            {
                                if (zoneCheckBoxArray_[zoneIndex].Checked)
                                {
                                    checkBox_Green_Check(zoneCheckBoxArray_[zoneIndex]);
                                }
                                else
                                {
                                    checkBox_Green_Uncheck(zoneCheckBoxArray_[zoneIndex]);
                                }
                            }

                            // zone_number to show the number of zones for each zone type Platform, concourse
                            zoneCheckBoxArray_[zoneIndex].Text = DataObject.zoneArray[zoneIndex].zoneName;
                        }
                    }
                    #endregion
                }
            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }
        }

        /// <summary>
        /// OCC - 1200 - Update the Station Buttons with color coded status
        /// </summary>
        /// <param name="globalZoneStatus">byte array containing the global zone array data. (excludes 5 bytes for packet header)</param>
        private void Refresh_Zone_Status_Display_OccLevel(byte[] globalZoneStatus)
        {
            if (globalZoneStatus == null)
            {
                return;
            }

            try
            {
                bool[] stnStatus = new bool[Program.MAXSTNID];

                // Set all stations to connected status
                for (int stnIndex = 0; stnIndex < Program.MAXSTNID; stnIndex++)
                {
                    stnStatus[stnIndex] = true;
                }

                // Loop thru all stations one by one
                for (int stnIndex = 0; stnIndex < Program.MAXSTNID; stnIndex++)
                {
                    // Loop thru the zones in the station
                    for (int zoneIndex = 0; zoneIndex < Program.MAXZONEID; zoneIndex++)
                    {
                        int zoneIndexOffset = ((zoneIndex + 1) * 2 - 1) + (stnIndex * Program.MAXZONEID * 2);

                        int zoneStatus = globalZoneStatus[zoneIndexOffset]; //zoneStatusData[zoneIndexOffset]; 

                        // Check status of zone amplifier, set station to false once a failed zone is encountered
                        if (zoneStatus >= 128)
                        {
                            stnStatus[stnIndex] = false;
                            break;
                        }
                    }
                }

                // Update status of the Station buttons
                foreach (KeyValuePair<int, CheckBox> stnCb in stnNoToStnCbDic_)
                {
                    if (stnStatus[stnCb.Key - 1])
                    {
                        // Amp is up - Green
                        if (stnCb.Value.Checked)
                        {
                            checkBox_Green_Check(stnCb.Value);
                        }
                        else
                        {
                            checkBox_Green_Uncheck(stnCb.Value);
                        }

                    }
                    else
                    {
                        // Amp is down - Red
                        checkBox_Red(stnCb.Value);
                    }
                }

            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }

        }
        #endregion

        #region General Functions
        /// <summary>
        /// OCC - Update the selected Station - Zone state
        /// </summary>
        /// <param name="stnNo">Station number (1-based)</param>
        /// <param name="zoneIndex">Zone index (0-based)</param>
        /// <param name="zoneFlag">Zone state</param>
        private void Update_ZoneState_Global(int stnNo, int zoneIndex, bool zoneFlag)
        {
            // Nothing to validate : stnNo, zoneIndex, zoneFlag
            DataObject.Selected_OCC_Zone[((stnNo - 1) * Program.MAXZONEID) + zoneIndex] = zoneFlag;
        }

        /// <summary>
        /// Update the state of a particular zone. (0-based index)
        /// </summary>
        /// <param name="zoneIndex">Zone index (0-based)</param>
        /// <param name="zoneFlag">State to be set</param>
        private void Update_ZoneState(int zoneIndex, bool zoneFlag)
        {
            // Nothing to validate : zoneIndex, zoneFlag
            DataObject.Selected_Station_Zone[zoneIndex] = zoneFlag;
        }

        /// <summary>
        /// Store into DataObject the latest DVA Message Information
        /// </summary>
        /// <param name="availableDvaMsg">byte array containing the DVA Message Information</param>
        private void Store_Into_DVA_Message_DataObject(byte[] availableDvaMsg)
        {
            // Nothing to validate : availableDvaMsg
            int labelLength = 40;
            int elementSize = 41;

            // 494 messages in total
            for (int msgIndex = Program.NBADHOC; msgIndex < Program.MAXDVAMSG; msgIndex++)
            {
                DvaMessageStruct dvaMessage = new DvaMessageStruct();

                byte[] DvaMsgArray = new byte[labelLength];

                // starting byte of the DVA label
                int labelArrayHead = (msgIndex - Program.NBADHOC) * elementSize;

                // The limit of the last byte of the DVA label
                int labelArrayTail = labelArrayHead + labelLength;
                int msgLength = 0;

                // Read out required label bytes
                for (int i = labelArrayHead; i < labelArrayTail; i++)
                {
                    if (availableDvaMsg[i] == 0x0)
                    {
                        break;
                    }
                    else
                    {
                        DvaMsgArray[i - labelArrayHead] = availableDvaMsg[i];
                        msgLength++;
                    }
                }

                // Convert label in Byte[] format to String format
                dvaMessage.msgLabel = System.Text.ASCIIEncoding.ASCII.GetString(DvaMsgArray, 0, msgLength);
                dvaMessage.msgType = (EnumType.MessageType)Convert.ToInt32(availableDvaMsg[labelArrayTail]);

                // offset required for stn messages (Set B), otherwise OCC (Set A) and Depot (Set D) do not require offset
                if (Program.occMode || Program.depotMode)
                {
                    dvaMessage.msgNo = msgIndex + 1;
                }
                else
                {
                    dvaMessage.msgNo = msgIndex + 1 + (Program.MAXDVAMSG);
                }

                DataObject.dvaMessages[msgIndex] = dvaMessage;
            }
        }

        #endregion

        #region CheckBox Effect
        /// <summary>
        /// Method to provide the effect on the Station checkbox
        /// </summary>
        /// <param name="checkboxRef">Reference to the checkbox</param>
        private void checkBox_StnCheckedEffect(ref CheckBox checkboxRef)
        {
            // Nothing to validate : checkboxRef

            if (checkboxRef.Checked)
            {
                // White Border + Light Green
                checkboxRef.FlatAppearance.BorderColor = Color.White;
                checkboxRef.Image = CbCheckedImage_;
                //checkBox_Green_Check(checkboxRef);
            }
            else
            {
                // Gray Border + Normal Green
                checkboxRef.FlatAppearance.BorderColor = Color.Gray;
                checkboxRef.Image = CbUncheckedImage_;
                //checkBox_Green_Uncheck(checkboxRef);
            }

        }

        /// <summary>
        /// Method to provide the effect on the Zone checkbox
        /// </summary>
        /// <param name="checkboxRef">Reference to the checkbox</param>
        private void checkBox_ZoneCheckedEffect(ref CheckBox checkboxRef)
        {
            // Nothing to validate : checkboxRef
            if (checkboxRef.Checked)
            {
                // White Border + Light Green + tick
                checkboxRef.FlatAppearance.BorderColor = Color.White;
                checkboxRef.Image = CbCheckedImage_;
            }
            else
            {
                // Gray Border + Normal Green + no tick
                checkboxRef.FlatAppearance.BorderColor = Color.Gray;
                checkboxRef.Image = CbUncheckedImage_;
            }

            //if (checkboxRef.BackColor != Color.Red)
            //{
            //    if (checkboxRef.Checked)
            //    {
            //        checkBox_Green_Check(checkboxRef);
            //    }
            //    else
            //    {
            //        checkBox_Green_Uncheck(checkboxRef);
            //    }
            //}

        }

        /// <summary>
        /// Set the appearance of the checkbox to red.
        /// </summary>
        /// <param name="checkBoxRef">Reference to the checkbox</param>
        private void checkBox_Red(CheckBox checkBoxRef)
        {
            // Nothing to validate : checkBoxRef
            checkBoxRef.BackColor = Color.Red;
            checkBoxRef.FlatAppearance.CheckedBackColor = Color.Red;
            checkBoxRef.FlatAppearance.MouseDownBackColor = Color.Red;
            checkBoxRef.FlatAppearance.MouseOverBackColor = Color.Red;
        }


        /// <summary>
        /// Set the appearance of the checkbox to yellow.
        /// </summary>
        /// <param name="checkBoxRef">Reference to the checkbox</param>
        private void checkBox_Yellow(CheckBox checkBoxRef)
        {
            // Nothing to validate : checkBoxRef
            checkBoxRef.BackColor = Color.Yellow;
            checkBoxRef.FlatAppearance.CheckedBackColor = Color.Yellow;
            checkBoxRef.FlatAppearance.MouseDownBackColor = Color.Yellow;
            checkBoxRef.FlatAppearance.MouseOverBackColor = Color.Yellow;
        }

        /// <summary>
        /// Set the uncheck appearance of the checkbox to "Normal" green.
        /// </summary>
        /// <param name="checkBoxRef">Reference to the checkbox</param>
        private void checkBox_Green_Uncheck(CheckBox checkBoxRef)
        {
            // Nothing to validate : checkBoxRef
            checkBoxRef.BackColor = Color.Green;
            checkBoxRef.FlatAppearance.CheckedBackColor = Color.Green;
            checkBoxRef.FlatAppearance.MouseDownBackColor = Color.Green;
            checkBoxRef.FlatAppearance.MouseOverBackColor = Color.Green;
        }
        /// <summary>
        /// Set the checked appearance of the checkbox to "Light" green.
        /// </summary>
        /// <param name="checkBoxRef">Reference to the checkbox</param>
        private void checkBox_Green_Check(CheckBox checkBoxRef)
        {
            // Nothing to validate : checkBoxRef
            checkBoxRef.BackColor = Color.Green;
            checkBoxRef.FlatAppearance.CheckedBackColor = Color.Green;
            checkBoxRef.FlatAppearance.MouseDownBackColor = Color.Green;
            checkBoxRef.FlatAppearance.MouseOverBackColor = Color.Green;
        }
        #endregion

        /// <summary>
        /// Convert Type ID to Name
        /// </summary>
        /// <param name="typeId">Type ID</param>
        /// <returns>Type Name</returns>
        private string zoneTypeID_to_Name(int typeId)
        {
            // Nothing to validate : typeId
            return typeIdToNameDic_[typeId];
        }


        #region Database Loading
        /// <summary>
        /// Load names of Station that has zones configured for it in the Database
        /// </summary>
        private void Load_StnNames()
        {
            dbConnEnum dbConn = dbConnEnum.ErrNoConn;
            //// Read Station Names of all stations
            localSql_ = @" SELECT DISTINCT PC.STATION_NO, SL.NAME, SL.ORDER_ID FROM PA_CONFIG PC, LOCATION SL 
            WHERE PC.LOCATION_ID=SL.PKEY AND SL.NAME NOT IN ('NDOCC', 'TYOCC', 'OVERALL') ORDER BY SL.ORDER_ID ";
            var queries = new List<SqlQuery> { new SqlQuery { CommandText = localSql_ } };
            DataSet ds = Program.dbLock.FetchData(queries, ref dbConn);

            if (ds.Tables != null || ds.Tables.Count > 0)
            {
                foreach (DataRow retrievedRow in ds.Tables[0].Rows)
                {
                    int stationNo = int.Parse(retrievedRow[0].ToString());
                    string locationNo = retrievedRow[1].ToString();
                    if (!Program.stnNoToStnNameDic_.ContainsKey(stationNo))
                    {
                        Program.stnNoToStnNameDic_.Add(stationNo, locationNo);
                    }
                }
            }
        }

        /// <summary>
        /// Load PA Zone Category From Databasae
        /// </summary>
        /// <exception cref="NoOracleDBException"></exception>
        /// <exception cref="Exception"></exception>
        private void Load_ZoneCategory()
        {
            dbConnEnum dbConn = dbConnEnum.ErrNoConn;

            try
            {
                string[] zoneCatLabels = new string[10];

                // Load the zone labels type (Platform, concourse..etc. Maximum of 10 labels allowed)
                if (Program.defaultLanguage == "en-US")
                {
                    localSql_ = " SELECT ZONE_CAT_ID, ZONE_CAT_NAME FROM PA_ZONE_CATEGORY ORDER BY ZONE_CAT_ID ";
                }
                else
                {
                    localSql_ = " SELECT ZONE_CAT_ID, ZONE_CAT_NAME_LOCAL FROM PA_ZONE_CATEGORY ORDER BY ZONE_CAT_ID ";
                }

                var queries = new List<SqlQuery> { new SqlQuery { CommandText = localSql_ } };
                DataSet ds = Program.dbLock.FetchData(queries, ref dbConn);

                if (ds.Tables != null || ds.Tables.Count > 0)
                {
                    foreach (DataRow retrievedRow in ds.Tables[0].Rows)
                    {
                        int zoneCatIndex = int.Parse(retrievedRow[0].ToString());

                        if (zoneCatIndex >= 1 && zoneCatIndex < 10)
                        {
                            zoneCatLabels[zoneCatIndex] = retrievedRow[1].ToString();

                            catIdToNameDic_.Add(zoneCatIndex, retrievedRow[1].ToString());
                        }
                    }
                }

            }
            catch (NoOracleDBException ex)
            {
                Program.WriteEventLog("[ERROR] [NoOracleDBException] " + Program.MB_NoDBConn + " Details: " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
                throw;
            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
                throw;
            }
        }

        /// <summary>
        /// Load PA Zone Type From Databasae
        /// </summary>
        /// <exception cref="NoOracleDBException"></exception>
        /// <exception cref="Exception"></exception>
        private void Load_ZoneType()
        {
            dbConnEnum dbConn = dbConnEnum.ErrNoConn;
            try
            {
                // Load the zone type definition (Public, non-public)
                if (Program.defaultLanguage == "en-US")
                {
                    localSql_ = " SELECT ZONE_TYPE_ID, ZONE_TYPE FROM PA_ZONE_TYPE ORDER BY ZONE_TYPE_ID ";
                }
                else
                {
                    localSql_ = " SELECT ZONE_TYPE_ID, ZONE_TYPE_LOCAL FROM PA_ZONE_TYPE ORDER BY ZONE_TYPE_ID ";
                }

                var queries = new List<SqlQuery> { new SqlQuery { CommandText = localSql_ } };
                DataSet ds = Program.dbLock.FetchData(queries, ref dbConn);

                if (ds.Tables != null || ds.Tables.Count > 0)
                {
                    foreach (DataRow retrievedRow in ds.Tables[0].Rows)
                    {
                        int zoneTypeNo = int.Parse(retrievedRow[0].ToString());

                        if (zoneTypeNo >= 1 && zoneTypeNo < 10)
                        {
                            typeIdToNameDic_.Add(zoneTypeNo, retrievedRow[1].ToString());
                        }
                    }
                }

            }
            catch (NoOracleDBException ex)
            {
                Program.WriteEventLog("[ERROR] [NoOracleDBException] " + Program.MB_NoDBConn + " Details: " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
                throw;
            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
                throw;
            }
        }

        /// <summary>
        /// Station Mode: Load Information of all zones From Databasae
        /// </summary>
        private void Load_ZoneInfo_StnMode()
        {
            // Station mode
            dbConnEnum dbConn = dbConnEnum.ErrNoConn;
            DataObject.zoneArray = new ZoneStruct[Program.MAXZONEID];

            localSql_ = " SELECT ZONE_NO, ZONE_CAT_ID, ZONE_TYPE_ID, SL.NAME AS LOCATION_NO FROM PA_ZONES PZ, LOCATION SL WHERE PZ.LOCATION_ID=SL.PKEY AND SL.PKEY =  " + Program.profileLocIndex;

            var queries = new List<SqlQuery> { new SqlQuery { CommandText = localSql_ } };
            DataSet ds = Program.dbLock.FetchData(queries, ref dbConn);

            if (ds.Tables != null || ds.Tables.Count > 0)
            {
                foreach (DataRow retrievedRow in ds.Tables[0].Rows)
                {
                    ZoneStruct paZone = new ZoneStruct();

                    int zoneNo = int.Parse(retrievedRow[0].ToString());
                    paZone.stnName = retrievedRow[3].ToString();
                    paZone.zoneCategory = catIdToNameDic_[int.Parse(retrievedRow[1].ToString())];
                    paZone.zoneType = zoneTypeID_to_Name(int.Parse(retrievedRow[2].ToString()));
                    ////paZone.zoneName = paZone.zoneCategory + zoneNo;
                    paZone.zoneName = paZone.zoneCategory;
                    paZone.zoneTypeId = int.Parse(retrievedRow[2].ToString());

                    DataObject.zoneArray[zoneNo - 1] = paZone;
                }
            }
        }

        /// <summary>
        /// OCC Mode: Load Information of all Stations, followed by Zones From Databasae
        /// </summary>
        private void Load_ZoneInfo_OccMode()
        {
            // Load the zone config id of each zone (Each zone is defined with a id associated with its
            // label (Platform, concourse....etc) and its zone type (Public, non-public)
            dbConnEnum dbConn = dbConnEnum.ErrNoConn;
            localSql_ = " SELECT PC.STATION_NO, ZONE_NO, ZONE_CAT_ID, ZONE_TYPE_ID FROM PA_CONFIG PC, PA_ZONES PZ WHERE PC.LOCATION_ID=PZ.LOCATION_ID ORDER BY PC.STATION_NO, ZONE_NO ";

            var queries = new List<SqlQuery> { new SqlQuery { CommandText = localSql_ } };
            DataSet ds = Program.dbLock.FetchData(queries, ref dbConn);

            if (ds.Tables != null || ds.Tables.Count > 0)
            {
                foreach (DataRow retrievedRow in ds.Tables[0].Rows)
                {
                    ZoneStruct paZone = new ZoneStruct();

                    // stationNo is 1-based
                    int stationNo = int.Parse(retrievedRow[0].ToString());
                    int stnIndex = stationNo - 1;

                    // Zone Details
                    int zoneNo = int.Parse(retrievedRow[1].ToString());
                    int zoneIndex = zoneNo - 1;

                    int stnArrayOffset = (stnIndex * Program.MAXZONEID) + zoneIndex;

                    // Note: Do not change the sequence of the following 5 lines of codes
                    paZone.stnName = Program.stnNoToStnNameDic_[stationNo];
                    paZone.zoneCategory = catIdToNameDic_[int.Parse(retrievedRow[2].ToString())];
                    ////paZone.zoneName = paZone.zoneCategory + zoneNo;
                    paZone.zoneName = paZone.zoneCategory;
                    paZone.zoneType = zoneTypeID_to_Name(int.Parse(retrievedRow[3].ToString()));
                    paZone.zoneTypeId = int.Parse(retrievedRow[3].ToString());

                    DataObject.stnArray[stnArrayOffset] = paZone;
                }
            }
        }

        private void Load_station_device_name()
        {
            dbConnEnum dbConn = dbConnEnum.ErrNoConn;
            localSql_ = "select bit_position, device_name from pa_station_device_type where enabled = 1 order by bit_position asc";

            if (Program.defaultLanguage == "en-US")
            {
                localSql_ = "select bit_position, device_code from pa_station_device_type where enabled = 1 order by bit_position asc";
            }

            var queries = new List<SqlQuery> { new SqlQuery { CommandText = localSql_ } };
            DataSet ds = Program.dbLock.FetchData(queries, ref dbConn);
            Program.deviceNameDic_.Clear();
            if (ds.Tables != null || ds.Tables.Count > 0)
            {
                foreach (DataRow retrievedRow in ds.Tables[0].Rows)
                {
                    int bitPos = int.Parse(retrievedRow[0].ToString());
                    string deviceName = retrievedRow[1].ToString();
                    Program.deviceNameDic_.Add(bitPos, deviceName);
                }
            }
        }


        #endregion


        #region Enable/Disable Button Decider Codes
        /// <summary>
        /// Decide whether to enable or disable the Clear Button buttons
        /// </summary>
        private void setClearButton()
        {
            if (!string.IsNullOrEmpty(textBox_selected_msgcontent.Text))
            {
                button_clearOne_msg.Enabled = true;
            }
            else
            {
                button_clearOne_msg.Enabled = false;
            }
        }

        /// <summary>
        /// Enable/Disable Execute Button according to selection make by operator and the MFT control/monitor mode.
        /// </summary>
        private void setExecuteButton()
        {
            button_execute.Enabled = paServerConnected_;
            button_live.Enabled = paServerConnected_;
            button_air_alarm.Enabled = paServerConnected_;
            button_device_status.Enabled = paServerConnected_;
        }

        /// <summary>
        /// Decide whether to enable or disable the various buttons within the PA Mode Section
        /// </summary>
        private void setPaMode()
        {
            // DVA/Live button
            if (Program.controlMode)
            {
            }
            else
            {
            }
        }

        /// <summary>
        /// Decide which HMI components to display according to OCC or Station mode
        /// </summary>
        private void ConfigurePaManDisplay()
        {
            // To display the display panel of selected zones according to loggin profile 
            // (additional 'Stn' column if loggin location is OCC)
            if (Program.occMode)
            {
                // OCC components - Enable
                groupBox_station.Visible = true;
                dataGridView_OCC_selectedzone.Visible = true;
                ////Button_BackToStn.Visible = true;

                // Station components - Disable
                groupBox_zones.Visible = true;
                dataGridView_selectedzone.Visible = true;
            }
            else
            {
                // OCC components - Disable
                groupBox_station.Visible = true;
                dataGridView_OCC_selectedzone.Visible = false;
                ////Button_BackToStn.Visible = false;

                // Station components - Enable
                groupBox_zones.Visible = true;
                dataGridView_selectedzone.Visible = true;
            }
        }
        #endregion

        #region General Functions
        /// <summary>
        /// Resume the broadcast check
        /// </summary>
        private void resumeBcCheck()
        {
            Program.WriteEventLog("[INFO] Resume BC Check!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

            lock (bcInProgressLock_)
            {
                bcInProgressFlag_ = false;
            }
        }

        /// <summary>
        /// Pause the check of BC related tables
        /// </summary>
        private void pauseBcCheck()
        {
            Program.WriteEventLog("[INFO] Pause BC Check!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

            lock (bcInProgressLock_)
            {
                bcInProgressFlag_ = true;
            }
        }

        /// <summary>
        /// Check/uncheck BGM checkbox
        /// </summary>
        /// <param name="checkState">Check state of the checkbox to be set</param>
        private void bgmCheckBox_CheckState(bool checkState)
        {

        }
        #endregion

        /// <summary>
        /// Refresh the datagridview to show the selected zones
        /// </summary>
        private void Refresh_ZonesDgv()
        {
            if (Program.occMode)
            {
                DGV_Selected_Zones_OCC();
            }
            else
            {
                DGV_Selected_Zones();
            }
        }

        /// <summary>
        /// Check if all zones in a category is checked
        /// </summary>
        /// <param name="categoryName">Category Name</param>
        /// <returns>bool: true if all the zones are checked, otherwise false</returns>
        private bool allZoneGroupChecked(string categoryName)
        {
            // Nothing to validate : categoryName
            bool catExistsInStn = false;

            if (Program.occMode)
            {
                int stnIndex = currSelStnNo_ - 1;
                int stnZoneOffset = 0;

                if (stnIndex < 0)
                {
                    Program.WriteEventLog("[ERROR] stnIndex value less than 0! Setting it to 0. ", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
                    stnIndex = 0;
                }

                stnZoneOffset = (stnIndex * Program.MAXZONEID);

                for (int zIndex = 0; zIndex < Program.MAXZONEID; zIndex++)
                {
                    // Check the category of each zone within the station, return false as soon as a checkbox is detected as uncheck
                    if (DataObject.stnArray[stnZoneOffset + zIndex] != null && DataObject.stnArray[stnZoneOffset + zIndex].zoneCategory == categoryName)
                    {
                        catExistsInStn = true;

                        if (!zoneCheckBoxArray_[zIndex].Checked)
                        {
                            return false;
                        }
                    }
                }
            }
            else
            {
                // Check the category of all zones, return false as soon as a checkbox is detected as uncheck
                for (int zIndex = 0; zIndex < Program.MAXZONEID; zIndex++)
                {
                    //Set zone check status as according to the status of the Zone Group Checkbox 
                    if (DataObject.zoneArray[zIndex] != null && DataObject.zoneArray[zIndex].zoneCategory == categoryName)
                    {
                        catExistsInStn = true;

                        if (!zoneCheckBoxArray_[zIndex].Checked)
                        {
                            return false;
                        }
                    }
                }

            }

            if (catExistsInStn)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        /// <summary>
        /// Centers a windows form on the MFT screen from which it was launched.
        /// </summary>
        /// <param name="screenXCoordinate">The X origin coordinate of the MFT screen.</param>
        /// <param name="screenYCoordinate">The Y origin coordinate of the MFT screen.</param>
        /// <param name="formToCenter">The form to center.</param>
        private void CenterApplicationOnScreen(int screenXCoordinate, int screenYCoordinate, Form formToCenter)
        {
            Point startLoc = new Point();

            FormPosition.initialize();
            FormPosition.setFormLocation(formToCenter, screenXCoordinate, screenYCoordinate);

            startLoc = formToCenter.Location;
            startLoc = FormPosition.checkMFTFormPos(startLoc);
            formToCenter.Location = startLoc;
        }

        /// <summary>
        /// Set the mouse cursor image
        /// </summary>
        /// <param name="cursorType">Selected mouse cursor type</param>
        private void SetCursorType(Cursor cursorType)
        {
            this.Cursor = cursorType;
        }

        /// <summary>
        /// Clear everything from the datagrid view
        /// </summary>
        /// <param name="oControl">Reference to the control object</param>
        private void dgvRowClear(Control oControl)
        {
            // Nothing to validate : oControl
            try
            {
                if (oControl.InvokeRequired)
                {
                    DgvClearCallback d = new DgvClearCallback(dgvRowClear);
                    oControl.Invoke(d, new object[] { oControl });
                }
                else
                {
                    DataGridView dgvObj = oControl as DataGridView;
                    dgvObj.Rows.Clear();
                }
            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }
        }

        /// <summary>
        /// Add row to datagridview object
        /// </summary>
        /// <param name="oControl">Reference to control object</param>
        /// <param name="rowDetails">Row details to be added to datagridview</param>
        private void dgvRowAdd(Control oControl, string[] rowDetails)
        {
            // Nothing to validate : oControl, rowDetails
            try
            {
                if (oControl.InvokeRequired)
                {
                    DgvAddRowCallback d = new DgvAddRowCallback(dgvRowAdd);
                    oControl.Invoke(d, new object[] { oControl, rowDetails });
                }
                else
                {
                    DataGridView dgvObj = oControl as DataGridView;
                    dgvObj.Rows.Add(rowDetails);

                    dgvObj.AutoResizeRow(dgvObj.Rows.Count - 1);
                    dgvObj.PerformLayout();
                }
            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }
        }

        /// <summary>
        /// Set the color of the connection box
        /// </summary>
        /// <param name="oControl">Reference to the control object</param>
        /// <param name="paSysConnected">true if the PA System is connected</param>
        private void setStatusColor(Control oControl, bool paSysConnected)
        {
            // Nothing to validate : oControl, paSysConnected
            try
            {
                if (oControl.InvokeRequired)
                {
                    SetConnectionStatusCallback d = new SetConnectionStatusCallback(setStatusColor);
                    oControl.Invoke(d, new object[] { oControl, paSysConnected });
                }
                else
                {
                    CheckBox checkboxObj = oControl as CheckBox;

                    if (paSysConnected)
                    {
                        checkboxObj.BackColor = Color.Green;
                        checkboxObj.ForeColor = Color.White;
                        checkboxObj.Text = paConnected_;
                    }
                    else
                    {
                        checkboxObj.BackColor = Color.Red;
                        checkboxObj.ForeColor = Color.White;
                        checkboxObj.Text = paDisconnected_;
                    }
                }
            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }
        }

        private void setFormTitle(Control oControl, string titleText)
        {
            Form formObj = oControl as Form;
            formObj.Text = titleText;
        }

        private void PaSysConnChange(bool linkStatus)
        {
        }

        private void listBox_prerecorded_msg_SelectedIndexChanged(object sender, EventArgs e)
        {
            ListBox messageListBox = sender as ListBox;
            selectedPreRecordedMsg_ = messageListBox?.SelectedItem as PreRecordedMessageStruct;
            if (selectedPreRecordedMsg_ != null)
            {
                if (messageListBox == listBox_prerecorded_msg)
                {
                    listBox_emergency_msg.SelectedIndex = -1;
                }
                else
                {
                    listBox_prerecorded_msg.SelectedIndex = -1;
                }

                textBox_selected_msgcontent.Text = selectedPreRecordedMsg_.MsgContent;
                setClearButton();
                toolTip_msg.SetToolTip(messageListBox, string.Empty);
                toolTip_msg.Hide(messageListBox);
            }
        }

        private void StartPaServerStatusTimer()
        {
            _paServiceStatusTimer = new System.Threading.Timer(TimerCallback_PaServiceStatus, null, 0, 1000);
        }

        private void TimerCallback_PaServiceStatus(object state)
        {
            if (paServerConnected_ != PaService.Instance.IsPaServerAvailable)
            {
                paServerConnected_ = PaService.Instance.IsPaServerAvailable;
                this.Invoke((MethodInvoker)(() =>
                {
                    setStatusColor(checkBox_status, paServerConnected_);
                    setExecuteButton();
                }));

                if (paServerConnected_)
                {
                    //query msg version
                    QueryMsgVersion();
                }
            }

            if (paServerConnected_)
            {
                RequestStationStatus();
            }
        }

        private async void button_require_record_Click(object sender, EventArgs e)
        {
            try
            {
                byte seatId = Convert.ToByte(comboBox_seats.SelectedValue);
                ushort recordId = Convert.ToUInt16(comboBox_record_id.SelectedItem);
                M47 cmd = new M47(seatId, recordId);

                bool result = await PaService.Instance.RequestRecord(cmd);

                if (result)
                {
                    Program.MessageBox_Normal(mbSendCmdSuccessful_, sysErr_, this);
                }
                else
                {
                    Program.MessageBox_Error(mbSendCmdFailed_, sysErr_);
                }
            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }
        }

        private async void button_start_record_Click(object sender, EventArgs e)
        {
            try
            {
                M48 cmd = new M48();

                bool result = await PaService.Instance.StartRecord(cmd);

                if (result)
                {
                    Program.MessageBox_Normal(mbSendCmdSuccessful_, sysErr_, this);
                }
                else
                {
                    Program.MessageBox_Error(mbSendCmdFailed_, sysErr_);
                }
            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }

        }

        private async void button_end_record_Click(object sender, EventArgs e)
        {
            try
            {
                M49 cmd = new M49();

                bool result = await PaService.Instance.EndRecord(cmd);

                if (result)
                {
                    Program.MessageBox_Normal(mbSendCmdSuccessful_, sysErr_, this);
                }
                else
                {
                    Program.MessageBox_Error(mbSendCmdFailed_, sysErr_);
                }
            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }
        }

        private async void buttonVolume_Click(object sender, EventArgs e)
        {
            try
            {
                bool stationSelected = false;
                foreach (var item in stnNoToStnCbDic_)
                {
                    if (item.Value.Checked)
                    {
                        stationSelected = true;
                        break;
                    }
                }

                if (stationSelected)
                {
                    List<StationVolume> stationVolumeList = new List<StationVolume>();
                    foreach (var item in stnNoToStnCbDic_)
                    {
                        if (item.Value.Checked)
                        {
                            byte volumePeak = Convert.ToByte(numericUpDown_peak.Value);
                            byte volumeOffPeak = Convert.ToByte(numericUpDown_off_peak.Value);
                            byte volumeNight = Convert.ToByte(numericUpDown_night.Value);
                            byte stationId = Convert.ToByte(item.Value.Tag);
                            stationVolumeList.Add(new StationVolume(stationId, volumePeak, volumeOffPeak, volumeNight));
                        }
                    }

                    M41 cmd = new M41(stationVolumeList);

                    bool result = await PaService.Instance.SetVolume(cmd);

                    if (result)
                    {
                        Program.MessageBox_Normal(mbSendCmdSuccessful_, sysErr_, this);
                    }
                    else
                    {
                        Program.MessageBox_Error(mbSendCmdFailed_, sysErr_);
                    }
                }
                else
                {
                    Program.MessageBox_Normal(mbSelectStationMsg_, sysErr_, this);
                }
            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }
        }

        private void PaService_OnStationStatusReceived(object sender, List<KeyValuePair<byte, int>> stationStatusList)
        {
            try
            {
                for (int i = 0; i < stationStatusList.Count(); i++)
                {
                    int stationId = stationStatusList[i].Key;
                    EnumType.StationStatus status = EnumType.StationStatus.Offline;
                    if (stationStatusList[i].Value == 0)
                    {
                        status = EnumType.StationStatus.Offline;
                    }
                    else if (stationStatusList[i].Value == 1)
                    {
                        status = EnumType.StationStatus.Busy;
                    }
                    else
                    {
                        status = EnumType.StationStatus.Availabel;
                    }
                    Program.stationStatusDic_[stationId] = status;
                }
                UpdateStationCheckBoxesStatus();
            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }
        }

        private void UpdateStationCheckBoxesStatus()
        {
            foreach (var item in stnNoToStnCbDic_)
            {
                if (Program.stationStatusDic_.ContainsKey(item.Key))
                {
                    if (Program.stationStatusDic_[item.Key] == EnumType.StationStatus.Offline)
                    {
                        checkBox_Red(item.Value);
                    }
                    else if (Program.stationStatusDic_[item.Key] == EnumType.StationStatus.Busy)
                    {
                        checkBox_Yellow(item.Value);
                    }
                    else
                    {
                        if (item.Value.Checked)
                        {
                            checkBox_Green_Check(item.Value);
                        }
                        else
                        {
                            checkBox_Green_Uncheck(item.Value);
                        }
                    }
                }
            }
        }

        private async void RequestStationStatus()
        {
            try
            {
                List<byte> stationList = new List<byte>() { 0 };
                //foreach (var item in stnNoToStnCbDic_)
                //{
                //    stationList.Add(Convert.ToByte(item.Key));
                //}

                M1 cmd = new M1(stationList.ToArray());

                bool result = await PaService.Instance.GetStationStatus(cmd);
            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }
        }

        private void button_station_status_Click(object sender, EventArgs e)
        {
            RequestStationStatus();
        }

        private void button_device_status_Click(object sender, EventArgs e)
        {
            DeviceStatus wnd = new DeviceStatus();
            wnd.ShowDialog();
        }

        private void button_air_alarm_Click(object sender, EventArgs e)
        {
            try
            {
                bool stationSelected = false;
                foreach (var item in stnNoToStnCbDic_)
                {
                    if (item.Value.Checked)
                    {
                        stationSelected = true;
                        break;
                    }
                }

                if (stationSelected)
                {
                    List<byte> idList = new List<byte>();

                    foreach (var item in stnNoToStnCbDic_)
                    {
                        if (item.Value.Checked)
                        {
                            byte stationId = Convert.ToByte(item.Value.Tag);
                            idList.Add(stationId);
                        }
                    }

                    AirRaidBroadcast dlg = new AirRaidBroadcast(idList);
                    dlg.ShowDialog();
                }
                else
                {
                    Program.MessageBox_Normal(mbSelectStationMsg_, sysErr_, this);
                }

            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }
        }

        private void button_live_Click(object sender, EventArgs e)
        {
            try
            {
                bool stationSelected = false;
                foreach (var item in stnNoToStnCbDic_)
                {
                    if (item.Value.Checked)
                    {
                        stationSelected = true;
                        break;
                    }
                }

                if (!stationSelected)
                {
                    Program.MessageBox_Normal(mbSelectStationMsg_, sysErr_, this);
                    return;
                }

                if ((dataGridView_selectedzone.RowCount == 0) && (dataGridView_OCC_selectedzone.RowCount == 0))
                {
                    Program.MessageBox_Normal(mbSelectZoneMsg_, sysErr_, this);
                    return;
                }

                byte playArea = 0;

                bool[] selectedZones = DataObject.Selected_Station_Zone;
                for (int i = 0; i < Program.MAXZONEID; i++)
                {
                    if (selectedZones[i])
                    {
                        ZoneStruct paZone = DataObject.zoneArray[i];

                        playArea |= paZone.zoneTypeIdBitValue;
                    }
                }

                List<StationBroadcast> sbList = new List<StationBroadcast>();

                foreach (var item in stnNoToStnCbDic_)
                {
                    if (item.Value.Checked)
                    {
                        byte stationId = Convert.ToByte(item.Value.Tag);
                        sbList.Add(new StationBroadcast(stationId, playArea));
                    }
                }

                LiveBroadcastEx dlg = new LiveBroadcastEx(sbList);
                dlg.ShowDialog();
            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }
        }

        int lastIndex_ = -1;
        ListBox lastToolTipListBox_ = null;
        private void listBox_prerecorded_msg_MouseMove(object sender, MouseEventArgs e)
        {
            ListBox messageListBox = sender as ListBox;
            if (messageListBox == null)
            {
                return;
            }

            if (messageListBox != lastToolTipListBox_)
            {
                lastToolTipListBox_ = messageListBox;
                lastIndex_ = -1;
            }

            int index = messageListBox.IndexFromPoint(e.Location);

            if (index >= 0 && index != messageListBox.SelectedIndex)
            {
                if (index != lastIndex_)
                {
                    lastIndex_ = index;

                    string text = messageListBox.Items[index].ToString();
                    Rectangle itemRect = messageListBox.GetItemRectangle(index);

                    if(isTextTruncated(messageListBox, text, itemRect))
                    {
                        toolTip_msg.Show(text, messageListBox, e.X + 15, e.Y + 10);
                    }
                    else
                    {
                        toolTip_msg.Hide(messageListBox);
                    }               
                }
            }
            else
            {
                toolTip_msg.Hide(messageListBox);
            }
        }

        private bool isTextTruncated(ListBox listBox, string text, Rectangle itemRect)
        {
            using (Graphics g = listBox.CreateGraphics())
            {
                Size textSize = TextRenderer.MeasureText(g, text, listBox.Font);
                return textSize.Width > itemRect.Width;
            }
        }

        private void listBox_prerecorded_msg_MouseLeave(object sender, EventArgs e)
        {
            ListBox messageListBox = sender as ListBox;
            if (messageListBox != null)
            {
                toolTip_msg.SetToolTip(messageListBox, string.Empty);
                toolTip_msg.Hide(messageListBox);
            }
        }
    }
}
