namespace ste.pa.pamanager
{
    partial class AirRaidBroadcast
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(AirRaidBroadcast));
            this.but_close = new System.Windows.Forms.Button();
            this.groupBox_liveBCstatus = new System.Windows.Forms.GroupBox();
            this.listBox_Live_Bc_Status = new System.Windows.Forms.ListBox();
            this.groupBox_filter = new System.Windows.Forms.GroupBox();
            this.comboBox_voice_id = new System.Windows.Forms.ComboBox();
            this.label_voice_id = new System.Windows.Forms.Label();
            this.button_air_raid_alarm_on = new System.Windows.Forms.Button();
            this.button_air_raid_alarm_off = new System.Windows.Forms.Button();
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
            this.groupBox_liveBCstatus.Location = new System.Drawing.Point(12, 123);
            this.groupBox_liveBCstatus.Name = "groupBox_liveBCstatus";
            this.groupBox_liveBCstatus.Size = new System.Drawing.Size(776, 269);
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
            this.listBox_Live_Bc_Status.Size = new System.Drawing.Size(770, 244);
            this.listBox_Live_Bc_Status.TabIndex = 0;
            // 
            // groupBox_filter
            // 
            this.groupBox_filter.Controls.Add(this.comboBox_voice_id);
            this.groupBox_filter.Controls.Add(this.label_voice_id);
            this.groupBox_filter.Controls.Add(this.button_air_raid_alarm_on);
            this.groupBox_filter.Controls.Add(this.button_air_raid_alarm_off);
            this.groupBox_filter.Controls.Add(this.comboBox_seats);
            this.groupBox_filter.Controls.Add(this.label_seat);
            this.groupBox_filter.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.groupBox_filter.ForeColor = System.Drawing.Color.White;
            this.groupBox_filter.Location = new System.Drawing.Point(12, 0);
            this.groupBox_filter.Name = "groupBox_filter";
            this.groupBox_filter.Size = new System.Drawing.Size(776, 117);
            this.groupBox_filter.TabIndex = 12;
            this.groupBox_filter.TabStop = false;
            // 
            // comboBox_voice_id
            // 
            this.comboBox_voice_id.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.comboBox_voice_id.FormattingEnabled = true;
            this.comboBox_voice_id.Location = new System.Drawing.Point(95, 68);
            this.comboBox_voice_id.Name = "comboBox_voice_id";
            this.comboBox_voice_id.Size = new System.Drawing.Size(121, 28);
            this.comboBox_voice_id.TabIndex = 11;
            // 
            // label_voice_id
            // 
            this.label_voice_id.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label_voice_id.ForeColor = System.Drawing.Color.White;
            this.label_voice_id.Location = new System.Drawing.Point(3, 73);
            this.label_voice_id.Name = "label_voice_id";
            this.label_voice_id.Size = new System.Drawing.Size(88, 20);
            this.label_voice_id.TabIndex = 12;
            this.label_voice_id.Text = " 聲音種類:";
            this.label_voice_id.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // button_air_raid_alarm_on
            // 
            this.button_air_raid_alarm_on.ForeColor = System.Drawing.Color.Black;
            this.button_air_raid_alarm_on.Image = ((System.Drawing.Image)(resources.GetObject("button_air_raid_alarm_on.Image")));
            this.button_air_raid_alarm_on.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.button_air_raid_alarm_on.Location = new System.Drawing.Point(374, 62);
            this.button_air_raid_alarm_on.Name = "button_air_raid_alarm_on";
            this.button_air_raid_alarm_on.Size = new System.Drawing.Size(195, 40);
            this.button_air_raid_alarm_on.TabIndex = 10;
            this.button_air_raid_alarm_on.Text = "Air Raid Alarm On";
            this.button_air_raid_alarm_on.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.button_air_raid_alarm_on.UseVisualStyleBackColor = true;
            this.button_air_raid_alarm_on.Click += new System.EventHandler(this.button_air_raid_alarm_on_Click);
            // 
            // button_air_raid_alarm_off
            // 
            this.button_air_raid_alarm_off.ForeColor = System.Drawing.Color.Black;
            this.button_air_raid_alarm_off.Image = ((System.Drawing.Image)(resources.GetObject("button_air_raid_alarm_off.Image")));
            this.button_air_raid_alarm_off.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.button_air_raid_alarm_off.Location = new System.Drawing.Point(575, 62);
            this.button_air_raid_alarm_off.Name = "button_air_raid_alarm_off";
            this.button_air_raid_alarm_off.Size = new System.Drawing.Size(195, 40);
            this.button_air_raid_alarm_off.TabIndex = 9;
            this.button_air_raid_alarm_off.Text = "Air Raid Alarm Off";
            this.button_air_raid_alarm_off.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.button_air_raid_alarm_off.UseVisualStyleBackColor = true;
            this.button_air_raid_alarm_off.Click += new System.EventHandler(this.button_air_raid_alarm_off_Click);
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
            this.label_seat.Location = new System.Drawing.Point(6, 31);
            this.label_seat.Name = "label_seat";
            this.label_seat.Size = new System.Drawing.Size(85, 20);
            this.label_seat.TabIndex = 8;
            this.label_seat.Text = "操控席位:";
            this.label_seat.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // AirRaidBroadcast
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
            this.Name = "AirRaidBroadcast";
            this.Text = "Air Raid Broadcast";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.AirRaidBroadcast_FormClosing);
            this.Load += new System.EventHandler(this.AirRaidBroadcast_Load);
            this.groupBox_liveBCstatus.ResumeLayout(false);
            this.groupBox_filter.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button but_close;
        private System.Windows.Forms.GroupBox groupBox_liveBCstatus;
        private System.Windows.Forms.ListBox listBox_Live_Bc_Status;
        private System.Windows.Forms.GroupBox groupBox_filter;
        private System.Windows.Forms.Button button_air_raid_alarm_on;
        private System.Windows.Forms.Button button_air_raid_alarm_off;
        private System.Windows.Forms.ComboBox comboBox_seats;
        private System.Windows.Forms.Label label_seat;
        private System.Windows.Forms.ComboBox comboBox_voice_id;
        private System.Windows.Forms.Label label_voice_id;
    }
}