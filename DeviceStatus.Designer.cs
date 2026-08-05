namespace ste.pa.pamanager
{
    partial class DeviceStatus
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(DeviceStatus));
            this.listView_device_status = new System.Windows.Forms.ListView();
            this.but_close = new System.Windows.Forms.Button();
            this.comboBox_station = new System.Windows.Forms.ComboBox();
            this.label_station = new System.Windows.Forms.Label();
            this.groupBox_filter = new System.Windows.Forms.GroupBox();
            this.button_query_device_status = new System.Windows.Forms.Button();
            this.groupBox_device_status = new System.Windows.Forms.GroupBox();
            this.groupBox_filter.SuspendLayout();
            this.groupBox_device_status.SuspendLayout();
            this.SuspendLayout();
            // 
            // listView_device_status
            // 
            this.listView_device_status.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.listView_device_status.HideSelection = false;
            this.listView_device_status.Location = new System.Drawing.Point(6, 25);
            this.listView_device_status.Name = "listView_device_status";
            this.listView_device_status.Size = new System.Drawing.Size(763, 273);
            this.listView_device_status.TabIndex = 0;
            this.listView_device_status.UseCompatibleStateImageBehavior = false;
            // 
            // but_close
            // 
            this.but_close.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F);
            this.but_close.Image = ((System.Drawing.Image)(resources.GetObject("but_close.Image")));
            this.but_close.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.but_close.Location = new System.Drawing.Point(668, 395);
            this.but_close.Name = "but_close";
            this.but_close.Size = new System.Drawing.Size(120, 43);
            this.but_close.TabIndex = 6;
            this.but_close.Text = "Close";
            this.but_close.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.but_close.UseVisualStyleBackColor = true;
            this.but_close.Click += new System.EventHandler(this.but_close_Click);
            // 
            // comboBox_station
            // 
            this.comboBox_station.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.comboBox_station.FormattingEnabled = true;
            this.comboBox_station.Location = new System.Drawing.Point(95, 27);
            this.comboBox_station.Name = "comboBox_station";
            this.comboBox_station.Size = new System.Drawing.Size(121, 28);
            this.comboBox_station.TabIndex = 7;
            this.comboBox_station.SelectedIndexChanged += new System.EventHandler(this.comboBox_station_SelectedIndexChanged);
            // 
            // label_station
            // 
            this.label_station.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label_station.ForeColor = System.Drawing.SystemColors.ControlText;
            this.label_station.Location = new System.Drawing.Point(7, 31);
            this.label_station.Name = "label_station";
            this.label_station.Size = new System.Drawing.Size(84, 20);
            this.label_station.TabIndex = 8;
            this.label_station.Text = "Station:";
            this.label_station.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // groupBox_filter
            // 
            this.groupBox_filter.Controls.Add(this.button_query_device_status);
            this.groupBox_filter.Controls.Add(this.comboBox_station);
            this.groupBox_filter.Controls.Add(this.label_station);
            this.groupBox_filter.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.groupBox_filter.ForeColor = System.Drawing.SystemColors.ControlText;
            this.groupBox_filter.Location = new System.Drawing.Point(12, 3);
            this.groupBox_filter.Name = "groupBox_filter";
            this.groupBox_filter.Size = new System.Drawing.Size(776, 76);
            this.groupBox_filter.TabIndex = 10;
            this.groupBox_filter.TabStop = false;
            // 
            // button_query_device_status
            // 
            this.button_query_device_status.ForeColor = System.Drawing.Color.Black;
            this.button_query_device_status.Image = ((System.Drawing.Image)(resources.GetObject("button_query_device_status.Image")));
            this.button_query_device_status.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.button_query_device_status.Location = new System.Drawing.Point(575, 21);
            this.button_query_device_status.Name = "button_query_device_status";
            this.button_query_device_status.Size = new System.Drawing.Size(195, 40);
            this.button_query_device_status.TabIndex = 9;
            this.button_query_device_status.Text = "Query Device Status";
            this.button_query_device_status.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.button_query_device_status.UseVisualStyleBackColor = true;
            this.button_query_device_status.Click += new System.EventHandler(this.button_query_device_status_Click);
            // 
            // groupBox_device_status
            // 
            this.groupBox_device_status.Controls.Add(this.listView_device_status);
            this.groupBox_device_status.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.groupBox_device_status.ForeColor = System.Drawing.SystemColors.ControlText;
            this.groupBox_device_status.Location = new System.Drawing.Point(13, 85);
            this.groupBox_device_status.Name = "groupBox_device_status";
            this.groupBox_device_status.Size = new System.Drawing.Size(775, 304);
            this.groupBox_device_status.TabIndex = 11;
            this.groupBox_device_status.TabStop = false;
            this.groupBox_device_status.Text = "Device Status";
            // 
            // DeviceStatus
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.Control;
            this.ClientSize = new System.Drawing.Size(800, 450);
            this.Controls.Add(this.groupBox_device_status);
            this.Controls.Add(this.groupBox_filter);
            this.Controls.Add(this.but_close);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "DeviceStatus";
            this.Text = "Device Status";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.DeviceStatus_FormClosing);
            this.Load += new System.EventHandler(this.DeviceStatus_Load);
            this.groupBox_filter.ResumeLayout(false);
            this.groupBox_device_status.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ListView listView_device_status;
        private System.Windows.Forms.Button but_close;
        private System.Windows.Forms.ComboBox comboBox_station;
        private System.Windows.Forms.Label label_station;
        private System.Windows.Forms.GroupBox groupBox_filter;
        private System.Windows.Forms.GroupBox groupBox_device_status;
        private System.Windows.Forms.Button button_query_device_status;
    }
}
