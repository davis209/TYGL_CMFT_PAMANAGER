using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ste.pa.cos;
using System.Runtime.Remoting;
using ste.syscontrol.cos;
using ste.common.util;
using System.Threading;
using omg.org.CosNaming;

namespace ste.pa.pamanager
{
    abstract class CorbaServer<T> : AbstractThread
    {
        public delegate void onAgentStatusUpdateDelegate(bool isConnected);
        public delegate void onIncomingCorbaEventDelegate(cosStatusUpdate[] eventArray);
        protected string serverAddress_ = "";           // Corba address of the service
        protected object server_ = null;                // COrba object
        protected string fileName_;
        readonly int CORBA_MAX_RETRIES = 1;
       
        protected enum ENUM_CONNECTION_STATE { UNKNOWN, CONNECTED, DISCONNECTED }

        protected event onAgentStatusUpdateDelegate onAgentStatusUpdateEvent;
        protected event onIncomingCorbaEventDelegate onIncomingCorbaEventEvent;

        protected int subscriptionGroupId_ = -1;
        protected string subscriptionGroupName_ = "";
        protected ENUM_CONNECTION_STATE connectionState_;

        protected readonly object eventLock = new object();
        protected readonly object syncLock = new object();
        protected readonly object serverLock = new object();

        public CorbaServer(string srvAddr, string loggingName)
            : base(Utility.PollIntervalInMs)
        {
            serverAddress_ = srvAddr;
            fileName_ = loggingName;
        }

        public void register(onAgentStatusUpdateDelegate func,
                             onIncomingCorbaEventDelegate processFunc)
        {
            lock (eventLock)
            {
                if (func != null)
                    onAgentStatusUpdateEvent += func;

                if (processFunc != null)
                    onIncomingCorbaEventEvent += processFunc;
            }
        }



        public void deregister(onAgentStatusUpdateDelegate func,
                               onIncomingCorbaEventDelegate processFunc)
        {
            lock (eventLock)
            {
                if (func != null)
                    onAgentStatusUpdateEvent -= func;

                if (processFunc != null)
                    onIncomingCorbaEventEvent -= processFunc;

            }
        }

        protected void RetryFunction(Action action)
        {
            if (action == null)
                throw new ArgumentNullException("action");


            // Retry only when it is a connection lost.  otherwise 
            // just throw the exception back to user to continue 
            // processing specifc errors
            for (int i = 0; i < CORBA_MAX_RETRIES; i++)
            {
                try
                {
                    action.Invoke();
                    break;
                }
                catch (CorbaServerNotAvailableException)
                {
                    invalidateServerObject();
                    if (i == CORBA_MAX_RETRIES - 1)
                        throw;
                }
                catch (omg.org.CORBA.TRANSIENT transient)
                {
                    Utility.logError("omg.org.CORBA.TRANSIENT Exception encountered => " + transient.Message, fileName_);
                    invalidateServerObject();
                    
                    if (i == CORBA_MAX_RETRIES - 1)
                        throw new CorbaServerNotAvailableException("Exception omg.org.CORBA.TRANSIENT detected. Treating it as a disconnect");
                }
                catch (omg.org.CORBA.OBJECT_NOT_EXIST notexist)
                {
                    Utility.logError("omg.org.CORBA.OBJECT_NOT_EXIST Exception encountered => " + notexist.Message, fileName_);
                    invalidateServerObject();

                    if (i == CORBA_MAX_RETRIES - 1)
                        throw new CorbaServerNotAvailableException("Exception omg.org.CORBA.OBJECT_NOT_EXIST detected. Treating it as a disconnect");
                }
                catch (omg.org.CORBA.TIMEOUT timeoutExc)
                {
                    Utility.logError("omg.org.CORBA.TIMEOUT Exception encountered => " + timeoutExc.Message, fileName_);
                    invalidateServerObject();

                    if (i == CORBA_MAX_RETRIES - 1)
                        throw;
                }
                catch (CosPAServiceException corbaEx)
                {
                    Utility.logError("CosPAServiceException Exception (" + corbaEx.errorCode + ")caught. Not retrying", fileName_, corbaEx);
                    throw;
                }
                catch (Exception e)
                {
                    Utility.logError("Unknown exception caught", fileName_, e);
                    throw;
                }
                Thread.Sleep(Utility.CorbaRetryIntervalInMs);
            }
        }

        // Override this function if we are using namingmanager instead
        internal virtual object getServerReference()
        {
            if (server_ == null)
            {
                Utility.log("Connecting to service Exist at " + serverAddress_, fileName_);
                server_ = (T)RemotingServices.Connect(typeof(T), serverAddress_);
            }

            return server_;
        }

        internal void invalidateServerObject()
        {
            server_ = null; //throw new CorbaServerNotAvailableException("Service not available @ [" + serverAddress_ + "]");
            Utility.logError("Service not available @ [" + serverAddress_ + "]", fileName_);

            if(connectionState_ != ENUM_CONNECTION_STATE.DISCONNECTED)
                notifyAgentStatusUpdate(false);
        }

        // Child classes only need to call the respective corba methods and return the subscription ID
        // No need to handle or catch exceptions.  They will be handled accordingly by this parent class
        // in function connectToServer()
        protected abstract int createSubscriptionGroup(string name, int expiryTime);

        // Execute will check that connection is there before executing the command
        // If drop in the connection, it will update the internal state
        protected void Execute(Action action)
        {
            bool lockTaken = false;
            Monitor.Enter(serverLock, ref lockTaken);
            try
            {
                if (action == null)
                    throw new ArgumentNullException("null action passed to Execute function");

                RetryFunction(() =>
                {
                    if (connectToServer())
                        action();
                    else
                        Utility.logError("Unable to get local radio agent", fileName_);
                });

            }
            catch (CorbaServerNotAvailableException)
            {
                onAgentDisconnected();
            }
            finally
            {
                if (lockTaken)
                    Monitor.Exit(serverLock);
            }
        }

        // Return true if is connected to the agent
        // If there is a sudden drop in the connection, it will update the internal state of corbaserver
        // and raise the event to notify interested parties
        // If a connection to the agent is now detected, it will create the subscription group automatically
        protected virtual bool connectToServer()
        {
            if (server_ != null)
                return true;

            // attempt to get active agent reference
            getServerReference();
            if (server_ == null)
            {
                return false;
            }

            Utility.log("Service went online, available @ [" + serverAddress_ + "]", fileName_);
            connectionState_ = ENUM_CONNECTION_STATE.CONNECTED;
            notifyAgentStatusUpdate(true);
            return true;

            // Can get agent successfully, now attempt to create the subscription group
            // If failed, exception will be thrown and caught by the calling function
            // Calling function should do the cleanup and notification
            //bool lockTaken = false;
            //Monitor.Enter(syncLock, ref lockTaken);
            //try
            //{
            //    lock (syncLock)
            //    {
            //        subscriptionGroupName_ = Utility.SUBSCRIBER_NAME + "_" + DateTime.Now.Ticks;
            //        subscriptionGroupId_ = createSubscriptionGroup(subscriptionGroupName_, Utility.SubscriberExpiryTime);
            //        if (subscriptionGroupId_ >= 0)
            //        {
            //            Utility.log("Service went online, available @ [" + serverAddress_ + "]", fileName_);
            //            connectionState_ = ENUM_CONNECTION_STATE.CONNECTED;
            //            notifyAgentStatusUpdate(true);
            //        }
            //    }
            //}
            //finally
            //{
            //    if (lockTaken)
            //        Monitor.Exit(syncLock);
            //}
            //return true;
        }

        // If internal state is already disconnected, no notifcation will be raise 
        protected void onAgentDisconnected()
        {
            lock (syncLock)
            {
                // Already in disconnected state, no need to go on further
                if (connectionState_ == ENUM_CONNECTION_STATE.DISCONNECTED)
                    return;

                Utility.logError("onAgentDisconnected::Service not available @ [" + serverAddress_ + "]", fileName_);
                connectionState_ = ENUM_CONNECTION_STATE.DISCONNECTED;
                subscriptionGroupId_ = -1;
            }
        }

        protected virtual void notifyAgentStatusUpdate(bool isConnected)
        {
            lock (eventLock)
            {
                if (onAgentStatusUpdateEvent != null)
                    onAgentStatusUpdateEvent(isConnected);
            }
        }

        protected void notifyIncomingCorbaEvent(cosStatusUpdate[] eventArray)
        {
            lock (eventLock)
            {
                if (onIncomingCorbaEventEvent != null && eventArray.Length > 0)
                {
                    Utility.logError("notifyIncomingCorbaEvent::Incoming corba events. Notifying subscribers", fileName_);
                    onIncomingCorbaEventEvent(eventArray);
                }
            }
        }

        internal static string encodeStringForCorba(string strToEncode)
        {
            Encoding unicode = Encoding.Unicode;
            Encoding isoLatin1 = System.Text.Encoding.GetEncoding(Utility.CorbaStringEncoding);   /* java server is "iso-8859-1" */

            //get the bytes in utf-16 format
            byte[] unicodeBytes = unicode.GetBytes(strToEncode);

            //force it using ISO-Latin-1 to encode the string to work around 
            //JacOrb/iiop.net limitation of codeset conversion.
            char[] isoLatin1Chars = new char[isoLatin1.GetCharCount(unicodeBytes, 0, unicodeBytes.Length)];
            isoLatin1.GetChars(unicodeBytes, 0, unicodeBytes.Length, isoLatin1Chars, 0);
            return new String(isoLatin1Chars);
        }

        internal static String decodeStringFromCorba(string input)
        {
            Encoding unicode = Encoding.Unicode;
            Encoding isoLatin1 = System.Text.Encoding.GetEncoding(Utility.CorbaStringEncoding);

            //force it using ISO-Latin-1 to decode the string to work around 
            //JacOrb/iiop.net limitation of codeset conversion.
            byte[] isoBytes = isoLatin1.GetBytes(input);
            byte[] unicodeBytes = Encoding.Convert(unicode, unicode, isoBytes);
            char[] unicodeChars = new char[unicode.GetCharCount(unicodeBytes, 0, unicodeBytes.Length)];
            unicode.GetChars(unicodeBytes, 0, unicodeBytes.Length, unicodeChars, 0);
            return new String(unicodeChars);
        }
    }
}
