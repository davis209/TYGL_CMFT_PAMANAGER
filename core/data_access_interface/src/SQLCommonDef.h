//////////////////////////////////////////////////////////////////////
///     @file       SQLCommonDef.h
///     @author     zhilin,ouyang
///     @date       2011-09-20 16:44:41
///
///     @brief      to define some common use macro used in the SQL
///                 encapsulation.
//////////////////////////////////////////////////////////////////////
#ifndef SQLCOMMONDEF_H
#define SQLCOMMONDEF_H

#define  NAMESPACE_BEGIN(name) namespace name {
#define  NAMESPACE_END(name)   };

NAMESPACE_BEGIN(TA_Base_Core)

// the database server type support in the system
enum DbServerType
{
    enumBeginDbType = 0,    // the begin of the supported database type.
    enumOracleDb,           // Oracle database  (Support)
    enumMysqlDb,            // Mysql database   (Support)
    enumSqliteDb,           // SqLite database   (Support)

    enumEndDbType,          // the end tag of supported database type, all supported database should list above this line.
    enumMSSvrDb,            // Microsoft SQL Server  (Not support currently, just list here)
    enumReserve             // For other database system should add above
};

enum ESQLOperatorKey
{
    enumOperKey_EQUAL              = 0,
    enumOperKey_NOTEQU,
    enumOperKey_ABOVE,
    enumOperKey_BELOW,
    enumOperKey_GETHAN,
    enumOperKey_LETHAN,

    /** adding new key?
      * 1. insert new key before enumOperKey_End
      * 2. insert new element in SQLParamsHelper.cpp file
    */
    enumOperKey_End
};

enum ESQLParaValType
{
    enumValType_STR               = 0,
    enumValType_NONSTR
};

enum EDynamicSQLKey
{
    enumDateFormat_1                = 0,
    enumDateFormat_2,
    enumDateFormat_3,
    enumDateFormat_4,
    enumDateFormat_5,
    enumDateFormat_6,
    enumDateFormat_7,
    enumDateFormat_8,
    enumDateFormat_9,   // ISO8601 Format date

    enumFuncFormat_1,
    enumFuncFormat_2,
    enumFuncFormat_3,
    enumFuncFormat_4,
    enumFuncFormat_5,
    enumFuncFormat_6,
    enumFuncFormat_7,
    enumFuncFormat_8,
    enumFuncFormat_9,
    enumFuncFormat_10,
    enumFuncFormat_11,
    enumFuncFormat_12,

    enumSyntax_Format_1,
    enumSyntax_Format_2,
    enumSyntax_Format_3,    // date_modified
    enumSyntax_Format_4,
    enumSyntax_Format_5,
    enumSyntax_Format_6,    // date_created

    /** adding new key?
      * 1. insert new key before DynamicSQLKeyLast
      * 2. insert new element in SQLParamsHelper.cpp file
    */

    DynamicSQLKeyLast    //never pass this one as a parameter!
};

#define  MAXLOGMESGSIZE           2000  // the max length of message for log module to log
#define  MAX_SQLSTRING_LEN        2047  // max length of one SQL statement 2048 (2047 + 1 NULL)
#define  defMINPARAMSIZE          1     // the min require parameter count in SQL contruction
#define  defMINSTATICAQPARAMSIZE  2     // the min require parameters count in SQL construction for static AQ Statement.
#define  defMINDYNAMICAQPARAMSIZE 3     // the min require parameters count in SQL construction for dynamic AQ statement.
#define  defAQSQL_Demit           0x2c  // ','
#define  defAQSQL_EndTag          0x29  // ')'
#define  defAQSQL_SingleQuote     0x27  // '''
#define  defSupportDBCount        4     // the number of database which supported in the system. Added: Sqlite

//typedef   ta_uint32    u_long;

NAMESPACE_END(TA_Base_Core)

#include "core/types/src/ta_types.h"

#endif  // SQLCOMMONDEF_H
