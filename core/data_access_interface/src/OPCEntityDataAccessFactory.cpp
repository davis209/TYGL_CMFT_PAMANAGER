/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source : $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/OPCEntityDataAccessFactory.cpp $
 * @author  DarrenS
 * @version $Revision: #3 $
 *
 * Last modification : $DateTime: 2023/10/16 18:31:43 $
 * Last modified by : $Author: CM $
 *
 * class representing OPCEntityDataAccessFactory
 *
 */

#include "core/types/src/ta_types.h"
#include "core/data_access_interface/src/OPCEntityDataAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/utilities/src/TAAssert.h"
#include <sstream>
#include "core/exceptions/src/DataException.h"

// Need to bring in the DataException
using TA_Base_Core::DataException;

namespace TA_Base_Core
{
    OPCEntityDataAccessFactory* OPCEntityDataAccessFactory::s_instance = NULL;
    const std::string OCC_OPCSERVERAGENT_NAME = "OccOpcServerAgent";

    OPCEntityDataAccessFactory::OPCEntityDataAccessFactory(bool bCentralizeOpcFlag): m_bCentralizeOpcFlag(bCentralizeOpcFlag), m_OpcServerAgentEK(0)
    {
        m_OpcServerAgentName = OCC_OPCSERVERAGENT_NAME;
    }

    OPCEntityDataAccessFactory::~OPCEntityDataAccessFactory()
    {
    }

    OPCEntityDataAccessFactory& OPCEntityDataAccessFactory::getInstance(bool bCentralizeOpcFlag)
    {
        if (NULL == s_instance)
        {
            s_instance = new OPCEntityDataAccessFactory(bCentralizeOpcFlag);
        }

        return *s_instance;
    }

    OPCDataPointDataList* OPCEntityDataAccessFactory::getOPCDpDataAtLocations(std::vector<ta_uint32>& locationKeys)
    {
        OPCDataPointDataList* pDataList = new OPCDataPointDataList();
        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_SCd/*OnlineUpdatable_Cd*/, Read);

        std::stringstream sqlLocationKeys;
        sqlLocationKeys << "(";
        int locSize = locationKeys.size();
        std::vector<ta_uint32>::const_iterator itLoc = locationKeys.begin();

        for (int i = 0; i < locSize - 1; ++i)
        {
            sqlLocationKeys << *itLoc;
            sqlLocationKeys << ",";
            ++itLoc;
        }

        sqlLocationKeys << *itLoc;
        sqlLocationKeys << ")";

        std::string dpTypeName = DataPointEntityData::getStaticType();
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ENTITYTYPE_SELECT_4023, dpTypeName, sqlLocationKeys.str());

        getOPCDpData(databaseConnection, sql, *pDataList);

        return pDataList;
    }

    OPCDataPointDataList* OPCEntityDataAccessFactory::getOPCDpDataAtAllLocations()
    {
        OPCDataPointDataList* pDataList = new OPCDataPointDataList();

        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_SCd/*OnlineUpdatable_Cd*/, Read);

        std::string dpTypeName = DataPointEntityData::getStaticType();
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ENTITYTYPE_SELECT_4022, dpTypeName);

        getOPCDpData(databaseConnection, sql, *pDataList);

        return pDataList;
    }

    OPCDataNodeDataList*  OPCEntityDataAccessFactory::getOPCDnDataAtLocations(std::vector<ta_uint32>& locationKeys)
    {
        OPCDataNodeDataList* pDataList = new OPCDataNodeDataList();
        EntityParameterDataList dnParaList;

        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_SCd/*OnlineUpdatable_Cd*/, Read);

        std::stringstream sqlLocationKeys;
        sqlLocationKeys << "(";
        int locSize = locationKeys.size();
        std::vector<ta_uint32>::const_iterator itLoc = locationKeys.begin();

        for (int i = 0; i < locSize - 1; ++i)
        {
            sqlLocationKeys << *itLoc;
            sqlLocationKeys << ",";
            ++itLoc;
        }

        sqlLocationKeys << *itLoc;
        sqlLocationKeys << ")";

        std::string sqlParameterNames("(");
        sqlParameterNames += "'";
        sqlParameterNames += DataNodeEntityData::DATANODE_SHORT_LABEL;
        sqlParameterNames += "'";
        sqlParameterNames += ",";

        sqlParameterNames += "'";
        sqlParameterNames += DataNodeEntityData::DATANODE_LABEL;
        sqlParameterNames += "'";
        sqlParameterNames += ",";

        sqlParameterNames += "'";
        sqlParameterNames += DataNodeEntityData::DATANODE_ASSET_NAME;
        sqlParameterNames += "'";
        sqlParameterNames += ")";

        std::string dnTypeName = DataNodeEntityData::getStaticType();

        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ENTITYTYPE_SELECT_4023, dnTypeName, sqlLocationKeys.str());
        SQLStatement sqlParameter;
        databaseConnection->prepareSQLStatement(sqlParameter, ENTITYPARAMETERVALUE_V_SELECT_3074, sqlLocationKeys.str(), dnTypeName,  sqlParameterNames);

        getOPCDnData(databaseConnection, sql, sqlParameter, *pDataList, dnParaList);
        assembleDnOPCData(*pDataList, dnParaList);

        return pDataList;
    }

    OPCDataNodeDataList* OPCEntityDataAccessFactory::getOPCDnDataAtAllLocations()
    {
        OPCDataNodeDataList* pDataList = new OPCDataNodeDataList();
        EntityParameterDataList dnParaList;

        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_SCd/*OnlineUpdatable_Cd*/, Read);

        std::string dnTypeName = DataNodeEntityData::getStaticType();

        std::string sqlParameterNames("(");
        sqlParameterNames += "'";
        sqlParameterNames += DataNodeEntityData::DATANODE_SHORT_LABEL;
        sqlParameterNames += "'";
        sqlParameterNames += ",";

        sqlParameterNames += "'";
        sqlParameterNames += DataNodeEntityData::DATANODE_LABEL;
        sqlParameterNames += "'";
        sqlParameterNames += ",";

        sqlParameterNames += "'";
        sqlParameterNames += DataNodeEntityData::DATANODE_ASSET_NAME;
        sqlParameterNames += "'";
        sqlParameterNames += ")";

        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ENTITYTYPE_SELECT_4022, dnTypeName);
        SQLStatement sqlParameter;
        databaseConnection->prepareSQLStatement(sqlParameter, ENTITYPARAMETERVALUE_V_SELECT_3073, dnTypeName, sqlParameterNames);

        getOPCDnData(databaseConnection, sql, sqlParameter, *pDataList, dnParaList);
        assembleDnOPCData(*pDataList, dnParaList);

        return pDataList;
    }

    void OPCEntityDataAccessFactory::getOPCDpData(IDatabase* pDatabase, const SQLStatement& sql, OPCDataPointDataList& dataList)
    {
        if (m_bCentralizeOpcFlag == true)
        {
            m_OpcServerAgentEK = getOPCServerAgentEK(pDatabase);
        }

        std::string entityKeyColumn = "ENTITYKEY";
        std::string entityNameColumn = "ENTITYNAME";
        std::string agentKeyColumn = "AGENTKEY";
        std::string locationKeyColumn = "LOCATIONKEY";
        std::vector<std::string> columnNames;
        columnNames.push_back(entityKeyColumn);
        columnNames.push_back(agentKeyColumn);
        columnNames.push_back(locationKeyColumn);
        columnNames.push_back(entityNameColumn);

        TA_ASSERT(0 != pDatabase, "the database connection is null");

        IData* data = pDatabase->executeQuery(sql, columnNames);

        do
        {
            for (ta_uint32 i = 0; i < data->getNumRows(); i++)
            {
                if (m_bCentralizeOpcFlag == false)
                {
                    dataList.push_back(
                        OPCDataPointData(data->getUnsignedLongData(i, entityKeyColumn),
                                         data->getUnsignedLongData(i, agentKeyColumn),
                                         data->getUnsignedLongData(i, locationKeyColumn),
                                         data->getStringData(i, entityNameColumn)));
                }
                else
                {
                    dataList.push_back(
                        OPCDataPointData(data->getUnsignedLongData(i, entityKeyColumn),
                                         m_OpcServerAgentEK,
                                         data->getUnsignedLongData(i, locationKeyColumn),
                                         data->getStringData(i, entityNameColumn)));
                }
            }

            delete data;
            data = NULL;
        }
        while (pDatabase->moreData(data));
    }

    void OPCEntityDataAccessFactory::getOPCDnData(IDatabase* pDatabase, const SQLStatement& sql, const SQLStatement& sqlParameter,
                                                  OPCDataNodeDataList& dataList, EntityParameterDataList& dnParameterList)
    {
        if (m_bCentralizeOpcFlag == true)
        {
            m_OpcServerAgentEK = getOPCServerAgentEK(pDatabase);
        }

        std::vector<std::string> columnNames;
        std::string entityKeyColumn("ENTITYKEY");
        std::string entityNameColumn("ENTITYNAME");
        std::string agentKeyColumn("AGENTKEY");
        std::string locationKeyColumn("LOCATIONKEY");
        columnNames.push_back(entityKeyColumn);
        columnNames.push_back(agentKeyColumn);
        columnNames.push_back(locationKeyColumn);
        columnNames.push_back(entityNameColumn);

        std::vector<std::string> paraColumnNames;
        std::string p1("ENTITYKEY");
        std::string p2("NAME");
        std::string p3("VALUE");
        paraColumnNames.push_back(p1);
        paraColumnNames.push_back(p2);
        paraColumnNames.push_back(p3);

        TA_ASSERT(0 != pDatabase, "the database connection is null");

        IData* dataParameter = pDatabase->executeQuery(sqlParameter, paraColumnNames);

        do
        {
            for (ta_uint32 i = 0; i < dataParameter->getNumRows(); i++)
            {
                EntityParameterData parameterData;
                parameterData.entityKey = dataParameter->getUnsignedLongData(i, p1);
                parameterData.parameterName = dataParameter->getStringData(i, p2);
                parameterData.parameterValue = dataParameter->getStringData(i, p3);
                dnParameterList.push_back(parameterData);
            }

            delete dataParameter;
            dataParameter = NULL;
        }
        while (pDatabase->moreData(dataParameter));

        std::string emptyStr("");
        IData* data = pDatabase->executeQuery(sql, columnNames);

        do
        {
            for (ta_uint32 i = 0; i < data->getNumRows(); i++)
            {
                if (m_bCentralizeOpcFlag == false)
                {
                    dataList.push_back(
                        OPCDataNodeData(data->getUnsignedLongData(i, entityKeyColumn),
                                        data->getUnsignedLongData(i, agentKeyColumn),
                                        data->getUnsignedLongData(i, locationKeyColumn),
                                        data->getStringData(i, entityNameColumn),
                                        emptyStr, emptyStr, emptyStr));
                }
                else
                {
                    dataList.push_back(
                        OPCDataNodeData(data->getUnsignedLongData(i, entityKeyColumn),
                                        m_OpcServerAgentEK,
                                        data->getUnsignedLongData(i, locationKeyColumn),
                                        data->getStringData(i, entityNameColumn),
                                        emptyStr, emptyStr, emptyStr));
                }
            }

            delete data;
            data = NULL;
        }
        while (pDatabase->moreData(data));
    }

    //This function is for the purpose of getting the EK of OccOpcServerAgent, which will be used by OPCBrigde
    //to prepare the EPV and Entity data for DN and DP attributes, when CentralizeOpcBridge = true.
    ta_uint32 OPCEntityDataAccessFactory::getOPCServerAgentEK(IDatabase* databaseConnection)
    {
        TA_ASSERT(0 != databaseConnection, "the database connection is null");

        SQLStatement strSql;
        databaseConnection->prepareSQLStatement(strSql, ENTITY_SELECT_4503,
                                                databaseConnection->escapeInsertString(m_OpcServerAgentName.c_str()));

        std::string entityKeyColumn = "PKEY";
        std::vector<std::string> columnNames;
        columnNames.push_back(entityKeyColumn);

        IData* data = databaseConnection->executeQuery(strSql, columnNames);

        //check the returned data - we should have exactly 1 row returned

        if (0 == data->getNumRows())
        {
            // not found
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
            reasonMessage << "No data found for EntityKey with entityname = " << m_OpcServerAgentName;
            TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(), TA_Base_Core::DataException::NO_VALUE, "OpcServerAgent EntityKey"));
        }
        else if (1 == data->getNumRows()) // The name was found
        {
            ta_uint32 eAgentKey = data->getUnsignedLongData(0, entityKeyColumn);
            delete data;
            data =  NULL;
            return eAgentKey;
        }
        else if (1 < data->getNumRows()) // Two or more entities already exist with this name
        {   // This code will not be executed unless there is an error in the database
            // clean up the pointer
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
            reasonMessage << "More than one entity already exists name = " << m_OpcServerAgentName.c_str();
            TA_THROW(DataException(reasonMessage.str().c_str(), DataException::NOT_UNIQUE, "name"));
        }

        return 0;
    }

    void OPCEntityDataAccessFactory::assembleDnOPCData(OPCDataNodeDataList& dataList, EntityParameterDataList& dnParameterList)
    {
        std::map<ta_uint32, OPCDataNodeData*> tmpDataMap;

        OPCDataNodeDataList::iterator itData;

        for (itData = dataList.begin(); itData != dataList.end(); ++itData)
        {
            tmpDataMap[itData->entityKey] = &(*itData);
        }

        ta_uint32 preEntityKey = TA_MAX_UINT32;
        ta_uint32 entityKey    = TA_MAX_UINT32;
        OPCDataNodeData* opcDnData = NULL;
        EntityParameterDataList::iterator itParameter;
        std::map<ta_uint32, OPCDataNodeData*>::iterator itDataMap = tmpDataMap.end();

        for (itParameter = dnParameterList.begin(); itParameter != dnParameterList.end(); ++itParameter)
        {
            ta_uint32 entityKey = itParameter->entityKey;

            if (preEntityKey != entityKey)
            {
                preEntityKey = entityKey;
                itDataMap = tmpDataMap.find(entityKey);

                if (itDataMap != tmpDataMap.end())
                {
                    opcDnData = itDataMap->second;
                }
                else
                {
                    opcDnData = NULL;
                }
            }

            if (NULL == opcDnData) { continue; }

            if (itParameter->parameterName == TA_Base_Core::DataNodeEntityData::DATANODE_SHORT_LABEL)
            {
                opcDnData->shortLabel = itParameter->parameterValue;
            }
            else if (itParameter->parameterName == TA_Base_Core::DataNodeEntityData::DATANODE_LABEL)
            {
                opcDnData->label = itParameter->parameterValue;
            }
            else if (itParameter->parameterName == TA_Base_Core::DataNodeEntityData::DATANODE_ASSET_NAME)
            {
                opcDnData->assetName = itParameter->parameterValue;
            }
        }
    }
}
