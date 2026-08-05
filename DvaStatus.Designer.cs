namespace ste.pa.pamanager
{
	partial class DvaStatus
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(DvaStatus));
            this.but_close = new System.Windows.Forms.Button();
            this.but_abort = new System.Windows.Forms.Button();
            this.timer_updateprogress = new System.Windows.Forms.Timer(this.components);
            this.grpBox_Sel_Zones = new System.Windows.Forms.GroupBox();
            this.textBox_Sel_Zones = new System.Windows.Forms.TextBox();
            this.grpBox_Bc_status = new System.Windows.Forms.GroupBox();
            this.listBox_Dva_Bc_status = new System.Windows.Forms.ListBox();
            this.grpBox_Dva_Bc_config = new System.Windows.Forms.GroupBox();
            this.textBox_language = new System.Windows.Forms.TextBox();
            this.textBox_seat = new System.Windows.Forms.TextBox();
            this.label_language = new System.Windows.Forms.Label();
            this.label_seat = new System.Windows.Forms.Label();
            this.textBox_selected_msgcontent = new System.Windows.Forms.TextBox();
            this.textBox_IntervalSec = new System.Windows.Forms.TextBox();
            this.textBox_RecycleTime = new System.Windows.Forms.TextBox();
            this.label_sec = new System.Windows.Forms.Label();
            this.label_recycle_time = new System.Windows.Forms.Label();
            this.label_cyclic_interval = new System.Windows.Forms.Label();
            this.label_selMsg = new System.Windows.Forms.Label();
            this.panel1 = new System.Windows.Forms.Panel();
            this.dataGridView_SelectedZones = new System.Windows.Forms.DataGridView();
            this.Column_Zone = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.Column1 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.dataGridView_SelectedZones_Occ = new System.Windows.Forms.DataGridView();
            this.Loc = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.dataGridViewTextBoxColumn1 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.Column3 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.listBox8 = new System.Windows.Forms.ListBox();
            this.tableLayoutPanel3 = new System.Windows.Forms.TableLayoutPanel();
            this.label8 = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.grpBox_Sel_Stations = new System.Windows.Forms.GroupBox();
            this.textBox_Sel_Stations = new System.Windows.Forms.TextBox();
            this.grpBox_Sel_Zones.SuspendLayout();
            this.grpBox_Bc_status.SuspendLayout();
            this.grpBox_Dva_Bc_config.SuspendLayout();
            this.panel1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView_SelectedZones)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView_SelectedZones_Occ)).BeginInit();
            this.groupBox3.SuspendLayout();
            this.tableLayoutPanel3.SuspendLayout();
            this.grpBox_Sel_Stations.SuspendLayout();
            this.SuspendLayout();
            // 
            // but_close
            // 
            this.but_close.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F);
            this.but_close.Image = ((System.Drawing.Image)(resources.GetObject("but_close.Image")));
            this.but_close.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.but_close.Location = new System.Drawing.Point(638, 558);
            this.but_close.Name = "but_close";
            this.but_close.Size = new System.Drawing.Size(120, 43);
            this.but_close.TabIndex = 4;
            this.but_close.Text = "Close";
            this.but_close.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.but_close.UseVisualStyleBackColor = true;
            this.but_close.Click += new System.EventHandler(this.button_close_Click);
            // 
            // but_abort
            // 
            this.but_abort.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F);
            this.but_abort.ForeColor = System.Drawing.Color.Black;
            this.but_abort.Image = ((System.Drawing.Image)(resources.GetObject("but_abort.Image")));
            this.but_abort.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.but_abort.Location = new System.Drawing.Point(509, 558);
            this.but_abort.Name = "but_abort";
            this.but_abort.Size = new System.Drawing.Size(120, 43);
            this.but_abort.TabIndex = 5;
            this.but_abort.Text = "Abort";
            this.but_abort.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.but_abort.UseVisualStyleBackColor = true;
            this.but_abort.Click += new System.EventHandler(this.button_abort_Click);
            // 
            // timer_updateprogress
            // 
            this.timer_updateprogress.Interval = 500;
            this.timer_updateprogress.Tick += new System.EventHandler(this.timer_updateprogress_Tick);
            // 
            // grpBox_Sel_Zones
            // 
            this.grpBox_Sel_Zones.Controls.Add(this.textBox_Sel_Zones);
            this.grpBox_Sel_Zones.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F);
            this.grpBox_Sel_Zones.ForeColor = System.Drawing.SystemColors.ControlText;
            this.grpBox_Sel_Zones.Location = new System.Drawing.Point(12, 328);
            this.grpBox_Sel_Zones.Name = "grpBox_Sel_Zones";
            this.grpBox_Sel_Zones.Padding = new System.Windows.Forms.Padding(10);
            this.grpBox_Sel_Zones.Size = new System.Drawing.Size(746, 65);
            this.grpBox_Sel_Zones.TabIndex = 10;
            this.grpBox_Sel_Zones.TabStop = false;
            this.grpBox_Sel_Zones.Text = "Selected Zones";
            // 
            // textBox_Sel_Zones
            // 
            this.textBox_Sel_Zones.BackColor = System.Drawing.Color.White;
            this.textBox_Sel_Zones.Dock = System.Windows.Forms.DockStyle.Fill;
            this.textBox_Sel_Zones.Location = new System.Drawing.Point(10, 29);
            this.textBox_Sel_Zones.Multiline = true;
            this.textBox_Sel_Zones.Name = "textBox_Sel_Zones";
            this.textBox_Sel_Zones.ReadOnly = true;
            this.textBox_Sel_Zones.Size = new System.Drawing.Size(726, 26);
            this.textBox_Sel_Zones.TabIndex = 34;
            // 
            // grpBox_Bc_status
            // 
            this.grpBox_Bc_status.Controls.Add(this.listBox_Dva_Bc_status);
            this.grpBox_Bc_status.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F);
            this.grpBox_Bc_status.ForeColor = System.Drawing.SystemColors.ControlText;
            this.grpBox_Bc_status.Location = new System.Drawing.Point(12, 411);
            this.grpBox_Bc_status.Name = "grpBox_Bc_status";
            this.grpBox_Bc_status.Padding = new System.Windows.Forms.Padding(10);
            this.grpBox_Bc_status.Size = new System.Drawing.Size(746, 136);
            this.grpBox_Bc_status.TabIndex = 9;
            this.grpBox_Bc_status.TabStop = false;
            this.grpBox_Bc_status.Text = "Current Broadcast Status";
            // 
            // listBox_Dva_Bc_status
            // 
            this.listBox_Dva_Bc_status.Dock = System.Windows.Forms.DockStyle.Fill;
            this.listBox_Dva_Bc_status.FormattingEnabled = true;
            this.listBox_Dva_Bc_status.ItemHeight = 20;
            this.listBox_Dva_Bc_status.Location = new System.Drawing.Point(10, 29);
            this.listBox_Dva_Bc_status.Name = "listBox_Dva_Bc_status";
            this.listBox_Dva_Bc_status.Size = new System.Drawing.Size(726, 97);
            this.listBox_Dva_Bc_status.TabIndex = 0;
            // 
            // grpBox_Dva_Bc_config
            // 
            this.grpBox_Dva_Bc_config.Controls.Add(this.textBox_language);
            this.grpBox_Dva_Bc_config.Controls.Add(this.textBox_seat);
            this.grpBox_Dva_Bc_config.Controls.Add(this.label_language);
            this.grpBox_Dva_Bc_config.Controls.Add(this.label_seat);
            this.grpBox_Dva_Bc_config.Controls.Add(this.textBox_selected_msgcontent);
            this.grpBox_Dva_Bc_config.Controls.Add(this.textBox_IntervalSec);
            this.grpBox_Dva_Bc_config.Controls.Add(this.textBox_RecycleTime);
            this.grpBox_Dva_Bc_config.Controls.Add(this.label_sec);
            this.grpBox_Dva_Bc_config.Controls.Add(this.label_recycle_time);
            this.grpBox_Dva_Bc_config.Controls.Add(this.label_cyclic_interval);
            this.grpBox_Dva_Bc_config.Controls.Add(this.label_selMsg);
            this.grpBox_Dva_Bc_config.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F);
            this.grpBox_Dva_Bc_config.ForeColor = System.Drawing.SystemColors.ControlText;
            this.grpBox_Dva_Bc_config.Location = new System.Drawing.Point(12, 12);
            this.grpBox_Dva_Bc_config.Name = "grpBox_Dva_Bc_config";
            this.grpBox_Dva_Bc_config.Size = new System.Drawing.Size(746, 190);
            this.grpBox_Dva_Bc_config.TabIndex = 6;
            this.grpBox_Dva_Bc_config.TabStop = false;
            this.grpBox_Dva_Bc_config.Text = "DVA Broadcast Configuration";
            // 
            // textBox_language
            // 
            this.textBox_language.BackColor = System.Drawing.Color.White;
            this.textBox_language.Location = new System.Drawing.Point(116, 146);
            this.textBox_language.Name = "textBox_language";
            this.textBox_language.ReadOnly = true;
            this.textBox_language.Size = new System.Drawing.Size(276, 26);
            this.textBox_language.TabIndex = 32;
            // 
            // textBox_seat
            // 
            this.textBox_seat.BackColor = System.Drawing.Color.White;
            this.textBox_seat.Location = new System.Drawing.Point(116, 41);
            this.textBox_seat.Name = "textBox_seat";
            this.textBox_seat.ReadOnly = true;
            this.textBox_seat.Size = new System.Drawing.Size(65, 26);
            this.textBox_seat.TabIndex = 31;
            // 
            // label_language
            // 
            this.label_language.Location = new System.Drawing.Point(17, 149);
            this.label_language.Name = "label_language";
            this.label_language.Size = new System.Drawing.Size(94, 20);
            this.label_language.TabIndex = 30;
            this.label_language.Text = "播放語言";
            this.label_language.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // label_seat
            // 
            this.label_seat.Location = new System.Drawing.Point(13, 44);
            this.label_seat.Name = "label_seat";
            this.label_seat.Size = new System.Drawing.Size(98, 20);
            this.label_seat.TabIndex = 29;
            this.label_seat.Text = "操控席位";
            this.label_seat.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // textBox_selected_msgcontent
            // 
            this.textBox_selected_msgcontent.BackColor = System.Drawing.Color.White;
            this.textBox_selected_msgcontent.Location = new System.Drawing.Point(426, 53);
            this.textBox_selected_msgcontent.Multiline = true;
            this.textBox_selected_msgcontent.Name = "textBox_selected_msgcontent";
            this.textBox_selected_msgcontent.ReadOnly = true;
            this.textBox_selected_msgcontent.Size = new System.Drawing.Size(310, 119);
            this.textBox_selected_msgcontent.TabIndex = 28;
            // 
            // textBox_IntervalSec
            // 
            this.textBox_IntervalSec.BackColor = System.Drawing.Color.White;
            this.textBox_IntervalSec.Location = new System.Drawing.Point(116, 111);
            this.textBox_IntervalSec.Name = "textBox_IntervalSec";
            this.textBox_IntervalSec.ReadOnly = true;
            this.textBox_IntervalSec.Size = new System.Drawing.Size(65, 26);
            this.textBox_IntervalSec.TabIndex = 27;
            // 
            // textBox_RecycleTime
            // 
            this.textBox_RecycleTime.BackColor = System.Drawing.Color.White;
            this.textBox_RecycleTime.Location = new System.Drawing.Point(117, 76);
            this.textBox_RecycleTime.Name = "textBox_RecycleTime";
            this.textBox_RecycleTime.ReadOnly = true;
            this.textBox_RecycleTime.Size = new System.Drawing.Size(65, 26);
            this.textBox_RecycleTime.TabIndex = 25;
            // 
            // label_sec
            // 
            this.label_sec.AutoSize = true;
            this.label_sec.Location = new System.Drawing.Point(185, 114);
            this.label_sec.Name = "label_sec";
            this.label_sec.Size = new System.Drawing.Size(34, 20);
            this.label_sec.TabIndex = 23;
            this.label_sec.Text = "sec";
            // 
            // label_recycle_time
            // 
            this.label_recycle_time.Location = new System.Drawing.Point(5, 79);
            this.label_recycle_time.Name = "label_recycle_time";
            this.label_recycle_time.Size = new System.Drawing.Size(107, 20);
            this.label_recycle_time.TabIndex = 18;
            this.label_recycle_time.Text = "Recycle Time";
            this.label_recycle_time.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // label_cyclic_interval
            // 
            this.label_cyclic_interval.Location = new System.Drawing.Point(17, 114);
            this.label_cyclic_interval.Name = "label_cyclic_interval";
            this.label_cyclic_interval.Size = new System.Drawing.Size(94, 20);
            this.label_cyclic_interval.TabIndex = 19;
            this.label_cyclic_interval.Text = "Interval";
            this.label_cyclic_interval.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // label_selMsg
            // 
            this.label_selMsg.AutoSize = true;
            this.label_selMsg.Location = new System.Drawing.Point(422, 31);
            this.label_selMsg.Name = "label_selMsg";
            this.label_selMsg.Size = new System.Drawing.Size(145, 20);
            this.label_selMsg.TabIndex = 15;
            this.label_selMsg.Text = "Selected Message:";
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.dataGridView_SelectedZones);
            this.panel1.Controls.Add(this.dataGridView_SelectedZones_Occ);
            this.panel1.ForeColor = System.Drawing.Color.Black;
            this.panel1.Location = new System.Drawing.Point(6, 25);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(200, 100);
            this.panel1.TabIndex = 0;
            // 
            // dataGridView_SelectedZones
            // 
            this.dataGridView_SelectedZones.BackgroundColor = System.Drawing.SystemColors.Window;
            this.dataGridView_SelectedZones.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dataGridView_SelectedZones.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.Column_Zone,
            this.Column1});
            this.dataGridView_SelectedZones.Dock = System.Windows.Forms.DockStyle.Fill;
            this.dataGridView_SelectedZones.GridColor = System.Drawing.Color.White;
            this.dataGridView_SelectedZones.Location = new System.Drawing.Point(0, 0);
            this.dataGridView_SelectedZones.Name = "dataGridView_SelectedZones";
            this.dataGridView_SelectedZones.ReadOnly = true;
            this.dataGridView_SelectedZones.RowHeadersVisible = false;
            this.dataGridView_SelectedZones.RowTemplate.Height = 24;
            this.dataGridView_SelectedZones.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.dataGridView_SelectedZones.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.FullRowSelect;
            this.dataGridView_SelectedZones.Size = new System.Drawing.Size(200, 100);
            this.dataGridView_SelectedZones.TabIndex = 0;
            // 
            // Column_Zone
            // 
            this.Column_Zone.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
            this.Column_Zone.FillWeight = 3F;
            this.Column_Zone.HeaderText = "Zone";
            this.Column_Zone.Name = "Column_Zone";
            this.Column_Zone.ReadOnly = true;
            // 
            // Column1
            // 
            this.Column1.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
            this.Column1.FillWeight = 2F;
            this.Column1.HeaderText = "Index";
            this.Column1.Name = "Column1";
            this.Column1.ReadOnly = true;
            this.Column1.Visible = false;
            // 
            // dataGridView_SelectedZones_Occ
            // 
            this.dataGridView_SelectedZones_Occ.BackgroundColor = System.Drawing.SystemColors.Window;
            this.dataGridView_SelectedZones_Occ.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dataGridView_SelectedZones_Occ.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.Loc,
            this.dataGridViewTextBoxColumn1,
            this.Column3});
            this.dataGridView_SelectedZones_Occ.Dock = System.Windows.Forms.DockStyle.Fill;
            this.dataGridView_SelectedZones_Occ.GridColor = System.Drawing.Color.White;
            this.dataGridView_SelectedZones_Occ.Location = new System.Drawing.Point(0, 0);
            this.dataGridView_SelectedZones_Occ.Name = "dataGridView_SelectedZones_Occ";
            this.dataGridView_SelectedZones_Occ.ReadOnly = true;
            this.dataGridView_SelectedZones_Occ.RowHeadersVisible = false;
            this.dataGridView_SelectedZones_Occ.RowTemplate.Height = 24;
            this.dataGridView_SelectedZones_Occ.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.dataGridView_SelectedZones_Occ.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.FullRowSelect;
            this.dataGridView_SelectedZones_Occ.Size = new System.Drawing.Size(200, 100);
            this.dataGridView_SelectedZones_Occ.TabIndex = 1;
            // 
            // Loc
            // 
            this.Loc.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
            this.Loc.FillWeight = 3F;
            this.Loc.HeaderText = "Loc";
            this.Loc.Name = "Loc";
            this.Loc.ReadOnly = true;
            // 
            // dataGridViewTextBoxColumn1
            // 
            this.dataGridViewTextBoxColumn1.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
            this.dataGridViewTextBoxColumn1.FillWeight = 5F;
            this.dataGridViewTextBoxColumn1.HeaderText = "Zone";
            this.dataGridViewTextBoxColumn1.Name = "dataGridViewTextBoxColumn1";
            this.dataGridViewTextBoxColumn1.ReadOnly = true;
            // 
            // Column3
            // 
            this.Column3.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
            this.Column3.FillWeight = 2F;
            this.Column3.HeaderText = "Index";
            this.Column3.Name = "Column3";
            this.Column3.ReadOnly = true;
            this.Column3.Visible = false;
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.listBox8);
            this.groupBox3.Location = new System.Drawing.Point(0, 0);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(200, 100);
            this.groupBox3.TabIndex = 0;
            this.groupBox3.TabStop = false;
            // 
            // listBox8
            // 
            this.listBox8.FormattingEnabled = true;
            this.listBox8.Location = new System.Drawing.Point(7, 185);
            this.listBox8.Name = "listBox8";
            this.listBox8.Size = new System.Drawing.Size(574, 95);
            this.listBox8.TabIndex = 0;
            // 
            // tableLayoutPanel3
            // 
            this.tableLayoutPanel3.ColumnCount = 2;
            this.tableLayoutPanel3.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
            this.tableLayoutPanel3.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
            this.tableLayoutPanel3.Controls.Add(this.label8, 0, 0);
            this.tableLayoutPanel3.Location = new System.Drawing.Point(0, 0);
            this.tableLayoutPanel3.Name = "tableLayoutPanel3";
            this.tableLayoutPanel3.RowCount = 1;
            this.tableLayoutPanel3.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.tableLayoutPanel3.Size = new System.Drawing.Size(200, 100);
            this.tableLayoutPanel3.TabIndex = 0;
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(3, 0);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(36, 13);
            this.label8.TabIndex = 1;
            this.label8.Text = "Chime";
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(3, 13);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(47, 13);
            this.label9.TabIndex = 3;
            this.label9.Text = "Dwell (s)";
            // 
            // grpBox_Sel_Stations
            // 
            this.grpBox_Sel_Stations.Controls.Add(this.textBox_Sel_Stations);
            this.grpBox_Sel_Stations.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F);
            this.grpBox_Sel_Stations.ForeColor = System.Drawing.SystemColors.ControlText;
            this.grpBox_Sel_Stations.Location = new System.Drawing.Point(12, 219);
            this.grpBox_Sel_Stations.Name = "grpBox_Sel_Stations";
            this.grpBox_Sel_Stations.Padding = new System.Windows.Forms.Padding(10);
            this.grpBox_Sel_Stations.Size = new System.Drawing.Size(746, 91);
            this.grpBox_Sel_Stations.TabIndex = 11;
            this.grpBox_Sel_Stations.TabStop = false;
            this.grpBox_Sel_Stations.Text = "Selected Stations";
            // 
            // textBox_Sel_Stations
            // 
            this.textBox_Sel_Stations.BackColor = System.Drawing.Color.White;
            this.textBox_Sel_Stations.Dock = System.Windows.Forms.DockStyle.Fill;
            this.textBox_Sel_Stations.Location = new System.Drawing.Point(10, 29);
            this.textBox_Sel_Stations.Multiline = true;
            this.textBox_Sel_Stations.Name = "textBox_Sel_Stations";
            this.textBox_Sel_Stations.ReadOnly = true;
            this.textBox_Sel_Stations.Size = new System.Drawing.Size(726, 52);
            this.textBox_Sel_Stations.TabIndex = 33;
            // 
            // DvaStatus
            // 
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
            this.BackColor = System.Drawing.SystemColors.Control;
            this.ClientSize = new System.Drawing.Size(774, 613);
            this.Controls.Add(this.grpBox_Sel_Stations);
            this.Controls.Add(this.grpBox_Sel_Zones);
            this.Controls.Add(this.grpBox_Bc_status);
            this.Controls.Add(this.but_close);
            this.Controls.Add(this.grpBox_Dva_Bc_config);
            this.Controls.Add(this.but_abort);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "DvaStatus";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "DVA Status";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.DvaStatus_FormClosing);
            this.Load += new System.EventHandler(this.DvaStatus_Load);
            this.grpBox_Sel_Zones.ResumeLayout(false);
            this.grpBox_Sel_Zones.PerformLayout();
            this.grpBox_Bc_status.ResumeLayout(false);
            this.grpBox_Dva_Bc_config.ResumeLayout(false);
            this.grpBox_Dva_Bc_config.PerformLayout();
            this.panel1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView_SelectedZones)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView_SelectedZones_Occ)).EndInit();
            this.groupBox3.ResumeLayout(false);
            this.tableLayoutPanel3.ResumeLayout(false);
            this.tableLayoutPanel3.PerformLayout();
            this.grpBox_Sel_Stations.ResumeLayout(false);
            this.grpBox_Sel_Stations.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

		private System.Windows.Forms.Button but_close;
		private System.Windows.Forms.Button but_abort;
		private System.Windows.Forms.Timer timer_updateprogress;
		private System.Windows.Forms.GroupBox groupBox3;
		private System.Windows.Forms.ListBox listBox8;
		private System.Windows.Forms.TableLayoutPanel tableLayoutPanel3;
		private System.Windows.Forms.Label label8;
		private System.Windows.Forms.Label label9;
		private System.Windows.Forms.GroupBox grpBox_Dva_Bc_config;
		private System.Windows.Forms.Label label_selMsg;
		private System.Windows.Forms.GroupBox grpBox_Bc_status;
		private System.Windows.Forms.ListBox listBox_Dva_Bc_status;
		private System.Windows.Forms.GroupBox grpBox_Sel_Zones;
		private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Label label_sec;
        private System.Windows.Forms.Label label_recycle_time;
        private System.Windows.Forms.Label label_cyclic_interval;
        private System.Windows.Forms.TextBox textBox_IntervalSec;
        private System.Windows.Forms.TextBox textBox_RecycleTime;
        private System.Windows.Forms.TextBox textBox_selected_msgcontent;
        private System.Windows.Forms.TextBox textBox_language;
        private System.Windows.Forms.TextBox textBox_seat;
        private System.Windows.Forms.Label label_language;
        private System.Windows.Forms.Label label_seat;
        private System.Windows.Forms.DataGridView dataGridView_SelectedZones;
        private System.Windows.Forms.DataGridViewTextBoxColumn Column_Zone;
        private System.Windows.Forms.DataGridViewTextBoxColumn Column1;
        private System.Windows.Forms.DataGridView dataGridView_SelectedZones_Occ;
        private System.Windows.Forms.DataGridViewTextBoxColumn Loc;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn1;
        private System.Windows.Forms.DataGridViewTextBoxColumn Column3;
        private System.Windows.Forms.GroupBox grpBox_Sel_Stations;
        private System.Windows.Forms.TextBox textBox_Sel_Zones;
        private System.Windows.Forms.TextBox textBox_Sel_Stations;
    }
}
