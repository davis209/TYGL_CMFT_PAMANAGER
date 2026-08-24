/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/pa/src/PaAtsTriggeredBroadcastAccessFactory.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * PaAtsTriggeredBroadcastAccessFactory is a singleton that is used to retrieve PaAtsTriggeredBroadcast objects either from the
  * database or newly created. All PaAtsTriggeredBroadcast objects returned will adhere to the IPaAtsTriggeredBroadcast interface.
  */
#pragma warning(disable:4786 4290)


#include "core/types/src/ta_types.h"
#include "core/data_access_interface/pa/src/PaAtsTriggeredBroadcastAccessFactory.h"

#include "core/data_access_interface/pa/src/PaAtsTriggeredBroadcast.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/pa/src/ConfigPaAtsTriggeredBroadcast.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


namespace TA_IRS_Core
{
    PaAtsTriggeredBroadcastAccessFactory* PaAtsTriggeredBroadcastAccessFactory::m_instance = 0;

    static const std::string KEY_COL_NAME = "PAATBR_ID";
    static const std::string PA_ATS_TRIG_BCAST_TABLE_NAME = "PA_ATS_TRIGGERED_BROADCAST";
    static const std::string LOCATIONKEY_COL = "LOCATIONKEY";
    
    static const std::string ATS_PLATFORM_TABLE = "AT_PLATFORM";
    static const std::string PLATFORM_KEY_COL = "ATPLAT_ID";
    static const std::string PLATFORM_ID_COL = "PLATFORM_ID";
    static const std::string PLATFORM_NAME_COL = "PLATFORM_NAME";
    static const std::string PLATFORM_STATION_ID_COL = "STATION_ID";

    PaAtsTriggeredBroadcastAccessFactory& PaAtsTriggeredBroadcastAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new PaAtsTriggeredBroadcastAccessFactory();
        }
        return *m_instance;
    }


    void PaAtsTriggeredBroadcastAccessFactory::removeInstance( )
    {
	    if ( m_instance != NULL )
	    {
		    delete m_instance;
		    m_instance = NULL;
	    }
    }


    IPaAtsTriggeredBroadcast* PaAtsTriggeredBroadcastAccessFactory::getPaAtsTriggeredBroadcast
    (
        const ta_uint32 key,
        const bool readWrite
    )
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Pa_Cd, TA_Base_Core::Read);

        // create the SQL string to retrieve the name of the PaAtsTriggeredBroadcast
        // this is a check to ensure an PaAtsTriggeredBroadcast with the specified
        // PAZONE_ID actually exists.        
        std::ostringstream sql;
        sql << "select " << KEY_COL_NAME 
            << " from " << PA_ATS_TRIG_BCAST_TABLE_NAME
            << " where " << KEY_COL_NAME << " = " << key;        

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL_NAME);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(sql.str(), columnNames);
        
        // Bring the DataException into the namespace
        using TA_Base_Core::DataException;

        if (0 == data->getNumRows()) // No entry found with the specified pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            char reasonMessage[256] = {0};
			sprintf(reasonMessage, "No data found for pkey = %lu",key);			
                TA_THROW(DataException(reasonMessage,DataException::NO_VALUE,""));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            char reasonMessage[256] = {0};
			sprintf(reasonMessage, "More than one entry found for pkey = %lu",key);			
                TA_THROW(DataException(reasonMessage,DataException::NOT_UNIQUE,""));
        }

        // Don't need to do anything with the data - the check was only that a single
        // item was returned.
        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        // Create the PaAtsTriggeredBroadcast object to represent this object. Initially only populate it
        // with id. We will populate the rest of the data on demand.
        IPaAtsTriggeredBroadcast* thePaAtsTriggeredBroadcast;
        
        // decide which PaAtsTriggeredBroadcast object to return
        if (readWrite) // need a config PaAtsTriggeredBroadcast
        {
            thePaAtsTriggeredBroadcast = new ConfigPaAtsTriggeredBroadcast(key);
        }
        else // need a standard PaAtsTriggeredBroadcast
        {
            thePaAtsTriggeredBroadcast = new PaAtsTriggeredBroadcast(key);
        }

        // Return the PaAtsTriggeredBroadcast pointer. The class that recieves this pointer is responsible
        // for deleting it.
        return thePaAtsTriggeredBroadcast;
    }


    IPaAtsTriggeredBroadcasts PaAtsTriggeredBroadcastAccessFactory::getAllPaAtsTriggeredBroadcasts( const bool readWrite)
    {
        return getPaAtsTriggeredBroadcastsReturnedFromStatement("", readWrite);
    }


    IPaAtsTriggeredBroadcasts PaAtsTriggeredBroadcastAccessFactory::getPaAtsTriggeredBroadcastsByLocationKey
    (
    ta_uint32 locationKey,
     const bool readWrite 
    )
    {
        std::ostringstream sql;
        sql << LOCATIONKEY_COL << " = " << locationKey;

        return getPaAtsTriggeredBroadcastsReturnedFromStatement(sql.str(), readWrite);   
    }


    IPaAtsTriggeredBroadcasts PaAtsTriggeredBroadcastAccessFactory::getPaAtsTriggeredBroadcastsReturnedFromStatement
    (
     const std::string& whereClause,
     const bool readWrite
    )
    {
        return DatabaseQueryHelper::getDatabaseRecordsReturnedFromStatement
                    <IPaAtsTriggeredBroadcasts, ConfigPaAtsTriggeredBroadcast, PaAtsTriggeredBroadcast>
                        (PaAtsTriggeredBroadcastHelper::getBasicQueryData(), whereClause, readWrite);
    }
    

    IConfigPaAtsTriggeredBroadcast* PaAtsTriggeredBroadcastAccessFactory::createPaAtsTriggeredBroadcast()
    {            
        return new ConfigPaAtsTriggeredBroadcast();    
    }


    IConfigPaAtsTriggeredBroadcast* PaAtsTriggeredBroadcastAccessFactory::copyPaAtsTriggeredBroadcast(const IConfigPaAtsTriggeredBroadcast* paAtsTriggeredBroadcastToCopy)
    {
        FUNCTION_ENTRY("copyPaAtsTriggeredBroadcast");

        TA_ASSERT(paAtsTriggeredBroadcastToCopy !=  NULL, "The PaAtsTriggeredBroadcast to copy was NULL");
        
        const ConfigPaAtsTriggeredBroadcast* cast = dynamic_cast<const ConfigPaAtsTriggeredBroadcast*>(paAtsTriggeredBroadcastToCopy);

        TA_ASSERT(cast != NULL, "PaAtsTriggeredBroadcast passed could not be converted into a ConfigPaAtsTriggeredBroadcast");

        FUNCTION_EXIT;
        return new ConfigPaAtsTriggeredBroadcast(*cast);

    }
        
    PaAtsTriggeredBroadcastAccessFactory::PlatformIds 
    PaAtsTriggeredBroadcastAccessFactory::enumeratePlatformIds()
    {
        std::vector<std::string> columnNames;
        columnNames.push_back(PLATFORM_KEY_COL);
        columnNames.push_back(PLATFORM_ID_COL);
        columnNames.push_back(PLATFORM_NAME_COL);
        columnNames.push_back(PLATFORM_STATION_ID_COL);

        std::string query = TA_IRS_Core::DatabaseQueryHelper::createBasicSelectStatement(
                                        columnNames, ATS_PLATFORM_TABLE, false);
        
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Pa_Cd, TA_Base_Core::Read);
        std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery(query, columnNames));
     
        PlatformIds result;
          
        if (data.get() == 0)
        {
            // Return empty set in the event of no rows
            return result;
        }
        
        // Fill in resulting structures
        for (unsigned int i = 0; i < data->getNumRows(); ++i)
        {
            PlatformId msg;

            msg.pKey            = data->getUnsignedLongData(i, PLATFORM_KEY_COL);
            msg.platformId      = data->getUnsignedLongData(i, PLATFORM_ID_COL);

            msg.platformName    = data->getStringData(i, PLATFORM_NAME_COL);
            msg.stationId       = data->getUnsignedLongData(i, PLATFORM_STATION_ID_COL);  

            result.push_back(msg);      
        }

        return result;
    }
} // closes TA_IRS_Core


