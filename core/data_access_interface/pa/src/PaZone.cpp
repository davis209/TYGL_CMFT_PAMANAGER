 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/pa/src/PaZone.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * PaZone is an implementation of IPaZone. It holds the data specific to an PaZone entry
  * in the database, and allows read-only access to that data.
  */
#ifdef __WIN32__
#pragma warning(disable:4786 4290)
#endif

#include "core/types/src/ta_types.h"
#include "core/data_access_interface/pa/src/PaZone.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_Core
{

    PaZone::PaZone(const ta_uint32 key)
     : m_paZoneHelper ( new PaZoneHelper(key))
    {}


    PaZone::PaZone(ta_uint32 row, TA_Base_Core::IData& data)
     : m_paZoneHelper( new PaZoneHelper(row, data))
    {
    }


    PaZone::~PaZone() 
	{
        delete m_paZoneHelper;
        m_paZoneHelper=NULL;
    }

    ta_uint32 PaZone::getKey()
    {
        TA_ASSERT(m_paZoneHelper != NULL,"The PaZoneHelper pointer is null.");
        return m_paZoneHelper->getKey();
    }

    ta_uint32 PaZone::getLocationKey()
    {
        TA_ASSERT(m_paZoneHelper != NULL,"The PaZoneHelper pointer is null.");
        return m_paZoneHelper->getLocationKey();
    }

    ta_uint32 PaZone::getId()
    {
        TA_ASSERT(m_paZoneHelper != NULL,"The PaZoneHelper pointer is null.");
        return m_paZoneHelper->getId();
    }

    std::string PaZone::getLabel()
    {
        TA_ASSERT(m_paZoneHelper != NULL,"The PaZoneHelper pointer is null.");
        return m_paZoneHelper->getLabel();
    }    
    
    std::string PaZone::getStnEntityName()
    {
        TA_ASSERT(m_paZoneHelper != NULL,"The PaZoneHelper pointer is null.");
        return m_paZoneHelper->getStnEntityName();
    }    
    
    std::string PaZone::getOccEntityName()
    {
        TA_ASSERT(m_paZoneHelper != NULL,"The PaZoneHelper pointer is null.");
        return m_paZoneHelper->getOccEntityName();
    }    

/*
    ta_uint32 PaZone::getStatusEntityKey()
    {
        TA_ASSERT(m_paZoneHelper != NULL,"The PaZoneHelper pointer is null.");
        return m_paZoneHelper->getStatusEntityKey();
    }
*/

    std::list<std::string> PaZone::getAssociatedEquipmentEntities()
    {
        TA_ASSERT(m_paZoneHelper != NULL,"The PaZoneHelper pointer is null.");
        return m_paZoneHelper->getAssociatedEquipmentEntities();
    }

    void PaZone::invalidate()
    {
        TA_ASSERT(m_paZoneHelper != NULL,"The PaZoneHelper pointer is null.");
        m_paZoneHelper->invalidate();
    }

    std::string PaZone::getName()
    {
        TA_ASSERT(m_paZoneHelper != NULL,"The m_paZoneHelper pointer is null.");

        std::ostringstream name;
		name << "PA Zone " << m_paZoneHelper->getKey();

        return name.str();
    }
} // closes TA_IRS_Core
