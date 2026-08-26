/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/data_access_interface/src/OpcServerAgentAccessFactory.h $
 * @author:  Dhanshri
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2016/07/15 17:01:52 $
 * Last modified by:  $Author: dhanshri $
 *
 * Data is primarily retrieved from the entity table
 *
 */

#if !defined(DAI_OPCSERVERAGENTACCESSFACTORY_H_)
#define DAI_OPCSERVERAGENTACCESSFACTORY_H_

#include "core/types/src/ta_types.h"
#include <string>
#include <vector>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

namespace TA_Base_Core
{
    class AgentData
    {
    public:

        ta_uint32 agentKey;
        std::string   agentName;
        AgentData::AgentData(): agentName(""), agentKey(0) {}
        AgentData::AgentData(ta_uint32 p1, std::string p2): agentName(p2), agentKey(p1)
        {
        }
    };
    typedef std::vector<AgentData*> AgentDataList;

    class OpcServerAgentAccessFactory
    {
    public:

        virtual ~OpcServerAgentAccessFactory() {};

        static void getOccAndStationAgentList(std::vector<AgentData*>& list);

    private:

        OpcServerAgentAccessFactory() {};
        OpcServerAgentAccessFactory(const OpcServerAgentAccessFactory& theOpcServerAgentAccessFactory);
        OpcServerAgentAccessFactory& operator=(const OpcServerAgentAccessFactory&);
    };

} // closes TA_Base_Core

#endif // !defined(DAI_OPCSERVERAGENTACCESSFACTORY_H_)
