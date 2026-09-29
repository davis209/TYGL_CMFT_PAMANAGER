/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 *
 * Source:    $File: //depot/TYGL_CMFT_TIP/J155/transactive/app/signs/tis_agent/src/Exceptions.h $
 * @author:   Robin Ashcroft
 * @version:  $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * Specifies all the exceptions thrown by the TIS Agent.
 */

#pragma once
#include "core/exceptions/src/TransactiveException.h"

namespace TA_IRS_Core
{
    // Generic TIS Agent Exception

    struct TISAgentException : TA_Base_Core::TransactiveException
    {
        using TransactiveException::TransactiveException;
    };
}
