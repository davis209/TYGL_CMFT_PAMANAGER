/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/J155/transactive/app/signs/tis_agent/src/main.cpp $
 * @author:  Ripple
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 */

#include "pch.h"
#include "TISAgent.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/versioning/src/Version.h"
#include "core/exceptions/src/GenericAgentException.h"

using namespace TA_IRS_App;
using namespace TA_Base_Core;

int main(int argc, char* argv[])
{
    FUNCTION_ENTRY("TISAgent");

    Version::checkCommandLine(argc, argv);

    try
    {
        TISAgent agent(argc, argv);
        agent.run();
    }
    catch (const GenericAgentException& e)
    {
        LOG_EXCEPTION("GenericAgentException", e.what());
    }

    FUNCTION_EXIT;
    return 0;
}
