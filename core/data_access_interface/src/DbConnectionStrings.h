/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/DbConnectionStrings.h $
 * @author:  R.Stagg
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * This class reads and parses a csv file to get the connection
 * strings for all known data types
 */

#pragma once
#include "core/data_access_interface/src/DbStatus.h"
#include "core/data_access_interface/src/DataTypeEnums.h"
#include <map>
#include <string>
#include <vector>
#include <set>
#include <boost/thread/once.hpp>

namespace TA_Base_Core
{
    using ConnectionStringSet = std::set<std::string>;
    using DatabaseNameSet = std::set<std::string>;
    using DataKey = std::pair<EDataTypes, EDataActions>;
    using DataConnection = std::pair<std::string, std::string>; // database name, connection string
    using DataConnections = std::vector<DataConnection>;
    using DatabaseConnectionMap = std::map<DataKey, DataConnections>;

    class DbConnectionStrings
    {
        friend class DbConnectionStringsImpl;

    public:

        DbConnectionStrings() = default;
        DbConnectionStrings(const std::string& filepath);

        DatabaseConnectionMap& getConnectionMap();
        DatabaseConnectionMap& getRawConnectionMap();
        ConnectionStringSet& getConnectionStringSet();
        DatabaseNameSet& getDatabaseNameSet();
        DataConnections& getConnectionList(EDataTypes, EDataActions);
        bool empty();

        static DbConnectionStrings& getInstance();

    protected:

        void init();

    protected:

        std::string m_filepath;
        DatabaseNameSet m_database_name_set;
        ConnectionStringSet m_connection_string_set;
        DatabaseConnectionMap m_connections_map;
        DatabaseConnectionMap m_raw_connections_map;
        boost::once_flag m_initOnce = BOOST_ONCE_INIT;
    };
}
