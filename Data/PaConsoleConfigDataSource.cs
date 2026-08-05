using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data;
using System.Data.SqlClient;
using System.Collections.ObjectModel;
using ste.common.util;
using ste.common.exceptions;

namespace ste.pa.pamanager
{
    class PaConsoleConfigDataSource
    {
        public static readonly string COLUMN_NAME_PREDEFINED_SUPPORTED_FORMAT = "SUPPORTED_AUDIO_FORMAT";
        public static readonly string COLUMN_NAME_MAX_NUM_MSG = "MAX_MSG_PER_BROADCAST";
        public static readonly string COLUMN_NAME_CONSOLE_NAME = "CONSOLE_HOST_NAME";
        public static readonly string COLUMN_NAME_PRIMARY_ROOT_DIR = "PREDEF_PRIMARY_ROOT_DIR";
        public static readonly string COLUMN_NAME_SECONDARY_ROOT_DIR = "PREDEF_SECONDARY_ROOT_DIR";

        private static readonly string appName_ = typeof(PaConsoleConfigDataSource).Name;
        //private static readonly string query = "SELECT " + COLUMN_NAME_PREDEFINED_SUPPORTED_FORMAT + ", "
        //                                                 + COLUMN_NAME_MAX_NUM_MSG + ", "
        //                                                 + COLUMN_NAME_PRIMARY_ROOT_DIR + ", "
        //                                                 + COLUMN_NAME_SECONDARY_ROOT_DIR
        //                                                 + " from  PA_CONSOLE_CONFIG where " + COLUMN_NAME_CONSOLE_NAME + " = @consoleName";
        private static readonly string queryTemplate = "SELECT "
                                                        + COLUMN_NAME_PREDEFINED_SUPPORTED_FORMAT + ", "
                                                        + COLUMN_NAME_MAX_NUM_MSG + ", "
                                                        + COLUMN_NAME_PRIMARY_ROOT_DIR + ", "
                                                        + COLUMN_NAME_SECONDARY_ROOT_DIR
                                                        + " from PA_CONSOLE_CONFIG where " + COLUMN_NAME_CONSOLE_NAME + " = '{0}'";

        public PaConsoleConfigDataSource()
        {
        }

        public bool getConsoleParam(out string predefPriDir, out string predefSecDir, out string supportedFormat, out int maxNumOfMsgPerBroadcast)
        {
            predefPriDir = "";
            predefSecDir = "";
            supportedFormat = "";
            maxNumOfMsgPerBroadcast = 5;

            dbConnEnum dbConn = dbConnEnum.ErrNoConn;
            string query = string.Format(queryTemplate, Environment.MachineName);
            var queries = new List<SqlQuery> { new SqlQuery { CommandText = query } };
            try
            {
                //Collection<SqlParameter> sqlParams = new Collection<SqlParameter>();
                //sqlParams.Add(new SqlParameter("@consoleName",  Environment.MachineName));

                //DataSet ds = Program.msSqlDBHelper.GetSQLQueryPreparedStatement(query, appName_, sqlParams);
                DataSet ds = Program.dbLock.FetchData(queries, ref dbConn);

                if (ds.Tables != null && ds.Tables.Count > 0)
                {
                    DataTable dt = ds.Tables[0];
                    foreach (DataRow row in dt.Rows)
                    {
                        predefPriDir = (string)row[COLUMN_NAME_PRIMARY_ROOT_DIR];
                        predefSecDir = (string)row[COLUMN_NAME_SECONDARY_ROOT_DIR];
                        supportedFormat = (string)row[COLUMN_NAME_PREDEFINED_SUPPORTED_FORMAT];
                        maxNumOfMsgPerBroadcast = (int)row[COLUMN_NAME_MAX_NUM_MSG];

                        if (!predefPriDir.EndsWith("\\"))
                            predefPriDir += "\\";

                        if (!predefSecDir.EndsWith("\\"))
                            predefSecDir += "\\";

                        return true;
                    }
                }
                else
                    Utility.logError("Nothing found from PA_CONSOLE_CONFIG. Please check DB!!", appName_);
            }
            catch (NoOracleDBException ex)
            {
                Utility.logError(ex.StackTrace, appName_);
                throw;
            }
            catch (Exception ex)
            {
                Utility.logError(ex.StackTrace, appName_);
                throw;
            }

            return false ;
        }
    }
}
