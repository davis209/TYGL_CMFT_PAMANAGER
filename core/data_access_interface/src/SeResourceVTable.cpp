/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/SeResourceVTable.cpp $
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

#include "SeResourceVTable.h"
#include "DatabaseFactory.h"
#include "core/utilities/src/StdUtil.h"
#include <mutex>

using namespace std::string_literals;

namespace TA_Base_Core
{
    SeResourceVTable::Table& SeResourceVTable::select()
    {
        static SeResourceVTable::Table s_table;
        static std::once_flag s_once;
        std::call_once(s_once, [&]
        {
            auto* db = DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_SCd, Read);
            auto data = db->executeQueryAll("SELECT * FROM se_resource_v");
            s_table.reserve(data->getNumRows());

            for (auto row : *data)
            {
                s_table.emplace_back(Row
                {
                    row.getUnsignedLongData("PKEY"),
                    row.getStringData("NAME"),
                    row.getUnsignedLongData("SUBSYSTEMKEY")
                });
            }
        });
        return s_table;
    }

    SeResourceVTable::Table SeResourceVTable::select(Where where)
    {
        return stdutil::filter_copy(select(), where);
    }

    boost::optional<SeResourceVTable::Row&> SeResourceVTable::selectFirst(Where where)
    {
        return stdutil::get_if_optional(select(), where);
    }
}
