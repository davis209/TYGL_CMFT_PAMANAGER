/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/C830G_TIP/TA_BASE/transactive/core/data_access_interface/src/OperatorTable.cpp $
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

#include "OperatorTable.h"
#include "DatabaseFactory.h"
#include "SQLCode.h"
#include "core/utilities/src/StdUtil.h"
#include "core/exceptions/src/DataException.h"
#include <mutex>

using namespace std::string_literals;

namespace TA_Base_Core
{
    OperatorTable::Table& OperatorTable::select()
    {
        static OperatorTable::Table s_table;
        static std::once_flag s_once;
        std::call_once(s_once, [&]
        {
            auto* db = DatabaseFactory::getInstance().getDatabase({Sessions_Ad, Sessions_Cached}, Read);
            auto data = db->executeQueryAll("SELECT * FROM operator");
            s_table.reserve(data->getNumRows());

            for (auto row : *data)
            {
                s_table.emplace_back(Row
                {
                    row.getUnsignedLongData("PKEY"),
                    row.getStringData("NAME"),
                    row.getStringData("PASSWD"),
                    row.getUnsignedLongData("ACCESSLEVEL"),
                    row.getStringData("DESCRIPTION"),
                    row.getBooleanData("IS_SYSTEM"),
                    row.getBooleanData("DELETED"),
                    row.getStringData("CREATED_BY"),
                    row.getDateData("DATE_CREATED"),
                    row.getStringData("MODIFIED_BY"),
                    row.getDateData("DATE_MODIFIED")
                });
            }
        });

        return s_table;
    }

    OperatorTable::Table OperatorTable::select(Where where)
    {
        return stdutil::filter_copy(select(), where);
    }

    boost::optional<OperatorTable::Row&> OperatorTable::selectFirst(Where where)
    {
        return stdutil::get_if_optional(select(), where);
    }
}
