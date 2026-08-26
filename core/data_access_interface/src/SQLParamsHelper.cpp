//////////////////////////////////////////////////////////////////////
///     @file       SQLParamsHelper.cpp
///     @author     zhilin,ouyang
///     @date       2012-12-11 14:44:41
///
///     @brief      the implementation of class CSQLParamsHelper to help
///                 class SQLTypeAdapter to accept dynamic  parameters
///                 between different database such as datetime format
///                 different between Oracle & MySQL.
//////////////////////////////////////////////////////////////////////
#include "SQLParamsHelper.h"

NAMESPACE_BEGIN(TA_Base_Core)

struct stOperator
{
    int           nOperKey;
    const char*   pczOperator;
};

struct stDynamicParaData
{
    int              nKey;
    const char*      pczOracle;
    const char*      pczMysql;
    const char*      pczSqlite;
};

static const stOperator s_stOperator[] =
{
    //operator Key                   //the operator
    {enumOperKey_EQUAL,               "="  },
    {enumOperKey_NOTEQU,              "!=" },
    {enumOperKey_ABOVE,               ">"  },
    {enumOperKey_BELOW,               "<"  },
    {enumOperKey_GETHAN,              ">=" },
    {enumOperKey_LETHAN,              "<=" }
};

static const stDynamicParaData s_stParaData[] =
{
    //EDynamicSQLKey                 //the format of Oracle           //the format of MySQL     // The format for SQLite
    {enumDateFormat_1,              "dd/mm/yyyy hh24:mi:ss.ff3",      "%d/%m/%Y %H:%i:%S.%f",   "%Y/%m/%d %H:%M:%f"},
    {enumDateFormat_2,              "dd/mm/yyyy hh24:mi:ss",          "%d/%m/%Y %H:%i:%S",      "%Y/%m/%d %H:%M:%S"},
    {enumDateFormat_3,              "DDMMYYYYHH24MISS",               "%d%m%Y%H%i%S",           "%d%m%Y%H%M%S"  },
    {enumDateFormat_4,              "YYYYMMDDHH24MISS",               "%Y%m%d%H%i%S",           "%Y%m%d%H%M%S" },
    {enumDateFormat_5,              "Mon DD HH24:MI:SS YYYY",         "%b %d %H:%i:%S %Y",      "%w %d %H:%M:%S %Y" },  // Note SQLite does not have format for day of week in ddd format.
    {enumDateFormat_6,              "YYYYMMDD",                       "%Y%m%d",                 "%Y%m%d" },
    {enumDateFormat_7,              "hh:mi:ss dd/mm/yyyy",            "%H:%i:%S %d/%m/%Y",      "%H:%M:%S %d/%m/%Y" },
    {enumDateFormat_8,              "DD/MM/YYYY",                     "%d/%m/%Y",               "%d/%m/%Y" },
    {enumDateFormat_9,              "YYYY-MM-DD HH24:MI:SS",          "%Y-%m-%d %H:%i:%S",      "%Y-%m-%d %H:%M:%S" },  // Added ISO-8601 Datetime format

    {enumFuncFormat_1,              "TO_TIMESTAMP",                   "STR_TO_DATE",            "strftime" },   // SQLite does not have Str_to_date equivalent
    {enumFuncFormat_2,              "TO_DATE",                        "STR_TO_DATE",            "strftime" },
    {enumFuncFormat_3,              "SYSDATE",                        "SYSDATE()",              "date('now')" },    // SQLite in UTC
    {enumFuncFormat_4,              "INTERVAL '1' DAY",               "INTERVAL 1 DAY",         "1 days" },
    {enumFuncFormat_5,              "INTERVAL '1' Second",            "INTERVAL 1 Second",      "1 seconds" },
    {enumFuncFormat_6,              "-",                              "DATE_SUB",               "-" },
    {enumFuncFormat_7,              "+",                              "DATE_ADD",               "+" },
    {enumFuncFormat_8,              "TO_TIMESTAMP",                   "DATE_FORMAT",            "strftime" },
    {enumFuncFormat_9,              "SYSDATE - INTERVAL '1' DAY",     "DATE_SUB( SYSDATE() , INTERVAL 1 DAY)",      "date('now', '-1 days')" },
    {enumFuncFormat_10,             "TO_CHAR",                        "DATE_FORMAT",            "strftime" },
    {enumFuncFormat_11,             "NVL",                            "IF",                     "" },   // SQLite does not support IF statement yet
    {enumFuncFormat_12,             "TO_CHAR(NVL",                    "DATE_FORMAT(IF",         "strftime(" },

    {enumSyntax_Format_1,           " and rownum < ",                 " LIMIT ",                " LIMIT " },
    {enumSyntax_Format_4,           " rownum < ",                     " LIMIT ",                " LIMIT " },
    {enumSyntax_Format_2,           " into se_profile_location (SEPROF_ID,LOCATIONKEY) values (", "(",      "(" },

    {   enumSyntax_Format_3,           "TO_CHAR(nvl(DATE_MODIFIED,TO_DATE('12:00:00 01/01/1990','hh:mi:ss dd/mm/yyyy')),'YYYYMMDDHH24MISS') as DATE_MODIFIED",
        "DATE_FORMAT(IF(DATE_MODIFIED IS NULL,STR_TO_DATE('12:00:00 01/01/1990','%H:%i:%S %d/%m/%Y'),DATE_MODIFIED),'%Y%m%d%H%i%S') as DATE_MODIFIED",
        "ifnull( strftime('%Y%m%d%H%M%S', DATE_MODIFIED),'19900101120000') as DATE_MODIFIED"
    },

    {   enumSyntax_Format_6, "TO_CHAR(nvl(DATE_CREATED,TO_DATE('12:00:00 01/01/1990','hh:mi:ss dd/mm/yyyy')),'YYYYMMDDHH24MISS') as DATE_CREATED",
        "DATE_FORMAT(IF(DATE_CREATED IS NULL,STR_TO_DATE('12:00:00 01/01/1990','%H:%i:%S %d/%m/%Y'),DATE_CREATED),'%Y%m%d%H%i%S') as DATE_CREATED",
        "ifnull( strftime('%Y%m%d%H%M%S', DATE_CREATED),'19900101120000') as DATE_CREATED"
    },

    {enumSyntax_Format_5,           "CONDITION",                      "`CONDITION`",            "`CONDITION`" } // To use a reserved keyword 'condition' as table name/column?
};

void  CSQLParamsHelper::getOperator(int nKey, std::string& strVal)
{
    const stOperator* s_pOperator = s_stOperator;

    while (s_pOperator->nOperKey != enumOperKey_End)
    {
        if (s_pOperator->nOperKey == nKey)
        {
            strVal = s_pOperator->pczOperator;

            break;
        }

        s_pOperator++;
    }
}

void  CSQLParamsHelper::getDynamicSQLPara(int nKey, DynamicData& rData)
{
    const stDynamicParaData* s_pParaData = s_stParaData;

    while (s_pParaData->nKey != DynamicSQLKeyLast)
    {
        if (s_pParaData->nKey == nKey)
        {
            rData.strOracl = s_pParaData->pczOracle;
            rData.strMysql = s_pParaData->pczMysql;
            rData.strSqlite = s_pParaData->pczSqlite;

            break;
        }

        s_pParaData++;
    }
}

NAMESPACE_END(TA_Base_Core)
