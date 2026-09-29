/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/J155/transactive/app/signs/tis_gateway/src/main.cpp $
 * @author:  Ripple
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 */

#include "pch.h"
#include "TISGateway.h"
#include "core/utility/src/base_ex/DebugUtilEx.h"
#include "core/versioning/src/Version.h"
#include "core/exceptions/src/GenericAgentException.h"

using namespace TA_Base_Core;
using namespace TA_IRS_App;

int main(int argc, char* argv[])
{
    FUNCTION_ENTRY("TISGateway");

    Version::checkCommandLine(argc, argv);

    try
    {
        TISGateway agent(argc, argv);
        agent.run();
    }
    catch (const GenericAgentException& gae)
    {
        LOG_EXCEPTION("GenericAgentException", gae.what());
    }

    FUNCTION_EXIT;
    return 0;
}
