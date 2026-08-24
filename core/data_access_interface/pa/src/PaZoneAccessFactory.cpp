/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/pa/src/PaZoneAccessFactory.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * PaZoneAccessFactory is a singleton that is used to retrieve PaZone objects either from the
  * database or newly created. All PaZone objects returned will adhere to the IPaZone interface.
  */
#pragma warning(disable:4786 4290)


#include "core/types/src/ta_types.h"
#include "core/data_access_interface/pa/src/PaZoneAccessFactory.h"
#include "core/data_access_interface/pa/src/DatabaseQueryHelper.h"

#include "core/data_access_interface/pa/src/PaZone.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/pa/src/ConfigPaZone.h"

#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


namespace TA_IRS_Core
{
    PaZoneAccessFactory* PaZoneAccessFactory::m_instance = 0;

    static const std::string PA_ZONE_GROUP_HELPER_TABLE_NAME = "PA_ZONE_GROUP_HELPER pazgrohelper";
    static const std::string PA_ZONE_GROUP_HELPER_FOREIGN_KEY_PA_ZONE = "pazgrohelper.PAZONE_ID";
    static const std::string PA_ZONE_GROUP_HELPER_GROUP_ID = "pazgrohelper.PAZGRO_ID";

    PaZoneAccessFactory& PaZoneAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new PaZoneAccessFactory();
        }
        return *m_instance;
    }


    void PaZoneAccessFactory::removeInstance( )
    {
	    if ( m_instance != NULL )
	    {
		    delete m_instance;
		    m_instance = NULL;
	    }
    }


    IPaZone* PaZoneAccessFactory::getPaZone(const ta_uint32 key,
                                            const bool readWrite)
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Pa_Cd, TA_Base_Core::Read);

        // create the SQL string to retrieve the name of the PaZone
        // this is a check to ensure an PaZone with the specified
        // PAZONE_ID actually exists.        
        std::ostringstream sql;
        sql << "select " << PaZoneHelper::KEY_COL 
            << " from " << PaZoneHelper::PA_ZONE_TABLE_NAME
            << " where " << PaZoneHelper::KEY_COL << " = " << key;        

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(PaZoneHelper::KEY_COL);

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

        // Create the PaZone object to represent this object. Initially only populate it
        // with id. We will populate the rest of the data on demand.
        IPaZone* thePaZone;
        
        // decide which PaZone object to return
        if (readWrite) // need a config PaZone
        {
            thePaZone = new ConfigPaZone(key);
        }
        else // need a standard PaZone
        {
            thePaZone = new PaZone(key);
        }

        // Return the PaZone pointer. The class that recieves this pointer is responsible
        // for deleting it.
        return thePaZone;
    }


    IPaZones PaZoneAccessFactory::getPaZonesByLocationKey(ta_uint32 locationKey, 
                                                          const bool readWrite)
    {
        std::ostringstream sql;
        sql << PaZoneHelper::LOCATIONKEY_COL << " = " << locationKey;

        return getPaZonesReturnedFromStatement("", sql.str(), readWrite);   
    }


    std::list< ta_uint32 > PaZoneAccessFactory::getPaZoneKeysByGroupId(ta_uint32 paZoneGroupKey)
    {
        std::ostringstream sql;
       
        sql << "select " << PA_ZONE_GROUP_HELPER_FOREIGN_KEY_PA_ZONE
            << " from " << PA_ZONE_GROUP_HELPER_TABLE_NAME
            << " where PAZGRO_ID = " << paZoneGroupKey;     // Filter out non matching group IDs
               
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Pa_Cd, TA_Base_Core::Read);

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(PA_ZONE_GROUP_HELPER_FOREIGN_KEY_PA_ZONE);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(sql.str(), columnNames);
        
        // create the vector of pointers to IEntityData
        std::list< ta_uint32 > paZoneKeys;

        do
        {
            for (ta_uint32 i = 0; i < data->getNumRows() ; ++i )
            {   
                ta_uint32 key = data->getUnsignedLongData(i, PA_ZONE_GROUP_HELPER_FOREIGN_KEY_PA_ZONE);
                paZoneKeys.push_back(key);
            }

            delete data;
            data = NULL;
        }
        while( databaseConnection->moreData(data) );

        return paZoneKeys;
    }


    IPaZones PaZoneAccessFactory::getPaZonesByGroupId(ta_uint32 paZoneGroupKey, 
                                                      const bool readWrite)
    {   
        // Only want matches where group key is a match, and group zone ID matches zone ID
        std::ostringstream whereClause;
        whereClause << PA_ZONE_GROUP_HELPER_GROUP_ID << " = " << paZoneGroupKey
                    << " and " << PA_ZONE_GROUP_HELPER_FOREIGN_KEY_PA_ZONE
                               << " = " << PaZoneHelper::KEY_COL;
        
        return getPaZonesReturnedFromStatement(PA_ZONE_GROUP_HELPER_TABLE_NAME, 
                                               whereClause.str(), 
                                               readWrite);
    }

    IPaZones PaZoneAccessFactory::getAllPaZones(const bool readWrite)
    {
        return getPaZonesReturnedFromStatement("", "", readWrite);
    }


    std::list< ta_uint32 > PaZoneAccessFactory::getPagingConsolePaZoneKeys()
    {
        std::list< ta_uint32 > zoneKeys;

        // get all consoles
        TA_Base_Core::IConsole* console = NULL;

        std::vector<TA_Base_Core::IConsole*> consoles = TA_Base_Core::ConsoleAccessFactory::getInstance().getAllConsoles();

        try
        {
            // for each console - get the paging zone
            for (std::vector<TA_Base_Core::IConsole*>::iterator iter = consoles.begin();
                 iter != consoles.end(); iter++)
            {
                try
                {
                    ta_uint32 zoneId = (*iter)->getPagingConsoleZoneId();

                    // add the paging zone to the list (only if it is a valid zone)
                    if (zoneId > 0)
                    {
                        zoneKeys.push_back(zoneId);
                    }
                }
                catch(const TA_Base_Core::DataException& de)
                {
                    // this item is invalid - skip it and move on
                    LOG_EXCEPTION_CATCH( SourceInfo, "DataException", de.what());
                }

                // item is no longer needed
                delete (*iter);
                (*iter) = NULL;
            }

            consoles.clear();
        }
        catch(const TA_Base_Core::DatabaseException&)
        {
            // A database exception is more fatal
            // none of the items can be loaded

            // clean up
            for (std::vector<TA_Base_Core::IConsole*>::iterator iter = consoles.begin();
                 iter != consoles.end(); iter++)
            {
                if ( (*iter) != NULL )
                {
                    delete (*iter);
                    (*iter) = NULL;
                }
            }
            
            consoles.clear();

            // throw on
            throw;
        }

        // the consoles should already be cleaned up by this point

        return zoneKeys;
    }


    IPaZones PaZoneAccessFactory::getPaZonesReturnedFromStatement(const std::string& tableToJoin,
                                                                  const std::string& whereClause, 
                                                                  const bool readWrite)
    {
        const DatabaseQueryHelper::QueryData queryData = 
                            PaZoneHelper::getBasicQueryData(tableToJoin);
        
        return DatabaseQueryHelper::getDatabaseRecordsReturnedFromStatement
                    <IPaZones, ConfigPaZone, PaZone>
                        (queryData, whereClause, readWrite);
    }
    

    IConfigPaZone* PaZoneAccessFactory::createPaZone()
    {            
        return new ConfigPaZone();    
    }


    IConfigPaZone* PaZoneAccessFactory::copyPaZone(const IConfigPaZone* paZoneToCopy)
    {
        FUNCTION_ENTRY("copyPaZone");

        TA_ASSERT(paZoneToCopy !=  NULL, "The PaZone to copy was NULL");
        
        const ConfigPaZone* cast = dynamic_cast<const ConfigPaZone*>(paZoneToCopy);

        TA_ASSERT(cast != NULL, "PaZone passed could not be converted into a ConfigPaZone");

        FUNCTION_EXIT;
        return new ConfigPaZone(*cast);

    }

} // closes TA_IRS_Core


