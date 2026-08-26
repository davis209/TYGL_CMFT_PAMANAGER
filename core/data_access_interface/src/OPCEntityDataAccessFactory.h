/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source : $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/OPCEntityDataAccessFactory.h $
 * @author  DarrenS
 * @version $Revision: #3 $
 *
 * Last modification : $DateTime: 2023/10/16 18:31:43 $
 * Last modified by : $Author: CM $
 *
 * class representing OPCEntityDataAccessFactory
 *
 */
#ifndef OPC_ENTITY_DATA_ACCESS_FACTORY_H
#define OPC_ENTITY_DATA_ACCESS_FACTORY_H

#include "core/types/src/ta_types.h"
#include <string>
#include <vector>
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

namespace TA_Base_Core
{
    struct OPCDataPointData
    {
        ta_uint32 entityKey;
        ta_uint32 agentKey;
        ta_uint32 locationKey;
        std::string   entityName;
        OPCDataPointData() {}
        OPCDataPointData(ta_uint32 paraEntityKey, ta_uint32 paraAgentKey,
                         ta_uint32 paraLocationKey, const std::string& paraName)
            :
            entityKey(paraEntityKey),
            agentKey(paraAgentKey),
            locationKey(paraLocationKey),
            entityName(paraName)
        {
        }
    };

    struct OPCDataNodeData
    {
        ta_uint32 entityKey;
        ta_uint32 agentKey;
        ta_uint32 locationKey;
        std::string   entityName;
        std::string   shortLabel;
        std::string   label;
        std::string   assetName;

        OPCDataNodeData() {}

        OPCDataNodeData(ta_uint32 paraEntityKey, ta_uint32 paraAgentKey,
                        ta_uint32 paraLocationKey, const std::string& paraName, const std::string& paraShortLabel,
                        const std::string& paraLabel, const std::string& paraAssetName)
            :
            entityKey(paraEntityKey),
            agentKey(paraAgentKey),
            locationKey(paraLocationKey),
            entityName(paraName),
            shortLabel(paraShortLabel),
            label(paraLabel),
            assetName(paraAssetName)
        {
        }
    };

    typedef std::vector<OPCDataPointData> OPCDataPointDataList;
    typedef std::vector<OPCDataNodeData>  OPCDataNodeDataList;

    class OPCEntityDataAccessFactory
    {
    public:

        OPCEntityDataAccessFactory(bool bCentralizeOpcFlag = false);
        virtual ~OPCEntityDataAccessFactory();
        static OPCEntityDataAccessFactory& getInstance(bool bCentralizeOpcFlag = false);
        OPCDataPointDataList* getOPCDpDataAtLocations(std::vector<ta_uint32>& locationKeys);
        OPCDataPointDataList* getOPCDpDataAtAllLocations();
        OPCDataNodeDataList* getOPCDnDataAtLocations(std::vector<ta_uint32>& locationKeys);
        OPCDataNodeDataList* getOPCDnDataAtAllLocations();

    private:

        struct EntityParameterData
        {
            ta_uint32 entityKey;
            std::string   parameterName;
            std::string   parameterValue;
        };
        typedef std::vector<EntityParameterData> EntityParameterDataList;

        ta_uint32 getOPCServerAgentEK(IDatabase* pDatabase);

        void getOPCDpData(IDatabase* pDatabase, const SQLStatement& sql, OPCDataPointDataList& dataList);
        void getOPCDnData(IDatabase* pDatabase, const SQLStatement& sql, const SQLStatement& sqlParameter,
                          OPCDataNodeDataList& dataList, EntityParameterDataList& dnParameterList);
        void assembleDnOPCData(OPCDataNodeDataList& dataList, EntityParameterDataList& dnParameterList);

        static OPCEntityDataAccessFactory* s_instance;

        ta_uint32 m_OpcServerAgentEK;
        std::string m_OpcServerAgentName;
        bool m_bCentralizeOpcFlag;
    };
}

#endif
