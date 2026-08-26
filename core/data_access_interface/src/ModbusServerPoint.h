
/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/core/data_access_interface/src/ModbusServerPoint.h $
 * @author:  Derrick Liew
 * @version: $Revision: #4 $
 *
 * Last modification: $DateTime: 2016/05/05 18:05:34 $
 * Last modified by:  $Author: dhanshri $
 *
 *
 *
 */
#if !defined(DAI_MODBUSSERVERPOINT_H)
#define DAI_MODBUSSERVERPOINT_H

#include "core/data_access_interface/src/IModbusServerPoint.h"

namespace TA_Base_Core
{
    class IData;

    class ModbusServerPoint : public IModbusServerPoint
    {
    public:

        ModbusServerPoint(const ta_uint32 row, TA_Base_Core::IData& data);
        ~ModbusServerPoint();

        /**
         * Refer to IModbusServerPoint.h for a description of this method.
         */
        ta_uint32 getKey() {return m_pkey;};

        /**
         * Refer to IModbusServerPoint.h for a description of this method.
         */
        virtual void invalidate();

        ta_uint32 getAgentKey() {return m_agentKey;};
        std::string getAddress() {return m_address;};
        ta_uint32 getEntitykey() {return m_entityKey;};
        unsigned short getPort() {return m_port;};
        std::string getName() {return    m_name;};
        std::string getLength() {return  m_dataLength;}
        std::string getEntityAddress() {return m_entity_address;}

    public:

        static const std::string    ENTITY_AGENTKEY;
        static const std::string    MODBUS_POINT_PKEY;
        static const std::string    MODBUS_POINT_ENTITY_KEY;
        static const std::string    MODBUS_POINT_ADDRESS;
        static const std::string    MODBUS_POINT_PORT;
        static const std::string    MODBUS_POINT_DATALENGTH;
        static const std::string    MODBUS_POINT_ENTITY_ADDRESS;
        static const std::string    MODBUS_POINT_NAME;

    private:

        // Assignment operator not used so it is made private
        ModbusServerPoint(const ModbusServerPoint&);
        ModbusServerPoint& operator=(const ModbusServerPoint&);

        ta_uint32       m_agentKey;
        ta_uint32       m_pkey;
        ta_uint32       m_entityKey;
        std::string     m_address;
        std::string     m_entity_address;
        std::string     m_dataLength;
        ta_uint16       m_port;
        std::string     m_name;
    };
} // closes TA_Base_Core

#endif // !defined(DAI_MODBUSSERVERPOINT_H)
