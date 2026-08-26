/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/SeRegSubVTable.cpp $
 * @author:  Justin Ebedes
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * SystemControllerAccessFactory is a singleton that is
 * used to retrieve ISystemControllerData objects from
 * the database.
 *
 */

#include "SeRegSubVTable.h"
#include "DatabaseFactory.h"
#include "core/utilities/src/StdUtil.h"
#include <mutex>

using namespace TA_Base_Core;
using namespace std::string_literals;

namespace TA_Base_Bus
{
    SeRegSubVTable::Table& SeRegSubVTable::select()
    {
        static SeRegSubVTable::Table s_table;
        static std::once_flag s_once;
        std::call_once(s_once, [&]
        {
            auto* db = DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_SCd/*SysController_Cd*/, Read);
            auto data = db->executeQueryAll("SELECT * FROM se_reg_sub_v");
            s_table.reserve(data->getNumRows());

            for (auto row : *data)
            {
                s_table.emplace_back(Row
                {
                    row.getUnsignedLongData("SEREGI_ID"),
                    row.getUnsignedLongData("PHYSICAL_SUBSYSTEM_KEY")
                });
            }
        });
        return s_table;
    }

    SeRegSubVTable::Table SeRegSubVTable::select(Where where)
    {
        return stdutil::filter_copy(select(), where);
    }

    boost::optional<SeRegSubVTable::Row&> SeRegSubVTable::selectFirst(Where where)
    {
        return stdutil::get_if_optional(select(), where);
    }
}
