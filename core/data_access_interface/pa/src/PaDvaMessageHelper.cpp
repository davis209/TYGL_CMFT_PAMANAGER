/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TYGL_CMFT_TIP/J155/transactive/core/data_access_interface/pa/src/PaDvaMessageHelper.cpp $
  * @author:  Jade Lee
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2023/10/16 18:31:43 $
  * Last modified by:  $Author: CM $
  *
  * PaDvaMessageHelper is an object that is held by PaDvaMessage and ConfigPaDvaMessage objects. 
  * It contains all data used by PaDvaMessages, and manipulation
  * methods for the data. It helps avoid re-writing code for both PaDvaMessage and ConfigPaDvaMessage.
  */


#pragma warning(disable:4786 4290)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated

#include "core/types/src/ta_types.h"
#include <vector>
#include <algorithm>
#include <ctime>

#include "core/data_access_interface/pa/src/PaDvaMessageHelper.h"
#include "core/data_access_interface/pa/src/DatabaseQueryHelper.h"
#include "core/data_access_interface/pa/src/PaDvaMessageAccessFactory.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/exceptions/src/DataException.h"
#include "core/data_access_interface/src/DatabaseKey.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/exceptions/src/ValueNotSetException.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_Core
{
    static const std::string PA_DVA_MESSAGE_TABLE_NAME = "PA_DVA_MESSAGE";
    static const std::string PA_DVA_MESSAGE_TABLE_SHORT_NAME = "PADMES";
    static const std::string KEY_COL = "PADMES_ID";
    static const std::string PA_DVA_MESSAGE_SEQUENCE_COUNTER = "PADMES_SEQ";

    static const std::string LOCATIONKEY_COL = "LOCATIONKEY";
    static const std::string ID_COL = "ID";
    static const std::string LABEL_COL = "LABEL";
    static const std::string TYPE_COL = "TYPE";
    
    static const std::string TISMESSAGETAG_COL = "TIS_MESSAGE_TAG";
    static const std::string TISLIBRARYSECTION_COL = "TIS_LIBRARY_SECTION";  
    static const std::string TISLIBRARYVERSION_COL = "TIS_LIBRARY_VERSION";

    static const std::string DATEMODIFIED_COL = "DATE_MODIFIED";
    static const std::string DATECREATED_COL = "DATE_CREATED";

	PaDvaMessageHelper::PaDvaMessageHelper( const PaDvaMessageHelper& thePaDvaMessageHelper)
		: m_key(TA_Base_Core::DatabaseKey::getInvalidKey()),
		  m_locationKey(thePaDvaMessageHelper.m_locationKey),          
          m_id(thePaDvaMessageHelper.m_id),       
          m_label(thePaDvaMessageHelper.m_label),
          m_type(thePaDvaMessageHelper.m_type),
          m_tisMessageTag(thePaDvaMessageHelper.m_tisMessageTag),
          m_tisLibrarySection(thePaDvaMessageHelper.m_tisLibrarySection),
          m_tisLibraryVersion(thePaDvaMessageHelper.m_tisLibraryVersion),
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(true)
    {	  
	}


    PaDvaMessageHelper::PaDvaMessageHelper(const ta_uint32 key)
        : m_key(key),		  
          m_label(""),
          m_type(""),
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(false)
    {
    }


    PaDvaMessageHelper::PaDvaMessageHelper(ta_uint32 row, TA_Base_Core::IData& data)
        : m_key(data.getUnsignedLongData(row, KEY_COL)),		  
          m_label(""),
          m_type(""),
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(false)
    {
        reloadUsing(row, data);
    }


    PaDvaMessageHelper::PaDvaMessageHelper()
        : m_key(TA_Base_Core::DatabaseKey::getInvalidKey()),
		  m_label(""),
          m_type(""),
          m_dateCreated(0),
          m_dateModified(0),
          m_isValidData(false),
          m_isNew(true)
    {
    }


    PaDvaMessageHelper::~PaDvaMessageHelper()
    {
    }
  

    ta_uint32 PaDvaMessageHelper::getKey()
    {
        FUNCTION_ENTRY("getKey" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_key;
    }

    ta_uint32 PaDvaMessageHelper::getLocationKey()
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
            if (isNew() && !m_locationKey.hasBeenSet())
            {
                // 4669FAT TD #782 - if this is a new record that hasn't been saved yet, return a default
                // value for an unset key (otherwise, if loaded from DB, it must be set)
                return TA_Base_Core::DatabaseKey::getInvalidKey();                
            }

            return m_locationKey.getValue();
        }
        catch (::TA_Base_Core::ValueNotSetException&)
        {
            // reload() should have either thrown exception or set this value
            TA_ASSERT(false, "m_locationKey value not set after reload");
            throw;
        }
    }

    void PaDvaMessageHelper::setLocationKey(const ta_uint32 locationKey)
    {
        FUNCTION_ENTRY("setLocationKey" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_locationKey.setValue(locationKey);
        
        FUNCTION_EXIT;
    }

    ta_uint32 PaDvaMessageHelper::getId()
    {
        FUNCTION_ENTRY("getId" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        
        FUNCTION_EXIT;

        try 
        {
            if (isNew() && !m_id.hasBeenSet())
            {
                // 4669FAT TD #782 - if this is a new record that hasn't been saved yet, return a default
                // value for an unset key (otherwise, if loaded from DB, it must be set)
                return TA_Base_Core::DatabaseKey::getInvalidKey();                
            }

            return m_id.getValue();
        }
        catch (TA_Base_Core::ValueNotSetException&)
        {
            // reload() should have either thrown exception or set this value
            TA_ASSERT(false, "m_id value not set after reload");
            throw;
        }
    }


    void PaDvaMessageHelper::setId(const ta_uint32 id)
    {
        FUNCTION_ENTRY("setId" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_id.setValue(id);
        FUNCTION_EXIT;
    }


    std::string PaDvaMessageHelper::getLabel()
    {
        FUNCTION_ENTRY("getLabel" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_label;
    }


    void PaDvaMessageHelper::setLabel(const std::string& label)
    {
        FUNCTION_ENTRY("setLabel" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_label = label;
        FUNCTION_EXIT;
    }

    std::string PaDvaMessageHelper::getType()
    {
        FUNCTION_ENTRY("getType" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_type;
    }


    void PaDvaMessageHelper::setType(const std::string& type)
    {
        FUNCTION_ENTRY("setType" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_type = type;
        FUNCTION_EXIT;
    }

    ta_uint32 PaDvaMessageHelper::getTisLibraryVersion()
    {
        FUNCTION_ENTRY("getTisLibraryVersion");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        try
        {
            if ( !m_tisLibraryVersion.hasBeenSet() ) 
            {
                // If the value is unset (a valid state), return -1
                return (ta_uint32 )-1;
            }

            return m_tisLibraryVersion.getValue();
        }
        catch (const TA_Base_Core::ValueNotSetException&)
        {
            // reload() should have either thrown exception or set this value
            TA_ASSERT(false, "m_tisLibraryVersion value not set after reload");
            throw;
        }
    }


    ta_uint32 PaDvaMessageHelper::getTisMessageTag()
    {
        FUNCTION_ENTRY("getTisMessageTag" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        try
        {
            if ( !m_tisMessageTag.hasBeenSet() ) 
            {
                // If the value is unset (a valid state), return -1
                return (ta_uint32 )-1;
            }

            return m_tisMessageTag.getValue();
        }
        catch (const TA_Base_Core::ValueNotSetException&)
        {
            // reload() should have either thrown exception or set this value
            TA_ASSERT(false, "m_tisMessageTag value not set after reload");
            throw;
        }

        FUNCTION_EXIT;
    }
    

    ta_uint32 PaDvaMessageHelper::getTisLibrarySection()
    {
        FUNCTION_ENTRY("getTisLibrarySection" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        try
        {
            if ( !m_tisLibrarySection.hasBeenSet() ) 
            {
                // If the value is unset (a valid state), return -1
                return (ta_uint32 )-1;
            }

            return m_tisLibrarySection.getValue();
        }
        catch (const TA_Base_Core::ValueNotSetException&)
        {
            // reload() should have either thrown exception or set this value
            TA_ASSERT(false, "m_tisLibrarySection value not set after reload");
            throw;
        }

        FUNCTION_EXIT;
    }


    void PaDvaMessageHelper::setTisLibraryVersion(ta_uint32 version)
    {
        FUNCTION_ENTRY("setTisLibraryVersion");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_tisLibraryVersion.setValue(version);
        FUNCTION_EXIT;
    }    


    void PaDvaMessageHelper::setTisMessageTag(const ta_uint32 tisMessageTag)
    {
        FUNCTION_ENTRY("setTisMessageTag" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_tisMessageTag.setValue(tisMessageTag);
        FUNCTION_EXIT;
    }


    void PaDvaMessageHelper::setTisLibrarySection(const ta_uint32 tisLibrarySection)
    {
        FUNCTION_ENTRY("setTisLibrarySection" );
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_tisLibrarySection.setValue(tisLibrarySection);
        FUNCTION_EXIT;
    }


    time_t PaDvaMessageHelper::getDateCreated()
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


    time_t PaDvaMessageHelper::getDateModified()
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



    void PaDvaMessageHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate" );

        // This method cannot be called until the key has been set
        TA_ASSERT(!m_isNew,"Attempted to call invalidate() on a new PaDvaMessage");

        m_isValidData = false;

        FUNCTION_EXIT;
    }


    const DatabaseQueryHelper::QueryData 
        PaDvaMessageHelper::getBasicQueryData()
    {
        DatabaseQueryHelper::QueryData result;

        result.primaryKeyCol    = KEY_COL;
	    result.tableName        = PA_DVA_MESSAGE_TABLE_NAME;

        result.columnNames.push_back(KEY_COL);
        result.columnNames.push_back(LOCATIONKEY_COL);
        result.columnNames.push_back(ID_COL);
		result.columnNames.push_back(LABEL_COL);
		result.columnNames.push_back(TYPE_COL);
		result.columnNames.push_back(TISMESSAGETAG_COL);
        result.columnNames.push_back(TISLIBRARYSECTION_COL);
        result.columnNames.push_back(TISLIBRARYVERSION_COL);
        result.columnNames.push_back(DATECREATED_COL);
        result.columnNames.push_back(DATEMODIFIED_COL);

        result.statement = 
            DatabaseQueryHelper::createBasicSelectStatement(result.columnNames, 
                                                            result.tableName);

        return result;
    }


    void PaDvaMessageHelper::reloadUsing(ta_uint32 row, TA_Base_Core::IData& data)
    {
        FUNCTION_ENTRY("reloadUsing");

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_locationKey.setValue(data.getUnsignedLongData( row, LOCATIONKEY_COL ));
        m_id.setValue(data.getUnsignedLongData( row, ID_COL ));
		m_label   = data.getStringData( row, LABEL_COL );
		m_type       = data.getStringData( row, TYPE_COL );
        
        if (data.isNull( row, TISMESSAGETAG_COL))            
        {
            m_tisMessageTag.resetValue();
        }
        else
        {
            m_tisMessageTag.setValue(data.getUnsignedLongData( row, TISMESSAGETAG_COL ));
        }
            
        if (data.isNull( row, TISLIBRARYSECTION_COL))            
        {
            m_tisLibrarySection.resetValue();
        }
        else
        {
            m_tisLibrarySection.setValue(data.getUnsignedLongData( row, TISLIBRARYSECTION_COL ));
        }
    
        
        if (data.isNull( row, TISLIBRARYVERSION_COL ) )
        {
            m_tisLibraryVersion.resetValue();
        }
        else
        {
            m_tisLibraryVersion.setValue(
                    data.getUnsignedLongData (row, TISLIBRARYVERSION_COL ));
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


    void PaDvaMessageHelper::writePaDvaMessageData()
    {       
        FUNCTION_ENTRY( "writePaDvaMessageData" );     

        // Check ALL values exist. If not add them to the dataConfiguratonException
        std::vector<std::string> fieldNames;
        if (!m_locationKey.hasBeenSet())
        {
            fieldNames.push_back(LOCATIONKEY_COL);
        }
        if (!m_id.hasBeenSet())
        {
            fieldNames.push_back(ID_COL);
        }
        if (m_type.empty())
        {
            fieldNames.push_back(TYPE_COL);
        }

        // These properties can be unset, 
        // translated to a value of -1 when stored in database
        // TISMESSAGETAG_COL, TISLIBRARYSECTION_COL, TISLIBRARYVERSION_COL
        
        
        if ( !fieldNames.empty() ) // Need to throw an exception
        {
            TA_THROW(TA_Base_Core::DataConfigurationException(
                "PaDvaMessage data not fully specified. PaDvaMessage cannot be written to database",
                fieldNames));
        }

        validateIdLocationPairUniqueness();

        // Write the PaDvaMessage to the database

        try 
        {
            // Need to check if this is a new PaDvaMessage or an existing PaDvaMessage - 
            // as a new PaDvaMessage will be inserted, while an existing PaDvaMessage will be updated
            if (m_isNew) // This is a new PaDvaMessage
            {
                addNewPaDvaMessage();
            }
            else // This is an existing PaDvaMessage
            {
                modifyExistingPaDvaMessage();
            }
        }
        catch (TA_Base_Core::ValueNotSetException&)
        {
            // Should not get here if properly validated set state of fields before calling
            // statements in the try block
            std::vector<std::string> dummyNames;    // Just some empty names, to allow exception construction

            TA_THROW(TA_Base_Core::DataConfigurationException(
                        "PaDvaMessage data not fully specified. PaDvaMessage cannot be written to database",
                        dummyNames));
        }
        
        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        m_isNew = false;

        FUNCTION_EXIT;
    }


    void PaDvaMessageHelper::validateIdLocationPairUniqueness() 
    {   
        std::ostringstream sql;
        sql << "select " << KEY_COL
            << " from " << PA_DVA_MESSAGE_TABLE_NAME 
            << " where " << LOCATIONKEY_COL << " = " << m_locationKey.getValue()
            << " and " << ID_COL << " = " << m_id.getValue();

        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL);
                  
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Pa_Cd, TA_Base_Core::Read);        
        TA_Base_Core::IData* data = databaseConnection->executeQuery(sql.str(), columnNames);

        if (0 != data->getNumRows()) 
        {        
            ta_uint32 duplicateItemKey = data->getUnsignedLongData(0, KEY_COL);

            // Ignore ourself..
            if (duplicateItemKey != m_key)
            {
                std::ostringstream str;
                str << "Attempting to write Pa Dva Message record that doesn't "
                    << "have unique id/location (existing duplicate has pKey="
                    << duplicateItemKey
                    << ")";
    

                delete data;
                data = NULL;

                TA_THROW( TA_Base_Core::DataException(str.str().c_str(), 
                            TA_Base_Core::DataException::NOT_UNIQUE,
                            "LocationKey and Id") );
            }
        }

        delete data;
        data = NULL;

    }


    void PaDvaMessageHelper::deletePaDvaMessage(bool cascade)
    {
        FUNCTION_ENTRY("deletePaDvaMessage");

        TA_ASSERT(!m_isNew, "This PaDvaMessage does not yet exist in the database, and therefore cannot be deleted");
        
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Pa_Cd, TA_Base_Core::Write);
        std::ostringstream sql;
        
        if (!cascade)
        {
            std::string info;
            
            // If don't want cascade, must check for foreign links
            if (DatabaseQueryHelper::checkIfExistLinksToTableRow(
                                            PA_DVA_MESSAGE_TABLE_NAME,
                                            KEY_COL,
                                            m_key,
                                            info))
            {
                TA_THROW(TA_Base_Core::DataException(info.c_str(), 
                                    TA_Base_Core::DataException::CANNOT_DELETE, 
                                    PA_DVA_MESSAGE_TABLE_NAME.c_str()));
            }
        }

        // Good to go...
        sql.str("");
        sql << "delete " << PA_DVA_MESSAGE_TABLE_NAME << " where " << KEY_COL << " = " << m_key;

        databaseConnection->executeModification(sql.str());

        FUNCTION_EXIT;
    }
   

    void PaDvaMessageHelper::modifyExistingPaDvaMessage()
    {
        FUNCTION_ENTRY("modifyExistingPaDvaMessage");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
            "PA Zone %u already exists. It's data will be updated.", m_key);

        validateLocationKeyExistence();

        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////
        TA_Base_Core::IDatabase *databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Pa_Cd, TA_Base_Core::Write);
        std::ostringstream sql;

        sql << "update " << PA_DVA_MESSAGE_TABLE_NAME << " set " 
            << LOCATIONKEY_COL << " = " << m_locationKey.getValue() << ", "
            << ID_COL << " = " << m_id.getValue() << ", "
            << LABEL_COL << " = '"
            << (databaseConnection->escapeInsertString(m_label)).c_str() << "', " 
            << TYPE_COL << " = '"
            << (databaseConnection->escapeInsertString(m_type)).c_str() << "', " 
            << TISMESSAGETAG_COL << " = " << getTisMessageTagDatabaseRepresentation() << ", " 
            << TISLIBRARYSECTION_COL << " = " << getTisLibrarySectionDatabaseRepresentation() << ", " 
            << TISLIBRARYVERSION_COL << " = " << getTisLibraryVersionDatabaseRepresentation() << " " 
            << "where " << KEY_COL << " = " << m_key;        

        databaseConnection->executeModification(sql.str().c_str());

        // now need to update the date fields
        sql.str("");
        sql << "select TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS'), "
            << "TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS') "
            << "from " << PA_DVA_MESSAGE_TABLE_NAME << " where " << KEY_COL << " = " << m_key;

        std::vector<std::string> columnNames;
        columnNames.push_back(DATECREATED_COL);
        columnNames.push_back(DATEMODIFIED_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(sql.str(), columnNames);

        if (0 == data->getNumRows()) 
        {
            // PA Zone record not found
            
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for PA Zone key = " << m_key;			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"PA Zone key" ) );
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


    std::string PaDvaMessageHelper::getTisLibraryVersionDatabaseRepresentation()
    {
        if ( m_tisLibraryVersion.hasBeenSet() )
        {
            // This internal field is valid so simply use that value
            std::ostringstream buf;

            buf << m_tisLibraryVersion.getValue();
            return buf.str();
        } 
        else
        {
            return "null";
        }
    }


    std::string PaDvaMessageHelper::getTisMessageTagDatabaseRepresentation()
    {
        if ( m_tisMessageTag.hasBeenSet() )
        {
            // This internal field is valid so simply use that value
            std::ostringstream buf;

            buf << m_tisMessageTag.getValue();
            return buf.str();
        } 
        else
        {
            return "null";
        }
    }


    std::string PaDvaMessageHelper::getTisLibrarySectionDatabaseRepresentation()
    {
        if ( m_tisLibrarySection.hasBeenSet() )
        {
            // This internal field is valid so simply use that value
            std::ostringstream buf;

            buf << m_tisLibrarySection.getValue();
            return buf.str();
        } 
        else
        {
            return "null";
        }
    }


    void PaDvaMessageHelper::addNewPaDvaMessage()
    {
        FUNCTION_ENTRY("addNewPaDvaMessage");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
            "Station Mode %u is a new PaDvaMessage. It will be added to the database.",m_locationKey.getValue());

        validateLocationKeyExistence();


        ////////////////////////////////////////////////////////////////////////////
        //
        // Good to go...
        //
        ////////////////////////////////////////////////////////////////////////////
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Pa_Cd, TA_Base_Core::Write);
        // Obtain the primary key that should be used for this next entry
        ta_uint32 sequenceNumber = DatabaseQueryHelper::getNextSequenceNumber(
                                                                databaseConnection, 
                                                                PA_DVA_MESSAGE_SEQUENCE_COUNTER);

        std::ostringstream sql;
        sql << "insert into " << PA_DVA_MESSAGE_TABLE_NAME << " (" 
            << KEY_COL << ", "
            << LOCATIONKEY_COL << ", "
            << ID_COL << ", "
            << LABEL_COL << ", "
            << TYPE_COL << ", "
            << TISMESSAGETAG_COL << ", "
            << TISLIBRARYSECTION_COL << ", "
            << TISLIBRARYVERSION_COL << ") "
            << "values (" << sequenceNumber << ","
            << m_locationKey.getValue() << "," 
            << m_id.getValue() << ",'" 
            << (databaseConnection->escapeInsertString(m_label)).c_str() << "','" 
            << (databaseConnection->escapeInsertString(m_type)).c_str() << "'," 
            << getTisMessageTagDatabaseRepresentation() << ", "
            << getTisLibrarySectionDatabaseRepresentation() << ", "
            << getTisLibraryVersionDatabaseRepresentation() << ")";

        databaseConnection->executeModification(sql.str());

        sql.str("");

        // Obtain the newly created record
        sql << "select " 
            << KEY_COL << ", "
            << "TO_CHAR(DATE_CREATED,'YYYYMMDDHH24MISS') "
            << "from " << PA_DVA_MESSAGE_TABLE_NAME << " where "
            << KEY_COL << " = " << sequenceNumber;

        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL);
        columnNames.push_back(DATECREATED_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(sql.str(),columnNames);

        if (0 == data->getNumRows()) 
        {
            // PA Zone record not found            
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for PA Zone key = " << m_key;			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"PA Zone key" ) );
        }

        // Retrieve the pkey as an ta_uint32 . There should only be a single returned item 
        // (as any other circumstance is specifically prevented by the above checks).
        // This method can throw a DataException
        try
        {
            m_key = data->getUnsignedLongData(0,KEY_COL);
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


    void PaDvaMessageHelper::validateLocationKeyExistence()
    {
        ////////////////////////////////////////////////////////////////////////////
        //
        // First check that the LOCATIONKEY already exists.
        //
        ////////////////////////////////////////////////////////////////////////////
        
        std::ostringstream sql;
        sql << "select PKEY from LOCATION where PKEY = " << m_locationKey.getValue();
        
        std::vector<std::string> columnNames;
        columnNames.clear();
        columnNames.push_back("PKEY");

        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Pa_Cd, TA_Base_Core::Read);
        TA_Base_Core::IData* data = databaseConnection->executeQuery(sql.str(),columnNames);

        if (0 == data->getNumRows()) 
        {
            // Location key not found
            
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for Location key = " << m_locationKey.getValue();			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"Location key" ) );
        }

        delete data;
        data = NULL;
    }


} // closes TA_IRS_Core

