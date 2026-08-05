using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Timers;
using System.IO;


namespace ste.pa.pamanager
{
    public class PaTcpClient
    {
        private const int HEADER_LENGTH = 16;
        private const byte DLE = 0xAA;
        private const byte STX = 0xBB;
        private const byte ETX = 0xCC;
        private byte _SEQ = 0x00;

        public static readonly byte PACKET_NUM_A0 = 0x64;
        public static readonly byte PACKET_NUM_A1 = 0x65;
        public static readonly byte PACKET_NUM_A2 = 0x66;
        public static readonly byte PACKET_NUM_A41 = 0x8D;
        public static readonly byte PACKET_NUM_A43 = 0x8F;
        public static readonly byte PACKET_NUM_A44 = 0x90;
        public static readonly byte PACKET_NUM_A45 = 0x91;
        public static readonly byte PACKET_NUM_A47 = 0x93;
        public static readonly byte PACKET_NUM_A48 = 0x94;
        public static readonly byte PACKET_NUM_A49 = 0x95;
        public static readonly byte PACKET_NUM_A51 = 0x97;
        public static readonly byte PACKET_NUM_A61 = 0xA1;
        public static readonly byte PACKET_NUM_A62 = 0xA2;
        public static readonly byte PACKET_NUM_A63 = 0xA3;
        public static readonly byte PACKET_NUM_A64 = 0xA4;
        public static readonly byte PACKET_NUM_A142 = 0x8E;
        public static readonly byte PACKET_NUM_M31 = 0x83;
        public static readonly byte PACKET_NUM_M132 = 0x84;
        public static readonly byte PACKET_NUM_M133 = 0x85;
        public static readonly byte PACKET_NUM_M134 = 0x86;
        public static readonly byte PACKET_NUM_M46 = 0x92;
        public static readonly byte PACKET_NUM_M152 = 0x98;

        private TcpClient _tcpClient;
        private NetworkStream _stream;
        private Thread _receiveThread;
        private System.Timers.Timer _heartbeatTimer;
        private bool _connected = false;
        private readonly object _lock = new object();
        private string _fileName;

        // TaskCompletionSource for waiting play request response
        private TaskCompletionSource<bool> _getStationStatusResponseTcs;
        private TaskCompletionSource<bool> _setVolumeResponseTcs;
        private TaskCompletionSource<bool> _setScheduleResponseTcs;
        private TaskCompletionSource<bool> _playResponseTcs;
        private TaskCompletionSource<bool> _stopPlayResponseTcs;
        private TaskCompletionSource<bool> _requestRecordResponseTcs;
        private TaskCompletionSource<bool> _startRecordResponseTcs;
        private TaskCompletionSource<bool> _endRecordResponseTcs;
        private TaskCompletionSource<bool> _queryDeviceStatusResponseTcs;
        private TaskCompletionSource<bool> _queryMsgVersionResponseTcs;
        private TaskCompletionSource<bool> _startLiveBroadcastResponseTcs;
        private TaskCompletionSource<bool> _endLiveBroadcastResponseTcs;
        private TaskCompletionSource<bool> _startAirRaidAlarmResponseTcs;
        private TaskCompletionSource<bool> _endAirRaidAlarmResponseTcs;
        private readonly int RESPONSE_TIMEOUT_MS = 4000;

        //heartbeat message
        private byte[] _heartbeatPacket;

        private PaMessageHeader _defaultHeader = new PaMessageHeader()
        {
            Server = 1,
            Console = 54,
            Ver = "00001",
            LineID = "000",
            StationID = "      "
        };

        // ip & port
        private Queue<string> _ipQueue = new Queue<string>();
        private int _port;

        // Events for external handling
        public event EventHandler<ReceivedPacket> OnMessageReceived;
        public event EventHandler OnConnected;
        public event EventHandler OnDisconnected;
        public event EventHandler<Exception> OnError;
        public event EventHandler OnHeartbeatAckReceived;

        public PaTcpClient(string priIP, string secIP, int port, int consoleId)
        {
            _heartbeatPacket = new byte[] { 0x00 };

            if (!string.IsNullOrEmpty(priIP))
            {
                _ipQueue.Enqueue(priIP);
            }
            if (!string.IsNullOrEmpty(secIP))
            {
                _ipQueue.Enqueue(secIP);
            }
            _port = port;
            _fileName = typeof(PaTcpClient).Name;

            _defaultHeader.Console = consoleId;
        }

        public void Start()
        {
            Connect();

            _heartbeatTimer = new System.Timers.Timer(60000);
            _heartbeatTimer.Elapsed += (s, e) => SendHeartbeat();
            _heartbeatTimer.Start();
        }

        public void Stop()
        {
            _heartbeatTimer?.Stop();
            _tcpClient?.Close();
            _connected = false;
            Log("Client stopped");
        }

        public async Task<bool> SetVolumeAndWaitResponse(M41 cmd)
        {
            byte[] packet = cmd.ToBytes();

            _setVolumeResponseTcs = new TaskCompletionSource<bool>();

            Log($"Sending set volume request");
            SendMessage(_SEQ++, packet);

            var timeoutTask = Task.Delay(RESPONSE_TIMEOUT_MS);
            var completed = await Task.WhenAny(_setVolumeResponseTcs.Task, timeoutTask);

            if (completed == timeoutTask)
            {
                Log("Set volume response timed out");
                _setVolumeResponseTcs.TrySetResult(false);
                return false;
            }

            return await _setVolumeResponseTcs.Task;
        }

        public async Task<bool> SetScheduleAndWaitResponse(M43 cmd)
        {
            byte[] packet = cmd.ToBytes();

            _setScheduleResponseTcs = new TaskCompletionSource<bool>();

            Log($"Sending set Schedule request");
            SendMessage(_SEQ++, packet);

            var timeoutTask = Task.Delay(RESPONSE_TIMEOUT_MS);
            var completed = await Task.WhenAny(_setScheduleResponseTcs.Task, timeoutTask);

            if (completed == timeoutTask)
            {
                Log("Set Schedule response timed out");
                _setScheduleResponseTcs.TrySetResult(false);
                return false;
            }

            return await _setScheduleResponseTcs.Task;
        }

        /// <summary>
        /// Get station status
        /// </summary>
        /// <param name="cmd"></param>
        /// <returns></returns>
        public async Task<bool> GetStationStatusAndWaitResponse(M1 cmd)
        {
            byte[] packet = cmd.ToBytes();

            _getStationStatusResponseTcs = new TaskCompletionSource<bool>();

            Log($"Sending get station status request");
            SendMessage(_SEQ++, packet);

            var timeoutTask = Task.Delay(RESPONSE_TIMEOUT_MS);
            var completed = await Task.WhenAny(_getStationStatusResponseTcs.Task, timeoutTask);

            if (completed == timeoutTask)
            {
                Log("Play request response timed out");
                _getStationStatusResponseTcs.TrySetResult(false);
                return false;
            }

            return await _getStationStatusResponseTcs.Task;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="cmd"></param>
        /// <returns></returns>
        public async Task<bool> SendPlayRequestAndWaitResponse(M44 cmd)
        {
            byte[] packet = cmd.ToBytes();

            _playResponseTcs = new TaskCompletionSource<bool>();

            Log($"Sending play request, Seat={cmd.SeatId}");
            SendMessage(_SEQ++, packet);

            var timeoutTask = Task.Delay(RESPONSE_TIMEOUT_MS);
            var completed = await Task.WhenAny(_playResponseTcs.Task, timeoutTask);

            if (completed == timeoutTask)
            {
                Log("Play request response timed out");
                _playResponseTcs.TrySetResult(false);
                return false;
            }

            return await _playResponseTcs.Task;
        }

        public async Task<bool> RequestRecordAndWaitResponse(M47 cmd)
        {
            byte[] packet = cmd.ToBytes();

            _requestRecordResponseTcs = new TaskCompletionSource<bool>();

            Log($"Sending request record, Seat={cmd.SeatId}");
            SendMessage(_SEQ++, packet);

            var timeoutTask = Task.Delay(RESPONSE_TIMEOUT_MS);
            var completed = await Task.WhenAny(_requestRecordResponseTcs.Task, timeoutTask);

            if (completed == timeoutTask)
            {
                Log("Request record response timed out");
                _requestRecordResponseTcs.TrySetResult(false);
                return false;
            }

            return await _requestRecordResponseTcs.Task;
        }

        public async Task<bool> StartRecordAndWaitResponse(M48 cmd)
        {
            byte[] packet = cmd.ToBytes();

            _startRecordResponseTcs = new TaskCompletionSource<bool>();

            Log($"Sending start record");
            SendMessage(_SEQ++, packet);

            var timeoutTask = Task.Delay(RESPONSE_TIMEOUT_MS);
            var completed = await Task.WhenAny(_startRecordResponseTcs.Task, timeoutTask);

            if (completed == timeoutTask)
            {
                Log("Start record response timed out");
                _startRecordResponseTcs.TrySetResult(false);
                return false;
            }

            return await _startRecordResponseTcs.Task;
        }

        public async Task<bool> EndRecordAndWaitResponse(M49 cmd)
        {
            byte[] packet = cmd.ToBytes();

            _endRecordResponseTcs = new TaskCompletionSource<bool>();

            Log($"Sending end record");
            SendMessage(_SEQ++, packet);

            var timeoutTask = Task.Delay(RESPONSE_TIMEOUT_MS);
            var completed = await Task.WhenAny(_endRecordResponseTcs.Task, timeoutTask);

            if (completed == timeoutTask)
            {
                Log("End record response timed out");
                _endRecordResponseTcs.TrySetResult(false);
                return false;
            }

            return await _endRecordResponseTcs.Task;
        }

        public async Task<bool> QueryDeviceStatusAndWaitResponse(M51 cmd)
        {
            byte[] packet = cmd.ToBytes();

            _queryDeviceStatusResponseTcs = new TaskCompletionSource<bool>();

            Log($"Sending query device status command");
            SendMessage(_SEQ++, packet);

            var timeoutTask = Task.Delay(RESPONSE_TIMEOUT_MS);
            var completed = await Task.WhenAny(_queryDeviceStatusResponseTcs.Task, timeoutTask);

            if (completed == timeoutTask)
            {
                Log("query device status timed out");
                _queryDeviceStatusResponseTcs.TrySetResult(false);
                return false;
            }

            return await _queryDeviceStatusResponseTcs.Task;
        }

        public async Task<bool> StartLiveBroadcastAndWaitResponse(M61 cmd)
        {
            byte[] packet = cmd.ToBytes();

            _startLiveBroadcastResponseTcs = new TaskCompletionSource<bool>();

            Log($"Sending start live broadcast command");
            SendMessage(_SEQ++, packet);

            var timeoutTask = Task.Delay(RESPONSE_TIMEOUT_MS);
            var completed = await Task.WhenAny(_startLiveBroadcastResponseTcs.Task, timeoutTask);

            if (completed == timeoutTask)
            {
                Log("start live broadcast timed out");
                _startLiveBroadcastResponseTcs.TrySetResult(false);
                return false;
            }

            return await _startLiveBroadcastResponseTcs.Task;
        }

        public async Task<bool> EndLiveBroadcastAndWaitResponse(M62 cmd)
        {
            byte[] packet = cmd.ToBytes();

            _endLiveBroadcastResponseTcs = new TaskCompletionSource<bool>();

            Log($"Sending end live broadcast command");
            SendMessage(_SEQ++, packet);

            var timeoutTask = Task.Delay(RESPONSE_TIMEOUT_MS);
            var completed = await Task.WhenAny(_endLiveBroadcastResponseTcs.Task, timeoutTask);

            if (completed == timeoutTask)
            {
                Log("end live broadcast timed out");
                _endLiveBroadcastResponseTcs.TrySetResult(false);
                return false;
            }

            return await _endLiveBroadcastResponseTcs.Task;
        }

        public async Task<bool> StartAirRaidAlarmAndWaitResponse(M63 cmd)
        {
            byte[] packet = cmd.ToBytes();

            _startAirRaidAlarmResponseTcs = new TaskCompletionSource<bool>();

            Log($"Sending start air raid alarm command");
            SendMessage(_SEQ++, packet);

            var timeoutTask = Task.Delay(RESPONSE_TIMEOUT_MS);
            var completed = await Task.WhenAny(_startAirRaidAlarmResponseTcs.Task, timeoutTask);

            if (completed == timeoutTask)
            {
                Log("start air raid alarm timed out");
                _startAirRaidAlarmResponseTcs.TrySetResult(false);
                return false;
            }

            return await _startAirRaidAlarmResponseTcs.Task;
        }


        public async Task<bool> EndAirRaidAlarmAndWaitResponse(M64 cmd)
        {
            byte[] packet = cmd.ToBytes();

            _endAirRaidAlarmResponseTcs = new TaskCompletionSource<bool>();

            Log($"Sending end air raid alarm command");
            SendMessage(_SEQ++, packet);

            var timeoutTask = Task.Delay(RESPONSE_TIMEOUT_MS);
            var completed = await Task.WhenAny(_endAirRaidAlarmResponseTcs.Task, timeoutTask);

            if (completed == timeoutTask)
            {
                Log("end air raid alarm timed out");
                _endAirRaidAlarmResponseTcs.TrySetResult(false);
                return false;
            }

            return await _endAirRaidAlarmResponseTcs.Task;
        }

        public async Task<bool> SendStopPlayRequestAndWaitResponse(M45 cmd)
        {
            byte[] packet = cmd.ToBytes();

            _stopPlayResponseTcs = new TaskCompletionSource<bool>();

            Log("Sending stop play request");
            SendMessage(_SEQ++, packet);

            var timeoutTask = Task.Delay(RESPONSE_TIMEOUT_MS);
            var completed = await Task.WhenAny(_stopPlayResponseTcs.Task, timeoutTask);

            if (completed == timeoutTask)
            {
                Log("Stop play request response timed out");
                _stopPlayResponseTcs.TrySetResult(false);
                return false;
            }

            return await _stopPlayResponseTcs.Task;
        }

        public async Task<bool> QueryMsgVersionAndWaitResponse()
        {
            byte[] packet = (new M42()).ToBytes();

            _queryMsgVersionResponseTcs = new TaskCompletionSource<bool>();

            Log("Sending release control request");
            SendMessage(_SEQ++, packet);

            var timeoutTask = Task.Delay(RESPONSE_TIMEOUT_MS);
            var completed = await Task.WhenAny(_queryMsgVersionResponseTcs.Task, timeoutTask);

            if (completed == timeoutTask)
            {
                Log("Release control request response timed out");
                _queryMsgVersionResponseTcs.TrySetResult(false);
                return false;
            }

            return await _queryMsgVersionResponseTcs.Task;
        }

        public void SendMessagePacket(byte sequence, PaMessagePacket packet)
        {
            Log("Sending message packet");
            SendMessage(sequence, packet.ToBytes());
        }

        private void Connect()
        {
            ThreadPool.QueueUserWorkItem(_ =>
            {
                while (!_connected)
                {
                    try
                    {
                        string ip = _ipQueue.Dequeue();
                        _ipQueue.Enqueue(ip);
                        Log($"Attempting to connect to {ip}:{_port}...");
                        _tcpClient = new TcpClient();
                        _tcpClient.Connect(IPAddress.Parse(ip), _port);
                        _stream = _tcpClient.GetStream();
                        _connected = true;

                        SendHeartbeat();

                        Log("Connected successfully");
                        OnConnected?.Invoke(this, EventArgs.Empty);

                        _receiveThread = new Thread(ReceiveLoop);
                        _receiveThread.IsBackground = true;
                        _receiveThread.Start();
                    }
                    catch (Exception ex)
                    {
                        Log($"Connection failed: {ex.Message}");
                        OnError?.Invoke(this, ex);
                        Thread.Sleep(5000);
                    }
                }
            });
        }

        private void Reconnect()
        {
            _connected = false;
            Log("Connection lost, trying to reconnect...");
            OnDisconnected?.Invoke(this, EventArgs.Empty);
            Connect();
        }

        private void ReceiveLoop()
        {
            try
            {
                var buffer = new byte[1024];
                var ms = new MemoryStream();

                while (_connected && _stream != null)
                {
                    int bytesRead = _stream.Read(buffer, 0, buffer.Length);
                    if (bytesRead == 0) throw new IOException("Connection closed");

                    ms.Write(buffer, 0, bytesRead);

                    byte[] data = ms.ToArray();
                    Log("Received message:" + BitConverter.ToString(data));
                    int index = 0;
                    while (TryParseMessage(data, ref index, out byte sequence, out byte[] payload))
                    {
                        // Heartbeat Ack packet PacketNum=100(0x64)
                        if (payload.Length > PaMessageHeader.HEADER_LENGTH && payload[PaMessageHeader.HEADER_LENGTH] == PACKET_NUM_A0)
                        {
                            Log("Received heartbeat acknowledgment packet");
                            OnHeartbeatAckReceived?.Invoke(this, EventArgs.Empty);
                            continue;
                        }

                        if (payload.Length > PaMessageHeader.HEADER_LENGTH && payload[PaMessageHeader.HEADER_LENGTH] == PACKET_NUM_A1)
                        {
                            bool success = true;
                            Log($"Received play response packet, result: {(success ? "Success" : "Failure")}");
                            _getStationStatusResponseTcs?.TrySetResult(success);
                            //continue;
                        }

                        if (payload.Length > PaMessageHeader.HEADER_LENGTH && payload[PaMessageHeader.HEADER_LENGTH] == PACKET_NUM_A41)
                        {
                            bool success = true;
                            Log($"Received play response packet, result: {(success ? "Success" : "Failure")}");
                            _setVolumeResponseTcs?.TrySetResult(success);
                            continue;
                        }

                        if (payload.Length > PaMessageHeader.HEADER_LENGTH && payload[PaMessageHeader.HEADER_LENGTH] == PACKET_NUM_A43)
                        {
                            bool success = true;
                            Log($"Received set-schedule response packet, result: {(success ? "Success" : "Failure")}");
                            _setScheduleResponseTcs?.TrySetResult(success);
                            continue;
                        }

                        // Play response packet PacketNum=144(0x90)
                        if (payload.Length > PaMessageHeader.HEADER_LENGTH && payload[PaMessageHeader.HEADER_LENGTH] == PACKET_NUM_A44)
                        {
                            bool success = true;
                            Log($"Received play response packet, result: {(success ? "Success" : "Failure")}");
                            _playResponseTcs?.TrySetResult(success);
                            continue;
                        }

                        // Stop play response packet PacketNum=145(0x91)
                        if (payload.Length > PaMessageHeader.HEADER_LENGTH && payload[PaMessageHeader.HEADER_LENGTH] == PACKET_NUM_A45)
                        {
                            bool success = true;
                            Log($"Received stop play response packet, result: {(success ? "Success" : "Failure")}");
                            _stopPlayResponseTcs?.TrySetResult(success);
                            continue;
                        }

                        if (payload.Length > PaMessageHeader.HEADER_LENGTH && payload[PaMessageHeader.HEADER_LENGTH] == PACKET_NUM_A47)
                        {
                            bool success = true;
                            Log($"Received request record response packet, result: {(success ? "Success" : "Failure")}");
                            _requestRecordResponseTcs?.TrySetResult(success);
                            continue;
                        }

                        if (payload.Length > PaMessageHeader.HEADER_LENGTH && payload[PaMessageHeader.HEADER_LENGTH] == PACKET_NUM_A48)
                        {
                            bool success = true;
                            Log($"Received start record response packet, result: {(success ? "Success" : "Failure")}");
                            _startRecordResponseTcs?.TrySetResult(success);
                            continue;
                        }

                        if (payload.Length > PaMessageHeader.HEADER_LENGTH && payload[PaMessageHeader.HEADER_LENGTH] == PACKET_NUM_A49)
                        {
                            bool success = true;
                            Log($"Received end record response packet, result: {(success ? "Success" : "Failure")}");
                            _endRecordResponseTcs?.TrySetResult(success);
                            continue;
                        }

                        if (payload.Length > PaMessageHeader.HEADER_LENGTH && payload[PaMessageHeader.HEADER_LENGTH] == PACKET_NUM_A51)
                        {
                            bool success = true;
                            Log($"Received query device status response packet, result: {(success ? "Success" : "Failure")}");
                            _queryDeviceStatusResponseTcs?.TrySetResult(success);
                            //continue;
                        }

                        if (payload.Length > PaMessageHeader.HEADER_LENGTH && payload[PaMessageHeader.HEADER_LENGTH] == PACKET_NUM_A61)
                        {
                            bool success = true;
                            Log($"Received start live broadcast response packet, result: {(success ? "Success" : "Failure")}");
                            _startLiveBroadcastResponseTcs?.TrySetResult(success);
                        }

                        if (payload.Length > PaMessageHeader.HEADER_LENGTH && payload[PaMessageHeader.HEADER_LENGTH] == PACKET_NUM_A62)
                        {
                            bool success = true;
                            Log($"Received end live broadcast response packet, result: {(success ? "Success" : "Failure")}");
                            _endLiveBroadcastResponseTcs?.TrySetResult(success);
                        }

                        if (payload.Length > PaMessageHeader.HEADER_LENGTH && payload[PaMessageHeader.HEADER_LENGTH] == PACKET_NUM_A63)
                        {
                            bool success = true;
                            Log($"Received start air raid alarm response packet, result: {(success ? "Success" : "Failure")}");
                            _startAirRaidAlarmResponseTcs?.TrySetResult(success);
                        }

                        if (payload.Length > PaMessageHeader.HEADER_LENGTH && payload[PaMessageHeader.HEADER_LENGTH] == PACKET_NUM_A64)
                        {
                            bool success = true;
                            Log($"Received end air raid alarm response packet, result: {(success ? "Success" : "Failure")}");
                            _endAirRaidAlarmResponseTcs?.TrySetResult(success);
                        }

                        // Release control response packet PacketNum=101(0x66)
                        if (payload.Length > PaMessageHeader.HEADER_LENGTH && payload[PaMessageHeader.HEADER_LENGTH] == PACKET_NUM_A2)
                        {
                            bool success = payload[PaMessageHeader.HEADER_LENGTH + 1] == 0;
                            Log($"Received release control response packet, result: {(success ? "Success" : "Failure")}");
                            //_releaseCtrlResponseTcs?.TrySetResult(success);
                            continue;
                        }

                        // Other packets
                        OnMessageReceived?.Invoke(this, new ReceivedPacket(sequence, payload));
                    }

                    byte[] remaining = new byte[data.Length - index];
                    Array.Copy(data, index, remaining, 0, remaining.Length);
                    ms.SetLength(0);
                    ms.Write(remaining, 0, remaining.Length);
                }
            }
            catch (Exception ex)
            {
                Log($"Receive exception: {ex.Message}");
                OnError?.Invoke(this, ex);
                Reconnect();
            }
        }

        private bool TryParseMessage(byte[] data, ref int index, out byte sequence, out byte[] payload)
        {
            sequence = 0;
            payload = null;
            if (data.Length - index < 8) return false;
            if (data[index] != DLE || data[index + 1] != STX) return false;

            int len = (data[index + 2] << 8) | data[index + 3];
            int totalLen = 2 + 2 + len + 1 + 2;

            if (data.Length - index < totalLen) return false;

            byte[] message = new byte[len];
            Array.Copy(data, index + 4, message, 0, len);

            byte lrc = 0;
            foreach (var b in message) lrc ^= b;
            if (lrc != data[index + 4 + len])
            {
                Log("LRC check failed, discarding packet");
                //index += totalLen;
                //return false;
            }

            if (data[index + totalLen - 2] != DLE || data[index + totalLen - 1] != ETX)
            {
                Log("Packet tail check failed, reconnect");
                throw new SystemException("Packet tail check failed.");
                //index += totalLen;
                //return false;
            }

            payload = message;
            index += totalLen;
            return true;
        }

        private void SendMessage(byte sequence, byte[] packet)
        {
            if (!_connected || _stream == null)
            {
                Log("Send failed: not connected");
                return;
            }

            lock (_lock)
            {
                byte[] message = BuildMessage(sequence, packet);
                _stream.Write(message, 0, message.Length);
                Log("Sent message:" + BitConverter.ToString(message));
            }
        }

        private void SendHeartbeat()
        {
            Log("Sending heartbeat packet");
            SendMessage(_SEQ++, _heartbeatPacket);
        }

        private byte[] BuildMessage(byte sequence, byte[] packet)
        {
            byte[] header = _defaultHeader.ToBytes();
            byte[] data = new byte[header.Length + packet.Length];
            Buffer.BlockCopy(header, 0, data, 0, header.Length);
            Buffer.BlockCopy(packet, 0, data, header.Length, packet.Length);

            byte lrc = 0;
            foreach (byte b in data) lrc ^= b;

            int len = data.Length;
            byte[] message = new byte[2 + 2 + len + 1 + 2];
            int i = 0;
            message[i++] = DLE;
            message[i++] = STX;
            message[i++] = (byte)((len >> 8) & 0xFF);
            message[i++] = (byte)(len & 0xFF);
            Buffer.BlockCopy(data, 0, message, i, len);
            i += len;
            message[i++] = lrc;
            message[i++] = DLE;
            message[i++] = ETX;

            return message;
        }

        private void Log(string message)
        {
            Utility.log(message, _fileName);
        }
    }

    public class ReceivedPacket
    {
        public byte Sequence { get; }

        public byte[] Packet { get; }

        public ReceivedPacket(byte sequence, byte[] packet)
        {
            Sequence = sequence;
            Packet = packet ?? throw new ArgumentNullException(nameof(packet));
        }
    }
}
