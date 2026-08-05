using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ste.pa.pamanager
{
    /// <summary>
    /// Stop prerecorded message command
    /// </summary>
    public class M45 : PaMessagePacket
    {
        private const byte PacketId = 45;
        private readonly byte[] _stationIds;

        public byte SeatId { get; }
        public IReadOnlyList<byte> StationIds => _stationIds;

        public M45(byte seatId, IEnumerable<byte> stationIds)
            : base(PacketId)
        {
            if (stationIds == null)
                throw new ArgumentNullException(nameof(stationIds));

            var ids = stationIds.ToArray();

            if (ids.Length == 0 || ids.Length > byte.MaxValue)
                throw new ArgumentException("Station number must be between 1 and 255.", nameof(stationIds));

            SeatId = seatId;
            _stationIds = ids;
        }

        public override byte[] ToBytes()
        {
            int totalLength = 3 + _stationIds.Length; // PacketNum + SeatId + StationNumber + StationIds

            byte[] buffer = new byte[totalLength];

            int offset = 0;

            buffer[offset++] = PacketNum;
            buffer[offset++] = SeatId;
            buffer[offset++] = (byte)_stationIds.Length;

            foreach (var id in _stationIds)
            {
                buffer[offset++] = id;
            }

            return buffer;
        }
    }
}
