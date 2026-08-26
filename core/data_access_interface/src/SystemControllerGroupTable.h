/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/SystemControllerGroupTable.h $
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

namespace TA_Base_Core
{
    struct SystemControllerGroupTable
    {
        struct Row
        {
            size_t snscgr_id;
            std::string name;
            std::string description;
        };

        using Table = std::vector<Row>;
        using Where = std::function<bool(const Row&)>;

        static Table& select();
        static Table select(Where where);
        static boost::optional<Row&> selectFirst(Where where);
    };
}
