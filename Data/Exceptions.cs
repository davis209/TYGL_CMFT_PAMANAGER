using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ste.pa.cos;

namespace ste.pa.pamanager
{
    public class CorbaServerNotAvailableException : SystemException
    {
        public CorbaServerNotAvailableException(string msg)
            : base(msg)
        {
        }
    }

    public class CorbaBroadcastException : SystemException
    {
        CosErrorCodeEnum errorCode_;
        public CosErrorCodeEnum CosErrorCode { get { return errorCode_; } }

        String errorString_;
        public String ErrorString { get { return errorString_; } }

        public CorbaBroadcastException(CosErrorCodeEnum ec, string msg)
            : base(msg)
        {
            errorCode_ = ec;
            errorString_ = ec.ToString();
        }
    }

}
