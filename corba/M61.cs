using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ste.pa.pamanager
{
    public sealed class StationBroadcast
    {
        public byte StationId { get; }
        public byte PlayArea { get; }

        public StationBroadcast(byte stationId, byte playArea)
        {
            StationId = stationId;
            PlayArea = playArea;
        }

        internal void WriteTo(byte[] buffer, ref int offset)
        {
            buffer[offset++] = StationId;
            buffer[offset++] = PlayArea;
        }

        internal int GetLength() => 2;
    }

    public class M61 : PaMessagePacket
    {
        private const byte PacketId = 61;

        public byte SeatId { get; }
        public IReadOnlyList<StationBroadcast> Stations => _stations;

        private readonly StationBroadcast[] _stations;

        public M61(byte seatId, IEnumerable<StationBroadcast> stations)
            : base(PacketId)
        {
            if (stations == null)
                throw new ArgumentNullException(nameof(stations));

            var arr = stations.ToArray();

            if (arr.Length == 0 || arr.Length > byte.MaxValue)
                throw new ArgumentException("Station number must be between 1 and 255.", nameof(stations));

            SeatId = seatId;
            _stations = arr;
        }

        public override byte[] ToBytes()
        {
            int totalLength = 1 + 1 + 1 + _stations.Length * 2; // PacketNum + SeatId + StationNumber + 2 bytes per station
            byte[] buffer = new byte[totalLength];

            int offset = 0;
            buffer[offset++] = PacketNum;
            buffer[offset++] = SeatId;
            buffer[offset++] = (byte)_stations.Length;

            foreach (var station in _stations)
            {
                station.WriteTo(buffer, ref offset);
            }

            return buffer;
        }
    }
}
