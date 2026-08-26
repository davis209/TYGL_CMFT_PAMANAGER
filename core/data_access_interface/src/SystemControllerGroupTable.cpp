/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/SystemControllerGroupTable.cpp $
 * @author:  Justin Ebedes
 * @version: $Revision: #4 $
 *
 * Last modification: $DateTime: 2024/09/27 18:12:18 $
 * Last modified by:  $Author: josef $
 *
 * SystemControllerAccessFactory is a singleton that is
 * used to retrieve ISystemControllerData objects from
 * the database.
 *
 */

#include "SystemControllerGroupTable.h"
#include "DatabaseFactory.h"
#include "SQLCode.h"
#include "core/utilities/src/StdUtil.h"
#include "core/exceptions/src/DataException.h"
#include <mutex>

using namespace std::string_literals;

namespace TA_Base_Core
{
    SystemControllerGroupTable::Table& SystemControllerGroupTable::select()
    {
        static SystemControllerGroupTable::Table s_table;
        static std::once_flag s_once;
        std::call_once(s_once, [&]
        {
            auto* db = DatabaseFactory::getInstance().getDatabase(/*SysController_SCd*/SysController_Cd, Read);
            auto data = db->executeQueryAll("SELECT * FROM sn_system_controller_group");
            s_table.reserve(data->getNumRows());

            for (auto row : *data)
            {
                s_table.emplace_back(Row
                {
                    row.getUnsignedLongData("SNSCGR_ID"),
                    row.getStringData("NAME"),
                    row.getStringData("DESCRIPTION")
                });
            }
        });
        return s_table;
    }

    SystemControllerGroupTable::Table SystemControllerGroupTable::select(Where where)
    {
        return stdutil::filter_copy(select(), where);
    }

    boost::optional<SystemControllerGroupTable::Row&> SystemControllerGroupTable::selectFirst(Where where)
    {
        return stdutil::get_if_optional(select(), where);
    }
}
