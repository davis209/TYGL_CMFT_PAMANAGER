#region Header Block
/// <classname>Program</classname>
/// <description>This module allows runs the PA Manager application.</description>
/// <filename>Program.cs</filename>
/// <authors>Tay Yu Hui</authors>
#endregion

using System;						// Contain all other namespaces as subordinates
using System.Collections.Generic;	// Provide commonly used classes for the creation of MFT
using System.Net;                   // Provides a simple programming interface for many of the protocols used on networks
using System.Reflection;	        // Retrieve information about assemblies, modules, members, parameters, and other entities in managed code by examining their metadata. 
using System.Threading;				// Enable multithreaded programming
using System.Windows.Forms;			// Contain classes for creating Window-based applications
using System.Data.SqlClient;
using System.IO;
using ste.common.exceptions;
using ste.common.util;
using ste.common.Forms;
using ste.common.eventmanagement;
using System.Data;

namespace ste.pa.pamanager
{
    /// <summary>
    /// Main point of entry for PA Manager
    /// </summary>	

    public static class Program
    {
        private const string stdAppName_ = "PAMGR";
        private static string appName_ = "";
        private static string fileName_ = MethodBase.GetCurrentMethod().DeclaringType.Name;
        private static GeneralFunction generalFun_ = new GeneralFunction();
        private static object fileLock_ = new object();

        public static DatabaseWrapper dbLock = new DatabaseWrapper();

        public static int screenX = 0;
        public static int screenY = 0;

        public static bool showIndex = false;
        public static string defaultLanguage = "en-US";
        public static string localUser = "";
        public static string moduleName = "PAManager";
        public static string hostName = "";
        public static string configFolder_ = "";
        public static int profileLocIndex = 0;
        public static List<KeyValuePair<int, string>> seats_ = new List<KeyValuePair<int, string>>();
        /// <summary>
        /// How long to keep the lock on the equipment exclusive lock
        /// </summary>
        public static int paAgentLockInterval = 30;

        /// <summary>
        /// Polling interval during Monitor mode
        /// </summary>
        public static int monitorTimeout = 60000;

        /// <summary>
        /// Occ mode is true, if profile is set to OCC location
        /// </summary>
        public static bool occMode = false;

        /// <summary>
        /// Occ mode is true, if profile is set to Depot location
        /// </summary>
        public static bool depotMode = false;

        /// <summary>
        /// Control mode is true, if operator is able to fully control the PA System
        /// </summary>
        public static bool controlMode = false;

        /// <summary>
        /// Remote mode is true if current mft location is not the same as profile location
        /// </summary>
        public static bool remoteMode = false;

        public static Dictionary<int, string> stnNoToStnNameDic_ = new Dictionary<int, string>();

        public static Dictionary<int, EnumType.StationStatus> stationStatusDic_ = new Dictionary<int, EnumType.StationStatus>();

        public static Dictionary<int, string> deviceNameDic_ = new Dictionary<int, string>();

        public static List<string> liveStatusList_ = new List<string>();

        public static List<string> airRaidStatusList_ = new List<string>();

        /// <summary>
        /// Maximum Station Id
        /// </summary>
        public const int MAXSTNID = 50;

        /// <summary>
        /// Number of bits per byte
        /// </summary>
        public const int BITS_PER_BYTE = 8;

        /// <summary>
        /// Size of packet header
        /// </summary>
        public const int WRITE_HEADER_SIZE = 5;

        /// <summary>
        /// Maximum number of source Id
        /// </summary>
        public const int MAXSRCID = 32;

        /// <summary>
        /// Maximum Zone Id
        /// </summary>
        public const int MAXZONEID = 64;

        /// <summary>
        /// Total number of DVA Messages (include adhoc messages)
        /// </summary>
        public const int MAXDVAMSG = 500;   // need to add offset for STN Messages

        /// <summary>
        /// Number of DVA messages (exclude adhoc messages)
        /// </summary>
        public const int NBPRECSET = 494;

        /// <summary>
        /// Number of adhoc message available
        /// </summary>
        public const int NBADHOC = 6;

        /// <summary>
        /// Maximum number of dva message in a broadcast
        /// </summary>
        public const int MAXDVAPERSEQ = 4;

        /// <summary>
        /// Maximum number of message slots
        /// </summary>
        public const int MAXMSGSEQ = 14;
        /// <summary>
        /// Progress update interval for Dva Broadcast and adhoc recording
        /// </summary>
        public static int dvaBcPoll = 300;

        /// <summary>
        ///  Live Broadcast PTT button 'polling' interval
        /// </summary>
        public static int liveBcPttPollSec = 300;

        /// <summary>
        /// UTC timezone offset(in sec) = 8hr * 60min/hr * 60sec/min
        /// </summary>
        public static int utcTimeOffset = 28800;

        /// <summary>
        /// IP address of Primary PA System
        /// </summary>
        public static string paSysAddPri = "";

        /// <summary>
        /// IP address of Secondary PA System
        /// </summary>
        public static string paSysAddSec = "";

        /// <summary>
        /// Port number to connect to PA System
        /// </summary>
        public static int paSysPort = 0;

        #region Config File
        /// <summary>
        /// Debug mode - Set to true to have a more detailed printing of log file
        /// </summary>
        public static bool debugMode = false;

        /// <summary>
        /// Timeout for Live Broadcast request - in ms
        /// </summary>
        public static int liveBcTimeoutMs = 60000;

        /// <summary>
        ///  Max time to wait for reply from PA System
        /// </summary>
        public static int pacTimeoutMs = 10000;

        /// <summary>
        /// Fast Poll interval for Flag table 560 & Table 200/1200 - in ms
        /// </summary>
        public static int fastPollIntervalMs = 500;

        /// <summary>
        /// Time interval to try to reconnect to PA System - in ms
        /// </summary>
        public static int pasConnectionIntervalMs = 1000;
        #endregion

        // Error messages to be localized
        private static string sysErr_ = "PA Manager";
        private static string mftNotConfig_ = "Current MFT not configured for use with PA System.";
        private static string MB_ErrNoRights_ = "User does not have the rights to run PA Manager.";
        private static string MB_PaRunning_ = "PA Manager is already running";
        private static string MB_LoginMft_ = "No user specified. Application will now exit!";
        private static string MB_NotAuth_ = "User Not Authenticated. Please Login!";
        private static string MB_UserNotMatch_ = "Wrong user specified. Application will now exit!";
        private static string MB_InvalidArgs_ = "Invalid arguments detected! PA Manager will not start!";
        public static string MB_FormEx = "An error has occurred, please try to relaunch the form.";
        public static string MB_GeneralEx = "An error has occurred, please contact the system administrator.";
        public static string MB_NoDBConn = "The program cannot establish a connection to the database, please check your connection!";
        public static string MB_NoPaAgent = "The program cannot establish a connection to the PA Agent!";
        public static string LB_PasConnectionFail = "PA System Connection Failure!";
        public static string saveBeforeExit = "Do you want to save the edited values?";
        public static string pasReplyTimeOut = "Timeout in waiting for response from PA System!";
        public static string langChinese_ = "Chinese";
        public static string langTw_ = "Taiwanese";
        public static string langHakka_ = "Hakka";
        public static string langEnglish_ = "English";

        /// <summary>
        /// Static constructor 
        /// </summary>
        static Program()
        {
        }

        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        /// <param name="argsList">Runtime input parameters</param>
        [STAThread]
        static void Main(string[] argsList)
        {
            //	Load config.ini settings
            //	Localize Form
            //	Get hostname
            //	Parse Arguments (e.g. read localuser arguments)
            //	Check if process exists, if yes prompt user, then close
            //	Try to connect to Oracle DB, if cannot connect, will have exception
            //	Check if user is authenticated, if not prompt user, then close
            //	Initialize EventApi
            //	Check if current user ID is the same as localuser, if not prompt user, then close
            //	Launch PA Manager

            // Nothing to validate : argsList

            ////dbConnEnum dbConn = dbConnEnum.ErrNoConn;

            try
            {
                Application.EnableVisualStyles();
                Application.SetCompatibleTextRenderingDefault(false);

                Localization.LoadEventMessages(moduleName);

                loadConfig();
                localizeForm();
                hostName = Dns.GetHostName();

                if (Utility.loadCommonLibAndConfig() == false)
                {
                    WriteEventLog("[ERROR] Invalid runtime arguments specified", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
                    Program.MessageBox_Error(MB_UserNotMatch_, sysErr_);
                    return;
                }

                //if (!parseArguments(argsList))
                //{
                //    WriteEventLog("[ERROR] Invalid runtime arguments specified", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
                //    return;
                //}

                if (processExists(moduleName))
                {
                    Program.MessageBox_Error(MB_PaRunning_, sysErr_);
                    return;
                }

                initEventApi();

                //if (!checkLocalUser())
                //{
                //    WriteEventLog(MB_UserNotMatch_, fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
                //    Program.MessageBox_Error(MB_UserNotMatch_, sysErr_);
                //    return;
                //}

                //profileLocIndex = Translater.getProfileLocationId();
                profileLocIndex = int.Parse(ConfigFile.GetINIDataString("DEFAULT_MFT_SETTING", "DEF_LOCATION_ID", "", 255, Path.Combine(configFolder_, "config.ini")));
                remoteMode = isRemoteMode();

                if (!paSettingValid(remoteMode, profileLocIndex.ToString(), hostName))
                {
                    MessageBox_Error(mftNotConfig_, sysErr_);
                    return;
                }

                if (rightsToRunPaManager())
                {
                    string[] eventParam = new string[] { "", "" };
                    string[] eventParamLocal = new string[] { "", "" };
                    logEvent(EnumType.PaEvent.PaStart, eventParam, eventParamLocal);

                    System.Threading.Thread.CurrentThread.CurrentUICulture = System.Globalization.CultureInfo.GetCultureInfo(defaultLanguage);

                    Application.Run(new PAManager());
                }
                else
                {
                    MessageBox_Error(MB_ErrNoRights_, sysErr_);
                }
            }
            catch (NoOracleDBException ex)
            {
                WriteEventLog("[ERROR] [NoOracleDBException] " + MB_NoDBConn + " Details: " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

                string[] eventParam = new string[] { "", "", Localization.GetEventMessage("EVT_NO_ORACLE_DB", "en-US") };
                string[] eventParamLocal = new string[] { "", "", Localization.GetEventMessage("EVT_NO_ORACLE_DB", "zh-TW") };
                Program.logEvent(EnumType.PaEvent.PaStop, eventParam, eventParamLocal);

                MessageBox_Error(MB_NoDBConn, sysErr_);
            }
            catch (ObjectDisposedException ex)
            {
                WriteEventLog("[ERROR] [ObjectDisposedException] " + MB_GeneralEx + " Details: " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

                string[] eventParam = new string[] { "", "", Localization.GetEventMessage("EVT_GENERAL_EX", "en-US") };
                string[] eventParamLocal = new string[] { "", "", Localization.GetEventMessage("EVT_GENERAL_EX", "zh-TW") };
                Program.logEvent(EnumType.PaEvent.PaStop, eventParam, eventParamLocal);

                MessageBox_Error(MB_GeneralEx + "\n(" + ex.Message + ")", sysErr_);
            }
            catch (NullReferenceException ex)
            {
                WriteEventLog("[ERROR] [NullReferenceException] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }
            catch (Exception ex)
            {
                WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

                string[] eventParam = new string[] { "", "", Localization.GetEventMessage("EVT_GENERAL_EX", "en-US") };
                string[] eventParamLocal = new string[] { "", "", Localization.GetEventMessage("EVT_GENERAL_EX", "zh-TW") };
                Program.logEvent(EnumType.PaEvent.PaStop, eventParam, eventParamLocal);

                MessageBox_Error(MB_GeneralEx, sysErr_);
            }
            finally
            {
                cleanUp();
            }
        }

        /// <summary>
        /// This method sets the variables to its localized equivalent text
        /// </summary>
        private static void localizeForm()
        {
            System.Globalization.CultureInfo ci = new System.Globalization.CultureInfo(defaultLanguage);
            Localization.setCulture(ci);
            Localization.setApplicationName(moduleName);

            if (Localization.configure())
            {
                sysErr_ = Localization.localize("PA_FORM_NAME", sysErr_);
                mftNotConfig_ = Localization.localize("PA_MFT_NOT_CONFIG", mftNotConfig_);
                MB_ErrNoRights_ = Localization.localize("PRG_MB_NO_RIGHTS", MB_ErrNoRights_);
                MB_PaRunning_ = Localization.localize("PRG_MB_SM_RUNNING", MB_PaRunning_);
                MB_LoginMft_ = Localization.localize("PRG_MB_LOGIN_MFT", MB_LoginMft_);
                MB_NotAuth_ = Localization.localize("PRG_MB_NOT_AUTH", MB_NotAuth_);
                MB_InvalidArgs_ = Localization.localize("PA_INVALID_ARGS", MB_InvalidArgs_);

                MB_FormEx = Localization.localize("PRG_MB_FORM_EX", MB_FormEx);
                MB_GeneralEx = Localization.localize("PRG_MB_GENERAL_EX", MB_GeneralEx);
                MB_NoDBConn = Localization.localize("PRG_MB_NO_DB_CONN", MB_NoDBConn);
                MB_NoPaAgent = Localization.localize("PA_NO_PA_AGENT_LINK", MB_NoPaAgent);
                MB_UserNotMatch_ = Localization.localize("PRG_MB_USER_NOT_MATCH", MB_UserNotMatch_);
                LB_PasConnectionFail = Localization.localize("PA_NO_PAS_LINK", LB_PasConnectionFail);

                saveBeforeExit = Localization.localize("SAVE_CHANGES", saveBeforeExit);
                pasReplyTimeOut = Localization.localize("EVT_PAS_REPLY_TIMEOUT", pasReplyTimeOut);

                langChinese_ = Localization.localize("TB_LANGUAGE_CHINESE", langChinese_);
                langTw_ = Localization.localize("TB_LANGUAGE_TW", langTw_);
                langHakka_ = Localization.localize("TB_LANGUAGE_HAKKA", langHakka_);
                langEnglish_ = Localization.localize("TB_LANGUAGE_ENGLISH", langEnglish_);
            }
        }

        /// <summary>
        /// Load the configuration of PA Manager
        /// </summary>
        private static void loadConfig()
        {
            configFolder_ = System.Environment.GetEnvironmentVariable("MFT_HOME");
            defaultLanguage = ConfigFile.GetINIDataString("MFT_LANGUAGE", "DEFAULT_LANGUAGE", "zh-TW", 255, Path.Combine(configFolder_, "config.ini"));

            // Debug mode
            try
            {
                debugMode = bool.Parse(ConfigFile.GetINIDataString("PA_MANAGER", "DEBUG", "false", 255, Path.Combine(configFolder_, "config.ini")));

            }
            catch (Exception)
            {
                debugMode = false;
                Program.WriteEventLog("[ERROR] DEBUG not found in config file! Default value [" + debugMode + "] used!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }

            // Fast poll timeout timing
            try
            {
                fastPollIntervalMs = int.Parse(ConfigFile.GetINIDataString("PA_MANAGER", "FAST_POLL_INTERVAL_SEC", "500", 255, Path.Combine(configFolder_, "config.ini")));
                Program.WriteEventLog("[INFO] FAST_POLL_INTERVAL_SEC for PA System: " + fastPollIntervalMs, fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }
            catch (Exception)
            {
                Program.WriteEventLog("[ERROR] FAST_POLL_INTERVAL_SEC not found in config file! Default value [" + fastPollIntervalMs + "] used!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }

            // Live broadcast timeout interval
            try
            {
                liveBcTimeoutMs = int.Parse(ConfigFile.GetINIDataString("PA_MANAGER", "LIVE_TIMEOUT_MSEC", "60000", 255, Path.Combine(configFolder_, "config.ini")));
                Program.WriteEventLog("[INFO] LIVE_TIMEOUT_MSEC for PA System: " + liveBcTimeoutMs, fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }
            catch (Exception)
            {
                Program.WriteEventLog("[ERROR] LIVE_TIMEOUT_MSEC not found in config file! Default value [" + liveBcTimeoutMs + "] used!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }

            // Packet Request timeout timing
            try
            {
                pacTimeoutMs = int.Parse(ConfigFile.GetINIDataString("PA_MANAGER", "PAC_REQ_TIMEOUT_MSEC", "10000", 255, Path.Combine(configFolder_, "config.ini")));
                Program.WriteEventLog("[INFO] PAC_REQ_TIMEOUT_MSEC for PA System: " + pacTimeoutMs, fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }
            catch (Exception)
            {
                Program.WriteEventLog("[ERROR] PAC_REQ_TIMEOUT_MSEC not found in config file! Default value [" + pacTimeoutMs + "] used!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }

            // Time Interval to try to connect to PA System
            try
            {
                pasConnectionIntervalMs = int.Parse(ConfigFile.GetINIDataString("PA_MANAGER", "PAS_CONNECTION_INTERVAL", "1000", 255, Path.Combine(configFolder_, "config.ini")));
                Program.WriteEventLog("[INFO] PAS_CONNECTION_INTERVAL for PAS: " + pasConnectionIntervalMs, fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }
            catch (Exception)
            {
                Program.WriteEventLog("[ERROR] PAS_CONNECTION_INTERVAL not found in config file! Default value [" + pasConnectionIntervalMs + "] used!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }
        }

        /// <summary>
        /// Check if localuser matches logged in user id
        /// </summary>
        /// <returns>bool: true if it matches, otherwise false</returns>
        private static bool checkLocalUser()
        {
            //dbConnEnum dbConn = dbConnEnum.ErrNoConn;
            //string userName = generalFun_.getCurrentUserID(Program.dbLock.getActiveConnection(ref dbConn), appName_);
            string userName = ScadaAuthentication.getLoginUser(true, dbLock);
            return userName.Equals(localUser, StringComparison.CurrentCultureIgnoreCase);
        }

        /// <summary>
        /// Initialize Event API
        /// </summary>
        private static void initEventApi()
        {
            int appId_ = ApplicationInfo.getAppId(stdAppName_, dbLock);
            appName_ = ApplicationInfo.getAppName(appId_, dbLock);

            EventAPI.createEventContext(dbLock, appId_, localUser);
        }

        /// <summary>
        /// Check if current user has rights to run PA Manager
        /// </summary>
        /// <returns>bool: true is user has rights to run, otherwise false</returns>
        private static bool rightsToRunPaManager()
        {
            return true;
        }

        /// <summary>
        /// Check if current user has rights to run PA Manager Configuration Form
        /// </summary>
        /// <returns>bool: true is user has rights to run, otherwise false</returns>
        public static bool rightsToRunPaManagerConfiguration()
        {
            return true;
        }

        /// <summary>
        /// Check if the interested process is running on the system
        /// </summary>
        /// <param name="appName">Application Name</param>
        /// <returns>bool: true if the interested application is running, otherwise false</returns>
        private static bool processExists(string appName)
        {
            // Nothing to validate : appName
            System.Diagnostics.Process[] processList = System.Diagnostics.Process.GetProcessesByName(appName);

            if (processList.Length > 1)
            {
                return true;
            }
            else
            {
                return false;
            }

        }


        /// <summary>
        /// This method parses and process the command line arguments.
        /// </summary>
        /// <param name="args">The argument passed into the application</param>
        /// <returns>bool: true if arguments are successfully parse, otherwise false</returns>
        private static bool parseArguments(string[] args)
        {
            // Nothing to validate : args 
            // Command line parsing
            Arguments CommandLine = new Arguments(args);

            if (args.Length == 0)
            {
                Program.MessageBox_Error(MB_InvalidArgs_, sysErr_);
                return false;
            }

            if ((CommandLine["localuser"] != null))
            {
                localUser = CommandLine["localuser"];
            }
            else
            {
                MessageBox_Error(MB_LoginMft_, sysErr_);
                return false;
            }

            try
            {
                if (CommandLine["screenX"] != null)
                {
                    screenX = int.Parse(CommandLine["screenX"]);
                }

                if (CommandLine["screenY"] != null)
                {
                    screenY = int.Parse(CommandLine["screenY"]);
                }

                WriteEventLog("[INFO] Screen X: " + screenX + " Y: " + screenY, fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }
            catch (Exception ex)
            {
                WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }

            return true;
        }

        /// <summary>
        /// Determine if the operator is in a Remote MFT location different from his/her profile determined location
        /// </summary>
        /// <returns>bool: true if operator is using PA Manager in the same location as defined by his profile, otherwise false</returns>
        private static bool isRemoteMode()
        {
            bool remoteModeLocal = false;

            string currLocIndex = ConfigFile.GetINIDataString("DEFAULT_MFT_SETTING", "DEF_LOCATION_ID", "", 255, Path.Combine(configFolder_, "config.ini"));
            Program.WriteEventLog("[INFO] Current MFT Location Index: " + currLocIndex, fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

            if (currLocIndex == profileLocIndex.ToString())
            {
                remoteModeLocal = false;
                Program.WriteEventLog("[INFO] Operator is operating at the profile determined location!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }
            else
            {
                remoteModeLocal = true;
                Program.WriteEventLog("[INFO] Operator is operating in a remote location!", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }

            return remoteModeLocal;
        }

        /// <summary>
        /// Check if the current MFT is configured in the Database
        /// </summary>
        /// <param name="remoteLoc">true if user is logged in at a location different from that defined in his/her profile</param>
        /// <param name="profileLocId">locId of the logged in user's profile</param>
        /// <param name="mftHostName">hostname of mft</param>
        /// <returns>bool: true if the MFT is configured, otherwise false</returns>
        private static bool paSettingValid(bool remoteLoc, string profileLocId, string mftHostName)
        {
            // Nothing to validate : remoteLoc, profileLocId, mftHostName
            return true;
            ////
            //string localSql = "";
            //bool recExists = false;
            //OracleDataReader locOracleDr = null;
            //dbConnEnum dbConn = dbConnEnum.ErrNoConn;

            //if (remoteLoc)
            //{
            //	localSql = " SELECT COUNT(*) FROM PA_SETTING WHERE LOCATION_ID=" + profileLocId;
            //}
            //else
            //{
            //	localSql = " SELECT COUNT(*) FROM PA_SETTING WHERE LOCATION_ID=" + profileLocId + " AND HOST_NAME='" + mftHostName + "' ";
            //}

            //Program.dbLock.oracleQuery2(ref locOracleDr, localSql, ref dbConn);

            //if (locOracleDr.Read())
            //{
            //	if (!locOracleDr.IsDBNull(0))
            //	{
            //		if (locOracleDr.GetOracleDecimal(0).ToInt32() != 0)
            //		{
            //			recExists = true;
            //		}
            //	}
            //}

            //if (locOracleDr != null)
            //{
            //	locOracleDr.Close();
            //}

            //return recExists;

        }

        /// <summary>
        /// Display Normal Message Box 
        /// </summary>
        /// <param name="textStr">String to be displayed</param>
        /// <param name="captionStr">Caption of message box</param>
        /// <param name="callingForm">Reference to the calling form</param>
        public static void MessageBox_Normal(string textStr, string captionStr, Form callingForm)
        {
            // Nothing to validate : textStr, captionStr, callingForm
            STEMessageBox.Show(textStr, captionStr, defaultLanguage, MsgBoxButton.Ok, MessageBoxIcon.Information, callingForm);
        }

        /// <summary>
        /// Display Error Message Box 
        /// </summary>
        /// <param name="textStr">String to be displayed</param>
        /// <param name="captionStr">Caption of message box</param>
        public static void MessageBox_Error(string textStr, string captionStr)
        {
            // Nothing to validate : textStr, captionStr
            System.Diagnostics.StackTrace stackTrace = new System.Diagnostics.StackTrace();

            // Class name of calling function
            string callingClsName = stackTrace.GetFrame(1).GetMethod().DeclaringType.Name;

            // Method name of calling function
            string callingFunName = stackTrace.GetFrame(1).GetMethod().Name;

            //WriteEventLog("[ERROR] Error MessageBox was called by " + callingClsName + "-" + callingFunName, fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");

            STEMessageBox.Show(textStr, captionStr, defaultLanguage, MsgBoxButton.Ok, MessageBoxIcon.Error);
        }

        /// <summary>
        /// Display Warning Message Box 
        /// </summary>
        /// <param name="textStr">String to be displayed</param>
        /// <param name="captionStr">Caption of message box</param>
        /// <param name="ownerForm">Reference to the calling form</param>
        public static void MessageBox_Warn(string textStr, string captionStr, Form ownerForm)
        {
            // Nothing to validate : textStr, captionStr, ownerForm
            STEMessageBox.Show(textStr, captionStr, defaultLanguage, MsgBoxButton.Ok, MessageBoxIcon.Warning, ownerForm);
        }

        /// <summary>
        /// This method logs all events thru EventAPI and will also write to log file.
        /// </summary>
        /// <param name="paEvent">ID of event</param>
        /// <param name="paramList">Parameter List</param>
        /// <param name="paramListLocal">Localized Parameter List</param>
        public static void logEvent(EnumType.PaEvent paEvent, string[] paramList, string[] paramListLocal)
        {
            try
            {
                ////EventAPI.writeEvent((int)paEvent, paramList, paramListLocal);

                System.Diagnostics.StackTrace stackTrace = new System.Diagnostics.StackTrace();

                // Class name of calling function
                string callingClsName = stackTrace.GetFrame(1).GetMethod().DeclaringType.Name;

                // Method name of calling function
                string callingFunName = stackTrace.GetFrame(1).GetMethod().Name;

                String tmpStr = "";

                for (int i = 0; i < paramList.Length; i++)
                {
                    tmpStr += " [" + paramList.GetValue(i) + "]";
                }

                WriteEventLog("Event [" + (int)paEvent + "] -> Details: " + tmpStr, callingClsName + " - " + callingFunName);
            }
            catch (Exception ex)
            {
                WriteEventLog("[ERROR] [Exception] " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }

        }

        /// <summary>
        /// Function called to write to log file
        /// </summary>
        /// <param name="errorMsg">Error message</param>
        /// <param name="methodInfo">Calling method that the error occurs</param>
        public static void WriteEventLog(string errorMsg, string methodInfo)
        {
            lock (fileLock_)
            {
                LoggerManager.WriteAppLog(methodInfo + ": " + errorMsg, moduleName);
            }
        }

        /// <summary>
        /// Function called to write to log file
        /// </summary>
        /// <param name="errorMsg">Error message</param>
        /// <param name="methodInfo">Calling method that the error occurs</param>
        public static void WriteDebugEventLog(string errorMsg, string methodInfo)
        {
            if (debugMode == false)
            {
                return;
            }

            lock (fileLock_)
            {
                LoggerManager.WriteAppLog(methodInfo + " : " + errorMsg, moduleName);
            }
        }

        /// <summary>
        /// Log byte[] array containing packet data to file.
        /// </summary>
        /// <param name="msgIn">Customized message to be printed</param>
        /// <param name="receivedPacket">byte array to be logged</param>
        public static void writePacketLog(string msgIn, byte[] receivedPacket)
        {
            // Nothing to validate : msgIn, receivedPacket
            if (debugMode == true)
            {
                int tableNo = 0;
                string streamOfFile = "";

                if (receivedPacket == null || receivedPacket.Length == 0)
                {
                    Program.WriteEventLog("[ERROR] [" + msgIn + "] IS EMPTY.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
                    return;
                }

                tableNo = Util.Byte_To_Int(receivedPacket[1], receivedPacket[2]);

                for (int k = 0; k < receivedPacket.Length; k++)
                {
                    //streamOfFile = streamOfFile + receivedPacket[k].ToString() + "(" + k + ") ";
                    streamOfFile = streamOfFile + receivedPacket[k].ToString() + ",";
                }
                Program.WriteEventLog("" + msgIn + " Data: [" + streamOfFile + "]", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }
        }

        /// <summary>
        /// The cleanUp method is for freeing up resource that was held by the class during runtime.
        /// </summary>
        private static void cleanUp()
        {
            try
            {
                WriteDebugEventLog("[DEBUG] Releasing resources held by " + appName_, fileName_ + " -> " + MethodInfo.GetCurrentMethod().Name + "()");
                EventAPI.cleanup();
                WriteDebugEventLog("[DEBUG] Resources released successfully.", fileName_ + " -> " + MethodInfo.GetCurrentMethod().Name + "()");

                WriteEventLog("[INFO] PA Manager [Main] successfully closed.", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
            }
            catch (Exception)
            {

            }
        }

        /// <summary>
        /// Query Oracle Database - Get a single integer value
        /// </summary>
        /// <param name="localSql">SQL query to be executed</param>
        /// <returns>Integer value</returns>
        /// <exception cref="NoOracleDBException"></exception>
        /// <exception cref="Exception"></exception>
        public static int mySqlQuery_SingleInt(string localSql)
        {
            // Nothing to validate : localSql
            dbConnEnum dbConn = dbConnEnum.ErrNoConn;
            int intVal = 0;
            try
            {
                var queries = new List<SqlQuery> { new SqlQuery { CommandText = localSql } };
                DataSet ds = Program.dbLock.FetchData(queries, ref dbConn);

                if (ds.Tables != null || ds.Tables.Count > 0)
                {
                    foreach (DataRow retrievedRow in ds.Tables[0].Rows)
                    {
                        intVal = int.Parse(retrievedRow[0].ToString());
                    }
                }
            }
            catch (NoOracleDBException ex)
            {
                Program.WriteEventLog("[ERROR] [NoOracleDBException] " + Program.MB_NoDBConn + " Details: " + ex.ToString(), fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
                throw;
            }
            catch (Exception)
            {
                Program.WriteEventLog("[ERROR] Possible SQL Error: [" + localSql + "]", fileName_ + "." + MethodInfo.GetCurrentMethod().Name + "()");
                throw;
            }

            return intVal;

        }

        /// <summary>
        /// Check if the interval period from the base time is reached
        /// </summary>
        /// <param name="baseTime">Base time used for comparision</param>
        /// <param name="intervalPeriod">Interval from base time to be used</param>
        /// <param name="timeToSleep">Time to sleep if interval is not reached</param>
        /// <returns>bool: true if interval is reached, otherwise false</returns>
        public static bool reachInterval(DateTime baseTime, int intervalPeriod, out double timeToSleep)
        {
            // Nothing to validate : baseTime, intervalPeriod, timeToSleep
            DateTime currentTime = DateTime.Now;
            long elapsedTicks = currentTime.Ticks - baseTime.Ticks;
            TimeSpan elapsedSpan = new TimeSpan(elapsedTicks);
            timeToSleep = 0;

            System.Diagnostics.StackTrace stackTrace = new System.Diagnostics.StackTrace();

            // Class name of calling function
            string callingClsName = stackTrace.GetFrame(1).GetMethod().DeclaringType.Name;

            // Method name of calling function
            string callingFunName = stackTrace.GetFrame(1).GetMethod().Name;

            if (elapsedSpan.TotalMilliseconds > intervalPeriod)
            {
                timeToSleep = 0;
                WriteDebugEventLog("[DEBUG] [reachInterval] Interval [" + intervalPeriod + "ms] reached. Elapsed time: " + elapsedSpan.TotalMilliseconds + "ms.", callingClsName + "." + callingFunName);
                return true;
            }
            else
            {
                timeToSleep = ((double)intervalPeriod - elapsedSpan.TotalMilliseconds) + 1;
                WriteDebugEventLog("[ERROR] [reachInterval] Interval [" + intervalPeriod + "ms] NOT reached. Elapsed time: " + elapsedSpan.TotalMilliseconds + "ms.", callingClsName + "." + callingFunName);
                return false;
            }
        }

        /// <summary>
        /// Return the meaning of the returned packet from PAS
        /// </summary>
        /// <param name="errorByteCode">Packet error code</param>
        /// <param name="languageCode">Language to return the packet error message in</param>
        /// <returns>Error Message</returns>
        public static string returnPacketError(byte errorByteCode, string languageCode)
        {
            // Nothing to validate :  errorByteCode, languageCode
            int errorCode = Convert.ToInt32(errorByteCode);
            string errorMsg = "";

            switch (errorCode)
            {
                case 0:
                    errorMsg = Localization.GetEventMessage("EVT_PAC_NO_ERROR", languageCode);
                    break;
                case 1:
                    errorMsg = Localization.GetEventMessage("EVT_PAC_GENERIC_ERROR", languageCode);
                    break;
                case 2:
                    errorMsg = Localization.GetEventMessage("EVT_PAC_BZ_MSG", languageCode);
                    break;
                case 3:
                    errorMsg = Localization.GetEventMessage("EVT_PAC_BZ_SOURCE", languageCode);
                    break;
                case 4:
                    errorMsg = Localization.GetEventMessage("EVT_PAC_BZ_RECORD", languageCode);
                    break;
                case 5:
                    errorMsg = Localization.GetEventMessage("EVT_PAC_BZ_BC", languageCode);
                    break;
                case 6:
                    errorMsg = Localization.GetEventMessage("EVT_PAC_INVALID_ANNOUNCE", languageCode);
                    break;
                case 7:
                    errorMsg = Localization.GetEventMessage("EVT_PAC_NO_FLAG", languageCode);
                    break;
                default:
                    errorMsg = "Unknown Error (Error code 8) " + errorByteCode;
                    break;
            }

            return errorMsg;
        }

        /// <summary>
        /// Returns the localized exception string
        /// </summary>
        /// <param name="exceptionCode">Exception code to be translated</param>
        /// <returns>Localized exception string</returns>
        public static string returnExceptionString(EnumType.ExceptionCode exceptionCode)
        {
            // Nothing to validate :  exceptionCode
            string errorMsg = "";

            switch ((int)exceptionCode)
            {
                case 0:
                    errorMsg = Localization.GetEventMessage("EVT_NO_DB_CONN", Program.defaultLanguage);
                    break;
                case 1:
                    errorMsg = Localization.GetEventMessage("EVT_PAS_ERROR", Program.defaultLanguage);
                    break;
                case 2:
                    errorMsg = Localization.GetEventMessage("EVT_GENERAL_EX", Program.defaultLanguage);
                    break;
                case 3:
                    errorMsg = Localization.GetEventMessage("EVT_PAS_REPLY_TIMEOUT", Program.defaultLanguage);
                    break;
                case 4:
                    errorMsg = Localization.GetEventMessage("EVT_NO_PAS_LINK", Program.defaultLanguage);
                    break;
                default:
                    errorMsg = "[ERROR] exceptionCode:" + exceptionCode.ToString();
                    break;
            }

            return errorMsg;
        }

    }

}