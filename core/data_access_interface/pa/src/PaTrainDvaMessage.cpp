 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/pa/src/PaTrainDvaMessage.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * PaTrainDvaMessage is an implementation of IPaTrainDvaMessage. It holds the data specific to an PaTrainDvaMessage entry
  * in the database, and allows read-only access to that data.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786 4290)
#endif

#include "core/types/src/ta_types.h"
#include "core/data_access_interface/pa/src/PaTrainDvaMessage.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_Core
{

    PaTrainDvaMessage::PaTrainDvaMessage(const ta_uint32 key)
     : m_paTrainDvaMessageHelper ( new PaTrainDvaMessageHelper(key))
    {}


    PaTrainDvaMessage::PaTrainDvaMessage(ta_uint32 row, TA_Base_Core::IData& data)
     : m_paTrainDvaMessageHelper(new PaTrainDvaMessageHelper(row, data))
    {
    }


    PaTrainDvaMessage::~PaTrainDvaMessage() 
	{
        delete m_paTrainDvaMessageHelper;
        m_paTrainDvaMessageHelper=NULL;
    }


    ta_uint32 PaTrainDvaMessage::getKey()
    {
        TA_ASSERT(m_paTrainDvaMessageHelper != NULL,"The PaTrainDvaMessageHelper pointer is null.");
        return m_paTrainDvaMessageHelper->getKey();
    }


    std::string PaTrainDvaMessage::getLabel()
    {
        TA_ASSERT(m_paTrainDvaMessageHelper != NULL,"The PaTrainDvaMessageHelper pointer is null.");
        return m_paTrainDvaMessageHelper->getLabel();
    }    
    

    void PaTrainDvaMessage::invalidate()
    {
        TA_ASSERT(m_paTrainDvaMessageHelper != NULL,"The PaTrainDvaMessageHelper pointer is null.");
        m_paTrainDvaMessageHelper->invalidate();
    }


    std::string PaTrainDvaMessage::getName()
    {
        TA_ASSERT(m_paTrainDvaMessageHelper != NULL,"The m_paTrainDvaMessageHelper pointer is null.");

        std::ostringstream name;
		name << "PA Train DVA Message " << m_paTrainDvaMessageHelper->getKey();

        return name.str();
    }

} // closes TA_IRS_Core
