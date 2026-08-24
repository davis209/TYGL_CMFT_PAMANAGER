/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/pa/src/ConfigPaDvaMessageVersion.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * ConfigPaDvaMessageVersion is a concrete implementation of IConfigPaDvaMessageVersion, 
  * which is in turn an implementation
  * of IPaDvaMessageVersion. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for PaDvaMessageVersions.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786 4290)
#endif

#include "core/types/src/ta_types.h"
#include <algorithm>
#include <sstream>
#include <iterator>

#include "core/data_access_interface/pa/src/ConfigPaDvaMessageVersion.h"
#include "core/data_access_interface/pa/src/PaDvaMessageVersionHelper.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_Core
{
    ta_int32 ConfigPaDvaMessageVersion::s_nextAvailableIdentifier = 0;
    // These are the names of the fixed fields that will be used in places where we are creating
    // strings that will be shown to the user.
    static const std::string LOCATIONKEY              = "Label";
    static const std::string VERSIONCOLUMN_STRINGS[] =
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
    
    
    ConfigPaDvaMessageVersion::ConfigPaDvaMessageVersion()
     : m_paDvaMessageVersionHelper( new PaDvaMessageVersionHelper() ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }


    ConfigPaDvaMessageVersion::ConfigPaDvaMessageVersion(ta_uint32 row, TA_Base_Core::IData& data)
     : m_paDvaMessageVersionHelper(new PaDvaMessageVersionHelper(row, data)),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }


    ConfigPaDvaMessageVersion::ConfigPaDvaMessageVersion(const ta_uint32 key)
     : m_paDvaMessageVersionHelper( new PaDvaMessageVersionHelper(key) ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }


    ConfigPaDvaMessageVersion::ConfigPaDvaMessageVersion( const ConfigPaDvaMessageVersion& theConfigPaDvaMessageVersion)
     : m_paDvaMessageVersionHelper( new PaDvaMessageVersionHelper( *(theConfigPaDvaMessageVersion.m_paDvaMessageVersionHelper)) ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {      
    }



    ConfigPaDvaMessageVersion::~ConfigPaDvaMessageVersion()
    {
        try
        {
            delete m_paDvaMessageVersionHelper;
            m_paDvaMessageVersionHelper = NULL;
        }
        catch ( ... )
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
    }


    bool ConfigPaDvaMessageVersion::isNew()
    {
        TA_ASSERT(m_paDvaMessageVersionHelper != NULL,"The PaDvaMessageVersionHelper pointer is null.");

        return m_paDvaMessageVersionHelper->isNew();
    }


    void ConfigPaDvaMessageVersion::setLocationKey(ta_uint32 key)
    {
        FUNCTION_ENTRY("setLocationKey");
        
        TA_ASSERT(m_paDvaMessageVersionHelper != NULL,"The PaDvaMessageVersionHelper pointer is null.");
    
        updatePaDvaMessageVersionChanges(LOCATIONKEY,
                                isNew() ? 0 : m_paDvaMessageVersionHelper->getLocationKey(), 
                                key);

        m_paDvaMessageVersionHelper->setLocationKey(key);

        FUNCTION_EXIT;
    }


    void ConfigPaDvaMessageVersion::setVersionData(const std::string& newVersion,
                                        IPaDvaMessageVersion::EVersionColumn column)
    {
        FUNCTION_ENTRY("setVersionData");

        TA_ASSERT(m_paDvaMessageVersionHelper != NULL,"The PaDvaMessageVersionHelper pointer is null.");
    
        updatePaDvaMessageVersionChanges(VERSIONCOLUMN_STRINGS[column],
                                isNew() ? "" : m_paDvaMessageVersionHelper->getVersionData(column), 
                                newVersion);

        m_paDvaMessageVersionHelper->setVersionData(newVersion, column);

        FUNCTION_EXIT;
    }


    ta_uint32 ConfigPaDvaMessageVersion::getKey()
    {
        TA_ASSERT(m_paDvaMessageVersionHelper != NULL,"The PaDvaMessageVersionHelper pointer is null.");
        return m_paDvaMessageVersionHelper->getKey();
    }


    ta_uint32 ConfigPaDvaMessageVersion::getLocationKey()
    {
        TA_ASSERT(m_paDvaMessageVersionHelper != NULL,"The PaDvaMessageVersionHelper pointer is null.");
        return m_paDvaMessageVersionHelper->getLocationKey();
    }    
    

    std::string ConfigPaDvaMessageVersion::getMessageVersion(
                                            IPaDvaMessageVersion::ELocation location,
                                            ta_uint32 stationDvaMessageId)
    {
        TA_ASSERT(m_paDvaMessageVersionHelper != NULL,"The PaDvaMessageVersionHelper pointer is null.");
        return m_paDvaMessageVersionHelper->getMessageVersion(location, stationDvaMessageId);
    }


    std::string ConfigPaDvaMessageVersion::getVersionData(EVersionColumn column)
    {
        TA_ASSERT(m_paDvaMessageVersionHelper != NULL,"The PaDvaMessageVersionHelper pointer is null.");
        return m_paDvaMessageVersionHelper->getVersionData(column);
    }


    time_t ConfigPaDvaMessageVersion::getDateCreated()
    {
        TA_ASSERT(m_paDvaMessageVersionHelper != NULL,"The PaDvaMessageVersionHelper pointer is null.");
		return m_paDvaMessageVersionHelper->getDateCreated();
    }


    time_t ConfigPaDvaMessageVersion::getDateModified()
    {
        TA_ASSERT(m_paDvaMessageVersionHelper != NULL,"The PaDvaMessageVersionHelper pointer is null.");
		return m_paDvaMessageVersionHelper->getDateModified();
    }
 
	
    void ConfigPaDvaMessageVersion::invalidate()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigPaDvaMessageVersion::invalidate");
        TA_ASSERT(m_paDvaMessageVersionHelper != NULL,"The PaDvaMessageVersionHelper pointer is null.");

        m_paDvaMessageVersionHelper->invalidate();
        m_paDvaMessageVersionChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigPaDvaMessageVersion::invalidate");
    }


    void ConfigPaDvaMessageVersion::deleteThisObject()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigPaDvaMessageVersion::deleteThisObject");
        TA_ASSERT(m_paDvaMessageVersionHelper != NULL,"The PaDvaMessageVersionHelper pointer is null.");

        // Delete the ConfigPaDvaMessageVersion from the database
        m_paDvaMessageVersionHelper->deletePaDvaMessageVersion();
        
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "About to delete PaDvaMessageVersionHelper pointer m_paDvaMessageVersionHelper");

        delete m_paDvaMessageVersionHelper;
        m_paDvaMessageVersionHelper = NULL;

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigPaDvaMessageVersion::deleteThisObject");
    }


    void ConfigPaDvaMessageVersion::applyChanges()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigPaDvaMessageVersion::applyChanges");
        TA_ASSERT(m_paDvaMessageVersionHelper != NULL, "applyChanges() has been called after ConfigPaDvaMessageVersion was deleted");

        m_paDvaMessageVersionHelper->writePaDvaMessageVersionData();

        m_paDvaMessageVersionChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigPaDvaMessageVersion::applyChanges");
    }


    void ConfigPaDvaMessageVersion::updatePaDvaMessageVersionChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
    {
        FUNCTION_ENTRY("updatePaDvaMessageVersionChanges");

        // Search for this name in the map
        TA_Base_Core::ItemChanges::iterator matching = m_paDvaMessageVersionChanges.find(name);

        if (matching != m_paDvaMessageVersionChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_paDvaMessageVersionChanges.erase(matching);
            }
            else
            {
                //Update the stored new value.
                matching->second.newValue = newValue;
            }

            FUNCTION_EXIT;
            return;
        }

        // The first time we come in we should get to here and we set the old value to be the original value of the attribute
        if (oldValue != newValue)
        {
            // If the old and new values are different then we can add this item to the map
            TA_Base_Core::Values values;
            values.newValue = newValue;
            values.oldValue = oldValue;
            m_paDvaMessageVersionChanges.insert( TA_Base_Core::ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }

    void ConfigPaDvaMessageVersion::updatePaDvaMessageVersionChanges(const std::string& name, const ta_uint32 oldValue, const ta_uint32 newValue)
    {
        FUNCTION_ENTRY("updatePaDvaMessageVersionChanges_ul");
        std::ostringstream oldValueStr;
        oldValueStr << oldValue;
        std::ostringstream newValueStr;
        newValueStr << newValue;

        updatePaDvaMessageVersionChanges(name, oldValueStr.str(), newValueStr.str());
        FUNCTION_EXIT;
    }
    
    void ConfigPaDvaMessageVersion::setName(const std::string& name)
    {
        TA_ASSERT(false ,"setName() should not be called.");
    }

    std::string ConfigPaDvaMessageVersion::getName()
    {
        TA_ASSERT(m_paDvaMessageVersionHelper != NULL,"The m_paDvaMessageVersionHelper pointer is null.");

        std::ostringstream name;
		name << "PA DVA Message Version " << m_paDvaMessageVersionHelper->getKey();

        return name.str();
    }     

} // closes TA_IRS_Core

