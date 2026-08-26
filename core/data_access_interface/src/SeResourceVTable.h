/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/SeResourceVTable.h $
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
    struct SeResourceVTable
    {
        struct Row
        {
            size_t pkey;
            std::string name;
            size_t subsystemkey;
        };

        using Table = std::vector<Row>;
        using Where = std::function<bool(const Row&)>;

        static Table& select();
        static Table select(Where where);
        static boost::optional<Row&> selectFirst(Where where);
    };
}
