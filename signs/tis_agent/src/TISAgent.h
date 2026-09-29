/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 *
 * Source:    $File: //depot/TYGL_CMFT_TIP/J155/transactive/app/signs/tis_agent/src/TISAgent.h $
 * @author:   Robin Ashcroft
 * @version:  $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 */

#pragma once
#include <memory>

namespace TA_IRS_App::tisagent::detail
{
    struct TISAgent
    {
        TISAgent(int argc, char* argv[]);

        void run();

        struct Impl;
        std::shared_ptr<Impl> m_impl;
    };
}

namespace TA_IRS_App
{
    using tisagent::detail::TISAgent;
}
