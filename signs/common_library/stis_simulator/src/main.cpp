/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/J155/transactive/app/signs/common_library/stis_simulator/src/main.cpp $
 * @author:  Ripple
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 */

#include "pch.h"
#include "UserInterface.h"
#include "STISSimulator.h"
#include "core/process_management/src/UtilityInitialiser.h"
#include "core/versioning/src/Version.h"
#include "core/utilities/src/DebugUtilInit.h"
#include "core/utility/src/base_ex/RunParamsEx.h"
#include "core/utility/src/base_ex/DebugUtilEx.h"
#include "core/utility/src/core/StdEx.h"
#include "core/utility/src/core/algorithm/strings.h"
#include "core/utility/src/core/SimpleTimer.h"

using namespace std::literals;
using namespace TA_Base_Ex;
using namespace TA_Base_Core;
using namespace TA_IRS_App::STIS_PROTOCOL::SIMULATOR;
using namespace st2::string_literals;

int main(int argc, char* argv[])
{
    Version::checkCommandLine(argc, argv);

    if (1 == argc || (1 < argc && st::any_of_iequal("--help,help,-h,?,-?,/?"_csv, argv[1])))
    {
        std::cout << STISSimulator::help() << std::endl;
        return 0;
    }

    RunParamsEx::set("DebugRemoveOldFiles", "true");
    RunParamsEx::set("NoNamingPort");
    RunParamsEx::set("DebugCallstack", "true");

    CallstackLoggerConfig::enable();

    RunParamsEx::parseCmdLine(argc, argv);
    parseLocalConfigurationFile();

    gSetDebugUtilFromRunParams();

    auto simulator = std::make_shared<STISSimulator>(argc, argv);
    simulator->start();

    UserInterface(simulator).run();

    return 0;
}
