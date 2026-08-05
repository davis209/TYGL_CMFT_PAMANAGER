using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ste.pa.pamanager
{
    public class PaService
    {
        #region Singleton required implementations

        public event EventHandler OnDvaStatusChanged;
        public event EventHandler<string> OnRadioListVersionReceived;
        public event EventHandler<List<KeyValuePair<byte, int>>> OnStationStatusReceived;
        public event EventHandler<Dictionary<byte/*stationId*/, bool[]>> OnDeviceStatusReceived;
        public event EventHandler<List<KeyValuePair<byte, bool>>> OnStartLiveBroadcastRespond;
        public event EventHandler<List<KeyValuePair<byte, bool>>> OnEndLiveBroadcastRespond;
        public event EventHandler<List<KeyValuePair<byte, bool>>> OnStartAirRaidAlarmRespond;
        public event EventHandler<List<KeyValuePair<byte, bool>>> OnEndAirRaidAlarmRespond;

        private static volatile PaService instance_ = null;
        private static object syncRoot = new Object();
        private string _fileName;
        private PaTcpClient _client = null;
        private bool _isConnected = false;
        private DateTime _lastHeartbeatTime = DateTime.MinValue;
        private const int HEARTBEAT_TIMEOUT = 15; //second
        private BroadcastStatusEnum _broadcastStatus = BroadcastStatusEnum.Completed;

        public static PaService Instance
        {
            get
            {
                if (instance_ == null)
                {
                    lock (syncRoot)
                    {
                        if (instance_ == null)
                            instance_ = new PaService();
                    }
                }

                return instance_;
            }
        }

        public void Iniatilize(string priIP, string secIP, int port, int consoleId)
        {
            _client = new PaTcpClient(priIP, secIP, port, consoleId);

            _client.OnHeartbeatAckReceived += _client_OnHeartbeatAckReceived;
            _client.OnConnected += _client_OnConnected;
            _client.OnDisconnected += _client_OnDisconnected;
            _client.OnMessageReceived += _client_OnMessageReceived;
            _client.Start();
        }

        private void _client_OnMessageReceived(object sender, ReceivedPacket recvPacket)
        {
            byte[] packet = recvPacket.Packet;
            // M31, play interrupted
            if (packet.Length > PaMessageHeader.HEADER_LENGTH  && packet[PaMessageHeader.HEADER_LENGTH] == PaTcpClient.PACKET_NUM_M31)
            {
                _broadcastStatus = BroadcastStatusEnum.Interrupted;
                _client.SendMessagePacket(recvPacket.Sequence, new A31(packet[PaMessageHeader.HEADER_LENGTH + 1]));
                OnDvaStatusChanged?.Invoke(this, EventArgs.Empty);
            }
            else if (packet.Length > PaMessageHeader.HEADER_LENGTH && packet[PaMessageHeader.HEADER_LENGTH] == PaTcpClient.PACKET_NUM_M46)
            {
                _broadcastStatus = BroadcastStatusEnum.Completed;
                _client.SendMessagePacket(recvPacket.Sequence, new A46(packet[PaMessageHeader.HEADER_LENGTH + 1]));
                OnDvaStatusChanged?.Invoke(this, EventArgs.Empty);
            }
            else if (packet.Length > PaMessageHeader.HEADER_LENGTH && packet[PaMessageHeader.HEADER_LENGTH] == PaTcpClient.PACKET_NUM_A142)
            {
                string result = Encoding.ASCII.GetString(packet, PaMessageHeader.HEADER_LENGTH + 1, packet.Length - PaMessageHeader.HEADER_LENGTH - 1);
                OnRadioListVersionReceived?.Invoke(this, result);
            }
            else if (packet.Length > PaMessageHeader.HEADER_LENGTH && packet[PaMessageHeader.HEADER_LENGTH] == PaTcpClient.PACKET_NUM_A1)
            {
                int stationNum = packet[PaMessageHeader.HEADER_LENGTH + 1];
                List<KeyValuePair<byte, int>> statusList = new List<KeyValuePair<byte, int>>();
                for (int i = 0; i < stationNum; i++)
                {
                    statusList.Add(new KeyValuePair<byte, int>(packet[PaMessageHeader.HEADER_LENGTH + 2 + i * 2], packet[PaMessageHeader.HEADER_LENGTH + 2 + i * 2 + 1]));
                }
                
                OnStationStatusReceived?.Invoke(this, statusList);
            }
            else if (packet.Length > PaMessageHeader.HEADER_LENGTH && packet[PaMessageHeader.HEADER_LENGTH] == PaTcpClient.PACKET_NUM_M132)
            {
                byte stationId = packet[PaMessageHeader.HEADER_LENGTH + 1];
                _client.SendMessagePacket(recvPacket.Sequence, new A32(stationId));
            }
            else if (packet.Length > PaMessageHeader.HEADER_LENGTH && packet[PaMessageHeader.HEADER_LENGTH] == PaTcpClient.PACKET_NUM_M133)
            {
                _client.SendMessagePacket(recvPacket.Sequence, new A33());
            }
            else if (packet.Length > PaMessageHeader.HEADER_LENGTH && packet[PaMessageHeader.HEADER_LENGTH] == PaTcpClient.PACKET_NUM_M134)
            {
                _client.SendMessagePacket(recvPacket.Sequence, new A34());
            }
            else if (packet.Length > PaMessageHeader.HEADER_LENGTH && packet[PaMessageHeader.HEADER_LENGTH] == PaTcpClient.PACKET_NUM_M152)
            {
                byte stationId = packet[PaMessageHeader.HEADER_LENGTH + 1];
                bool[] statusArr = new bool[24];

                for (int i = 0; i < 3; i++)
                {
                    for(int j = 0; j < 8; j++)
                    {
                        statusArr[i * 8 + j] = ((byte)(packet[PaMessageHeader.HEADER_LENGTH + 2 + i] >> j) & 1) == 1;
                    }
                }

                Dictionary<byte, bool[]> deviceStatus = new Dictionary<byte, bool[]>();
                deviceStatus[stationId] = statusArr;

                _client.SendMessagePacket(recvPacket.Sequence, new A52());

                OnDeviceStatusReceived?.Invoke(this, deviceStatus);
            }
            else if (packet.Length > PaMessageHeader.HEADER_LENGTH && packet[PaMessageHeader.HEADER_LENGTH] == PaTcpClient.PACKET_NUM_A51)
            {
                Dictionary<byte, bool[]> deviceStatus = new Dictionary<byte, bool[]>();
                int stationCount = packet[PaMessageHeader.HEADER_LENGTH + 1];
                int pos = PaMessageHeader.HEADER_LENGTH + 2;
                for (int k = 0; k < stationCount; k++)
                {
                    byte stationId = packet[pos];
                    pos += 1;

                    bool[] statusArr = new bool[24];
                    for (int i = 0; i < 3; i++)
                    {
                        for (int j = 0; j < 8; j++)
                        {
                            statusArr[i * 8 + j] = ((byte)(packet[pos + i] >> j) & 1) == 1;
                        }
                    }
                    pos += 3;

                    deviceStatus[stationId] = statusArr;
                }

                OnDeviceStatusReceived?.Invoke(this, deviceStatus);
            }
            else if (packet.Length > PaMessageHeader.HEADER_LENGTH && packet[PaMessageHeader.HEADER_LENGTH] == PaTcpClient.PACKET_NUM_A61)
            {
                int stationNum = packet[PaMessageHeader.HEADER_LENGTH + 1];
                List<KeyValuePair<byte, bool>> result = new List<KeyValuePair<byte, bool>>();
                for (int i = 0; i < stationNum; i++)
                {
                    result.Add(new KeyValuePair<byte, bool>(packet[PaMessageHeader.HEADER_LENGTH + 2 + i * 2], packet[PaMessageHeader.HEADER_LENGTH + 2 + i * 2 + 1] == 0));
                }

                OnStartLiveBroadcastRespond?.Invoke(this, result);
            }
            else if (packet.Length > PaMessageHeader.HEADER_LENGTH && packet[PaMessageHeader.HEADER_LENGTH] == PaTcpClient.PACKET_NUM_A62)
            {
                int stationNum = packet[PaMessageHeader.HEADER_LENGTH + 1];
                List<KeyValuePair<byte, bool>> result = new List<KeyValuePair<byte, bool>>();
                for (int i = 0; i < stationNum; i++)
                {
                    result.Add(new KeyValuePair<byte, bool>(packet[PaMessageHeader.HEADER_LENGTH + 2 + i * 2], packet[PaMessageHeader.HEADER_LENGTH + 2 + i * 2 + 1] == 0));
                }

                OnEndLiveBroadcastRespond?.Invoke(this, result);
            }
            else if (packet.Length > PaMessageHeader.HEADER_LENGTH && packet[PaMessageHeader.HEADER_LENGTH] == PaTcpClient.PACKET_NUM_A63)
            {
                int stationNum = packet[PaMessageHeader.HEADER_LENGTH + 1];
                List<KeyValuePair<byte, bool>> result = new List<KeyValuePair<byte, bool>>();
                for (int i = 0; i < stationNum; i++)
                {
                    result.Add(new KeyValuePair<byte, bool>(packet[PaMessageHeader.HEADER_LENGTH + 2 + i * 2], packet[PaMessageHeader.HEADER_LENGTH + 2 + i * 2 + 1] == 0));
                }

                OnStartAirRaidAlarmRespond?.Invoke(this, result);
            }
            else if (packet.Length > PaMessageHeader.HEADER_LENGTH && packet[PaMessageHeader.HEADER_LENGTH] == PaTcpClient.PACKET_NUM_A64)
            {
                int stationNum = packet[PaMessageHeader.HEADER_LENGTH + 1];
                List<KeyValuePair<byte, bool>> result = new List<KeyValuePair<byte, bool>>();
                for (int i = 0; i < stationNum; i++)
                {
                    result.Add(new KeyValuePair<byte, bool>(packet[PaMessageHeader.HEADER_LENGTH + 2 + i * 2], packet[PaMessageHeader.HEADER_LENGTH +2 + i * 2 + 1] == 0));
                }

                OnEndAirRaidAlarmRespond?.Invoke(this, result);
            }
        }

        private void _client_OnDisconnected(object sender, EventArgs e)
        {
            _isConnected = false;
        }

        private void _client_OnConnected(object sender, EventArgs e)
        {
            _isConnected = true;
        }

        private void _client_OnHeartbeatAckReceived(object sender, EventArgs e)
        {
            _lastHeartbeatTime = DateTime.Now;
        }

        public static void destroyInstance()
        {
            lock (syncRoot)
            {
                if (instance_ != null)
                {
                    instance_ = null;
                }
            }
        }
        #endregion

        private PaService()
        {
            _fileName = typeof(PaService).Name;
        }

        public async Task<bool> GetStationStatus(M1 cmd)
        {
            bool result = false;
            try
            {
                Utility.log("GetStationStatus triggering ", _fileName);
                result = await _client.GetStationStatusAndWaitResponse(cmd);
                Utility.log("GetStationStatus completed, result=" + result.ToString(), _fileName);
            }
            catch (Exception ex)
            {
                Utility.logError("StartBroadcast unexpected exception caught.", _fileName, ex);
            }
            return result;
        }

        public async Task<bool> SetVolume(M41 cmd)
        {
            bool result = false;
            try
            {
                result = await _client.SetVolumeAndWaitResponse(cmd);
            }
            catch (Exception ex)
            {
                Utility.logError("SetVolume unexpected exception caught.", _fileName, ex);
            }
            return result;
        }

        public async Task<bool> SetSchedule(M43 cmd)
        {
            bool result = false;
            try
            {
                result = await _client.SetScheduleAndWaitResponse(cmd);
            }
            catch (Exception ex)
            {
                Utility.logError("SetSchedule unexpected exception caught.", _fileName, ex);
            }
            return result;
        }

        public async Task<bool> StartBroadcast(M44 cmd)
        {
            bool result = false;
            try
            {
                Utility.log("StartBroadcast triggering ", _fileName);
                result = await _client.SendPlayRequestAndWaitResponse(cmd);
                if (result)
                {
                    _broadcastStatus = BroadcastStatusEnum.InProgress;
                }
                Utility.log("StartBroadcast completed, result=" + result.ToString(), _fileName);
            }
            catch (Exception ex)
            {
                Utility.logError("StartBroadcast unexpected exception caught.", _fileName, ex);
            }
            return result;
        }

        public async Task<bool> StopBroadcast(M45 cmd)
        {
            bool result = false;
            try
            {
                Utility.log("StopBroadcast triggering ", _fileName);
                result = await _client.SendStopPlayRequestAndWaitResponse(cmd);
                if (result)
                {
                    _broadcastStatus = BroadcastStatusEnum.Interrupted;
                }
                Utility.log("StopBroadcast completed, result=" + result.ToString(), _fileName);
            }
            catch (Exception ex)
            {
                Utility.logError("StopBroadcast unexpected exception caught.", _fileName, ex);
            }
            return result;
        }

        public async Task<bool> RequestRecord(M47 cmd)
        {
            bool result = false;
            try
            {
                result = await _client.RequestRecordAndWaitResponse(cmd);
            }
            catch (Exception ex)
            {
                Utility.logError("RequestRecord unexpected exception caught.", _fileName, ex);
            }
            return result;
        }

        public async Task<bool> StartRecord(M48 cmd)
        {
            bool result = false;
            try
            {
                result = await _client.StartRecordAndWaitResponse(cmd);
            }
            catch (Exception ex)
            {
                Utility.logError("StartRecord unexpected exception caught.", _fileName, ex);
            }
            return result;
        }

        public async Task<bool> EndRecord(M49 cmd)
        {
            bool result = false;
            try
            {
                result = await _client.EndRecordAndWaitResponse(cmd);
            }
            catch (Exception ex)
            {
                Utility.logError("EndRecord unexpected exception caught.", _fileName, ex);
            }
            return result;
        }

        public async Task<bool> QueryDeviceStatus(M51 cmd)
        {
            bool result = false;
            try
            {
                result = await _client.QueryDeviceStatusAndWaitResponse(cmd);
            }
            catch (Exception ex)
            {
                Utility.logError("RequestRecord unexpected exception caught.", _fileName, ex);
            }
            return result;
        }

        public async Task<bool> StartLiveBroadcast(M61 cmd)
        {
            bool result = false;
            try
            {
                result = await _client.StartLiveBroadcastAndWaitResponse(cmd);
            }
            catch (Exception ex)
            {
                Utility.logError("StartLiveBroadcast unexpected exception caught.", _fileName, ex);
            }
            return result;
        }

        public async Task<bool> EndLiveBroadcast(M62 cmd)
        {
            bool result = false;
            try
            {
                result = await _client.EndLiveBroadcastAndWaitResponse(cmd);
            }
            catch (Exception ex)
            {
                Utility.logError("EndLiveBroadcast unexpected exception caught.", _fileName, ex);
            }
            return result;
        }

        public async Task<bool> StartAirRaidAlarm(M63 cmd)
        {
            bool result = false;
            try
            {
                result = await _client.StartAirRaidAlarmAndWaitResponse(cmd);
            }
            catch (Exception ex)
            {
                Utility.logError("StartAirRaidAlarm unexpected exception caught.", _fileName, ex);
            }
            return result;
        }

        public async Task<bool> EndAirRaidAlarm(M64 cmd)
        {
            bool result = false;
            try
            {
                result = await _client.EndAirRaidAlarmAndWaitResponse(cmd);
            }
            catch (Exception ex)
            {
                Utility.logError("EndAirRaidAlarm unexpected exception caught.", _fileName, ex);
            }
            return result;
        }

        public async Task<bool> QueryMsgVersion()
        {
            bool result = false;
            try
            {
                Utility.log("Query Msg version triggering ", _fileName);
                result = await _client.QueryMsgVersionAndWaitResponse();
                Utility.log("Query Msg version completed, result=" + result.ToString(), _fileName);
            }
            catch (Exception ex)
            {
                Utility.logError("QueryMsgVersion unexpected exception caught.", _fileName, ex);
            }
            return result;
        }


        public BroadcastStatusEnum BroadcastStatus
        {
            get
            {
                return _broadcastStatus;
            }
        }

        public bool IsPaServerAvailable
        {
            get
            {
                return _isConnected;
            }
        }
    }

    public enum BroadcastStatusEnum
    {
        InProgress = 0,
        Interrupted = 1,
        Completed = 2
    }
}
