 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/pa/src/PaDvaMessageVersion.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * PaDvaMessageVersion is an implementation of IPaDvaMessageVersion. It holds the data specific to an PaDvaMessageVersion entry
  * in the database, and allows read-only access to that data.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786 4290)
#endif

#include "core/types/src/ta_types.h"
#include "core/data_access_interface/pa/src/PaDvaMessageVersion.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_Core
{

    PaDvaMessageVersion::PaDvaMessageVersion(const ta_uint32 key)
     : m_paDvaMessageVersionHelper ( new PaDvaMessageVersionHelper(key))
    {}


    PaDvaMessageVersion::PaDvaMessageVersion(ta_uint32 row, TA_Base_Core::IData& data)
     : m_paDvaMessageVersionHelper(new PaDvaMessageVersionHelper(row, data))
    {
    }


    PaDvaMessageVersion::~PaDvaMessageVersion() 
	{
        delete m_paDvaMessageVersionHelper;
        m_paDvaMessageVersionHelper=NULL;
    }


    ta_uint32 PaDvaMessageVersion::getKey()
    {
        TA_ASSERT(m_paDvaMessageVersionHelper != NULL,"The PaDvaMessageVersionHelper pointer is null.");
        return m_paDvaMessageVersionHelper->getKey();
    }


    ta_uint32 PaDvaMessageVersion::getLocationKey()
    {
        TA_ASSERT(m_paDvaMessageVersionHelper != NULL,"The PaDvaMessageVersionHelper pointer is null.");
        return m_paDvaMessageVersionHelper->getLocationKey();
    }    
    

    std::string PaDvaMessageVersion::getMessageVersion(
                                            IPaDvaMessageVersion::ELocation location,
                                            ta_uint32 stationDvaMessageId)
    {
        TA_ASSERT(m_paDvaMessageVersionHelper != NULL,"The PaDvaMessageVersionHelper pointer is null.");
        return m_paDvaMessageVersionHelper->getMessageVersion(location, stationDvaMessageId);
    }


    std::string PaDvaMessageVersion::getVersionData(EVersionColumn column)
    {
        TA_ASSERT(m_paDvaMessageVersionHelper != NULL,"The PaDvaMessageVersionHelper pointer is null.");
        return m_paDvaMessageVersionHelper->getVersionData(column);
    }


    void PaDvaMessageVersion::invalidate()
    {
        TA_ASSERT(m_paDvaMessageVersionHelper != NULL,"The PaDvaMessageVersionHelper pointer is null.");
        m_paDvaMessageVersionHelper->invalidate();
    }


    std::string PaDvaMessageVersion::getName()
    {
        TA_ASSERT(m_paDvaMessageVersionHelper != NULL,"The m_paDvaMessageVersionHelper pointer is null.");

        std::ostringstream name;
		name << "PA DVA Message Version " << m_paDvaMessageVersionHelper->getKey();

        return name.str();
    }

} // closes TA_IRS_Core
