using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ste.pa.pamanager
{
    public class A31 : PaMessagePacket
    {
        private const byte PacketId = 31;

        public byte StationId { get; }

        public A31(byte stationId) : base(PacketId)
        {
            StationId = stationId;
        }

        public override byte[] ToBytes()
        {
            return new byte[]
            {
            PacketNum,
            StationId
            };
        }
    }
}
