using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ste.pa.pamanager
{
    public class M41 : PaMessagePacket
    {
        private const byte PacketId = 41;
        private readonly StationVolume[] _stations;

        public M41(IEnumerable<StationVolume> stations)
            : base(PacketId)
        {
            if (stations == null)
                throw new ArgumentNullException(nameof(stations));

            _stations = stations.ToArray();

            if (_stations.Length == 0 || _stations.Length > byte.MaxValue)
                throw new ArgumentException("Station count must be between 1 and 255.");
        }

        public override byte[] ToBytes()
        {
            int stationCount = _stations.Length;

            // 1(PacketNum) + 1(StationNumber) + 4(each station)
            int totalLength = 2 + stationCount * 4;

            byte[] packet = new byte[totalLength];

            packet[0] = PacketNum;
            packet[1] = (byte)stationCount;

            int offset = 2;

            foreach (var station in _stations)
            {
                packet[offset++] = station.StationId;
                packet[offset++] = station.PeakVolume;
                packet[offset++] = station.OffPeakVolume;
                packet[offset++] = station.NightVolume;
            }

            return packet;
        }
    }

    public struct StationVolume
    {
        public byte StationId { get; }
        public byte PeakVolume { get; }
        public byte OffPeakVolume { get; }
        public byte NightVolume { get; }

        public StationVolume(byte stationId, byte peak, byte offPeak, byte night)
        {
            StationId = stationId;
            PeakVolume = peak;
            OffPeakVolume = offPeak;
            NightVolume = night;
        }
    }
}
