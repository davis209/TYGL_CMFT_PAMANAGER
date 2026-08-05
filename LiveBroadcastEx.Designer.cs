namespace ste.pa.pamanager
{
    partial class LiveBroadcastEx
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(LiveBroadcastEx));
            this.but_close = new System.Windows.Forms.Button();
            this.groupBox_liveBCstatus = new System.Windows.Forms.GroupBox();
            this.listBox_Live_Bc_Status = new System.Windows.Forms.ListBox();
            this.groupBox_filter = new System.Windows.Forms.GroupBox();
            this.button_live_start = new System.Windows.Forms.Button();
            this.button_live_end = new System.Windows.Forms.Button();
            this.comboBox_seats = new System.Windows.Forms.ComboBox();
            this.label_seat = new System.Windows.Forms.Label();
            this.groupBox_liveBCstatus.SuspendLayout();
            this.groupBox_filter.SuspendLayout();
            this.SuspendLayout();
            // 
            // but_close
            // 
            this.but_close.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F);
            this.but_close.Image = ((System.Drawing.Image)(resources.GetObject("but_close.Image")));
            this.but_close.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.but_close.Location = new System.Drawing.Point(665, 398);
            this.but_close.Name = "but_close";
            this.but_close.Size = new System.Drawing.Size(120, 43);
            this.but_close.TabIndex = 6;
            this.but_close.Text = "Close";
            this.but_close.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.but_close.UseVisualStyleBackColor = true;
            this.but_close.Click += new System.EventHandler(this.but_close_Click);
            // 
            // groupBox_liveBCstatus
            // 
            this.groupBox_liveBCstatus.Controls.Add(this.listBox_Live_Bc_Status);
            this.groupBox_liveBCstatus.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F);
            this.groupBox_liveBCstatus.ForeColor = System.Drawing.Color.White;
            this.groupBox_liveBCstatus.Location = new System.Drawing.Point(12, 81);
            this.groupBox_liveBCstatus.Name = "groupBox_liveBCstatus";
            this.groupBox_liveBCstatus.Size = new System.Drawing.Size(776, 311);
            this.groupBox_liveBCstatus.TabIndex = 7;
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
            this.listBox_Live_Bc_Status.Size = new System.Drawing.Size(770, 286);
            this.listBox_Live_Bc_Status.TabIndex = 0;
            // 
            // groupBox_filter
            // 
            this.groupBox_filter.Controls.Add(this.button_live_start);
            this.groupBox_filter.Controls.Add(this.button_live_end);
            this.groupBox_filter.Controls.Add(this.comboBox_seats);
            this.groupBox_filter.Controls.Add(this.label_seat);
            this.groupBox_filter.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.groupBox_filter.ForeColor = System.Drawing.Color.White;
            this.groupBox_filter.Location = new System.Drawing.Point(12, 0);
            this.groupBox_filter.Name = "groupBox_filter";
            this.groupBox_filter.Size = new System.Drawing.Size(776, 76);
            this.groupBox_filter.TabIndex = 11;
            this.groupBox_filter.TabStop = false;
            // 
            // button_live_start
            // 
            this.button_live_start.ForeColor = System.Drawing.Color.Black;
            this.button_live_start.Image = ((System.Drawing.Image)(resources.GetObject("button_live_start.Image")));
            this.button_live_start.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.button_live_start.Location = new System.Drawing.Point(374, 21);
            this.button_live_start.Name = "button_live_start";
            this.button_live_start.Size = new System.Drawing.Size(195, 40);
            this.button_live_start.TabIndex = 10;
            this.button_live_start.Text = "Start Live Broadcast";
            this.button_live_start.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.button_live_start.UseVisualStyleBackColor = true;
            this.button_live_start.Click += new System.EventHandler(this.button_live_start_Click);
            // 
            // button_live_end
            // 
            this.button_live_end.ForeColor = System.Drawing.Color.Black;
            this.button_live_end.Image = ((System.Drawing.Image)(resources.GetObject("button_live_end.Image")));
            this.button_live_end.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.button_live_end.Location = new System.Drawing.Point(575, 21);
            this.button_live_end.Name = "button_live_end";
            this.button_live_end.Size = new System.Drawing.Size(195, 40);
            this.button_live_end.TabIndex = 9;
            this.button_live_end.Text = "End Live Broadcast";
            this.button_live_end.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.button_live_end.UseVisualStyleBackColor = true;
            this.button_live_end.Click += new System.EventHandler(this.button_live_end_Click);
            // 
            // comboBox_seats
            // 
            this.comboBox_seats.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.comboBox_seats.FormattingEnabled = true;
            this.comboBox_seats.Location = new System.Drawing.Point(95, 27);
            this.comboBox_seats.Name = "comboBox_seats";
            this.comboBox_seats.Size = new System.Drawing.Size(121, 28);
            this.comboBox_seats.TabIndex = 7;
            // 
            // label_seat
            // 
            this.label_seat.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label_seat.ForeColor = System.Drawing.Color.White;
            this.label_seat.Location = new System.Drawing.Point(14, 31);
            this.label_seat.Name = "label_seat";
            this.label_seat.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.label_seat.Size = new System.Drawing.Size(79, 20);
            this.label_seat.TabIndex = 8;
            this.label_seat.Text = "Seat:";
            this.label_seat.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // LiveBroadcastEx
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(70)))), ((int)(((byte)(70)))), ((int)(((byte)(70)))));
            this.ClientSize = new System.Drawing.Size(800, 450);
            this.Controls.Add(this.groupBox_filter);
            this.Controls.Add(this.groupBox_liveBCstatus);
            this.Controls.Add(this.but_close);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "LiveBroadcastEx";
            this.Text = "Live Broadcast";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.LiveBroadcastEx_FormClosing);
            this.Load += new System.EventHandler(this.LiveBroadcastEx_Load);
            this.groupBox_liveBCstatus.ResumeLayout(false);
            this.groupBox_filter.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button but_close;
        private System.Windows.Forms.GroupBox groupBox_liveBCstatus;
        private System.Windows.Forms.ListBox listBox_Live_Bc_Status;
        private System.Windows.Forms.GroupBox groupBox_filter;
        private System.Windows.Forms.Button button_live_end;
        private System.Windows.Forms.ComboBox comboBox_seats;
        private System.Windows.Forms.Label label_seat;
        private System.Windows.Forms.Button button_live_start;
    }
}