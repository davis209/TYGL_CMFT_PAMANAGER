using System;
using System.Collections.Generic;
using System.Data;
using System.Security.Cryptography;
using System.Text;
using ste.common.exceptions;
using ste.common.util;

namespace ste.pa.pamanager
{
    internal static class AirRaidPasswordService
    {
        private const string PasswordTable = "PA_AIR_RAID_PASSWORD";

        internal static bool TryVerify(string password, out string failureReason)
        {
            failureReason = "\u5BC6\u78BC\u4E0D\u6B63\u78BA\u3002";
            if (String.IsNullOrEmpty(password))
            {
                failureReason = "\u8ACB\u8F38\u5165\u5BC6\u78BC\u3002";
                return false;
            }

            try
            {
                string storedHash = GetStoredHash();
                if (String.IsNullOrEmpty(storedHash))
                {
                    failureReason = "\u672A\u8A2D\u5B9A\u7A7A\u8972\u8B66\u5831\u5BC6\u78BC\uFF0C\u7121\u6CD5\u767C\u9001\u6307\u4EE4\u3002";
                    return false;
                }

                return FixedTimeEquals(storedHash, GetMd5(password));
            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] Air-raid password verification failed: " + ex.Message,
                    "AirRaidPasswordService.TryVerify()");
                failureReason = "\u5BC6\u78BC\u9A57\u8B49\u670D\u52D9\u7121\u6CD5\u4F7F\u7528\uFF0C\u70BA\u5B89\u5168\u8D77\u898B\u672A\u767C\u9001\u7A7A\u8972\u8B66\u5831\u3002";
                return false;
            }
        }

        internal static bool TryChangePassword(string currentPassword, string newPassword, string confirmPassword, out string failureReason)
        {
            failureReason = String.Empty;
            if (String.IsNullOrEmpty(newPassword))
            {
                failureReason = "\u65B0\u5BC6\u78BC\u4E0D\u53EF\u70BA\u7A7A\u3002";
                return false;
            }
            if (newPassword != confirmPassword)
            {
                failureReason = "\u5169\u6B21\u8F38\u5165\u7684\u65B0\u5BC6\u78BC\u4E0D\u4E00\u81F4\u3002";
                return false;
            }
            if (!TryVerify(currentPassword, out failureReason))
            {
                return false;
            }

            try
            {
                dbConnEnum dbConn = dbConnEnum.ErrNoConn;
                string sql = "UPDATE " + PasswordTable + " SET PASSWORD_MD5='" + GetMd5(newPassword) + "', UPDATED_AT=NOW(3) WHERE PASSWORD_ID=1";
                var queries = new List<SqlQuery> { new SqlQuery { CommandText = sql } };
                if (Program.dbLock.ExcuteNoneResultQuery(queries, ref dbConn) < 1)
                {
                    failureReason = "\u5BC6\u78BC\u4FEE\u6539\u5931\u6557\u3002";
                    return false;
                }

                Program.WriteEventLog("[AUDIT] Air-raid password changed. Operator=[" + Program.localUser + "]",
                    "AirRaidPasswordService.TryChangePassword()");
                return true;
            }
            catch (Exception ex)
            {
                Program.WriteEventLog("[ERROR] Air-raid password change failed: " + ex.Message,
                    "AirRaidPasswordService.TryChangePassword()");
                failureReason = "\u5BC6\u78BC\u4FEE\u6539\u5931\u6557\u3002";
                return false;
            }
        }

        private static string GetStoredHash()
        {
            dbConnEnum dbConn = dbConnEnum.ErrNoConn;
            var queries = new List<SqlQuery> { new SqlQuery { CommandText = "SELECT PASSWORD_MD5 FROM " + PasswordTable + " WHERE PASSWORD_ID=1" } };
            DataSet dataSet = Program.dbLock.FetchData(queries, ref dbConn);
            if (dataSet == null || dataSet.Tables.Count == 0 || dataSet.Tables[0].Rows.Count == 0)
            {
                return String.Empty;
            }
            return Convert.ToString(dataSet.Tables[0].Rows[0][0]).Trim().ToUpperInvariant();
        }

        private static string GetMd5(string password)
        {
            using (MD5 md5 = MD5.Create())
            {
                byte[] bytes = md5.ComputeHash(Encoding.UTF8.GetBytes(password));
                return BitConverter.ToString(bytes).Replace("-", String.Empty);
            }
        }

        private static bool FixedTimeEquals(string left, string right)
        {
            if (left == null || right == null || left.Length != right.Length) return false;
            int difference = 0;
            for (int i = 0; i < left.Length; i++) difference |= left[i] ^ right[i];
            return difference == 0;
        }
    }
}
