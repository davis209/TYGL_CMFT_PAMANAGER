using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ste.pa.pamanager
{
    /// <summary>
    /// Start prerecorded message command
    /// </summary>
    public struct PlayItem
    {
        public ushort PlayId { get; }
        public byte Language { get; }  // bit0=国语, bit1=台语, bit2=客语, bit3=英语

        public PlayItem(ushort playId, byte language)
        {
            if (language == 0 || language > 0x0F)
                throw new ArgumentOutOfRangeException(nameof(language), "Language bitmask must be between 1 and 0x0F.");

            PlayId = playId;
            Language = language;
        }

        internal void WriteTo(byte[] buffer, ref int offset)
        {
            // PlayId 2 bytes big endian
            buffer[offset++] = (byte)(PlayId >> 8);
            buffer[offset++] = (byte)(PlayId & 0xFF);

            buffer[offset++] = Language;
        }
    }

    public sealed class StationPlay
    {
        public byte StationId { get; }
        public byte PlayArea { get; }  // 每个位代表区域, 0xFF表示全区
        public IReadOnlyList<PlayItem> PlayItems => _playItems;
        public byte PlayCount { get; }
        public byte PlayInterval { get; }

        private readonly PlayItem[] _playItems;

        public StationPlay(byte stationId, byte playArea, IEnumerable<PlayItem> playItems, byte playCount, byte playInterval)
        {
            if (playItems == null)
                throw new ArgumentNullException(nameof(playItems));

            var items = playItems.ToArray();
            if (items.Length == 0 || items.Length > byte.MaxValue)
                throw new ArgumentException("Play ID count must be between 1 and 255.", nameof(playItems));

            if (playCount == 0)
                throw new ArgumentOutOfRangeException(nameof(playCount), "Play count must be at least 1.");

            StationId = stationId;
            PlayArea = playArea;
            _playItems = items;
            PlayCount = playCount;
            PlayInterval = playInterval;
        }

        internal void WriteTo(byte[] buffer, ref int offset)
        {
            buffer[offset++] = StationId;
            buffer[offset++] = PlayArea;
            buffer[offset++] = (byte)_playItems.Length;

            foreach (var item in _playItems)
            {
                item.WriteTo(buffer, ref offset);
            }

            buffer[offset++] = PlayCount;
            buffer[offset++] = PlayInterval;
        }

        internal int GetLength()
        {
            // 1 + 1 + 1 + (3 * playItems.Count) + 1 + 1
            return 5 + 3 * _playItems.Length;
        }
    }

    public class M44 : PaMessagePacket
    {
        private const byte PacketId = 44;

        public byte SeatId { get; }
        public IReadOnlyList<StationPlay> Stations => _stations;

        private readonly StationPlay[] _stations;

        public M44(byte seatId, IEnumerable<StationPlay> stations)
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
            // 1 PacketNum + 1 SeatId + 1 StationNumber + sum(station length)
            int totalLength = 3;
            foreach (var station in _stations)
            {
                totalLength += station.GetLength();
            }

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
