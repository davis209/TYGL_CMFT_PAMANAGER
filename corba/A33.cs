using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ste.pa.pamanager
{
    public class A33 : PaMessagePacket
    {
        private const byte PacketId = 33;

        public A33() : base(PacketId)
        {
        }

        public override byte[] ToBytes()
        {
            return new[] { PacketNum };
        }
    }
}
