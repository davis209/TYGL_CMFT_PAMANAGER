using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using ste.common.util;

namespace ste.pa.pamanager
{
    class Utility
    {
        #region const values
        public const string moduleName_ = "Utility";
        public const char SELECTION_TAG_SEPARATOR = ';';

        public const string CORBA_SERVER_NAME = ste.syscontrol.cos.STR_PA_SERVER.ConstVal;
        public const string CONFIG_FILENAME = "config.ini";
        public const string REGISTRY_NAME = "MFT_HOME";
        public const string STD_APP_NAME = "PAMGR";//"PaMGR"; //PAManager
        public const string PROG_NAME = "PAManager";

        public const string OCC_LOCATION_NAME = "OCC";
        public const string STN_LOCATION_NAME = "STN";
        public const string ALL_LOCATION_NAME = "ALL";
        public const string UNDEFINED_LOCATION_NAME = "unknown";

        public const string SUBSCRIBER_NAME = "PAMANAGER";
        public const int INVALID_PRIORITY_VALUE = -999;
        public const int DISCONNECTED_PRIORITY_VALUE = -2;
        public const int IDLE_PRIORITY_VALUE = -1;

        #endregion

        #region access rights const values
        public const string PA_MGR_RUN_APPLICATION = "PA_MGR_RUN_APPLICATION";
        public const string PA_MGR_EXECUTE_LIVE = "PA_MGR_EXECUTE_LIVE";
        public const string PA_MGR_EXECUTE_PREDEFINE = "PA_MGR_EXECUTE_PREDEFINE";
        public const string PA_MGR_CONFIG = "PA_MGR_CONFIG";
        #endregion

        #region common parameters
        public static GeneralFunction generalFunctions = null;
        public static string DEFAULT_LANGUAGE = String.Empty;

        static int appId_ = 0;
        public static int AppId { get { return appId_; } }

        static string appName_ = "";
        public static string AppName { get { return PROG_NAME; } }

        static int serviceCorbaPort_ = 0;
        public static int ServiceCorbaPort { get { return serviceCorbaPort_; } }

        static string serviceName_ = "";
        public static string ServiceName { get { return serviceName_; } }

        static int agentCorbaPort_ = 0;
        public static int AgentCorbaPort { get { return agentCorbaPort_; } }

        static string paAgentName_ = "";
        public static string PaAgentName { get { return paAgentName_; } }

        static string paAgentHost_ = "";
        public static string PaAgentHost { get { return paAgentHost_; } }

        static int pollTimeInMs_ = 500;
        public static int PollTimeInMs { get { return pollTimeInMs_; } }

        static string corbaStringEncoding_ = "iso-8859-1";
        public static string CorbaStringEncoding { get { return corbaStringEncoding_; } }

        static string consolePredefinedPriMsgRootDir_ = "";
        public static string ConsolePredefinedMsgPriRootDir { get { return consolePredefinedPriMsgRootDir_; } }

        static string consolePredefinedMsgSecRootDir_ = "";
        public static string ConsolePredefinedMsgSecRootDir { get { return consolePredefinedMsgSecRootDir_; } }

        // Base on toUsePredefinedMsgPrimaryDirectory, this function will decide which path should be used 
        public static string ConsolePredefinedMsgRootDir
        {
            get
            {
                if (toUsePredefinedMsgPrimaryDirectory)
                    return consolePredefinedPriMsgRootDir_;
                else
                    return consolePredefinedMsgSecRootDir_;
            }
        }
        
        static string consoleSupportedAudioFormat_ = "";
        public static string ConsoleSupportedAudioFormat { get { return consoleSupportedAudioFormat_; } }

        static int mainSchedulerLocationId_ = 1;
        public static int MainSchedulerLocationId { get { return mainSchedulerLocationId_; } }

        static int maxNumOfMsgPerBroadcast_ = 3;
        public static int MaxNumOfMsgPerBroadcast { get { return maxNumOfMsgPerBroadcast_; } }

        static int pollIntervalInMs_ = 1000;
        public static int PollIntervalInMs { get { return pollIntervalInMs_; } }

        static int corbaRetryIntervalInMs_ = 1000;
        public static int CorbaRetryIntervalInMs { get { return corbaRetryIntervalInMs_; } }

        static int subscriberExpiryTime_ = 30000;
        public static int SubscriberExpiryTime { get { return subscriberExpiryTime_; } }

        static int pollScheduleStatusIntervalInMs_ = 30000;
        public static int PollScheduleStatusIntervalInMs { get { return pollScheduleStatusIntervalInMs_; } }

        static bool toUsePredefinedMsgPrimaryDirectory = true;
        public static bool ToUsePredefinedMsgPrimaryDirectory { get { return toUsePredefinedMsgPrimaryDirectory; } }

        public static bool CanRunApplication { get; set; }

        public static bool CanExecuteLiveBroadcast { get; set; }

        // Combined state of whether user can execute predefined broadcast
        public static bool CanExecutePredefinedBroadcast 
        { 
            get 
            {
                return (CanAccessSharedDirectory && HasAccessRightsExecutePredefinedBroadcast);
            } 
        }

        public static bool CanExecuteConfig { get; set; }

        public static bool CanExecuteSavePredefinedMsg { get { return CanAccessSharedDirectory && toUsePredefinedMsgPrimaryDirectory; } }

        public static bool CanAccessSharedDirectory { get; set; }

        public static bool HasAccessRightsExecutePredefinedBroadcast { get; set; }

        #endregion

#region Commonlib-related initialization
        static public bool loadCommonLibAndConfig()
        {
        DatabaseWrapper dbLock = new DatabaseWrapper();
        string configFolder = System.Environment.GetEnvironmentVariable(REGISTRY_NAME);
            if (String.IsNullOrEmpty(configFolder))
                return false;

            generalFunctions = new GeneralFunction();

            appId_ = ApplicationInfo.getAppId(STD_APP_NAME, dbLock);
            appName_ = ApplicationInfo.getAppName(appId_, dbLock);

            int serviceCorbaPort = 0;
            string serviceName = String.Empty;
            if (Int32.TryParse(ConfigFile.GetINIDataString("PA_MANAGER", "PA_SERVICE_PORT", "", 255, Path.Combine(configFolder,CONFIG_FILENAME)), out serviceCorbaPort))
            {
                log("PA_SERVICE_PORT is set to [" + serviceCorbaPort + "]", moduleName_);
            }
            else
            {
                log("Unable to get PA_SERVICE_PORT. Intialization failed.", moduleName_);
                return false;
            }

            serviceName = ConfigFile.GetINIDataString("PA_MANAGER", "PA_SERVICE_HOST", "", 255, Path.Combine(configFolder,CONFIG_FILENAME));
            if (String.IsNullOrEmpty(serviceName))
            {
                log("Unable to get PA_SERVICE_HOST. Intialization failed.", moduleName_);
                return false;
            }

            serviceCorbaPort_ = serviceCorbaPort;
            serviceName_ = serviceName;

            int agentCorbaPort = 0;
            string paAgentName = String.Empty;
            string paAgentHost = string.Empty;

            if (Int32.TryParse(ConfigFile.GetINIDataString("PA_MANAGER", "PA_AGENT_PORT", "", 255, Path.Combine(configFolder,CONFIG_FILENAME)), out agentCorbaPort))
            {
                log("PA_AGENT_PORT is set to [" + agentCorbaPort + "]", moduleName_);
            }
            else
            {
                agentCorbaPort = 3050;
                log("PA_AGENT_PORT is set to default value 3050", moduleName_);
            }

            paAgentHost = ConfigFile.GetINIDataString("PA_MANAGER", "PA_AGENT_HOST", "", 255, Path.Combine(configFolder,CONFIG_FILENAME));
            if (String.IsNullOrEmpty(paAgentHost))
            {
                paAgentHost = "localhost";
                log("PA_AGENT_HOST is set to default value localhost", moduleName_);
            }

            paAgentName = ConfigFile.GetINIDataString("PA_MANAGER", "PA_AGENT_NAME", "", 255, Path.Combine(configFolder,CONFIG_FILENAME));
            if (String.IsNullOrEmpty(paAgentName))
            {
                paAgentName = "PaAgent";
                log("PA_AGENT_NAME is set to default value PaAgent", moduleName_);
            }


            agentCorbaPort_ = agentCorbaPort;
            paAgentName_ = paAgentName;
            paAgentHost_ = paAgentHost;

            DEFAULT_LANGUAGE = ConfigFile.GetINIDataString("MFT_LANGUAGE", "DEFAULT_LANGUAGE", "", 255, Path.Combine(configFolder,CONFIG_FILENAME));
            if (String.IsNullOrEmpty(DEFAULT_LANGUAGE))
            {
                DEFAULT_LANGUAGE = "en-US";
            }
            log("DEFAULT_LANGUAGE for the application = " + DEFAULT_LANGUAGE, moduleName_);

            string corbaStrEncoding = ConfigFile.GetINIDataString("PA_MANAGER", "PA_CORBA_STRING_ENCODING", "", 255, Path.Combine(configFolder,CONFIG_FILENAME));
            if (String.IsNullOrEmpty(corbaStrEncoding))
            {
                log("Unable to get PA_CORBA_STRING_ENCODING. Intialization failed.", moduleName_);
                return false;
            }
            corbaStringEncoding_ = corbaStrEncoding;
            log("CORBA String encoding for c# application = " + CorbaStringEncoding, moduleName_);


            if (Int32.TryParse(ConfigFile.GetINIDataString("PA_MANAGER", "PA_MAIN_SCHEDULER_LOCATION", "", 255, Path.Combine(configFolder,CONFIG_FILENAME)), out mainSchedulerLocationId_))
            {
                log("PA_MAIN_SCHEDULER_LOCATION is set to [" + mainSchedulerLocationId_ + "]", moduleName_);
            }
            else
            {
                log("Unable to get PA_MAIN_SCHEDULER_LOCATION. Intialization failed.", moduleName_);
                return false;
            }

            if (Int32.TryParse(ConfigFile.GetINIDataString("PA_MANAGER", "PA_POLL_CORBA_INTERVAL_MSEC", "1000", 255, Path.Combine(configFolder,CONFIG_FILENAME)), out pollIntervalInMs_))
            {
                log("PollInterval is set to [" + pollIntervalInMs_ + "]", moduleName_);
            }
            else
            {
                log("Unable to get PA_POLL_CORBA_INTERVAL_MSEC. Intialization failed.", moduleName_);
                return false;
            }

            if (Int32.TryParse(ConfigFile.GetINIDataString("PA_MANAGER", "PA_CORBA_RETRY_INTERVAL_MSEC", "200", 255, Path.Combine(configFolder,CONFIG_FILENAME)), out corbaRetryIntervalInMs_))
            {
                log("corbaRetryIntervalInMs_ is set to [" + corbaRetryIntervalInMs_ + "]", moduleName_);
            }
            else
            {
                log("Unable to get PA_CORBA_RETRY_INTERVAL_MSEC. Intialization failed.", moduleName_);
                return false;
            }

            if (Int32.TryParse(ConfigFile.GetINIDataString("PA_MANAGER", "PA_SUBSCRIBER_EXPIRY", "30000", 255, Path.Combine(configFolder,CONFIG_FILENAME)), out subscriberExpiryTime_))
            {
                log("subscriberExpiryTime_ is set to [" + subscriberExpiryTime_ + "]", moduleName_);
            }
            else
            {
                log("Unable to get PA_SUBSCRIBER_EXPIRY. Intialization failed.", moduleName_);
                return false;
            }

            if (Int32.TryParse(ConfigFile.GetINIDataString("PA_MANAGER", "PA_POLL_SCHEDULE_INTERVAL_MS", "30000", 255, Path.Combine(configFolder,CONFIG_FILENAME)), out pollScheduleStatusIntervalInMs_))
            {
                log("pollScheduleStatusIntervalInMs_ is set to [" + pollScheduleStatusIntervalInMs_ + "]", moduleName_);
            }
            else
            {
                log("Unable to get PA_POLL_SCHEDULE_INTERVAL_MS. Intialization failed.", moduleName_);
                return false;
            }

            if (Boolean.TryParse(ConfigFile.GetINIDataString("PA_MANAGER", "PA_PREDEF_USE_PRI_DIR", "1", 255, Path.Combine(configFolder,CONFIG_FILENAME)), out toUsePredefinedMsgPrimaryDirectory))
            {
                log("toUsePredefinedMsgPrimaryDirectory is set to [" + toUsePredefinedMsgPrimaryDirectory + "]", moduleName_);
            }
            else
            {
                log("Unable to get PA_PREDEF_USE_PRI_DIR. Intialization failed.", moduleName_);
                return false;
            }

            return true;
        }
        
        public static string getPaServiceCorbaAddress()
        {
            return "corbaloc::" + ServiceName + ":" + ServiceCorbaPort + "/" + CORBA_SERVER_NAME;
        }

        public static string getPaAgentCorbaAddress()
        {
            return "corbaloc::" + PaAgentHost + ":" + AgentCorbaPort + "/" + PaAgentName;
        }

        #endregion

        #region logging functions
        public static void logDebug(String message, string appName)
        {
            //if (Utility.IS_IN_DBG_MODE)
            //    LoggerManager.WriteAppLog(message, appName);
            //else
            LoggerManager.logger.Debug(message + "[" + appName + "]");
        }

        public static void log(String message, string appName)
        {
            LoggerManager.WriteAppLog(message, appName);
        }

        public static void logError(String message, string appName)
        {
            LoggerManager.WriteAppLog(message, appName);
        }

        public static void logError(String message, string appName, Exception e)
        {
            LoggerManager.logger.Error(message + "[" + appName + "]", e);
        }

        public static void logWarn(String message, string appName)
        {
            LoggerManager.logger.Warn(message + "[" + appName + "]");
        }
#endregion

#region Load console-related parameters
        static public bool loadDbSettings()
        {
            PaConsoleConfigDataSource consoleConfig = new PaConsoleConfigDataSource();
            return consoleConfig.getConsoleParam(out consolePredefinedPriMsgRootDir_, out consolePredefinedMsgSecRootDir_, out consoleSupportedAudioFormat_, out maxNumOfMsgPerBroadcast_);
        }
#endregion

    }
}
