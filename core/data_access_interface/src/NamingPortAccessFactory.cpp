#include "core/types/src/ta_types.h"
#include "core/data_access_interface/src/NamingPortAccessFactory.h"
#include "core/data_access_interface/src/NamingPort.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/FunctionEntry.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/StaticObject.h"
#include "core/utilities/src/TA_String.h"
#include "core/utilities/src/DebugUtil.h"
#include <boost/format.hpp>

namespace TA_Base_Core
{
    using Storage = StaticObject<NamingPortAccessFactory*, NamingPortAccessFactory>;

    NamingPortAccessFactory& NamingPortAccessFactory::getInstance()
    {
        if (Storage::value() == NULL)
        {
            Storage::value() = ACE_Unmanaged_Singleton<NamingPortAccessFactory, ACE_Recursive_Thread_Mutex>::instance();
        }

        return *Storage::value();
    }

    void NamingPortAccessFactory::removeInstance()
    {
        ACE_Unmanaged_Singleton<NamingPortAccessFactory, ACE_Recursive_Thread_Mutex>::close();
        Storage::value() = NULL;
    }

    NamingPortAccessFactory::NamingPortAccessFactory()
    {
    }

    NamingPortAccessFactory::~NamingPortAccessFactory()
    {
        /*NamingPortMap::iterator iter = m_namingPorts.begin();

        while (iter != m_namingPorts.end())
        {
            delete iter->second;
            ++iter;
        }

        m_namingPorts.clear();*/
    }

    /*INamingPort& NamingPortAccessFactory::getNamingPort(const std::string& agentName, int& map_id)
    {
        FUNCTION_ENTRY("getNamingPort");

        TA_THREADGUARD(m_lock);
        std::string key = getKey(agentName, map_id);
        NamingPortMap::iterator iter = m_namingPorts.find(key);

        if (iter == m_namingPorts.end())
        {
            iter = m_namingPorts.insert(NamingPortMap::value_type(key, new NamingPort(agentName))).first;
        }

        INamingPort* np = iter->second;
        FUNCTION_EXIT;
        return *np;
    }*/

    ta_uint32 NamingPortAccessFactory::gePort(const std::string& agentName, const std::string& hostname, int map_id)
    {
        for (const HostnamePort* pair : getHostnamePortList(agentName, map_id))
        {
            if (pair->hostname == hostname)
            {
                return pair->port;
            }
        }

        return 0;
    }

    HostnamePortList& NamingPortAccessFactory::getHostnamePortList(const std::string& agentName, int map_id)
    {
        TA_THREADGUARD(m_lock);
        map_id = getAgentMapID(map_id);  //get real map id
        std::string key = getKey(agentName, map_id);

        if (m_ports.find(key) == m_ports.end())
        {
            try
            {
                loadHostnamePortList(agentName, map_id);
            }
            catch (...)
            {
            }
        }

        return m_ports[key];
    }

    void NamingPortAccessFactory::loadHostnamePortList(const std::string& agentName, int& map_id)
    {
        FUNCTION_ENTRY("loadDataFromDb");

        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(/*Naming_SCd*/Naming_Cd, Read);
        auto nDbServerType = databaseConnection->getDbServerType();

        // query host name of agent
        // std::string sql = "SELECT hostname, port FROM naming_ports_ex WHERE entity_key = (SELECT sour_agent FROM agent_map_v m WHERE m.sour_agent_name = '%s' and m.map_id=%s)";
        SQLStatement strSql;
        databaseConnection->prepareSQLStatement(strSql, NAMING_SELECT_23903, databaseConnection->escapeAQSQLString(agentName), map_id);

        // Set up the columnNames vector to be passed to executeQuery()
        std::string HOSTNAME_COLUMN = "HOSTNAME";
        std::string PORT_COLUMN = "PORT";
        std::string MAPAGENT_COLUMN = "MAPAGENT";

        std::vector<std::string> columnNames;
        columnNames.push_back(HOSTNAME_COLUMN);
        columnNames.push_back(PORT_COLUMN);
        columnNames.push_back(MAPAGENT_COLUMN);

        // Execute the query
        IData* data = databaseConnection->executeQuery(strSql, columnNames);

        if (NULL == data || 0 == data->getNumRows()) // No entry found with the specified message_id
        {
            delete data;
            return;
        }

        HostnamePortList list;

        do
        {
            for (int i = 0; i < (int)data->getNumRows(); ++i)
            {
                HostnamePort* hostport = new HostnamePort();

                hostport->hostname = data->getStringData(i, HOSTNAME_COLUMN);
                hostport->port = data->getIntegerData(i, PORT_COLUMN);
                hostport->mapAgent = data->getStringData(i, MAPAGENT_COLUMN);
                list.emplace_back(hostport);
            }
        }
        while (databaseConnection->moreData(data));

        delete data;
        data = NULL;
        std::string key = getKey(agentName, map_id);
        m_ports[key] = list;

        FUNCTION_EXIT;
    }

    std::string NamingPortAccessFactory::getKey(const std::string& agentName, int& map_id)
    {
        std::string key = agentName + toString(map_id);
        return key;
    }
}
