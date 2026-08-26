/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/C830G_TIP/TA_BASE/transactive/core/data_access_interface/src/OperatorPasswordTable.cpp $
 * @author:  Justin Ebedes
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2021/07/28 19:31:27 $
 * Last modified by:  $Author: limin.zhu $
 *
 * SystemControllerAccessFactory is a singleton that is
 * used to retrieve ISystemControllerData objects from
 * the database.
 *
 */

#include "OperatorPasswordTable.h"
#include "DatabaseFactory.h"
#include "SQLCode.h"
#include "core/utilities/src/StdUtil.h"
#include "core/exceptions/src/DataException.h"
#include <mutex>

using namespace std::string_literals;

namespace TA_Base_Core
{
    OperatorPasswordTable::Table& OperatorPasswordTable::select()
    {
        static OperatorPasswordTable::Table s_table;
        static std::once_flag s_once;
        std::call_once(s_once, [&]
        {
            auto* db = DatabaseFactory::getInstance().getDatabase({Sessions_Ad, Sessions_Cached}, Read);
            auto data = db->executeQueryAll("SELECT * FROM operator_password");
            s_table.reserve(data->getNumRows());

            for (auto row : *data)
            {
                s_table.emplace_back(Row
                {
                    row.getUnsignedLongData("PKEY"),
                    row.getDateData("LAST_LOGIN_DATE"),
                    row.getDateData("PASSWD_MODIFIED_DATE"),
                    row.getDateData("LATEST_LOCKED_TIME"),
                    row.getUnsignedLongData("IS_PASSWORD_LOCKOUT"),
                    row.getUnsignedLongData("CURRENT_PASSWD_INDICATOR"),
                    row.getStringData("PASSWORD_0"),
                    row.getStringData("PASSWORD_1"),
                    row.getStringData("PASSWORD_2"),
                });
            }
        });

        return s_table;
    }

    OperatorPasswordTable::Table OperatorPasswordTable::select(Where where)
    {
        return stdutil::filter_copy(select(), where);
    }

    boost::optional<OperatorPasswordTable::Row&> OperatorPasswordTable::selectFirst(Where where)
    {
        return stdutil::get_if_optional(select(), where);
    }
}
