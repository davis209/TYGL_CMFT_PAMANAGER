using System;

namespace ste.pa.pamanager
{
    public class M48 : PaMessagePacket
    {
        private const byte PacketId = 48;
        public M48()
            : base(PacketId)
        {
        }

        public override byte[] ToBytes()
        {
            return new byte[] { PacketNum };
        }
    }
}
