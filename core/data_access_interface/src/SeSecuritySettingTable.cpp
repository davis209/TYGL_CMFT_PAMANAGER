/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/C830G_TIP/TA_BASE/transactive/core/data_access_interface/src/SeSecuritySettingTable.cpp $
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

#include "SeSecuritySettingTable.h"
#include "DatabaseFactory.h"
#include "SQLCode.h"
#include "core/utilities/src/StdUtil.h"
#include "core/exceptions/src/DataException.h"
#include <mutex>

using namespace std::string_literals;

namespace TA_Base_Core
{
    SeSecuritySettingTable::Table& SeSecuritySettingTable::select()
    {
        static SeSecuritySettingTable::Table s_table;
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
                    row.getStringData("setting_name"),
                    row.getUnsignedLongData("setting_value"),
                    row.getStringData("description")
                });
            }
        });

        return s_table;
    }

    SeSecuritySettingTable::Table SeSecuritySettingTable::select(Where where)
    {
        return stdutil::filter_copy(select(), where);
    }

    boost::optional<SeSecuritySettingTable::Row&> SeSecuritySettingTable::selectFirst(Where where)
    {
        return stdutil::get_if_optional(select(), where);
    }
}
