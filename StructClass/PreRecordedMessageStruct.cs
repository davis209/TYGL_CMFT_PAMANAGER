using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ste.pa.pamanager
{
    public class PreRecordedMessageStruct
    {
        public int MsgId { get; set; }

        public string MsgLabel { get; set; }

        public string MsgContent { get; set; }

        public override string ToString()
        {
            return MsgId.ToString() + "  " + MsgLabel;
        }
    }
}
