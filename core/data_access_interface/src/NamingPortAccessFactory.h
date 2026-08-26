// NamingPortAccessFactory.h: interface for the NamingPortAccessFactory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NAMINGPORTACCESSFACTORY_H__73B68F49_6DC8_40F4_A1AD_83B5EF146321__INCLUDED_)
#define AFX_NAMINGPORTACCESSFACTORY_H__73B68F49_6DC8_40F4_A1AD_83B5EF146321__INCLUDED_

#if _MSC_VER > 1000
    #pragma once
#endif // _MSC_VER > 1000

#include "core/types/src/ta_types.h"
#include "core/data_access_interface/src/INamingPort.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/utilities/src/RunParams.h"
#include <boost/shared_ptr.hpp>
#include <map>
#include <vector>
#include <string>
#include <ace/Singleton.h>

namespace TA_Base_Core
{
    /**
     * This factory class is used for create NamingPort object.
     * This class is added for CL-21230
     */

    //    typedef std::pair<std::string, unsigned long> HostnamePort;
    struct HostnamePort
    {
        std::string hostname;
        ta_uint32   port;
        std::string mapAgent;
    };

    typedef std::vector<HostnamePort*> HostnamePortList;

    class NamingPortAccessFactory
    {
    public:

        static NamingPortAccessFactory& getInstance();
        static void removeInstance();

        //replaced by getHostnamePortList (naming_ports_ex)
        //virtual INamingPort& getNamingPort(const std::string& agentName, int& map_id);
        virtual HostnamePortList& getHostnamePortList(const std::string& agentName, int map_id = NULL_AGENT_MAP_ID);
        virtual ta_uint32 gePort(const std::string& agentName, const std::string& hostname, int map_id = NULL_AGENT_MAP_ID);

    protected:

        NamingPortAccessFactory();
        virtual ~NamingPortAccessFactory();
        friend class ACE_Singleton<NamingPortAccessFactory, ACE_Recursive_Thread_Mutex>;

        virtual void loadHostnamePortList(const std::string& agentName, int& map_id);

    protected:

        ReEntrantThreadLockable m_lock;
        //typedef std::map<std::string, INamingPort*> NamingPortMap; //Key=mapid@AgentName
        //NamingPortMap m_namingPorts;

        typedef std::map<std::string, HostnamePortList> Agent2HostnamePortMap;  //Key=mapid@AgentName
        Agent2HostnamePortMap m_ports;

    private:

        std::string getKey(const std::string& agentName, int& map_id);
    };
}

#endif // !defined(AFX_NAMINGPORTACCESSFACTORY_H__73B68F49_6DC8_40F4_A1AD_83B5EF146321__INCLUDED_)
