/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/DbConnectionStringsImpl.cpp $
 * @author:  R.Stagg
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 */

#include "DbConnectionStringsImpl.h"
#include "DataTypesLookup.h"
#include "core/database/src/CommonType.h"
#include "core/exceptions/src/DbConnectionFailed.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/StdUtil.h"
#include "core/utilities/src/RunParamsAny.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include <boost/format.hpp>
#include <ace/Singleton.h>

namespace TA_Base_Core
{
    using boost::starts_with;
    using boost::istarts_with;

    DbConnectionStringsImpl::DbConnectionStringsImpl()
    {
        RunParams::getInstance().registerRunParamUser(this, RPARAM_DBCONNECTIONFILE);
    }

    DbConnectionStrings& DbConnectionStringsImpl::getDbConnectionStrings(const std::string& filepath)
    {
        TA_THREADGUARD(m_lock);
        auto& result = m_dbconnectionstrings[stdutil::filename(filepath)];  // [aqiu] implicit insert if not exist
        return result.empty() ? (filepath.size() ? parseFile(filepath, result) : result) : result;
    }

    DbConnectionStrings& DbConnectionStringsImpl::parseFile(const std::string& filepath, DbConnectionStrings& result)
    {
        result.m_filepath = filepath;
        auto filename = stdutil::filename(filepath);

        if (!RunParamsAny::getInstance().isSet(filename))
        {
            if (boost::filesystem::exists(filepath))
            {
                auto content = stdutil::string_from_file(filepath);
                LOG_DEBUG_IF(content.empty(), boost::format("parseFile(): cannot get content from local file %s") % filepath);

                if (content.size())
                {
                    // set the dbconnectionstring csv filename as parameter name
                    RunParamsAny::getInstance().set(filename, content);
                    LOG_DEBUG("parseFile(): get content from local file %s (size=%d bytes)", filepath, content.size());
                }
            }
        }

        TA_ASSERT(RunParamsAny::getInstance().isSet(filename), str(boost::format("Cannot get content for file: %s") % filepath).c_str());
        LOG_DEBUG("parseFile(): Parsing DbConnectionFile %s", filepath);
        return parseFileString(RunParamsAny::getInstance().value<std::string>(filename), result);
    }

    DbConnectionStrings& DbConnectionStringsImpl::parseFileString(const std::string& content, DbConnectionStrings& result)
    {
        DatabaseConnectionMap& connections_map = result.m_connections_map;
        DatabaseConnectionMap& raw_connections_map = result.m_raw_connections_map;
        DatabaseNameSet& database_name_set = result.m_database_name_set;
        ConnectionStringSet& connection_string_set = result.m_connection_string_set;

        for (auto& line : stdutil::splitted(content, "\n", "--minimize"))
        {
            boost::trim(line);

            // run some checks to ensure we have the right number
            if (istarts_with(line, "Data Type") || starts_with(line, "#"))
            {
                continue; // Skip header row
            }

            auto items = stdutil::splitted(line, ",:;", "--trim");

            if (items.size() < CON_SIZE + 2 || (items.size() - 2) % CON_SIZE)  // min required is datatype,dataaction,dbtype,dbname,dbuser,dbpass,dbhostname
            {
                LOG_WARN("parseFileString(): Incorrect number of parameters in file: %s, SKIPPING line is:\n%s", result.m_filepath, line);
                continue;
            }

            // we have the correct number of parameters, store them
            EDataTypes datatype = getDataTypesEnum(items[0]);
            EDataActions dataaction = getDataActionsEnum(items[1]);

            if (NotExists_d == datatype || MaxDataTypes == datatype || MaxDataActions == dataaction)
            {
                LOG_WARN("parseFileString(): Incorrect data-type/data-action in file: %s, SKIPPING line:\n%s", result.m_filepath, line);
                continue;
            }

            DataKey key = std::make_pair(datatype, dataaction);
            items.erase(items.begin(), items.begin() + 2);

            DataConnections& connections = connections_map[key];    // [aqiu] implicit insert if does not exist
            DataConnections& raw_connections = raw_connections_map[key];

            while (items.size())
            {
                if (items[0].size())
                {
                    std::string first = items[CON_STR_SchemaName] + (items[CON_STR_Hostname].empty() ? "" : "@" + items[CON_STR_Hostname]); //Database Name
                    std::string second = boost::join(std::vector<std::string>(items.begin(), items.begin() + CON_SIZE), ":"); //Connection String
                    connections.push_back(std::make_pair(first, second));
                    raw_connections.push_back(std::make_pair(items[CON_STR_SchemaName], second));
                    database_name_set.insert(first);
                    connection_string_set.insert(second);
                }

                items.erase(items.begin(), items.begin() + CON_SIZE);
            }
        }

        return result;
    }

    void DbConnectionStringsImpl::onRunParamChange(const std::string& name, const std::string& value)
    {
        if (RPARAM_DBCONNECTIONFILE == name)
        {
            getDbConnectionStrings(value);
        }
    }
}
