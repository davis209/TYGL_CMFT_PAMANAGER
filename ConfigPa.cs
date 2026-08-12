#region Header Block
/// <classname>ConfigPa</classname>
/// <description>This module allows the operator to configure the Zones.</description>
/// <filename>ConfigPa.cs</filename>
/// <authors>Tay Yu Hui</authors>
/// <version>00.03.01.00</version>
/// <changes> Not application</changes>
/// <todo>Not application</todo>
/// <copyright>Copyright @ 2010-2010 Singapore Technologies Electronics Ltd</copyright>
#endregion

using System;							// Contain all other namespaces as subordinates
using System.Collections.Generic;		// Allow subitems to be contained in a variety of containers
using System.Reflection;				// Retrieve information about assemblies, modules, members, parameters, and other entities in managed code by examining their metadata. using Oracle.DataAccess.Client;		// Provide Oracle Data Provider object which expose inherited, provider-specific, interface implementations of methods and properties
using System.Text.RegularExpressions;	// It can be used to quickly parse large amounts of text to find specific character patterns
using System.Windows.Forms;				// Contain classes for creating Window-based applications
//using Oracle.DataAccess.Client;			// Provide Oracle Data Provider object which expose inherited, provider-specific, interface implementations of methods and properties
////using ste.scada.common;					// Provide commonly used classes for the creation of MFT
using ste.common.exceptions;
using ste.common.util;
using ste.common.Forms;
using System.Data;
using System.Text;
using System.Linq;

namespace ste.pa.pamanager
{
    /// <summary>
    /// Implements the GUI for configuring the PA System, such as the zones in each station, IP address, etc
    /// </summary>
    public partial class ConfigPa : Form
    {
        // Flag to indicate if the changes has been saved 
        private bool isSaveZoneCReq_ = false;
        private bool isSavePaScCReq_ = false;

        // Map Station Id to TreeNode		
        Dictionary<string, TreeNode> stnIdToTreeNode_ = new Dictionary<string, TreeNode>();

        // Store values of [Configure Zones] tab to use for comparsion
        private bool zoneAddFormShown_ = false;
        private string prevZoneStnName_ = "";
        private string prevZoneNo_ = "";
        private string prevZoneCat_ = "";
        private string prevZoneType_ = "";
        private string prevZoneDesc_ = "";
        private string prevZoneDescLocal_ = "";

        // Store values of [PA System] tab to use for comparsion
        private string prevPasStnName_ = "";
        private string prevPasStnNo_ = "";
        private string prevPasPriIp_ = "";
        private string prevPasSecIp_ = "";
        private string prePasPortNo_ = "";


        private static string fileName_ = MethodBase.GetCurrentMethod().DeclaringType.Name;
        private static string localSql_ = "";

        //private OracleDataReader oracleDr_ = null;

        private static List<DisplayValue> stationList_ = new List<DisplayValue>();
        private static List<DisplayValue> categoryList_ = new List<DisplayValue>();
        private static List<DisplayValue> typeList_ = new List<DisplayValue>();
        private  List<KeyValuePair<int, string>> weekdays_ = new List<KeyValuePair<int, string>>();
        private  List<KeyValuePair<int, string>> stations_ = new List<KeyValuePair<int, string>>();

        private static string sysErr_ = "Configure Zones";
        private static string nodeRoot_ = "Root";
        private static string gbAddZone_ = "Add Zones";
        private static string gbModifyZone_ = "Zone Configuration";
        private static string lbAdd_ = "Add to";
        private static string emptyZoneNo_ = "Please enter a Zone Number!";
        private static string zoneExists_ = "Zone Number already exists";
        private static string mbLaunch_ = "Please restart PA Manager for the new configuration to take effect!";

        private static string updateZonePass_ = "Update of Zone successful!";
        private static string updateZoneFail_ = "Update of Zone failed!";
        private static string addZonePass_ = "Add Zone successful!";
        private static string addZoneFail_ = "Add Zone failed!";
        private static string deleteZonePass_ = "Deletion of Zone successful!";
        private static string deleteZoneFail_ = "Deletion of Zone failed!";

        private static string cannotEmpty_ = " cannot be empty!";
        private static string isInvalidIp_ = " is not a valid IP Address Format!";
        private static string isNotNumeric_ = " is not a numeric value!";
        private static string isNotValidRange_ = " is not in the valid range of 1 to ";

        private static string addPasPass_ = "Adding of PA System Configuration successful!";
        private static string addPasFail_ = "Adding of PA System Configuration failed!";
        private static string updatePasPass_ = "Update of PA System Configuration successful!";
        private static string updatePasFail_ = "Update of PA System Configuration failed!";

        private static string addScPass_ = "Adding of Source Code successful!";
        private static string addScFail_ = "Adding of Source Code failed!";
        private static string updateScPass_ = "Update of Source Code successful!";
        private static string updateScFail_ = "Update of Source Code failed!";

        private static string mbZoneNotExists_ = "Zone doesn't exists in this station!";
        private static string recordsExistsNoAdd_ = "Add Failed! Record already exists!";
        private static string recordsExists_ = "Record already exists!";
        private static string recordsUpdateFail_ = "Update failed! Record doesn't exists!";

        private static string CmfDelZone_ = "Delete Zone from station?";

        private TabPage tabPage_schedule_;
        private DataGridView dataGridView_schedule_;

        /// <summary>
        /// Static constructor 
        /// </summary>
        static ConfigPa()
        {
        }

        /// <summary>
        /// Default constructor
        /// </summary>
        public ConfigPa()
        {
            InitializeComponent();
            InitializeScheduleTab();
        }

        /// <summary>
        /// Method invoked when form is first loaded.
        /// </summary>
        /// <param name="sender">Control that raised the event</param>
        /// <param name="e">Additional information about the event that was raised</param>
        /// <exception cref="NoOracleDBException">Handle No Oracle DB Exception</exception>
        /// <exception cref="Exception">Handle generic exception</exception>
        /// <since>00.03.01.00</since>
        private void ConfigZones_Load(object sender, EventArgs e)
        {
            // Nothing to validate : sender, e
            try
            {
                localizeForm();

                loadStation();
                loadCategory();
                loadType();

                initTreeViewStationNodes();

                saveCurrPasTab();

                initStationWeekdaySchedul();
                LoadBroadcastSchedules();

            }
            catch (NoOracleDBException ex)
            {
                Program.WriteEventLog("[ERROR] [NoOracleDBException] " + ex.ToString(), fileName_ + " -> " + MethodInfo.GetCurrentMethod().Name + "()");
                Program.MessageBox_Error(Program.MB_NoDBConn, sysErr_);
                this.Close();
            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + " -> " + MethodInfo.GetCurrentMethod().Name + "()");
                Program.MessageBox_Error(Program.MB_FormEx, sysErr_);
                this.Close();
            }
        }

        private void InitializeScheduleTab()
        {
            tabPage_schedule_ = new TabPage("訊息排程")
            {
                BackColor = System.Drawing.SystemColors.Control,
                Padding = new Padding(6)
            };
            tabControl_Config.TabPages.Add(tabPage_schedule_);

            var layout = new TableLayoutPanel { Dock = DockStyle.Fill, RowCount = 2 };
            layout.RowStyles.Add(new RowStyle(SizeType.AutoSize));
            layout.RowStyles.Add(new RowStyle(SizeType.Percent, 100));

            var buttons = new FlowLayoutPanel { Dock = DockStyle.Fill, AutoSize = true };
            var addButton = new Button { Text = "新增", AutoSize = true };
            var editButton = new Button { Text = "編輯", AutoSize = true };
            var enabledButton = new Button { Text = "啟用 / 停用", AutoSize = true };
            var deleteButton = new Button { Text = "刪除", AutoSize = true };
            var refreshButton = new Button { Text = "重新整理", AutoSize = true };
            addButton.Click += (s, e) => CreateSchedule();
            editButton.Click += (s, e) => EditSelectedSchedule();
            enabledButton.Click += (s, e) => ToggleScheduleEnabled();
            deleteButton.Click += (s, e) => DeleteSchedule();
            refreshButton.Click += (s, e) => LoadBroadcastSchedules();
            buttons.Controls.AddRange(new Control[] { addButton, editButton, enabledButton, deleteButton, refreshButton });

            dataGridView_schedule_ = new DataGridView
            {
                Dock = DockStyle.Fill,
                AutoGenerateColumns = true,
                ReadOnly = true,
                AllowUserToAddRows = false,
                AllowUserToDeleteRows = false,
                MultiSelect = false,
                SelectionMode = DataGridViewSelectionMode.FullRowSelect,
                ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing,
                ColumnHeadersHeight = 32,
                BackgroundColor = System.Drawing.SystemColors.Window
            };
            dataGridView_schedule_.CellDoubleClick += (s, e) => EditSelectedSchedule();
            layout.Controls.Add(buttons, 0, 0);
            layout.Controls.Add(dataGridView_schedule_, 0, 1);
            tabPage_schedule_.Controls.Add(layout);
        }

        private void LoadBroadcastSchedules()
        {
            if (dataGridView_schedule_ == null) return;
            try
            {
                dbConnEnum dbConn = dbConnEnum.ErrNoConn;
                string sql = "SELECT SCHEDULE_ID AS '識別碼', SCHEDULE_NAME AS '名稱', " +
                    "CASE WHEN ENABLED=1 THEN '啟用' ELSE '停用' END AS '狀態', " +
                    "MSG_ID AS '訊息編號', STATIONS AS '車站', ZONES AS '區域', " +
                    "SCHEDULE_TYPE AS '類型', START_AT AS '開始時間', NEXT_RUN_AT AS '下次執行時間' " +
                    "FROM pa_broadcast_schedule WHERE LOCATION_ID=" + Program.profileLocIndex + " ORDER BY SCHEDULE_ID DESC";
                var queries = new List<SqlQuery> { new SqlQuery { CommandText = sql } };
                DataSet ds = Program.dbLock.FetchData(queries, ref dbConn);
                dataGridView_schedule_.DataSource = ds != null && ds.Tables.Count > 0 ? ds.Tables[0] : null;
                if (dataGridView_schedule_.Columns.Contains("識別碼")) dataGridView_schedule_.Columns["識別碼"].Visible = false;
            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] Load broadcast schedules: " + ex, fileName_ + "." + MethodBase.GetCurrentMethod().Name + "()");
                MessageBox.Show("無法載入訊息排程，請確認已建立 PA_BROADCAST_SCHEDULE 資料表。", sysErr_, MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private long? SelectedScheduleId()
        {
            if (dataGridView_schedule_ == null || dataGridView_schedule_.CurrentRow == null) return null;
            object value = dataGridView_schedule_.CurrentRow.Cells["識別碼"].Value;
            return value == null || value == DBNull.Value ? (long?)null : Convert.ToInt64(value);
        }

        private void CreateSchedule()
        {
            OpenScheduleEditor(null);
        }

        private void EditSelectedSchedule()
        {
            long? scheduleId = SelectedScheduleId();
            if (!scheduleId.HasValue)
            {
                MessageBox.Show("請先選擇要編輯的排程。", sysErr_, MessageBoxButtons.OK, MessageBoxIcon.Information);
                return;
            }
            OpenScheduleEditor(scheduleId);
        }

        private void OpenScheduleEditor(long? scheduleId)
        {
            using (var form = new ScheduleConfigForm(scheduleId, LoadBroadcastSchedules))
            {
                form.ShowDialog(this);
            }
        }

        private void ToggleScheduleEnabled()
        {
            long? id = SelectedScheduleId();
            if (!id.HasValue) { MessageBox.Show("請先選擇排程。", sysErr_); return; }
            object current = dataGridView_schedule_.CurrentRow.Cells["狀態"].Value;
            bool enabled = current != null && current.ToString() == "啟用";
            ExecuteScheduleCommand("UPDATE pa_broadcast_schedule SET ENABLED=" + (enabled ? 0 : 1) + ", UPDATED_AT=NOW(3) WHERE SCHEDULE_ID=" + id.Value);
        }

        private void DeleteSchedule()
        {
            long? id = SelectedScheduleId();
            if (!id.HasValue) { MessageBox.Show("請先選擇排程。", sysErr_); return; }
            if (MessageBox.Show("確定要刪除選取的排程？", sysErr_, MessageBoxButtons.YesNo, MessageBoxIcon.Warning) != DialogResult.Yes) return;
            ExecuteScheduleCommand("DELETE FROM pa_broadcast_schedule WHERE SCHEDULE_ID=" + id.Value);
        }

        private void ExecuteScheduleCommand(string sql)
        {
            try
            {
                dbConnEnum dbConn = dbConnEnum.ErrNoConn;
                var queries = new List<SqlQuery> { new SqlQuery { CommandText = sql } };
                if (Program.dbLock.ExcuteNoneResultQuery(queries, ref dbConn) < 1)
                    throw new InvalidOperationException("No record was changed.");
                LoadBroadcastSchedules();
            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] Update broadcast schedule: " + ex, fileName_ + "." + MethodBase.GetCurrentMethod().Name + "()");
                MessageBox.Show("無法更新排程。已有執行紀錄的排程應停用，而非刪除。", sysErr_, MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void initStationWeekdaySchedul()
        {            
            stations_ = Program.stnNoToStnNameDic_.Select(x => new KeyValuePair<int, string>(x.Key, x.Value)).ToList();
            stations_.Insert(0, new KeyValuePair<int, string>(0, "OVERALL"));

            if (Program.defaultLanguage == "en-US")
            {
                weekdays_.Add(new KeyValuePair<int, string>(1, "Monday"));
                weekdays_.Add(new KeyValuePair<int, string>(2, "Tuesday"));
                weekdays_.Add(new KeyValuePair<int, string>(3, "Wednesday"));
                weekdays_.Add(new KeyValuePair<int, string>(4, "Thursday"));
                weekdays_.Add(new KeyValuePair<int, string>(5, "Friday"));
                weekdays_.Add(new KeyValuePair<int, string>(6, "Saturday"));
                weekdays_.Add(new KeyValuePair<int, string>(7, "Sunday"));
            }
            else
            {
                weekdays_.Add(new KeyValuePair<int, string>(1, "星期一"));
                weekdays_.Add(new KeyValuePair<int, string>(2, "星期二"));
                weekdays_.Add(new KeyValuePair<int, string>(3, "星期三"));
                weekdays_.Add(new KeyValuePair<int, string>(4, "星期四"));
                weekdays_.Add(new KeyValuePair<int, string>(5, "星期五"));
                weekdays_.Add(new KeyValuePair<int, string>(6, "星期六"));
                weekdays_.Add(new KeyValuePair<int, string>(7, "星期日"));
            }

            comboBox_time_station.DataSource = stations_;
            comboBox_time_station.DisplayMember = "Value";
            comboBox_time_station.ValueMember = "Key";

            comboBox_time_weekday.DataSource = weekdays_;
            comboBox_time_weekday.DisplayMember = "Value";
            comboBox_time_weekday.ValueMember = "Key";

            DisplayStationSchedule();
        }
          
		/// <summary>
		/// This method sets the variables to its localized equivalent text.
		/// </summary>
		/// <since>00.03.01.00</since>
		private void localizeForm()
		{
			System.Globalization.CultureInfo ci = new System.Globalization.CultureInfo(Program.defaultLanguage);
			Localization.setCulture(ci);
			Localization.setApplicationName(Program.moduleName);

			if (Localization.configure())
			{
				this.Text = Localization.localize("CZ_FORM_NAME", this.Text);

				sysErr_ = Localization.localize("CZ_FORM_NAME", sysErr_);
				nodeRoot_ = Localization.localize("CZ_ROOT", nodeRoot_);
				gbAddZone_ = Localization.localize("CZ_GP_ADD_ZONE", gbAddZone_);
				gbModifyZone_ = Localization.localize("CZ_GP_MODIFY_ZONE", gbModifyZone_);
				lbAdd_ = Localization.localize("CZ_LB_ADD", lbAdd_);
				emptyZoneNo_ = Localization.localize("CZ_EMPTY_ZONE_NUM", emptyZoneNo_);
				zoneExists_ = Localization.localize("CZ_ZONE_EXISTS", zoneExists_);
				mbLaunch_ = Localization.localize("CZ_MB_LAUNCH", mbLaunch_);

				updateZonePass_ = Localization.localize("CZ_UPDATE_ZONE_PASS", updateZonePass_);
				updateZoneFail_ = Localization.localize("CZ_UPDATE_ZONE_FAIL", updateZoneFail_);
				addZonePass_ = Localization.localize("CZ_ADD_ZONE_PASS", addZonePass_);
				addZoneFail_ = Localization.localize("CZ_ADD_ZONE_FAIL", addZoneFail_);
				deleteZonePass_ = Localization.localize("CZ_DELETE_ZONE_PASS", deleteZonePass_);
				deleteZoneFail_ = Localization.localize("CZ_DELETE_ZONE_FAIL", deleteZoneFail_);

				cannotEmpty_ = Localization.localize("CZ_CANNOT_EMPTY", cannotEmpty_);
				isInvalidIp_ = Localization.localize("CZ_INVALID_IP", isInvalidIp_);
				isNotNumeric_ = Localization.localize("CZ_NOT_NUMERIC", isNotNumeric_);
				isNotValidRange_ = Localization.localize("CZ_NOT_VALID_RANGE", isNotValidRange_) + Program.MAXZONEID;

				addPasPass_ = Localization.localize("CZ_ADD_PAS_PASS", addPasPass_);
				addPasFail_ = Localization.localize("CZ_ADD_PAS_FAIL", addPasFail_);
				updatePasPass_ = Localization.localize("CZ_UPDATE_PAS_PASS", updatePasPass_);
				updatePasFail_ = Localization.localize("CZ_UPDATE_PAS_FAIL", updatePasFail_);

				addScPass_ = Localization.localize("CZ_ADD_SC_PASS", addScPass_);
				addScFail_ = Localization.localize("CZ_ADD_SC_FAIL", addScFail_);
				updateScPass_ = Localization.localize("CZ_UPDATE_SC_PASS", updateScPass_);
				updateScFail_ = Localization.localize("CZ_UPDATE_SC_FAIL", updateScFail_);

				mbZoneNotExists_ = Localization.localize("CZ_ZONE_NOT_EXISTS", mbZoneNotExists_);
				recordsExistsNoAdd_ = Localization.localize("CZ_REC_ADD_FAIL", recordsExistsNoAdd_);
				recordsExists_ = Localization.localize("CZ_REC_EXISTS", recordsExists_);
				recordsUpdateFail_ = Localization.localize("CZ_UPDATE_FAIL", recordsUpdateFail_);

				tabPage1.Text = Localization.localize("CZ_GB_STATION", tabPage1.Text);
				tabPage2.Text = Localization.localize("CZ_GP_PAS", tabPage2.Text);
				tabPage3.Text = Localization.localize("TAB_CONFIGURE_TIME", tabPage3.Text);

				#region Zone Tab
				groupBox_Station.Text = Localization.localize("CZ_GB_STATION", groupBox_Station.Text);
				groupBox_Config.Text = Localization.localize("CZ_GB_ADD_ZONE", groupBox_Config.Text);

				label_stationName.Text = Localization.localize("CZ_LB_STATION", label_stationName.Text);
				label_zoneNo.Text = Localization.localize("CZ_LB_ZONE_NUM", label_zoneNo.Text);
				label_Category.Text = Localization.localize("CZ_LB_CAT", label_Category.Text);
				label_zoneType.Text = Localization.localize("CZ_LB_TYPE", label_zoneType.Text);
				label_desc.Text = Localization.localize("CZ_LB_DESC", label_desc.Text);
				label_descLocal.Text = Localization.localize("CZ_LB_DESC_LOCAL", label_descLocal.Text);

				comboBox_stationName.Tag = removeLastChar(Localization.localize("CZ_LB_STATION", label_stationName.Text));
				textBox_zoneNo.Tag = removeLastChar(Localization.localize("CZ_LB_ZONE_NUM", label_zoneNo.Text));
				comboBox_Category.Tag = removeLastChar(Localization.localize("CZ_LB_CAT", label_Category.Text));
				comboBox_zoneType.Tag = removeLastChar(Localization.localize("CZ_LB_TYPE", label_zoneType.Text));
				textBox_desc.Tag = removeLastChar(Localization.localize("CZ_LB_DESC", label_desc.Text));
				textBox_descLocal.Tag = removeLastChar(Localization.localize("CZ_LB_DESC_LOCAL", label_descLocal.Text));

				but_Zone_Save.Text = Localization.localize("BUT_UPDATE", but_Zone_Save.Text);
				but_Zone_Delete.Text = Localization.localize("BUT_DELETE", but_Zone_Delete.Text);
				but_Zone_Add.Text = Localization.localize("BUT_ADD", but_Zone_Add.Text);
				#endregion

				// REMEMBER TO ADD THE TEXTBOX Tag string for use in validate_pas method
				#region PA System Tab
				groupBox_pas.Text = Localization.localize("CZ_GP_PAS", groupBox_pas.Text);

				// Text
				label_pas_StationNames.Text = Localization.localize("CZ_LB_STATION", label_pas_StationNames.Text);
				label_pas_PriIp.Text = Localization.localize("CZ_LB_PRI_IP", label_pas_PriIp.Text);
				label_pas_SecIp.Text = Localization.localize("CZ_LB_SEC_IP", label_pas_SecIp.Text);
				label_pas_Port.Text = Localization.localize("CZ_LB_PORT_NUM", label_pas_Port.Text);
				label_pas_stnNo.Text = Localization.localize("CZ_LB_STN_NUM", label_pas_stnNo.Text);

				// Tags
				comboBox_pas_StationNames.Tag = removeLastChar(Localization.localize("CZ_LB_STATION", label_pas_StationNames.Text));
				textBox_pas_PriIp.Tag = removeLastChar(Localization.localize("CZ_LB_PRI_IP", label_pas_PriIp.Text));
				textBox_pas_SecIp.Tag = removeLastChar(Localization.localize("CZ_LB_SEC_IP", label_pas_SecIp.Text));
				textBox_pas_Port.Tag = removeLastChar(Localization.localize("CZ_LB_PORT_NUM", label_pas_Port.Text));
				textBox_pas_stnNo.Tag = removeLastChar(Localization.localize("CZ_LB_STN_NUM", label_pas_stnNo.Text));

				but_pas_Update.Text = Localization.localize("BUT_UPDATE", but_pas_Update.Text);
                #endregion

                #region Tab Configure Time
                label_time_station.Text = Localization.localize("LABLE_STATION", label_time_station.Text);
                label_time_weekday.Text = Localization.localize("LABLE_WEEKDAY", label_time_weekday.Text);
                label_peak_start.Text = Localization.localize("LABLE_START_TIME", label_peak_start.Text);
                label_peak_end.Text = Localization.localize("LABLE_END_TIME", label_peak_end.Text);
                label_offpeak_start.Text = Localization.localize("LABLE_START_TIME", label_offpeak_start.Text);
                label_offpeak_end.Text = Localization.localize("LABLE_END_TIME", label_offpeak_end.Text);
                label_night_start.Text = Localization.localize("LABLE_START_TIME", label_night_start.Text);
                label_night_end.Text = Localization.localize("LABLE_END_TIME", label_night_end.Text);
                groupBox_time_peak.Text = Localization.localize("GROUP_PEAK", groupBox_time_peak.Text);
                groupBox_time_offpeak.Text = Localization.localize("GROUP_OFF_PEAK", groupBox_time_offpeak.Text);
                groupBox_time_night.Text = Localization.localize("GROUP_NIGHT", groupBox_time_night.Text);
                button_time_update.Text = Localization.localize("BUT_UPDATE", button_time_update.Text);
                

                #endregion

                CmfDelZone_ = Localization.localize("CMF_DEL_ZONE", CmfDelZone_);

				if (Program.defaultLanguage == "en-US")
				{
					cannotEmpty_ = " " + cannotEmpty_;
					isInvalidIp_ = " " + isInvalidIp_;
					isNotNumeric_ = " " + isNotNumeric_;
					isNotValidRange_ = " " + isNotValidRange_;
				}
			}
		}

		/// <summary>
		/// This method load information of all stations.
		/// </summary>
		/// <returns>bool: true if stations combobox can be loaded, otherwise false</returns>
		/// <exception cref="NoOracleDBException">Database not found exception</exception>
		/// <exception cref="Exception">Handle generic exception</exception>
		/// <since>00.03.01.00</since>
		private void loadStation()
		{
			dbConnEnum dbConn = dbConnEnum.ErrNoConn;

			localSql_ = " SELECT NAME, PKEY FROM LOCATION WHERE PKEY<>0 ORDER BY NAME ";

			stationList_.Clear();

            var queries = new List<SqlQuery> { new SqlQuery { CommandText = localSql_ } };
            DataSet ds = Program.dbLock.FetchData(queries, ref dbConn);

            if (ds.Tables != null || ds.Tables.Count > 0)
            {
                foreach (DataRow retrievedRow in ds.Tables[0].Rows)
                {
                    stationList_.Add(new DisplayValue(retrievedRow[0].ToString(), retrievedRow[1].ToString()));
                }
            }

			// Populate comboBox_station comboBox with station names
			if (stationList_ != null)
			{
				comboBox_stationName.DataSource = stationList_;
				comboBox_stationName.DisplayMember = "displayItem";
				comboBox_stationName.ValueMember = "valueItem";

				comboBox_pas_StationNames.DataSource = stationList_;
				comboBox_pas_StationNames.DisplayMember = "displayItem";
				comboBox_pas_StationNames.ValueMember = "valueItem";
			}

		}

		/// <summary>
		/// Load and populate the Category ComboBox
		/// </summary>
		/// <returns>bool: true if category combobox can be loaded, otherwise false</returns>
		/// <exception cref="NoOracleDBException">Database not found exception</exception>
		/// <exception cref="Exception">Handle generic exception</exception>
		/// <since>00.03.01.00</since>
		private void loadCategory()
		{
			dbConnEnum dbConn = dbConnEnum.ErrNoConn;

			localSql_ = " SELECT DISTINCT ZONE_CAT_ID, ZONE_CAT_NAME, ZONE_CAT_NAME_LOCAL FROM PA_ZONE_CATEGORY ORDER BY ZONE_CAT_ID ASC ";

			categoryList_.Clear();

            var queries = new List<SqlQuery> { new SqlQuery { CommandText = localSql_ } };
            DataSet ds = Program.dbLock.FetchData(queries, ref dbConn);

            if (ds.Tables != null || ds.Tables.Count > 0)
            {
                foreach (DataRow retrievedRow in ds.Tables[0].Rows)
                {
                    if (Program.defaultLanguage == "en-US")
                    {
                        categoryList_.Add(new DisplayValue(retrievedRow[1].ToString(), retrievedRow[0].ToString()));
                    }
                    else
                    {
                        categoryList_.Add(new DisplayValue(retrievedRow[2].ToString(), retrievedRow[0].ToString()));
                    }
                }
            }

			// Populate comboBox_Category comboBox with category names
			if (categoryList_ != null)
			{
				comboBox_Category.DataSource = categoryList_;
				comboBox_Category.DisplayMember = "displayItem";
				comboBox_Category.ValueMember = "valueItem";
			}

		}

		/// <summary>
		/// Load and populate the Type ComboBox
		/// </summary>
		/// <returns>bool: true if type combobox can be loaded, otherwise false</returns>
		/// <exception cref="NoOracleDBException">Database not found exception</exception>
		/// <exception cref="Exception">Handle generic exception</exception>
		/// <since>00.03.01.00</since>
		private void loadType()
		{
			dbConnEnum dbConn = dbConnEnum.ErrNoConn;

			localSql_ = " SELECT DISTINCT ZONE_TYPE_ID, ZONE_TYPE, ZONE_TYPE_LOCAL FROM PA_ZONE_TYPE ORDER BY ZONE_TYPE_ID ASC ";

			typeList_.Clear();

            var queries = new List<SqlQuery> { new SqlQuery { CommandText = localSql_ } };
            DataSet ds = Program.dbLock.FetchData(queries, ref dbConn);

            if (ds.Tables != null || ds.Tables.Count > 0)
            {
                foreach (DataRow retrievedRow in ds.Tables[0].Rows)
                {
                    if (Program.defaultLanguage == "en-US")
                    {
                        typeList_.Add(new DisplayValue(retrievedRow[1].ToString(), retrievedRow[0].ToString()));
                    }
                    else
                    {
                        typeList_.Add(new DisplayValue(retrievedRow[2].ToString(), retrievedRow[0].ToString()));
                    }
                }
            }

			// Populate comboBox_zoneType comboBox with category names
			if (typeList_ != null)
			{
				comboBox_zoneType.DataSource = typeList_;
				comboBox_zoneType.DisplayMember = "displayItem";
				comboBox_zoneType.ValueMember = "valueItem";
			}

		}

		/// <summary>
		/// Load and populate the Host Name ComboBox
		/// </summary>
		/// <returns>bool: true if Host Name combobox can be loaded, otherwise false</returns>
		/// <exception cref="NoOracleDBException">Database not found exception</exception>
		/// <exception cref="Exception">Handle generic exception</exception>
		/// <since>00.03.01.00</since>
		private void loadHostNames()
		{
			dbConnEnum dbConn = dbConnEnum.ErrNoConn;
			List<string> hostName = new List<string>();

			localSql_ = " SELECT DISTINCT HOST_NAME FROM SYS_CLIENT ORDER BY HOST_NAME ASC ";

			hostName.Clear();

            var queries = new List<SqlQuery> { new SqlQuery { CommandText = localSql_ } };
            DataSet ds = Program.dbLock.FetchData(queries, ref dbConn);

            if (ds.Tables != null || ds.Tables.Count > 0)
            {
                foreach (DataRow retrievedRow in ds.Tables[0].Rows)
                {
                    hostName.Add(retrievedRow[0].ToString());
                }
            }

			// Populate comboBox_sc_HostName comboBox with Host Names
			if (hostName != null)
			{
				////comboBox_sc_HostName.DataSource = hostName;
			}

		}

		/// <summary>
		/// Populate the TreeView with Station and Category details
		/// </summary>
		/// <exception cref="NoOracleDBException">Database not found exception</exception>
		/// <exception cref="Exception">Handle generic exception</exception>
		/// <since>00.03.01.00</since>
		private void initTreeViewStationNodes()
		{
			if (stationList_ != null)
			{
				List<string> configuredStnList = loadConfiguredStnList();

				treeView_Stn_Zones.Nodes.Clear();

				foreach (DisplayValue dvStation in stationList_)
				{
					TreeNode stnTreeNode = new TreeNode();

					// Level 1 - Add Station Names to Root
					stnTreeNode.Text = dvStation.displayItem;
					stnTreeNode.Tag = dvStation.valueItem;

					treeView_Stn_Zones.Nodes.Add(stnTreeNode);

					stnIdToTreeNode_.Add(dvStation.valueItem, stnTreeNode);

					// Level 2 - Add dummy "+" sign to station node
					if (configuredStnList.Contains(dvStation.valueItem.ToString()))
					{
						stnTreeNode.Nodes.Add("Dummy");
					}
				}
			}

			treeView_Stn_Zones.SelectedNode = treeView_Stn_Zones.Nodes[0];
			treeView_Stn_Zones.SelectedNode.Collapse();
		}

		/// <summary>
		/// Load a list of stations that has been configured for use
		/// </summary>
		/// <returns>List of configured station locId</returns>
		/// <since>00.03.18.02</since>
		private List<string> loadConfiguredStnList()
		{
			dbConnEnum dbConn = dbConnEnum.ErrNoConn;
			List<string> configuredStnList = new List<string>();

			localSql_ = " SELECT DISTINCT LOCATION_ID FROM PA_ZONES ORDER BY LOCATION_ID ";

            var queries = new List<SqlQuery> { new SqlQuery { CommandText = localSql_ } };
            DataSet ds = Program.dbLock.FetchData(queries, ref dbConn);

            if (ds.Tables != null || ds.Tables.Count > 0)
            {
                foreach (DataRow retrievedRow in ds.Tables[0].Rows)
                {
                    configuredStnList.Add(retrievedRow[0].ToString());
                }
            }

			return configuredStnList;
		}

		/// <summary>
		/// Show the correct station
		/// </summary>
		/// <param name="stnLocId">Station Location Id (PK)</param>
		/// <since>00.03.01.00</since>
		private void selectCbStation(string stnLocId)
		{
			// Nothing to validate : stnLocId
			for (int i = 0; i < stationList_.Count; i++)
			{
				string val = ((DisplayValue)stationList_[i]).valueItem;

				if (val.Equals(stnLocId))
				{
					comboBox_stationName.Text = ((DisplayValue)stationList_[i]).displayItem;
					comboBox_stationName.SelectedIndex = i;
					break;
				}
			}
		}

		/// <summary>
		/// Show the correct Zone Category on HMI
		/// </summary>
		/// <param name="catId">Zone Category ID</param>
		/// <since>00.03.01.00</since>
		private void selectCbCategory(string catId)
		{
			// Nothing to validate : catId
			for (int i = 0; i < categoryList_.Count; i++)
			{
				string val = ((DisplayValue)categoryList_[i]).displayItem;

				if (val.Equals(catId))
				{
					comboBox_Category.Text = ((DisplayValue)categoryList_[i]).displayItem;
					comboBox_Category.SelectedIndex = i;
					break;
				}
			}
		}

		/// <summary>
		/// Show the correct Zone Type on HMI
		/// </summary>
		/// <param name="typeId">Zone Type ID</param>
		/// <since>00.03.01.00</since>
		private void selectCbType(string typeId)
		{
			// Nothing to validate : typeId
			for (int i = 0; i < typeList_.Count; i++)
			{
				string val = ((DisplayValue)typeList_[i]).valueItem;

				if (val.Equals(typeId))
				{
					comboBox_zoneType.Text = ((DisplayValue)typeList_[i]).displayItem;
					comboBox_zoneType.SelectedIndex = i;
					break;
				}
			}
		}

		/// <summary>
		/// Operator clicks on the Save Button after editing the Deu Details
		/// </summary>
		/// <param name="sender">Control that raised the event</param>
		/// <param name="e">Additional information about the event that was raised</param>
		/// <exception cref="NoOracleDBException">Database not found exception</exception>
		/// <exception cref="Exception">Handle generic exception</exception>
		/// <since>00.03.01.00</since>
		private void but_Zone_Save_Click(object sender, EventArgs e)
		{
			// Nothing to validate : sender, e
			string[] eventParam = null;
			string[] eventParamLocal = null;
			dbConnEnum dbConn = dbConnEnum.ErrNoConn;

			try
			{
				DisplayValue dvStation = new DisplayValue();
				DisplayValue dvCatName = new DisplayValue();
				DisplayValue dvTypeName = new DisplayValue();

				if (!validateUserInputs())
				{
					return;
				}

				dvStation = (DisplayValue)stationList_[comboBox_stationName.SelectedIndex];
				dvCatName = (DisplayValue)categoryList_[comboBox_Category.SelectedIndex];
				dvTypeName = (DisplayValue)typeList_[comboBox_zoneType.SelectedIndex];

				localSql_ = " UPDATE PA_ZONES SET ZONE_DESC='" + parseSpecialCharacters(textBox_desc.Text) + "'" +
							" , ZONE_DESC_LOCAL='" + parseSpecialCharacters(textBox_descLocal.Text) + "'" +
							" , ZONE_CAT_ID=" + dvCatName.valueItem +
							" , ZONE_TYPE_ID=" + dvTypeName.valueItem +
							" WHERE LOCATION_ID=" + dvStation.valueItem +
							" AND ZONE_NO=" + int.Parse(textBox_zoneNo.Text);
                var queries = new List<SqlQuery> { new SqlQuery { CommandText = localSql_ } };
                if (Program.dbLock.ExcuteNoneResultQuery(queries, ref dbConn) < 1)
                {
                    Program.WriteEventLog("[ERROR] UPDATE PA_ZONES", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
                    return;
                }

                Program.WriteEventLog("[INFO] Update Zone Successful.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

				eventParam = new string[] { "", "", textBox_zoneNo.Text, dvStation.displayItem };
				eventParamLocal = new string[] { "", "", textBox_zoneNo.Text, dvStation.displayItem };
				Program.logEvent(EnumType.PaEvent.PaZoneEdit, eventParam, eventParamLocal);

				Program.MessageBox_Normal(updateZonePass_, sysErr_, this);

				isSaveZoneCReq_ = false;

				TreeViewEventArgs tvEventArgs = new TreeViewEventArgs(stnIdToTreeNode_[dvStation.valueItem]);
				treeView_Stn_Zone_AfterSelect(stnIdToTreeNode_[dvStation.valueItem], tvEventArgs);
			}
			catch (NoOracleDBException ex)
			{
				Program.WriteEventLog("[ERROR] [NoOracleDBException] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				Program.MessageBox_Error(updateZoneFail_ + "\n(" + Program.MB_NoDBConn + ")", sysErr_);
			}
			catch (Exception ex)
			{
				Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				Program.MessageBox_Error(updateZoneFail_, sysErr_);
			}
		}

		/// <summary>
		/// Operator clicks on the Add Button after editing the Deu Details
		/// </summary>
		/// <param name="sender">Control that raised the event</param>
		/// <param name="e">Additional information about the event that was raised</param>
		/// <exception cref="NoOracleDBException">Database not found exception</exception>
		/// <exception cref="Exception">Handle generic exception</exception>
		/// <since>00.03.01.00</since>
		private void but_Zone_Add_Click(object sender, EventArgs e)
		{
			// Nothing to validate : sender, e
			string[] eventParam = null;
			string[] eventParamLocal = null;
			dbConnEnum dbConn = dbConnEnum.ErrNoConn;

			DisplayValue dvStation = new DisplayValue();
			DisplayValue dvCatName = new DisplayValue();
			DisplayValue dvTypeName = new DisplayValue();

			try
			{
				if (!validateUserInputs())
				{
					return;
				}

				dvStation = (DisplayValue)stationList_[comboBox_stationName.SelectedIndex];
				dvCatName = (DisplayValue)categoryList_[comboBox_Category.SelectedIndex];
				dvTypeName = (DisplayValue)typeList_[comboBox_zoneType.SelectedIndex];

				if (zoneExists(dvStation.valueItem, textBox_zoneNo.Text))
				{
					Program.MessageBox_Error(zoneExists_, sysErr_);
					return;
				}

				localSql_ = " INSERT INTO PA_ZONES " +
								" (LOCATION_ID, ZONE_NO, ZONE_DESC, ZONE_DESC_LOCAL, ZONE_CAT_ID, ZONE_TYPE_ID) " +
								" VALUES(" + dvStation.valueItem +
								" , " + int.Parse(textBox_zoneNo.Text) +
								" , '" + parseSpecialCharacters(textBox_desc.Text) + "' " +
								" , '" + parseSpecialCharacters(textBox_descLocal.Text) + "' " +
								" , " + dvCatName.valueItem +
								" , " + dvTypeName.valueItem +
								" ) ";

                var queries = new List<SqlQuery> { new SqlQuery { CommandText = localSql_ } };
                if (Program.dbLock.ExcuteNoneResultQuery(queries, ref dbConn) < 1)
                {
                    Program.WriteEventLog("[ERROR] INSERT INTO PA_ZONES", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
                    return;
                }

                Program.WriteEventLog("[INFO] Add Zone successful!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

				eventParam = new string[] { "", "", textBox_zoneNo.Text, dvStation.displayItem };
				eventParamLocal = new string[] { "", "", textBox_zoneNo.Text, dvStation.displayItem };
				Program.logEvent(EnumType.PaEvent.PaZoneAdd, eventParam, eventParamLocal);

				Program.MessageBox_Normal(addZonePass_, sysErr_, this);

				isSaveZoneCReq_ = false;

				TreeViewEventArgs tvEventArgs = new TreeViewEventArgs(stnIdToTreeNode_[dvStation.valueItem]);
				treeView_Stn_Zone_AfterSelect(stnIdToTreeNode_[dvStation.valueItem], tvEventArgs);
			}
			catch (NoOracleDBException ex)
			{
				Program.WriteEventLog("[ERROR] [NoOracleDBException] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				Program.MessageBox_Error(addZoneFail_ + "\n(" + Program.MB_NoDBConn + ")", sysErr_);
			}
			catch (Exception ex)
			{
				Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				Program.MessageBox_Error(addZoneFail_, sysErr_);
			}
		}

		/// <summary>
		/// Operator clicks on the Delete Button to delete the selected display unit away
		/// </summary>
		/// <param name="sender">Control that raised the event</param>
		/// <param name="e">Additional information about the event that was raised</param>
		/// <exception cref="NoOracleDBException">Database not found exception</exception>
		/// <exception cref="Exception">Handle generic exception</exception>
		/// <since>00.03.01.00</since>
		private void but_Zone_Delete_Click(object sender, EventArgs e)
		{
			// Nothing to validate : sender, e
			DisplayValue dvStation = new DisplayValue();
			DisplayValue dvCatName = new DisplayValue();
			DisplayValue dvTypeName = new DisplayValue();

			string[] eventParam = null;
			string[] eventParamLocal = null;
			dbConnEnum dbConn = dbConnEnum.ErrNoConn;

			try
			{
				dvStation = (DisplayValue)stationList_[comboBox_stationName.SelectedIndex];
				dvCatName = (DisplayValue)categoryList_[comboBox_Category.SelectedIndex];
				dvTypeName = (DisplayValue)typeList_[comboBox_zoneType.SelectedIndex];

				if (zoneExists(dvStation.valueItem, textBox_zoneNo.Text))
				{
					// Zone exists, popup delete confirmation window
					if (STEMessageBox.Show(CmfDelZone_, sysErr_, Program.defaultLanguage, MsgBoxButton.YesNo, MessageBoxIcon.Question) == DialogResult.No)
					{
						return;
					}
					else
					{
						localSql_ = " DELETE FROM PA_ZONES " +
									" WHERE LOCATION_ID=" + dvStation.valueItem +
									" AND ZONE_NO=" + textBox_zoneNo.Text;
                        var queries = new List<SqlQuery> { new SqlQuery { CommandText = localSql_ } };
                        if (Program.dbLock.ExcuteNoneResultQuery(queries, ref dbConn) < 1)
                        {
                            Program.WriteEventLog("[ERROR] DELETE FROM PA_ZONES", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
                            Program.MessageBox_Error(deleteZoneFail_, sysErr_);
                            return;
                        }                      
						else
                        {
							Program.WriteEventLog("[INFO] Delete zone successful.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

							eventParam = new string[] { "", "", textBox_zoneNo.Text, dvStation.displayItem };
							eventParamLocal = new string[] { "", "", textBox_zoneNo.Text, dvStation.displayItem };
							Program.logEvent(EnumType.PaEvent.PaZoneDelete, eventParam, eventParamLocal);

							//Program.MessageBox_Normal(deleteZonePass_, sysErr_);

							isSaveZoneCReq_ = false;

							TreeViewEventArgs tvEventArgs = new TreeViewEventArgs(stnIdToTreeNode_[dvStation.valueItem]);
							treeView_Stn_Zone_AfterSelect(stnIdToTreeNode_[dvStation.valueItem], tvEventArgs);
						}
					}
				}
				else
				{
					// Zone doesn't exists, prompt error
					Program.MessageBox_Error(mbZoneNotExists_, sysErr_);
				}
			}
			catch (NoOracleDBException ex)
			{
				Program.WriteEventLog("[ERROR] [NoOracleDBException] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				Program.MessageBox_Error(deleteZoneFail_ + "\n(" + Program.MB_NoDBConn + ")", sysErr_);
			}
			catch (Exception ex)
			{
				Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				Program.MessageBox_Error(deleteZoneFail_, sysErr_);
			}
		}

		/// <summary>
		/// Method invoked when the form is closing.
		/// </summary>
		/// <param name="sender">Control that raised the event</param>
		/// <param name="e">Additional information about the event that was raised</param>
		/// <exception cref="Exception">Handle generic exception</exception>
		/// <since>00.03.01.00</since>
		private void ConfigZones_FormClosing(object sender, FormClosingEventArgs e)
		{
			// Nothing to validate : sender, e
			try
			{
				DialogResult operatorResponse = DialogResult.None;

				if ((tabControl_Config.SelectedIndex == 0 && isSaveZoneCReq_ && zoneAddFormShown_ == true) ||
					(tabControl_Config.SelectedIndex == 0 && isSaveZoneCReq_ && zoneAddFormShown_ == false && changeInZoneTab()) ||
					(tabControl_Config.SelectedIndex == 1 && changeInPasTab()) ||
					(tabControl_Config.SelectedIndex == 2 && isSavePaScCReq_))
				{
					operatorResponse = STEMessageBox.Show(Program.saveBeforeExit, sysErr_, Program.defaultLanguage, MsgBoxButton.YesNoCancel, MessageBoxIcon.Question);

					if (operatorResponse == DialogResult.Yes)
					{
						if (tabControl_Config.SelectedIndex == 0)
						{
							if (zoneAddFormShown_)
							{
								but_Zone_Add_Click(new object(), e);
							}
							else
							{
								but_Zone_Save_Click(new object(), e);
							}
						}
						else if (tabControl_Config.SelectedIndex == 1)
						{
							but_pas_Update_Click(new object(), e);
						}
						else
						{
							// do nth
						}
					}
					else if (operatorResponse == DialogResult.Cancel)
					{
						e.Cancel = true;
						return;
					}
					else
					{
						// Just quit, without saving
					}
				}

				// Only prompt if the form closing event is not being cancelled due to errors
				if (!e.Cancel)
				{
					Program.MessageBox_Normal(mbLaunch_, sysErr_, this);
				}

				CleanUp();
			}
			catch (Exception ex)
			{
				Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
			}
		}

		/// <summary>
		/// Method invoked after a tree node is selected
		/// </summary>
		/// <param name="sender">Control that raised the event</param>
		/// <param name="e">Additional information about the event that was raised</param>
		/// <exception cref="NoOracleDBException">Handle No Oracle DB Exception</exception>
		/// <exception cref="Exception">Handle generic exception</exception>
		/// <since>00.03.01.00</since>
		private void treeView_Stn_Zone_AfterSelect(object sender, TreeViewEventArgs e)
		{
			// Nothing to validate : sender, e
			try
			{
				if (e.Node.Level == 0)
				{
					selectCbStation(e.Node.Tag.ToString());

					showZoneAddForm();
					populateStnCategory(ref e);
				}
				else if (e.Node.Level == 1)
				{
					selectCbStation(e.Node.Parent.Tag.ToString());

					showZoneAddForm();
					populateStnCatZones(ref e);
				}
				else if (e.Node.Level == 2)
				{
					displayZoneInfo(ref e);
					saveCurrZoneTab();
				}
				else
				{
					// Do nothing
				}

				isSaveZoneCReq_ = false;
			}
			catch (Exception ex)
			{
				Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
			}
		}

		/// <summary>
		/// Show the HMI components related to the Zone add function
		/// </summary>
		/// <since>00.03.18.02</since>
		private void showZoneAddForm()
		{
			groupBox_Config.Text = gbAddZone_;

			textBox_zoneNo.Text = "";
			comboBox_Category.SelectedIndex = 0;
			comboBox_zoneType.SelectedIndex = 0;

			textBox_desc.Text = "";
			textBox_descLocal.Text = "";

			// Can enter unit number - Check that number doesn't exists!
			textBox_zoneNo.Enabled = true;

			but_Zone_Add.Visible = true;

			but_Zone_Save.Visible = false;
			but_Zone_Delete.Visible = false;

			zoneAddFormShown_ = true;
		}

		/// <summary>
		/// Show the HMI components related to the Zone edit function
		/// </summary>
		/// <returns></returns>
		/// <since>00.03.18.02</since>
		private void showZoneEditForm()
		{
			groupBox_Config.Text = gbModifyZone_;

			// Can only change category, type 
			textBox_zoneNo.Enabled = false;

			but_Zone_Add.Visible = false;

			but_Zone_Save.Visible = true;
			but_Zone_Delete.Visible = true;

			zoneAddFormShown_ = false;
		}

		/// <summary>
		/// Display the selected deu unit information
		/// </summary>
		/// <param name="e">Additional information about the event that was raised</param>
		/// <since>00.03.18.02</since>
		private void displayZoneInfo(ref TreeViewEventArgs e)
		{
			// Nothing to validate : e
			dbConnEnum dbConn = dbConnEnum.ErrNoConn;
			string locationId = e.Node.Parent.Parent.Tag.ToString();

			selectCbStation(locationId);
			textBox_zoneNo.Text = e.Node.Text;
			selectCbCategory(e.Node.Parent.Text);
			selectCbType(Translater.Get_Zone_Type(e.Node.Parent.Parent.Tag.ToString(), e.Node.Text));

			textBox_desc.Text = "";
			textBox_descLocal.Text = "";

			localSql_ = " SELECT ZONE_DESC, ZONE_DESC_LOCAL FROM PA_ZONES WHERE LOCATION_ID=" + locationId +
						" AND ZONE_NO=" + textBox_zoneNo.Text;

            var queries = new List<SqlQuery> { new SqlQuery { CommandText = localSql_ } };
            DataSet ds = Program.dbLock.FetchData(queries, ref dbConn);

            if (ds.Tables != null || ds.Tables.Count > 0)
            {
                foreach (DataRow retrievedRow in ds.Tables[0].Rows)
                {
                    textBox_desc.Text = retrievedRow[0].ToString();
                    textBox_descLocal.Text = retrievedRow[1].ToString();
                    break;
                }
            }

			showZoneEditForm();
		}

		/// <summary>
		/// Populate the zones within the station category
		/// </summary>
		/// <param name="e">Additional information about the event that was raised</param>
		/// <since>00.03.18.02</since>
		private void populateStnCatZones(ref TreeViewEventArgs e)
		{
			// Nothing to validate : e
			// Category level - Load display units list
			string stnId = e.Node.Parent.Tag.ToString();
			string catId = e.Node.Tag.ToString();

			List<string> zoneList = load_StnCatZoneList(stnId, catId);

			// Remove "dummy" node
			e.Node.Nodes.Clear();

			// Populate node with display units number
			foreach (string zoneNo in zoneList)
			{
				e.Node.Nodes.Add(zoneNo);
			}

			e.Node.Expand();
		}

		/// <summary>
		/// Populate the Station's category
		/// </summary>
		/// <param name="e">Additional information about the event that was raised</param>
		/// <since>00.03.18.02</since>
		private void populateStnCategory(ref TreeViewEventArgs e)
		{
			// Nothing to validate : e

			// Station level - Load category name list
			string stnId = e.Node.Tag.ToString();

			// Map catId to catName
			Dictionary<string, string> catIdToNameDic = new Dictionary<string, string>();
			catIdToNameDic = load_StnCatList(stnId);

			// Remove "dummy" node
			e.Node.Nodes.Clear();

			// Populate category node with display units number
			foreach (KeyValuePair<string, string> catKeyValue in catIdToNameDic)
			{
				string catId = catKeyValue.Key;
				string catName = catKeyValue.Value;
				TreeNode catNode = new TreeNode();

				catNode.Text = catName;
				catNode.Tag = catId;

				populateZones(stnId, catId, ref catNode);

				e.Node.Nodes.Add(catNode);
			}

			e.Node.Expand();
		}

		/// <summary>
		/// Load all category names in the selected station
		/// </summary>
		/// <param name="stnId">Station ID</param>
		/// <returns>A list of category names in the selected station</returns>
		/// <since>00.01.09.00</since>
		private Dictionary<string, string> load_StnCatList(string stnId)
		{
			// Nothing to validate : stnId

			dbConnEnum dbConn = dbConnEnum.ErrNoConn;
			Dictionary<string, string> catIdNameDic = new Dictionary<string, string>();

			if (Program.defaultLanguage == "en-US")
			{
				localSql_ = " SELECT DISTINCT PZ.ZONE_CAT_ID, PC.ZONE_CAT_NAME " +
							" FROM PA_ZONES PZ, PA_ZONE_CATEGORY PC, LOCATION SL " +
							" WHERE SL.PKEY=PZ.LOCATION_ID AND SL.PKEY='" + stnId + "' AND PZ.ZONE_CAT_ID=PC.ZONE_CAT_ID ";
			}
			else
			{
				localSql_ = " SELECT DISTINCT PZ.ZONE_CAT_ID, PC.ZONE_CAT_NAME_LOCAL " +
							" FROM PA_ZONES PZ, PA_ZONE_CATEGORY PC, LOCATION SL " +
							" WHERE SL.PKEY=PZ.LOCATION_ID AND SL.PKEY='" + stnId + "' AND PZ.ZONE_CAT_ID=PC.ZONE_CAT_ID ";
			}

            var queries = new List<SqlQuery> { new SqlQuery { CommandText = localSql_ } };
            DataSet ds = Program.dbLock.FetchData(queries, ref dbConn);

            if (ds.Tables != null || ds.Tables.Count > 0)
            {
                foreach (DataRow retrievedRow in ds.Tables[0].Rows)
                {
                    string catId = retrievedRow[0].ToString();
                    string catName = retrievedRow[1].ToString();
                    catIdNameDic.Add(catId, catName);
                }
            }

			return catIdNameDic;
		}

		/// <summary>
		/// Populate the Zones given the station id, category id and category node reference
		/// </summary>
		/// <param name="stnId">Station ID</param>
		/// <param name="catId">Category ID</param>
		/// <param name="catNode">Reference to tree node</param>
		/// <since>00.03.18.02</since>
		private void populateZones(string stnId, string catId, ref TreeNode catNode)
		{
			// Nothing to validate : stnId, catId, catNode

			List<string> zoneList = load_StnCatZoneList(stnId, catId);

			// Populate node with display units number
			foreach (string zoneNo in zoneList)
			{
				catNode.Nodes.Add(zoneNo);
			}

			catNode.Expand();
		}

		/// <summary>
		/// Load all zones in the selected station and category
		/// </summary>
		/// <param name="stnId">Station ID</param>
		/// <param name="catId">Category ID</param>
		/// <returns>A list of zones in the selected station and category</returns>
		/// <since>00.01.09.00</since>
		private List<string> load_StnCatZoneList(string stnId, string catId)
		{
			// Nothing to validate : stnId, catId

			dbConnEnum dbConn = dbConnEnum.ErrNoConn;
			List<string> deuList = new List<string>();

			localSql_ = " SELECT ZONE_NO FROM PA_ZONES WHERE LOCATION_ID=" + stnId + " AND ZONE_CAT_ID=" + catId + " ORDER BY ZONE_NO ";
            var queries = new List<SqlQuery> { new SqlQuery { CommandText = localSql_ } };
            DataSet ds = Program.dbLock.FetchData(queries, ref dbConn);

            if (ds.Tables != null || ds.Tables.Count > 0)
            {
                foreach (DataRow retrievedRow in ds.Tables[0].Rows)
                {
                    deuList.Add(retrievedRow[0].ToString());
                }
            }

			return deuList;
		}

		/// <summary>
		/// Method invoked after a tree node is expanded
		/// </summary>
		/// <param name="sender">Control that raised the event</param>
		/// <param name="e">Additional information about the event that was raised</param>
		/// <exception cref="NoOracleDBException">Database not found exception</exception>
		/// <exception cref="Exception">Handle generic exception</exception>
		/// <since>00.03.01.00</since>
		private void treeView_Stn_Zone_AfterExpand(object sender, TreeViewEventArgs e)
		{
			// Nothing to validate : sender, e
			try
			{
				if (e.Node.Level == 0)
				{
					populateStnCategory(ref e);
				}
				else if (e.Node.Level == 1)
				{
					populateStnCatZones(ref e);
				}
			}
			catch (NoOracleDBException ex)
			{
				Program.WriteEventLog("[ERROR] [NoOracleDBException] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
			}
			catch (Exception ex)
			{
				Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
			}

		}

		/// <summary>
		/// Check if user inputs are correct or not
		/// </summary>
		/// <returns>bool: true if all inputs are correct, otherwise false</returns>
		/// <since>00.03.01.00</since>
		private bool validateUserInputs()
		{
			//int intVal = 0;
			string errorMsg = "";

			if (textBox_zoneNo.Text.Trim() == "")
			{
				Program.MessageBox_Error(emptyZoneNo_, sysErr_);
				return false;
			}

			if (!IsNumeric(textBox_zoneNo.Text))
			{
				errorMsg = textBox_zoneNo.Tag.ToString() + isNotNumeric_;
				Program.MessageBox_Error(errorMsg, sysErr_);
				return false;
			}

			if (Convert.ToInt32(textBox_zoneNo.Text) < 1 || Convert.ToInt32(textBox_zoneNo.Text) > Program.MAXZONEID)
			{
				errorMsg = textBox_zoneNo.Tag.ToString() + isNotValidRange_;
				Program.MessageBox_Error(errorMsg, sysErr_);
				return false;
			}

			return true;
		}

		/// <summary>
		/// Check if user inputs are correct or not for the PA System Tab
		/// </summary>
		/// <returns>bool: true if all inputs are correct, otherwise false</returns>
		/// <since>00.03.01.00</since>
		private bool validatePasTab()
		{
			string errorMsg = "";

			if (!string.IsNullOrEmpty(textBox_pas_PriIp.Text) && !IsValidIP(textBox_pas_PriIp.Text))
			{
				errorMsg = textBox_pas_PriIp.Tag.ToString() + isInvalidIp_;
				Program.MessageBox_Error(errorMsg, sysErr_);
				return false;
			}

			if (!string.IsNullOrEmpty(textBox_pas_SecIp.Text) && !IsValidIP(textBox_pas_SecIp.Text))
			{
				errorMsg = textBox_pas_SecIp.Tag.ToString() + isInvalidIp_;
				Program.MessageBox_Error(errorMsg, sysErr_);
				return false;
			}

			if (!string.IsNullOrEmpty(textBox_pas_Port.Text) && !IsNumeric(textBox_pas_Port.Text))
			{
				errorMsg = textBox_pas_Port.Tag.ToString() + isNotNumeric_;
				Program.MessageBox_Error(errorMsg, sysErr_);
				return false;
			}

			return true;
		}


		/// <summary>
		/// Check if the current entered zone unit record exists
		/// </summary>
		/// <param name="stnLocId">Station Location Number</param>
		/// <param name="stnZoneNo">Station Zone Unit Number</param>
		/// <returns>bool: true if record exists, otherwise false</returns>
		/// <exception cref="NoOracleDBException">Database not found exception</exception>
		/// <exception cref="Exception">Handle generic exception</exception>
		/// <since>00.03.01.00</since>
		private bool zoneExists(string stnLocId, string stnZoneNo)
		{
			// Nothing to validate : stnLocId, stnZoneNo
			dbConnEnum dbConn = dbConnEnum.ErrNoConn;
			bool zoneExists = false;

			localSql_ = " SELECT COUNT(*) FROM PA_ZONES WHERE LOCATION_ID=" + stnLocId +
						" AND ZONE_NO=" + stnZoneNo;
            var queries = new List<SqlQuery> { new SqlQuery { CommandText = localSql_ } };
            DataSet ds = Program.dbLock.FetchData(queries, ref dbConn);

            if (ds.Tables != null || ds.Tables.Count > 0)
            {
                foreach (DataRow retrievedRow in ds.Tables[0].Rows)
                {
                    zoneExists = int.Parse(retrievedRow[0].ToString()) > 0;
                    break;
                }
            }

			return zoneExists;
		}

        private bool StationWeekdayExists(int stationId, int weekday)
        {
            dbConnEnum dbConn = dbConnEnum.ErrNoConn;
            bool exists = false;
            try
            {
                localSql_ = $"SELECT COUNT(*) FROM PA_STATION_SCHEDULE WHERE STATION_ID={stationId} AND WEEK_DAY={weekday}";

                var queries = new List<SqlQuery> { new SqlQuery { CommandText = localSql_ } };
                DataSet ds = Program.dbLock.FetchData(queries, ref dbConn);

                if (ds.Tables != null || ds.Tables.Count > 0)
                {
                    foreach (DataRow retrievedRow in ds.Tables[0].Rows)
                    {
                        exists = int.Parse(retrievedRow[0].ToString()) > 0;
                        break;
                    }
                }
            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }

            return exists;
        }

        private void saveStationWeekdaySchedule(StationWeekdaySchedule schedule)
        {
            try
            {
                dbConnEnum dbConn = dbConnEnum.ErrNoConn;

                localSql_ = $@"
                INSERT INTO pa_station_schedule
                (
                    STATION_ID,
                    WEEK_DAY,
                    PEAK_START,
                    PEAK_END,
                    OFFPEAK_START,
                    OFFPEAK_END,
                    NIGHT_START,
                    NIGHT_END
                )
                VALUES
                (
                    {schedule.StationId},
                    {schedule.WeekDay},
                    '{schedule.PeakStart}',
                    '{schedule.PeakEnd}',
                    '{schedule.OffPeakStart}',
                    '{schedule.OffPeakEnd}',
                    '{schedule.NightStart}',
                    '{schedule.NightEnd}'
                )
                ON DUPLICATE KEY UPDATE
                    PEAK_START = VALUES(PEAK_START),
                    PEAK_END = VALUES(PEAK_END),
                    OFFPEAK_START = VALUES(OFFPEAK_START),
                    OFFPEAK_END = VALUES(OFFPEAK_END),
                    NIGHT_START = VALUES(NIGHT_START),
                    NIGHT_END = VALUES(NIGHT_END);";

                var queries = new List<SqlQuery> { new SqlQuery { CommandText = localSql_ } };

                if (Program.dbLock.ExcuteNoneResultQuery(queries, ref dbConn) < 1)
                {
                    Program.WriteEventLog("[ERROR] INSERT INTO pa_station_schedule", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
                }
            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }
        }

        private StationWeekdaySchedule loadStationWeekdaySchedule(int stationId, int weekday)
        {
            try
            {
                dbConnEnum dbConn = dbConnEnum.ErrNoConn;
                string localSql_ = $@"  
                                    SELECT
                                    STATION_ID,
                                    WEEK_DAY,
                                    PEAK_START,
                                    PEAK_END,
                                    OFFPEAK_START,
                                    OFFPEAK_END,
                                    NIGHT_START,
                                    NIGHT_END
                                    FROM pa_station_schedule
                                    WHERE STATION_ID = {stationId}
                                    AND WEEK_DAY = {weekday};";
                var queries = new List<SqlQuery> { new SqlQuery { CommandText = localSql_ } };
                DataSet ds = Program.dbLock.FetchData(queries, ref dbConn);

                if (ds.Tables != null || ds.Tables.Count > 0 && ds.Tables[0].Rows.Count > 0)
                {
                    var retrievedRow = ds.Tables[0].Rows[0];
                    return new StationWeekdaySchedule
                    {
                        StationId = stationId,
                        WeekDay = weekday,

                        PeakStart = retrievedRow["PEAK_START"]?.ToString(),
                        PeakEnd = retrievedRow["PEAK_END"]?.ToString(),

                        OffPeakStart = retrievedRow["OFFPEAK_START"]?.ToString(),
                        OffPeakEnd = retrievedRow["OFFPEAK_END"]?.ToString(),

                        NightStart = retrievedRow["NIGHT_START"]?.ToString(),
                        NightEnd = retrievedRow["NIGHT_END"]?.ToString()
                    };
                }
            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }
            return null;
        }

        /// <summary>
        /// Check if the current entered location record exists
        /// </summary>
        /// <param name="stnLocId">Station Location Number</param>
        /// <returns>bool: true if record exists, otherwise false</returns>
        /// <exception cref="NoOracleDBException">Database not found exception</exception>
        /// <exception cref="Exception">Handle generic exception</exception>
        /// <since>00.03.01.00</since>
        private bool configExists(string stnLocId)
		{
			// Nothing to validate : stnLocId, stnZoneNo
			dbConnEnum dbConn = dbConnEnum.ErrNoConn;
			bool recExists = false;

			localSql_ = " SELECT COUNT(*) FROM PA_CONFIG WHERE LOCATION_ID=" + stnLocId;
            var queries = new List<SqlQuery> { new SqlQuery { CommandText = localSql_ } };
            DataSet ds = Program.dbLock.FetchData(queries, ref dbConn);

            if (ds.Tables != null || ds.Tables.Count > 0)
            {
                foreach (DataRow retrievedRow in ds.Tables[0].Rows)
                {
                    recExists = int.Parse(retrievedRow[0].ToString()) > 0;
                    break;
                }
            }

			return recExists;
		}

		/// <summary>
		/// Check if the current entered location record exists
		/// </summary>
		/// <param name="stnLocId">Station Location Number</param>
		/// <param name="hostName">Hostname of the MFT in the station</param>
		/// <returns>bool: true if record exists, otherwise false</returns>
		/// <exception cref="NoOracleDBException">Database not found exception</exception>
		/// <exception cref="Exception">Handle generic exception</exception>
		/// <since>00.03.01.00</since>
		private bool settingExists(string stnLocId, string hostName)
		{
			// Nothing to validate : stnLocId, hostName
			dbConnEnum dbConn = dbConnEnum.ErrNoConn;
			bool recExists = false;

			localSql_ = " SELECT COUNT(*) FROM PA_SETTING WHERE LOCATION_ID=" + stnLocId + " AND HOST_NAME='" + hostName + "' ";
            var queries = new List<SqlQuery> { new SqlQuery { CommandText = localSql_ } };
            DataSet ds = Program.dbLock.FetchData(queries, ref dbConn);

            if (ds.Tables != null || ds.Tables.Count > 0)
            {
                foreach (DataRow retrievedRow in ds.Tables[0].Rows)
                {
                    recExists = int.Parse(retrievedRow[0].ToString()) > 0;
                    break;
                }
            }

			return recExists;
		}

		#region PA System Tab
		/// <summary>
		/// PA System Tab - Operator changed selected Station Names
		/// </summary>
		/// <param name="sender">Control that raised the event</param>
		/// <param name="e">Additional information about the event that was raised</param>
		/// <exception cref="NoOracleDBException">Handle No Oracle DB Exception</exception>
		/// <exception cref="Exception">Handle generic exception</exception>
		/// <since>00.03.01.00</since>
		private void comboBox_pas_StationNames_SelectedValueChanged(object sender, EventArgs e)
		{
			// Nothing to validate : sender, e
			dbConnEnum dbConn = dbConnEnum.ErrNoConn;
			DisplayValue dvStation = new DisplayValue();

			try
			{
				dvStation = (DisplayValue)stationList_[comboBox_pas_StationNames.SelectedIndex];

				localSql_ = " SELECT STATION_NO, PRI_IP, SEC_IP, PORT_NO FROM PA_CONFIG WHERE LOCATION_ID=" + dvStation.valueItem;

				textBox_pas_stnNo.Text = "";
				textBox_pas_PriIp.Text = "";
				textBox_pas_SecIp.Text = "";
				textBox_pas_Port.Text = "";

                var queries = new List<SqlQuery> { new SqlQuery { CommandText = localSql_ } };
                DataSet ds = Program.dbLock.FetchData(queries, ref dbConn);

                if (ds.Tables != null || ds.Tables.Count > 0)
                {
                    foreach (DataRow retrievedRow in ds.Tables[0].Rows)
                    {
                        textBox_pas_stnNo.Text = retrievedRow[0].ToString();
                        textBox_pas_PriIp.Text = retrievedRow[1].ToString();
                        textBox_pas_SecIp.Text = retrievedRow[2].ToString();
                        textBox_pas_Port.Text = retrievedRow[3].ToString();
                        break;
                    }
                }

				saveCurrPasTab();
			}
			catch (NoOracleDBException ex)
			{
				Program.WriteEventLog("[ERROR] [NoOracleDBException] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
			}
			catch (Exception ex)
			{
				Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
			}
		}

		/// <summary>
		/// PA System Tab - Operator clicks on the Ok Button
		/// </summary>
		/// <param name="sender">Control that raised the event</param>
		/// <param name="e">Additional information about the event that was raised</param>
		/// <exception cref="NoOracleDBException">Handle No Oracle DB Exception</exception>
		/// <exception cref="Exception">Handle generic exception</exception>
		/// <since>00.03.01.00</since>
		private void but_pas_Update_Click(object sender, EventArgs e)
		{
			string[] eventParam = null;
			string[] eventParamLocal = null;
			dbConnEnum dbConn = dbConnEnum.ErrNoConn;
			DisplayValue dvStation = new DisplayValue();

			try
			{
				if (!validatePasTab())
				{
					return;
				}

				dvStation = (DisplayValue)stationList_[comboBox_pas_StationNames.SelectedIndex];

				if (configExists(dvStation.valueItem))
				{
					// Update as config exists
					localSql_ = " UPDATE PA_CONFIG SET PRI_IP='" + textBox_pas_PriIp.Text + "'" +
							" , SEC_IP='" + textBox_pas_SecIp.Text + "'" +
							" , PORT_NO=" + (string.IsNullOrEmpty(textBox_pas_Port.Text) ? "NULL" : textBox_pas_Port.Text) +
							" , STATION_NO=" + textBox_pas_stnNo.Text +
							" WHERE LOCATION_ID=" + dvStation.valueItem;
				}
				else
				{
					// Insert as config doesn't exists
					localSql_ = " INSERT INTO PA_CONFIG " +
							" ( LOCATION_ID, PRI_IP, SEC_IP, PORT_NO, STATION_NO, MAX_ZONE ) " +
							" VALUES(" + dvStation.valueItem +
							" , '" + textBox_pas_PriIp.Text + "' " +
							" , '" + textBox_pas_SecIp.Text + "' " +
							" , " + (string.IsNullOrEmpty(textBox_pas_Port.Text) ? "NULL" : textBox_pas_Port.Text) +
							" , " + textBox_pas_stnNo.Text +
							" , " + Program.MAXZONEID + " ) ";
				}

                var queries = new List<SqlQuery> { new SqlQuery { CommandText = localSql_ } };
                if (Program.dbLock.ExcuteNoneResultQuery(queries, ref dbConn) < 1)
                {
                    Program.WriteEventLog("[ERROR] INSERT INTO PA_CONFIG", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
                    return;
                }

                eventParam = new string[] { "", "", textBox_pas_stnNo.Text, dvStation.displayItem };
				eventParamLocal = new string[] { "", "", textBox_pas_stnNo.Text, dvStation.displayItem };
				Program.logEvent(EnumType.PaEvent.PaSysConfig, eventParam, eventParamLocal);

				saveCurrPasTab();

				Program.MessageBox_Normal(updatePasPass_, sysErr_, this);

			}
			catch (NoOracleDBException ex)
			{
				Program.WriteEventLog("[ERROR] [NoOracleDBException] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				Program.MessageBox_Error(updatePasFail_ + "\n(" + Program.MB_NoDBConn + ")", sysErr_);
			}
			catch (Exception ex)
			{
				Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
				Program.MessageBox_Error(updatePasFail_, sysErr_);
			}

		}

		#endregion

		/// <summary>
		/// Method to validate an IP address
		/// using regular expressions. The pattern
		/// being used will validate an ip address
		/// with the range of 1.0.0.0 to 255.255.255.255
		/// </summary>
		/// <param name="addrStr">Address to validate</param>
		/// <returns>bool: true if it is a valid IP Address, otherwise false</returns>
		private bool IsValidIP(string addrStr)
		{
			// Nothing to validate : addrStr
			// IP match pattern
			string ipPattern = @"\b(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\b";
			Regex reGex = new Regex(ipPattern);

			bool isIp = false;

			//check to make sure an ip address was provided
			if (addrStr == "" || addrStr == null)
			{
				// No address provided so return false
				isIp = false;
			}
			else
			{
				// Address provided so use the IsMatch Method of the Regular Expression object
				isIp = reGex.IsMatch(addrStr, 0);
			}

			return isIp;
		}

		/// <summary>
		/// Method to validate whether the passed in string is numeric 
		/// </summary>
		/// <param name="strToCheck">String to be checked</param>
		/// <returns>bool: true if it is numeric, false otherwise</returns>
		private static bool IsNumeric(string strToCheck)
		{
			Regex objAlphaNumericPattern = new Regex("[^0-9]");
			if (strToCheck == null)
			{
				return false;
			}

			return !objAlphaNumericPattern.IsMatch(strToCheck);
		}

		/// <summary>
		/// Method to remove the last character of the input string
		/// </summary>
		/// <param name="inputStr">Input string</param>
		/// <returns>String with last character removed</returns>
		/// <since>00.03.02.00</since>
		private static string removeLastChar(string inputStr)
		{
			// Nothing to validate : inputStr
			return inputStr.Remove(inputStr.Length - 1);
		}

		/// <summary>
		/// The cleanUp method is for freeing up resource that was held by the class during runtime.
		/// </summary>
		/// <since>00.03.11.00</since>
		private void CleanUp()
		{

		}

		/// <summary>
		/// Operator make changes to the Zone Config
		/// </summary>
		/// <param name="sender">Control that raised the event</param>
		/// <param name="e">Additional information about the event that was raised</param>
		/// <since>00.03.18.02</since>
		private void textBox_ZoneConfig_TextChanged(object sender, EventArgs e)
		{
			// Nothing to validate : sender, e
			isSaveZoneCReq_ = true;
		}

		/// <summary>
		/// Operator make changes to the Zone Config
		/// </summary>
		/// <param name="sender">Control that raised the event</param>
		/// <param name="e">Additional information about the event that was raised</param>
		/// <since>00.03.18.02</since>
		private void comboBox_ZoneConfig_SelectedIndexChanged(object sender, EventArgs e)
		{
			// Nothing to validate : sender, e
			isSaveZoneCReq_ = true;
		}

		/// <summary>
		/// Operator make changes to the PAS Source Code mapping
		/// </summary>
		/// <param name="sender">Control that raised the event</param>
		/// <param name="e">Additional information about the event that was raised</param>
		/// <since>00.03.18.02</since>
		private void textBox_PaSourceCode_TextChanged(object sender, EventArgs e)
		{
			// Nothing to validate : sender, e
			isSavePaScCReq_ = true;
		}

		/// <summary>
		/// Save the settings of the current zone tab to be used for comparision later on
		/// </summary>
		/// <since>00.03.18.02</since>
		private void saveCurrZoneTab()
		{
			prevZoneStnName_ = comboBox_stationName.Text.ToString();
			prevZoneNo_ = textBox_zoneNo.Text;
			prevZoneCat_ = comboBox_Category.Text.ToString();
			prevZoneType_ = comboBox_zoneType.Text.ToString();
			prevZoneDesc_ = textBox_desc.Text;
			prevZoneDescLocal_ = textBox_descLocal.Text;
		}

		/// <summary>
		/// Check if there is a change in the zone tab
		/// </summary>
		/// <returns>bool: true if there is a change, otherwise false</returns>
		/// <since>00.03.18.02</since>
		private bool changeInZoneTab()
		{
			if (prevZoneStnName_.Equals(comboBox_stationName.Text.ToString(), StringComparison.CurrentCultureIgnoreCase) == false)
			{
				return true;
			}

			if (prevZoneNo_.Equals(textBox_zoneNo.Text.ToString(), StringComparison.CurrentCultureIgnoreCase) == false)
			{
				return true;
			}

			if (prevZoneCat_.Equals(comboBox_Category.Text.ToString(), StringComparison.CurrentCultureIgnoreCase) == false)
			{
				return true;
			}

			if (prevZoneType_.Equals(comboBox_zoneType.Text.ToString(), StringComparison.CurrentCultureIgnoreCase) == false)
			{
				return true;
			}

			if (prevZoneDesc_.Equals(textBox_desc.Text.ToString(), StringComparison.CurrentCultureIgnoreCase) == false)
			{
				return true;
			}

			if (prevZoneDescLocal_.Equals(textBox_descLocal.Text.ToString(), StringComparison.CurrentCultureIgnoreCase) == false)
			{
				return true;
			}

			return false;
		}

		/// <summary>
		/// Save the settings of the current PAS tab to be used for comparision later on
		/// </summary>
		/// <since>00.03.18.02</since>
		private void saveCurrPasTab()
		{
			prevPasStnName_ = ((DisplayValue)stationList_[comboBox_pas_StationNames.SelectedIndex]).displayItem.ToString();
			prevPasStnNo_ = textBox_pas_stnNo.Text;
			prevPasPriIp_ = textBox_pas_PriIp.Text;
			prevPasSecIp_ = textBox_pas_SecIp.Text;
			prePasPortNo_ = textBox_pas_Port.Text;
		}

		/// <summary>
		/// Check if there is a change in the PAS tab
		/// </summary>
		/// <returns>bool: true if there is a change, otherwise false</returns>
		/// <since>00.03.18.02</since>
		private bool changeInPasTab()
		{
			if (prevPasStnName_.Equals(((DisplayValue)stationList_[comboBox_pas_StationNames.SelectedIndex]).displayItem.ToString(), StringComparison.CurrentCultureIgnoreCase) == false)
			{
				return true;
			}

			if (prevPasStnNo_.Equals(textBox_pas_stnNo.Text.ToString(), StringComparison.CurrentCultureIgnoreCase) == false)
			{
				return true;
			}

			if (prevPasPriIp_.Equals(textBox_pas_PriIp.Text.ToString(), StringComparison.CurrentCultureIgnoreCase) == false)
			{
				return true;
			}

			if (prevPasSecIp_.Equals(textBox_pas_SecIp.Text.ToString(), StringComparison.CurrentCultureIgnoreCase) == false)
			{
				return true;
			}

			if (prePasPortNo_.Equals(textBox_pas_Port.Text.ToString(), StringComparison.CurrentCultureIgnoreCase) == false)
			{
				return true;
			}

			return false;
		}

		/// <summary>
		/// Method to correct the passed in string to a format suitable for use in SQL query 
		/// </summary>
		/// <param name="strToParse">String to be converted</param>
		/// <returns>Converted string that can be used in SQL query</returns>
		private static string parseSpecialCharacters(string strToParse)
		{
			// Nothing to validate : strToParse
			strToParse = strToParse.Replace("'", "''");

			return strToParse.Trim();
		}

        private async void button_time_update_Click(object sender, EventArgs e)
        {
            var selectedStation = (KeyValuePair<int, string>)comboBox_time_station.SelectedItem;
            var selectedDay = (KeyValuePair<int, string>)comboBox_time_weekday.SelectedItem;
            StationWeekdaySchedule schedule = new StationWeekdaySchedule
            {
                StationId = selectedStation.Key,
                WeekDay = selectedDay.Key,
                PeakStart = dateTimePicker_peak_start.Value.ToString("HHmm"),
                PeakEnd = dateTimePicker_peak_end.Value.ToString("HHmm"),
                OffPeakStart = dateTimePicker_offpeak_start.Value.ToString("HHmm"),
                OffPeakEnd = dateTimePicker_offpeak_end.Value.ToString("HHmm"),
                NightStart = dateTimePicker_night_start.Value.ToString("HHmm"),
                NightEnd = dateTimePicker_night_end.Value.ToString("HHmm")
            };

            //send command
            List<TimeInterval> tmList = new List<TimeInterval>();
            tmList.Add(new TimeInterval(1, schedule.PeakStart, schedule.PeakEnd));
            tmList.Add(new TimeInterval(2, schedule.OffPeakStart, schedule.OffPeakEnd));
            tmList.Add(new TimeInterval(3, schedule.NightStart, schedule.NightEnd));
            StationSchedule stnSchedule = new StationSchedule((byte)schedule.StationId, (byte)schedule.WeekDay, tmList);
            List<StationSchedule> stnScheduleList = new List<StationSchedule>();
            stnScheduleList.Add(stnSchedule);
            M43 cmd = new M43(stnScheduleList);

            bool result = await PaService.Instance.SetSchedule(cmd);

            if (result)
            {
                Program.MessageBox_Normal(PAManager.mbSendCmdSuccessful_, sysErr_, this);
            }
            else
            {
                Program.MessageBox_Error(PAManager.mbSendCmdFailed_, sysErr_);
                return;
            }

            if (schedule.StationId != 0)
            {
                saveStationWeekdaySchedule(schedule);
            }
            else //all stations
            {
                foreach (var item in stations_)
                {
                    schedule.StationId = item.Key;
                    saveStationWeekdaySchedule(schedule);
                }
            }
        }

        private void comboBox_time_station_SelectedIndexChanged(object sender, EventArgs e)
        {
            DisplayStationSchedule();
        }

        private void comboBox_time_weekday_SelectedIndexChanged(object sender, EventArgs e)
        {
            DisplayStationSchedule();
        }

        private void DisplayStationSchedule()
        {
            try
            {
                int stationId = ((KeyValuePair<int, string>)comboBox_time_station.SelectedItem).Key;
                int weekday = ((KeyValuePair<int, string>)comboBox_time_weekday.SelectedItem).Key;

                StationWeekdaySchedule schedule = loadStationWeekdaySchedule(stationId, weekday);

                if (schedule != null)
                {
                    int hour = int.Parse(schedule.PeakStart.Substring(0, 2));
                    int minute = int.Parse(schedule.PeakStart.Substring(2, 2));
                    dateTimePicker_peak_start.Value = DateTime.Today.AddHours(hour).AddMinutes(minute);

                    hour = int.Parse(schedule.PeakEnd.Substring(0, 2));
                    minute = int.Parse(schedule.PeakEnd.Substring(2, 2));
                    dateTimePicker_peak_end.Value = DateTime.Today.AddHours(hour).AddMinutes(minute);

                    hour = int.Parse(schedule.OffPeakStart.Substring(0, 2));
                    minute = int.Parse(schedule.OffPeakStart.Substring(2, 2));
                    dateTimePicker_offpeak_start.Value = DateTime.Today.AddHours(hour).AddMinutes(minute);

                    hour = int.Parse(schedule.OffPeakEnd.Substring(0, 2));
                    minute = int.Parse(schedule.OffPeakEnd.Substring(2, 2));
                    dateTimePicker_offpeak_end.Value = DateTime.Today.AddHours(hour).AddMinutes(minute);

                    hour = int.Parse(schedule.NightStart.Substring(0, 2));
                    minute = int.Parse(schedule.NightStart.Substring(2, 2));
                    dateTimePicker_night_start.Value = DateTime.Today.AddHours(hour).AddMinutes(minute);

                    hour = int.Parse(schedule.NightEnd.Substring(0, 2));
                    minute = int.Parse(schedule.NightEnd.Substring(2, 2));
                    dateTimePicker_night_end.Value = DateTime.Today.AddHours(hour).AddMinutes(minute);
                }
            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }
        }
    }
}
