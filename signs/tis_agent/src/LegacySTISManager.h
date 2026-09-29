/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 *
 * Source:    $File: //depot/J155_TIP/J155/transactive/app/signs/tis_agent/src/TISAgent.h $
 * @author:   Robin Ashcroft
 * @version:  $Revision: #5 $
 *
 * Last modification: $DateTime: 2022/02/04 10:20:04 $
 * Last modified by:  $Author: limin.zhu $
 *
 */

#pragma once
#include <memory>

namespace TA_IRS_App::legacytisagent::detail
{
    struct LegacySTISManager
    {
        LegacySTISManager();

        static LegacySTISManager& instance();

        void start();
        void stop();

        struct Impl;
        std::shared_ptr<Impl> m_impl;
    };
}

namespace TA_IRS_App
{
    using legacytisagent::detail::LegacySTISManager;
}
