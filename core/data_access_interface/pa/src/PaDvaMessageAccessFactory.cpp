/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/pa/src/PaDvaMessageAccessFactory.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * PaDvaMessageAccessFactory is a singleton that is used to retrieve PaDvaMessage objects either from the
  * database or newly created. All PaDvaMessage objects returned will adhere to the IPaDvaMessage interface.
  */
#pragma warning(disable:4786 4290)


#include "core/types/src/ta_types.h"
#include "core/data_access_interface/pa/src/PaDvaMessageAccessFactory.h"

#include "core/data_access_interface/pa/src/PaDvaMessage.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/pa/src/ConfigPaDvaMessage.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


namespace TA_IRS_Core
{
    PaDvaMessageAccessFactory* PaDvaMessageAccessFactory::m_instance = 0;

    static const std::string KEY_COL_NAME = "PADMES_ID";
    static const std::string PA_DVA_MESSAGE_TABLE = "PA_DVA_MESSAGE";
    static const std::string LOCATIONKEY_COL = "LOCATIONKEY";

    static const std::string TIS_TABLE_NAME = "TI_PREDEFINED_MESSAGE";
    static const std::string TIS_KEY_COL_NAME = "TIPMES_ID";
    static const std::string TIS_MESSAGE_TAG_COL = "MESSAGE_TAG";
    static const std::string TIS_LIBRARY_SECTION_COL = "LIBRARY_SECTION";
    static const std::string TIS_DESCRIPTION_COL = "DESCRIPTION";

    static const std::string TIS_LIBRARY_VER_KEY_COL = "TIPMLI_ID";
    static const std::string TIS_LIBRARY_VER_TABLE = "TI_PREDEFINED_MESSAGE_LIBRARY";
    static const std::string TIS_LIBRARY_VERSION_COL = "VERSION";

    PaDvaMessageAccessFactory::TisMessage::TisMessage()
    :
    pKey(TA_Base_Core::DatabaseKey::getInvalidKey()),
    tisMessageTag(TA_Base_Core::DatabaseKey::getInvalidKey()),
    tisLibrarySection(TA_Base_Core::DatabaseKey::getInvalidKey()),
    tisLibraryVersion(TA_Base_Core::DatabaseKey::getInvalidKey())
    {
    }

    PaDvaMessageAccessFactory& PaDvaMessageAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new PaDvaMessageAccessFactory();
        }
        return *m_instance;
    }


    void PaDvaMessageAccessFactory::removeInstance( )
    {
	    if ( m_instance != NULL )
	    {
		    delete m_instance;
		    m_instance = NULL;
	    }
    }


    IPaDvaMessage* PaDvaMessageAccessFactory::getPaDvaMessage(const ta_uint32 key,const bool readWrite)
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Pa_Cd, TA_Base_Core::Read);

        // create the SQL string to retrieve the name of the PaDvaMessage
        // this is a check to ensure an PaDvaMessage with the specified
        // PAZONE_ID actually exists.        
        std::ostringstream sql;
        sql << "select " << KEY_COL_NAME 
            << " from " << PA_DVA_MESSAGE_TABLE
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

        // Create the PaDvaMessage object to represent this object. Initially only populate it
        // with id. We will populate the rest of the data on demand.
        IPaDvaMessage* thePaDvaMessage;
        
        // decide which PaDvaMessage object to return
        if (readWrite) // need a config PaDvaMessage
        {
            thePaDvaMessage = new ConfigPaDvaMessage(key);
        }
        else // need a standard PaDvaMessage
        {
            thePaDvaMessage = new PaDvaMessage(key);
        }

        // Return the PaDvaMessage pointer. The class that recieves this pointer is responsible
        // for deleting it.
        return thePaDvaMessage;
    }


    IPaDvaMessages PaDvaMessageAccessFactory::getAllPaDvaMessages(const bool readWrite )
    {
        return getPaDvaMessagesReturnedFromStatement("", readWrite);
    }


    IPaDvaMessages PaDvaMessageAccessFactory::getPaDvaMessagesByLocationKey(ta_uint32 locationKey, const bool readWrite )
    {
        std::ostringstream sql;
        sql << LOCATIONKEY_COL << " = " << locationKey;

        return getPaDvaMessagesReturnedFromStatement(sql.str(), readWrite);   
    }


    IPaDvaMessages PaDvaMessageAccessFactory::getPaDvaMessagesReturnedFromStatement( const std::string& whereClause,  const bool readWrite)
    {
        return DatabaseQueryHelper::getDatabaseRecordsReturnedFromStatement
                    <IPaDvaMessages, ConfigPaDvaMessage, PaDvaMessage>
                    (PaDvaMessageHelper::getBasicQueryData(), whereClause, readWrite);
    }
    

    IConfigPaDvaMessage* PaDvaMessageAccessFactory::createPaDvaMessage()
    {            
        return new ConfigPaDvaMessage();    
    }


    IConfigPaDvaMessage* PaDvaMessageAccessFactory::copyPaDvaMessage(const IConfigPaDvaMessage* paDvaMessageToCopy)
    {
        FUNCTION_ENTRY("copyPaDvaMessage");

        TA_ASSERT(paDvaMessageToCopy !=  NULL, "The PaDvaMessage to copy was NULL");
        
        const ConfigPaDvaMessage* cast = dynamic_cast<const ConfigPaDvaMessage*>(paDvaMessageToCopy);

        TA_ASSERT(cast != NULL, "PaDvaMessage passed could not be converted into a ConfigPaDvaMessage");

        FUNCTION_EXIT;
        return new ConfigPaDvaMessage(*cast);

    }


    std::vector<PaDvaMessageAccessFactory::TisMessage> PaDvaMessageAccessFactory::enumerateTisMessages()
    {
        std::vector<std::string> columnNames;

        std::string msgTable("a.");
        std::string verTable("b.");
        
        columnNames.push_back(msgTable + TIS_KEY_COL_NAME);
        columnNames.push_back(msgTable + TIS_MESSAGE_TAG_COL);
        columnNames.push_back(msgTable + TIS_LIBRARY_SECTION_COL);
        columnNames.push_back(msgTable + TIS_DESCRIPTION_COL);
        columnNames.push_back(verTable + TIS_LIBRARY_VERSION_COL);

        unsigned int i;
        std::ostringstream sql;
        sql << "select ";
        for (i = 0; i < columnNames.size(); i ++)
        {
            if (i > 0)
            {
                sql << ", ";
            }
            sql << columnNames[i];
        }
        
        // Join with the TIS message version table, to get version information
        sql << " from " << TIS_TABLE_NAME << " a, " << TIS_LIBRARY_VER_TABLE << " b"
            << " where a." << TIS_LIBRARY_VER_KEY_COL << " = " << "b." << TIS_LIBRARY_VER_KEY_COL;
        
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Pa_Cd, TA_Base_Core::Read);

        std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery(sql.str(), columnNames));
               
        std::vector<TisMessage> result;
        
        if (data.get() == 0)
        {
            // Return empty set in the event of no rows
            return result;
        }

        // Fill in resulting structures
        for (i = 0; i < data->getNumRows(); ++i)
        {
            TisMessage msg;

            msg.pKey            = data->getUnsignedLongData(i, msgTable + TIS_KEY_COL_NAME);

            if (!data->isNull(i, msgTable + TIS_MESSAGE_TAG_COL))
            {
                msg.tisMessageTag   = data->getUnsignedLongData(i, msgTable + TIS_MESSAGE_TAG_COL);
            }
            if (!data->isNull(i, msgTable + TIS_LIBRARY_SECTION_COL))
            {
                msg.tisLibrarySection   = data->getUnsignedLongData(i, msgTable + TIS_LIBRARY_SECTION_COL);
            }            
            if (!data->isNull(i, msgTable + TIS_DESCRIPTION_COL))
            {
                msg.description = data->getStringData(i, msgTable + TIS_DESCRIPTION_COL);
            }

            // One of the few fields that isn't possibly null
            msg.tisLibraryVersion = data->getUnsignedLongData(i, verTable + TIS_LIBRARY_VERSION_COL);

            result.push_back(msg);
        }

        return result;
    }

} // closes TA_IRS_Core


