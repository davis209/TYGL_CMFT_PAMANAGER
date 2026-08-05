using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ste.pa.pamanager
{
    public class A52 : PaMessagePacket
    {
        private const byte PacketId = 52;

        public A52() : base(PacketId)
        {
        }

        public override byte[] ToBytes()
        {
            return new[] { PacketNum };
        }
    }
}
