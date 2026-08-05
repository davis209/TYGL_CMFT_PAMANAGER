using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ste.pa.pamanager
{
    public class DvaMessageStatusStruct
    {
        public static readonly string COLUMN_PKEY = "PKEY";
        public static readonly string COLUMN_LOCAITON_ID = "LOCATION_ID";
        public static readonly string COLUMN_CONSOLE_ID = "CONSOLE_ID";
        public static readonly string COLUMN_MSG_ID = "MSG_ID";
        public static readonly string COLUMN_MSG_VERSION = "MSG_VERSION";
        public static readonly string COLUMN_RECYCLE_TIME = "RECYCLE_TIME";
        public static readonly string COLUMN_INTERVAL_SEC = "INTERVAL_SEC";
        public static readonly string COLUMN_ZONES = "ZONES";
        public static readonly string COLUMN_STATIONS = "STATIONS";
        public static readonly string COLUMN_SEAT_ID = "SEAT_ID";
        public static readonly string COLUMN_LANGUAGE = "LANGUAGE";
        public static readonly string COLUMN_STATUS = "STATUS";
        public static readonly string COLUMN_SENT_DATETIME = "SENT_TIME";
        public static readonly string COLUMN_LAST_STATUS_DATETIME = "LAST_STATUS_TIME";


        public int PKey { get; set; }
        public int LocationId { get; set; }
        public int ConsoleId { get; set; }
        public int MsgId { get; set; }
        public int RecycleTime { get; set; }
        public int IntervalSec { get; set; }
        public int Zones { get; set; }
        public int Status { get; set; }
        public int SeatId { get; set; }
        public int Language { get; set; }
        public string Stations { get; set; }
        public string MsgVersion { get; set; }
        public DateTime SentTime { get; set; }
        public DateTime LastStatusTime { get; set; }
    }
}
