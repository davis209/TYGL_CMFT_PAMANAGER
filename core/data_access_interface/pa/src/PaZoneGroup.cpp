 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/pa/src/PaZoneGroup.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * PaZoneGroup is an implementation of IPaZoneGroup. It holds the data specific to an PaZoneGroup entry
  * in the database, and allows read-only access to that data.
  */

#ifdef __WIN32__
#pragma warning(disable:4786 4290)
#endif

#include "core/types/src/ta_types.h"
#include "core/data_access_interface/pa/src/PaZoneGroup.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_Core
{

    PaZoneGroup::PaZoneGroup(const ta_uint32 key)
     : m_paZoneGroupHelper ( new PaZoneGroupHelper(key))
    {}


    PaZoneGroup::PaZoneGroup(ta_uint32 row, TA_Base_Core::IData& data)
     : m_paZoneGroupHelper(new PaZoneGroupHelper(row, data))
    {
    }


    PaZoneGroup::~PaZoneGroup() 
	{
        delete m_paZoneGroupHelper;
        m_paZoneGroupHelper=NULL;
    }

    ta_uint32 PaZoneGroup::getKey()
    {
        TA_ASSERT(m_paZoneGroupHelper != NULL,"The PaZoneGroupHelper pointer is null.");
        return m_paZoneGroupHelper->getKey();
    }

    ta_uint32 PaZoneGroup::getLocationKey()
    {
        TA_ASSERT(m_paZoneGroupHelper != NULL,"The PaZoneGroupHelper pointer is null.");
        return m_paZoneGroupHelper->getLocationKey();
    }

    std::string PaZoneGroup::getLabel()
    {
        TA_ASSERT(m_paZoneGroupHelper != NULL,"The PaZoneGroupHelper pointer is null.");
        return m_paZoneGroupHelper->getLabel();
    }    
    
    ta_uint32 PaZoneGroup::getIsEventGroup()
    {
        TA_ASSERT(m_paZoneGroupHelper != NULL,"The PaZoneGroupHelper pointer is null.");
        return m_paZoneGroupHelper->getIsEventGroup();
    }

    void PaZoneGroup::invalidate()
    {
        TA_ASSERT(m_paZoneGroupHelper != NULL,"The PaZoneGroupHelper pointer is null.");
        m_paZoneGroupHelper->invalidate();
    }

    std::string PaZoneGroup::getName()
    {
        TA_ASSERT(m_paZoneGroupHelper != NULL,"The m_paZoneGroupHelper pointer is null.");

        std::ostringstream name;
		name << "PA Zone Group " << m_paZoneGroupHelper->getKey();

        return name.str();
    }

    std::vector<IPaZone*> PaZoneGroup::getAssociatedPaZones()
	{
        TA_ASSERT(m_paZoneGroupHelper != NULL,"The m_paZoneGroupHelper pointer is null.");
		return m_paZoneGroupHelper->getAssociatedPaZones();
	}


} // closes TA_IRS_Core
