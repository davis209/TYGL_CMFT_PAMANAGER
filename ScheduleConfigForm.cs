using System;
using System.Collections.Generic;
using System.Data;
using System.Linq;
using System.Windows.Forms;
using ste.common.util;

namespace ste.pa.pamanager
{
    internal sealed class ScheduleConfigForm : Form
    {
        private readonly long? scheduleId_;
        private readonly Action saved_;
        private readonly TextBox nameBox_ = new TextBox();
        private readonly CheckBox enabledBox_ = new CheckBox { Text = "Enabled" };
        private readonly ListBox normalMessages_ = new ListBox();
        private readonly ListBox emergencyMessages_ = new ListBox();
        private readonly TextBox messageContent_ = new TextBox { Multiline = true, ReadOnly = true, ScrollBars = ScrollBars.Vertical };
        private readonly CheckedListBox stations_ = new CheckedListBox();
        private readonly CheckedListBox zones_ = new CheckedListBox();
        private readonly CheckBox selectAllStations_ = new CheckBox { Text = "Select all stations" };
        private readonly CheckBox selectAllZones_ = new CheckBox { Text = "Select all zones" };
        private readonly ComboBox seatBox_ = new ComboBox { DropDownStyle = ComboBoxStyle.DropDownList };
        private readonly CheckBox chineseBox_ = new CheckBox { Text = "Chinese" };
        private readonly CheckBox taiwaneseBox_ = new CheckBox { Text = "Taiwanese" };
        private readonly CheckBox hakkaBox_ = new CheckBox { Text = "Hakka" };
        private readonly CheckBox englishBox_ = new CheckBox { Text = "English" };
        private readonly NumericUpDown playCount_ = new NumericUpDown { Minimum = 1, Maximum = 255, Value = 1 };
        private readonly NumericUpDown playInterval_ = new NumericUpDown { Minimum = 0, Maximum = 255 };
        private readonly ComboBox scheduleType_ = new ComboBox { DropDownStyle = ComboBoxStyle.DropDownList };
        private readonly DateTimePicker startAt_ = new DateTimePicker { Format = DateTimePickerFormat.Custom, CustomFormat = "yyyy-MM-dd HH:mm" };
        private readonly NumericUpDown repeatInterval_ = new NumericUpDown { Minimum = 1, Maximum = 999, Value = 1 };
        private readonly CheckedListBox weekdays_ = new CheckedListBox();
        private readonly Button saveButton_ = new Button { Text = "Save" };
        private bool synchronizingSelectAll_;

        private sealed class MessageItem
        {
            public int Id;
            public string Version;
            public string Label;
            public string Content;
            public override string ToString() { return Id + "  " + Label; }
        }

        private sealed class SelectionItem
        {
            public int Id;
            public string Name;
            public override string ToString() { return Name; }
        }

        public ScheduleConfigForm(long? scheduleId, Action saved)
        {
            scheduleId_ = scheduleId;
            saved_ = saved;
            Text = scheduleId.HasValue ? "Edit Broadcast Schedule" : "New Broadcast Schedule";
            StartPosition = FormStartPosition.CenterParent;
            MinimumSize = new System.Drawing.Size(900, 650);
            Size = new System.Drawing.Size(1040, 730);
            BuildLayout();
            Load += ScheduleConfigForm_Load;
        }

        private void BuildLayout()
        {
            var root = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 2, RowCount = 0, Padding = new Padding(10) };
            root.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 48));
            root.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 52));
            root.RowStyles.Add(new RowStyle(SizeType.Percent, 100));
            root.RowStyles.Add(new RowStyle(SizeType.AutoSize));
            Controls.Add(root);

            var messageGroup = new GroupBox { Text = "Message", Dock = DockStyle.Fill };
            var messageLayout = new TableLayoutPanel { Dock = DockStyle.Fill, RowCount = 2 };
            messageLayout.RowStyles.Add(new RowStyle(SizeType.Percent, 58));
            messageLayout.RowStyles.Add(new RowStyle(SizeType.Percent, 42));
            var messageTabs = new TabControl { Dock = DockStyle.Fill };
            messageTabs.TabPages.Add(CreateMessagePage("Normal Messages", normalMessages_));
            messageTabs.TabPages.Add(CreateMessagePage("Emergency Messages", emergencyMessages_));
            messageContent_.Dock = DockStyle.Fill;
            messageLayout.Controls.Add(messageTabs, 0, 0);
            messageLayout.Controls.Add(messageContent_, 0, 1);
            messageGroup.Controls.Add(messageLayout);
            root.Controls.Add(messageGroup, 0, 0);

            var configGroup = new GroupBox { Text = "Schedule Configuration", Dock = DockStyle.Fill };
            var config = new TableLayoutPanel { Dock = DockStyle.Fill, ColumnCount = 2, AutoScroll = true, Padding = new Padding(8) };
            config.ColumnStyles.Add(new ColumnStyle(SizeType.Absolute, 125));
            config.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100));
            AddRow(config, "Name", nameBox_);
            AddRow(config, "", enabledBox_);
            AddRow(config, "Stations", selectAllStations_);
            AddRow(config, "", stations_, 105);
            AddRow(config, "Zones", selectAllZones_);
            AddRow(config, "", zones_, 85);
            AddRow(config, "Seat", seatBox_);
            AddRow(config, "Languages", CreateLanguagePanel());
            AddRow(config, "Play count", playCount_);
            AddRow(config, "Interval (sec)", playInterval_);
            AddRow(config, "Schedule type", scheduleType_);
            AddRow(config, "Start time", startAt_);
            AddRow(config, "Repeat every", repeatInterval_);
            AddRow(config, "Weekdays", weekdays_, 85);
            configGroup.Controls.Add(config);
            root.Controls.Add(configGroup, 1, 0);

            var buttons = new FlowLayoutPanel { Dock = DockStyle.Fill, FlowDirection = FlowDirection.RightToLeft, AutoSize = true };
            var cancel = new Button { Text = "Cancel", DialogResult = DialogResult.Cancel };
            saveButton_.Click += saveButton_Click;
            buttons.Controls.Add(cancel);
            buttons.Controls.Add(saveButton_);
            root.SetColumnSpan(buttons, 2);
            root.Controls.Add(buttons, 0, 1);
            AcceptButton = saveButton_;
            CancelButton = cancel;

            normalMessages_.Dock = DockStyle.Fill;
            emergencyMessages_.Dock = DockStyle.Fill;
            normalMessages_.SelectedIndexChanged += message_SelectedIndexChanged;
            emergencyMessages_.SelectedIndexChanged += message_SelectedIndexChanged;
            selectAllStations_.CheckedChanged += (s, e) => SetAllItemsChecked(stations_, selectAllStations_.Checked);
            selectAllZones_.CheckedChanged += (s, e) => SetAllItemsChecked(zones_, selectAllZones_.Checked);
            stations_.ItemCheck += (s, e) => BeginInvoke(new Action(UpdateSelectAllCheckBoxes));
            zones_.ItemCheck += (s, e) => BeginInvoke(new Action(UpdateSelectAllCheckBoxes));
            scheduleType_.Items.AddRange(new object[] { "ONCE", "DAILY", "WEEKLY" });
            scheduleType_.SelectedIndex = 0;
            scheduleType_.SelectedIndexChanged += scheduleType_SelectedIndexChanged;
            weekdays_.Items.AddRange(new object[] { "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday" });
        }

        private static TabPage CreateMessagePage(string title, ListBox list)
        {
            var page = new TabPage(title);
            page.Controls.Add(list);
            return page;
        }

        private static void AddRow(TableLayoutPanel layout, string label, Control control, int height = 32)
        {
            int row = layout.RowCount++;
            layout.RowStyles.Add(new RowStyle(SizeType.Absolute, height));
            var caption = new Label { Text = label, TextAlign = System.Drawing.ContentAlignment.MiddleRight, Dock = DockStyle.Fill };
            control.Dock = DockStyle.Fill;
            layout.Controls.Add(caption, 0, row);
            layout.Controls.Add(control, 1, row);
        }

        private Control CreateLanguagePanel()
        {
            var panel = new FlowLayoutPanel { Dock = DockStyle.Fill };
            chineseBox_.Margin = new Padding(0, 3, 4, 3);
            taiwaneseBox_.Margin = new Padding(0, 3, 4, 3);
            hakkaBox_.Margin = new Padding(0, 3, 4, 3);
            englishBox_.Margin = new Padding(0, 3, 0, 3);
            panel.Controls.AddRange(new Control[] { chineseBox_, taiwaneseBox_, hakkaBox_, englishBox_ });
            chineseBox_.Checked = true;
            return panel;
        }

        private void ScheduleConfigForm_Load(object sender, EventArgs e)
        {
            LoadMessages();
            LoadStations();
            LoadZones();
            LoadSeats();
            if (scheduleId_.HasValue) LoadSchedule();
        }

        private void LoadMessages()
        {
            DataSet ds = Fetch("SELECT M.MSG_ID, M.MSG_LABEL, M.MSG_CONTENT, M.VERSION " +
                "FROM PA_PRERECORDED_MSG M WHERE M.VERSION=(SELECT DVA_ADHOC_PREREC FROM PA_DVA_VERSION WHERE LOCATION_ID=" + Program.profileLocIndex + ") ORDER BY M.MSG_ID");
            if (ds == null || ds.Tables.Count == 0) return;
            foreach (DataRow row in ds.Tables[0].Rows)
            {
                var item = new MessageItem { Id = Convert.ToInt32(row[0]), Label = row[1].ToString(), Content = row[2].ToString(), Version = row[3].ToString() };
                (item.Id < 666 ? emergencyMessages_ : normalMessages_).Items.Add(item);
            }
        }

        private void LoadStations()
        {
            foreach (var station in Program.stnNoToStnNameDic_.OrderBy(x => x.Key))
                stations_.Items.Add(new SelectionItem { Id = station.Key, Name = station.Value });
        }

        private void LoadZones()
        {
            DataSet ds = Fetch("SELECT ZONE_TYPE_ID, " + (Program.defaultLanguage == "en-US" ? "ZONE_TYPE" : "ZONE_TYPE_LOCAL") + " FROM PA_ZONE_TYPE ORDER BY ZONE_TYPE_ID");
            if (ds == null || ds.Tables.Count == 0) return;
            foreach (DataRow row in ds.Tables[0].Rows)
            {
                int typeId = Convert.ToInt32(row[0]);
                if (typeId >= 1 && typeId <= 4)
                    zones_.Items.Add(new SelectionItem { Id = typeId, Name = row[1].ToString() });
            }
        }

        private void LoadSeats()
        {
            foreach (var seat in Program.seats_)
                seatBox_.Items.Add(new SelectionItem { Id = seat.Key, Name = seat.Value });
            if (seatBox_.Items.Count > 0) seatBox_.SelectedIndex = 0;
        }

        private void LoadSchedule()
        {
            DataSet ds = Fetch("SELECT * FROM pa_broadcast_schedule WHERE SCHEDULE_ID=" + scheduleId_.Value);
            if (ds == null || ds.Tables.Count == 0 || ds.Tables[0].Rows.Count == 0) return;
            DataRow row = ds.Tables[0].Rows[0];
            nameBox_.Text = row["SCHEDULE_NAME"].ToString();
            enabledBox_.Checked = Convert.ToInt32(row["ENABLED"]) == 1;
            SelectMessage(Convert.ToInt32(row["MSG_ID"]), row["MSG_VERSION"].ToString());
            CheckStations(row["STATIONS"].ToString());
            CheckZones(Convert.ToInt32(row["ZONES"]));
            SelectSeat(Convert.ToInt32(row["SEAT_ID"]));
            SetLanguage(Convert.ToInt32(row["LANGUAGE"]));
            playCount_.Value = Convert.ToDecimal(row["PLAY_COUNT"]);
            playInterval_.Value = Convert.ToDecimal(row["PLAY_INTERVAL_SEC"]);
            scheduleType_.SelectedItem = row["SCHEDULE_TYPE"].ToString();
            startAt_.Value = Convert.ToDateTime(row["START_AT"]);
            repeatInterval_.Value = Convert.ToDecimal(row["REPEAT_INTERVAL"]);
            if (row["WEEKDAY_MASK"] != DBNull.Value) CheckWeekdays(Convert.ToInt32(row["WEEKDAY_MASK"]));
        }

        private void message_SelectedIndexChanged(object sender, EventArgs e)
        {
            var list = sender as ListBox;
            var selected = list == null ? null : list.SelectedItem as MessageItem;
            if (selected == null) return;
            if (list == normalMessages_) emergencyMessages_.ClearSelected(); else normalMessages_.ClearSelected();
            messageContent_.Text = selected.Content;
        }

        private void scheduleType_SelectedIndexChanged(object sender, EventArgs e)
        {
            weekdays_.Enabled = scheduleType_.SelectedItem != null && scheduleType_.SelectedItem.ToString() == "WEEKLY";
            repeatInterval_.Enabled = scheduleType_.SelectedItem == null || scheduleType_.SelectedItem.ToString() != "ONCE";
        }

        private MessageItem SelectedMessage()
        {
            return normalMessages_.SelectedItem as MessageItem ?? emergencyMessages_.SelectedItem as MessageItem;
        }

        private void saveButton_Click(object sender, EventArgs e)
        {
            MessageItem message = SelectedMessage();
            if (string.IsNullOrWhiteSpace(nameBox_.Text) || message == null || stations_.CheckedItems.Count == 0 || zones_.CheckedItems.Count == 0 || seatBox_.SelectedItem == null)
            {
                MessageBox.Show("Name, message, station and zone are required.", Text, MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }
            if (scheduleType_.SelectedItem.ToString() == "WEEKLY" && weekdays_.CheckedItems.Count == 0)
            {
                MessageBox.Show("Select at least one weekday for a weekly schedule.", Text, MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            string stationIds = string.Join(",", stations_.CheckedItems.Cast<SelectionItem>().Select(x => x.Id.ToString()).OrderBy(x => int.Parse(x)));
            int zoneMask = zones_.CheckedItems.Cast<SelectionItem>().Aggregate(0, (mask, x) => mask | (1 << (x.Id - 1)));
            int weekdayMask = scheduleType_.SelectedItem.ToString() == "WEEKLY" ? weekdays_.CheckedIndices.Cast<int>().Aggregate(0, (mask, x) => mask | (1 << x)) : 0;
            int language = (chineseBox_.Checked ? 1 : 0) | (taiwaneseBox_.Checked ? 2 : 0) | (hakkaBox_.Checked ? 4 : 0) | (englishBox_.Checked ? 8 : 0);
            int seatId = ((SelectionItem)seatBox_.SelectedItem).Id;
            string escapedName = Escape(nameBox_.Text.Trim());
            string sql;
            if (scheduleId_.HasValue)
            {
                sql = "UPDATE pa_broadcast_schedule SET SCHEDULE_NAME='" + escapedName + "', ENABLED=" + (enabledBox_.Checked ? 1 : 0) +
                    ", MSG_ID=" + message.Id + ", MSG_VERSION='" + Escape(message.Version) + "', STATIONS='" + stationIds + "', ZONES=" + zoneMask +
                    ", SEAT_ID=" + seatId + ", LANGUAGE=" + language + ", PLAY_COUNT=" + playCount_.Value + ", PLAY_INTERVAL_SEC=" + playInterval_.Value +
                    ", SCHEDULE_TYPE='" + scheduleType_.SelectedItem + "', START_AT='" + DateSql(startAt_.Value) + "'" +
                    ", REPEAT_INTERVAL=" + repeatInterval_.Value + ", WEEKDAY_MASK=" + weekdayMask + ", NEXT_RUN_AT='" + DateSql(startAt_.Value) + "', UPDATED_AT=NOW(3) WHERE SCHEDULE_ID=" + scheduleId_.Value;
            }
            else
            {
                sql = "INSERT INTO pa_broadcast_schedule (LOCATION_ID,SCHEDULE_NAME,ENABLED,MSG_ID,MSG_VERSION,STATIONS,ZONES,SEAT_ID,LANGUAGE,PLAY_COUNT,PLAY_INTERVAL_SEC,SCHEDULE_TYPE,START_AT,REPEAT_INTERVAL,WEEKDAY_MASK,NEXT_RUN_AT,CREATED_AT,UPDATED_AT) VALUES (" +
                    Program.profileLocIndex + ",'" + escapedName + "'," + (enabledBox_.Checked ? 1 : 0) + "," + message.Id + ",'" + Escape(message.Version) + "','" + stationIds + "'," + zoneMask + "," + seatId + "," + language + "," + playCount_.Value + "," + playInterval_.Value + ",'" + scheduleType_.SelectedItem + "','" + DateSql(startAt_.Value) + "'," + repeatInterval_.Value + "," + weekdayMask + ",'" + DateSql(startAt_.Value) + "',NOW(3),NOW(3))";
            }
            dbConnEnum dbConn = dbConnEnum.ErrNoConn;
            var queries = new List<SqlQuery> { new SqlQuery { CommandText = sql } };
            if (Program.dbLock.ExcuteNoneResultQuery(queries, ref dbConn) < 1)
            {
                MessageBox.Show("Unable to save the schedule.", Text, MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            saved_?.Invoke();
            DialogResult = DialogResult.OK;
            Close();
        }

        private static DataSet Fetch(string sql)
        {
            dbConnEnum dbConn = dbConnEnum.ErrNoConn;
            return Program.dbLock.FetchData(new List<SqlQuery> { new SqlQuery { CommandText = sql } }, ref dbConn);
        }

        private static string Escape(string value) { return (value ?? string.Empty).Replace("'", "''"); }
        private static string DateSql(DateTime value) { return value.ToString("yyyy-MM-dd HH:mm:ss"); }
        private void SelectMessage(int id, string version) { SelectMessage(normalMessages_, id, version); SelectMessage(emergencyMessages_, id, version); }
        private static void SelectMessage(ListBox list, int id, string version) { for (int i = 0; i < list.Items.Count; i++) { var item = (MessageItem)list.Items[i]; if (item.Id == id && item.Version == version) { list.SelectedIndex = i; return; } } }
        private void CheckStations(string ids) { var set = new HashSet<string>(ids.Split(',')); for (int i = 0; i < stations_.Items.Count; i++) stations_.SetItemChecked(i, set.Contains(((SelectionItem)stations_.Items[i]).Id.ToString())); }
        private void CheckZones(int mask) { for (int i = 0; i < zones_.Items.Count; i++) { int type = ((SelectionItem)zones_.Items[i]).Id; zones_.SetItemChecked(i, (mask & (1 << (type - 1))) != 0); } }
        private void SelectSeat(int id) { for (int i = 0; i < seatBox_.Items.Count; i++) if (((SelectionItem)seatBox_.Items[i]).Id == id) { seatBox_.SelectedIndex = i; return; } }
        private void SetLanguage(int value) { chineseBox_.Checked = (value & 1) != 0; taiwaneseBox_.Checked = (value & 2) != 0; hakkaBox_.Checked = (value & 4) != 0; englishBox_.Checked = (value & 8) != 0; }
        private void CheckWeekdays(int mask) { for (int i = 0; i < weekdays_.Items.Count; i++) weekdays_.SetItemChecked(i, (mask & (1 << i)) != 0); }

        private void SetAllItemsChecked(CheckedListBox list, bool value)
        {
            if (synchronizingSelectAll_) return;
            synchronizingSelectAll_ = true;
            try
            {
                for (int i = 0; i < list.Items.Count; i++) list.SetItemChecked(i, value);
            }
            finally
            {
                synchronizingSelectAll_ = false;
            }
        }

        private void UpdateSelectAllCheckBoxes()
        {
            if (synchronizingSelectAll_) return;
            synchronizingSelectAll_ = true;
            try
            {
                selectAllStations_.Checked = stations_.Items.Count > 0 && stations_.CheckedItems.Count == stations_.Items.Count;
                selectAllZones_.Checked = zones_.Items.Count > 0 && zones_.CheckedItems.Count == zones_.Items.Count;
            }
            finally
            {
                synchronizingSelectAll_ = false;
            }
        }
    }
}
