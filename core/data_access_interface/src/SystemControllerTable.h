/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/SystemControllerTable.h $
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

#pragma once
#include <boost/optional.hpp>
#include <vector>
#include <functional>
#include <string>
#include <boost/any.hpp>
#include <map>

namespace TA_Base_Core
{
    struct SystemControllerTable
    {
        struct Row
        {
            size_t snscon_id;
            std::string name;
            size_t angent_poll_period;
            size_t heartbeat_send_period;
            size_t agent_response_timeout;
            size_t server_entity_pkey;
            size_t snscgr_id;
            std::string debug_log_dir;
            std::string bin_dir;
            size_t heartbeat_send_number;

            const std::string& groupName();

            std::map<std::string, boost::any> m_cache;
        };

        using Table = std::vector<Row>;
        using Where = std::function<bool(const Row&)>;

        static Table& select();
        static boost::optional<Row&> selectFirst(Where where);
        static Table select(Where where);
    };
}
