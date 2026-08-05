namespace ste.pa.pamanager
{
    partial class DvaBroadcast
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
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle1 = new System.Windows.Forms.DataGridViewCellStyle();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(DvaBroadcast));
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle2 = new System.Windows.Forms.DataGridViewCellStyle();
            this.grpBox_Dva_Zone_Progress = new System.Windows.Forms.GroupBox();
            this.dataGridView_Dva_Progress = new System.Windows.Forms.DataGridView();
            this.grpBox_Dva_Zone_Failure = new System.Windows.Forms.GroupBox();
            this.but_Dva_Retry_All = new System.Windows.Forms.Button();
            this.but_Dva_Retry_Sel = new System.Windows.Forms.Button();
            this.dataGridView_Dva_Failure = new System.Windows.Forms.DataGridView();
            this.grpBox_Dva_Bc_config = new System.Windows.Forms.GroupBox();
            this.label_selMsg = new System.Windows.Forms.Label();
            this.label_dwell = new System.Windows.Forms.Label();
            this.listBox_chime = new System.Windows.Forms.ListBox();
            this.label_chime = new System.Windows.Forms.Label();
            this.listBox_period = new System.Windows.Forms.ListBox();
            this.listBox_stop = new System.Windows.Forms.ListBox();
            this.label_stop = new System.Windows.Forms.Label();
            this.listBox_start = new System.Windows.Forms.ListBox();
            this.listBox_dwell = new System.Windows.Forms.ListBox();
            this.label_start = new System.Windows.Forms.Label();
            this.label_period = new System.Windows.Forms.Label();
            this.listBox_msg_no = new System.Windows.Forms.ListBox();
            this.grpBox_Dva_Bc_status = new System.Windows.Forms.GroupBox();
            this.listBox_Dva_Bc_status = new System.Windows.Forms.ListBox();
            this.but_close = new System.Windows.Forms.Button();
            this.but_abort = new System.Windows.Forms.Button();
            this.backgroundWorker_Dva_SubmitRequest = new System.ComponentModel.BackgroundWorker();
            this.grpBox_Dva_Zone_Progress.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView_Dva_Progress)).BeginInit();
            this.grpBox_Dva_Zone_Failure.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView_Dva_Failure)).BeginInit();
            this.grpBox_Dva_Bc_config.SuspendLayout();
            this.grpBox_Dva_Bc_status.SuspendLayout();
            this.SuspendLayout();
            // 
            // grpBox_Dva_Zone_Progress
            // 
            this.grpBox_Dva_Zone_Progress.Controls.Add(this.dataGridView_Dva_Progress);
            this.grpBox_Dva_Zone_Progress.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F);
            this.grpBox_Dva_Zone_Progress.ForeColor = System.Drawing.SystemColors.ControlText;
            this.grpBox_Dva_Zone_Progress.Location = new System.Drawing.Point(12, 12);
            this.grpBox_Dva_Zone_Progress.Name = "grpBox_Dva_Zone_Progress";
            this.grpBox_Dva_Zone_Progress.Size = new System.Drawing.Size(380, 250);
            this.grpBox_Dva_Zone_Progress.TabIndex = 0;
            this.grpBox_Dva_Zone_Progress.TabStop = false;
            this.grpBox_Dva_Zone_Progress.Text = "Zone Progress";
            // 
            // dataGridView_Dva_Progress
            // 
            this.dataGridView_Dva_Progress.BackgroundColor = System.Drawing.SystemColors.Window;
            this.dataGridView_Dva_Progress.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dataGridView_Dva_Progress.Dock = System.Windows.Forms.DockStyle.Fill;
            this.dataGridView_Dva_Progress.GridColor = System.Drawing.Color.White;
            this.dataGridView_Dva_Progress.Location = new System.Drawing.Point(3, 22);
            this.dataGridView_Dva_Progress.Name = "dataGridView_Dva_Progress";
            this.dataGridView_Dva_Progress.ReadOnly = true;
            this.dataGridView_Dva_Progress.RowHeadersVisible = false;
            dataGridViewCellStyle1.BackColor = System.Drawing.SystemColors.Window;
            dataGridViewCellStyle1.ForeColor = System.Drawing.Color.Black;
            this.dataGridView_Dva_Progress.RowsDefaultCellStyle = dataGridViewCellStyle1;
            this.dataGridView_Dva_Progress.RowTemplate.Height = 24;
            this.dataGridView_Dva_Progress.RowTemplate.Resizable = System.Windows.Forms.DataGridViewTriState.False;
            this.dataGridView_Dva_Progress.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.FullRowSelect;
            this.dataGridView_Dva_Progress.Size = new System.Drawing.Size(374, 225);
            this.dataGridView_Dva_Progress.TabIndex = 1;
            // 
            // grpBox_Dva_Zone_Failure
            // 
            this.grpBox_Dva_Zone_Failure.Controls.Add(this.but_Dva_Retry_All);
            this.grpBox_Dva_Zone_Failure.Controls.Add(this.but_Dva_Retry_Sel);
            this.grpBox_Dva_Zone_Failure.Controls.Add(this.dataGridView_Dva_Failure);
            this.grpBox_Dva_Zone_Failure.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F);
            this.grpBox_Dva_Zone_Failure.ForeColor = System.Drawing.SystemColors.ControlText;
            this.grpBox_Dva_Zone_Failure.Location = new System.Drawing.Point(402, 12);
            this.grpBox_Dva_Zone_Failure.Name = "grpBox_Dva_Zone_Failure";
            this.grpBox_Dva_Zone_Failure.Size = new System.Drawing.Size(380, 250);
            this.grpBox_Dva_Zone_Failure.TabIndex = 1;
            this.grpBox_Dva_Zone_Failure.TabStop = false;
            this.grpBox_Dva_Zone_Failure.Text = "Zone Failures";
            // 
            // but_Dva_Retry_All
            // 
            this.but_Dva_Retry_All.Enabled = false;
            this.but_Dva_Retry_All.ForeColor = System.Drawing.Color.Black;
            this.but_Dva_Retry_All.Image = ((System.Drawing.Image)(resources.GetObject("but_Dva_Retry_All.Image")));
            this.but_Dva_Retry_All.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.but_Dva_Retry_All.Location = new System.Drawing.Point(254, 201);
            this.but_Dva_Retry_All.Name = "but_Dva_Retry_All";
            this.but_Dva_Retry_All.Size = new System.Drawing.Size(120, 43);
            this.but_Dva_Retry_All.TabIndex = 2;
            this.but_Dva_Retry_All.Text = "Retry all";
            this.but_Dva_Retry_All.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.but_Dva_Retry_All.UseVisualStyleBackColor = true;
            this.but_Dva_Retry_All.Click += new System.EventHandler(this.but_Dva_Retry_All_Click);
            // 
            // but_Dva_Retry_Sel
            // 
            this.but_Dva_Retry_Sel.ForeColor = System.Drawing.Color.Black;
            this.but_Dva_Retry_Sel.Image = ((System.Drawing.Image)(resources.GetObject("but_Dva_Retry_Sel.Image")));
            this.but_Dva_Retry_Sel.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.but_Dva_Retry_Sel.Location = new System.Drawing.Point(6, 201);
            this.but_Dva_Retry_Sel.Name = "but_Dva_Retry_Sel";
            this.but_Dva_Retry_Sel.Size = new System.Drawing.Size(120, 43);
            this.but_Dva_Retry_Sel.TabIndex = 1;
            this.but_Dva_Retry_Sel.Text = "Retry";
            this.but_Dva_Retry_Sel.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.but_Dva_Retry_Sel.UseVisualStyleBackColor = true;
            this.but_Dva_Retry_Sel.Click += new System.EventHandler(this.but_Dva_Retry_Sel_Click);
            // 
            // dataGridView_Dva_Failure
            // 
            this.dataGridView_Dva_Failure.BackgroundColor = System.Drawing.SystemColors.Window;
            this.dataGridView_Dva_Failure.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dataGridView_Dva_Failure.Dock = System.Windows.Forms.DockStyle.Top;
            this.dataGridView_Dva_Failure.GridColor = System.Drawing.Color.White;
            this.dataGridView_Dva_Failure.Location = new System.Drawing.Point(3, 22);
            this.dataGridView_Dva_Failure.Name = "dataGridView_Dva_Failure";
            this.dataGridView_Dva_Failure.ReadOnly = true;
            this.dataGridView_Dva_Failure.RowHeadersVisible = false;
            dataGridViewCellStyle2.BackColor = System.Drawing.SystemColors.Window;
            dataGridViewCellStyle2.ForeColor = System.Drawing.Color.Black;
            this.dataGridView_Dva_Failure.RowsDefaultCellStyle = dataGridViewCellStyle2;
            this.dataGridView_Dva_Failure.RowTemplate.Height = 24;
            this.dataGridView_Dva_Failure.RowTemplate.Resizable = System.Windows.Forms.DataGridViewTriState.False;
            this.dataGridView_Dva_Failure.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.FullRowSelect;
            this.dataGridView_Dva_Failure.Size = new System.Drawing.Size(374, 170);
            this.dataGridView_Dva_Failure.TabIndex = 3;
            // 
            // grpBox_Dva_Bc_config
            // 
            this.grpBox_Dva_Bc_config.Controls.Add(this.label_selMsg);
            this.grpBox_Dva_Bc_config.Controls.Add(this.label_dwell);
            this.grpBox_Dva_Bc_config.Controls.Add(this.listBox_chime);
            this.grpBox_Dva_Bc_config.Controls.Add(this.label_chime);
            this.grpBox_Dva_Bc_config.Controls.Add(this.listBox_period);
            this.grpBox_Dva_Bc_config.Controls.Add(this.listBox_stop);
            this.grpBox_Dva_Bc_config.Controls.Add(this.label_stop);
            this.grpBox_Dva_Bc_config.Controls.Add(this.listBox_start);
            this.grpBox_Dva_Bc_config.Controls.Add(this.listBox_dwell);
            this.grpBox_Dva_Bc_config.Controls.Add(this.label_start);
            this.grpBox_Dva_Bc_config.Controls.Add(this.label_period);
            this.grpBox_Dva_Bc_config.Controls.Add(this.listBox_msg_no);
            this.grpBox_Dva_Bc_config.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F);
            this.grpBox_Dva_Bc_config.ForeColor = System.Drawing.SystemColors.ControlText;
            this.grpBox_Dva_Bc_config.Location = new System.Drawing.Point(12, 268);
            this.grpBox_Dva_Bc_config.Name = "grpBox_Dva_Bc_config";
            this.grpBox_Dva_Bc_config.Size = new System.Drawing.Size(770, 191);
            this.grpBox_Dva_Bc_config.TabIndex = 2;
            this.grpBox_Dva_Bc_config.TabStop = false;
            this.grpBox_Dva_Bc_config.Text = "DVA Broadcast Configuration";
            // 
            // label_selMsg
            // 
            this.label_selMsg.AutoSize = true;
            this.label_selMsg.Location = new System.Drawing.Point(459, 29);
            this.label_selMsg.Name = "label_selMsg";
            this.label_selMsg.Size = new System.Drawing.Size(145, 20);
            this.label_selMsg.TabIndex = 15;
            this.label_selMsg.Text = "Selected Message:";
            // 
            // label_dwell
            // 
            this.label_dwell.Location = new System.Drawing.Point(6, 125);
            this.label_dwell.Name = "label_dwell";
            this.label_dwell.Size = new System.Drawing.Size(79, 20);
            this.label_dwell.TabIndex = 3;
            this.label_dwell.Text = "Dwell (s):";
            this.label_dwell.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // listBox_chime
            // 
            this.listBox_chime.BackColor = System.Drawing.SystemColors.Window;
            this.listBox_chime.Enabled = false;
            this.listBox_chime.FormattingEnabled = true;
            this.listBox_chime.ItemHeight = 20;
            this.listBox_chime.Location = new System.Drawing.Point(91, 89);
            this.listBox_chime.Name = "listBox_chime";
            this.listBox_chime.Size = new System.Drawing.Size(158, 24);
            this.listBox_chime.TabIndex = 8;
            // 
            // label_chime
            // 
            this.label_chime.Location = new System.Drawing.Point(6, 93);
            this.label_chime.Name = "label_chime";
            this.label_chime.Size = new System.Drawing.Size(79, 20);
            this.label_chime.TabIndex = 1;
            this.label_chime.Text = "Chime:";
            this.label_chime.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // listBox_period
            // 
            this.listBox_period.BackColor = System.Drawing.SystemColors.Window;
            this.listBox_period.Enabled = false;
            this.listBox_period.FormattingEnabled = true;
            this.listBox_period.ItemHeight = 20;
            this.listBox_period.Location = new System.Drawing.Point(91, 153);
            this.listBox_period.Name = "listBox_period";
            this.listBox_period.Size = new System.Drawing.Size(158, 24);
            this.listBox_period.TabIndex = 11;
            // 
            // listBox_stop
            // 
            this.listBox_stop.BackColor = System.Drawing.SystemColors.Window;
            this.listBox_stop.Enabled = false;
            this.listBox_stop.FormattingEnabled = true;
            this.listBox_stop.ItemHeight = 20;
            this.listBox_stop.Location = new System.Drawing.Point(91, 57);
            this.listBox_stop.Name = "listBox_stop";
            this.listBox_stop.Size = new System.Drawing.Size(349, 24);
            this.listBox_stop.TabIndex = 12;
            // 
            // label_stop
            // 
            this.label_stop.Location = new System.Drawing.Point(6, 61);
            this.label_stop.Name = "label_stop";
            this.label_stop.Size = new System.Drawing.Size(79, 20);
            this.label_stop.TabIndex = 6;
            this.label_stop.Text = "Stop:";
            this.label_stop.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // listBox_start
            // 
            this.listBox_start.BackColor = System.Drawing.SystemColors.Window;
            this.listBox_start.Enabled = false;
            this.listBox_start.FormattingEnabled = true;
            this.listBox_start.ItemHeight = 20;
            this.listBox_start.Location = new System.Drawing.Point(91, 25);
            this.listBox_start.Name = "listBox_start";
            this.listBox_start.Size = new System.Drawing.Size(349, 24);
            this.listBox_start.TabIndex = 13;
            // 
            // listBox_dwell
            // 
            this.listBox_dwell.BackColor = System.Drawing.SystemColors.Window;
            this.listBox_dwell.Enabled = false;
            this.listBox_dwell.FormattingEnabled = true;
            this.listBox_dwell.ItemHeight = 20;
            this.listBox_dwell.Location = new System.Drawing.Point(91, 121);
            this.listBox_dwell.Name = "listBox_dwell";
            this.listBox_dwell.Size = new System.Drawing.Size(158, 24);
            this.listBox_dwell.TabIndex = 9;
            // 
            // label_start
            // 
            this.label_start.Location = new System.Drawing.Point(6, 29);
            this.label_start.Name = "label_start";
            this.label_start.Size = new System.Drawing.Size(79, 20);
            this.label_start.TabIndex = 5;
            this.label_start.Text = "Start:";
            this.label_start.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // label_period
            // 
            this.label_period.Location = new System.Drawing.Point(6, 157);
            this.label_period.Name = "label_period";
            this.label_period.Size = new System.Drawing.Size(79, 20);
            this.label_period.TabIndex = 4;
            this.label_period.Text = "Period:";
            this.label_period.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // listBox_msg_no
            // 
            this.listBox_msg_no.BackColor = System.Drawing.SystemColors.Window;
            this.listBox_msg_no.Enabled = false;
            this.listBox_msg_no.FormattingEnabled = true;
            this.listBox_msg_no.ItemHeight = 20;
            this.listBox_msg_no.Location = new System.Drawing.Point(463, 57);
            this.listBox_msg_no.Name = "listBox_msg_no";
            this.listBox_msg_no.Size = new System.Drawing.Size(301, 104);
            this.listBox_msg_no.TabIndex = 14;
            // 
            // grpBox_Dva_Bc_status
            // 
            this.grpBox_Dva_Bc_status.Controls.Add(this.listBox_Dva_Bc_status);
            this.grpBox_Dva_Bc_status.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F);
            this.grpBox_Dva_Bc_status.ForeColor = System.Drawing.SystemColors.ControlText;
            this.grpBox_Dva_Bc_status.Location = new System.Drawing.Point(12, 465);
            this.grpBox_Dva_Bc_status.Name = "grpBox_Dva_Bc_status";
            this.grpBox_Dva_Bc_status.Size = new System.Drawing.Size(770, 136);
            this.grpBox_Dva_Bc_status.TabIndex = 3;
            this.grpBox_Dva_Bc_status.TabStop = false;
            this.grpBox_Dva_Bc_status.Text = "Broadcast Status";
            // 
            // listBox_Dva_Bc_status
            // 
            this.listBox_Dva_Bc_status.Dock = System.Windows.Forms.DockStyle.Fill;
            this.listBox_Dva_Bc_status.FormattingEnabled = true;
            this.listBox_Dva_Bc_status.ItemHeight = 20;
            this.listBox_Dva_Bc_status.Location = new System.Drawing.Point(3, 22);
            this.listBox_Dva_Bc_status.Name = "listBox_Dva_Bc_status";
            this.listBox_Dva_Bc_status.Size = new System.Drawing.Size(764, 104);
            this.listBox_Dva_Bc_status.TabIndex = 0;
            // 
            // but_close
            // 
            this.but_close.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F);
            this.but_close.Image = ((System.Drawing.Image)(resources.GetObject("but_close.Image")));
            this.but_close.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.but_close.Location = new System.Drawing.Point(662, 607);
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
            this.but_abort.Image = ((System.Drawing.Image)(resources.GetObject("but_abort.Image")));
            this.but_abort.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.but_abort.Location = new System.Drawing.Point(536, 607);
            this.but_abort.Name = "but_abort";
            this.but_abort.Size = new System.Drawing.Size(120, 43);
            this.but_abort.TabIndex = 5;
            this.but_abort.Text = "Abort";
            this.but_abort.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.but_abort.UseVisualStyleBackColor = true;
            this.but_abort.Click += new System.EventHandler(this.button_abort_Click);
            // 
            // backgroundWorker_Dva_SubmitRequest
            // 
            this.backgroundWorker_Dva_SubmitRequest.WorkerSupportsCancellation = true;
            this.backgroundWorker_Dva_SubmitRequest.DoWork += new System.ComponentModel.DoWorkEventHandler(this.backgroundWorker_Dva_SubmitRequest_DoWork);
            this.backgroundWorker_Dva_SubmitRequest.RunWorkerCompleted += new System.ComponentModel.RunWorkerCompletedEventHandler(this.backgroundWorker_Dva_SubmitRequest_RunWorkerCompleted);
            // 
            // DvaBroadcast
            // 
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
            this.BackColor = System.Drawing.SystemColors.Control;
            this.ClientSize = new System.Drawing.Size(794, 658);
            this.Controls.Add(this.but_abort);
            this.Controls.Add(this.but_close);
            this.Controls.Add(this.grpBox_Dva_Bc_status);
            this.Controls.Add(this.grpBox_Dva_Bc_config);
            this.Controls.Add(this.grpBox_Dva_Zone_Failure);
            this.Controls.Add(this.grpBox_Dva_Zone_Progress);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "DvaBroadcast";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "DVA Broadcast";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.DVABroadcast_FormClosing);
            this.Load += new System.EventHandler(this.DvaBroadcast_Load);
            this.grpBox_Dva_Zone_Progress.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView_Dva_Progress)).EndInit();
            this.grpBox_Dva_Zone_Failure.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView_Dva_Failure)).EndInit();
            this.grpBox_Dva_Bc_config.ResumeLayout(false);
            this.grpBox_Dva_Bc_config.PerformLayout();
            this.grpBox_Dva_Bc_status.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox grpBox_Dva_Zone_Progress;
        private System.Windows.Forms.GroupBox grpBox_Dva_Zone_Failure;
        private System.Windows.Forms.GroupBox grpBox_Dva_Bc_config;
		private System.Windows.Forms.GroupBox grpBox_Dva_Bc_status;
        private System.Windows.Forms.Button but_Dva_Retry_All;
		private System.Windows.Forms.Button but_Dva_Retry_Sel;
        private System.Windows.Forms.Label label_stop;
        private System.Windows.Forms.Label label_start;
        private System.Windows.Forms.Label label_period;
		private System.Windows.Forms.Label label_dwell;
		private System.Windows.Forms.Label label_chime;
        private System.Windows.Forms.ListBox listBox_msg_no;
        private System.Windows.Forms.ListBox listBox_start;
        private System.Windows.Forms.ListBox listBox_stop;
		private System.Windows.Forms.ListBox listBox_period;
        private System.Windows.Forms.ListBox listBox_dwell;
		private System.Windows.Forms.ListBox listBox_chime;
        private System.Windows.Forms.Button but_close;
		private System.Windows.Forms.Button but_abort;
		private System.Windows.Forms.DataGridView dataGridView_Dva_Failure;
		private System.Windows.Forms.ListBox listBox_Dva_Bc_status;
		private System.Windows.Forms.Label label_selMsg;
		private System.ComponentModel.BackgroundWorker backgroundWorker_Dva_SubmitRequest;
		private System.Windows.Forms.DataGridView dataGridView_Dva_Progress;
    }
}
