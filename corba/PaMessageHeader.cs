using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ste.pa.pamanager
{
    public class PaMessageHeader
    {
        public static readonly int HEADER_LENGTH = 16;

        public int Server { get; set; }

        public int Console { get; set; }

        public string Ver { get; set; }

        public string LineID { get; set; }

        public string StationID { get; set; }

        public PaMessageHeader(int server, int console, string ver, string lineID, string stationID)
        {
            Server = server;
            Console = console;
            Ver = ver;
            LineID = lineID;
            StationID = stationID;
        }

        public PaMessageHeader()
        {
        }

        public byte[] ToBytes()
        {
            byte[] header = new byte[HEADER_LENGTH];
            header[0] = (byte)Server;
            header[1] = (byte)Console;
            Array.Copy(Encoding.ASCII.GetBytes(Ver), 0, header, 2, Math.Min(5, Ver.Length));
            Array.Copy(Encoding.ASCII.GetBytes(LineID), 0, header, 7, Math.Min(3, LineID.Length));
            Array.Copy(Encoding.ASCII.GetBytes(StationID), 0, header, 10, Math.Min(6, StationID.Length));
            return header;
        }
    }
}
