using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ste.pa.cos;


namespace ste.pa.pamanager
{
    class PaAgentCorbaService : CorbaServer<ICosPAAgent>
    {
        #region Singleton required implementations
        private static volatile PaAgentCorbaService instance_ = null;
        private static object syncRoot = new Object();
        public static PaAgentCorbaService Instance
        {
            get
            {
                if (instance_ == null)
                {
                    lock (syncRoot)
                    {
                        if (instance_ == null)
                            instance_ = new PaAgentCorbaService();
                    }
                }

                return instance_;
            }
        }

        public static void destroyInstance()
        {
            lock (syncRoot)
            {
                if (instance_ != null)
                {
                    instance_.Cleanup();
                    instance_.ShutdownWaitForStop(5000);
                    instance_ = null;
                }
            }
        }
        #endregion

        private PaAgentCorbaService()
            : base(Utility.getPaAgentCorbaAddress(), "PaAgentCorbaService")
        {
            fileName_ = typeof(PaAgentCorbaService).Name;
        }

        protected override bool Preprocess()
        {
            return base.Preprocess();
        }

        protected override void Cleanup()
        {
            base.Cleanup();
        }

        protected override void Process()
        {
            if (IsRunning == false)
                return;
        }

        protected override int createSubscriptionGroup(string name, int expiryTime)
        {
            //no this method in PaAgent
            return -1;
        }

        public long TakeControl(cosMessageHeader header, int playArea)
        {
            long result = 0;
            try
            {
                Execute(() =>
                {
                    Utility.log("cosTakeControl triggering ", fileName_);
                    result = ((ICosPAAgent)server_).cosTakeControl(header, playArea);
                    Utility.log("cosTakeControl completed, result=" + result.ToString(), fileName_);
                });
            }
            catch (Exception ex)
            {
                Utility.logError("cosTakeControl unexpected exception caught.", fileName_, ex);
            }
            return result;
        }

        public bool ReleaseControl(cosMessageHeader header)
        {
            bool result = false;
            try
            {
                Execute(() =>
                {
                    Utility.log("cosReleaseControl triggering ", fileName_);
                    result = ((ICosPAAgent)server_).cosReleaseControl(header);
                    Utility.log("cosReleaseControl completed, result=" + result.ToString(), fileName_);
                });
            }
            catch (Exception ex)
            {
                Utility.logError("cosReleaseControl unexpected exception caught.", fileName_, ex);
            }
            return result;
        }

        public bool StartBroadcast(cosMessageHeader header, int playID, int playCount, int playInterval)
        {
            bool result = false;
            try
            {
                Execute(() =>
                {
                    Utility.log("cosStartBroadcast triggering ", fileName_);
                    result = ((ICosPAAgent)server_).cosStartBroadcast(header, playID, playCount, playInterval);
                    Utility.log("cosStartBroadcast completed, result=" + result.ToString(), fileName_);
                });
            }
            catch (Exception ex)
            {
                Utility.logError("cosStartBroadcast unexpected exception caught.", fileName_, ex);
            }
            return result;
        }

        public bool StopBroadcast(cosMessageHeader header)
        {
            bool result = false;
            try
            {
                Execute(() =>
                {
                    Utility.log("cosStopBroadcast triggering ", fileName_);
                    result = ((ICosPAAgent)server_).cosStopBroadcast(header);
                    Utility.log("cosStopBroadcast completed, result=" + result.ToString(), fileName_);
                });
            }
            catch (Exception ex)
            {
                Utility.logError("cosStopBroadcast unexpected exception caught.", fileName_, ex);
            }
            return result;
        }

        public cosBroadcastStatusEnum BroadcastStatus(cosMessageHeader header)
        {
            cosBroadcastStatusEnum result = cosBroadcastStatusEnum.InProgress;
            try
            {
                Execute(() =>
                {
                    Utility.log("cosBroadcastStatus triggering ", fileName_);
                    result = ((ICosPAAgent)server_).cosBroadcastStatus(header);
                    Utility.log("cosBroadcastStatus completed, result=" + result.ToString(), fileName_);
                });
            }
            catch (Exception ex)
            {
                Utility.logError("cosBroadcastStatus unexpected exception caught.", fileName_, ex);
            }
            return result;
        }

        public bool IsPaServerConnected(cosMessageHeader header)
        {
            bool result = false;
            try
            {
                Execute(() =>
                {
                    Utility.log("cosIsPaServerConnected triggering ", fileName_);
                    result = ((ICosPAAgent)server_).cosIsPaServerConnected(header);
                    Utility.log("cosIsPaServerConnected completed, result=" + result.ToString(), fileName_);
                });
            }
            catch (Exception ex)
            {
                Utility.logError("cosIsPaServerConnected unexpected exception caught.", fileName_, ex);
            }
            return result;
        }

    }
}
