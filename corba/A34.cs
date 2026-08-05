using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ste.pa.pamanager
{
    public class A34 : PaMessagePacket
    {
        private const byte PacketId = 34;

        public A34() : base(PacketId)
        {
        }

        public override byte[] ToBytes()
        {
            return new[] { PacketNum };
        }
    }
}
