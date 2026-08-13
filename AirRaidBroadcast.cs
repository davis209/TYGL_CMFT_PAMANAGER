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
    public partial class AirRaidBroadcast : Form
    {
        private string fileName_ = MethodBase.GetCurrentMethod().DeclaringType.Name;
        private string sysErr_ = "Air Raid Alarm Broadcast";
        private IEnumerable<byte> stations_ = null;

        public AirRaidBroadcast(IEnumerable<byte> stations)
        {
            if (stations == null || stations.Count() == 0)
            {
                throw new SystemException();
            }

            stations_ = stations;
            InitializeComponent();
        }

        private void AirRaidBroadcast_Load(object sender, EventArgs e)
        {
            localizeForm();

            comboBox_seats.DataSource = Program.seats_;
            comboBox_seats.DisplayMember = "Value";
            comboBox_seats.ValueMember = "Key";
            comboBox_seats.SelectedIndex = 0;

            comboBox_voice_id.Items.Add(1);
            comboBox_voice_id.Items.Add(2);
            comboBox_voice_id.Items.Add(3);
            comboBox_voice_id.Items.Add(4);
            comboBox_voice_id.Items.Add(5);
            comboBox_voice_id.SelectedIndex = 0;

            DisplayBroadcastStatus(Program.airRaidStatusList_);

            PaService.Instance.OnStartAirRaidAlarmRespond += PaService_OnStartAirRaidAlarmRespond;
            PaService.Instance.OnEndAirRaidAlarmRespond += PaService_OnEndAirRaidAlarmRespond;
        }

        private void PaService_OnEndAirRaidAlarmRespond(object sender, List<KeyValuePair<byte, bool>> e)
        {
            try
            {
                foreach (var item in e)
                {
                    if (!Program.stnNoToStnNameDic_.ContainsKey(item.Key))
                    {
                        continue;
                    }

                    string result = Localization.localize(item.Value ? "EVT_AIRRAID_ALARM_END_SUCCESS" : "EVT_AIRRAID_ALARM_END_FAIL");
                    string info = $"{Program.stnNoToStnNameDic_[item.Key]}:   {result} ";
                    Program.airRaidStatusList_.Add(info);
                }

                DisplayBroadcastStatus(Program.airRaidStatusList_);
            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }
        }

        private void PaService_OnStartAirRaidAlarmRespond(object sender, List<KeyValuePair<byte, bool>> e)
        {
            try
            {
                foreach (var item in e)
                {
                    if (!Program.stnNoToStnNameDic_.ContainsKey(item.Key))
                    {
                        continue;
                    }

                    string result = Localization.localize(item.Value ? "EVT_AIRRAID_ALARM_START_SUCCESS" : "EVT_AIRRAID_ALARM_START_FAIL");
                    string info = $"{Program.stnNoToStnNameDic_[item.Key]}:   {result} ";
                    Program.airRaidStatusList_.Add(info);
                }

                DisplayBroadcastStatus(Program.airRaidStatusList_);
            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }
        }

        private void AirRaidBroadcast_FormClosing(object sender, FormClosingEventArgs e)
        {
            PaService.Instance.OnStartAirRaidAlarmRespond -= PaService_OnStartAirRaidAlarmRespond;
            PaService.Instance.OnEndAirRaidAlarmRespond -= PaService_OnEndAirRaidAlarmRespond;
        }

        private void localizeForm()
        {
            System.Globalization.CultureInfo ci = new System.Globalization.CultureInfo(Program.defaultLanguage);
            Localization.setCulture(ci);
            Localization.setApplicationName(Program.moduleName);

            if (Localization.configure())
            {
                this.Text = Localization.localize("TXT_AIR_RAID_BROADCAST", this.Text);
                sysErr_ = Localization.localize("TXT_AIR_RAID_BROADCAST", sysErr_);

                label_seat.Text = Localization.localize("LABLE_SEAT", label_seat.Text);
                label_voice_id.Text = Localization.localize("LABLE_VOICE_ID", label_voice_id.Text);
                groupBox_liveBCstatus.Text = Localization.localize("GP_LIVE_BC_STATUS", groupBox_liveBCstatus.Text);
                but_close.Text = Localization.localize("BUT_CLOSE", but_close.Text);
                button_air_raid_alarm_on.Text = Localization.localize("BUT_AIR_RAID_ALARM_ON", button_air_raid_alarm_on.Text);
                button_air_raid_alarm_off.Text = Localization.localize("BUT_AIR_RAID_ALARM_OFF", button_air_raid_alarm_off.Text);
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

        private async void button_air_raid_alarm_on_Click(object sender, EventArgs e)
        {
            try
            {
                if (!RequestPasswordVerification()) return;

                byte seatId = Convert.ToByte(comboBox_seats.SelectedValue);
                byte voiceId = Convert.ToByte(comboBox_voice_id.SelectedItem);

                List<StationAirRaid> stationList = stations_.Select(stn => new StationAirRaid(stn, voiceId)).ToList();

                M63 cmd = new M63(seatId, stationList);

                bool result = await PaService.Instance.StartAirRaidAlarm(cmd);

                WriteAirRaidAudit("START", seatId, result);

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

        private async void button_air_raid_alarm_off_Click(object sender, EventArgs e)
        {
            try
            {
                if (!RequestPasswordVerification()) return;

                byte seatId = Convert.ToByte(comboBox_seats.SelectedValue);

                M64 cmd = new M64(seatId, stations_);

                bool result = await PaService.Instance.EndAirRaidAlarm(cmd);

                WriteAirRaidAudit("END", seatId, result);

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

        private bool RequestPasswordVerification()
        {
            using (AirRaidPasswordForm dialog = new AirRaidPasswordForm())
            {
                bool verified = dialog.ShowDialog(this) == DialogResult.OK;
                if (!verified)
                {
                    Program.WriteEventLog("[WARN] Air-raid command cancelled or password verification failed. Operator=[" + Program.localUser + "]",
                        fileName_ + ".RequestPasswordVerification()");
                }
                return verified;
            }
        }

        private void WriteAirRaidAudit(string operation, byte seatId, bool sendResult)
        {
            string stations = String.Join(",", stations_.Select(station => station.ToString()));
            Program.WriteEventLog("[AUDIT] Air-raid command. Operator=[" + Program.localUser + "] Operation=[" + operation +
                "] Seat=[" + seatId + "] Stations=[" + stations + "] SendResult=[" + sendResult + "]",
                fileName_ + ".WriteAirRaidAudit()");
        }

    }
}
