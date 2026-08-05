using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ste.pa.pamanager
{
    public class M51 : PaMessagePacket
    {
        private const byte PacketId = 51;

        private readonly byte[] _stationIds;

        public IReadOnlyList<byte> StationIds => _stationIds;

        public M51(IEnumerable<byte> stationIds)
            : base(PacketId)
        {
            if (stationIds == null)
                throw new ArgumentNullException(nameof(stationIds));

            var arr = stationIds.ToArray();

            if (arr.Length == 0 || arr.Length > byte.MaxValue)
                throw new ArgumentException("Station number must be between 1 and 255.", nameof(stationIds));

            _stationIds = arr;
        }

        public override byte[] ToBytes()
        {
            int totalLength = 2 + _stationIds.Length; // PacketNum + StationNumber + StationIds
            byte[] buffer = new byte[totalLength];

            int offset = 0;
            buffer[offset++] = PacketNum;
            buffer[offset++] = (byte)_stationIds.Length;

            foreach (var id in _stationIds)
            {
                buffer[offset++] = id;
            }

            return buffer;
        }
    }
}
