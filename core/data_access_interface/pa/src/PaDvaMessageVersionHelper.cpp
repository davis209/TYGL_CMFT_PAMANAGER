/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TYGL_CMFT_TIP/J155/transactive/core/data_access_interface/pa/src/PaDvaMessageVersionHelper.cpp $
  * @author:  Jade Lee
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2023/10/16 18:31:43 $
  * Last modified by:  $Author: CM $
  *
  * PaDvaMessageVersionHelper is an object that is held by PaDvaMessageVersion and ConfigPaDvaMessageVersion objects. 
  * It contains all data used by PaDvaMessageVersionGroups, and manipulation
  * methods for the data. It helps avoid re-writing code for both PaDvaMessageVersion and ConfigPaDvaMessageVersion.
  *
  */

#pragma warning(disable:4786 4290)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated

#include "core/types/src/ta_types.h"
#include <vector>
#include <algorithm>
#include <ctime>

#include "core/data_access_interface/pa/src/PaDvaMessageVersionHelper.h"
#include "core/data_access_interface/pa/src/PaDvaMessageVersionAccessFactory.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/pa/src/DatabaseQueryHelper.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/exceptions/src/ValueNotSetException.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_Core
{
    static const std::string PA_DVA_MESSAGE_VERSION_TABLE_NAME = "PA_DVA_MESSAGE_VERSION";
    static const std::string PA_DVA_MESSAGE_VERSION_TABLE_SHORT_NAME = "PADMVE";
    static const std::string KEY_COL = "PADMVE_ID";

    static const std::string LOCATIONKEY_COL = "LOCATIONKEY";

    // NOTE: These must have same order as EVersionColumn enum
    static const std::string VERSION_COLUMNS[] = 
    {
        "PUBLIC_ADHOC1",
        "PUBLIC_ADHOC2",
        "PUBLIC_ADHOC3",
        "PUBLIC_ADHOC4",
        "PUBLIC_ADHOC5",
        "PUBLIC_ADHOC6",
        "PUBLIC_PRERECORDED",
        "PRIVATE_ADHOC1",
        "PRIVATE_ADHOC2",
        "PRIVATE_ADHOC3",
        "PRIVATE_ADHOC4",
        "PRIVATE_ADHOC5",
        "PRIVATE_ADHOC6",
        "PRIVATE_PRERECORDED"
    };
    
    static const std::string DATEMODIFIED_COL = "DATE_MODIFIED";
    static const std::string DATECREATED_COL = "DATE_CREATED";

	PaDvaMessageVersionHelper::PaDvaMessageVersionHelper( const PaDvaMessageVersionHelper& thePaDvaMessageVersionHelper)
		: // m_key(),   // Don't set the key upon copy..
          m_locationKey(thePaDvaMessageVersionHelper.m_locationKey),
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(true)
    {	
        for (int i = 0; i < IPaDvaMessageVersion::VERSION_COLUMN_MAX; i ++)
        {
            m_versionData[i] = thePaDvaMessageVersionHelper.m_versionData[i];
        }
	}


    PaDvaMessageVersionHelper::PaDvaMessageVersionHelper(const ta_uint32 key)
        : m_key(key),	
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(false)
    {
        for (int i = 0; i < IPaDvaMessageVersion::VERSION_COLUMN_MAX; i ++)
        {
            m_versionData[i] = "";
        }
    }


    PaDvaMessageVersionHelper::PaDvaMessageVersionHelper(ta_uint32 row, TA_Base_Core::IData& data)
        : m_key(data.getUnsignedLongData(row, KEY_COL)),	
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(false)
    {
        for (int i = 0; i < IPaDvaMessageVersion::VERSION_COLUMN_MAX; i ++)
        {
            m_versionData[i] = "";
        }
        
        reloadUsing(row, data);
    }


    PaDvaMessageVersionHelper::PaDvaMessageVersionHelper()
        : m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(true)
    {
        for (int i = 0; i < IPaDvaMessageVersion::VERSION_COLUMN_MAX; i ++)
        {
            m_versionData[i] = "";
        }
    }


    PaDvaMessageVersionHelper::~PaDvaMessageVersionHelper()
    {
    }
  

    ta_uint32 PaDvaMessageVersionHelper::getKey()
    {
        FUNCTION_ENTRY("getKey" );

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        
        if (!m_key.hasBeenSet())
        {
            TA_THROW(TA_Base_Core::DataException("Key not set",  
                            TA_Base_Core::DataException::NO_VALUE, "Primary key"));
        }        

        FUNCTION_EXIT;
        return m_key.getValue();
    }


    std::string PaDvaMessageVersionHelper::getMessageVersionUsingParams(
                                ta_uint32 stationDvaMessageId,
                                ta_uint32 startRange,
                                ta_uint32 endRange,
                                EBasicSet set)
    {
        TA_ASSERT(stationDvaMessageId > startRange, "Message out of range");
        TA_ASSERT(stationDvaMessageId < endRange, "Message out of range");

        // There are 6 adhoc messages
        const unsigned int adhocUpperRange = 6;

        if (stationDvaMessageId < startRange + adhocUpperRange)
        {
            // This is an ad-hoc message, determine offset from first
            ta_uint32 offset = stationDvaMessageId - startRange;

            if (PRIVATE == set)
            {
                return getVersionData(IPaDvaMessageVersion::EVersionColumn
                                      (IPaDvaMessageVersion::PRIVATE_ADHOC1 + offset));
            }
            else
            {
                return getVersionData(IPaDvaMessageVersion::EVersionColumn
                                      (IPaDvaMessageVersion::PUBLIC_ADHOC1 + offset));
            }
        }
        else
        {
            if (PRIVATE == set)
            {
                return getVersionData(IPaDvaMessageVersion::EVersionColumn
                                      (IPaDvaMessageVersion::PRIVATE_PRERECORDED));
            }
            else
            {
                return getVersionData(IPaDvaMessageVersion::EVersionColumn
                                      (IPaDvaMessageVersion::PUBLIC_PRERECORDED));
            }
        }
    }


    std::string PaDvaMessageVersionHelper::getMessageVersion(
                                            IPaDvaMessageVersion::ELocation location,
                                            ta_uint32 stationDvaMessageId)
    {
        switch (location)
        {
        case IPaDvaMessageVersion::DEPOT:
            // Depots only have private sets [Set D 1-500]
            return getMessageVersionUsingParams(stationDvaMessageId, 1, 500, PRIVATE);
        case IPaDvaMessageVersion::STATION:
            // Stations have public [Set A 1->500]
            // and private [Set B 501->1000]
            if (stationDvaMessageId <= 500)
            {
                return getMessageVersionUsingParams(stationDvaMessageId, 1, 500, PUBLIC);
            }
            else
            {
                return getMessageVersionUsingParams(stationDvaMessageId, 501, 1000, PRIVATE);
            }            
        case IPaDvaMessageVersion::OCC:
            return getMessageVersionUsingParams(stationDvaMessageId, 1, 500, PUBLIC);
        default:
            TA_ASSERT(false, "Unrecognized location");
            return "";
        }   
    }


    std::string PaDvaMessageVersionHelper::getVersionData(
        IPaDvaMessageVersion::EVersionColumn column)
    {
        TA_ASSERT(column < IPaDvaMessageVersion::VERSION_COLUMN_MAX, "Out of range");

        FUNCTION_ENTRY("getVersionData" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;

        return m_versionData[column];        
    }


    ta_uint32 PaDvaMessageVersionHelper::getLocationKey()
    {
        FUNCTION_ENTRY("getLocationKey" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;

        // Following the reload of data, all values should have been set
        try
        {
            return m_locationKey.getValue();
        }
        catch (::TA_Base_Core::ValueNotSetException&)
        {
            // reload() should have either thrown exception or set this value
            TA_ASSERT(false, "m_locationKey value not set after reload");
            throw;
        }
    }


    void PaDvaMessageVersionHelper::setLocationKey(const ta_uint32 locationKey)
    {
        FUNCTION_ENTRY("setLocationKey" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_locationKey.setValue(locationKey);
        
        FUNCTION_EXIT;
    }


    void PaDvaMessageVersionHelper::setVersionData(
            const std::string& newVersion,
            IPaDvaMessageVersion::EVersionColumn column)
    {
        TA_ASSERT(column < IPaDvaMessageVersion::VERSION_COLUMN_MAX, "Out of range");

        FUNCTION_ENTRY("setVersionData" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        m_versionData[column] = newVersion;
        
        FUNCTION_EXIT;
    }


    time_t PaDvaMessageVersionHelper::getDateCreated()
    {
        FUNCTION_ENTRY("getDateCreated" );
        TA_ASSERT(!m_isNew,"The data must be written to the database before the date modified can be retrieved");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_dateCreated;
    }


    time_t PaDvaMessageVersionHelper::getDateModified()
    {
        FUNCTION_ENTRY("getDateModified" );
        TA_ASSERT(!m_isNew,"The data must be written to the database before the date modified can be retrieved");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        FUNCTION_EXIT;
        return m_dateModified;
    }



    void PaDvaMessageVersionHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate" );

        // This method cannot be called until the key has been set
        TA_ASSERT(!m_isNew,"Attempted to call invalidate() on a new PaDvaMessageVersion");

        m_isValidData = false;

        FUNCTION_EXIT;
    }

    std::string PaDvaMessageVersionHelper::getVersionDataColumns()
    {
        std::ostringstream result;

        for (int i = 0; i < IPaDvaMessageVersion::VERSION_COLUMN_MAX; i ++)
        {
            result << VERSION_COLUMNS[i];
            if (i < (IPaDvaMessageVersion::VERSION_COLUMN_MAX - 1))
            {
                // Add commas for all items except last
                result << ", ";
            }
        }

        return result.str();
    }


    const DatabaseQueryHelper::QueryData 
        PaDvaMessageVersionHelper::getBasicQueryData()
    {
        DatabaseQueryHelper::QueryData result;

        result.primaryKeyCol    = KEY_COL;
	    result.tableName        = PA_DVA_MESSAGE_VERSION_TABLE_NAME;

        result.columnNames.push_back(KEY_COL);
        result.columnNames.push_back(LOCATIONKEY_COL);

        int i;

        for (i = 0; i < IPaDvaMessageVersion::VERSION_COLUMN_MAX; i ++)
        {
            result.columnNames.push_back(VERSION_COLUMNS[i]);
        }

        result.columnNames.push_back(DATECREATED_COL);
        result.columnNames.push_back(DATEMODIFIED_COL);

        result.statement = 
            DatabaseQueryHelper::createBasicSelectStatement(result.columnNames, 
                                                            result.tableName);        
        return result;
    }


    void PaDvaMessageVersionHelper::reloadUsing(ta_uint32 row, TA_Base_Core::IData& data)
    {
        FUNCTION_ENTRY("reloadUsing" );
        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_locationKey.setValue(data.getUnsignedLongData( row, LOCATIONKEY_COL ));
    
        int i;
        for (i = 0; i < IPaDvaMessageVersion::VERSION_COLUMN_MAX; i ++)
        {
            m_versionData[i] = data.getStringData( row, VERSION_COLUMNS[i]);
        }

        m_dateCreated = data.getDateData( row, DATECREATED_COL );

        try
        {
            m_dateModified = data.getDateData( row, DATEMODIFIED_COL );
            struct tm newtm;

            // Set up the date created structure to equal the value we set it to when we get NULL
            // from the database
		    newtm.tm_sec = 0;		//seconds
		    newtm.tm_min = 0;		// minutes 
		    newtm.tm_hour = 12;		// hours 
		    newtm.tm_mday = 1;		// day of the month 
		    newtm.tm_mon = 0;	// month of Year - goes from 0 to 11
		    newtm.tm_year = 90;	// years since 1900 - this structure only deals with years 
									    // between 1901 and 2038
		    newtm.tm_isdst = 0;			// disable daylight savings - this could have implications

		    time_t value;
		    value = mktime(&newtm);

            if (value == m_dateModified) // The date was actually NULL
            {
                m_dateModified = 0;
            }
        }
        catch (TA_Base_Core::DataException& ex)
        {
            //If we catch this exception, need to check if its a "NO_VALUE" type
            if (TA_Base_Core::DataException::NO_VALUE != ex.getFailType())
            {   // Need to on-throw this exception
                throw;
            }
            else // The date-modified column was null - this is not fatal
            {
                m_dateModified = 0;
            }
        }
        
        // Need to record that we now have valid data
        m_isValidData = true;
        FUNCTION_EXIT;
    }


    void PaDvaMessageVersionHelper::writePaDvaMessageVersionData()
    {       
        FUNCTION_ENTRY( "writePaDvaMessageVersionData" );     

        // Check ALL values exist. If not add them to the dataConfiguratonException
        std::vector<std::string> fieldNames;        
        if (!m_locationKey.hasBeenSet())
        {
            fieldNames.push_back(LOCATIONKEY_COL);
        }
        
        /*
        // Can have empty version data values ""
        int i;
        for (i = 0; i < IPaDvaMessageVersion::VERSION_COLUMN_MAX; i ++)
        {
            if (m_versionData[i].length() == 0)
            {
                fieldNames.push_back(VERSION_COLUMNS[i]);
            }
        }*/

        if ( !fieldNames.empty() ) // Need to throw an exception
        {
            TA_THROW(TA_Base_Core::DataConfigurationException("PaDvaMessageVersion data not fully specified. PaDvaMessageVersion cannot be written to database",
                                                         fieldNames));
        }
        
        // Write the PaDvaMessageVersion to the database

        try 
        {
            // Need to check if this is a new PaDvaMessageVersion or an existing PaDvaMessageVersion - 
            // as a new PaDvaMessageVersion will be inserted, while an existing PaDvaMessageVersion will be updated
            if (m_isNew) // This is a new PaDvaMessageVersion
            {
                addNewPaDvaMessageVersion();
            }
            else // This is an existing PaDvaMessageVersion
            {
                modifyExistingPaDvaMessageVersion();
            }
        }
        catch (TA_Base_Core::ValueNotSetException&)
        {
            // Should not get here if properly validated set state of fields before calling
            // statements in the try block
            std::vector<std::string> dummyNames;    // Just some empty names, to allow exception construction

            TA_THROW(TA_Base_Core::DataConfigurationException(
                        "PaDvaMessageVersion data not fully specified. PaDvaMessageVersion cannot be written to database",
                        dummyNames));
        }
        
        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        m_isNew = false;

        FUNCTION_EXIT;
    }
   

    void PaDvaMessageVersionHelper::deletePaDvaMessageVersion()
    {
        FUNCTION_ENTRY("deletePaDvaMessageVersion");

        TA_ASSERT(!m_isNew, "This PaDvaMessageVersion does not yet exist in the database, and therefore cannot be deleted");
        
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Pa_Cd, TA_Base_Core::Write);
        std::ostringstream sql;        

        // Good to go...
        sql.str("");
        sql << "delete " << PA_DVA_MESSAGE_VERSION_TABLE_NAME << " where " << KEY_COL << " = " << m_key.getValue();

        databaseConnection->executeModification(sql.str());

        FUNCTION_EXIT;
    }


    void PaDvaMessageVersionHelper::modifyExistingPaDvaMessageVersion()
    {
        FUNCTION_ENTRY("modifyExistingPaDvaMessageVersion");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
            "PA DVA Message Version %u already exists. It's data will be updated.", m_key.getValue());


        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////
        TA_Base_Core::IDatabase *databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Pa_Cd, TA_Base_Core::Write);
        std::ostringstream sql;

        sql << "update " << PA_DVA_MESSAGE_VERSION_TABLE_NAME << " set " 
            << LOCATIONKEY_COL << " = "
            << m_locationKey.getValue()
            << ", ";

        for (int i = 0; i < IPaDvaMessageVersion::VERSION_COLUMN_MAX; i ++)
        {
            sql << VERSION_COLUMNS[i] << " = '" 
                << (databaseConnection->escapeInsertString(m_versionData[i])).c_str()
                << "'";

            if (i < (IPaDvaMessageVersion::VERSION_COLUMN_MAX - 1))
            {
                // Don't append comma on last entry
                sql << ", ";
            }
        }

        sql << " where " << KEY_COL << " = " << m_key.getValue();        

        databaseConnection->executeModification(sql.str().c_str());

        // now need to update the date fields
        sql.str("");
        sql << "select TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS'), "
            << "TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS') "
            << "from " << PA_DVA_MESSAGE_VERSION_TABLE_NAME << 
            " where " << KEY_COL << " = " << m_key.getValue();

        std::vector<std::string> columnNames;
        columnNames.push_back(DATECREATED_COL);
        columnNames.push_back(DATEMODIFIED_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(sql.str(), columnNames);

        if (0 == data->getNumRows()) 
        {
            // PA DVA Message Version record not found
            
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for PA DVA Message Version key = " << m_key.getValue();			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"PA DVA Message Version key" ) );
        }

        try
        {
            m_dateCreated = data->getDateData(0, DATECREATED_COL);
            m_dateModified = data->getDateData(0, DATEMODIFIED_COL);
        }
        catch ( ... )
        {
            delete data;
            data = NULL;
            throw;
        }

        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        // nothing more to be done here
        FUNCTION_EXIT;
    }


    void PaDvaMessageVersionHelper::addNewPaDvaMessageVersion() 
    {
        FUNCTION_ENTRY("addNewPaDvaMessageVersion");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
            "PA DVA Message Version %u is a new PaDvaMessageVersion. It will be added to the database.", m_key.getValue());


        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Pa_Cd, TA_Base_Core::Write);

        std::ostringstream sql;
        sql << "insert into " << PA_DVA_MESSAGE_VERSION_TABLE_NAME << " (" 
            << KEY_COL << ", "
            << LOCATIONKEY_COL << ", "
            << getVersionDataColumns() << ") "
            << "values (" << m_key.getValue() << ","
            << m_locationKey.getValue() << ", ";

        for (int i = 0; i < IPaDvaMessageVersion::VERSION_COLUMN_MAX; i ++)
        {
            sql << "'" << 
                (databaseConnection->escapeInsertString(m_versionData[i])).c_str()
                << "'";

            if (i < (IPaDvaMessageVersion::VERSION_COLUMN_MAX - 1))
            {
                sql << ", ";
            }
            else
            {
                sql << ")";
            }
        }

        databaseConnection->executeModification(sql.str());

        sql.str("");
        sql << "select " 
            << KEY_COL << ", "
            << "TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') "
            << "from " << PA_DVA_MESSAGE_VERSION_TABLE_NAME << " where "
            << KEY_COL << " = " << m_key.getValue();

        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL);
        columnNames.push_back(DATECREATED_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(sql.str(),columnNames);

        if (0 == data->getNumRows()) 
        {
            // PA DVA Message Version record not found            
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for PA DVA Message Version key = " << m_key.getValue();			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"PA DVA Message Version key" ) );
        }

        // Retrieve the pkey as an ta_uint32 . There should only be a single returned item 
        // (as any other circumstance is specifically prevented by the above checks).
        // This method can throw a DataException
        try
        {
            m_key.setValue(data->getUnsignedLongData(0,KEY_COL));
            m_dateCreated = data->getDateData(0,DATECREATED_COL);
        }
        catch ( ... )
        {
            delete data;
            data = NULL;
            throw;
        }

        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        FUNCTION_EXIT;
    }
} // closes TA_IRS_Core

