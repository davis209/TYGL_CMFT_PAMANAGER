 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/pa/src/PaDvaMessage.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * PaDvaMessage is an implementation of IPaDvaMessage. It holds the data specific to an PaDvaMessage entry
  * in the database, and allows read-only access to that data.
  */
#ifdef __WIN32__
#pragma warning(disable:4786 4290)
#endif

#include "core/types/src/ta_types.h"
#include "core/data_access_interface/pa/src/PaDvaMessage.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_Core
{

    PaDvaMessage::PaDvaMessage(const ta_uint32 key)
     : m_paDvaMessageHelper ( new PaDvaMessageHelper(key))
    {}


    PaDvaMessage::PaDvaMessage(ta_uint32 row, TA_Base_Core::IData& data)
     : m_paDvaMessageHelper(new PaDvaMessageHelper(row, data))
    {
    }


    PaDvaMessage::~PaDvaMessage() 
	{
        delete m_paDvaMessageHelper;
        m_paDvaMessageHelper=NULL;
    }

    ta_uint32 PaDvaMessage::getKey()
    {
        TA_ASSERT(m_paDvaMessageHelper != NULL,"The PaDvaMessageHelper pointer is null.");
        return m_paDvaMessageHelper->getKey();
    }

    ta_uint32 PaDvaMessage::getLocationKey()
    {
        TA_ASSERT(m_paDvaMessageHelper != NULL,"The PaDvaMessageHelper pointer is null.");
        return m_paDvaMessageHelper->getLocationKey();
    }

    ta_uint32 PaDvaMessage::getId()
    {
        TA_ASSERT(m_paDvaMessageHelper != NULL,"The PaDvaMessageHelper pointer is null.");
        return m_paDvaMessageHelper->getId();
    }

    std::string PaDvaMessage::getLabel()
    {
        TA_ASSERT(m_paDvaMessageHelper != NULL,"The PaDvaMessageHelper pointer is null.");
        return m_paDvaMessageHelper->getLabel();
    }    
    
    std::string PaDvaMessage::getType()
    {
        TA_ASSERT(m_paDvaMessageHelper != NULL,"The PaDvaMessageHelper pointer is null.");
        return m_paDvaMessageHelper->getType();
    }    

    
    ta_uint32 PaDvaMessage::getTisMessageTag()
    {
        TA_ASSERT(m_paDvaMessageHelper != NULL,"The PaDvaMessageHelper pointer is null.");
        return m_paDvaMessageHelper->getTisMessageTag();
    }


    ta_uint32 PaDvaMessage::getTisLibrarySection()
    {
        TA_ASSERT(m_paDvaMessageHelper != NULL,"The PaDvaMessageHelper pointer is null.");
        return m_paDvaMessageHelper->getTisLibrarySection();
    }


    ta_uint32 PaDvaMessage::getTisLibraryVersion()
    {
        TA_ASSERT(m_paDvaMessageHelper != NULL,"The PaDvaMessageHelper pointer is null.");
        return m_paDvaMessageHelper->getTisLibraryVersion();
    }

    void PaDvaMessage::invalidate()
    {
        TA_ASSERT(m_paDvaMessageHelper != NULL,"The PaDvaMessageHelper pointer is null.");
        m_paDvaMessageHelper->invalidate();
    }

    std::string PaDvaMessage::getName()
    {
        TA_ASSERT(m_paDvaMessageHelper != NULL,"The m_paDvaMessageHelper pointer is null.");

        std::ostringstream name;
		name << "PA DVA Message " << m_paDvaMessageHelper->getKey();

        return name.str();
    }


} // closes TA_IRS_Core
