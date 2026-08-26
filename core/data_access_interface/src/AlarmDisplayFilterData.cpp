/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TYGL_CMFT_TIP/TA_BASE/transactive/core/data_access_interface/src/AlarmDisplayFilterData.cpp $
 * @author:  Noel R. Tajanlangit
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2023/10/16 18:31:43 $
 * Last modified by:  $Author: CM $
 *
 * AlarmDisplayFilterData is an interface to the Alarm_Display_Filter table. It provides both read and write access
 * for agents and user applicaitons.
 */

#ifdef __WIN32__
    #pragma warning(disable:4786)
#endif

#include "core/types/src/ta_types.h"
#include "core/data_access_interface/src/AlarmDisplayFilterData.h"

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    /*
    * Constructor
    */
    AlarmDisplayFilterData::AlarmDisplayFilterData(ta_uint32 _pkey,
                                                   ta_uint32 _locationKey,
                                                   ta_uint32 _alarmType,
                                                   ta_uint32 _subSystemKey,
                                                   ta_uint32 _alarmLocationKey,
                                                   ta_uint32 _displayCondition,
                                                   ta_uint32 _deleted)
        : m_pkey(_pkey)
        , m_locationKey(_locationKey)
        , m_alarmType(_alarmType)
        , m_alarmLocationKey(_alarmLocationKey)
        , m_subSystemKey(_subSystemKey)
        , m_displayCondition((IAlarmDisplayFilterData::DISPLAY_CONDITION)_displayCondition)
        , m_deleted(_deleted)
        , m_isValidData(true)
        , m_isNew(false)
        , m_isLocationKeyModified(false)
        , m_isAlarmLocationKeyModified(false)
        , m_isAlarmTypeModified(false)
        , m_isSubSystemKeyModified(false)
        , m_isDisplayConditionModified(false)
        , m_isDeletedModified(false)
    {}

    /**
     * getKey
     */
    ta_uint32 AlarmDisplayFilterData::getKey()
    {
        TA_ASSERT(0 != m_pkey, "A newly created data must be written to the database before the key can be retrieved");
        return m_pkey;
    }

    /**
     * getLocationKey
     */
    ta_uint32 AlarmDisplayFilterData::getLocationKey()
    {
        _reload();
        return m_locationKey;
    }

    /**
     * setLocationKey
     */
    void AlarmDisplayFilterData::setLocationKey(const int& _nLocationKey)
    {
        _reload();

        if (m_locationKey != _nLocationKey)
        {
            m_locationKey = _nLocationKey;
            m_isLocationKeyModified = true;
        }
    }

    /**
     * setAlarmLocationKey
     */
    ta_uint32 AlarmDisplayFilterData::getAlarmLocationKey()
    {
        _reload();
        return m_alarmLocationKey;
    }

    /**
     * getAlarmLocationKey
     */
    void AlarmDisplayFilterData::setAlarmLocationKey(const ta_uint32& _nAlarmLocationKey)
    {
        _reload();

        if (m_alarmLocationKey != _nAlarmLocationKey)
        {
            m_alarmLocationKey = _nAlarmLocationKey;
            m_isAlarmLocationKeyModified = true;
        }
    }

    /**
     * getAlarmType
     */
    ta_uint32 AlarmDisplayFilterData::getAlarmType()
    {
        _reload();
        return m_alarmType;
    }

    /**
     * setAlarmType
     */
    void AlarmDisplayFilterData::setAlarmType(const ta_uint32& _nAlarmType)
    {
        _reload();

        if (m_alarmType != _nAlarmType)
        {
            m_alarmType = _nAlarmType;
            m_isAlarmTypeModified = true;
        }
    }

    /**
     * getSubSystemKey
     */
    ta_uint32 AlarmDisplayFilterData::getSubSystemKey()
    {
        _reload();
        return m_subSystemKey;
    }

    /**
     * setSubSystemKey
     */
    void AlarmDisplayFilterData::setSubSystemKey(const ta_uint32& _nSubSystemKey)
    {
        _reload();

        if (m_subSystemKey != _nSubSystemKey)
        {
            m_subSystemKey = _nSubSystemKey;
            m_isSubSystemKeyModified = true;
        }
    }

    /**
     * getAlarmLocationKey
     */
    IAlarmDisplayFilterData::DISPLAY_CONDITION AlarmDisplayFilterData::getDisplayCondition()
    {
        _reload();
        return m_displayCondition;
    }

    /**
     * setDisplayCondition
     */
    void AlarmDisplayFilterData::setDisplayCondition(const DISPLAY_CONDITION& _nDisplayCondition)
    {
        _reload();

        if (m_displayCondition != _nDisplayCondition)
        {
            m_displayCondition = _nDisplayCondition;
            m_isDisplayConditionModified = true;
        }
    }

    /**
     * getDeleted
     */
    ta_uint32 AlarmDisplayFilterData::getDeleted()
    {
        _reload();
        return m_deleted;
    }

    /**
     * setDeleted
     */
    void AlarmDisplayFilterData::setDeleted(const ta_uint32& _isDeleted)
    {
        _reload();

        if (m_deleted != _isDeleted)
        {
            m_deleted = _isDeleted;
            m_isDeletedModified = true;
        }
    }

    /**
     * applyChanges
     */
    void AlarmDisplayFilterData::applyChanges()
    {
        //Todo: Add validation here

        if (m_isNew)
        {
            _createNew();
        }
        else
        {
            _update();
        }
    }

    /**
     * invalidate
     */
    void AlarmDisplayFilterData::invalidate()
    {
        m_isValidData = false;
    }

    void AlarmDisplayFilterData::_reload()
    {
        if (m_isValidData || m_isNew)
        {
            return;
        }

        IDatabase* databaseConnection =
            DatabaseFactory::getInstance().getDatabase(Alarm_Sd, Read);

        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ALARM_DISPLAY_FILTERING_SELECT_84001, m_pkey);

        std::string locationKey = "a.LOCATIONKEY";
        std::string alarmType = "a.ALARMTYPE";
        std::string subsystemKey = "a.SUBSYSTEMKEY";
        std::string alarmLocationKey = "a.ALARMLOCATIONKEY";
        std::string displayCondition = "a.DISPLAYCONDITION";
        std::string deleted = "a.DELETED";

        std::vector<std::string> columnNames;
        columnNames.push_back(locationKey);
        columnNames.push_back(alarmType);
        columnNames.push_back(subsystemKey);
        columnNames.push_back(alarmLocationKey);
        columnNames.push_back(displayCondition);
        columnNames.push_back(deleted);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(sql, columnNames);

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        if (NULL == data || 0 == data->getNumRows()) // No entry found with the specified alarm_id
        {
            delete data;
            data = NULL;
            char reasonMessage[256] = {0};
            sprintf(reasonMessage, "No filter found for pkey = %d", m_pkey);
            throw DataException(reasonMessage, DataException::NO_VALUE, "");
        }
        else if (1 < data->getNumRows()) // More than one entry found for the alarm_id
        {
            delete data;
            data = NULL;
            char reasonMessage[256] = {0};
            sprintf(reasonMessage, "More than one alarm found for pkey = %d", m_pkey);
            throw DataException(reasonMessage, DataException::NOT_UNIQUE, "");
        }

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_locationKey = data->getUnsignedLongData(0, locationKey);
        m_alarmType = data->getUnsignedLongData(0, alarmType);
        m_subSystemKey = data->getUnsignedLongData(0, subsystemKey);
        m_alarmLocationKey = data->getUnsignedLongData(0, alarmLocationKey);
        m_displayCondition = (IAlarmDisplayFilterData::DISPLAY_CONDITION)data->getUnsignedLongData(0, displayCondition);
        m_deleted = data->getUnsignedLongData(0, deleted);

        // Now that we're done with the IData object, it's our responsibility to delete it
        delete data;
        data = NULL;

        // Need to record that we now have valid data
        m_isValidData = true;
        m_isNew = false;

        m_isLocationKeyModified = false;
        m_isAlarmLocationKeyModified = false;
        m_isSubSystemKeyModified = false;
        m_isDisplayConditionModified = false;
        m_isDeletedModified = false;
    }

    void AlarmDisplayFilterData::_createNew()
    {
        FUNCTION_ENTRY("_createNew");

        // Now we can get on with writing the entity to the database.
        // Convert non-string elements to char*'s
        char locationKeyString[50] = {0};
        char alarmTypeString[50] = {0};
        char subSystemString[50] = {0};
        char alarmLocationKeyString[50] = {0};
        char displayConditionString[2] = {0};
        std::string deletedString;

        sprintf(locationKeyString, "%u", m_locationKey);
        sprintf(alarmTypeString, "%u", m_alarmType);
        sprintf(subSystemString, "%u", m_subSystemKey);
        sprintf(alarmLocationKeyString, "%u", m_alarmLocationKey);
        sprintf(displayConditionString, "%d", (int)m_displayCondition);
        m_deleted ? deletedString = "1" : deletedString = "0";

        // Get the database connection
        IDatabase* databaseConnection =
            DatabaseFactory::getInstance().getDatabase(Alarm_Sd, Write);

        SQLStatement formatSQL;
        databaseConnection->prepareSQLStatement(formatSQL, ALARM_DISPLAY_FILTERING_INSERT_84004,
                                                getLocalDatabaseName(), std::string(locationKeyString), std::string(alarmTypeString),
                                                std::string(subSystemString), std::string(alarmLocationKeyString), std::string(displayConditionString),
                                                std::string(deletedString));

        databaseConnection->executeModification(formatSQL);

        //Query the newly created pkey

        m_isLocationKeyModified = false;
        m_isAlarmLocationKeyModified = false;
        m_isSubSystemKeyModified = false;
        m_isDisplayConditionModified = false;
        m_isDeletedModified = false;

        m_isNew = false;
        m_isValidData = true;
        FUNCTION_EXIT;
    }

    void AlarmDisplayFilterData::_update()
    {
        FUNCTION_ENTRY("_update");

        if (true == m_isLocationKeyModified || true == m_isAlarmTypeModified || true == m_isSubSystemKeyModified ||
                true == m_isAlarmLocationKeyModified || true == m_isDisplayConditionModified || true == m_isDeletedModified)
        {   // Data was changed

            // Get the database connection
            IDatabase* databaseConnection =
                DatabaseFactory::getInstance().getDatabase(Alarm_Sd, Write);

            bool firstField = true;
            SQLStatement strSql;
            SQLTypeAdapter sqlParams;

            if (true == m_isLocationKeyModified)
            {
                char locationKeyString[50] = {0};
                sprintf(locationKeyString, "%u", m_locationKey);

                sqlParams.add(std::string("LOCATIONKEY"), std::string(locationKeyString), enumValType_NONSTR, enumOperKey_EQUAL);
                firstField = false;
            }

            if (true == m_isAlarmTypeModified)
            {
                char alarmTypeString[50] = {0};
                sprintf(alarmTypeString, "%u", m_alarmType);

                sqlParams.add(std::string("ALARMTYPE"), std::string(alarmTypeString), enumValType_NONSTR, enumOperKey_EQUAL);
                firstField = false;
            }

            if (true == m_isSubSystemKeyModified)
            {
                char subSystemString[50] = {0};
                sprintf(subSystemString, "%u", m_subSystemKey);

                sqlParams.add(std::string("SUBSYSTEMKEY"), std::string(subSystemString), enumValType_NONSTR, enumOperKey_EQUAL);
                firstField = false;
            }

            if (true == m_isAlarmLocationKeyModified)
            {
                char alarmLocationKeyString[50] = {0};
                sprintf(alarmLocationKeyString, "%u", m_alarmLocationKey);

                sqlParams.add(std::string("ALARMLOCATIONKEY"), std::string(alarmLocationKeyString), enumValType_NONSTR, enumOperKey_EQUAL);
                firstField = false;
            }

            if (true == m_isDisplayConditionModified)
            {
                char displayConditionString[2] = {0};
                sprintf(displayConditionString, "%d", (int)m_displayCondition);

                sqlParams.add(std::string("DISPLAYCONDITION"), std::string(displayConditionString), enumValType_NONSTR, enumOperKey_EQUAL);
                firstField = false;
            }

            if (true == m_isDeletedModified)
            {
                std::string deletedString;
                m_deleted ? deletedString = "1" : deletedString = "0";

                sqlParams.add(std::string("DELETED"), std::string(deletedString), enumValType_NONSTR, enumOperKey_EQUAL);
                firstField = false;
            }

            databaseConnection->prepareSQLStatement(strSql, ALARM_DISPLAY_FILTERING_UPDATE_84005, getLocalDatabaseName(), sqlParams, m_pkey);
            databaseConnection->executeModification(strSql);

            m_isLocationKeyModified = false;
            m_isAlarmLocationKeyModified = false;
            m_isSubSystemKeyModified = false;
            m_isDisplayConditionModified = false;
            m_isDeletedModified = false;

            m_isNew = false;
            m_isValidData = true;
        }

        FUNCTION_EXIT;
    }

    std::string AlarmDisplayFilterData::getLocalDatabaseName()
    {
        if (m_localDatabase.empty())
        {
            // have to find out what is the local db name
            // this should be the first in the connection list so grab it
            if (! RunParams::getInstance().isSet(RPARAM_DBCONNECTIONFILE))
            {
                throw DatabaseException("DbConnectionFile not set");
            }

            try
            {
                std::string strDbConnFile(RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
                m_localDatabase = DatabaseFactory::getInstance().getDatabaseName(strDbConnFile, Alarm_Sd, Write);

                /*DbConnectionStrings connectionStrings(RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
                DataConnections connections = connectionStrings.getConnectionList(Alarm_Sd, Write);
                m_localDatabase = connections[0].first;  */
            }
            catch (...) // most likely a file not found error
            {
                throw DatabaseException("Unable to find database connection infomation");
            }
        }

        return m_localDatabase;
    }
}
