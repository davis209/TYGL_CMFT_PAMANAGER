namespace ste.pa.pamanager
{
    partial class LiveBroadcast
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
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle2 = new System.Windows.Forms.DataGridViewCellStyle();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(LiveBroadcast));
            this.groupBox_liveBCstatus = new System.Windows.Forms.GroupBox();
            this.listBox_Live_Bc_Status = new System.Windows.Forms.ListBox();
            this.grpBox_Live_Zone_Progress = new System.Windows.Forms.GroupBox();
            this.dataGridView_Bc_Progress = new System.Windows.Forms.DataGridView();
            this.grpBox_Live_Zone_Failure = new System.Windows.Forms.GroupBox();
            this.dataGridView_Bc_Failure = new System.Windows.Forms.DataGridView();
            this.but_Live_Retry_All = new System.Windows.Forms.Button();
            this.but_Live_Retry_Sel = new System.Windows.Forms.Button();
            this.but_close = new System.Windows.Forms.Button();
            this.backgroundWorker_submitRequest = new System.ComponentModel.BackgroundWorker();
            this.groupBox_liveBCstatus.SuspendLayout();
            this.grpBox_Live_Zone_Progress.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView_Bc_Progress)).BeginInit();
            this.grpBox_Live_Zone_Failure.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView_Bc_Failure)).BeginInit();
            this.SuspendLayout();
            // 
            // groupBox_liveBCstatus
            // 
            this.groupBox_liveBCstatus.Controls.Add(this.listBox_Live_Bc_Status);
            this.groupBox_liveBCstatus.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F);
            this.groupBox_liveBCstatus.ForeColor = System.Drawing.SystemColors.ControlText;
            this.groupBox_liveBCstatus.Location = new System.Drawing.Point(12, 269);
            this.groupBox_liveBCstatus.Name = "groupBox_liveBCstatus";
            this.groupBox_liveBCstatus.Size = new System.Drawing.Size(770, 136);
            this.groupBox_liveBCstatus.TabIndex = 0;
            this.groupBox_liveBCstatus.TabStop = false;
            this.groupBox_liveBCstatus.Text = "Live Announcement Status";
            // 
            // listBox_Live_Bc_Status
            // 
            this.listBox_Live_Bc_Status.Dock = System.Windows.Forms.DockStyle.Fill;
            this.listBox_Live_Bc_Status.FormattingEnabled = true;
            this.listBox_Live_Bc_Status.ItemHeight = 20;
            this.listBox_Live_Bc_Status.Location = new System.Drawing.Point(3, 22);
            this.listBox_Live_Bc_Status.Name = "listBox_Live_Bc_Status";
            this.listBox_Live_Bc_Status.Size = new System.Drawing.Size(764, 104);
            this.listBox_Live_Bc_Status.TabIndex = 0;
            // 
            // grpBox_Live_Zone_Progress
            // 
            this.grpBox_Live_Zone_Progress.Controls.Add(this.dataGridView_Bc_Progress);
            this.grpBox_Live_Zone_Progress.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F);
            this.grpBox_Live_Zone_Progress.ForeColor = System.Drawing.SystemColors.ControlText;
            this.grpBox_Live_Zone_Progress.Location = new System.Drawing.Point(12, 12);
            this.grpBox_Live_Zone_Progress.Name = "grpBox_Live_Zone_Progress";
            this.grpBox_Live_Zone_Progress.Size = new System.Drawing.Size(380, 250);
            this.grpBox_Live_Zone_Progress.TabIndex = 1;
            this.grpBox_Live_Zone_Progress.TabStop = false;
            this.grpBox_Live_Zone_Progress.Text = "Broadcast Progress";
            // 
            // dataGridView_Bc_Progress
            // 
            this.dataGridView_Bc_Progress.BackgroundColor = System.Drawing.SystemColors.Window;
            this.dataGridView_Bc_Progress.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dataGridView_Bc_Progress.Dock = System.Windows.Forms.DockStyle.Fill;
            this.dataGridView_Bc_Progress.GridColor = System.Drawing.Color.White;
            this.dataGridView_Bc_Progress.Location = new System.Drawing.Point(3, 22);
            this.dataGridView_Bc_Progress.Name = "dataGridView_Bc_Progress";
            this.dataGridView_Bc_Progress.ReadOnly = true;
            this.dataGridView_Bc_Progress.RowHeadersVisible = false;
            dataGridViewCellStyle1.ForeColor = System.Drawing.Color.Black;
            this.dataGridView_Bc_Progress.RowsDefaultCellStyle = dataGridViewCellStyle1;
            this.dataGridView_Bc_Progress.RowTemplate.Height = 24;
            this.dataGridView_Bc_Progress.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.FullRowSelect;
            this.dataGridView_Bc_Progress.Size = new System.Drawing.Size(374, 225);
            this.dataGridView_Bc_Progress.TabIndex = 1;
            // 
            // grpBox_Live_Zone_Failure
            // 
            this.grpBox_Live_Zone_Failure.Controls.Add(this.dataGridView_Bc_Failure);
            this.grpBox_Live_Zone_Failure.Controls.Add(this.but_Live_Retry_All);
            this.grpBox_Live_Zone_Failure.Controls.Add(this.but_Live_Retry_Sel);
            this.grpBox_Live_Zone_Failure.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F);
            this.grpBox_Live_Zone_Failure.ForeColor = System.Drawing.SystemColors.ControlText;
            this.grpBox_Live_Zone_Failure.Location = new System.Drawing.Point(402, 12);
            this.grpBox_Live_Zone_Failure.Name = "grpBox_Live_Zone_Failure";
            this.grpBox_Live_Zone_Failure.Size = new System.Drawing.Size(380, 250);
            this.grpBox_Live_Zone_Failure.TabIndex = 2;
            this.grpBox_Live_Zone_Failure.TabStop = false;
            this.grpBox_Live_Zone_Failure.Text = "Broadcast Failures";
            // 
            // dataGridView_Bc_Failure
            // 
            this.dataGridView_Bc_Failure.BackgroundColor = System.Drawing.SystemColors.Window;
            this.dataGridView_Bc_Failure.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dataGridView_Bc_Failure.Dock = System.Windows.Forms.DockStyle.Top;
            this.dataGridView_Bc_Failure.GridColor = System.Drawing.Color.White;
            this.dataGridView_Bc_Failure.Location = new System.Drawing.Point(3, 22);
            this.dataGridView_Bc_Failure.Name = "dataGridView_Bc_Failure";
            this.dataGridView_Bc_Failure.ReadOnly = true;
            this.dataGridView_Bc_Failure.RowHeadersVisible = false;
            dataGridViewCellStyle2.ForeColor = System.Drawing.Color.Black;
            this.dataGridView_Bc_Failure.RowsDefaultCellStyle = dataGridViewCellStyle2;
            this.dataGridView_Bc_Failure.RowTemplate.Height = 24;
            this.dataGridView_Bc_Failure.SelectionMode = System.Windows.Forms.DataGridViewSelectionMode.FullRowSelect;
            this.dataGridView_Bc_Failure.Size = new System.Drawing.Size(374, 173);
            this.dataGridView_Bc_Failure.TabIndex = 2;
            // 
            // but_Live_Retry_All
            // 
            this.but_Live_Retry_All.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F);
            this.but_Live_Retry_All.ForeColor = System.Drawing.Color.Black;
            this.but_Live_Retry_All.Image = ((System.Drawing.Image)(resources.GetObject("but_Live_Retry_All.Image")));
            this.but_Live_Retry_All.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.but_Live_Retry_All.Location = new System.Drawing.Point(244, 201);
            this.but_Live_Retry_All.Name = "but_Live_Retry_All";
            this.but_Live_Retry_All.Size = new System.Drawing.Size(120, 43);
            this.but_Live_Retry_All.TabIndex = 4;
            this.but_Live_Retry_All.Text = "Retry all";
            this.but_Live_Retry_All.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.but_Live_Retry_All.UseVisualStyleBackColor = true;
            this.but_Live_Retry_All.Click += new System.EventHandler(this.but_Live_Retry_All_Click);
            // 
            // but_Live_Retry_Sel
            // 
            this.but_Live_Retry_Sel.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F);
            this.but_Live_Retry_Sel.ForeColor = System.Drawing.Color.Black;
            this.but_Live_Retry_Sel.Image = ((System.Drawing.Image)(resources.GetObject("but_Live_Retry_Sel.Image")));
            this.but_Live_Retry_Sel.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.but_Live_Retry_Sel.Location = new System.Drawing.Point(7, 201);
            this.but_Live_Retry_Sel.Name = "but_Live_Retry_Sel";
            this.but_Live_Retry_Sel.Size = new System.Drawing.Size(120, 43);
            this.but_Live_Retry_Sel.TabIndex = 3;
            this.but_Live_Retry_Sel.Text = "Retry";
            this.but_Live_Retry_Sel.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.but_Live_Retry_Sel.UseVisualStyleBackColor = true;
            this.but_Live_Retry_Sel.Click += new System.EventHandler(this.but_Live_Retry_Sel_Click);
            // 
            // but_close
            // 
            this.but_close.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F);
            this.but_close.Image = ((System.Drawing.Image)(resources.GetObject("but_close.Image")));
            this.but_close.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.but_close.Location = new System.Drawing.Point(659, 411);
            this.but_close.Name = "but_close";
            this.but_close.Size = new System.Drawing.Size(120, 43);
            this.but_close.TabIndex = 5;
            this.but_close.Text = "Close";
            this.but_close.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.but_close.UseVisualStyleBackColor = true;
            this.but_close.Click += new System.EventHandler(this.but_closeClick);
            // 
            // backgroundWorker_submitRequest
            // 
            this.backgroundWorker_submitRequest.WorkerSupportsCancellation = true;
            this.backgroundWorker_submitRequest.DoWork += new System.ComponentModel.DoWorkEventHandler(this.backgroundWorker_submitRequest_DoWork);
            this.backgroundWorker_submitRequest.RunWorkerCompleted += new System.ComponentModel.RunWorkerCompletedEventHandler(this.backgroundWorker_submitRequest_RunWorkerCompleted);
            // 
            // LiveBroadcast
            // 
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
            this.BackColor = System.Drawing.SystemColors.Control;
            this.ClientSize = new System.Drawing.Size(794, 461);
            this.Controls.Add(this.but_close);
            this.Controls.Add(this.grpBox_Live_Zone_Failure);
            this.Controls.Add(this.grpBox_Live_Zone_Progress);
            this.Controls.Add(this.groupBox_liveBCstatus);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "LiveBroadcast";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Live Broadcast Status";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.LiveBroadcast_FormClosing);
            this.Load += new System.EventHandler(this.LiveBroadcast_Load);
            this.groupBox_liveBCstatus.ResumeLayout(false);
            this.grpBox_Live_Zone_Progress.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView_Bc_Progress)).EndInit();
            this.grpBox_Live_Zone_Failure.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView_Bc_Failure)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox_liveBCstatus;
        private System.Windows.Forms.ListBox listBox_Live_Bc_Status;
        private System.Windows.Forms.GroupBox grpBox_Live_Zone_Progress;
        private System.Windows.Forms.GroupBox grpBox_Live_Zone_Failure;
        private System.Windows.Forms.Button but_Live_Retry_Sel;
		private System.Windows.Forms.Button but_Live_Retry_All;
        private System.Windows.Forms.Button but_close;
        private System.Windows.Forms.DataGridView dataGridView_Bc_Progress;
		private System.Windows.Forms.DataGridView dataGridView_Bc_Failure;
		private System.ComponentModel.BackgroundWorker backgroundWorker_submitRequest;
    }
}
