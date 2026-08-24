 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/pa/src/PaStation.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * PaStation is an implementation of IPaStation. It holds the data specific to an PaStation entry
  * in the database, and allows read-only access to that data.
  */
#ifdef __WIN32__
#pragma warning(disable:4786 4290)
#endif

#include "core/types/src/ta_types.h"
#include "core/data_access_interface/pa/src/PaStation.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_Core
{

    PaStation::PaStation(const ta_uint32 key)
     : m_paStationHelper ( new PaStationHelper(key))
    {}


    PaStation::PaStation(ta_uint32 row, TA_Base_Core::IData& data)
     : m_paStationHelper(new PaStationHelper(row, data))
    {
    }


    PaStation::~PaStation() 
	{
        delete m_paStationHelper;
        m_paStationHelper=NULL;
    }

    ta_uint32 PaStation::getKey()
    {
        TA_ASSERT(m_paStationHelper != NULL,"The PaStationHelper pointer is null.");
        return m_paStationHelper->getKey();
    }  
    
    ta_uint32 PaStation::getLocationKey()
    {
        TA_ASSERT(m_paStationHelper != NULL,"The PaStationHelper pointer is null.");
        return m_paStationHelper->getLocationKey();
    }


    void PaStation::invalidate()
    {
        TA_ASSERT(m_paStationHelper != NULL,"The PaStationHelper pointer is null.");
        m_paStationHelper->invalidate();
    }

    std::string PaStation::getName()
    {
        TA_ASSERT(m_paStationHelper != NULL,"The m_paStationHelper pointer is null.");

        std::ostringstream name;
		name << "PA Station " << m_paStationHelper->getKey();

        return name.str();
    }


} // closes TA_IRS_Core
