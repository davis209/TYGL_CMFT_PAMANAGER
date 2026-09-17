/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/main.cpp $
 * @author:  christiandw
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2012/02/06 16:15:14 $
 * Last modified by:  $Author: hongzhi.zhang $
 *
 * This file contains the main that starts the PA Agent application
 *
 */

#include "pch.h"
#include "PAAgent.h"
#include "core/utility/src/base_ex/DebugUtilEx.h"
#include "core/utility/src/base_ex/RunParamsEx.h"
#include "core/exceptions/src/GenericAgentException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/process_management/src/UtilityInitialiser.h"
#include "core/utilities/src/CallstackLogger.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;
using TA_Base_Core::GenericAgentException;

int main(int argc, char* argv[])
{
    FUNCTION_ENTRY("main");
    TA_Base_Ex::DebugUtilEx::enable_log_callstack();
    TA_Base_Ex::DebugUtilEx::setup_file_debug_levels(argc, argv);
    LOG_CALLSTACK("main");

    TA_IRS_App::PAAgent* agent = 0;

    try
    {
        agent = new TA_IRS_App::PAAgent(argc, argv);
        agent->run();
        delete agent;
    }
    catch (const GenericAgentException& gae)
    {
        LOG_EXCEPTION("GenericAgentException", gae.what());
    }
    catch (const TA_Base_Core::DatabaseException& dbe)
    {
        LOG_EXCEPTION("DatabaseException", dbe.what());
    }
    catch (const TA_Base_Core::DataException& de)
    {
        LOG_EXCEPTION("DataException", de.what());
    }
    catch (std::exception e)
    {
        LOG_EXCEPTION("generic exception class", e.what());
    }
    catch (...)
    {
        LOG_EXCEPTION("unknown", "exception of unknown type caught");
    }

    FUNCTION_EXIT;
    return 0;
}
