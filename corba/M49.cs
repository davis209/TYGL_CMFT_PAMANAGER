using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ste.pa.pamanager
{
    public class M49 : PaMessagePacket
    {
        private const byte PacketId = 49;

        public M49() : base(PacketId)
        {
        }

        public override byte[] ToBytes()
        {
            return new[] { PacketNum };
        }
    }
}
