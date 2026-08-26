/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/SystemControllerTable.cpp $
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

#include "SystemControllerTable.h"
#include "SystemControllerGroupTable.h"
#include "ManagedProcessTable.h"
#include "DatabaseFactory.h"
#include "SQLCode.h"
#include "core/utilities/src/StdUtil.h"
#include "core/exceptions/src/DataException.h"
#include <mutex>

using namespace std::string_literals;

namespace TA_Base_Core
{
    SystemControllerTable::Table& SystemControllerTable::select()
    {
        static SystemControllerTable::Table s_table;
        static std::once_flag s_once;
        std::call_once(s_once, [&]
        {
            auto* db = DatabaseFactory::getInstance().getDatabase(/*SysController_SCd*/SysController_Cd, Read);
            auto data = db->executeQueryAll("SELECT * FROM sn_system_controller"s);

            for (auto row : *data)
            {
                s_table.emplace_back(Row
                {
                    row.getUnsignedLongData("SNSCGR_ID"),
                    row.getStringData("NAME"),
                    row.getUnsignedLongData("ANGENT_POLL_PERIOD"),
                    row.getUnsignedLongData("HEARTBEAT_SEND_PERIOD"),
                    row.getUnsignedLongData("AGENT_RESPONSE_TIMEOUT"),
                    row.getUnsignedLongData("SERVER_ENTITY_PKEY"),
                    row.getUnsignedLongData("SNSCGR_ID"),
                    row.getStringData("DEBUG_LOG_DIR"),
                    row.getStringData("BIN_DIR"),
                    row.getUnsignedLongData("HEARTBEAT_SEND_NUMBER")
                });
            }
        });
        return s_table;
    }

    SystemControllerTable::Table SystemControllerTable::select(Where where)
    {
        return stdutil::filter_copy(select(), where);
    }

    boost::optional<SystemControllerTable::Row&> SystemControllerTable::selectFirst(Where where)
    {
        return stdutil::get_if_optional(select(), where);
    }

    const std::string& SystemControllerTable::Row::groupName()
    {
        if (!stdutil::has_key(m_cache, "groupName"s))
        {
            if (auto x = SystemControllerGroupTable::selectFirst([&](auto & r) { return r.snscgr_id == snscgr_id; }))
            {
                m_cache.emplace("groupName", x->name);
            }
        }

        return *boost::any_cast<std::string>(&m_cache["groupName"s]);
    }
}
