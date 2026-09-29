/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 *
 * Source:    $File: //depot/TYGL_CMFT_TIP/J155/transactive/app/signs/tis_agent/src/stis_protocol/STISEvents.h $
 * @author:   Robin Ashcroft
 * @version:  $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 */

#pragma once
#include "core/utility/src/core/GenericSignals.h"
#include "core/utility/src/core/StaticObject.h"

namespace TA_IRS_App::stisevents::detail
{
    using st::GenericSignals;
    using st::StaticObject;

    struct stis_events_tag {};

    using STISEvents = StaticObject<GenericSignals, stis_events_tag>;

    static inline STISEvents s_stis_events;
}

namespace TA_IRS_App
{
    using stisevents::detail::s_stis_events;
}
