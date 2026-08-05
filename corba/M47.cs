using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ste.pa.pamanager
{
    public class M47 : PaMessagePacket
    {
        private const byte PacketId = 47;

        public byte SeatId { get; }
        public ushort RecordingId { get; }

        public M47(byte seatId, ushort recordId)
            : base(PacketId)
        {
            if (recordId < 60001 || recordId > 60006)
                throw new ArgumentOutOfRangeException(nameof(recordId), "RecordingId must be between 60001 and 60006.");

            SeatId = seatId;
            RecordingId = recordId;
        }

        public override byte[] ToBytes()
        {
            var buffer = new byte[4]; // 1 + 1 + 2

            int offset = 0;
            buffer[offset++] = PacketNum;
            buffer[offset++] = SeatId;
            buffer[offset++] = (byte)(RecordingId >> 8);
            buffer[offset++] = (byte)(RecordingId & 0xFF);

            return buffer;
        }
    }
}
