using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ste.pa.pamanager
{
    /// <summary>
    /// Set time interval command
    /// </summary>
    public class M43 : PaMessagePacket
    {
        private const byte PacketId = 43;
        private readonly StationSchedule[] _stations;

        public IReadOnlyList<StationSchedule> Stations => _stations;

        public M43(IEnumerable<StationSchedule> stations)
            : base(PacketId)
        {
            if (stations == null)
                throw new ArgumentNullException(nameof(stations));

            _stations = stations.ToArray();

            if (_stations.Length == 0 || _stations.Length > byte.MaxValue)
                throw new ArgumentException("Station count must be 1~255.");
        }

        public override byte[] ToBytes()
        {
            int totalLength = 2; // PacketNum + StationNumber

            foreach (var station in _stations)
            {
                totalLength += station.GetLength();
            }

            byte[] buffer = new byte[totalLength];

            buffer[0] = PacketNum;
            buffer[1] = (byte)_stations.Length;

            int offset = 2;

            foreach (var station in _stations)
            {
                station.WriteTo(buffer, ref offset);
            }

            return buffer;
        }
    }

    public sealed class StationSchedule
    {
        public byte StationId { get; }
        public byte Day { get; }
        public IReadOnlyList<TimeInterval> Intervals => _intervals;

        private readonly TimeInterval[] _intervals;

        public StationSchedule(byte stationId, byte day, IEnumerable<TimeInterval> intervals)
        {
            if (day < 1 || day > 7)
                throw new ArgumentOutOfRangeException(nameof(day));

            if (intervals == null)
                throw new ArgumentNullException(nameof(intervals));

            _intervals = intervals.ToArray();

            if (_intervals.Length == 0 || _intervals.Length > byte.MaxValue)
                throw new ArgumentException("Interval count must be 1~255.");

            StationId = stationId;
            Day = day;
        }

        internal void WriteTo(byte[] buffer, ref int offset)
        {
            buffer[offset++] = StationId;
            buffer[offset++] = Day;
            buffer[offset++] = (byte)_intervals.Length;

            foreach (var interval in _intervals)
            {
                interval.WriteTo(buffer, ref offset);
            }
        }

        internal int GetLength()
        {
            return 3 + _intervals.Length * 9;
        }
    }

    public struct TimeInterval
    {
        public byte Type { get; }           // 1~3
        public string StartTime { get; }       // "1230"
        public string EndTime { get; }

        public TimeInterval(byte type, string startTime, string endTime)
        {
            if (type < 1 || type > 3)
                throw new ArgumentOutOfRangeException(nameof(type));

            if(startTime.Length != 4)
                throw new ArgumentOutOfRangeException(nameof(startTime));

            if (endTime.Length != 4)
                throw new ArgumentOutOfRangeException(nameof(endTime));

            Type = type;
            StartTime = startTime;
            EndTime = endTime;
        }

        internal void WriteTo(byte[] buffer, ref int offset)
        {
            buffer[offset++] = Type;
            byte[] startBytes = Encoding.ASCII.GetBytes(StartTime);
            Buffer.BlockCopy(startBytes, 0, buffer, offset, 4);
            offset += 4;
            byte[] endBytes = Encoding.ASCII.GetBytes(EndTime);
            Buffer.BlockCopy(endBytes, 0, buffer, offset, 4);
            offset += 4;
        }
    }
}
