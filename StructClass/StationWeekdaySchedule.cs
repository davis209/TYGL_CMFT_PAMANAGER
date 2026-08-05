using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ste.pa.pamanager
{
    public class StationWeekdaySchedule
    {
        public int StationId { get; set; }

        public int WeekDay { get; set; }

        public string PeakStart { get; set; }

        public string PeakEnd { get; set; }

        public string OffPeakStart { get; set; }

        public string OffPeakEnd { get; set; }

        public string NightStart { get; set; }

        public string NightEnd { get; set; }
    }
}
