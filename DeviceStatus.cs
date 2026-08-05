using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Reflection;

using ste.common.exceptions;
using ste.common.util;


namespace ste.pa.pamanager
{
    public partial class DeviceStatus : Form
    {
        private string fileName_ = MethodBase.GetCurrentMethod().DeclaringType.Name;
        private string sysErr_ = "Device Status";
        private static List<KeyValuePair<int, string>> stations_ = new List<KeyValuePair<int, string>>();
        private Dictionary<byte, bool[]> deviceStatusDic_ = new Dictionary<byte, bool[]>();

        private string deviceStatus_Normal_ = "Normal";
        private string deviceStatus_Fail_ = "Fail";
        private string lv_header_location_ = "Location";
        private string lv_header_device_name_ = "Device Name";
        private string lv_header_status_ = "Status";

        public DeviceStatus()
        {
            InitializeComponent();
        }

        private async void DeviceStatus_Load(object sender, EventArgs e)
        {
            PaService.Instance.OnDeviceStatusReceived += Instance_OnDeviceStatusReceived;

            try
            {
                //send query status command
                M51 cmd = new M51(new byte[] { 0 });
                bool result = await PaService.Instance.QueryDeviceStatus(cmd);
            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }

            localizeForm();

            Init_ListView_DeviceStatus();

            stations_ = Program.stnNoToStnNameDic_
            .Where(x => x.Value != "NDOCC" && x.Value != "TYOCC" && x.Value != "OVERALL" && x.Value != "LOCAL")
            .Select(x => new KeyValuePair<int, string>(x.Key, x.Value)).ToList();
            stations_.Insert(0, new KeyValuePair<int, string>(0, "OVERALL"));

            comboBox_station.DataSource = stations_;
            comboBox_station.DisplayMember = "Value";
            comboBox_station.ValueMember = "Key";
            comboBox_station.SelectedIndex = 0;

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
                this.Text = Localization.localize("BUT_DEVICE_STATUS", this.Text);
                sysErr_ = Localization.localize("BUT_DEVICE_STATUS", sysErr_);

                lv_header_location_ = Localization.localize("TXT_LOCATION", lv_header_location_);
                lv_header_device_name_ = Localization.localize("TXT_DEVICE_NAME", lv_header_device_name_);
                lv_header_status_ = Localization.localize("TXT_STATUS", lv_header_status_);

                deviceStatus_Normal_ = Localization.localize("TXT_DEVICE_STATUS_NORMAL", deviceStatus_Normal_);
                deviceStatus_Normal_ = Localization.localize("TXT_DEVICE_STATUS_NORMAL", deviceStatus_Normal_);
                deviceStatus_Fail_ = Localization.localize("TXT_DEVICE_STATUS_FAIL", deviceStatus_Fail_);
                button_query_device_status.Text = Localization.localize("BUT_QUERY_DEVICE_STATUS", button_query_device_status.Text);
                but_close.Text = Localization.localize("BUT_CLOSE", but_close.Text);
                label_station.Text = Localization.localize("LABLE_STATION", label_station.Text);
                groupBox_device_status.Text = Localization.localize("BUT_DEVICE_STATUS", groupBox_device_status.Text);              
            }
        }

        private void Instance_OnDeviceStatusReceived(object sender, Dictionary<byte, bool[]> e)
        {
            Dictionary<byte, bool[]> receivedData = e as Dictionary<byte, bool[]>;

            if (receivedData == null)
            {
                return;
            }

            foreach (var kvp in receivedData)
            {
                bool[] copiedArray = kvp.Value?.ToArray() ?? Array.Empty<bool>();

                if (deviceStatusDic_.ContainsKey(kvp.Key))
                {
                    deviceStatusDic_[kvp.Key] = copiedArray;
                }
                else
                {
                    deviceStatusDic_.Add(kvp.Key, copiedArray);
                }
            }

            DisplayDeviceStatus();
        }

        private void Init_ListView_DeviceStatus()
        {
            listView_device_status.View = View.Details;
            listView_device_status.Columns.Add(lv_header_location_, 120);
            listView_device_status.Columns.Add(lv_header_device_name_, 220);
            listView_device_status.Columns.Add(lv_header_status_, 120);
        }

        private void comboBox_station_SelectedIndexChanged(object sender, EventArgs e)
        {
            DisplayDeviceStatus();
        }

        private void DisplayDeviceStatus()
        {
            listView_device_status.BeginUpdate();
            try
            {
                listView_device_status.Items.Clear();

                var selectedStation = (KeyValuePair<int, string>)comboBox_station.SelectedItem;
                if (selectedStation.Key == 0)
                {
                    foreach (var kvp in deviceStatusDic_)
                    {
                        if (!Program.stnNoToStnNameDic_.ContainsKey(kvp.Key))
                        {
                            continue;
                        }
                        string stationName = Program.stnNoToStnNameDic_[kvp.Key];
                        for (int index = 0; index < kvp.Value.Length; index++)
                        {
                            if (!Program.deviceNameDic_.ContainsKey(index))
                            {
                                continue;
                            }
                            AddDevice(stationName, Program.deviceNameDic_[index], kvp.Value[index]);
                        }
                    }
                }
                else
                {
                    if (!deviceStatusDic_.ContainsKey((byte)selectedStation.Key))
                    {
                        return;
                    }

                    if (!Program.stnNoToStnNameDic_.ContainsKey(selectedStation.Key))
                    {
                        return;
                    }

                    string stationName = Program.stnNoToStnNameDic_[selectedStation.Key];
                    for (int index = 0; index < deviceStatusDic_[(byte)selectedStation.Key].Length; index++)
                    {
                        if (!Program.deviceNameDic_.ContainsKey(index))
                        {
                            continue;
                        }

                        AddDevice(stationName, Program.deviceNameDic_[index], deviceStatusDic_[(byte)selectedStation.Key][index]);
                    }
                }
            }
            finally
            {
                listView_device_status.EndUpdate();
            }
            //listView_device_status.Refresh();
        }

        private void AddDevice(string stationName, string deviceName, bool isAbnormal)
        {
            ListViewItem item = new ListViewItem(stationName);
            item.SubItems.Add(deviceName);
            item.SubItems.Add(isAbnormal ? deviceStatus_Fail_ : deviceStatus_Normal_);

            item.ForeColor = isAbnormal ? Color.Red : Color.Black;
            listView_device_status.Items.Add(item);
        }

        private void DeviceStatus_FormClosing(object sender, FormClosingEventArgs e)
        {
            PaService.Instance.OnDeviceStatusReceived -= Instance_OnDeviceStatusReceived;
        }

        private void but_close_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private async void button_query_device_status_Click(object sender, EventArgs e)
        {
            try
            {
                //send query status command
                M51 cmd = new M51(new byte[] { 0 });
                bool result = await PaService.Instance.QueryDeviceStatus(cmd);
                if (result)
                {
                    Program.MessageBox_Normal(PAManager.mbSendMsgSuccessful_, sysErr_, this);
                }
                else
                {
                    Program.MessageBox_Error(PAManager.mbSendMsgFailed_, sysErr_);
                }
            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }

        }
    }
}
