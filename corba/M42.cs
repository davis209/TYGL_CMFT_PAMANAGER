using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ste.pa.pamanager
{
    /// <summary>
    /// Query prerecorded message version command
    /// </summary>
    public class M42 : PaMessagePacket
    {
        private const byte PacketId = 42;

        public M42() : base(PacketId)
        {
        }

        public override byte[] ToBytes()
        {
            return new[] { PacketNum };
        }
    }
}
