/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/C830G_TIP/TA_BASE/transactive/core/data_access_interface/src/OperatorTable.h $
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

#pragma once
#include <boost/optional.hpp>
#include <vector>
#include <functional>
#include <string>

namespace TA_Base_Core
{
    struct OperatorTable
    {
        struct Row
        {
            size_t pkey;
            std::string name;
            std::string passwd;
            size_t accesslevel;
            std::string description;
            bool is_system;
            bool deleted;
            std::string created_by;
            time_t date_created;
            std::string modified_by;
            time_t date_modified;
        };

        using Table = std::vector<Row>;
        using Where = std::function<bool(const Row&)>;

        static Table& select();
        static Table select(Where where);
        static boost::optional<Row&> selectFirst(Where where);
    };
}
