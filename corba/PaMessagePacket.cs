using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ste.pa.pamanager
{
    public abstract class PaMessagePacket
    {
        public byte PacketNum { get; }

        protected PaMessagePacket(byte packetNum)
        {
            PacketNum = packetNum;
        }

        public abstract byte[] ToBytes();
    }
}
