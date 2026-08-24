/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/pa/src/ConfigPaDvaMessage.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * ConfigPaDvaMessage is a concrete implementation of IConfigPaDvaMessage, which is in turn an implementation
  * of IPaDvaMessage. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for PaDvaMessages.
  */
#ifdef __WIN32__
#pragma warning(disable:4786 4290)
#endif

#include "core/types/src/ta_types.h"
#include <algorithm>
#include <sstream>
#include <iterator>

#include "core/data_access_interface/pa/src/ConfigPaDvaMessage.h"
#include "core/data_access_interface/pa/src/PaDvaMessageHelper.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_Core
{
    ta_int32 ConfigPaDvaMessage::s_nextAvailableIdentifier = 0;
    // These are the names of the fixed fields that will be used in places where we are creating
    // strings that will be shown to the user.
    static const std::string LOCATIONKEY        = "Location Key";
    static const std::string ID                 = "ID";
    static const std::string LABEL              = "Label";
    static const std::string TYPE               = "Type";
    
    static const std::string TISMESSAGETAG      = "Tis Message Tag";
    static const std::string TISLIBRARYSECTION  = "Tis Library Section";
    static const std::string TISLIBRARYVER      = "TIS Library Version";

    
    ConfigPaDvaMessage::ConfigPaDvaMessage()
     : m_paDvaMessageHelper( new PaDvaMessageHelper() ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }


    ConfigPaDvaMessage::ConfigPaDvaMessage(ta_uint32 row, TA_Base_Core::IData& data)
     : m_paDvaMessageHelper(new PaDvaMessageHelper(row, data)),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }


    ConfigPaDvaMessage::ConfigPaDvaMessage(const ta_uint32 key)
     : m_paDvaMessageHelper( new PaDvaMessageHelper(key) ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }

    ConfigPaDvaMessage::ConfigPaDvaMessage( const ConfigPaDvaMessage& theConfigPaDvaMessage)
     : m_paDvaMessageHelper( new PaDvaMessageHelper( *(theConfigPaDvaMessage.m_paDvaMessageHelper)) ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {      
    }



    ConfigPaDvaMessage::~ConfigPaDvaMessage()
    {
        try
        {
            delete m_paDvaMessageHelper;
            m_paDvaMessageHelper = NULL;
        }
        catch ( ... )
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
    }


    bool ConfigPaDvaMessage::isNew()
    {
        TA_ASSERT(m_paDvaMessageHelper != NULL,"The PaDvaMessageHelper pointer is null.");

        return m_paDvaMessageHelper->isNew();
    }

    void ConfigPaDvaMessage::setLocationKey(const ta_uint32 locationKey)
    {
        FUNCTION_ENTRY("setLocationKey");
        
        TA_ASSERT(m_paDvaMessageHelper != NULL,"The PaDvaMessageHelper pointer is null.");
        
        updatePaDvaMessageChanges(LOCATIONKEY, 
                                    isNew() ? 0 : m_paDvaMessageHelper->getLocationKey(), 
                                    locationKey);

        m_paDvaMessageHelper->setLocationKey(locationKey);

        FUNCTION_EXIT;
    }
    

    void ConfigPaDvaMessage::setId(const ta_uint32 id)
    {
        FUNCTION_ENTRY("setId");
        
        TA_ASSERT(m_paDvaMessageHelper != NULL,"The PaDvaMessageHelper pointer is null.");
    
        updatePaDvaMessageChanges(ID, 
                                    isNew() ? 0 : m_paDvaMessageHelper->getId(), 
                                    id);

        m_paDvaMessageHelper->setId(id);

        FUNCTION_EXIT;
    }

    
    void ConfigPaDvaMessage::setLabel(const std::string& label)
    {
        FUNCTION_ENTRY("setLabel");
        
        TA_ASSERT(m_paDvaMessageHelper != NULL,"The PaDvaMessageHelper pointer is null.");
    
        updatePaDvaMessageChanges(LABEL, 
                                    isNew() ? "" : m_paDvaMessageHelper->getLabel(), 
                                    label);

        m_paDvaMessageHelper->setLabel(label);

        FUNCTION_EXIT;
    }


    void ConfigPaDvaMessage::setType(const std::string& type)
    {
        FUNCTION_ENTRY("setType");
        
        TA_ASSERT(m_paDvaMessageHelper != NULL,"The PaDvaMessageHelper pointer is null.");
    
        updatePaDvaMessageChanges(TYPE, 
                                     isNew() ? "" : m_paDvaMessageHelper->getType(), 
                                     type);

        m_paDvaMessageHelper->setType(type);

        FUNCTION_EXIT;
    }


    void ConfigPaDvaMessage::setTisMessageTag(const ta_uint32 tisMessageTag)
    {
        FUNCTION_ENTRY("setTisMessageTag");
        
        TA_ASSERT(m_paDvaMessageHelper != NULL,"The PaDvaMessageHelper pointer is null.");
    
        updatePaDvaMessageChanges(TISMESSAGETAG, 
                                     isNew() ? 0 : m_paDvaMessageHelper->getTisMessageTag(),
                                     tisMessageTag);

        m_paDvaMessageHelper->setTisMessageTag(tisMessageTag);

        FUNCTION_EXIT;
    }


    void ConfigPaDvaMessage::setTisLibrarySection(const ta_uint32 tisLibrarySection)
    {
        FUNCTION_ENTRY("setTisLibrarySection");
        
        TA_ASSERT(m_paDvaMessageHelper != NULL,"The PaDvaMessageHelper pointer is null.");
    
        updatePaDvaMessageChanges(TISLIBRARYSECTION, 
                                     isNew() ? 0 : m_paDvaMessageHelper->getTisLibrarySection(),
                                     tisLibrarySection);

        m_paDvaMessageHelper->setTisLibrarySection(tisLibrarySection);

        FUNCTION_EXIT;
    }


    void ConfigPaDvaMessage::setTisLibraryVersion(const ta_uint32 version)
    {
        FUNCTION_ENTRY("setTisLibraryVersion");
        
        TA_ASSERT(m_paDvaMessageHelper != NULL,"The PaDvaMessageHelper pointer is null.");
    
        updatePaDvaMessageChanges(TISLIBRARYVER, 
                                  isNew() ? 0 : m_paDvaMessageHelper->getTisLibraryVersion(), 
                                  version);

        m_paDvaMessageHelper->setTisLibraryVersion(version);

        FUNCTION_EXIT;
    }


    ta_uint32 ConfigPaDvaMessage::getKey()
    {
        TA_ASSERT(m_paDvaMessageHelper != NULL,"The PaDvaMessageHelper pointer is null.");
        return m_paDvaMessageHelper->getKey();
    }

    ta_uint32 ConfigPaDvaMessage::getLocationKey()
    {
        TA_ASSERT(m_paDvaMessageHelper != NULL,"The PaDvaMessageHelper pointer is null.");
        return m_paDvaMessageHelper->getLocationKey();
    }

    ta_uint32 ConfigPaDvaMessage::getId()
    {
        TA_ASSERT(m_paDvaMessageHelper != NULL,"The PaDvaMessageHelper pointer is null.");
        return m_paDvaMessageHelper->getId();
    }

    std::string ConfigPaDvaMessage::getLabel()
    {
        TA_ASSERT(m_paDvaMessageHelper != NULL,"The PaDvaMessageHelper pointer is null.");
        return m_paDvaMessageHelper->getLabel();
    }    
    
    std::string ConfigPaDvaMessage::getType()
    {
        TA_ASSERT(m_paDvaMessageHelper != NULL,"The PaDvaMessageHelper pointer is null.");
        return m_paDvaMessageHelper->getType();
    }    
    

    ta_uint32 ConfigPaDvaMessage::getTisMessageTag()
    {
        TA_ASSERT(m_paDvaMessageHelper != NULL,"The PaDvaMessageHelper pointer is null.");
        return m_paDvaMessageHelper->getTisMessageTag();
    }


    ta_uint32 ConfigPaDvaMessage::getTisLibrarySection()
    {
        TA_ASSERT(m_paDvaMessageHelper != NULL,"The PaDvaMessageHelper pointer is null.");
        return m_paDvaMessageHelper->getTisLibrarySection();
    }


    ta_uint32 ConfigPaDvaMessage::getTisLibraryVersion()
    {
        TA_ASSERT(m_paDvaMessageHelper != NULL,"The PaDvaMessageHelper pointer is null.");
        return m_paDvaMessageHelper->getTisLibraryVersion();
    }

    time_t ConfigPaDvaMessage::getDateCreated()
    {
        TA_ASSERT(m_paDvaMessageHelper != NULL,"The PaDvaMessageHelper pointer is null.");
		return m_paDvaMessageHelper->getDateCreated();
    }


    time_t ConfigPaDvaMessage::getDateModified()
    {
        TA_ASSERT(m_paDvaMessageHelper != NULL,"The PaDvaMessageHelper pointer is null.");
		return m_paDvaMessageHelper->getDateModified();
    }
 
	
    void ConfigPaDvaMessage::invalidate()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigPaDvaMessage::invalidate");
        TA_ASSERT(m_paDvaMessageHelper != NULL,"The PaDvaMessageHelper pointer is null.");

        m_paDvaMessageHelper->invalidate();
        m_paDvaMessageChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigPaDvaMessage::invalidate");
    }


    void ConfigPaDvaMessage::deleteThisObject(bool cascade)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigPaDvaMessage::deleteThisObject");
        TA_ASSERT(m_paDvaMessageHelper != NULL,"The PaDvaMessageHelper pointer is null.");

        // Delete the ConfigPaDvaMessage from the database
        m_paDvaMessageHelper->deletePaDvaMessage(cascade);
        
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "About to delete PaDvaMessageHelper pointer m_paDvaMessageHelper");

        delete m_paDvaMessageHelper;
        m_paDvaMessageHelper = NULL;

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigPaDvaMessage::deleteThisObject");
    }


    void ConfigPaDvaMessage::applyChanges()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigPaDvaMessage::applyChanges");
        TA_ASSERT(m_paDvaMessageHelper != NULL, "applyChanges() has been called after ConfigPaDvaMessage was deleted");

        m_paDvaMessageHelper->writePaDvaMessageData();

        m_paDvaMessageChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigPaDvaMessage::applyChanges");
    }

    void ConfigPaDvaMessage::updatePaDvaMessageChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
    {
        FUNCTION_ENTRY("updatePaDvaMessageChanges");

        // Search for this name in the map
        TA_Base_Core::ItemChanges::iterator matching = m_paDvaMessageChanges.find(name);

        if (matching != m_paDvaMessageChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_paDvaMessageChanges.erase(matching);
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
            m_paDvaMessageChanges.insert( TA_Base_Core::ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }

    void ConfigPaDvaMessage::updatePaDvaMessageChanges(const std::string& name, const ta_uint32 oldValue, const ta_uint32 newValue)
    {
        FUNCTION_ENTRY("updatePaDvaMessageChanges_ul");
        std::ostringstream oldValueStr;
        oldValueStr << oldValue;
        std::ostringstream newValueStr;
        newValueStr << newValue;

        updatePaDvaMessageChanges(name, oldValueStr.str(), newValueStr.str());
        FUNCTION_EXIT;
    }
    
    void ConfigPaDvaMessage::setName(const std::string& name)
    {
        TA_ASSERT(false ,"setName() should not be called.");
    }

    std::string ConfigPaDvaMessage::getName()
    {
        TA_ASSERT(m_paDvaMessageHelper != NULL,"The m_paDvaMessageHelper pointer is null.");

        std::ostringstream name;
		name << "PA DVA Message " << m_paDvaMessageHelper->getKey();

        return name.str();
    }
    
} // closes TA_IRS_Core

