/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/DbConnectionStrings.cpp $
 * @author:  R.Stagg
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 */

#ifdef __WIN32__
    #pragma warning(disable:4786 4503 4819 4996)
#endif

#include "DbConnectionStrings.h"
#include "DbConnectionStringsImpl.h"
#include "DataTypesLookup.h"
#include "core/database/src/CommonType.h"
#include "core/exceptions/src/DbConnectionFailed.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/CacheDecorator.h"
#include "core/utilities/src/StdUtil.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/DebugUtilMacrosHelper.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include <boost/format.hpp>

using boost::filesystem::path;

namespace TA_Base_Core
{
    DbConnectionStrings::DbConnectionStrings(const std::string& filepath)
        : m_filepath(system_complete(path(filepath)).string())
    {
    }

    DatabaseConnectionMap& DbConnectionStrings::getConnectionMap()
    {
        init();
        return m_connections_map;
    }

    DatabaseConnectionMap& DbConnectionStrings::getRawConnectionMap()
    {
        init();
        return m_raw_connections_map;
    }

    ConnectionStringSet& DbConnectionStrings::getConnectionStringSet()
    {
        init();
        return m_connection_string_set;
    }

    DatabaseNameSet& DbConnectionStrings::getDatabaseNameSet()
    {
        init();
        return m_database_name_set;
    }

    DataConnections& DbConnectionStrings::getConnectionList(EDataTypes dataType, EDataActions action)
    {
        init();

        LOG_DEBUG("getConnectionList(): Looking for a database for data type %i, action %i.", dataType, action);

        auto it = m_raw_connections_map.find(std::make_pair(dataType, action));

        if (it == m_raw_connections_map.end())
        {
            throw DbConnectionFailed(str(boost::format("No connection string defined for %d/%d") % dataType % action));
        }

        return it->second;
    }

    bool DbConnectionStrings::empty()
    {
        init();
        return m_connections_map.empty();
    }

    DbConnectionStrings& DbConnectionStrings::getInstance()
    {
        TA_ASSERT(RunParams::getInstance().isSet(RPARAM_DBCONNECTIONFILE), "DbConnectionFile not set.");
        return DbConnectionStringsImpl::getInstance().getDbConnectionStrings(RPARAM_DBCONNECTIONFILE_value);
    }

    void DbConnectionStrings::init()
    {
        boost::call_once([&]
        {
            if (m_connections_map.empty() && m_filepath.size())
            {
                *this = DbConnectionStringsImpl::getInstance().getDbConnectionStrings(m_filepath);
            }
        },
        m_initOnce);
    }
}
