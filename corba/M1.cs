using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ste.pa.pamanager
{
    /// <summary>
    /// Query station status command
    /// </summary>
    public class M1 : PaMessagePacket
    {
        private const byte PacketId = 1;
        private readonly byte[] _stationArray;

        public M1(byte[] stationArray)
            : base(PacketId)
        {
            if (stationArray == null)
                throw new ArgumentNullException(nameof(stationArray));

            if (stationArray.Length > byte.MaxValue)
                throw new ArgumentException("Station array length cannot exceed 255.");

            _stationArray = (byte[])stationArray.Clone();
        }

        public override byte[] ToBytes()
        {
            var packet = new byte[2 + _stationArray.Length];

            packet[0] = PacketNum;
            packet[1] = (byte)_stationArray.Length;

            Buffer.BlockCopy(_stationArray, 0, packet, 2, _stationArray.Length);

            return packet;
        }
    }
}
