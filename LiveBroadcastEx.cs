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
    public partial class LiveBroadcastEx : Form
    {
        private string fileName_ = MethodBase.GetCurrentMethod().DeclaringType.Name;
        private string sysErr_ = "Live Broadcast";
        private IEnumerable<StationBroadcast> stations_ = null;

        public LiveBroadcastEx(IEnumerable<StationBroadcast> stations)
        {
            if (stations == null || stations.Count<StationBroadcast>() == 0)
            {
                throw new SystemException();
            }

            stations_ = stations;

            InitializeComponent();
        }

        private void LiveBroadcastEx_Load(object sender, EventArgs e)
        {
            localizeForm();

            comboBox_seats.DataSource = Program.seats_;
            comboBox_seats.DisplayMember = "Value";
            comboBox_seats.ValueMember = "Key";
            comboBox_seats.SelectedIndex = 0;

            DisplayBroadcastStatus(Program.liveStatusList_);

            PaService.Instance.OnStartLiveBroadcastRespond += PaService_OnStartLiveBroadcastRespond;
            PaService.Instance.OnEndLiveBroadcastRespond += PaService_OnEndLiveBroadcastRespond;
        }

        private void PaService_OnEndLiveBroadcastRespond(object sender, List<KeyValuePair<byte, bool>> e)
        {
            try
            {
                foreach (var item in e)
                {
                    if (!Program.stnNoToStnNameDic_.ContainsKey(item.Key))
                    {
                        continue;
                    }

                    string result = Localization.localize(item.Value ? "EVT_LIVE_END_SUCCESS" : "EVT_LIVE_END_FAIL");
                    string info = $"{Program.stnNoToStnNameDic_[item.Key]}:   {result} ";
                    Program.liveStatusList_.Add(info);
                }

                DisplayBroadcastStatus(Program.liveStatusList_);
            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }
        }

        private void PaService_OnStartLiveBroadcastRespond(object sender, List<KeyValuePair<byte, bool>> e)
        {
            try
            {
                foreach (var item in e)
                {
                    if (!Program.stnNoToStnNameDic_.ContainsKey(item.Key))
                    {
                        continue;
                    }

                    string result = Localization.localize(item.Value ? "EVT_LIVE_START_SUCCESS" : "EVT_LIVE_START_FAIL");
                    string info = $"{Program.stnNoToStnNameDic_[item.Key]}:   {result} ";
                    Program.liveStatusList_.Add(info);
                }
                DisplayBroadcastStatus(Program.liveStatusList_);
            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }
        }

        private void LiveBroadcastEx_FormClosing(object sender, FormClosingEventArgs e)
        {
            PaService.Instance.OnStartLiveBroadcastRespond -= PaService_OnStartLiveBroadcastRespond;
            PaService.Instance.OnEndLiveBroadcastRespond -= PaService_OnEndLiveBroadcastRespond;
        }

        private void localizeForm()
        {
            System.Globalization.CultureInfo ci = new System.Globalization.CultureInfo(Program.defaultLanguage);
            Localization.setCulture(ci);
            Localization.setApplicationName(Program.moduleName);

            if (Localization.configure())
            {
                this.Text = Localization.localize("TXT_LIVE_BROADCAST", this.Text);
                sysErr_ = Localization.localize("TXT_LIVE_BROADCAST", sysErr_);

                label_seat.Text = Localization.localize("LABLE_SEAT", label_seat.Text);
                groupBox_liveBCstatus.Text = Localization.localize("GP_LIVE_BC_STATUS", groupBox_liveBCstatus.Text);
                but_close.Text = Localization.localize("BUT_CLOSE", but_close.Text);
                button_live_start.Text = Localization.localize("BUT_LIVE_START", button_live_start.Text);
                button_live_end.Text = Localization.localize("BUT_LIVE_END", button_live_end.Text);
            }
        }

        private void DisplayBroadcastStatus(List<string> statusList)
        {
            listBox_Live_Bc_Status.BeginUpdate();
            try
            {
                listBox_Live_Bc_Status.Items.Clear();
                foreach (var item in statusList)
                {
                    listBox_Live_Bc_Status.Items.Add(item);
                }
            }
            finally
            {
                listBox_Live_Bc_Status.EndUpdate();
            }
            listBox_Live_Bc_Status.TopIndex = listBox_Live_Bc_Status.Items.Count - 1;
        }

        private async void button_live_start_Click(object sender, EventArgs e)
        {
            try
            {
                byte seatId = Convert.ToByte(comboBox_seats.SelectedValue);
                M61 cmd = new M61(seatId, stations_);

                bool result = await PaService.Instance.StartLiveBroadcast(cmd);

                if (!result)
                {
                    Program.MessageBox_Error(PAManager.mbSendCmdFailed_, sysErr_);
                }
            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }
        }

        private async void button_live_end_Click(object sender, EventArgs e)
        {
            try
            {
                byte seatId = Convert.ToByte(comboBox_seats.SelectedValue);
                List<byte> stationIds = stations_.Select(stn => stn.StationId).ToList();

                M62 cmd = new M62(seatId, stationIds);

                bool result = await PaService.Instance.EndLiveBroadcast(cmd);

                if (!result)
                {
                    Program.MessageBox_Error(PAManager.mbSendCmdFailed_, sysErr_);
                }
            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }
        }

        private void but_close_Click(object sender, EventArgs e)
        {
            this.Close();
        }

    }
}
